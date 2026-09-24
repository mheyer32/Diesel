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
#include "doommap.h"
#include <iostream>
#include "Camera.h"
#include "Engine.h"

using namespace std;

ConVar DoomMap::r_showarea("r_showarea", "0");
ConVar DoomMap::r_showportals("r_showportals", "1");
ConVar DoomMap::r_maxPortalDepth("r_maxPortalDepth", "20");
ConVar DoomMap::r_clipPortals("r_clipPortals", "1");

DoomMap::DoomMap(void)
{
    num_areas   = 0;
    num_portals = 0;
    num_nodes   = 0;

    areas   = NULL;
    portals = NULL;
    nodes   = NULL;
}

DoomMap::~DoomMap(void)
{
    delete[] areas;
    delete[] portals;
    delete[] nodes;
}

void DoomMap::cullMap()
{

    const VECTOR3& campos = g_Camera->getTransformation().getTranslation();

    int areanum = getArea(campos);

    if ((int)r_showarea && current_area != areanum)
        cout << "Camera in Area: " << areanum << endl;
    current_area = areanum;

    // cout<<"DoomMap::cullMap():"<<num_areas<<" "<<num_portals<<endl;
    if (areanum == -1) {

        int planemask = -1;
        for (int a = 0; a < num_areas; ++a) {
            if (g_Camera->BoxInFrustum(areas[a].areamodel->getBoundingBox(), planemask)) {
                areas[a].areamodel->batchFaces();
            }
        }
        return;
    }

    renderArea_r(areanum, g_Camera->getFrustum(), 0);
}

int DoomMap::getArea(const VECTOR3& position)
{
    int node = 0;
    do {
        node =
            (nodes[node].plane.n * position > nodes[node].plane.d) ? nodes[node].children[0] : nodes[node].children[1];
    } while (node > 0);

    if (!node)
        return -1;
    return ~node;
}

bool DoomMap::portalVisible(const PORTAL& portal, const Frustum& frustum)
{
    int                           side = 0;
    std::vector<PLANE>::size_type size = frustum.m_planes.size();
    for (std::vector<PLANE>::size_type p = 0; p < size; ++p) {
        side = 0;
        for (int v = 0; v < portal.num_vertices && side != SPANNING; ++v) {
            side |= PointOnPlaneSide(portal.vertices[v], frustum.m_planes[p]);
        }
        if (side == BACKSIDE)
            return false;
    }
    return true;
}

void DoomMap::makeNewFrustum(const VECTOR3& origin, const VECTOR3* portalpoints, int num_points, Frustum& newfrustum,
                             int otherside)
{
    newfrustum.m_planes.clear();
    if (otherside) {
        for (int v = 0; v < num_points; ++v) {
            // if (Distance(portalpoints[v],portalpoints[NEXTINDEX(v,num_points)])<0.4f) continue;
            newfrustum.m_planes.push_back(PLANE(origin, portalpoints[v], portalpoints[NEXTINDEX(v, num_points)]));
        }
    } else {
        for (int v = 0; v < num_points; ++v) {
            // if (Distance(portalpoints[v],portalpoints[PREVINDEX(v,num_points)])<0.4f) continue;
            newfrustum.m_planes.push_back(PLANE(origin, portalpoints[v], portalpoints[PREVINDEX(v, num_points)]));
        }
    }
}

#define MAX_PORTAL_CLIPVERTS 10

void DoomMap::renderArea_r(int areanum, const Frustum& frustum, int depth)
{
    // if ((int)r_showarea) cout<<"visting Area: "<<areanum<<endl;

    if (depth > (int)r_maxPortalDepth) {
        cout << "r_maxPortalDepth reached" << endl;
        return;
    }

    AREA& area = areas[areanum];

    if (area.markframe != g_FrameCounter) {
        area.markframe = g_FrameCounter;  // we may visit the area twice, but won´t render it twice
        area.areamodel->batchFaces();

        drawnnodes++;

        D3Entity::D3ENTSPTRITERATOR eIt = area.entities.begin();
        for (; eIt != area.entities.end(); ++eIt) {
            CEntity* ent = *eIt;
            if (ent->markentity != g_FrameCounter) {
                ent->markentity = g_FrameCounter;
                ent->cullEntity();
            }
        }
    }

    VECTOR3 clipverts1[MAX_PORTAL_CLIPVERTS];
    VECTOR3 clipverts2[MAX_PORTAL_CLIPVERTS];

    INDEXITERATOR iIt = area.portals.begin();
    for (; iIt != area.portals.end(); ++iIt) {
        int     portalnum = (*iIt);
        PORTAL& portal    = portals[portalnum];

        if (portal.markportal)
            continue;  // don´t walk this portal twice on the current path through the area-portal-area graph

        int otherside = 1;
        if (areanum == portal.areas[1]) {
            otherside = 0;
        }

        memcpy(clipverts1, portal.vertices, portal.num_vertices * sizeof(VECTOR3));

        VECTOR3* inverts     = clipverts1;
        VECTOR3* outverts    = clipverts2;
        int      num_inverts = portal.num_vertices;
        int      num_outverts;

        // if (fabs(PointDistance(g_Camera->getTransformation().getTranslation(),portal.plane))>0.1)
        {
            if ((int)r_clipPortals) {
                bool skipportal        = false;
                int  num_frustumplanes = frustum.m_planes.size();
                for (int p = 0; p < num_frustumplanes; ++p) {
                    int side = clipPortal(frustum.m_planes[p], inverts, num_inverts, outverts, num_outverts);
                    if (side == BACKSIDE || side == ON_PLANE) {
                        skipportal = true;
                        break;
                    } else if (side == SPANNING) {
                        VECTOR3* temp = inverts;
                        inverts       = outverts;
                        outverts      = temp;
                        num_inverts   = num_outverts;
                    }
                }
                if (skipportal)
                    continue;
            } else {
                if (!portalVisible(portal, frustum))
                    continue;
            }
        }

        portal.markportal = 1;  // we walk this portal from current area, but don´t do this twice

        if ((int)r_showportals)
            showportal(inverts, num_inverts);

        Frustum newfrustum;

        makeNewFrustum(g_Camera->getTransformation().getTranslation(), inverts, num_inverts, newfrustum, otherside);

        renderArea_r(portal.areas[otherside], newfrustum, depth + 1);

        portal.markportal = 0;
    }
}

void DoomMap::showportal(const VECTOR3* verts, int num_verts)
{
    glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT);
    glDisable(GL_DEPTH_TEST);
    glColor3ub(0, 255, 0);
    glBegin(GL_LINE_LOOP);
    for (int v = 0; v < num_verts; ++v) {
        glVertex3fv((float*)&verts[v]);
    }
    glEnd();
    glPopAttrib();
}

int DoomMap::clipPortal(const PLANE& plane, VECTOR3* inverts, int num_inverts, VECTOR3* outverts, int& num_outverts)
{
    int   sides[MAX_PORTAL_CLIPVERTS + 1];
    float dists[MAX_PORTAL_CLIPVERTS + 1];

    int allsides = 0;

    for (int v = 0; v < num_inverts; ++v) {
        dists[v] = PointDistance(inverts[v], plane);
        if (dists[v] > PLANE_EPSILON) {
            sides[v] = FRONTSIDE;
        } else if (dists[v] < -PLANE_EPSILON) {
            sides[v] = BACKSIDE;
        } else {
            sides[v] = ON_PLANE;
        }
        allsides |= sides[v];
    }
    if (allsides != SPANNING)  // portal completely on one side of the plane
    {
        return allsides;
    }

    sides[num_inverts]   = sides[0];  // clone vertex so we always have a valid v+1 vertex
    dists[num_inverts]   = dists[0];
    inverts[num_inverts] = inverts[0];
    num_outverts         = 0;

    for (int v = 0; v < num_inverts; ++v) {
        switch (sides[v]) {
        case FRONTSIDE:
            outverts[num_outverts++] = inverts[v];
            break;
        case ON_PLANE:
            outverts[num_outverts++] = inverts[v];
            continue;
            break;
        }
        if ((sides[v] | sides[v + 1]) != SPANNING)
            continue;

        float t                  = dists[v] / (dists[v] - dists[v + 1]);
        outverts[num_outverts++] = inverts[v] * (1.0f - t) + inverts[v + 1] * t;
    }
    return SPANNING;
}

void DoomMap::getTouchedAreas(const BBOX& box, INDEXLIST& arealist, int node)
{
    if (node < 0) {
        node = ~node;
        arealist.push_back(node);
        return;
    }
    int side = BoxOnPlaneSide(box, nodes[node].plane);
    if (((side & FRONTSIDE) || side == ON_PLANE) && nodes[node].children[0])
        getTouchedAreas(box, arealist, nodes[node].children[0]);
    if (((side & BACKSIDE) || side == ON_PLANE) && nodes[node].children[1])
        getTouchedAreas(box, arealist, nodes[node].children[1]);
}

bool DoomMap::linkEntity(D3Entity* Entity)
{
    INDEXLIST arealist;
    getTouchedAreas(Entity->getBoundingBox(), arealist);

    if (arealist.empty()) {
        cout << "tried to link Entity '" << Entity->getName() << "' into void" << endl;
        return false;
    }

    INDEXITERATOR iIt = arealist.begin();
    for (; iIt != arealist.end(); ++iIt) {
        areas[*iIt].entities.push_back(Entity);
    }

    return true;
}
