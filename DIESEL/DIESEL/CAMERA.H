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

#pragma once

#include <list>

#include <adt/ConVar.h>
#include <adt/NamedObject.h>
#include <geometry/aabb.h>
#include <math/plane.h>

#include "Rect.h"

#include "Frustum.h"
#include "transformation.h"

/** these values will be used for planemask values */
#define LEFTPLANE (1 << 0)
#define RIGHTPLANE (1 << 1)
#define TOPPLANE (1 << 2)
#define BOTTOMPLANE (1 << 3)
#define NEARPLANE (1 << 4)
#define FARPLANE (1 << 5)
#define CAMERAPOINT (1 << 6)
#define ALLPLANES LEFTPLANE | RIGHTPLANE | TOPPLANE | BOTTOMPLANE | NEARPLANE | CAMERAPOINT  //|FARPLANE

// TODO: more privacy, unify Cameras and Entities

class CCamera : public CNamedObject
{
public:
    typedef std::list<CCamera*>  CAMERALIST;
    typedef CAMERALIST::iterator CAMERAITERATOR;

    CCamera(float nFOV = 90.0f, float nAspect = 1.0f, float nNearZ = 1.0f, float nFarZ = 5000.0f);
    virtual ~CCamera();

    void setupFrustum(float nFOV, float nAspect = 1.0f, float nNearZ = 1.0f, float nFarZ = 5000.0f);

    void setInfiniteProjection(bool enable, float epsilon);

    // load Projection Matrix, load ModelViewMatrix, calculate Clipplanes for frustum
    void setupCamera();
    void setupFrustum();

    // syntactically equivalent to StaticEntity::getTransformation()
    inline Transformation& getTransformation() { return m_transform; };

    int  BoxOnFrustumSide(const BBOX& Box, int& Planemask);
    int  BoxOnFrustumSide(const BBOX& Box, int& Planemask, int& early_out);
    bool BoxInFrustum(const BBOX& Box, int& Planemask);
    bool BoxInFrustum(const BBOX& Box, int& Planemask, int& early_out);

    void            calculateClipPlanes();
    inline PLANE&   getClipPlane(CLIPPLANE plane) { return m_frustum.m_planes[plane]; };
    inline Frustum& getFrustum() { return m_frustum; };

    // project the given primitive into normalized device independed coordinates (still need viewport-transfomation)
    void projectBBox(const BBOX& box, RECTANGLE& rectangle);
    void projectLineLoop(const VECTOR3* points, int num_points, RECTANGLE& rectangle);

    // FIXME: more privacy!
    float FOV;
    float FarZ;
    float NearZ;
    float Aspect;

    static ConVar r_showboxes;

protected:
    Transformation m_transform;  /// camspace->worldspace transformation

    MATRIX4 m_projection;  ///< projection matrix in use
    MATRIX4 m_projworld;   ///< precomputed projection*world

    Frustum m_frustum;
    VECTOR3 m_cameraorigin;  ///< world-origin of camera, projected to nearplane
    float   m_infiniteEpsilon;
    bool    m_infiniteFarPlane;

    static CAMERALIST      s_CameraList;  // still not needed, delete ??
    static const CLIPPLANE s_planeseries[6][6];
    static const int       s_indexlist[64][7];
    static const int       s_planebits[6];
};
