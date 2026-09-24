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
// MESHLoader.cpp: Implementierung der Klasse MESHLoader.
//
//////////////////////////////////////////////////////////////////////

#include "MeshLoader.h"

#include "Mesh.h"
#include "VertexBuffer.h"
#include "Shader.h"

#include <MemoryTracker.h>
#include <iostream>
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

using namespace std;

MESHLoader::MESHLoader()
{
    file = NULL;
}

MESHLoader::~MESHLoader()
{
    if (file) {
        file->close();
        KILLOBJECT(file);
    }
}

CMesh* MESHLoader::loadMESH(std::string filename)
{

    std::cout << std::endl << "MESHLoader::loadMESH()" << std::endl;
    file = new CFile;

    if (!file->open(filename.c_str())) {
        return NULL;
    }

    int num_shaders = file->readDWORD();
    cout << "num_shader: " << num_shaders << endl;

    shaders.resize(num_shaders);

    char buffer[256];
    for (int s = 0; s < num_shaders; ++s) {
        file->readSTRING(buffer);
        std::cout << buffer << std::endl;
        shaders[s] = CShader::findOrLoadShader(buffer);
        if (shaders[s] == NULL) {
            std::cout << "MESHLoader::loadMESH() missing shader: " << buffer << std::endl;
            shaders[s] = CShader::DefaultShader;
        }
    }

    CMesh* mesh = new CMesh;
    mesh->setName("mesh");

    int num_vbuffers = file->readDWORD();
    cout << "num_vbuffers: " << num_vbuffers << endl;
    for (int v = 0; v < num_vbuffers; ++v) {
        CVertexBuffer* vbuffer;
        vbuffer = readVertexBuffer();
        mesh->addVertexBuffer(vbuffer);
    }
    mesh->calcBoundingBox();

    KILLOBJECT(file);

    return mesh;
}

CVertexBuffer* MESHLoader::readVertexBuffer()
{
    int arrays       = file->readDWORD();
    int num_vertices = file->readDWORD();
    int num_indices  = file->readDWORD();

    //	cout<<"face: num_vertices: "<<num_vertices<<" num_indices: "<<num_indices<<endl;

    CVertexBuffer* vbuffer = new CVertexBuffer;
    vbuffer->AllocArrays(arrays, num_vertices, num_indices, 1);
    // FIXME I should somehow automate this, it already brought too many unforseen consequences
    // when I forgot it
    vbuffer->num_vertices = num_vertices;
    vbuffer->num_indices  = num_indices;

    vbuffer->mode = file->readDWORD();
    int shadernum = file->readDWORD();

    vbuffer->setShader(shaders[shadernum]);

    for (int v = 0; v < num_vertices; ++v) {
        vbuffer->vertices[0][v] = readVector3();
    }
    if (arrays & NORMALARRAY) {
        for (int v = 0; v < num_vertices; ++v) {
            vbuffer->normals[0][v] = readVector3();
        }
    }
    if (arrays & TEXCOORDARRAY) {
        for (int v = 0; v < num_vertices; ++v) {
            vbuffer->texcoords[v] = readVector2();
        }
    }
    if (arrays & LMCOORDARRAY) {
        for (int v = 0; v < num_vertices; ++v) {
            vbuffer->lmcoords[v] = readVector2();
        }
    }
    if (arrays & COLORARRAY) {
        for (int v = 0; v < num_vertices; ++v) {
            vbuffer->colors[v] = readColor();
        }
    }
    if (arrays & INDEXARRAY) {
        for (int i = 0; i < num_indices; ++i) {
            vbuffer->indices[i] = (INDEX)file->readDWORD();
        }
    }

    return vbuffer;
}

VECTOR3 MESHLoader::readVector3()
{
    VECTOR3 vec;
    vec.x = file->readFLOAT();
    vec.y = file->readFLOAT();
    vec.z = file->readFLOAT();
    return vec;
}

VECTOR2 MESHLoader::readVector2()
{
    VECTOR2 vec;
    vec.x = file->readFLOAT();
    vec.y = file->readFLOAT();
    return vec;
}

COLOR MESHLoader::readColor()
{
    COLOR col;
    (DWORD&)col = file->readDWORD();
    return col;
}
