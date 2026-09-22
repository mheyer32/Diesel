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
// q3bspmesh.h: Schnittstelle für die Klasse Q3BSPMesh.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BSPMESH_H__FA8E89A0_F11C_11D3_BD8A_0000E85E86C1__INCLUDED_)
#define AFX_BSPMESH_H__FA8E89A0_F11C_11D3_BD8A_0000E85E86C1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <adt/ConVar.h>
#include <misc/Exception.h>
#include "EngineTypes.h"
#include "Features.h"
#include "Light.h"
#include "Mesh.h"
#include "Q3Entity.h"

#include "SurfaceFlags.h"

class Q3BSPMesh
{
    // FIXME: reduce friendships!
    friend class Q3BSPLoader;
    friend class Q3EntityFactory;
    friend class Q3Scene;
    friend class Q3Entity;

public:
    struct TRACE
    {
        VECTOR3 hitpoint;
        float   fraction;
        int     brushside;
        int     allsolid;
        int     startsolid;
        int     contents;
        float   bevel;
        TRACE()
        : contents(0)
        , bevel(0.0f)
        , allsolid(0)
        , startsolid(0) {};
    };

    // FIXME is it worth to reduce the size to 32 Bytes ?
    struct BSPNODE  // wird letztlich von BSPMesh zum Rendern benutzt
    {
        int  marknode;
        int  plane;  // index der planes im plane-array
        int  last_earlyout;
        int  front, back;  // darunterliegende Teilbäume als Indizes im Nodes-Array;
        BBOX bbox;         // BBox für Boundingbox

        BSPNODE() { front = back = last_earlyout = marknode = 0; }
    };

    struct BSPLEAF
    {
        int  marknode;
        int  cluster;
        BBOX bbox;
        int  last_earlyout;
        int  startface;
        int  num_faces;
        int  startleafbrush;
        int  num_leafbrushes;

        Q3Entity::Q3ENTITYLIST entities;

        BSPLEAF()
        {
            startface = num_faces = 0;
            startleafbrush = num_leafbrushes = last_earlyout = 0;
            cluster                                          = -1;
        }
    };

    struct BSPFACE
    {
        int                     type;
        int                     fognum;
        int                     markface;
        CVertexBuffer::SMARTPTR vbuffer;  // beherbergt shader und lightmap

        VECTOR3 tang_t;  // tangentsystem of the face
        VECTOR3 tang_s;
        VECTOR3 normal;
        VECTOR3 origin;

        int patchdim[2];
        BSPFACE()
        {
            type        = 0;
            fognum      = -1;
            markface    = 0;
            tang_t      = NullVector3;
            tang_s      = NullVector3;
            normal      = NullVector3;
            origin      = NullVector3;
            patchdim[0] = patchdim[1] = 0;
        }
    };

    struct LIGHTVOL  // comfortable 8 bytes
    {
        COLOR3 ambient;
        COLOR3 directional;
        BYTE   latlng[2];
    };

    struct BRUSH
    {
        int startbrushside;
        int num_brushsides;
        int surfaceinfo;
        int checkcount;
    };

    struct BRUSHSIDE
    {
        int plane;
        int surfaceinfo;
    };

    struct SURFACEINFO
    {
        CShader::SMARTPTR shader;
        int               contents;
        int               surfflags;
    };

    typedef std::list<int>      INDEXLIST;
    typedef INDEXLIST::iterator INDEXITERATOR;

    Q3BSPMesh();
    virtual ~Q3BSPMesh();

    /** render the bsp from g_Camera´s position, render all subentities*/
    void Render();

    /** get the leaf index for the given position */
    int getLeafNum(const VECTOR3& pos, int node = 0) const;

    /** get the cluster index for the given position */
    int getCluster(const VECTOR3& pos, int startnode = 0) const;

    /** can fromCluster and toCluster possibly see each other ? */
    int ClusterVisible(const unsigned int fromCluster, const unsigned int toCluster) const;

    /** returns the contents for a given position
        the content-flags are inside "surfaceflags.h"
    */
    int PointContents(const VECTOR3& Pos, int startnode = 0, int filter_contents = -1);

    /** Adds an entity to the bsp tree.
    The bounding box of the entitiy is pushed down the tree. All leafs that touch the box
    get a reference to the entity.
    */
    bool linkEntity(Q3Entity* Entity);
    void unlinkEntity(Q3Entity* Entity);

    /** get static lighting info for the given position */
    bool getLightVol(VECTOR3 pos, LIGHT& light);

    /** returns if the given point is inside (FRONTSIDE) or outside (BACKSIDE) of the map */
    DWORD PointOnMeshSide(const VECTOR3& P);

    /** tests if the given ray hits a brush that has at least one of the contents-flag given
    as contents inside the trace-structure
    if the rays hits something, it returns false and the trace structure is properly filled
    */
    bool RayTest(const VECTOR3& p1, const VECTOR3& p2, TRACE& trace);

    /** returns the leafstructure for leaf  "leafnum" */
    inline BSPLEAF& getLeaf(int leafnum) const
    {
        // if (leafnum>=num_leafs) throw CException("Q3BSPMesh::getLeaf(): invalid leafnum requested");
        return leafs[leafnum];
    }

    /** return face-index
    leafface is  n´th face of the leaf
    */
    inline int getFaceNum(unsigned int leafnum, unsigned int leafface) const
    {
        return leaffaces[leafs[leafnum].startface + leafface];
    }

    /** return face-structure for given faceindex

    */
    inline BSPFACE& getFace(int facenum) const
    {
        if (facenum >= num_faces)
            throw CException("Q3BSPMesh::getFace(): invalid facenum requested");
        return faces[facenum];
    }

    inline const BRUSHSIDE& getBrushside(int brushside) const
    {
        assert(brushsides);
        assert(brushside < num_brushsides);
        return brushsides[brushside];
    }
    inline const SURFACEINFO& getSurfaceInfo(int surfaceinfo) const
    {
        assert(surfaceinfos);
        assert(surfaceinfo < num_surfaceinfos);
        return surfaceinfos[surfaceinfo];
    }
    inline const PLANE& getPlane(int planenum) const
    {
        assert(planenum < num_planes);
        return planes[planenum];
    }

    inline const FOGFEATURE& getFog(int fognum) const
    {
        assert(fognum < num_fogs);
        return fogfeatures[fognum];
    }

    /** return a list of leafnums that are touched by the given bbox */
    void getTouchedLeafs(const BBOX& box, INDEXLIST& leafnum_list, int node = 0);

    /** FIXME: untested */
    void getTouchedBrushes(const BBOX& box, const INDEXLIST& leafnum_list, INDEXLIST& boxnum_list);
    void getTouchedBrushes(const BBOX& box, INDEXLIST& boxnum_list);

    bool BBoxTest(const BBOX& box, TRACE& trace);

    // FIXME public for testing purposes
    LIGHT DLight;

    int           eyecluster;
    unsigned int* visdata;
    unsigned int* visdata_eye;  // pointer to pvs-vector of eyecluster

    static ConVar r_lockpvs;
    static ConVar r_usepvs;
    static ConVar r_usebsp;
    static ConVar r_flares;
    static ConVar r_facePlaneCull;

protected:
    bool LeafBBoxTest(int leafnum, const BBOX& box, TRACE& trace);
    bool BrushBBoxTest(int brushnum, const BBOX& box, TRACE& trace);
    bool BrushRayTest(int brushnum, const VECTOR3& p1, const VECTOR3& p2, TRACE& trace);
    void genLatLongTable();

    bool LeafRayTest(int leafnum, const VECTOR3& p1, const VECTOR3& p2, TRACE& trace);
    bool RecursiveRayTest(int nodenum, const VECTOR3& p1, const VECTOR3& p2, const VECTOR3& start, const VECTOR3& end,
                          TRACE& trace);
    bool RecursiveRayTestNoBevel(int nodenum, const VECTOR3& p1, const VECTOR3& p2, const VECTOR3& start,
                                 const VECTOR3& end, TRACE& trace);

    bool PointInsideBrush(int brushnum, const VECTOR3& P);

    // return true, if any leaf is visible from a certain node
    inline bool RenderNode(int node, int planemask);
    inline bool RenderNodeNoFrustumCulling(int node);
    inline bool RenderLeaf(int leaf, int planemask);

    void       markLeaves();
    inline int ClusterVisibleToEye(const unsigned int toCluster) const;

    void drawBrush(int brushnum, float bevel = 0.0f);

    float testcos;
    int   checkcount;

    BSPNODE*     nodes;
    BSPLEAF*     leafs;
    BSPFACE*     faces;
    int*         leaffaces;
    int*         leafbrushes;
    BRUSH*       brushes;
    BRUSHSIDE*   brushsides;
    LIGHTVOL*    lightvols;
    SURFACEINFO* surfaceinfos;
    FOGFEATURE*  fogfeatures;
    PLANE*       planes;
    VECTOR3      gridsize;

    int num_lightvols_x;
    int num_lightvols_y;
    int num_lightvols_z;
    int num_lightvols;

    int num_nodes;
    int num_leafs;
    int num_faces;
    int num_leaffaces;
    int num_leafbrushes;
    int num_brushes;
    int num_brushsides;
    int num_surfaceinfos;
    int num_planes;
    int num_clusters;
    int ints_per_cluster;
    int num_fogs;

    BBOX bbox;

    int farleaf;

    //	static VECTOR3  *latlng2vec;
};

#endif  // !defined(AFX_BSPMESH_H__FA8E89A0_F11C_11D3_BD8A_0000E85E86C1__INCLUDED_)
