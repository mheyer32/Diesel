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

#include "doomloader.h"

#include <file/FileManager.h>
#include <misc/Lexer.h>
#include <misc/Exception.h>
#include <misc/ParseHelper.h>

#include "TextureManager.h"
#include "Mesh.h"
#include "Shader.h"
#include "VBServices.h"
#include "VertexBuffer.h"

using namespace std;

DoomLoader::DoomLoader(void)
{
}

DoomLoader::~DoomLoader(void)
{
}

DoomScene* DoomLoader::LoadPROC(const CPath& filename)
{

    CFile* file = NULL;
    if (!(file = CFileManager::Instance()->open(filename))) {
        cout << "DoomLoader::LoadPROC() could not open file " << filename << endl;
        return NULL;
    }

    char* filecontents = new char[file->getSize() + 1];
    file->readVOID(filecontents, file->getSize());
    filecontents[file->getSize()] = 0;
    delete file;

    DoomScene* doomscene = new DoomScene;
    DoomMap*   doommap   = new DoomMap;

    lexer.setText(filecontents);
    lexer.setCaseSensitive(false);

    try {
        while (true) {
            lexer.nextToken();

            if (lexer.matchAndSkip("model")) {
                CMesh* mesh = readModel();
                models.push_back(mesh);
            } else if (lexer.matchAndSkip("interareaportals")) {
                readInterAreaPortals(doommap);
            } else if (lexer.matchAndSkip("nodes")) {
                readNodes(doommap);
                break;
            }
        }
    } catch (LexException& e) {
        cout << e.getCompleteText() << endl;
    }

    delete[] filecontents;

    doomscene->map = doommap;
    char modelname[32];

    for (int a = 0; a < doommap->num_areas; ++a) {
        sprintf(modelname, "_area%d", a);
        CMesh* areamodel = CMesh::FindMesh(modelname);
        if (areamodel) {
            doommap->areas[a].areamodel = areamodel;
        } else {
            cout << "WARNING: could not find mesh: " << modelname << endl;
        }
    }

    for (int p = 0; p < doommap->num_portals; ++p) {
        DoomMap::PORTAL& portal = doommap->portals[p];
        doommap->areas[portal.areas[0]].portals.push_back(p);
        doommap->areas[portal.areas[1]].portals.push_back(p);
        portal.plane = PLANE(portal.vertices[0], portal.vertices[1], portal.vertices[2]);
    }

    LoadMAP(filename, doommap);

    return doomscene;
}

void DoomLoader::LoadMAP(const CPath& filename, DoomMap* doommap)
{
    CPath tempfilename(filename);
    tempfilename.setExtension("map");
    CFile* file = NULL;
    if (!(file = CFileManager::Instance()->open(tempfilename))) {
        cout << "DoomLoader::LoadPROC() could not open file" << endl;
        return;
    }

    char* filecontents = new char[file->getSize() + 1];
    file->readVOID(filecontents, file->getSize());
    filecontents[file->getSize()] = 0;
    delete file;

    lexer.setText(filecontents);
    lexer.setCaseSensitive(false);

    try  // as long as entity definitions follow
    {
        while (true) {

            lexer.skipUntil('{');
            D3Entity* ent         = new D3Entity;
            int       endofentity = lexer.getEndOfNextBlock();

            int eofposition = lexer.getMaxPosition();

            lexer.setMaxPosition(endofentity);

            try {

                while (true) {
                    // found start of key-value-pair
                    char key[1024];
                    char value[1024];
                    lexer.readQuotedString(key);
                    lexer.readQuotedString(value);
                    ent->addKeyValue(key, value);
                    lexer.nextToken();

                    // std::cout<<key<<' '<<value<<endl;
                }
            } catch (LexException&) {
                // hit the end of the entity
            }
            ent->parseKeyValues();
            doommap->linkEntity(ent);

            lexer.setMaxPosition(eofposition);
            lexer.setPosition(endofentity);

            // cout<<endl<<endl;
        }
    } catch (LexException&) {
        // probably end of file;
    }
}

void DoomLoader::readNodes(DoomMap* dmap)
{
    cout << "reading BSP Nodes... " << flush;

    lexer.skipBeyondNext('{');

    int numNodes = lexer.readInteger();

    cout << "numNodes: " << numNodes;

    dmap->nodes     = new DoomMap::NODE[numNodes];
    dmap->num_nodes = numNodes;

    DoomMap::NODE* nodes = dmap->nodes;

    for (int n = 0; n < numNodes; ++n) {
        PLANE plane = nodes[n].plane;
        readVec((float*)&plane, 4);
        convertVec(plane.n);
        plane.d        = -plane.d;
        nodes[n].plane = plane;

        nodes[n].children[0] = lexer.readInteger();
        nodes[n].children[1] = lexer.readInteger();

        // cout<<"Plane: "<<plane.n.x<<" "<<plane.n.y<<" "<<plane.n.z<<" "<<plane.d<<endl;
        // cout<<nodes[n].children[0]<<" "<<nodes[n].children[1]<<endl;
    }

    lexer.skipBeyondNext('}');
    cout << "successful." << endl;
}

void DoomLoader::readInterAreaPortals(DoomMap* dmap)
{
    cout << "reading InterAreaPortals... " << flush;

    lexer.skipBeyondNext('{');

    int numAreas   = lexer.readInteger();
    int numPortals = lexer.readInteger();

    dmap->num_areas   = numAreas;
    dmap->num_portals = numPortals;

    dmap->portals = new DoomMap::PORTAL[numPortals];
    dmap->areas   = new DoomMap::AREA[numAreas];

    for (int p = 0; p < numPortals; ++p) {
        dmap->portals[p].num_vertices = lexer.readInteger();
        dmap->portals[p].areas[0]     = lexer.readInteger();
        dmap->portals[p].areas[1]     = lexer.readInteger();

        dmap->portals[p].vertices = new VECTOR3[dmap->portals[p].num_vertices];
        VECTOR3* verts            = dmap->portals[p].vertices;
        for (int v = 0; v < dmap->portals[p].num_vertices; ++v) {
            readVec((float*)&verts[v], 3);
            convertVec(verts[v]);
        }
    }
    lexer.skipBeyondNext('}');
    cout << "successful." << endl;
}

void DoomLoader::readVec(float* coordinates, int dim)
{
    lexer.skipBeyondNext('(');
    for (int d = 0; d < dim; ++d) {
        coordinates[d] = lexer.readDouble();
    }
    lexer.skipBeyondNext(')');
}

CMesh* DoomLoader::readModel()
{
    lexer.skipBeyondNext('{');

    char modelname[1024];
    lexer.readQuotedString(modelname);

    CMesh* mesh = new CMesh();

    mesh->setName(modelname);
    cout << "load Model: " << mesh->getName() << endl;

    int num_surfaces = lexer.readInteger();

    // cout<<"num_Surfaces:"<<num_surfaces<<endl;

    for (int s = 0; s < num_surfaces; ++s) {
        CVertexBuffer* vbuffer = readSurface();
        mesh->addVertexBuffer(vbuffer);
    }

    lexer.skipBeyondNext('}');

    mesh->calcBoundingBox();

    return mesh;
}

CVertexBuffer* DoomLoader::readSurface()
{
    lexer.skipBeyondNext('{');

    CVertexBuffer* vbuffer = new CVertexBuffer;

    char materialname[1024];
    lexer.readQuotedString(materialname);

    strcat(materialname, "_d.tga");

    CShader* shader = CShader::findOrLoadShader(materialname, false);

    // Material einlesen
    if (shader) {
        vbuffer->setShader(shader);
    } else {

        vbuffer->setShader(CShader::DefaultShader);
    }

    int numVerts   = lexer.readInteger();
    int numIndices = lexer.readInteger();

    vbuffer->AllocArrays(VERTEXARRAY | INDEXARRAY | TEXCOORDARRAY, numVerts, numIndices);
    vbuffer->num_vertices = numVerts;
    vbuffer->num_indices  = numIndices;

    // cout<<"surface: "<<numVerts<<" "<<numIndices<<endl;

    for (int v = 0; v < numVerts; ++v) {
        lexer.skipBeyondNext('(');
        vbuffer->vertices[0][v].x = lexer.readDouble();
        vbuffer->vertices[0][v].z = -lexer.readDouble();
        vbuffer->vertices[0][v].y = lexer.readDouble();

        vbuffer->texcoords[v].x = lexer.readDouble();
        vbuffer->texcoords[v].y = lexer.readDouble();

        // readDouble();
        // readDouble();
        // readDouble();

        lexer.skipBeyondNext(')');
    }

    for (int i = 0; i < numIndices; ++i) {
        vbuffer->indices[i] = lexer.readInteger();
    }

    VBServices::reverseIndices(*vbuffer);

    lexer.skipBeyondNext('}');

    return vbuffer;
}

void DoomLoader::convertVec(VECTOR3& vec)
{
    float t = vec.y;
    vec.y   = vec.z;
    vec.z   = -t;
}
