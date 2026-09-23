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

#include "Q3Scene.h"

#include <cassert>

#include <misc/Timer.h>
#include <win/AppWindow.h>

#include "Engine.h"  //for g_TNLStack and g_Scene
#include "GLText.h"
#include "MD3Loader.h"
#include "Sound.h"
#include "TNLStack.h"

#include <MemoryTracker.h>

extern ConVar cg_fov;

ConVar Q3Scene::r_portalOnly("r_portalOnly", "0");
ConVar Q3Scene::cg_drawGun("cg_drawGun", "1");
ConVar Q3Scene::raytest("raytest", "0");
ConVar Q3Scene::weapon("weapon", "", 0, Q3Scene::weapon_CB);

Q3Scene::Q3Scene()
{
    for (int p = 0; p < MAX_NUM_PORTALS; ++p) {
        g_PortalStacks[p]        = new TNLStack;
        g_Portals[p].portalstack = g_PortalStacks[p];
    }
    g_num_Portals = 0;
}

Q3Scene::~Q3Scene()
{
    clearMapSounds();

    for (int p = 0; p < MAX_NUM_PORTALS; ++p) {
        KILLOBJECT(g_PortalStacks[p]);
    }

    sceneentities.clear();  // intentionally clear list before deleting worldmodel

    KILLOBJECT(worldmodel);
}

void Q3Scene::cullScene()
{

    // cull new scene for next frame
    assert(worldmodel);

    Timer4.start();

    setupCamera();

    // DLIGHT test

    /*	worldmodel->DLight.pos=g_Camera->getTransformation().getTranslation()-(g_Camera->getTransformation().getColumnVector(2)*100.0f);
        worldmodel->DLight.pos.w=1.0f;
        worldmodel->DLight.range=400;
        worldmodel->DLight.attenuation[0]=1.0f;
        //worldmodel->DLight.attenuation[1]=1.0f/(worldmodel->DLight.range);

        worldmodel->DLight.attenuation[2]=10.0f/(worldmodel->DLight.range*worldmodel->DLight.range);
        worldmodel->DLight.diffuse=VECTOR3(0.0,0.0,1.0);
        worldmodel->DLight.specular=VECTOR3(0,0,0);
        worldmodel->DLight.ambient=VECTOR3(0,0,0);
        worldmodel->DLight.used_attribs=POINT_LIGHT;
        g_TNLStack->addLight(worldmodel->DLight);
    */

    renderGun();

    worldmodel->Render();

    cullPortals();

    Timer4.pause();
}
void Q3Scene::renderScene()
{

    renderPortals();

    // now flush the normal stack
    if (!(int)r_portalOnly) {
        g_TNLStack->execute();
    } else {
        g_TNLStack->clearStack();
    }

    if (worldmodel && (int)raytest) {
        if (worldmodel->PointOnMeshSide(g_Camera->getTransformation().getTranslation()) == FRONTSIDE) {
            g_Text->PrintAt(" inside map", 0, 20);
        } else {
            g_Text->PrintAt("outside map", 0, 20);
        }

        VECTOR3 p1, p2;

        p1 =
            g_Camera->getTransformation().getTranslation() - (g_Camera->getTransformation().getColumnVector(1) * 10.0f);
        p2 = p1 - (g_Camera->getTransformation().getColumnVector(2) * 200.0f);

        Q3BSPMesh::TRACE trace;
        trace.contents = CONTENTS_SOLID | CONTENTS_DETAIL;
        trace.bevel    = 0.0f;
        if (!worldmodel->RayTest(p1, p2, trace)) {
            CShader* shader = worldmodel->getSurfaceInfo(worldmodel->getBrushside(trace.brushside).surfaceinfo).shader;
            char     text[512];
            sprintf(text, "ray hits wall at fraction of %1.4f\n", trace.fraction);
            if (shader)
                strcat(text, shader->getName().c_str());
            g_Text->PrintAt(text, 0, 21);
        } else {
            g_Text->PrintAt("ray does not hit wall", 0, 21);
        }

        VECTOR3 start(0, -10, 0);
        VECTOR3 end(0, -10, -200);

        glPushMatrix();
        glLoadIdentity();
        glPushAttrib(GL_ENABLE_BIT);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);
        glColor3ub(255, 255, 255);
        glBegin(GL_LINES);
        glVertex3fv((float*)&start);
        glVertex3fv((float*)&end);
        glEnd();
        glPopAttrib();
        glPopMatrix();
    }
}

void Q3Scene::addEntity(Q3Entity* newEntity)
{
    assert(newEntity);

    newEntity->setWorldModel(worldmodel);

    sceneentities.push_back(newEntity);
}

// FIXME: is this the right way?
void Q3Scene::addEntity(CEntity* newEntity)
{
    Q3Entity* ent = new Q3Entity;
    ent->assignEntity(newEntity);
    addEntity(ent);
}

void Q3Scene::addEntities(CEntity::ENTITYLIST& list)
{
    CEntity::ENTITYITERATOR eIt = list.begin();

    for (; eIt != list.end(); ++eIt) {
        addEntity(*eIt);
    }
}

void Q3Scene::setWeaponModel(CEntity* weapon)
{
    weaponmodel = weapon;
}

bool Q3Scene::initialize()
{
    return true;
}

void Q3Scene::renderGun()
{
    if (weaponmodel && (int)cg_drawGun) {
        // draw the weapon
        LIGHT light;
        worldmodel->getLightVol(g_Camera->getTransformation().getTranslation(), light);

        g_TNLStack->PushMatrix();
        g_TNLStack->pushLights();
        ;
        g_TNLStack->addLight(light);
        g_TNLStack->LoadIdentity();

        weaponmodel->cullEntity(0);  // dont need to test against frustum

        g_TNLStack->popLights();
        g_TNLStack->PopMatrix();
    }
}

void Q3Scene::setupCamera()
{
    int cont = worldmodel->PointContents(g_Camera->getTransformation().getTranslation(), 0, CONTENTS_WATER);
    if (cont != -1) {
        // if g_Camera in Water, fiddle around with FOV and aspect of the view
        float t = g_Time, t2 = g_Time;
        t *= 0.2f;
        t2 *= 0.137f;
        t  = t - floorf(t);
        t2 = t2 - floorf(t2);
        t2 = cosf(t2 * 2.0f * PI);
        t  = sinf(t * 2.0f * PI);

        float newFOV = (float)cg_fov + 1.0f * t2;
        float newAspect =
            (float)CAppWindow::Instance()->getWidth() / (float)CAppWindow::Instance()->getHeight() - 0.05f + t * 0.05f;

        g_Camera->setupFrustum(newFOV, newAspect, g_Camera->NearZ, g_Camera->FarZ);
    }
}

void Q3Scene::cullPortals()
{
    // if any portals where encountered during traversion, these portals must be culled now
    // pointers to those portal-entities are contained in Q3Entity::seen_portals
    // after culling these portals g_num_Portals contains the number of seen portals and g_Portals[] the pointers to the
    // filled portal-tnl-stacks that can be rendered next frame

    if (!Q3Entity::seen_portals.size())
        return;

    // save current tnlstack and camera pointer
    CCamera  portalcam;
    CCamera* savedcam = g_Camera;
    g_Camera          = &portalcam;

    TNLStack* savedstack = g_TNLStack;

    g_num_Portals                  = 0;
    Q3Entity::Q3ENTITYITERATOR pIt = Q3Entity::seen_portals.begin();
    for (; pIt != Q3Entity::seen_portals.end(); ++pIt) {
        // FIXME: rethink handling of TNLStack::camera
        g_TNLStack = g_Portals[g_num_Portals].portalstack;
        g_TNLStack->clearStack();
        *g_Camera = *savedcam;  // take over all settings from original camera

        (*pIt)->RenderPortal(g_Portals[g_num_Portals]);
        g_num_Portals++;
    }

    // restore old tnlstack and camera
    g_Camera   = savedcam;
    g_TNLStack = savedstack;

    Q3Entity::seen_portals.clear();
}

void Q3Scene::renderPortals()
{
    // if there were portals encountered in the last frame, draw them now before the
    // "normal" screen
    if (g_num_Portals) {
        glEnable(GL_CLIP_PLANE0);
        glEnable(GL_SCISSOR_TEST);

        float viewport[4];
        glGetFloatv(GL_VIEWPORT, viewport);
        RECTANGLE cliprect;
        cliprect.min = VECTOR2(viewport[0], viewport[1]);
        cliprect.max = VECTOR2(viewport[2], viewport[3]);
        cliprect.max += cliprect.min;

        glPushMatrix();

        GLdouble plane[4];
        MATRIX4  M;
        int      glstate_cullfront = 0;
        for (int p = g_num_Portals - 1; p >= 0; --p) {

            RECTANGLE rect = g_Portals[p].portalrect;
            rect.min.x     = viewport[0] + viewport[2] * (rect.min.x + 1.0f) * 0.5f;
            rect.min.y     = viewport[1] + viewport[3] * (rect.min.y + 1.0f) * 0.5f;
            rect.max.x     = viewport[0] + viewport[2] * (rect.max.x + 1.0f) * 0.5f;
            rect.max.y     = viewport[1] + viewport[3] * (rect.max.y + 1.0f) * 0.5f;
            clipRectangle(rect, cliprect);
            VECTOR2 dim = rect.max - rect.min;
            glScissor((int)rect.min.x, (int)rect.min.y, (int)dim.x, (int)dim.y);

            M = g_Portals[p].portalstack->getCamera().getTransformation().getInverseMatrix();

            glLoadMatrixf((float*)&M);

            plane[0] = g_Portals[p].portalplane.n.x;
            plane[1] = g_Portals[p].portalplane.n.y;
            plane[2] = g_Portals[p].portalplane.n.z;
            plane[3] = -g_Portals[p].portalplane.d;

            glClipPlane(GL_CLIP_PLANE0, plane);
            if (g_Portals[p].cullfront)
                glFrontFace(GL_CW);
            else
                glFrontFace(GL_CCW);

            g_Portals[p].portalstack->execute();

            glDepthMask(GL_TRUE);
            glClear(GL_DEPTH_BUFFER_BIT);
        }
        glPopMatrix();
        glFrontFace(GL_CCW);
        glDisable(GL_CLIP_PLANE0);
        glDisable(GL_SCISSOR_TEST);
        g_num_Portals = 0;
    }
}

void Q3Scene::weapon_CB(ConVar& var)
{
    Q3Scene* q3scene = dynamic_cast<Q3Scene*>(g_Scene);
    if (q3scene) {
        CEntity* weapon = CEntity::findEntity(var.getString());
        if (!weapon) {
            MD3Loader loader;
            weapon = loader.loadSimpleLODEntity(var.getString());
            if (!weapon) {
                std::cout << "Q3Scene::weapon_CB()->loader.loadSimpleLODEntity() could not load\n ^3" << var.getString()
                          << std::endl;
            } else {
                Transformation& t = weapon->getTransformation();
                t.setIdentity();
                t.setAngles(VECTOR3(0, -PI * 0.5f, 0));
                t.setTranslation(VECTOR3(3, -10, 15));
            }
        }
        q3scene->setWeaponModel(weapon);
    }
}
