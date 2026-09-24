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
#include ".\ms3dloader.h"

#include <defs.h>
#include <file/File.h>
#include <file/FileManager.h>

#include "TextureManager.h"

MS3DLoader::MS3DLoader(void)
{
}

MS3DLoader::~MS3DLoader(void)
{
}

MS3DAnimation* MS3DLoader::loadMS3DAnimation_ASCII(const CPath& Filename)
{
    CFile* file = NULL;

    if (!(file = CFileManager::Instance()->open(Filename))) {
        cout << "MS3DLoader::loadMS3DASCII could not open file '" << Filename << '\'' << endl;
        return 0;
    }

    int   filesize     = file->getSize();
    char* filecontents = new char[filesize];
    file->readVOID(filecontents, filesize);
    delete file;

    lex.setText(filecontents, filesize);
    delete[] filecontents;

    MS3DAnimation* anim = new MS3DAnimation;

    lex.skipBeyondNext("Frames:");
    anim->m_num_frames = lex.readInteger();

    readKeyframes(anim);

    return anim;
}

MS3DModel* MS3DLoader::loadMS3DModel_ASCII(const CPath& Filename)
{
    CFile* file = NULL;

    if (!(file = CFileManager::Instance()->open(Filename))) {
        cout << "MS3DLoader::loadMS3DASCII could not open file '" << Filename << '\'' << endl;
        return 0;
    }

    int   filesize     = file->getSize();
    char* filecontents = new char[filesize];
    file->readVOID(filecontents, filesize);
    delete file;

    lex.setText(filecontents, filesize);
    delete[] filecontents;

    MS3DModel* msmodel = new MS3DModel;

    readMeshes(msmodel);
    readMaterials(msmodel);
    readJoints(msmodel);

    msmodel->verticesToBonespace();
    msmodel->setName(Filename);

    return msmodel;
}

void MS3DLoader::readMeshes(MS3DModel* msmodel)
{
    lex.skipBeyondNext("Meshes:");

    int num_meshes = lex.readInteger();

    msmodel->m_num_meshes = num_meshes;
    msmodel->m_meshes     = new MS3DModel::MESH[num_meshes];

    for (int m = 0; m < num_meshes; ++m) {
        MS3DModel::MESH& mesh = msmodel->m_meshes[m];

        char name[256];
        lex.readQuotedString(name);
        lex.readInteger();  // read over flags;

        mesh.materialindex = lex.readInteger();

        int num_vertices  = lex.readInteger();
        mesh.num_vertices = num_vertices;
        mesh.vertices     = new MS3DModel::VERTEX[num_vertices];

        ZeroMemory(mesh.vertices, num_vertices * sizeof(MS3DModel::VERTEX));
        for (int v = 0; v < num_vertices; ++v) {
            MS3DModel::VERTEX& vertex = mesh.vertices[v];

            lex.readInteger();
            readVector3(vertex.pos);
            vertex.tex.x     = lex.readDouble();
            vertex.tex.y     = lex.readDouble();
            vertex.boneindex = lex.readInteger();
        }

        int num_normals = lex.readInteger();

        VECTOR3* normals = new VECTOR3[num_normals];
        for (int n = 0; n < num_normals; ++n) {
            readVector3(normals[n]);
        }
        // triangle: flags, vertex index1, vertex index2, vertex index3, normal index1, normal index 2, normal index 3,
        // smoothing group

        int num_tris     = lex.readInteger();
        mesh.num_indices = num_tris * 3;
        mesh.indices     = new INDEX[num_tris * 3];

        for (int t = 0; t < num_tris; ++t) {
            lex.readInteger();  // read over flags

            int i1, i2, i3;
            i1                      = lex.readInteger();
            i2                      = lex.readInteger();
            i3                      = lex.readInteger();
            mesh.indices[t * 3 + 0] = i1;
            mesh.indices[t * 3 + 1] = i2;
            mesh.indices[t * 3 + 2] = i3;
            int ni                  = lex.readInteger();
            mesh.vertices[i1].norm += normals[ni];
            ni = lex.readInteger();
            mesh.vertices[i2].norm += normals[ni];
            ni = lex.readInteger();
            mesh.vertices[i3].norm += normals[ni];

            lex.readInteger();  // read over smoothing group
        }

        delete[] normals;

        for (int v2 = 0; v2 < num_vertices; ++v2) {
            Normalize(mesh.vertices[v2].norm);
        }
    }
}

void MS3DLoader::readMaterials(MS3DModel* msmodel)
{
    lex.skipBeyondNext("Materials:");

    int num_materials = lex.readInteger();

    for (int m = 0; m < num_materials; ++m) {
        char name[256];
        lex.readQuotedString(name);

        CShader* shader = CShader::findOrLoadShader(name, false, false);
        if (shader) {
            // either found the shader in memory or found a script named that way
            msmodel->m_materials.push_back(shader);

            // skip over this material
            for (int i = 0; i < 4 * 4 + 2; ++i) {
                lex.readDouble();
            }
            lex.readQuotedString(name);
            lex.readQuotedString(name);
            continue;
        }

        CTexture* tex = new CTexture;
        tex->setName(name);

        readColor(tex->Ambient);
        readColor(tex->Diffuse);
        readColor(tex->Specular);
        readColor(tex->Emissive);
        tex->Shininess = lex.readDouble();

        lex.readQuotedString(name);
        CTextureManager::Instance()->loadTexture(tex, name);
        lex.readQuotedString(name);  // no support for extra alphamap (might be supported with shader tricks)

        tex->setTexParameter(TEX_USEMATERIAL, TEX_USEMATERIAL);  // enable the use of the material settings

        shader = CShader::genShaderFromTexture(tex);
        shader->getPass(0).setRGBGen(RGBGEN_LIGHTINGDIFFUSE);  // enable the use of the material settings

        shader->validate();

        msmodel->m_materials.push_back(shader);
    }
}

void MS3DLoader::readColor(float color[4])
{
    for (int i = 0; i < 4; ++i) {
        color[i] = lex.readDouble();
    }
}

void MS3DLoader::readJoints(MS3DModel* msmodel)
{
    lex.skipBeyondNext("Bones:");

    int num_bones = lex.readInteger();

    msmodel->m_num_joints    = num_bones;
    msmodel->m_joints        = new MS3DModel::JOINT[num_bones];
    msmodel->m_jointmatrices = new MATRIX34[num_bones];

    for (int j = 0; j < num_bones; ++j) {
        MS3DModel::JOINT& joint = msmodel->m_joints[j];
        lex.readQuotedString(joint.name);

        char parentname[33];
        lex.readQuotedString(parentname);
        if (strlen(parentname)) {
            for (int j2 = 0; j2 < j; ++j2) {
                if (EQ(msmodel->m_joints[j2].name, parentname)) {
                    joint.parent = j2;
                }
            }
        }
        // joint: flags, posx, posy, posz, rotx, roty, rotz
        lex.readInteger();  // ignore flags
        VECTOR3 refposition, refangles;
        readVector3(refposition);
        readVector3(refangles);

        MS3DModel::getMatrix(joint.local, refposition, refangles);

        // skip position keys
        int num_keys = lex.readInteger();
        lex.nextLine();
        for (int k = 0; k < num_keys; ++k) {
            lex.nextLine();
        }

        // skip rotation keys
        num_keys = lex.readInteger();
        lex.nextLine();
        for (int k2 = 0; k2 < num_keys; ++k2) {
            lex.nextLine();
        }
    }
}

void MS3DLoader::readKeyframes(MS3DAnimation* anim)
{
    lex.skipBeyondNext("Bones:");
    int num_bones = lex.readInteger();
    lex.nextLine();

    anim->m_num_joints     = num_bones;
    anim->m_jointkeyframes = new MS3DAnimation::JOINTKEYFRAMES[num_bones];

    for (int j = 0; j < num_bones; ++j) {
        lex.nextLine();
        lex.nextLine();
        lex.nextLine();

        MS3DAnimation::JOINTKEYFRAMES& joint = anim->m_jointkeyframes[j];

        joint.num_poskeys = lex.readInteger();
        joint.poskeys     = new MS3DAnimation::POSKEY[joint.num_poskeys];

        joint.pos_starttime = lex.readDouble();
        readVector3(joint.poskeys[0].poskey);

        for (int k = 1; k < joint.num_poskeys; ++k) {
            lex.readDouble();
            readVector3(joint.poskeys[k].poskey);
        }

        joint.num_rotkeys = lex.readInteger();
        joint.rotkeys     = new MS3DAnimation::ROTKEY[joint.num_rotkeys];

        VECTOR3 angles;

        joint.rot_starttime = lex.readDouble();
        readVector3(angles);
        QuaternionFromAngles(joint.rotkeys[0].rotkey, angles);

        for (int k2 = 1; k2 < joint.num_rotkeys; ++k2) {
            lex.readDouble();
            readVector3(angles);
            QuaternionFromAngles(joint.rotkeys[k2].rotkey, angles);
        }
        lex.nextLine();
    }
}

void MS3DLoader::readVector3(VECTOR3& vec)
{
    vec.x = lex.readDouble();
    vec.y = lex.readDouble();
    vec.z = lex.readDouble();
}
