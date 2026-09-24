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

#include "Camera.h"

#include <opengl/OpenGL.h>
#include <defs.h>
#include <iostream>

#include "Engine.h"
#include "TNLStack.h"

ConVar r_showboxoutlines("r_showboxoutlines", "0");

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
ConVar              CCamera::r_showboxes("r_showboxes", "0");
CCamera::CAMERALIST CCamera::s_CameraList;
// try for plane-coherency, test decisive plane of last frame first
// but always Farplane last!
const CLIPPLANE CCamera::s_planeseries[6][6] = {{CLIP_LEFT, CLIP_TOP, CLIP_BOTTOM, CLIP_RIGHT, CLIP_NEAR, CLIP_FAR},
                                                {CLIP_RIGHT, CLIP_TOP, CLIP_BOTTOM, CLIP_LEFT, CLIP_NEAR, CLIP_FAR},
                                                {CLIP_TOP, CLIP_LEFT, CLIP_RIGHT, CLIP_BOTTOM, CLIP_NEAR, CLIP_FAR},
                                                {CLIP_BOTTOM, CLIP_LEFT, CLIP_RIGHT, CLIP_TOP, CLIP_NEAR, CLIP_FAR},
                                                {CLIP_NEAR, CLIP_LEFT, CLIP_RIGHT, CLIP_TOP, CLIP_BOTTOM, CLIP_FAR},
                                                {CLIP_FAR, CLIP_LEFT, CLIP_RIGHT, CLIP_TOP, CLIP_BOTTOM, CLIP_NEAR}};
const int       CCamera::s_planebits[]       = {1 << 0, 1 << 1, 1 << 2, 1 << 3, 1 << 4, 1 << 5};
// indexlist: this table stores the 64 possible cases of classification of
// the eyepoint with respect to the 6 defining planes of the bbox (2^6=64)
// only 26 (3^3-1, where 1 is "inside" cube) of these cases are valid.
// the first 6 numbers in each row are the indices of the bbox vertices that
// form the outline of which we want to compute the area (counterclockwise
// ordering), the 7th entry means the number of vertices in the outline.
// there are 6 cases with a single face and and a 4-vertex outline, and
// 20 cases with 2 or 3 faces and a 6-vertex outline. a value of 0 indicates
// an invalid case.

//   NPN+-------+PPN
//     /|      /|
//    / |     / |
//   /NNN+---/--+PNN
// NPP+------+/PPP/    y
//  |  /    |  /     |
//  | /     | /      |
//  |/      |/       |
// NNP+-------+PNP     *---x
//                  /
//				   /
//				  Z
const int CCamera::s_indexlist[64][7] = {

    {-1, -1, -1, -1, -1, -1, 0},      // 0 inside					00 00 00
    {NNN, NNP, NPP, NPN, -1, -1, 4},  // 1 left				00 00 01
    {PNP, PNN, PPN, PPP, -1, -1, 4},  // 2 right			00 00 10
    {-1, -1, -1, -1, -1, -1, 0},      // 3 -

    {NNN, PNN, PNP, NNP, -1, -1, 4},    // 4 bottom 			00 01 00
    {NPP, NNP, PNP, PNN, NNN, NPN, 6},  // 5 bottom, left	00 01 01
    {NNP, NNN, PNN, PPN, PPP, PNP, 6},  // 6 bottom, right00 01 10
    {-1, -1, -1, -1, -1, -1, 0},        // 7 -

    {NPP, PPP, PPN, NPN, -1, -1, 4},    // 8 top				00 10 00
    {NNP, NPP, PPP, PPN, NPN, NNN, 6},  // 9 top, left	00 10 01
    {NPP, PPP, PNP, PNN, PPN, NPN, 6},  // 10 top, right	00 10 10
    {-1, -1, -1, -1, -1, -1, 0},        // 11 -

    {-1, -1, -1, -1, -1, -1, 0},  // 12 -
    {-1, -1, -1, -1, -1, -1, 0},  // 13 -
    {-1, -1, -1, -1, -1, -1, 0},  // 14 -
    {-1, -1, -1, -1, -1, -1, 0},  // 15 -

    {PNN, NNN, NPN, PPN, -1, -1, 4},    // 16 back				01 00 00
    {PNN, NNN, NNP, NPP, NPN, PPN, 6},  // 17 back, left	01 00 01
    {PNP, PNN, NNN, NPN, PPN, PPP, 6},  // 18 back, right	01 00 10
    {-1, -1, -1, -1, -1, -1, 0},        // 19 -

    {PNP, NNP, NNN, NPN, PPN, PNN, 6},  // 20 back, bottom			01 01 00
    {PNP, NNP, NPP, NPN, PPN, PNN, 6},  // 21 back, bottom, left	01 01 01
    {PNP, NNP, NNN, NPN, PPN, PPP, 6},  // 22 back, bottom, right	01 01 10
    {-1, -1, -1, -1, -1, -1, 0},        // 23 -

    {PNN, NNN, NPN, NPP, PPP, PPN, 6},  // 24 back, top				01 10 00
    {PNN, NNN, NNP, NPP, PPP, PPN, 6},  // 25 back, top, left			01 10 01
    {PNP, PNN, NNN, NPN, NPP, PPP, 6},  // 26 back, top, right			01 10 10
    {-1, -1, -1, -1, -1, -1, 0},        // 27 -

    {-1, -1, -1, -1, -1, -1, 0},  // 28 -
    {-1, -1, -1, -1, -1, -1, 0},  // 29 -
    {-1, -1, -1, -1, -1, -1, 0},  // 30 -
    {-1, -1, -1, -1, -1, -1, 0},  // 31 -

    {NNP, PNP, PPP, NPP, -1, -1, 4},    // 32 front			10 00 00
    {NNN, NNP, PNP, PPP, NPP, NPN, 6},  // 33 front, left	10 00 01
    {NNP, PNP, PNN, PPN, PPP, NPP, 6},  // 34 front, right	10 00 11
    {-1, -1, -1, -1, -1, -1, 0},        // 35 -

    {NNP, NNN, PNN, PNP, PPP, NPP, 6},  // 36 front, bottom			10 01 00
    {NNN, PNN, PNP, PPP, NPP, NPN, 6},  // 37 front, bottom, left		10 01 01
    {NNN, PNN, PPN, PPP, NPP, NNP, 6},  // 38 front, bottom, right		10 01 10
    {-1, -1, -1, -1, -1, -1, 0},        // 39 -

    {NNP, PNP, PPP, PPN, NPN, NPP, 6},  // 40 front, top				10 10 00
    {NNP, PNP, PPP, PPN, NPN, NNN, 6},  // 41 front, top, left			10 10 01
    {NNP, PNP, PNN, PPN, NPN, NPP, 6},  // 42 front, top, right		10 10 10
    {-1, -1, -1, -1, -1, -1, 0},        // 43 invalid

    {-1, -1, -1, -1, -1, -1, 0},  // 44 invalid
    {-1, -1, -1, -1, -1, -1, 0},  // 45 invalid
    {-1, -1, -1, -1, -1, -1, 0},  // 46 invalid
    {-1, -1, -1, -1, -1, -1, 0},  // 47 invalid
    {-1, -1, -1, -1, -1, -1, 0},  // 48 invalid
    {-1, -1, -1, -1, -1, -1, 0},  // 49 invalid
    {-1, -1, -1, -1, -1, -1, 0},  // 50 invalid
    {-1, -1, -1, -1, -1, -1, 0},  // 51 invalid
    {-1, -1, -1, -1, -1, -1, 0},  // 52 invalid
    {-1, -1, -1, -1, -1, -1, 0},  // 53 invalid
    {-1, -1, -1, -1, -1, -1, 0},  // 54 invalid
    {-1, -1, -1, -1, -1, -1, 0},  // 55 invalid
    {-1, -1, -1, -1, -1, -1, 0},  // 56 invalid
    {-1, -1, -1, -1, -1, -1, 0},  // 57 invalid
    {-1, -1, -1, -1, -1, -1, 0},  // 58 invalid
    {-1, -1, -1, -1, -1, -1, 0},  // 59 invalid
    {-1, -1, -1, -1, -1, -1, 0},  // 60 invalid
    {-1, -1, -1, -1, -1, -1, 0},  // 61 invalid
    {-1, -1, -1, -1, -1, -1, 0},  // 62 invalid
    {-1, -1, -1, -1, -1, -1, 0}   // 63 invalid
};

using namespace std;

CCamera::CCamera(float nFOV, float nAspect, float nNearZ, float nFarZ)
: m_frustum(6)
{
    FOV                = nFOV;
    Aspect             = nAspect;
    NearZ              = nNearZ;
    FarZ               = nFarZ;
    m_infiniteFarPlane = false;

    s_CameraList.push_back(this);
}

CCamera::~CCamera()
{
    s_CameraList.remove(this);
}

void CCamera::setupCamera()
{
    setupFrustum();
    calculateClipPlanes();
    m_projworld = m_projection * m_transform.getInverseMatrix();
    // m_cameraorigin=m_transform.getTranslation()-m_transform.getColumnVector(2)*NearZ;
    m_cameraorigin = m_transform.getTranslation();

    // glLoadMatrixf(m_transform.getInverseMatrix());
    g_TNLStack->LoadMatrix(m_transform.getInverseMatrix());
}

void CCamera::setupFrustum(float nFOV, float nAspect, float nNearZ, float nFarZ)
{
    FOV    = nFOV;
    Aspect = nAspect;
    NearZ  = nNearZ;
    FarZ   = nFarZ;
    setupFrustum();
    calculateClipPlanes();
}

void CCamera::setupFrustum()
{
    float s;
    glMatrixMode(GL_PROJECTION);
    if (!m_infiniteFarPlane) {
        s = NearZ * tanf(RAD(FOV * 0.5f));
        glLoadIdentity();
        glFrustum(-s, s, -s / Aspect, s / Aspect, NearZ, FarZ);
        glGetFloatv(GL_PROJECTION_MATRIX, m_projection);
    } else {
        float r = NearZ * tanf(RAD(FOV * 0.5f));
        float t = r / Aspect;

        m_projection.m00 = 2.0f * NearZ / (r - (-r));  // 2*n/(r-l)
        m_projection.m11 = 2.0f * NearZ / (t - (-t));  // 2*n/(t-b)
        m_projection.m02 = (r + (-r)) / (r - (-r));    // (r+l)/(r-l)
        m_projection.m12 = (t + (-t)) / (t - (-t));    // (t+b)/(t-b)
        m_projection.m22 = (m_infiniteEpsilon - 1.0f);
        m_projection.m32 = -1.0f;
        m_projection.m23 = NearZ * (m_infiniteEpsilon - 2.0f);
        m_projection.m33 = 0.0f;

        glLoadMatrixf(m_projection);
    }
    glMatrixMode(GL_MODELVIEW);
}
void CCamera::setInfiniteProjection(bool enable, float epsilon)
{
    m_infiniteFarPlane = enable;
    m_infiniteEpsilon  = epsilon;
}

void CCamera::calculateClipPlanes()
{
    MATRIX4 M;
    glGetFloatv(GL_PROJECTION_MATRIX, M);

    M = M * m_transform.getInverseMatrix();

    m_frustum.setFromMatrix(M);
}

bool CCamera::BoxInFrustum(const BBOX& Box, int& Planemask)
{
    // wenn es eine ClipPlane gibt, wo die Box KOMPLETT dahinter ist (Planes zeigen nach innen)
    // kann die Box unmöglich im Frustum liegen

    if ((Planemask & CAMERAPOINT) && PointInBox(m_transform.getTranslation(), Box)) {
        return true;
    }

    Planemask &= ~CAMERAPOINT;  // wenn der Kamerapunkt einmal ausserhalb der BBox ist, so ist er es auch fuer alle
                                // Kinder dieser Box

    // Planemask nicht verändern, wenn Box ausserhalb des Frustum sollte weiteres Testen
    // sowieso aufhören
    if ((Planemask & NEARPLANE) && BoxBehind(Box, m_frustum.m_planes[CLIP_NEAR]))
        return false;
    if ((Planemask & LEFTPLANE) && BoxBehind(Box, m_frustum.m_planes[CLIP_LEFT]))
        return false;
    if ((Planemask & RIGHTPLANE) && BoxBehind(Box, m_frustum.m_planes[CLIP_RIGHT]))
        return false;  // rechts
    if ((Planemask & TOPPLANE) && BoxBehind(Box, m_frustum.m_planes[CLIP_TOP]))
        return false;
    if ((Planemask & BOTTOMPLANE) && BoxBehind(Box, m_frustum.m_planes[CLIP_BOTTOM]))
        return false;
    //	if ((Planemask&FARPLANE)&&BoxBehind(Box,ClipPlanes[CLIP_FAR])) return false;

    if ((int)r_showboxes) {
        glPushMatrix();
        glLoadMatrixf((float*)&m_transform.getInverseMatrix());
        DrawBBox(Box);
        glPopMatrix();
    }

    return true;
}

bool CCamera::BoxInFrustum(const BBOX& Box, int& Planemask, int& early_out)
{
    if ((Planemask & CAMERAPOINT) && PointInBox(m_transform.getTranslation(), Box)) {
        early_out = 4;
        return true;
    }

    Planemask &= ~CAMERAPOINT;  // wenn der Kamerapunkt einmal ausserhalb der BBox ist, so ist er es auch fuer alle
                                // Kinder dieser Box

    // Planemask nicht verändern, wenn Box ausserhalb des Frustum sollte weiteres Testen
    // sowieso aufhören

    const CLIPPLANE* series = s_planeseries[early_out];
    int              plane;
    for (int p = 0; p < 5; ++p) {
        plane = series[p];
        if ((Planemask & s_planebits[plane]) && BoxBehind(Box, m_frustum.m_planes[plane])) {
            early_out = plane;
            return false;
        }
    }
    early_out = 4;

    if ((int)r_showboxes) {
        glPushMatrix();
        glLoadMatrixf((float*)&m_transform.getInverseMatrix());
        DrawBBox(Box);
        glPopMatrix();
    }
    return true;
}

int CCamera::BoxOnFrustumSide(const BBOX& box, int& planemask)
{
    if ((planemask & CAMERAPOINT) && PointInBox(m_transform.getTranslation(), box)) {
        return SPANNING;
    }
    planemask &= ~CAMERAPOINT;  // wenn der Kamerapunkt einmal ausserhalb der BBox ist, so ist er es auch fuer alle
                                // Kinder dieser Box

    int side = 0, allsides = 0;

    for (int p = 0; p < 5; ++p) {
        if (!(planemask & s_planebits[p]))
            continue;
        side = BoxOnPlaneSide(box, m_frustum.m_planes[p]);
        if (side == BACKSIDE) {
            return BACKSIDE;
        } else if (side == FRONTSIDE) {
            planemask ^= s_planebits[p];
        }
        allsides |= side;
    }

    if ((int)r_showboxes) {
        glPushMatrix();
        glLoadMatrixf((float*)&m_transform.getInverseMatrix());
        DrawBBox(box);
        glPopMatrix();
    }

    return allsides;
}

int CCamera::BoxOnFrustumSide(const BBOX& Box, int& Planemask, int& decision)
{
    if ((Planemask & CAMERAPOINT) && PointInBox(m_transform.getTranslation(), Box)) {
        decision = 4;
        return SPANNING;
    }

    Planemask &= ~CAMERAPOINT;  // wenn der Kamerapunkt einmal ausserhalb der BBox ist, so ist er es auch fuer alle
                                // Kinder dieser Box

    int allsides = 0, side = 0;

    const CLIPPLANE* series = s_planeseries[decision];
    unsigned int     plane;
    for (int p = 0; p < 5; ++p) {
        plane = series[p];
        if (!(Planemask & s_planebits[plane]))
            continue;
        side = BoxOnPlaneSide(Box, m_frustum.m_planes[plane]);

        if (side == BACKSIDE) {
            decision = plane;
            return BACKSIDE;
        } else if (side == FRONTSIDE) {
            Planemask ^= s_planebits[plane];
        }
        allsides |= side;
    }

    decision = 4;

    if ((int)r_showboxes) {
        glPushMatrix();
        glLoadMatrixf((float*)&m_transform.getInverseMatrix());
        DrawBBox(Box);
        glPopMatrix();
    }
    return side;
}

// box in worldspace
void CCamera::projectBBox(const BBOX& box, RECTANGLE& rectangle)
{

    // compute 6-bit code to classify eye with respect to the 6 defining planes
    int pos = m_cameraorigin.x < box.min.x;      // 1 = left
    pos |= (m_cameraorigin.x > box.max.x) << 1;  // 2 = right
    pos |= (m_cameraorigin.y < box.min.y) << 2;  // 4 = bottom
    pos |= (m_cameraorigin.y > box.max.y) << 3;  // 8 = top
    pos |= (m_cameraorigin.z < box.min.z) << 4;  // 16 = back
    pos |= (m_cameraorigin.z > box.max.z) << 5;  // 32 = front

    const int* outline          = s_indexlist[pos];
    int        num_outlineverts = s_indexlist[pos][6];  // look up number of vertices in outline
    if (!num_outlineverts)                              // zero indicates invalid case, make whole box visible
    {
        rectangle.min = VECTOR2(-1.0f, -1.0f);
        rectangle.max = VECTOR2(1.0f, 1.0f);
        return;
    }

    VECTOR3 vertexBox[8], clipverts[6];
    // generate the 8 corners of the bbox
    getCornerPoints(box, vertexBox);

    if ((int)r_showboxoutlines) {
        glPushMatrix();
        glLoadMatrixf(m_transform.getInverseMatrix());
        glPushAttrib(GL_ENABLE_BIT | GL_DEPTH_BUFFER_BIT);
        glColor3ub(255, 0, 255);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_ALPHA_TEST);
        glDisable(GL_BLEND);
        glDisable(GL_LIGHTING);
        glDepthFunc(GL_LEQUAL);
        glDepthMask(GL_TRUE);

        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < num_outlineverts; ++i) {
            glVertex3fv((float*)&vertexBox[s_indexlist[pos][i]]);
        }
        glEnd();

        glPopAttrib();

        glPopMatrix();
    }

    int sides[6];
    // transform all outline corners into screenspace
    for (int v = 0; v < num_outlineverts; ++v) {
        clipverts[v] = m_projworld * vertexBox[outline[v]];
        clipverts[v].z += 1.0f;
        sides[v] = (clipverts[v].z > 0.0f) ? FRONTSIDE : BACKSIDE;
    }

    resetRectangle(rectangle);

    VECTOR2 svert;
    // project corners onto screen
    for (int i = 0, j = num_outlineverts - 1; i < num_outlineverts; j = i++) {
        if (sides[i] == FRONTSIDE) {
            // this point is in front of the nearplane
            const VECTOR3& p1 = clipverts[i];
            const double   t =
                1.0 / (double)p1.z;  // do z-divide, should always work, because FRONTSIDE is classified as >0
            svert.x = (double)p1.x * t;
            svert.y = (double)p1.y * t;
            rectangle |= svert;
        }
        if (sides[i] ^ sides[j]) {
            // the outline crossed the nearplane, so clip it and use the clipped point as new corner
            const VECTOR3& p1 = clipverts[i];
            const VECTOR3& p2 = clipverts[j];
            const double   t2 = ((double)p2.z / ((double)p2.z - (double)p1.z));
            const double   t3 = 1.0 - t2;
            svert.x           = ((double)p2.x * t3 +
                       (double)p1.x * t2);  // no further projection neccessary, new point is _on_ the nearplane
            svert.y           = ((double)p2.y * t3 + (double)p1.y * t2);
            rectangle |= svert;
        }
        // points clearly behind nearplane need not to be touched
    }
}

// points in worldspace
void CCamera::projectLineLoop(const VECTOR3* points, int num_points, RECTANGLE& rectangle)
{
    assert(num_points <= 16);

    int     sides[16];
    VECTOR3 clipverts[16];
    // transform all outline corners into screenspace
    for (int v = 0; v < num_points; ++v) {
        clipverts[v] = m_projworld * points[v];
        clipverts[v].z += 1.0f;
        sides[v] = (clipverts[v].z > 0.0f) ? FRONTSIDE : BACKSIDE;
    }

    resetRectangle(rectangle);

    VECTOR2 svert;
    // project corners onto screen
    for (int i = 0, j = num_points - 1; i < num_points; j = i++) {
        if (sides[i] == FRONTSIDE) {
            // this point is in front of the nearplane
            const VECTOR3& p1 = clipverts[i];
            const double   t  = 1.0 / p1.z;  // do z-divide, should always work, because FRONTSIDE is classified as >0
            svert.x           = p1.x * t;
            svert.y           = p1.y * t;
            rectangle |= svert;
        }
        if (sides[i] ^ sides[j]) {
            // the outline crossed the nearplane, so clip it and use the clipped point as new corner
            const VECTOR3& p1 = clipverts[i];
            const VECTOR3& p2 = clipverts[j];
            const double   t2 = p2.z / (p2.z - p1.z);
            const double   t3 = 1.0 - t2;
            svert.x = (p2.x * t3 + p1.x * t2);  // no further projection neccessary, point is _on_ the nearplane
            svert.y = (p2.y * t3 + p1.y * t2);
            rectangle |= svert;
        }
        // points clearly behind nearplane need not to be touched
    }
}