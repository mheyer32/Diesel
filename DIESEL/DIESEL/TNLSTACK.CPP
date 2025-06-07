/*
This file is part of Diesel
(c) 2002 by Mathias Heyer
email: sonode@gmx.de

Diesel is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Diesel is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "TNLStack.h"

#include <cassert>

#include <misc/QSort.h>
#include <misc/Timer.h>

#include "Engine.h"
#include "Texture.h"
#include "renderer.h"
#include "vertexbuffer.h"

#include <memorytracker.h>

CVertexBuffer* TNLStack::bvbuffer     = NULL;
int            TNLStack::activelights = 0;
ConVar         TNLStack::r_maxpolys("r_maxpolys", "65536");
ConVar         TNLStack::r_maxpolyverts("r_maxpolyverts", "4096");
ConVar TNLStack::r_sortThreshold("r_sortThreshold", "50");  // threshold when quicksort will switch to insertion sort

TNLStack::TNLStack()
{
    clearStack();
    if (!bvbuffer) {
        // all TNLStacks are allowed to share their temporarily batch-buffer
        // should make no problems, because only one TNLSatck can execute at one time
        bvbuffer = new CVertexBuffer();
        bvbuffer->AllocArrays(MT_DYNAMIC, ALL_ARRAYS, MAX_RENDERER_VERTICES, MAX_RENDERER_INDICES);
    }
    bvbuffer->AddRef();

    num_infos   = 0;
    num_solids  = 0;
    num_blended = 0;

    enable_override = false;

    renderer = Renderer::Instance();
}

TNLStack::~TNLStack()
{
    // FIXME: if the last tnlstack was destroyed, bvbuffer will be released but the pointer won´t be set to NULL
    if (bvbuffer)
        bvbuffer->Release();
}

void TNLStack::executeBatch(VBREF* batch, int num_elements)
{
    VBREF*      curr;
    RENDERINFO* currinfo;
    RENDERINFO  info;

    info.vbuffer = bvbuffer;

    int       curr_tref = -1;
    int       curr_lref = -1;
    const int maxverts  = (int)r_maxpolyverts;
    const int maxpolys  = (int)r_maxpolys * 3;
    int       nextv;
    int       new_tref;
    int       new_lref;

    Timer5.resume();
    int v1 = 0;
    while (v1 < num_elements) {

        curr            = &batch[v1];
        currinfo        = &infos[curr->infidx];
        CShader* shader = currinfo->shader;

        new_tref = (curr->tnlref) & 0x0FFF;
        if (curr_tref != new_tref) {
            curr_tref = new_tref;
            setTState(tstates[curr_tref]);
        }
        new_lref = (curr->tnlref >> 16);
        // only set lightstate if shader really needs lighting
        if ((shader->getFlags() & SHADER_NEEDSLIGHTING) && curr_lref != new_lref) {
            curr_lref = new_lref;
            setLState(lstates[curr_lref]);
        }
        nextv = v1 + 1;

        // maxverts and maxpolys were included here to try different settings; in the end it brought not very much
        // choosing r_maxpolys or r_maxpolyverts= 0 would result in no batching
        if ((!(currinfo->sortkey & SORT_DONTBATCH)) &&  // sortkey must allow combining
            (nextv < num_elements) &&                   // not the last vbuffer
            (batch[nextv] == *curr)                     // TNL State of both is thew same
#ifdef _DEBUG
            && ((currinfo->vbuffer->num_vertices + infos[batch[nextv].infidx].vbuffer->num_vertices) <
                maxverts) &&  // both vbuffers cam be combined
            ((currinfo->vbuffer->num_indices + infos[batch[nextv].infidx].vbuffer->num_indices) < maxpolys)
#endif
        )

        {
            // at least two buffers can be batched

            bvbuffer->num_vertices = bvbuffer->num_indices = 0;
            bvbuffer->used_arrays                          = shader->getUsedArrays();
            bvbuffer->setLightmap(currinfo->vbuffer->getLightmap());

            do {
                if (bvbuffer->combine(*infos[batch[v1].infidx].vbuffer) != CVertexBuffer::VBCOMBINE_OK)
                    break;
                v1++;
            } while ((v1 < num_elements) && (batch[v1] == *curr)
#ifdef _DEBUG
                     && (bvbuffer->num_vertices < maxverts) && (bvbuffer->num_indices < maxpolys)
#endif
            );

            info.shader = currinfo->shader;

            info.num_features = currinfo->num_features;
            for (int f = 0; f < info.num_features; ++f)
                info.features[f] = currinfo->features[f];

            // sortkey is not of interest for the renderer
            renderer->Render(info);
        } else {
            // buffer cannot be batched, leave geometry where it is, render from there
            renderer->Render(*currinfo);
            v1++;
        }
    }

    Timer5.pause();
}

void TNLStack::execute()
{
    if (!num_infos) {
        clearStack();
        return;
    }

    ::glPushMatrix();

    CCamera* savedcam = g_Camera;  // save global g_Camera
    g_Camera          = &camera;   // use the stack´s own camera

    g_Camera->setupFrustum();  // clipplanes had to be set before TNLStack::setCamera() was called

    if (num_solids) {
        Timer6.resume();
        quicksort<256>(solidbatch, num_solids, (int)r_sortThreshold);
        Timer6.pause();
        executeBatch(solidbatch, num_solids);
    }

    if (num_blended) {
        Timer6.resume();
        quicksort<256>(blendedbatch, num_blended, 10000);
        Timer6.pause();
        executeBatch(blendedbatch, num_blended);
    }

    ::glPopMatrix();

    // maybe I should take this out
    // with some modifications it should be possible to alter the override-info and in this way draw
    // the complete scene with different overrides

    num_framevbuffers = num_infos;
    num_solids        = 0;  // clear batch
    num_blended       = 0;
    num_infos         = 0;

    clearStack();

    g_Camera = savedcam;
};

void TNLStack::setLState(const LSTATE& lstate)
{
    int lasttstate = -1;
    ::glPushMatrix();
    LIGHT* light;
    int    l = 0;
    for (l = 0; l < lstate.num_lights; ++l) {
        if (lstate.lights[l].lightnum != curr_lights[l]) {
            curr_lights[l] = lstate.lights[l].lightnum;
            if (lstate.lights[l].tstate_idx != lasttstate) {
                lasttstate = lstate.lights[l].tstate_idx;
                setTState(tstates[lasttstate]);
            }
            light = &scenelights[lstate.lights[l].lightnum];
            if (light->used_attribs & LIGHT_POSITION)
                glLightfv(GL_LIGHT0 + l, GL_POSITION, (float*)&light->pos);
            if (light->used_attribs & LIGHT_DIRECTION)
                glLightfv(GL_LIGHT0 + l, GL_SPOT_DIRECTION, (float*)&light->dir);
            if (light->used_attribs & LIGHT_DIFFUSE)
                glLightfv(GL_LIGHT0 + l, GL_DIFFUSE, (float*)&light->diffuse);
            if (light->used_attribs & LIGHT_AMBIENT)
                glLightfv(GL_LIGHT0 + l, GL_AMBIENT, (float*)&light->ambient);
            if (light->used_attribs & LIGHT_SPECULAR)
                glLightfv(GL_LIGHT0 + l, GL_SPECULAR, (float*)&light->specular);

            if (light->used_attribs & LIGHT_CUTOFF)
                glLightf(GL_LIGHT0 + l, GL_SPOT_CUTOFF, light->cutoff);
            if (light->used_attribs & LIGHT_EXPONENT)
                glLightf(GL_LIGHT0 + l, GL_SPOT_EXPONENT, light->exponent);
            if (light->used_attribs & LIGHT_CONSTANT_ATTENUATION)
                glLightf(GL_LIGHT0 + l, GL_CONSTANT_ATTENUATION, light->attenuation[0]);
            if (light->used_attribs & LIGHT_LINEAR_ATTENUATION)
                glLightf(GL_LIGHT0 + l, GL_LINEAR_ATTENUATION, light->attenuation[1]);
            if (light->used_attribs & LIGHT_QUADRATIC_ATTENUATION)
                glLightf(GL_LIGHT0 + l, GL_QUADRATIC_ATTENUATION, light->attenuation[2]);
        }
        if (l >= activelights)
            glEnable(GL_LIGHT0 + l);
    }
    ::glPopMatrix();
    for (; l < activelights; ++l) {
        glDisable(GL_LIGHT0 + l);
    }
    activelights = lstate.num_lights;
}

void TNLStack::setCamera(const CCamera& ncamera)
{
    camera = ncamera;
}

void TNLStack::setOverrideRenderInfo(const RENDERINFO& info)
{
    // TODO: introduce a stack of iverride-infos
    overrideinfo    = info;
    enable_override = true;
}
void TNLStack::clearOverrideRenderInfo()
{
    enable_override = false;
}