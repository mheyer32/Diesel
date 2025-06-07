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
// Q3BSPLoader.h: Schnittstelle für die Klasse Q3BSPLoader.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Q3BSPLOADER_H__2A70A7BA_5A45_4EE8_BD68_B9D9D489E1AA__INCLUDED_)
#define AFX_Q3BSPLOADER_H__2A70A7BA_5A45_4EE8_BD68_B9D9D489E1AA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER > 1000

#include <adt/ConVar.h>
#include <math/plane.h>
#include <string>

#include "Entity.h"
#include "Q3Scene.h"
#include "VertexBuffer.h"
#include "features.h"
#include "q3bspmesh.h"

#define LIGHTMAP_WIDTH 128
#define LIGHTMAP_HEIGHT 128
#define LIGHTMAPSIZE (LIGHTMAP_WIDTH * LIGHTMAP_HEIGHT * 3)  // RGB

class CShader;
class CTexture;
class CFile;

class Q3BSPLoader
{
public:
    Q3BSPLoader();
    ~Q3BSPLoader();

    Q3Scene* LoadQ3BSP(std::string Filename);

protected:
    enum LUMP
    {
        LUMP_ENTITIES     = 0,
        LUMP_SHADERS      = 1,
        LUMP_PLANES       = 2,
        LUMP_NODES        = 3,
        LUMP_LEAFS        = 4,
        LUMP_LEAFSURFACES = 5,
        LUMP_LEAFBRUSHES  = 6,
        LUMP_MODELS       = 7,
        LUMP_BRUSHES      = 8,
        LUMP_BRUSHSIDES   = 9,
        LUMP_DRAWVERTS    = 10,
        LUMP_DRAWINDEXES  = 11,
        LUMP_FOGS         = 12,
        LUMP_SURFACES     = 13,
        LUMP_LIGHTMAPS    = 14,
        LUMP_LIGHTGRID    = 15,
        LUMP_VISIBILITY   = 16
    };

    struct DIRENTRY
    {
        int offset;
        int length;
    };

    struct Q3HEADER
    {
        int      ident;
        int      version;
        DIRENTRY lumps[17];
    };

    struct Q3SHADER
    {
        char shader[64];
        int  surfflags;
        int  contents;
    };

    struct Q3NODE
    {
        int plane;
        int children[2];
        int mins[3];
        int maxs[3];
    };

    struct Q3PLANE
    {
        float n[3];
        float d;
    };

    struct Q3LEAF
    {
        int cluster;
        int area;
        int mins[3];
        int maxs[3];
        int firstleafsurface;
        int num_leafsurfaces;
        int firstleafbrush;
        int num_leafbrushes;
    };

    struct Q3MODEL
    {
        float mins[3];
        float maxs[3];
        int   firstsurface;
        int   num_surfaces;
        int   firstbrush;
        int   num_brushes;
    };

    struct Q3BRUSH
    {
        int firstside;
        int num_sides;
        int shadernum;
    };

    struct Q3BRUSHSIDE
    {
        int plane;
        int shadernum;
    };

    struct Q3VERTEX
    {
        float         position[3];
        float         texcoords[2][2];
        float         normal[3];
        unsigned char color[4];
    };

    struct Q3FOG
    {
        char shadername[64];
        int  brushnum;
        int  visibleside;
    };

    enum SURFACETYPE
    {
        MST_BAD           = 0,
        MST_PLANAR        = 1,
        MST_PATCH         = 2,
        MST_TRIANGLE_SOUP = 3,
        MST_FLARE         = 4
    };

    struct Q3SURFACE
    {
        int   shadernum;
        int   fognum;
        int   type;
        int   firstvertex;
        int   num_vertices;
        int   firstindex;
        int   num_indexes;
        int   lightmapnum;
        int   lightmap_x;
        int   lightmap_y;
        int   lightmap_width;
        int   lightmap_height;
        float lightmap_origin[3];
        float lightmap_vecs[2][3];
        float normal[3];
        int   patchwidth;
        int   patchheight;
    };

    struct Q3LIGHTVOL
    {
        unsigned char ambient[3];
        unsigned char directional[3];
        unsigned char dir[2];  // 0=phi, 1=theta
    };

    void ReadShaders();
    void ReadLeafSurfaces();
    void ReadVertices();
    void ReadModels();
    void ReadNodesAndLeafs();
    void ReadIndexes();
    void ReadSurfaces();
    void ReadLightmaps();
    void ReadPlanes();
    void ReadVisdata();
    void ReadEntities();
    void ReadLightgrid();
    void ReadBrushSides();
    void ReadBrushes();
    void ReadLeafBrushes();
    void ReadEffects();

    void evaluatePatch(const Q3SURFACE& face, Q3BSPMesh::BSPFACE& bspface);
    void generateFlare(const Q3SURFACE& face, CVertexBuffer& vb);

    Q3HEADER header;
    Q3MODEL* models;

    Q3SHADER*  shaders;
    CTexture** lightmaps;

    Q3BSPMesh* worldmodel;
    Q3Scene*   scene;

    CVertexBuffer* vbuffer;  // wird erstmal alle Vertices halten, speater wird er wieder zerlegt

    CFile* file;

    CMesh::SMARTPTRLIST meshlist;  // when its goin to be destroyed, all created but unused meshes will be released

    static ConVar r_curvefactor;
    static ConVar r_mapOverBrightBits;
};
#endif  // !defined(AFX_Q3BSPLOADER_H__2A70A7BA_5A45_4EE8_BD68_B9D9D489E1AA__INCLUDED_)
