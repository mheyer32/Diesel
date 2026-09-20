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
// MD3Loader.cpp: Implementierung der Klasse MD3Loader.
//
//////////////////////////////////////////////////////////////////////

#include <file/FileManager.h>
#include <math/MathLib.h>
#include <misc/ParseHelper.h>
#include <iostream>
#include "Entity.h"
#include "MD3Loader.h"
#include "Mesh.h"
#include "VBServices.h"
#include "Shader.h"
#include "VertexBuffer.h"

#include <cassert>

#include <MemoryTracker.h>
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
using namespace std;
MD3Loader::MD3Loader()
{
    file = NULL;
}

MD3Loader::~MD3Loader()
{
    if (file)
        file->close();
    KILLOBJECT(file);
}

CVertexBuffer* MD3Loader::loadMD3Mesh()
{
    char      shadername[68];  // 68 is hardcoded for MD3
    CShader** shaders = NULL;
    int       pos;

    pos = file->getPosition();
    file->readVOID(&meshheader, sizeof(meshheader));
    file->setPosition(pos + meshheader.HeaderSize);

    // bisher keine Multiskin Unterstützung, lade aber trotzdem alle Shader
    if (meshheader.Skin_num) {
        shaders = new CShader*[meshheader.Skin_num];
        for (int s = 0; s < meshheader.Skin_num; ++s) {
            file->readVOID(shadername, sizeof(shadername));
            if (strlen(shadername)) {
                shaders[s] = findOrLoadShader(shadername);
            } else {
                // wenn die Shadernamen leer sind, muss man nach der .skin suchen und die parsen
                shaders[s] = loadSkin(meshheader.Name);
            }
        }
    }

    CVertexBuffer* vbuffer = new CVertexBuffer;

    if (shaders && shaders[0]) {
        vbuffer->setShader(shaders[0]);  // wie kann CMesh multiskin unterstützen? enthält MD3 überhaupt multiskins?
    } else {
        vbuffer->setShader(CShader::DefaultShader);
    }
    KILLARRAY(shaders)

    int arrays = vbuffer->getShader()->getUsedArrays() | VERTEXARRAY | INDEXARRAY;

    // Mesh Arrays allokieren
    assert(meshheader.Vertex_num);
    assert(meshheader.Triangle_num);

    vbuffer->AllocArrays(MT_DYNAMIC, arrays & (VERTEXARRAY | NORMALARRAY | INDEXARRAY), meshheader.Vertex_num,
                         meshheader.Triangle_num * 3, meshheader.MeshFrame_num);
    vbuffer->AllocArrays(MT_STATIC, arrays & ~(VERTEXARRAY | NORMALARRAY | INDEXARRAY), meshheader.Vertex_num);

    vbuffer->num_vertices = vbuffer->getMaxNumVertices();
    vbuffer->num_indices  = vbuffer->getMaxNumIndices();

    // Dreiecks-IndexArray einlesen
    int* indices = new int[vbuffer->num_indices];
    file->setPosition(pos + meshheader.Triangle_Start);
    file->readVOID(indices, vbuffer->num_indices * sizeof(int));

    // Indizes umkehren, CW -> CCW order
    for (int s = 0; s < vbuffer->num_indices; s += 3) {
        vbuffer->indices[s + 2] = indices[s];
        vbuffer->indices[s + 1] = indices[s + 1];
        vbuffer->indices[s]     = indices[s + 2];
    }

    // Hilfsarrary löschen
    KILLARRAY(indices)

    // TexturKoordinaten einlesen
    if (arrays & TEXCOORDARRAY) {
        file->setPosition(pos + meshheader.TexVec_Start);
        file->readVOID(vbuffer->texcoords, meshheader.Vertex_num * sizeof(VECTOR2));
    }

    // Vertices einlesen
    file->setPosition(pos + meshheader.Vertex_Start);
    float lat, lng;
    for (int f = 0; f < vbuffer->getNumVertexSets(); ++f) {
        for (int v = 0; v < vbuffer->num_vertices; ++v) {
            // x-y-z ordung wiederherstellen
            vbuffer->vertices[f][v].x = ((float)((signed short)file->readWORD())) / 64.0f;
            vbuffer->vertices[f][v].z = -((float)((signed short)file->readWORD())) / 64.0f;
            vbuffer->vertices[f][v].y = ((float)((signed short)file->readWORD())) / 64.0f;

            if (arrays & NORMALARRAY) {
                lng = ((float)file->readBYTE()) * PI / 128.0f;
                lat = ((float)file->readBYTE()) * PI / 128.0f;
                getVecFromLatLong(lat, lng, vbuffer->normals[f][v]);
                Normalize(vbuffer->normals[f][v]);
            } else
                file->readWORD();
        }
    }

    // FIXME lookup all deformv
    if (vbuffer->getShader()->getVertexDeform(0) == VDEF_AUTOSPRITE) {
        VBServices::preTransformBillboard(*vbuffer);
    } else if (vbuffer->getShader()->getVertexDeform(0) == VDEF_AUTOSPRITE2) {
        VBServices::preTransformAxisBillboard(*vbuffer);
    }

    file->setPosition(pos + meshheader.MeshSize);

    return vbuffer;
}

// this may be obsolete
CEntity* MD3Loader::loadEntity(const CPath& Filename)
{

    CMesh*   mesh;
    CEntity *entity, *child;

    BOOL mesh_animated = TRUE;

    CFileManager* fman = CFileManager::Instance();

    filename = Filename;

    if (!(file = fman->open(Filename)))
        return NULL;
    file->readVOID(&header, sizeof(MD3HEADER));

    entity = new CEntity();
    if (child = CEntity::findEntity(header.FileName)) {
        // cout<<"LoadMD3(): reusing existing equal named Entity"<<endl<<"^3     "<<child->getName()<<endl;
        entity = child->clone();

        file->close();
        KILLOBJECT(file);

        return entity;
    }
    entity->setName(header.FileName);

    // file->setPosition(header.Tag_End); // alle Tags skippen und gleich zu den Meshes kommen
    file->setPosition(header.HeaderLength + header.Tag_num * sizeof(MD3TAG) +
                      header.BoneFrame_num * sizeof(MD3BONEFRAME));

    for (int m = 0; m < header.Mesh_num; m++) {

        CVertexBuffer* vbuffer = loadMD3Mesh();

        child = new CEntity();
        child->setName(meshheader.Name);
        mesh = new CMesh();
        mesh->setName(meshheader.Name);

        mesh_animated = mesh_animated && (vbuffer->getNumVertexSets() > 1);

        mesh->addVertexBuffer(vbuffer);
        mesh->optimize();
        mesh->calcBoundingBox();
        child->addMesh(mesh);
        entity->addChild(child);
    }

    if (!mesh_animated) {
        entity->collapseChildren();
        if (mesh = entity->getMesh()) {
            mesh->mergeVertexBuffers();
            mesh->optimize();
            mesh->calcBoundingBox();
        }
    }

    entity->getBoundingBox();
    file->close();
    KILLOBJECT(file);
    return entity;
}

CMesh* MD3Loader::loadMesh(const CPath& Filename)
{
    CMesh* mesh = NULL;

    if (mesh = CMesh::FindMesh(Filename)) {
        return mesh;
    }

    CFileManager* fman = CFileManager::Instance();

    if (!(file = fman->open(Filename)))
        return NULL;

    file->readVOID(&header, sizeof(MD3HEADER));
    file->setPosition(header.Tag_End);  // alle Tags skippen und gleich zu den Meshes kommen
    // file->setPosition(header.HeaderLength+header.Tag_num*sizeof(MD3TAG)+header.BoneFrame_num*sizeof(MD3BONEFRAME));

    mesh = new CMesh;
    mesh->setName(Filename);

    CVertexBuffer* vbuffer;
    for (int m = 0; m < header.Mesh_num; m++) {
        vbuffer = loadMD3Mesh();
        mesh->addVertexBuffer(vbuffer);
    }

    mesh->mergeVertexBuffers();
    mesh->optimize();
    mesh->calcBoundingBox();

    file->close();
    KILLOBJECT(file);
    return mesh;
}

CEntity* MD3Loader::loadSimpleEntity(const CPath& Filename)
{
    CEntity* entity = NULL;

    if (entity = CEntity::findEntity(Filename)) {
        CEntity* copy = new CEntity;
        // cout<<"LoadMD3(): reusing existing equal named Entity"<<endl<<"^4 "<<Filename<<endl;
        copy = entity->clone();
        return copy;
    }

    CMesh* mesh = loadMesh(Filename);
    if (mesh) {
        entity = new CEntity;
        entity->addMesh(mesh);
        entity->setName(Filename);
    }

    return entity;
}

CEntity* MD3Loader::loadSimpleLODEntity(const CPath& Filename)
{
    CEntity* entity = NULL;

    if (entity = CEntity::findEntity(Filename)) {
        CEntity* copy = new CEntity;
        // cout<<"LoadMD3(): reusing existing equal named Entity"<<endl<<"^4 "<<Filename<<endl;
        copy = entity->clone();
        return copy;
    }

    char tempname[MAX_PATH];
    filename = Filename;
    strcpy(tempname, filename.c_str());
    int len = strlen(tempname) - 4;

    CMesh* mesh = NULL;
    entity      = new CEntity;
    int lod     = 0;

    while (mesh = loadMesh(tempname)) {
        entity->addMesh(mesh);
        lod++;
        strcpy(tempname, filename.c_str());
        tempname[len] = 0;
        sprintf(tempname, "%s_%d.md3", tempname, lod);
    }

    if (entity->getMesh(0)) {
        entity->setName(Filename);
    } else {
        cout << "MD3Loader::loadSimpleLODEntity() model\n^5     " << Filename << "\nnot found" << endl;
        KILLOBJECT(entity);
    }
    return entity;
}

CShader* MD3Loader::loadSkin(char* Meshname)
{
    CShader* shader = NULL;
    char     skinfilename[MAX_PATH];
    char     meshname[MAX_PATH];

    strcpy(meshname, Meshname);
    strcpy(skinfilename, filename.c_str());

    skinfilename[strlen(skinfilename) - 4] = 0;
    strcat(skinfilename, "_default.skin");

    CFileManager* fman = CFileManager::Instance();
    CFile*        skinfile;

    if (skinfile = fman->open(skinfilename)) {
        char* contents = new char[skinfile->getSize() + 1];
        skinfile->readVOID(contents, skinfile->getSize());
        contents[skinfile->getSize()] = 0;
        ParseHelper::RemoveAllComments(contents);
        int len = strlen(meshname);
        if (meshname[len - 2] == '_')  // _x  lod-number
        {
            meshname[len - 2] = 0;
        }
        char* pos = strstr(contents, meshname);
        if (pos) {
            char shadername[MAX_PATH];
            sscanf(pos, "%*[a-z0-9._],%s", shadername);
            shader = findOrLoadShader(shadername);
        }
        KILLARRAY(contents);
        skinfile->close();
        delete skinfile;
    }
    return shader;
}

CShader* MD3Loader::findOrLoadShader(char* shadername)
{
    CShader* shader;

    char tempname[MAX_PATH];
    ZeroMemory(tempname, sizeof(tempname));
    strcpy(tempname, shadername);
    char* pos = strrchr(tempname, '.');  //.tga entfernen
    if (pos != NULL) {
        *pos = 0;
    }
    shader = CShader::findOrLoadShader(tempname, false);
    if (!shader)  // weder shader noch gleichnamige textur gefunden
    {
        shader = CShader::DefaultShader;
    } else {
        // angenommen, der shader wurde generiert FIXME: nötig ?
        if (shader->getFlags() & SHADER_TAGGED) {
            shader->getPass(0).setRGBGen(RGBGEN_LIGHTINGDIFFUSE);
            shader->validate();
        }
    }

    return shader;
}
