#pragma once

#include <adt/ConVar.h>
#include <math/Plane.h>
#include <math/Vector3.h>

#include "D3Entity.h"
#include "Mesh.h"

#include <vector>

class DoomMap
{
    friend class DoomLoader;

public:
    typedef std::vector<int>    INDEXLIST;
    typedef INDEXLIST::iterator INDEXITERATOR;

    DoomMap(void);
    ~DoomMap(void);

    void cullMap();

    /** Adds an entity to the bsp tree.
    The bounding box of the entitiy is pushed down the tree. All leafs that touch the box
    get a reference to the entity.
    */
    bool linkEntity(D3Entity* Entity);

    /** return a list of leafnums that are touched by the given bbox */
    void getTouchedAreas(const BBOX& box, INDEXLIST& arealist, int node = 0);

protected:
    struct NODE
    {
        FRUSTUMPLANE plane;
        int          children[2];
    };

    struct PORTAL
    {
        int      areas[2];
        int      num_vertices;
        VECTOR3* vertices;
        PLANE    plane;

        int markportal;

        PORTAL()
        : num_vertices(0)
        , vertices(NULL)
        , markportal(0) {};
        ~PORTAL() { delete[] vertices; };
    };

    struct AREA
    {
        CMesh::SMARTPTR areamodel;
        int             markframe;  // areamodel already pushed to the renderer;

        INDEXLIST portals;

        D3Entity::D3ENTSPTRLIST entities;
    };

    int  getArea(const VECTOR3& position);
    bool portalVisible(const PORTAL& portal, const Frustum& frustum);
    void makeNewFrustum(const VECTOR3& origin, const VECTOR3* portalpoints, int num_points, Frustum& frustum,
                        int otherside);

    void walkTree_r(int node);
    void renderArea_r(int areanum, const Frustum& frustum, int depht = 0);

    int clipPortal(const PLANE& plane, VECTOR3* inverts, int num_inverts, VECTOR3* outverts, int& num_outverts);

    void showportal(const VECTOR3* verts, int num_verts);

    int current_area;

    int num_areas;
    int num_portals;
    int num_nodes;

    AREA*   areas;
    NODE*   nodes;
    PORTAL* portals;

    static ConVar r_showarea;
    static ConVar r_showportals;
    static ConVar r_maxPortalDepth;
    static ConVar r_clipPortals;
};
