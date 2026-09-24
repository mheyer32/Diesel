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
#include "ASELoader.h"

#include <crtdbg.h>
#include <iostream>

#include <file/FileManager.h>
#include <misc/Exception.h>

#include "Mesh.h"

using namespace std;

ASELoader::ASELoader(void)
{
}

ASELoader::~ASELoader(void)
{
}

bool ASELoader::LoadASE(const CPath& Filename, CEntity::ENTITYLIST& EntityList)
{
    CFile* file = NULL;
    if (!(file = CFileManager::Instance()->open(Filename))) {
        cout << "ASELoader::LoadASE() could not open file '" << Filename << '\'' << endl;
        return false;
    }

    char* filecontents = new char[file->getSize() + 1];
    file->readVOID(filecontents, file->getSize());
    filecontents[file->getSize()] = 0;

    m_lexer.setText(filecontents);
    m_lexer.setCaseSensitive(false);

    CPath directory;
    CFileManager::Instance()->getCurrentDirectory(directory);
    CFileManager::Instance()->setCurrentDirectory(CPath(file->getFilename()).getPath());

    delete file;

    try {
        m_lexer.skipBeyondNext("*material_list");
        readMaterialList();
        m_lexer.skipBeyondNext("*geomobject");
        CEntity* entity = readGeomObject();
        if (entity) {
            entity->getMesh()->setName(Filename);  // hack
            EntityList.push_back(entity);
        }
    } catch (LexException&) {
    };

    CFileManager::Instance()->setCurrentDirectory(directory);

    return true;
}

void ASELoader::readMaterialList()
{
    m_lexer.skipBeyondNext('{');

    m_lexer.skipBeyondNext('*');

    if (!m_lexer.matchAndSkip("material_count")) {
        cout << "ASELoader::readMaterialList() expected material_count" << endl;
        return;
    }

    int num_materials = m_lexer.readInteger();

    materials.resize(num_materials);

    for (int m = 0; m < num_materials; ++m) {
        CShader* shader = readMaterial();
        if (shader)
            materials[m] = shader;
        else
            materials[m] = CShader::DefaultShader;
    }

    m_lexer.skipBeyondNext('}');
}

CShader* ASELoader::readMaterial()
{
    CShader* shader = NULL;

    m_lexer.enterBlock();

    try {
        m_lexer.skipUntil("*map_diffuse");
        m_lexer.enterBlock();

        try {
            m_lexer.skipUntil("*bitmap");

            char bitmapfile[256];
            m_lexer.readQuotedString(bitmapfile);

            CPath       texturefile(bitmapfile);
            std::string texturename = texturefile.getFilename();

            shader = CShader::findOrLoadShader(texturename);
            if (!shader) {
                shader = CShader::DefaultShader;
            }
        } catch (LexException&) {
        }

        m_lexer.exitBlock();
    } catch (LexException&) {
    }

    m_lexer.exitBlock();

    return shader;
}

CEntity* ASELoader::readGeomObject()
{
    m_lexer.enterBlock();

    CEntity* entity = new CEntity();
    CMesh*   mesh   = 0;
    try {
        while (true) {
            m_lexer.skipBeyondNext('*');
            if (m_lexer.matchAndSkip("node_name")) {
                char name[256];
                m_lexer.readQuotedString(name);
                entity->setName(name);
            } else if (m_lexer.matchAndSkip("mesh")) {
                mesh = readMesh();
                entity->addMesh(mesh);
            } else if (m_lexer.matchAndSkip("material_ref")) {
                int mtlid = m_lexer.readInteger();
                if (mesh && !mesh->getVBufferList().empty()) {
                    mesh->getVBufferList().front()->setShader(materials[mtlid]);
                }
            } else {
                m_lexer.skipNextBlock();
            }
        }
    } catch (LexException&) {
    }
    m_lexer.exitBlock();

    return entity;
}

CMesh* ASELoader::readMesh()
{
    m_lexer.enterBlock();

    CMesh* mesh = new CMesh();

    int num_vertices = 0;
    int num_faces    = 0;

    CVertexBuffer* vbuffer = 0;

    try {
        m_lexer.skipBeyondNext("*mesh_numvertex");
        num_vertices = m_lexer.readInteger();
        m_lexer.skipBeyondNext("*mesh_numfaces");
        num_faces = m_lexer.readInteger();

        m_lexer.skipBeyondNext("*mesh_vertex_list");
        VECTOR3* vertices = new VECTOR3[num_vertices];
        readVertices(vertices, num_vertices);

        m_lexer.skipBeyondNext("*mesh_face_list");
        INDEX* indices = new INDEX[num_faces * 3];
        readFaces(indices, num_faces);

        m_lexer.skipBeyondNext("*mesh_numtvertex");
        int      num_tvertices = m_lexer.readInteger();
        VECTOR2* tverts        = new VECTOR2[num_tvertices];
        m_lexer.skipBeyondNext("*mesh_tvertlist");
        readTVerts(tverts, num_tvertices);

        m_lexer.skipBeyondNext("*mesh_numtvfaces");
        int    num_tfaces = m_lexer.readInteger();
        INDEX* tfaces     = new INDEX[num_tfaces * 3];
        m_lexer.skipBeyondNext("*mesh_tfacelist");
        readTFaces(tfaces, num_tfaces);

        vbuffer = buildVBuffer(vertices, tverts, indices, tfaces, num_faces, num_vertices);

        delete[] vertices;
        delete[] indices;
        delete[] tverts;
        delete[] tfaces;

    } catch (LexException&) {
    };

    m_lexer.exitBlock();

    if (!vbuffer) {
        mesh->AddRef();
        mesh->Release();
        return NULL;
    }

    vbuffer->setShader(CShader::DefaultShader);
    mesh->addVertexBuffer(vbuffer);
    mesh->calcBoundingBox();
    return mesh;
}

CVertexBuffer* ASELoader::buildVBuffer(VECTOR3* verts, VECTOR2* tverts, INDEX* faces, INDEX* tfaces, int num_faces,
                                       int num_verts)
{
    assert(verts);
    assert(tverts);
    assert(faces);
    assert(tfaces);
    assert(num_faces);

    std::map<unsigned int, int>           indexmap;  // so we will have a minimum of doubled vertices
    std::map<unsigned int, int>::iterator iIt;

    int num_indices    = num_faces * 3;
    int num_finalverts = 0;

    // now make the texture coordinates unique (this will solve most of the bad lighting problems and facetted look)
    bool* remap = new bool[num_indices];
    for (int v = 0; v < num_indices; ++v)
        remap[v] = false;
    for (int t = 0; t < num_indices; ++t) {
        if (remap[t])
            continue;  // index is already remapped
        remap[t]          = true;
        const VECTOR2& v1 = tverts[tfaces[t]];
        for (int t2 = t + 1; t2 < num_indices; t2++) {
            if (remap[t2])
                continue;
            if (v1 == tverts[tfaces[t2]]) {
                tfaces[t2] = tfaces[t];
                remap[t2]  = true;
            }
        }
    }
    delete[] remap;

    // first find unique pairs of position and texturecoordinate
    for (int i = 0; i < num_indices; ++i) {
        unsigned int key = (faces[i] << 16) | tfaces[i];
        indexmap[key]    = 0;
    }

    num_finalverts = indexmap.size();
    // now we know how many vertices we´ll need
    assert(_CrtCheckMemory());
    CVertexBuffer* vbuffer =
        new CVertexBuffer(MT_STATIC, VERTEXARRAY | INDEXARRAY | TEXCOORDARRAY, num_finalverts, num_faces * 3);
    vbuffer->num_vertices = num_finalverts;
    vbuffer->num_indices  = num_faces * 3;

    // run through the unique pairs and put them into the vbuffer
    int fidx = 0;
    for (iIt = indexmap.begin(); iIt != indexmap.end(); ++iIt) {
        unsigned int vertidx       = iIt->first >> 16;
        unsigned int tvertidx      = iIt->first & 0xFFFF;
        iIt->second                = fidx;
        vbuffer->vertices[0][fidx] = verts[vertidx];
        vbuffer->texcoords[fidx]   = tverts[tvertidx];
        ++fidx;
    }

    // remap the original indices to our new vbuffer content
    for (int j = 0; j < num_indices; ++j) {
        unsigned int key = (faces[j] << 16) | tfaces[j];
        iIt              = indexmap.find(key);
        assert(iIt != indexmap.end());
        vbuffer->indices[j] = iIt->second;
    }

    return vbuffer;
}

void ASELoader::readVertices(VECTOR3* verts, int num_vertices)
{
    m_lexer.enterBlock();

    for (int v = 0; v < num_vertices; ++v) {
        m_lexer.skipUntil('*');
        m_lexer.nextToken();  // skip mesh_vertex
        m_lexer.nextToken();  // skip vertex number

        verts[v].x = (float)m_lexer.readDouble();
        verts[v].z = (float)-m_lexer.readDouble();
        verts[v].y = (float)m_lexer.readDouble();
    }

    m_lexer.exitBlock();
}

void ASELoader::readFaces(INDEX* indices, int num_faces)
{
    m_lexer.enterBlock();

    for (int f = 0; f < num_faces; ++f) {
        m_lexer.skipBeyondNext("*mesh_face");
        m_lexer.skipBeyondNext("a:");

        indices[f * 3] = m_lexer.readInteger();

        m_lexer.skipBeyondNext("b:");

        indices[f * 3 + 1] = m_lexer.readInteger();

        m_lexer.skipBeyondNext("c:");

        indices[f * 3 + 2] = m_lexer.readInteger();

        /*		m_lexer.skipBeyondNext("*mesh_mtlid");

        int mtlid=m_lexer.readInteger();*/
    }

    m_lexer.exitBlock();
}
void ASELoader::readTVerts(VECTOR2* tverts, int num_tverts)
{
    m_lexer.enterBlock();

    for (int v = 0; v < num_tverts; ++v) {
        m_lexer.skipUntil('*');
        m_lexer.nextToken();  // skip mesh_tvertex
        m_lexer.nextToken();  // skip vertex number

        tverts[v].x = (float)m_lexer.readDouble();
        tverts[v].y = 1.0f - (float)m_lexer.readDouble();
        // a third texcoord is following, but seems not to be used
    }
    m_lexer.exitBlock();
}
void ASELoader::readTFaces(INDEX* indices, int num_tfaces)
{
    m_lexer.enterBlock();

    for (int f = 0; f < num_tfaces; ++f) {
        m_lexer.skipUntil('*');
        m_lexer.nextToken();  // skip mesh_tvertex
        m_lexer.nextToken();  // skip face number

        indices[f * 3]     = m_lexer.readInteger();
        indices[f * 3 + 1] = m_lexer.readInteger();
        indices[f * 3 + 2] = m_lexer.readInteger();
    }
    m_lexer.exitBlock();
}
