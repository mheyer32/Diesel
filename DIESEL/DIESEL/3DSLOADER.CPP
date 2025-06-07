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
// 3DSLoader.cpp: Implementierung der Klasse C3DSLoader.
//
//////////////////////////////////////////////////////////////////////

#include <file/filemanager.h>
#include <math/mathlib.h>
#include <misc/exception.h>
#include "3DSLoader.h"
#include "Mesh.h"
#include "Shader.h"
#include "TextureManager.h"
#include "VertexBuffer.h"

#include <memorytracker.h>
#include <iostream>
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

//>>------ defines for viewport chunks
#define TOP 0x0001
#define BOTTOM 0x0002
#define LEFT 0x0003
#define RIGHT 0x0004
#define FRONT 0x0005
#define BACK 0x0006
#define USER 0x0007
#define CAMERA 0x0008  // 0xFFFF is the code read from file
#define LLIGHT 0x0009
#define DISABLED 0x0010
#define BOGUS 0x0011

C3DSLoader::C3DSLoader()
{
    file    = NULL;
    vbuffer = new CVertexBuffer();
    vbuffer->AddRef();
}

C3DSLoader::~C3DSLoader()
{
    if (file)
        file->close();
    KILLOBJECT(file);
    ChunkStack.clear();

    if (vbuffer)
        vbuffer->Release();
}

bool C3DSLoader::Load3DS(const CPath& Filename, CEntity::ENTITYLIST& entlist)
{
    CFileManager* fman = CFileManager::Instance();
    if (!(file = fman->open(Filename)))
        return false;

    EntitiesList = &entlist;

    CPath curr_dir;
    fman->getCurrentDirectory(curr_dir);

    CPath realpath = file->getFilename();
    realpath       = realpath.getPath();
    fman->setCurrentDirectory(realpath);

    numEntities   = 0;
    Chunk.CName   = DUMMYCHUNK;
    Chunk.CPos    = 0;
    Chunk.CSize   = 0;
    Chunk.maxSeek = file->getSize();

    ReadChunk();

    if (Chunk.CName != MAIN3DS) {
        throw CException("Load3DS: MAIN3DS Chunk erwartet");
        return false;
    };

    EnterSubChunk();
    ReadChunk();  // und zum ersten SubChunk vorrücken

    if (SeekChunk(EDIT3DS) != TRUE) {
        // FIXME: exception needed ?
        throw CException("Load3DS: EDIT3DS Chunk erwartet");
        return false;
    }

    EditorChunk();

    if (SeekChunk(KEYF3DS) == TRUE) {
        KeyChunk();
    }

    ExitSubChunk();

    // move all entities in hierarchy to their correct modelspace
    MATRIX4                 m   = IdentityMatrix4;
    CEntity::ENTITYITERATOR eIt = EntitiesList->begin();
    for (; eIt != EntitiesList->end(); ++eIt) {
        CorrectEntity(*eIt, m);
    }

    fman->setCurrentDirectory(curr_dir);

    return true;
}

BOOL C3DSLoader::EditorChunk()
{
    CEntity* newEntity;
    EnterSubChunk();
    ReadChunk();  // und zum ersten SubChunk vorrücken

    while (SeekChunk(EDIT_MATERIAL) == TRUE) {
        MaterialChunk();
    }

    while (SeekChunk(EDIT_OBJECT) == TRUE) {
        newEntity = ObjectChunk();
        if (newEntity != NULL) {
            EntitiesList->push_front(newEntity);
            ++numEntities;
        }
    }
    ExitSubChunk();
    return TRUE;
}

CEntity* C3DSLoader::ObjectChunk()
{
    CEntity* newEntity = new CEntity();
    CMesh*   newMesh;
    CHARLINE Name;

    EnterSubChunk();
    file->readSTRING(Name);
    ReadChunk();
    newEntity->setName(Name);
    strcat(Name, "_mesh");
    if (SeekChunk(OBJ_TRIMESH) == TRUE) {
        newMesh = MeshChunks(newEntity);
        if (newMesh) {
            newEntity->getMesh()->setName(Name);
        }
    }
    ExitSubChunk();

    newEntity->getBoundingBox();

    return newEntity;
}

CMesh* C3DSLoader::MeshChunks(CEntity* newEntity)
{
    CMesh* newMesh = new CMesh();

    EnterSubChunk();
    ReadChunk();  // und zum ersten SubChunk vorrücken
    PushChunk();
    if (SeekChunk(TRI_VERTEXL) == TRUE) {
        VertexChunk();
    } else {
        throw CException("Load3DS: TRI_VERTEXL-Chunk erwartet!");
        return NULL;
    }
    if (vbuffer->num_vertices > 0) {
        GetTopChunk();
        if (SeekChunk(TRI_TEXCORD) == TRUE) {
            TexCoordChunk();
        }
        GetTopChunk();
        if (SeekChunk(TRI_FACEL1) == TRUE) {
            FaceListChunks(newMesh);
        } else {
            throw CException("Load3DS: TRI_FACEL1-Chunk erwartet!");
            return NULL;
        }

        newMesh->mergeVertexBuffers();
        newMesh->calcBoundingBox();

        newEntity->addMesh(newMesh);

    } else {
        KILLOBJECT(newMesh);  // leeres Mesh (C4D exportiert sowas) -> GruppenEntity??
    }

    GetTopChunk();
    if (SeekChunk(TRI_LOCAL) == TRUE) {
        LocalChunk(newEntity);
    }
    PopChunk();

    // newMesh->OptimizeDrawOrders();

    vbuffer->FreeArrays();

    ExitSubChunk();
    return newMesh;
}

void C3DSLoader::VertexChunk()
{
    unsigned int num_verts;

    PushChunk();
    num_verts = file->readWORD();
    if (num_verts > 0) {
        vbuffer->AllocArrays(MT_SYSMEM, VERTEXARRAY, num_verts);
        vbuffer->num_vertices = num_verts;
        for (int v = 0; v < vbuffer->num_vertices; ++v) {
            vbuffer->vertices[0][v] = ReadVector();
        }
    } else {
        vbuffer->num_vertices = 0;
    }
    PopChunk();
    SkipChunk();
}

void C3DSLoader::TexCoordChunk()
{
    PushChunk();

    file->readWORD();  // num_Vertices skippen

    vbuffer->AllocArrays(MT_SYSMEM, TEXCOORDARRAY, vbuffer->num_vertices);

    for (WORD v = 0; v < vbuffer->num_vertices; ++v) {
        vbuffer->texcoords[v].x = file->readFLOAT();
        vbuffer->texcoords[v].y = file->readFLOAT();
    }
    PopChunk();
    SkipChunk();
}

void C3DSLoader::FaceListChunks(CMesh* Mesh)
{
    DWORD num_allFaces      = 0;
    DWORD num_FaceListFaces = 0;
    DWORD index = 0, f = 0, num_FaceLists = 0;

    // erstmal alle Faces in ALLEN Facelists summieren
    PushChunk();
    while (SeekChunk(TRI_FACEL1) == TRUE) {
        PushChunk();
        num_allFaces += file->readWORD();
        ++num_FaceLists;
        PopChunk();
        SkipChunk();
    }
    PopChunk();

    VECTOR3 v1, v2, n;

    WORD i1, i2, i3;
    WORD face = 0;

    while (SeekChunk(TRI_FACEL1) == TRUE) {
        EnterSubChunk();
        num_FaceListFaces = file->readWORD();

        if (!num_FaceListFaces) {
            std::cout << "WARNING: C3DSLoader::FaceListChunks() chunk contains 0 facelists" << std::endl;
        }

        CVertexBuffer* vb = new CVertexBuffer();

        vb->AllocArrays(MT_SYSMEM, INDEXARRAY, 0, num_FaceListFaces * 3);
        vb->num_indices = num_FaceListFaces * 3;

        index = 0;
        for (f = 0; f < num_FaceListFaces; ++f) {
            i1 = file->readWORD();
            i2 = file->readWORD();
            i3 = file->readWORD();
            file->readWORD();  // Kanten-Optionen skippen

            // Fläche aufnehmen
            vb->indices[index++] = (INDEX)i1;
            vb->indices[index++] = (INDEX)i2;
            vb->indices[index++] = (INDEX)i3;
        }

        INDEX min = MAX_INDEX, max = 0;

        for (f = 0; f < vb->num_indices; ++f) {
            if (vb->indices[f] > max)
                max = vb->indices[f];
            if (vb->indices[f] < min)
                min = vb->indices[f];
        }
        for (f = 0; f < vb->num_indices; ++f) {
            vb->indices[f] -= (INDEX)min;
        }

        vb->AllocArrays(MT_STATIC, VERTEXARRAY | NORMALARRAY, max - min + 1);
        vb->setVertices(&vbuffer->vertices[0][min], max - min + 1);

        if (vbuffer->texcoords) {
            vb->AllocArrays(MT_STATIC, TEXCOORDARRAY, vb->getMaxNumVertices());
            vb->setTexCoords(&vbuffer->texcoords[min], vb->num_vertices);
        }

        ReadChunk();  // hoffentlich folgt nun der/die Material-Chunk

        if (SeekChunk(TRI_FACEMAT) == TRUE) {
            FaceMaterialChunks(*vb);
        } else {
            vb->setShader(CShader::DefaultShader);
        }

        ExitSubChunk();
        vb->createNormals();

        Mesh->addVertexBuffer(vb);
    }

    return;
}
void C3DSLoader::FaceMaterialChunks(CVertexBuffer& face)
{
    CHARLINE name;
    CShader* shader;
    // WORD	num_faces;
    PushChunk();
    file->readSTRING(name);

    shader = CShader::findShader(name);

    if (shader != NULL) {
        // num_Faces=ReadWord();  // Anzahl der Flächen mit diesem material
        // face=ReadWord(); //gefolgt von den Indizes der betroffenen Flächen, brauche ich nicht
        face.setShader(shader);
    } else {
        std::cout << "C3DSLoader::FaceMaterialChunks() shader '" << name << "' not found, using default shader"
                  << std::endl;
        face.setShader(CShader::DefaultShader);
    }

    PopChunk();
    SkipChunk();
    return;
}

void C3DSLoader::LocalChunk(CEntity* Entity)
{
    VECTOR3 t;
    MATRIX4 m;

    PushChunk();
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 3; ++j) {
            m.a[i][j] = file->readFLOAT();
        }
    }

    // convert desired rotation to our coordinate system
    MATRIX4 mat2;
    mat2[5]  = 0;
    mat2[9]  = 1;
    mat2[6]  = -1;
    mat2[10] = 0;

    MATRIX4 mat3(mat2);
    Transpose4(mat3);

    m = mat2 * m * mat3;

    Entity->getTransformation().setMatrix(m);
    PopChunk();
    SkipChunk();
    return;
}

void C3DSLoader::KeyChunk()
{
    //	throw CException("Key-Chunk erreicht");
    EnterSubChunk();
    HierarchyChunks();
    ExitSubChunk();
}

void C3DSLoader::HierarchyChunks()
{
    signed short int        nH;
    CHARLINE                name;
    CEntity*                nextEntity;
    BOOL                    found;
    HIERARCHY               HPos;
    std::list<HIERARCHY>    Stack;
    CEntity::ENTITYITERATOR e;

    HPos.H      = -1;
    HPos.Parent = NULL;

    Stack.push_front(HPos);

    while (SeekChunk(KEYF_OBJDES) == TRUE) {
        //		throw CException("KEYF ObjectDescription erreicht");

        EnterSubChunk();
        ReadChunk();  // und zum ersten SubChunk vorrücken

        if (SeekChunk(KEYF_OBJHIERARCH) == TRUE) {
            file->readSTRING(name);  // Name des Objekts einlesen
            file->readWORD();
            file->readWORD();
            nH = file->readWORD();  // und seine Hierarchie-Ebene

            if (EQ(name, "$$$DUMMY")) {
                ReadChunk();
                if (Chunk.CName == KEYF_OBJDUMMYNAME) {

                    PushChunk();
                    file->readSTRING(name);
                    nextEntity = new CEntity;
                    nextEntity->setName(name);
                    EntitiesList->push_front(nextEntity);

                    MATRIX4 m;
                    if (HPos.Parent) {
                        m = HPos.Parent->getTransformation().getMatrix();
                    }
                    nextEntity->getTransformation().setMatrix(m);

                    PopChunk();
                    SkipChunk();
                }
            }
            found = FALSE;

            for (e = EntitiesList->begin(); e != EntitiesList->end(); ++e) {
                if (name == (*e)->getName()) {
                    nextEntity = *e;
                    found      = TRUE;
                    break;
                }
            }
            if (found == TRUE) {
                if (nH != -1) {
                    if (nH <= HPos.H) {
                        while (HPos.H >= nH) {
                            HPos = Stack.front();
                            Stack.pop_front();
                        }
                    }
                    if (HPos.Parent != NULL) {
                        EntitiesList->remove(nextEntity);
                        HPos.Parent->addChild(nextEntity);
                    }
                }
                Stack.push_front(HPos);
                HPos.Parent = nextEntity;
                HPos.H      = nH;
            }
        }
        ExitSubChunk();
    }

    return;
}

// Weltkoordinaten wieder zurück in Objektkoordinaten überführen.
// es sind bereits alle Objekte eingelesen und in der richtigen Hierarchie im baum gespeichert

void C3DSLoader::CorrectEntity(CEntity* entity, const MATRIX4& m)
{
    CEntity::ENTITYITERATOR e;

    MATRIX4 m3, im;

    FullInverse4(m, im);

    MATRIX4 m2 = entity->getTransformation().getMatrix();

    for (int i = 0; i < entity->getNumChildren(); ++i) {
        CEntity* child = entity->getChild(i);
        CorrectEntity(child, m2);
    }

    m3 = im * m2;
    entity->getTransformation().setMatrix(m3);

    if (entity->getMesh()) {
        FullInverse4(m2, m3);
        entity->getMesh()->transform(m3);
        entity->getMesh()->calcBoundingBox();
    }

    entity->getBoundingBox();
}

void C3DSLoader::MaterialChunk()
{
    CShader*  newMaterial;
    CTexture* tex;
    CHARLINE  name;

    EnterSubChunk();
    ReadChunk();  // und zum ersten SubChunk vorrücken
    PushChunk();

    if (Chunk.CName == MAT_NAME01) {
        file->readSTRING(name);

        if (!(newMaterial = CShader::findShader(name))) {
            // std::cout<<"C3DSLoader::MaterialChunk() created shader '"<<name<<"'"<<std::endl;

            tex = new CTexture();
            tex->setName(name);
            CShaderPass pass;
            pass.addTexture(tex);
            pass.setRGBGen(RGBGEN_MATERIAL);
            newMaterial = new CShader();
            newMaterial->setName(name);
            newMaterial->addPass(pass);

            Materials.push_back(newMaterial);  // push every Material in smartpointerlist, in the end the list will be
                                               // erased, killing unused materials automatically

            GetTopChunk();
            if (SeekChunk(MAT_AMBCOLOR) == TRUE) {
                ReadChunk();
                ColorChunk(tex->Ambient);
                tex->setTexParameter(TEX_USEMATERIAL, TEX_USEMATERIAL);
            }

            GetTopChunk();
            if (SeekChunk(MAT_DIFFCOLOR) == TRUE) {
                ReadChunk();
                ColorChunk(tex->Diffuse);
                tex->setTexParameter(TEX_USEMATERIAL, TEX_USEMATERIAL);
            }
            GetTopChunk();
            if (SeekChunk(MAT_SPECCOLOR) == TRUE) {
                ReadChunk();
                ColorChunk(tex->Specular);
                tex->setTexParameter(TEX_USEMATERIAL, TEX_USEMATERIAL);
            }
            GetTopChunk();
            if (SeekChunk(MAT_SHINE) == TRUE) {
                ReadChunk();
                tex->Shininess = (1.0f - ReadPercent()) * 128.0f;
                tex->setTexParameter(TEX_USEMATERIAL, TEX_USEMATERIAL);
            }
            GetTopChunk();
            if (SeekChunk(MAT_SHINESTRENGTH) == TRUE) {
                ReadChunk();
                float s = ReadPercent();
                tex->Specular[0] *= s;
                tex->Specular[1] *= s;
                tex->Specular[2] *= s;
            }

            GetTopChunk();

            if (SeekChunk(MAT_TEXTURE1) == TRUE) {
                EnterSubChunk();
                ReadChunk();  // Tex_Amount_of einlesen
                SkipChunk();  // Tex_Amount_of skippen
                CHARLINE filename;
                file->readSTRING(filename);  // Textur-Filename einlesen
                CTextureManager::Instance()->loadTexture(tex, filename);
                // der Rest interessiert erstmal nicht;
                ExitSubChunk();
            }
        }
        PopChunk();
    }

    ExitSubChunk();

    return;
}

void C3DSLoader::ReadChunk()
{
    Chunk.CName = (CHUNKNAME)file->readWORD();
    Chunk.CSize = file->readDWORD() - 6;
    Chunk.CPos  = file->getPosition();
}

BOOL C3DSLoader::SkipChunk()
{
    Chunk.CPos += Chunk.CSize;
    file->setPosition(Chunk.CPos);
    if (Chunk.CPos < Chunk.maxSeek)
        ReadChunk();  // falls ExitSubChunk ganz am Ende des Files aufgerufen wird
    return TRUE;
}

BOOL C3DSLoader::SeekChunk(CHUNKNAME Name)
{
    PushChunk();
    DWORD maxseek = Chunk.maxSeek;
    while ((Chunk.CPos < maxseek) && (Chunk.CName != Name)) {
        SkipChunk();
    }
    if (Chunk.CPos >= maxseek) {
        PopChunk();
        return FALSE;
    }
    if (Chunk.CName == Name) {

        ChunkStack.pop_front();  // nur oberstes Element löschen, nicht aber Chunk überschreiben
        return TRUE;
    }
    return FALSE;
}

VECTOR3 C3DSLoader::ReadVector()
{
    VECTOR3 v;
    v.x = file->readFLOAT();
    v.z = -file->readFLOAT();
    v.y = file->readFLOAT();
    return v;
}

void C3DSLoader::ColorChunk(GLfloat Color[4])
{

    if ((Chunk.CName == 0x0010) || (Chunk.CName == 0x0013)) {
        ReadRGBColor(Color);
        return;
    }
    if ((Chunk.CName == 0x0011) || (Chunk.CName == 0x0012)) {
        Read24BitColor(Color);
        return;
    }
}

void C3DSLoader::ReadRGBColor(GLfloat Color[4])
{
    Color[0] = file->readFLOAT();
    Color[1] = file->readFLOAT();
    Color[2] = file->readFLOAT();
}

void C3DSLoader::Read24BitColor(GLfloat Color[4])
{
    Color[0] = (float)file->readBYTE() / 255.0f;
    Color[1] = (float)file->readBYTE() / 255.0f;
    Color[2] = (float)file->readBYTE() / 255.0f;
}

float C3DSLoader::ReadPercent()
{
    float f;
    if (Chunk.CName == 0x0030) {
        WORD i = file->readWORD();
        f      = (float)i / 100.0f;
    }
    if (Chunk.CName == 0x0031) {
        f = file->readFLOAT();
    }
    return f;
}

void C3DSLoader::EnterSubChunk()
{
    PushChunk();                               // oberen Chunk auf Stack
    Chunk.maxSeek = Chunk.CPos + Chunk.CSize;  // Seek nur innerhalb dieses Chunks zulassen
}

void C3DSLoader::ExitSubChunk()
{
    PopChunk();   // wieder an den Anfang des Parent-Chunks
    SkipChunk();  // und gesamten Chunk überspringen
}

void C3DSLoader::PushChunk()
{
    ChunkStack.push_front(Chunk);
}

void C3DSLoader::PopChunk()
{
    GetTopChunk();
    ChunkStack.pop_front();
}

void C3DSLoader::GetTopChunk()
{
    Chunk = ChunkStack.front();
    file->setPosition(Chunk.CPos);
}
