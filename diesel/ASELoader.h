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

#include <vector>

#include <file/File.h>
#include <file/Path.h>
#include <misc/Lexer.h>

#include "Entity.h"
#include "Shader.h"

class ASELoader
{
public:
    ASELoader(void);
    ~ASELoader(void);

    bool LoadASE(const CPath& Filename, CEntity::ENTITYLIST& EntityList);

protected:
    void     readMaterialList();
    CShader* readMaterial();
    CEntity* readGeomObject();
    CMesh*   readMesh();

    void readVertices(VECTOR3* verts, int num_vertices);
    void readFaces(INDEX* indices, int num_indices);
    void readTVerts(VECTOR2* tverts, int num_tverts);
    void readTFaces(INDEX* indices, int num_tfaces);

    CVertexBuffer* buildVBuffer(VECTOR3* verts, VECTOR2* tverts, INDEX* faces, INDEX* tfaces, int num_faces,
                                int num_verts);

    Lexer m_lexer;

    std::vector<CShader::SMARTPTR> materials;
};
