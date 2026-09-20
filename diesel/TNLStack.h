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
// TNLStack.h: Schnittstelle für die Klasse TNLStack.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TNLSTACK_H__D6879FC0_732E_11D5_825A_00E07D966723__INCLUDED_)
#define AFX_TNLSTACK_H__D6879FC0_732E_11D5_825A_00E07D966723__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER > 1000

#include <cassert>

#include <math/Matrix4.h>
#include <opengl/OpenGL.h>

#include <iostream>
#include "EngineTypes.h"
#include "Camera.h"
#include "Light.h"
#include "Texture.h"
#include "VertexBuffer.h"

#define MAX_NUM_LIGHTS 8
#define MAX_T_STATES 512
#define MAX_L_STATES 512
#define MAX_TNL_STACK_DEPTH 256

#define MAX_BATCHED_VBUFFERS 32768
#define MAX_NUM_BLENDED_FACES (MAX_BATCHED_VBUFFERS)
#define MAX_NUM_SCENELIGHTS 512

#define UNLOCKED 0
#define LOCKED 1
#define PUSHED 2

class Renderer;

class TNLStack
{
public:
    TNLStack();
    ~TNLStack();

    inline void PushMatrix()
    {
        tstates[curr_tindex].locked |= PUSHED;
        tstack[tstackdepth++] = curr_tindex;
    }
    inline void PopMatrix()
    {
        curr_tindex = tstack[--tstackdepth];
        tstates[curr_tindex].locked &= ~PUSHED;
    }

    inline void Translate(const VECTOR3& t)
    {
        invalidateTState();
        Translate4(tstates[curr_tindex].transform, t);
    }
    inline void Rotate(const VECTOR3& axis, float angle)
    {
        invalidateTState();
        MATRIX4 M;
        AngleAxisMatrix4(M, axis, angle);
        tstates[curr_tindex].transform = tstates[curr_tindex].transform * M;
    }
    inline void MultMatrix(const MATRIX4& m)
    {
        invalidateTState();
        tstates[curr_tindex].transform = tstates[curr_tindex].transform * m;
    }
    inline void LoadMatrix(const MATRIX4& m)
    {
        invalidateTState();
        tstates[curr_tindex].transform = m;
    }
    inline void LoadIdentity()
    {
        invalidateTState();
        tstates[curr_tindex].transform = IdentityMatrix4;
    }

    inline void getMatrix(MATRIX4& M) const { M = tstates[curr_tindex].transform; }

    void clearStack()
    {
        num_scenelights       = 0;
        tstackdepth           = 0;
        lstackdepth           = 0;
        max_tindex            = 1;
        curr_tindex           = 0;
        max_lindex            = 1;
        curr_lindex           = 0;
        tstates[0].locked     = UNLOCKED;
        lstates[0].locked     = UNLOCKED;
        lstates[0].num_lights = 0;
        for (int i = 0; i < MAX_NUM_LIGHTS; ++i)
            curr_lights[i] = -1;
    }

    void clearLights()
    {
        invalidateLState();
        lstates[curr_lindex].num_lights = 0;
    }

    void addLight(const LIGHT& light)
    {
        if (lstates[curr_lindex].num_lights == MAX_NUM_LIGHTS - 1) {
            std::cout << "TNLStack::addLight()  ^4MAX_NUM_LIGHTS reached!!" << std::endl;
            return;
        }
        invalidateLState();
        lockTState();
        scenelights[num_scenelights]                                              = light;
        lstates[curr_lindex].lights[lstates[curr_lindex].num_lights].lightnum     = num_scenelights++;
        lstates[curr_lindex].lights[lstates[curr_lindex].num_lights++].tstate_idx = curr_tindex;
    }
    void pushLights()
    {
        lstates[curr_lindex].locked |= PUSHED;
        lstack[lstackdepth++] = curr_lindex;
    }

    void popLights()
    {
        curr_lindex = lstack[--lstackdepth];
        lstates[curr_lindex].locked &= ~PUSHED;
    }

    inline void batch(const RENDERINFO& newinfo)
    {
        if (num_infos < MAX_BATCHED_VBUFFERS) {
            lockLState();
            lockTState();

            RENDERINFO& info = infos[num_infos];
            info             = newinfo;
            if (enable_override)
                override(info);

            VBREF& batch  = info.sortkey < SORT_ADDITIVE ? solidbatch[num_solids++] : blendedbatch[num_blended++];
            batch.infidx  = num_infos++;
            batch.tnlref  = (curr_lindex << 16) | (curr_tindex);
            batch.sortkey = info.sortkey;

            assert(info.shader);

            return;
        } else {
            std::cout << "TNLStack::::Batch() reached MAX_BATCHED_VBUFFERS" << std::endl;
        }
    }

    inline void batch(const CVertexBuffer* vb)
    {
        if (num_infos < MAX_BATCHED_VBUFFERS) {
            lockLState();
            lockTState();

            RENDERINFO& info = infos[num_infos];

            vb->fillRenderInfo(info);
            if (enable_override)
                override(info);

            VBREF& batch  = info.sortkey < SORT_ADDITIVE ? solidbatch[num_solids++] : blendedbatch[num_blended++];
            batch.infidx  = num_infos++;
            batch.tnlref  = (curr_lindex << 16) | (curr_tindex);
            batch.sortkey = info.sortkey;

            assert(info.shader);

            return;
        } else {
            std::cout << "TNLStack::::Batch() reached MAX_BATCHED_VBUFFERS" << std::endl;
        }
    }

    void setOverrideRenderInfo(const RENDERINFO& info);
    void clearOverrideRenderInfo();
    void execute();

    inline int getNumBatchedVBuffers() const { return num_infos; };

    void setCamera(const CCamera& ncamera);

    CCamera& getCamera()  // I really don´t know if this is the right way...
    {
        return camera;
    }

protected:
    struct LSTATE
    {
        int locked;
        int num_lights;
        struct
        {
            int lightnum, tstate_idx;
        } lights[MAX_NUM_LIGHTS];
    };

    struct TSTATE
    {
        int     locked;
        MATRIX4 transform;
    };

    union VBREF
    {
        struct
        {
            unsigned int tnlref;  //  lref|tref
            unsigned int sortkey;
            unsigned int infidx;  // index of shaderinfo that points to the data to be rendered
        };

    private:
        unsigned __int64 k;

    public:
        inline bool operator<(const VBREF& vbr2) const { return k < vbr2.k; };
        inline bool operator==(const VBREF& vbr2) const { return k == vbr2.k; };

        inline void operator=(const VBREF& vbr2)
        {
            tnlref  = vbr2.tnlref;
            sortkey = vbr2.sortkey;
            infidx  = vbr2.infidx;
        };
    };

    inline void invalidateTState()
    {
        if (tstates[curr_tindex].locked != UNLOCKED) {
            tstates[max_tindex]         = tstates[curr_tindex];
            curr_tindex                 = max_tindex++;
            tstates[curr_tindex].locked = UNLOCKED;
        }
    }
    inline void invalidateLState()
    {
        if (lstates[curr_lindex].locked != UNLOCKED) {
            lstates[max_lindex]         = lstates[curr_lindex];
            curr_lindex                 = max_lindex++;
            lstates[curr_lindex].locked = UNLOCKED;
        }
    }
    inline void lockLState() { lstates[curr_lindex].locked |= LOCKED; }

    inline void lockTState() { tstates[curr_tindex].locked |= LOCKED; }

    void setLState(const LSTATE& lstate);

    inline void setTState(const TSTATE& tstate) const { ::glLoadMatrixf((float*)&tstate.transform); }

    inline void override(RENDERINFO& info)
    {
        if (overrideinfo.shader) {
            info.shader = overrideinfo.shader;  // allows halos, shield-effects and so on
            // adjust sortkey for new shader (shader-sort and shader id!):
            info.sortkey &= ~0xFFFE0000;
            info.sortkey |= overrideinfo.sortkey & 0xFFFE0000;
        }

        if (overrideinfo.num_features)  // allows objects in fog
        {
            for (int f = 0; f < overrideinfo.num_features; ++f)
                info.features[f] = overrideinfo.features[f];
            info.num_features = overrideinfo.num_features;
        }
        info.sortkey |= overrideinfo.sortkey & 0x7F;  // allows to avoid unwanted batching of fogged geometry
    }

    void executeBatch(VBREF* batch, int num_elements);

    TSTATE tstates[MAX_T_STATES];
    int    max_tindex;
    int    curr_tindex;

    int   num_scenelights;
    LIGHT scenelights[MAX_NUM_SCENELIGHTS];

    int curr_lights[MAX_NUM_LIGHTS];

    LSTATE lstates[MAX_L_STATES];
    int    max_lindex;
    int    curr_lindex;

    int lstack[MAX_TNL_STACK_DEPTH];
    int tstack[MAX_TNL_STACK_DEPTH];
    int lstackdepth;
    int tstackdepth;

    RENDERINFO infos[MAX_BATCHED_VBUFFERS];
    int        num_infos;

    VBREF solidbatch[MAX_BATCHED_VBUFFERS];
    VBREF blendedbatch[MAX_NUM_BLENDED_FACES];
    int   num_solids;
    int   num_blended;

    RENDERINFO overrideinfo;
    bool       enable_override;

    Renderer* renderer;
    CCamera   camera;  // this´ cameras settings will be restored when executing the stack

    static int            activelights;
    static CVertexBuffer* bvbuffer;  // this will be the rendering vertexbuffer, that collects multiple vb´s of same
                                     // shader,lightmap, transform...

    static ConVar r_maxpolys;
    static ConVar r_maxpolyverts;
    static ConVar r_sortThreshold;
};

#endif  // !defined(AFX_TNLSTACK_H__D6879FC0_732E_11D5_825A_00E07D966723__INCLUDED_)
