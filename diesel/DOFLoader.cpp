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
// DOFLoader.cpp: Implementierung der Klasse DOFLoader.
//
//////////////////////////////////////////////////////////////////////

#include <defs.h>
#include <file/FileManager.h>
#include "DOFLoader.h"
#include "Mesh.h"
#include "texturemanager.h"
#include "VBServices.h"
#include "VertexBuffer.h"

#include <MemoryTracker.h>
#include <iostream>
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

#define CHKNAME(a, b, c, d) (d << 24 | c << 16 | b << 8 | a)

#define DOF1 CHKNAME('D', 'O', 'F', '1')
#define EDOF CHKNAME('E', 'D', 'O', 'F')

#define MATS CHKNAME('M', 'A', 'T', 'S')
#define GEOB CHKNAME('G', 'E', 'O', 'B')

#define MAT0 CHKNAME('M', 'A', 'T', '0')
#define GOB1 CHKNAME('G', 'O', 'B', '1')

#define MHDR CHKNAME('M', 'H', 'D', 'R')
#define MCOL CHKNAME('M', 'C', 'O', 'L')
#define MUVW CHKNAME('M', 'U', 'V', 'W')
#define MTRA CHKNAME('M', 'T', 'R', 'A')
#define MTEX CHKNAME('M', 'T', 'E', 'X')
#define MSUB CHKNAME('M', 'S', 'U', 'B')
#define MEND CHKNAME('M', 'E', 'N', 'D')

#define GHDR CHKNAME('G', 'H', 'D', 'R')
#define INDI CHKNAME('I', 'N', 'D', 'I')
#define VERT CHKNAME('V', 'E', 'R', 'T')
#define TVER CHKNAME('T', 'V', 'E', 'R')
#define NORM CHKNAME('N', 'O', 'R', 'M')
#define BRST CHKNAME('B', 'R', 'S', 'T')
#define GEND CHKNAME('G', 'E', 'N', 'D')

DOFLoader::DOFLoader()
{
}

DOFLoader::~DOFLoader()
{
    KILLOBJECT(file);
}

bool DOFLoader::LoadDOF(const CPath& filename, CEntity::ENTITYLIST& entities)
{
    CFileManager* fman = CFileManager::Instance();
    if (!(file = fman->open(filename))) {
        std::cout << "DOFLoader::LoadDOF() could not open \"" << filename << "\"" << std::endl;
        return false;
    }

    // FIXME: this must be easier !
    CPath curr_dir;
    fman->getCurrentDirectory(curr_dir);

    if (filename.getType() == CPath::PT_RELATIVE) {
        CPath realpath = CPath(file->getFilename()).getPath();
        fman->setCurrentDirectory(realpath);
    } else {
        fman->setCurrentDirectory(filename.getPath());
    }

    readChunk();

    if (chunk.name != DOF1) {
        std::cout << "DOFLoader::LoadDOF() not a DOF1 file" << std::endl;
        KILLOBJECT(file);
        return false;
    }

    chunk.name    = 0;
    chunk.pos     = 0;
    chunk.length  = 0;
    chunk.maxseek = file->getSize();

    readChunk();
    enterSubChunk();

    int num_materials = 0;

    if (seekChunk(MATS)) {

        num_materials = file->readDWORD();
        CShader* mat;
        enterSubChunk();
        readChunk();
        for (int m = 0; m < num_materials; ++m) {
            pushChunk();
            mat = readMaterial();
            materials.push_back(mat);
            popChunk();
            skipChunk();
        }
        exitSubChunk();
    }

    topChunk();

    if (seekChunk(GEOB)) {
        CEntity*    ent  = new CEntity();
        std::string name = filename.getFilename();
        ent->setName(name);
        CMesh* mesh = new CMesh;
        name += "_mesh";
        mesh->setName(name);

        int num_geobs = file->readDWORD();

        enterSubChunk();
        readChunk();

        for (int g = 0; g < num_geobs; g++) {
            readGeometryObject(mesh);
        }

        mesh->optimize();
        mesh->calcBoundingBox();

        ent->addMesh(mesh);
        ent->getTransformation().setScale(25);

        entities.push_front(ent);

        exitSubChunk();
    }

    materials.clear();
    KILLOBJECT(file);

    // FIXME; an early exit doesnt set back the old path
    fman->setCurrentDirectory(curr_dir);

    return true;
}

void DOFLoader::readGeometryObject(CMesh* mesh)
{
    if (chunk.name != GOB1) {
        std::cout << "DOFLoader::readGeometryObject() not starting at GOB1 chunk" << std::endl;
        return;
    }

    enterSubChunk();
    readChunk();

    if (chunk.name != GHDR) {
        std::cout << "DOFLoader::readGeometryObject() GHDR chunk expected" << std::endl;
        return;
    }

    file->readDWORD();            // skip flags
    file->readDWORD();            // skip drawflags;
    int mat = file->readDWORD();  // MaterialIndex;

    readChunk();
    pushChunk();

    short* indices = NULL;
    int    num_indices;

    if (seekChunk(INDI)) {
        num_indices = file->readDWORD();
        if (!num_indices) {
            std::cout << "DOFLoader::readGeometryObject() funny index count found in file" << std::endl;
            return;
        }
        indices = new short[num_indices];
        file->readVOID(indices, num_indices * sizeof(short));
    }
    topChunk();

    float* vertices = NULL;
    int    num_vertices;
    if (seekChunk(VERT)) {
        num_vertices = file->readDWORD();
        if (!num_vertices) {
            std::cout << "DOFLoader::readGeometryObject() funny vertexcount found in file" << std::endl;
            return;
        }
        vertices = new float[num_vertices * 3];
        file->readVOID(vertices, 3 * num_vertices * sizeof(float));
    } else {
        std::cout << "DOFLoader::readGeometryObject() no vertices found in file" << std::endl;
        return;
    }
    topChunk();

    float* texcoords = NULL;
    if (seekChunk(TVER)) {
        int num_texcoords = file->readDWORD();
        if (num_texcoords != num_vertices) {
            std::cout << "DOFLoader::readGeometryObject() texcoord count differs from vertexcount" << std::endl;
            return;
        }
        texcoords = new float[num_vertices * 2];
        file->readVOID(texcoords, 2 * num_vertices * sizeof(float));
    }
    topChunk();

    float* normals = NULL;
    if (seekChunk(NORM)) {
        int num_normals = file->readDWORD();
        if (num_normals != num_vertices) {
            std::cout << "DOFLoader::readGeometryObject() normal count differs from vertexcount" << std::endl;
            return;
        }
        normals = new float[num_vertices * 3];
        file->readVOID(normals, 3 * num_vertices * sizeof(float));
    }

    topChunk();

    if (!seekChunk(BRST)) {
        std::cout << "DOFLoader::readGeometryObject() BRST chunk not found!" << std::endl;
        return;  // FIXME: clean up allocated memory
    }
    int num_bursts = file->readDWORD();
    if (!num_bursts) {
        std::cout << "DOFLoader::readGeometryObject() funny burst count!" << std::endl;
        return;
    }

    int* bursts = new int[num_bursts * 4];

    file->readVOID(bursts, 4 * num_bursts * sizeof(int));

    int startBurst;
    int burstCount;
    int burstMtlID;
    int burstVperP;

    for (int b = 0; b < num_bursts; ++b) {

        startBurst = bursts[b] / 3;
        burstCount = bursts[b + num_bursts] / 3;
        burstMtlID = bursts[b + 2 * num_bursts];
        burstVperP = bursts[b + 3 * num_bursts];  // should be always 3

        int minIndex = num_vertices;
        int maxIndex = 0;
        int i        = 0;
        for (i = startBurst; i < startBurst + burstCount; ++i) {
            if (indices[i] < minIndex)
                minIndex = indices[i];
            if (indices[i] > maxIndex)
                maxIndex = indices[i];
        }
        int burstNumVertices = maxIndex - minIndex + 1;

        CVertexBuffer* vb = new CVertexBuffer;

        int arrays = VERTEXARRAY | INDEXARRAY | TEXCOORDARRAY;
        arrays     = normals ? arrays | NORMALARRAY : arrays;

        vb->AllocArrays(MT_STATIC, arrays, burstNumVertices, burstCount);
        int j = 0;
        for (i = startBurst; i < startBurst + burstCount; ++i, ++j) {
            vb->indices[j] = indices[i] - minIndex;
        }
        vb->num_indices = burstCount;
        j               = 0;
        for (int v = minIndex; v < minIndex + burstNumVertices; ++v, ++j) {
            vb->vertices[0][j] = VECTOR3(&vertices[v * 3]);
            vb->texcoords[j]   = VECTOR2(&texcoords[v * 2]);
        }
        vb->num_vertices = burstNumVertices;
        if (normals) {
            j = 0;
            for (int v = minIndex; v < burstNumVertices; ++v, ++j) {
                vb->normals[0][j] = VECTOR3(&normals[v * 3]);
            }
        } else {
            vb->createNormals();  // should be really in vbservices
        }

        vb->setShader(materials[mat]);
        mesh->addVertexBuffer(vb);
    }
    KILLARRAY(bursts)
    KILLARRAY(indices)
    KILLARRAY(vertices)
    KILLARRAY(texcoords)
    KILLARRAY(normals)

    popChunk();

    exitSubChunk();

    return;
}

CShader* DOFLoader::readMaterial()
{
    if (chunk.name != MAT0)
        return NULL;

    enterSubChunk();
    readChunk();

    if (chunk.name != MHDR) {
        exitSubChunk();
        return NULL;
    }

    std::string name      = readString();
    std::string classname = readString();

    CShader* shader;
    if (shader = CShader::findShader(name.c_str())) {
        exitSubChunk();
        return shader;
    }

    CTexture* material = new CTexture;
    material->setName(name);

    readChunk();
    pushChunk();

    if (seekChunk(MCOL)) {
        material->setTexParameter(TEX_USEMATERIAL, TEX_USEMATERIAL);
        read4Floats(material->Ambient);
        read4Floats(material->Diffuse);
        read4Floats(material->Specular);
        read4Floats(material->Emissive);
        material->Shininess = file->readFLOAT();
    }
    topChunk();

    int blendmode = 0;
    if (seekChunk(MTRA)) {
        file->readFLOAT();  // transparency, not used
        blendmode = file->readDWORD();
    }

    topChunk();
    if (seekChunk(MTEX)) {
        int num_textures = file->readDWORD();  // I´ll support only 1 texture instead
        if (num_textures) {
            std::string texname = readString();
            CTextureManager::Instance()->loadTexture(material, texname, TEX_FLIPY);
        }
    }

    popChunk();

    shader = new CShader();
    shader->setName(name);

    CShaderPass diffuse;

    diffuse.setRGBGen(RGBGEN_MATERIAL);
    diffuse.addTexture(material);

    if (blendmode == 1)  // src-alpha-blending
    {
        diffuse.setBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    shader->addPass(diffuse);

    CTexture* spec = CTextureManager::Instance()->findOrLoadTexture("smap_whitesun_add.tga", TEX_DONTCOMPRESS);

    if (spec) {
        CShaderPass specular;
        spec->setTexParameter(TEX_CLAMP, TEX_CLAMP);
        specular.setBlendFunc(GL_DST_ALPHA, GL_ONE);
        specular.setTCGen(TCGEN_SPHEREMAP);
        specular.setDetailPass();
        specular.addTexture(spec);

        shader->addPass(specular);
    }

    exitSubChunk();

    return shader;
}

void DOFLoader::read4Floats(float* dest)
{
    for (int i = 0; i < 4; ++i) {
        dest[i] = file->readFLOAT();
    }
}
void DOFLoader::readChunk()
{
    chunk.name   = file->readDWORD();
    chunk.length = file->readDWORD();
    chunk.pos    = file->getPosition();
}
void DOFLoader::pushChunk()
{
    chunkstack.push_front(chunk);
}
void DOFLoader::popChunk()
{
    topChunk();
    chunkstack.pop_front();
}
void DOFLoader::topChunk()
{
    chunk = chunkstack.front();
    file->setPosition(chunk.pos);
}
void DOFLoader::skipChunk()
{
    file->setPosition(chunk.pos + chunk.length);
    readChunk();
}

bool DOFLoader::seekChunk(int chunkname)
{
    pushChunk();
    int maxseek = chunk.maxseek;
    while ((chunk.pos < maxseek) && (chunk.name != chunkname)) {
        skipChunk();
    }
    if (chunk.pos >= maxseek) {
        popChunk();
        return false;
    }
    if (chunk.name == chunkname) {
        chunkstack.pop_front();  // nur oberstes Element löschen, nicht aber Chunk überschreiben
        return true;
    }
    return false;  // never should come here
}

void DOFLoader::enterSubChunk()
{
    pushChunk();
    chunk.maxseek = chunk.pos + chunk.length;
}

void DOFLoader::exitSubChunk()
{
    popChunk();
    skipChunk();
}

std::string DOFLoader::readString()
{
    short len = file->readWORD();
    if (!len)
        return "";
    char* str = new char[len];
    file->readVOID(str, len);
    std::string s = std::string(str, len);
    KILLARRAY(str)
    return s;
}
