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
// 3DSLoader.h: Schnittstelle für die Klasse C3DSLoader.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_3DSLOADER_H__ED8AA187_0546_4097_AB39_C5B3ED7A5A56__INCLUDED_)
#define AFX_3DSLOADER_H__ED8AA187_0546_4097_AB39_C5B3ED7A5A56__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER > 1000

#include <adt/RefCount.h>
#include <file/Path.h>
#include <file/file.h>

#include "Entity.h"
#include "VertexBuffer.h"

class CShader;
class CMesh;

class C3DSLoader
{
public:
    C3DSLoader();
    ~C3DSLoader();

    bool Load3DS(const CPath& Filename, CEntity::ENTITYLIST& EntityList);

protected:
    enum CHUNKNAME
    {
        //>------ Id Chunk

        MAIN3DS = 0x4D4D,
        VERSION = 0x0002,

        //>------ Main Chunks

        EDIT3DS = 0x3D3D,  // this is the start of the editor config
        KEYF3DS = 0xB000,  // this is the start of the keyframer config

        //>------ sub defines of EDIT3DS

        EDIT_MATERIAL = 0xAFFF,
        EDIT_CONFIG1  = 0x0100,
        EDIT_CONFIG2  = 0x3E3D,
        EDIT_VIEW_P1  = 0x7012,
        EDIT_VIEW_P2  = 0x7011,
        EDIT_VIEW_P3  = 0x7020,
        EDIT_VIEW1    = 0x7001,
        EDIT_BACKGR   = 0x1200,
        EDIT_AMBIENT  = 0x2100,
        EDIT_OBJECT   = 0x4000,

        EDIT_UNKNW01 = 0x1100,
        EDIT_UNKNW02 = 0x1201,
        EDIT_UNKNW03 = 0x1300,
        EDIT_UNKNW04 = 0x1400,
        EDIT_UNKNW05 = 0x1420,
        EDIT_UNKNW06 = 0x1450,
        EDIT_UNKNW07 = 0x1500,
        EDIT_UNKNW08 = 0x2200,
        EDIT_UNKNW09 = 0x2201,
        EDIT_UNKNW10 = 0x2210,
        EDIT_UNKNW11 = 0x2300,
        EDIT_UNKNW12 = 0x2302,  // new chunk type
        EDIT_UNKNW13 = 0x3000,
        EDIT_UNKNW14 = 0xAFFF,

        //>------ sub defines of EDIT_MATERIAL
        MAT_NAME01        = 0xA000,  //> includes name (see mli doc for materials)
        MAT_AMBCOLOR      = 0xA010,
        MAT_DIFFCOLOR     = 0xA020,
        MAT_SPECCOLOR     = 0xA030,
        MAT_SHINE         = 0xA040,
        MAT_SHINESTRENGTH = 0xA041,
        MAT_TEXTURE1      = 0xA200,

        //>------ sub defines of EDIT_OBJECT

        OBJ_TRIMESH = 0x4100,
        OBJ_LIGHT   = 0x4600,
        OBJ_CAMERA  = 0x4700,

        OBJ_UNKNWN01 = 0x4010,
        OBJ_UNKNWN02 = 0x4012,  //>>---- Could be shadow

        //>------ sub defines of OBJ_CAMERA
        CAM_UNKNWN01 = 0x4710,  // new chunk type
        CAM_UNKNWN02 = 0x4720,  // new chunk type

        //>------ sub defines of OBJ_LIGHT
        LIT_OFF      = 0x4620,
        LIT_SPOT     = 0x4610,
        LIT_UNKNWN01 = 0x465A,

        //>------ sub defines of OBJ_TRIMESH
        TRI_VERTEXL = 0x4110,
        TRI_FACEL2  = 0x4111,  // unknown yet
        TRI_FACEL1  = 0x4120,
        TRI_FACEMAT = 0x4130,
        TRI_TEXCORD = 0x4140,
        TRI_SMOOTH  = 0x4150,
        TRI_LOCAL   = 0x4160,
        TRI_VISIBLE = 0x4165,

        //>>------ sub defs of KEYF3DS

        KEYF_UNKNWN01 = 0xB009,
        KEYF_UNKNWN02 = 0xB00A,
        KEYF_FRAMES   = 0xB008,
        KEYF_OBJDES   = 0xB002,

        KEYF_OBJHIERARCH  = 0xB010,
        KEYF_OBJDUMMYNAME = 0xB011,
        KEYF_OBJPIVOT     = 0xB013,
        KEYF_OBJUNKNWN02  = 0xB014,
        KEYF_OBJUNKNWN03  = 0xB015,
        KEYF_OBJPOSTRACK  = 0xB020,
        KEYF_OBJROTTRACK  = 0xB021,
        KEYF_OBJTRACK03   = 0xB022,

        //>>------  these define the different color chunk types
        COL_RGB = 0x0010,
        COL_TRU = 0x0011,
        COL_UNK = 0x0013,  // unknown

        DUMMYCHUNK = 0xFFFF  // um WORD-Groesse zu erzwingen

    };

    struct CHUNKHEADER
    {
        CHUNKNAME CName;  // Chunkname
        DWORD     CSize;  // Chunkgroesse
        DWORD     CPos;   // Chunkposition
        DWORD     maxSeek;
    } Chunk;

    struct HIERARCHY
    {
        int      H;
        CEntity* Parent;
    };

    inline void ReadChunk();
    inline BOOL SkipChunk();
    inline BOOL SeekChunk(CHUNKNAME Name);

    inline void PushChunk();
    inline void PopChunk();
    inline void GetTopChunk();

    float ReadPercent();
    void  EnterSubChunk();
    void  ExitSubChunk();

    BOOL           EditorChunk();
    CEntity*       ObjectChunk();
    CMesh*         MeshChunks(CEntity* newEntity);
    void           VertexChunk();
    void           TexCoordChunk();
    void           FaceListChunks(CMesh* Mesh);
    void           FaceMaterialChunks(CVertexBuffer& vb);
    void           LocalChunk(CEntity* Mesh);
    void           KeyChunk();
    void           HierarchyChunks();
    void           CorrectEntity(CEntity* entity, const MATRIX4& m);
    void           MaterialChunk();
    void           ReadMaterial(CShader* Material);
    void           ColorChunk(GLfloat Color[4]);
    void           Read24BitColor(GLfloat Color[4]);
    inline VECTOR3 ReadVector();
    inline void    ReadRGBColor(GLfloat Color[4]);

    CFile*                 file;
    CEntity::ENTITYLIST*   EntitiesList;
    std::list<CHUNKHEADER> ChunkStack;
    CShader::SMARTPTRLIST  Materials;

    CVertexBuffer* vbuffer;  // temporarily holds all per-mesh vertices

    int numEntities;
};

#endif  // !defined(AFX_3DSLOADER_H__ED8AA187_0546_4097_AB39_C5B3ED7A5A56__INCLUDED_)
