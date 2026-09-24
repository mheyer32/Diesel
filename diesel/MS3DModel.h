/*
This file is part of Diesel
(c) 2002-2026 by Mathias Heyer
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
#pragma once

#include <cassert>
#include <vector>

#include <adt/NamedObject.h>
#include <adt/RefCount.h>
#include <adt/SmartPointer.h>
#include <math/Matrix34.h>

#include "Quaternion.h"

#include "EngineTypes.h"  // just for INDEX type
#include "Mesh.h"
#include "Shader.h"

#include "MS3DAnimation.h"

class CVertexBuffer;

class MS3DModel : public CRefCount, public CNamedObject
{

    friend class MS3DLoader;

public:
    struct JOINT
    {
        int      parent;
        MATRIX34 local;  // relative to bone-parent, used for reference posture and as base for animation
        MATRIX34 final;
        BBOX     bbox;
        char     name[64];
    };
    struct FRAMEINFO
    {
        MS3DAnimation* anim;
        float          time;
    };

    typedef std::vector<FRAMEINFO>  FRAMEINFOS;
    typedef SmartPointer<MS3DModel> SMARTPTR;

    MS3DModel(void);

    CMesh* createMesh();

    void evaluateRuntimePosture(float time, CMesh* mesh, MS3DAnimation* anim);
    void evaluateRuntimePosture(CMesh* mesh, const FRAMEINFOS& frames);
    void evaluateReferencePosture(CMesh* mesh);

    inline JOINT& getJoint(int i) const
    {
        assert(i < m_num_joints);
        return m_joints[i];
    }
    int getJointIndex(const char* name);

    static void getMatrix(MATRIX34& mat, const VECTOR3& position, const VECTOR3& angles);
    static void getRotation(MATRIX34& mat, const VECTOR3& angles);

protected:
    ~MS3DModel(void);

    void verticesToBonespace();
    void fillVertexData(CMesh* mesh);
    void accumulateBoneMatrices();

    struct VERTEX
    {
        VECTOR3 pos;
        VECTOR3 norm;
        VECTOR2 tex;
        int     boneindex;
    };

    struct MESH
    {
        int     num_vertices;
        VERTEX* vertices;
        int     num_indices;
        INDEX*  indices;
        int     materialindex;

        MESH()
        : vertices(0)
        , indices(0) {};
        ~MESH()
        {
            delete[] vertices;
            delete[] indices;
        }
    };

    int   m_num_meshes;
    MESH* m_meshes;

    int       m_num_joints;
    JOINT*    m_joints;
    MATRIX34* m_jointmatrices;

    std::vector<CShader::SMARTPTR> m_materials;
};
