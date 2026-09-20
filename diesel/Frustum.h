#pragma once

#include <vector>

#include <geometry/AABB.h>
#include <math/Plane.h>
// #include "Sphere.h"

enum CLIPPLANE
{
    CLIP_LEFT   = 0,
    CLIP_RIGHT  = 1,
    CLIP_TOP    = 2,
    CLIP_BOTTOM = 3,
    CLIP_NEAR   = 4,
    CLIP_FAR    = 5  // not used
};

class Frustum
{
public:
    Frustum(int size = 0);
    Frustum(PLANE* plns, int num_plns);

    void setFromMatrix(const MATRIX4& M);

    bool BoxInFrustum(const BBOX& bbox) const;
    int  BoxOnFrustumSide(const BBOX& bbox) const;
    bool PointInFrustum(const VECTOR3& point) const;

    /*	bool SphereInFrustum(const SPHERE &sphere) const;
        bool EllipsoidInFrustum(const ELLIPSOID &sphere) const;
    */
    std::vector<PLANE> m_planes;
};
