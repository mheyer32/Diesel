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
