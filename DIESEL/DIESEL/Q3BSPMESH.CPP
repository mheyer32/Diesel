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
// BSPMesh.cpp: Implementierung der Klasse Q3BSPMesh.
//
//////////////////////////////////////////////////////////////////////

#include "Q3BSPMesh.h"

#include <cassert>

#include <math/mathlib.h>
#include <opengl/OpenGL.h>

#include "Camera.h"
#include "Engine.h"
#include "Mesh.h"
#include "Q3Entity.h"
#include "Shader.h"
#include "TNLStack.h"

#include <memorytracker.h>

ConVar Q3BSPMesh::r_lockpvs("r_lockpvs", "0");
ConVar Q3BSPMesh::r_usepvs("r_usepvs", "1");
ConVar Q3BSPMesh::r_usebsp("r_usebsp", "1");
ConVar Q3BSPMesh::r_flares("r_flares", "0");
ConVar Q3BSPMesh::r_facePlaneCull("r_facePlaneCull", "1");

// VECTOR3* Q3BSPMesh::latlng2vec=NULL;

Q3BSPMesh::Q3BSPMesh()
{
    num_nodes        = 0;
    num_leafs        = 0;
    num_brushes      = 0;
    num_brushsides   = 0;
    num_leafbrushes  = 0;
    num_surfaceinfos = 0;
    num_planes       = 0;
    num_fogs         = 0;

    nodes        = NULL;
    leafs        = NULL;
    visdata      = NULL;
    faces        = NULL;
    leaffaces    = NULL;
    brushsides   = NULL;
    brushes      = NULL;
    leafbrushes  = NULL;
    surfaceinfos = NULL;
    planes       = NULL;
    fogfeatures  = NULL;

    lightvols       = NULL;
    num_lightvols   = 0;
    num_lightvols_x = 0;
    num_lightvols_y = 0;
    num_lightvols_z = 0;
    gridsize        = VECTOR3(64.0f, 128.0f, 64.0f);

    eyecluster = 0;
    farleaf    = 0;

    checkcount = 0;
    //	if (!latlng2vec) genLatLongTable();
}

Q3BSPMesh::~Q3BSPMesh()
{
    KILLARRAY(nodes);
    KILLARRAY(leafs);
    KILLARRAY(visdata);
    KILLARRAY(faces);
    KILLARRAY(leaffaces);
    KILLARRAY(lightvols);
    KILLARRAY(leafbrushes);
    KILLARRAY(brushsides);
    KILLARRAY(brushes);
    KILLARRAY(surfaceinfos);
    KILLARRAY(planes);
    KILLARRAY(fogfeatures);
}

void Q3BSPMesh::Render()
{
    if (!(int)r_usebsp) {
        for (int f = 0; f < num_faces; ++f) {
            g_TNLStack->batch(faces[f].vbuffer);
        }
        for (int i = 0; i < num_leafs; ++i) {
            Q3Entity::Q3ENTITYITERATOR e = leafs[i].entities.begin();
            for (; e != leafs[i].entities.end(); ++e) {
                if ((*e)->markentity != g_FrameCounter) {
                    (*e)->markentity = g_FrameCounter;
                    (*e)->cullEntity();
                }
            }
        }
        return;
    }

    testcos = -cosf(RAD(90.0f - g_Camera->FOV * 0.5f));

    assert(nodes);

    if (visdata && (int)r_usepvs) {
        int new_eyecluster = getCluster(g_Camera->getTransformation().getTranslation());  // leaf des augpunkts

        if ((new_eyecluster != eyecluster) && (!(int)r_lockpvs)) {
            // FIXME: visdata will point into invalid memory for invalid eyeclusters
            //  but in this case, PVS-lookups aren´t used anyway;
            eyecluster  = new_eyecluster;
            visdata_eye = visdata + ints_per_cluster * eyecluster;
            markLeaves();
        }
    } else {
        if (eyecluster != -1) {
            eyecluster = -1;
            markLeaves();
        }
    }

    RenderNode(0, ALLPLANES);

    // dynamically adjusting zfar to the last leaf rendered (doesn´t work flawlessly)
    // g_Camera->SetupFrustum(g_Camera->FOV,g_Camera->Aspect,g_Camera->NearZ,1000+Length(faces[leaffaces[leafs[farleaf].startface]].vbuffer->vertices[0][0]-g_Camera->Position));
}

bool Q3BSPMesh::RenderNode(int node, int planemask)
{
    if (node < 0) {
        node = ~node;
        if (g_Camera->BoxInFrustum(leafs[node].bbox, planemask, leafs[node].last_earlyout)) {
            return RenderLeaf(node, planemask);
        }
        return true;  // even if this leaf is not visible at the moment, it might be if you turn around
    }

    BSPNODE& pnode = nodes[node];

    if (pnode.marknode == eyecluster) {
        ++skippednodes;
        return false;
    }

    DWORD side = g_Camera->BoxOnFrustumSide(pnode.bbox, planemask, pnode.last_earlyout);

    switch (side) {
    case BACKSIDE:
        return true;  // bbox completely outside the frustum;
    case FRONTSIDE:
        return RenderNodeNoFrustumCulling(node);  // bbox completely contained in frustum
    }

    // this test would not be neccessary if I´d regret the front-to-back traversal
    // the faces won´t be rendered in front-to-back-order either, because quick-sorting (done in TNLStack) them
    // is not a stable sorting algo
    bool rval = false;
    //	if (PointDistance(g_Camera->getTransformation().getTranslation(),planes[pnode.plane])>=PLANE_EPSILON)
    //	{
    // nodes are supposed to always have children
    rval = RenderNode(pnode.front, planemask);
    rval |= RenderNode(pnode.back, planemask);
    /*	}
        else
        {
            rval=RenderNode(pnode.back,planemask);
            rval|=RenderNode(pnode.front,planemask);
        }*/
    if (!rval)
        pnode.marknode = eyecluster;  // no child-leafs are visible from eyecluster (via pvs)
    return rval;
}

bool Q3BSPMesh::RenderNodeNoFrustumCulling(int node)
{
    if (node < 0) {
        return RenderLeaf(~node, 0);
    }

    BSPNODE& pnode = nodes[node];

    if (pnode.marknode == eyecluster) {
        ++skippednodes;
        return false;
    }

    bool rval = false;
    //	if (PointDistance(g_Camera->getTransformation().getTranslation(),planes[pnode.plane])>=PLANE_EPSILON)
    //	{
    rval = RenderNodeNoFrustumCulling(pnode.front);
    rval |= RenderNodeNoFrustumCulling(pnode.back);
    /*	}
        else
        {
            rval=RenderNodeNoFrustumCulling(pnode.back);
            rval|=RenderNodeNoFrustumCulling(pnode.front);
        }*/
    if (!rval)
        pnode.marknode = eyecluster;  // no childnodes are visible from eyecluster
    return rval;
}

bool Q3BSPMesh::RenderLeaf(int leaf, int planemask)
{
    BSPLEAF* pleaf = &leafs[leaf];

    if (pleaf->marknode != eyecluster || pleaf->cluster == -1)
        return false;

    farleaf = leaf;

    ++drawnnodes;

    const unsigned int max_faceindex = pleaf->startface + pleaf->num_faces;
    const VECTOR3&     camdir        = g_Camera->getTransformation().getColumnVector(2);
    RENDERINFO         info;
    for (DWORD f = pleaf->startface; f < max_faceindex; ++f) {
        BSPFACE& face = faces[leaffaces[f]];

        if (face.markface != g_FrameCounter) {
            face.markface = g_FrameCounter;
            switch (face.type) {
            case 1:
                /*
                #ifdef _DEBUG
                                if (	(int)r_facePlaneCull&&
                #else
                                if (
                #endif
                                    !(face.vbuffer->getShader()->getFlags()&(SHADER_NOCULL|SHADER_CULLFRONT)))
                                {
                                    if (face.normal*camdir<testcos) continue;
                                }*/
                face.vbuffer->fillRenderInfo(info);

                break;
            case 2:
                // TODO: a dedicated Patch-class could decide on LOD, maybe tesselating at runtime
                //  It would then add the patchdim-feature itself
                face.vbuffer->fillRenderInfo(info);
                info.features[info.num_features].name   = FEATURE_PATCHDIM;
                info.features[info.num_features++].data = &face.patchdim;
                break;
            case 3:
                face.vbuffer->fillRenderInfo(info);
                break;
            case 4: {
                if (!(int)r_flares)
                    continue;
                VECTOR3* verts = face.vbuffer->vertices[0];
                VECTOR3  mid   = (verts[1] + verts[2]) * 0.5;
                TRACE    trace;
                trace.contents = CONTENTS_SOLID;
                trace.bevel    = 0.0f;
                if (!RayTest(g_Camera->getTransformation().getTranslation(), mid, trace))
                    continue;
                face.vbuffer->fillRenderInfo(info);
            } break;
            }

            if (face.fognum != -1 && face.type != 4) {
                // the renderer will recognize this feature
                // and render a final fog-mapped pass
                info.features[info.num_features].name   = FEATURE_FOG;
                info.features[info.num_features++].data = &fogfeatures[face.fognum];
                // make this thing distinct to polygons with same shader/lightmap but without fog
                // lowest 7 bits are free choice
                info.sortkey |= (face.fognum + 1);
            }

            g_TNLStack->batch(info);
        }
    };

    LIGHT light;

    Q3Entity::Q3ENTITYLIST::const_iterator e = pleaf->entities.begin();

    for (; e != pleaf->entities.end(); ++e) {
        Q3Entity* ent = *e;
        if (ent->markentity != g_FrameCounter) {
            ent->markentity = g_FrameCounter;

            g_TNLStack->pushLights();
            getLightVol(ent->getTransformation().getTranslation(), light);
            g_TNLStack->addLight(light);
            ent->cullEntity(planemask);
            g_TNLStack->popLights();
        }
    }

    return true;
}

int Q3BSPMesh::getLeafNum(const VECTOR3& pos, int node) const
{
    do {
        node = (PointDistance(pos, planes[nodes[node].plane]) <= -PLANE_EPSILON) ? nodes[node].back : nodes[node].front;
    } while (node > 0);
    return ~node;  // Augpunkt liegt in keinem leaf
}

int Q3BSPMesh::getCluster(const VECTOR3& pos, int startnode) const
{
    int cluster = getLeafNum(pos, startnode);
    if (cluster != -1)
        cluster = leafs[cluster].cluster;
    return cluster;
}

inline int Q3BSPMesh::ClusterVisible(const unsigned int fromCluster, const unsigned int toCluster) const
{
    // return 	visdata[fromCluster * ints_per_cluster + (toCluster >> 5)]&( 1 << (toCluster &0x1F));
    return (visdata[fromCluster * ints_per_cluster + (toCluster >> 5)] >> (toCluster & 0x1F)) & 1;
}
inline int Q3BSPMesh::ClusterVisibleToEye(const unsigned int toCluster) const
{
    // return 	visdata_eye[(toCluster >> 5)]&( 1 << (toCluster &0x1F));
    return (visdata_eye[(toCluster >> 5)] >> (toCluster & 0x1F)) & 1;
}

void Q3BSPMesh::getTouchedLeafs(const BBOX& box, INDEXLIST& leafnum_list, int node)
{
    if (node < 0) {
        node = ~node;
        leafnum_list.push_back(node);
        return;
    }
    DWORD side = BoxOnPlaneSide(box, planes[nodes[node].plane]);
    if ((side & FRONTSIDE) || side == ON_PLANE)
        getTouchedLeafs(box, leafnum_list, nodes[node].front);
    if ((side & BACKSIDE) || side == ON_PLANE)
        getTouchedLeafs(box, leafnum_list, nodes[node].back);
}

void Q3BSPMesh::getTouchedBrushes(const BBOX& box, const INDEXLIST& leafnum_list, INDEXLIST& boxnum_list)
{
    INDEXLIST::const_iterator lIt = leafnum_list.begin();
    for (; lIt != leafnum_list.end(); ++lIt) {
        int lb    = leafs[*lIt].startleafbrush;
        int endlb = lb + leafs[*lIt].num_leafbrushes;
        for (; lb < endlb; ++lb) {
            boxnum_list.push_back(leafbrushes[lb]);
        }
    }
    boxnum_list.sort();
    boxnum_list.unique();

    TRACE         trace;
    INDEXITERATOR bIt = boxnum_list.begin();
    for (; bIt != boxnum_list.end();) {
        if (BrushBBoxTest(*bIt, box, trace)) {
            bIt = boxnum_list.erase(bIt);
        } else {
            ++bIt;
        }
    }
}

void Q3BSPMesh::getTouchedBrushes(const BBOX& box, INDEXLIST& boxnum_list)
{
    INDEXLIST leafs;
    getTouchedLeafs(box, leafs);
    getTouchedBrushes(box, leafs, boxnum_list);
}

bool Q3BSPMesh::linkEntity(Q3Entity* Entity)
{

    assert(Entity);

    bool rval = false;

    unlinkEntity(Entity);
    const BBOX& box = Entity->getBoundingBox();

    getTouchedLeafs(box, Entity->leafs);

    INDEXITERATOR lIt = Entity->leafs.begin();

    for (; lIt != Entity->leafs.end();) {
        if (leafs[*lIt].cluster != -1) {
            leafs[*lIt].entities.push_back(Entity);
            rval = true;
            ++lIt;
        } else {
            lIt = Entity->leafs.erase(lIt);
        }
    }

    Entity->fog = -1;
    TRACE trace;

    for (int f = 0; f < num_fogs; ++f) {
        if (!BrushBBoxTest(fogfeatures[f].brushnum, box, trace)) {
            Entity->fog = f;
            break;
        }
    }
    return rval;
}

void Q3BSPMesh::unlinkEntity(Q3Entity* Entity)
{
    assert(Entity);

    INDEXITERATOR lIt = Entity->leafs.begin();
    for (; lIt != Entity->leafs.end(); ++lIt) {
        leafs[*lIt].entities.remove(Entity);
    }
    Entity->leafs.clear();

    Entity->fog = -1;
}

template <class T>
inline T trilinear(const float tx, const float ty, const float tz, const T* p)
{
    const float tx2 = 1.0f - tx;
    const float ty2 = 1.0f - ty;
    const float tz2 = 1.0f - tz;

    T np;
    np = p[0] * (tx2 * ty2 * tz2) + p[1] * (tx * ty2 * tz2) + p[2] * (tx2 * ty2 * tz) + p[3] * (tx * ty2 * tz) +
         p[4] * (tx2 * tz2 * ty) + p[5] * (tx * ty * tz2) + p[6] * (tx2 * ty * tz) + p[7] * (tx * ty * tz);
    return np;
}

template <class T>
inline T trilinear2(const float tx, const float ty, const float tz, const T& p0, const T& p1, const T& p2, const T& p3,
                    const T& p4, const T& p5, const T& p6, const T& p7)
{
    const float tx2 = 1.0f - tx;
    const float ty2 = 1.0f - ty;
    const float tz2 = 1.0f - tz;

    T np;
    np = p0 * (tx2 * ty2 * tz2) + p1 * (tx * ty2 * tz2) + p2 * (tx2 * ty2 * tz) + p3 * (tx * ty2 * tz) +
         p4 * (tx2 * tz2 * ty) + p5 * (tx * ty * tz2) + p6 * (tx2 * ty * tz) + p7 * (tx * ty * tz);
    return np;
}
bool Q3BSPMesh::getLightVol(VECTOR3 pos, LIGHT& light)
{
    if (!lightvols)
        return false;
    pos.x = pos.x - bbox.min.x;
    pos.y = pos.y - bbox.min.y;
    pos.z = bbox.max.z - pos.z;

    float px = pos.x / gridsize.x;
    float py = pos.y / gridsize.y;
    float pz = pos.z / gridsize.z;

    unsigned int lx = (unsigned int)px;
    unsigned int ly = (unsigned int)(py + 0.5f);
    unsigned int lz = (unsigned int)pz;

    if (lx > num_lightvols_x - 2 || ly > num_lightvols_y - 2 || lz > num_lightvols_z - 2)
        return false;

    px = px - floorf(px);
    py = py - floorf(py);
    pz = pz - floorf(pz);

    // FIXME: this check is not sufficiant and causes crashes if you leave the map upwards
    // if (elem>=num_lightvols) return false;

    // by using lat and long instead of computed normals 1) space is saved to exactly 8 bytes
    // 2) both angles can be interpolated linearly
    // the ugly thing is that lat and long are converted to float every time
    const unsigned int elem1 = ly * num_lightvols_z * num_lightvols_x + lz * num_lightvols_x + lx;
    const unsigned int elem2 = elem1 + num_lightvols_x;
    const unsigned int elem3 = elem2 + num_lightvols_x * num_lightvols_z;
    const unsigned int elem4 = elem3 - num_lightvols_x;

    VECTOR3 temp[8];
    temp[0]       = lightvols[elem1].ambient;
    temp[1]       = lightvols[elem1 + 1].ambient;
    temp[2]       = lightvols[elem2].ambient;
    temp[3]       = lightvols[elem2 + 1].ambient;
    temp[4]       = lightvols[elem4].ambient;
    temp[5]       = lightvols[elem4 + 1].ambient;
    temp[6]       = lightvols[elem3].ambient;
    temp[7]       = lightvols[elem3 + 1].ambient;
    light.ambient = trilinear(px, py, pz, temp) * (1.0f / 255.0f);

    temp[0]       = lightvols[elem1].directional;
    temp[1]       = lightvols[elem1 + 1].directional;
    temp[2]       = lightvols[elem2].directional;
    temp[3]       = lightvols[elem2 + 1].directional;
    temp[4]       = lightvols[elem4].directional;
    temp[5]       = lightvols[elem4 + 1].directional;
    temp[6]       = lightvols[elem3].directional;
    temp[7]       = lightvols[elem3 + 1].directional;
    light.diffuse = trilinear(px, py, pz, temp) * (1.0f / 255.0f);

    float newlng = trilinear2(px, py, pz, (float)lightvols[elem1].latlng[0], (float)lightvols[elem1 + 1].latlng[0],
                              (float)lightvols[elem2].latlng[0], (float)lightvols[elem2 + 1].latlng[0],
                              (float)lightvols[elem4].latlng[0], (float)lightvols[elem4 + 1].latlng[0],
                              (float)lightvols[elem3].latlng[0], (float)lightvols[elem3 + 1].latlng[0]);

    float newlat = trilinear2(px, py, pz, (float)lightvols[elem1].latlng[1], (float)lightvols[elem1 + 1].latlng[1],
                              (float)lightvols[elem2].latlng[1], (float)lightvols[elem2 + 1].latlng[1],
                              (float)lightvols[elem4].latlng[1], (float)lightvols[elem4 + 1].latlng[1],
                              (float)lightvols[elem3].latlng[1], (float)lightvols[elem3 + 1].latlng[1]);

    //(VECTOR3&)light.pos=latlng2vec[((int)newlng)<<8|(int)newlat];
    getVecFromLatLong(newlat * PI / 128.0f, newlng * PI / 128.0f, (VECTOR3&)light.pos);
    light.pos[3] = 0.0f;
    light.pos    = -light.pos;

    light.specular = NullVector4;
    light.cutoff   = 180.0f;

    light.used_attribs = DIRECTIONAL_LIGHT;
    return true;
}

bool Q3BSPMesh::PointInsideBrush(int brushnum, const VECTOR3& P)
{
    const Q3BSPMesh::BRUSH& b = brushes[brushnum];

    int bs     = b.startbrushside;
    int lastbs = bs + b.num_brushsides;

    for (; bs < lastbs; ++bs) {
        if (PointDistance(P, planes[brushsides[bs].plane]) > PLANE_EPSILON)
            return false;
    }

    return true;
}

DWORD Q3BSPMesh::PointOnMeshSide(const VECTOR3& P)
{
    int leaf = getLeafNum(P);

    if (leaf == -1 || leafs[leaf].cluster == -1)
        return BACKSIDE;
    if (!brushsides)
        return BACKSIDE;

    DWORD side = FRONTSIDE;

    int leafbrush = leafs[leaf].startleafbrush;

    for (int lb = 0; lb < leafs[leaf].num_leafbrushes; ++lb) {
        if (PointInsideBrush(leafbrushes[lb], P))
            return BACKSIDE;
    }

    return FRONTSIDE;
}

// this is basically taken from q1 source code
bool Q3BSPMesh::RecursiveRayTestNoBevel(int nodenum, const VECTOR3& p1, const VECTOR3& p2, const VECTOR3& start,
                                        const VECTOR3& end, TRACE& trace)
{
    if (nodenum < 0)  // leaf
    {
        nodenum = -nodenum - 1;
        return LeafRayTest(nodenum, start, end, trace);
    }

    PLANE& p  = planes[nodes[nodenum].plane];
    float  t1 = PointDistance(p1, p);
    float  t2 = PointDistance(p2, p);

    if (t1 < 0.0f && t2 < 0.0f)  // both points on backside
    {
        return RecursiveRayTestNoBevel(nodes[nodenum].back, p1, p2, start, end, trace);
    }
    if (t1 >= 0.0f && t2 >= 0.0f)  // frontside
    {
        return RecursiveRayTestNoBevel(nodes[nodenum].front, p1, p2, start, end, trace);
    }

    float frac;
    if (t1 < 0) {
        frac = (t1 + PLANE_EPSILON) / (t1 - t2);  // bring the fragment in back of the plane
    } else {
        frac = (t1 - PLANE_EPSILON) / (t1 - t2);
    }

    if (frac < 0.0f)
        frac = 0;
    else if (frac > 1.0f)
        frac = 1.0f;

    VECTOR3 midpoint = p1 + (p2 - p1) * frac;

    // first check the line segment that is closer to the line start
    if (t1 < 0.0f) {
        if (!RecursiveRayTestNoBevel(nodes[nodenum].back, p1, midpoint, start, end, trace))
            return false;
        return RecursiveRayTestNoBevel(nodes[nodenum].front, midpoint, p2, start, end, trace);

    } else {
        if (!RecursiveRayTestNoBevel(nodes[nodenum].front, p1, midpoint, start, end, trace))
            return false;
        return RecursiveRayTestNoBevel(nodes[nodenum].back, midpoint, p2, start, end, trace);
    }

    return true;  // make compiler happy
}

// TODO: rework maths
bool Q3BSPMesh::RecursiveRayTest(int nodenum, const VECTOR3& p1, const VECTOR3& p2, const VECTOR3& start,
                                 const VECTOR3& end, TRACE& trace)
{

    if (nodenum < 0)  // leaf
    {
        nodenum = -nodenum - 1;
        return LeafRayTest(nodenum, start, end, trace);
    }

    PLANE& p  = planes[nodes[nodenum].plane];
    float  t1 = PointDistance(p1, p);
    float  t2 = PointDistance(p2, p);

    if (t1 <= -trace.bevel && t2 <= -trace.bevel)  // both points on backside
    {
        return RecursiveRayTest(nodes[nodenum].back, p1, p2, start, end, trace);
    }
    if (t1 >= trace.bevel && t2 >= trace.bevel)  // frontside
    {
        return RecursiveRayTest(nodes[nodenum].front, p1, p2, start, end, trace);
    }

    if (fabs(t1) <= trace.bevel && fabs(t2) <= trace.bevel)  // both points very close to the plane
    {
        return RecursiveRayTest(nodes[nodenum].back, p1, p2, start, end, trace) &&
               RecursiveRayTest(nodes[nodenum].front, p1, p2, start, end, trace);
    }

    // one of the points is far away, the other is within the bevel-range of the plane
    // either on the same or the other side
    float frontfrac, backfrac;

    if (t1 < 0.0f) {
        frontfrac = (t1 - trace.bevel) / (t1 - t2);
        backfrac  = (t1 + trace.bevel) / (t1 - t2);

    } else {
        frontfrac = (t1 + trace.bevel) / (t1 - t2);
        backfrac  = (t1 - trace.bevel) / (t1 - t2);
    }

    VECTOR3 frontpoint = p1 + (p2 - p1) * frontfrac;
    VECTOR3 backpoint  = p1 + (p2 - p1) * backfrac;

    // first check the line segment that is closer to the line start
    // FIXME: this is no good :-(
    bool r;
    if (t1 < 0.0f) {
        r = RecursiveRayTest(nodes[nodenum].back, p1, frontpoint, start, end, trace);
        r = r && RecursiveRayTest(nodes[nodenum].front, backpoint, p2, start, end, trace);
        return r;

    } else {
        r = RecursiveRayTest(nodes[nodenum].front, p1, backpoint, start, end, trace);
        r = r && RecursiveRayTest(nodes[nodenum].back, frontpoint, p2, start, end, trace);
        return r;
    }

    return true;
}

bool Q3BSPMesh::BrushRayTest(int brushnum, const VECTOR3& p1, const VECTOR3& p2, TRACE& trace)
{

    BRUSH* b         = &brushes[brushnum];
    int    bs        = b->startbrushside;
    int    lastbs    = bs + b->num_brushsides;
    int    brushside = -1;

    float tnear = -1.0f;
    float tfar  = 1.0f;
    float t1, t2, frac;
    bool  startsolid = true;
    int   side       = BACKSIDE;

    for (; bs < lastbs; ++bs) {

        // if (!(surfaceinfos[brushsides[bs].surfaceinfo].contents&trace.contents)) continue;
        t1 = PointDistance(p1, planes[brushsides[bs].plane]) - trace.bevel;
        t2 = PointDistance(p2, planes[brushsides[bs].plane]) - trace.bevel;

        if ((t1 < 0.0f) && (t2 < 0.0f))  // beide dahinter, es könnte noch eine andere brushside geben, die
        {                                // ein anderes Ergebnis liefert
            continue;
        }
        if ((t1 >= 0) && (t2 >= 0)) {
            // side=FRONTSIDE;
            // break;
            return true;
        }
        frac = t1 / (t1 - t2);  // immer positiv
        if (t1 >= 0)            // startpunkt auf Vorderseite des Brushes
        {
            startsolid = false;
            if (frac >= tfar)  // ist vorderseiten-abstand grösser rückseiten-abstand, ging der strahl am Brush vorbei
            {
                // side=FRONTSIDE;
                // break;
                return true;
            }
            if (frac > tnear) {
                side      = SPANNING;
                brushside = bs;
                tnear     = frac;
            }
        } else {
            if (frac <= tnear) {
                // side=FRONTSIDE;
                // break;
                return true;
            }
            if (frac < tfar) {
                side      = SPANNING;
                brushside = bs;
                tfar      = frac;
            }
        }
    }

    trace.contents   = surfaceinfos[brushes[brushnum].surfaceinfo].contents;
    trace.startsolid = startsolid;
    trace.brushside  = brushside;

    if (side == SPANNING) {
        trace.allsolid = 0;

        if (startsolid) {
            // trace went from inside to outside the brush
            if (tfar < trace.fraction)
                trace.fraction = tfar;
            else
                return true;
        } else {
            if (tnear < trace.fraction)
                trace.fraction = tnear;
            else
                return true;
        }
        assert(trace.fraction >= 0.0f && trace.fraction <= 1.0f);
        trace.hitpoint = p1 * (1.0f - trace.fraction) + p2 * trace.fraction;
        return false;
    } else if (side == BACKSIDE) {
        trace.allsolid = 1;
        return true;
    }
    return true;
}
bool Q3BSPMesh::LeafRayTest(int leaf, const VECTOR3& p1, const VECTOR3& p2, TRACE& trace)
{
    int leafbrush = leafs[leaf].startleafbrush;
    int lastbrush = -1;

    int  lastbrushside = -1;
    int  brushnum;
    bool result = true;
    /*
        if (leafs[leaf].cluster==-1)
        {
            std::cout<<"LeafRayTest(): solid leaf reached"<<std::endl;
            std::cout<<"numbers of brushes:"<<leafs[leaf].num_leafbrushes<<std::endl;

        }
    */

    for (int lb = 0; lb < leafs[leaf].num_leafbrushes; ++lb, ++leafbrush) {
        brushnum = leafbrushes[leafbrush];
        if (!(surfaceinfos[brushes[brushnum].surfaceinfo].contents & trace.contents) ||
            brushes[brushnum].checkcount == checkcount) {
            continue;
        }
        brushes[brushnum].checkcount = checkcount;
        result                       = result && BrushRayTest(brushnum, p1, p2, trace);
    }

    return result;
}

bool Q3BSPMesh::RayTest(const VECTOR3& p1, const VECTOR3& p2, TRACE& trace)
{
    if (!brushsides)
        return true;  // nicht kollidiert, da nicht getestet, Kandidat für exception

    ++checkcount;

    trace.fraction = 1.0f;
    trace.allsolid = 0;

    bool result       = true;
    int  lastbrushnum = -1;

    if (trace.bevel == 0.0f) {
        result = RecursiveRayTestNoBevel(0, p1, p2, p1, p2, trace);
    } else {
        // this one would be the real test, using the bsp
        result = RecursiveRayTest(0, p1, p2, p1, p2, trace);
    }

    // since I don´t know, if the clip-ray-against-bloated-bsp method works already
    // this will just test every brush
    /*	result=true;
        for (int b=0;b<num_brushes;++b)
        {
            if (!(surfaceinfos[brushes[b].surfaceinfo].contents&trace.contents)) continue;
            if (!BrushRayTest(b,p1,p2,trace))
            {
                lastbrushnum=b;
                result=false;
            }
        }
    */

    trace.hitpoint = p2 * trace.fraction + p1 * (1.0f - trace.fraction);
    return result;
}

// returns index of surfaceinfo or -1
int Q3BSPMesh::PointContents(const VECTOR3& Pos, int startnode, int filter_contents)
{
    int leaf;
    if ((leaf = getLeafNum(Pos, startnode)) == -1)
        return -1;

    const int endbrush = leafs[leaf].startleafbrush + leafs[leaf].num_leafbrushes;

    for (int b = leafs[leaf].startleafbrush; b < endbrush; ++b) {
        if (surfaceinfos[brushes[leafbrushes[b]].surfaceinfo].contents & filter_contents) {
            if (!PointInsideBrush(leafbrushes[b], Pos))
                continue;  // assume, brushes are discrete
            return brushes[leafbrushes[b]].surfaceinfo;
        }
    }

    return -1;
}

bool Q3BSPMesh::BrushBBoxTest(int brushnum, const BBOX& box, TRACE& trace)
{
    BRUSH* b      = &brushes[brushnum];
    int    bs     = b->startbrushside;
    int    lastbs = bs + b->num_brushsides;
    int    brushside;

    float dist;
    float neardist = -99999.0f;

    for (; bs < lastbs; ++bs) {
        dist = BoxDistance(box, planes[brushsides[bs].plane]);
        if (dist >= 0.0f) {
            //	std::cout<<"brush: "<< brushnum<< "box in front"<<std::endl;
            return true;
        }
        if (dist > neardist) {
            neardist  = dist;
            brushside = bs;
        }
    }

    /*	VECTOR3 points[8];
        getCornerPoints(box,points);

        for (int i=0;i<8; ++i)
        {
            if (PointInsideBrush(brushnum,points[i])) return false;
        }
    return true;*/

    // std::cout<<"brush: "<< brushnum<< "box inside"<<std::endl;
    if (neardist > trace.fraction) {
        trace.fraction  = neardist;
        trace.brushside = brushside;
        trace.contents  = surfaceinfos[brushsides[brushside].surfaceinfo].contents;
    }
    return false;
}

bool Q3BSPMesh::LeafBBoxTest(int leafnum, const BBOX& box, TRACE& trace)
{
    int leafbrush     = leafs[leafnum].startleafbrush;
    int lastleafbrush = leafbrush + leafs[leafnum].num_leafbrushes;
    int brushnum;

    bool result = true;

    for (; leafbrush < lastleafbrush; ++leafbrush) {
        brushnum = leafbrushes[leafbrush];
        // std::cout<<"Q3BSPMesh:LeafBBoxTest() testing brsuh: "<<brushnum<<std::endl;
        if (!(surfaceinfos[brushes[brushnum].surfaceinfo].contents & trace.contents) ||
            brushes[brushnum].checkcount == checkcount)
            continue;
        result                       = result && BrushBBoxTest(brushnum, box, trace);
        brushes[brushnum].checkcount = checkcount;
    }

    return result;
}

bool Q3BSPMesh::BBoxTest(const BBOX& box, TRACE& trace)
{
    INDEXLIST leafnums;

    ++checkcount;

    getTouchedLeafs(box, leafnums);

    INDEXITERATOR lIt = leafnums.begin();

    bool result    = true;
    trace.fraction = -99999.0f;
    trace.allsolid = false;

    for (; lIt != leafnums.end(); ++lIt) {
        std::cout << "Q3BSPMesh::BBoxTest() testing leaf: " << *lIt << std::endl;
        result = result && LeafBBoxTest(*lIt, box, trace);
    }

    return result;
}

void Q3BSPMesh::markLeaves()
{
    if (eyecluster == -1) {
        for (int l = 0; l < num_leafs; ++l)
            leafs[l].marknode = eyecluster;
        return;
    }

    for (int l = 0; l < num_leafs; ++l) {
        if ((leafs[l].cluster != -1) && ClusterVisibleToEye(leafs[l].cluster)) {
            leafs[l].marknode = eyecluster;
        }
    }
}
