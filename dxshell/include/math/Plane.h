/*
This file is part of DXShell
(c) 2002-2026 by Mathias Heyer

DXShell is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

DXShell is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#ifndef PLANE_H_INCLUDED
#define PLANE_H_INCLUDED

#include <math.h>

/*////////////////////////////////////////////////////////////////////
Defines
////////////////////////////////////////////////////////////////////*/
#define ON_PLANE 0
#define FRONTSIDE 1
#define BACKSIDE 2
#define SPANNING 3  // geeignet, um mit den Bits zu arbeiten!

#define PLANE_X 0
#define PLANE_Y 1
#define PLANE_Z 2
#define PLANE_XYZ 3

/*#define NNN 0
#define NNP 1
#define NPN 2
#define NPP 3
#define PNN 4
#define PNP 5
#define PPN 6
#define PPP 7*/
/*////////////////////////////////////////////////////////////////////
Types
////////////////////////////////////////////////////////////////////*/
#include "Matrix4.h"
#include "Vector3.h"
#include "Vector4.h"

struct PLANE
{
    VECTOR3 n;  // Ebene, gegeben durch ax+by+cz=d
    float   d;

    inline PLANE() {};
    inline PLANE(const VECTOR3& Normal, const VECTOR3& Point = VECTOR3(0, 0, 0));
    inline PLANE(const VECTOR3& Normal, const float dist);
    inline PLANE(const VECTOR4& v);
    inline PLANE(const VECTOR3& v1, const VECTOR3& v2, const VECTOR3& v3);
    inline PLANE(float a, float b, float c, float d);

    inline bool  operator==(const PLANE& p2) const;
    inline bool  operator!=(const PLANE& p2) const;
    inline PLANE operator-() const;
};

struct FRUSTUMPLANE : public PLANE
{
    inline FRUSTUMPLANE() {};
    inline FRUSTUMPLANE(const PLANE& plane);
    inline FRUSTUMPLANE& operator=(const PLANE& plane);

    inline void setSignBits() { signbits = getVecDir(n); }

    unsigned int signbits;
};

extern std::ostream& operator<<(std::ostream& stream, const PLANE& plane);

/*/////////////////////////////////////////////////////////////////////////
 Inline-Funktionen
///////////////////////////////////////////////////////////////////////////////////////////////////////*/
inline int PointOnPlaneSide(const VECTOR3& V, const PLANE& P)
{
    float d = V * P.n - P.d;
    if (d > PLANE_EPSILON)
        return FRONTSIDE;
    if (d < -PLANE_EPSILON)
        return BACKSIDE;
    return ON_PLANE;
}

inline float PointDistance(const VECTOR3& V, const PLANE& P)
{
    return (V * P.n) - P.d;
}

inline VECTOR3 ProjectPointOnPlane(const VECTOR3& Point, const PLANE& Plane)
{
    return Point - Plane.n * ((Point * Plane.n) - Plane.d);
}

inline void NormalizePlane(PLANE& p)
{
    float l = Length(p.n);
    p.n /= l;
    p.d /= l;
}

inline PLANE::PLANE(const VECTOR3& Normal, const VECTOR3& Point)
{
    n = Normal;
    Normalize(n);
    d = (n * Point);
};

inline PLANE::PLANE(const VECTOR3& Normal, const float dist)
{
    n = Normal;
    d = dist;
    NormalizePlane(*this);
};

inline PLANE::PLANE(const VECTOR4& v)
{
    n.x = v.x;
    n.y = v.y;
    n.z = v.z;
    d   = v.w;
}

inline PLANE::PLANE(const VECTOR3& v1, const VECTOR3& v2, const VECTOR3& v3)
{
    VECTOR3 v12, v13;
    Sub(v2, v1, v12);
    Sub(v3, v1, v13);
    CrossProduct(v12, v13, n);
    Normalize(n);
    d = (n * v1);
}
inline PLANE::PLANE(float a, float b, float c, float pd)
{
    n.x = a;
    n.y = b;
    n.z = c;
    d   = pd;
    NormalizePlane(*this);
}
inline bool PLANE::operator==(const PLANE& p2) const
{
    if (fabs(d - p2.d) > PLANE_EPSILON)
        return false;
    if (fabs(n[0] - p2.n[0]) > NORMAL_EPSILON)
        return false;
    if (fabs(n[1] - p2.n[1]) > NORMAL_EPSILON)
        return false;
    if (fabs(n[2] - p2.n[2]) > NORMAL_EPSILON)
        return false;
    return true;
}
inline bool PLANE::operator!=(const PLANE& p2) const
{
    if (fabs(d - p2.d) > PLANE_EPSILON)
        return true;
    if (fabs(n[0] - p2.n[0]) > NORMAL_EPSILON)
        return true;
    if (fabs(n[1] - p2.n[1]) > NORMAL_EPSILON)
        return true;
    if (fabs(n[2] - p2.n[2]) > NORMAL_EPSILON)
        return true;

    return false;
}
inline PLANE PLANE::operator-() const
{
    PLANE p;
    p.n = -n;
    p.d = -d;
    return p;
}
inline PLANE operator*(const MATRIX4& M, const PLANE& plane)
{
    PLANE p(plane);
    p.d         = -p.d;  // ist notwendig, um auf die form ax+by+cz+d=0 zu kommen
    (VECTOR4&)p = M * (VECTOR4&)p;
    p.d         = -p.d;
    return p;
}
//=============================================================================

inline FRUSTUMPLANE::FRUSTUMPLANE(const PLANE& plane)
: PLANE(plane)
{
    signbits = getVecDir(n);
}

inline FRUSTUMPLANE& FRUSTUMPLANE::operator=(const PLANE& plane)
{
    PLANE::operator=(plane);
    signbits = getVecDir(n);
    return *this;
}

inline void SetPlaneType(FRUSTUMPLANE& Plane)
{
    Plane.signbits = 0;
    if (Plane.n.x > 0.0f)
        Plane.signbits = 4;
    if (Plane.n.y > 0.0f)
        Plane.signbits |= 2;
    if (Plane.n.z > 0.0f)
        Plane.signbits |= 1;
}

#endif