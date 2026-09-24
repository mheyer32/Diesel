
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
#ifndef AABB_H_INCLUDED
#define AABB_H_INCLUDED

#include <ostream>

#include "..\math\Matrix4.h"
#include "..\math\Plane.h"
#include "..\math\Vector3.h"

/*////////////////////////////////////////////////////////////////////
Types
////////////////////////////////////////////////////////////////////*/
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

struct BBOX
{
    VECTOR3 min, max;
    inline BBOX()
    : min(VECTOR3(999999, 999999, 999999))
    , max(VECTOR3(-999999, -999999, -999999)) {};
    inline BBOX(const VECTOR3& nmin, const VECTOR3& nmax)
    : min(nmin)
    , max(nmax) {};
    inline BBOX  operator|(const BBOX& bbox2) const;
    inline BBOX  operator|(const VECTOR3& v) const;
    inline BBOX& operator|=(const BBOX& bbox2);
    inline BBOX& operator|=(const VECTOR3& v);

    inline BBOX  operator*(float S) const;
    inline BBOX  operator/(float S) const;
    inline BBOX& operator*=(float S);
    inline BBOX& operator/=(float S);

    inline BBOX& operator+=(const VECTOR3& v);
    inline BBOX& operator-=(const VECTOR3& v);
};

//////////////////////////////////////////////////////////////////////////////////////////////
// Funktions-Prototypen
//////////////////////////////////////////////////////////////////////////////////////////////
extern void          DrawBBox(const BBOX& Box);
extern bool          checkBBox(const BBOX& Box);
extern std::ostream& operator<<(std::ostream& stream, const BBOX& box);

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Inline-Funktionen
/////////////////////////////////////////////////////////////////////////////////////////////////////////
inline BBOX BBOX::operator|(const BBOX& bbox2) const
{
    BBOX newbbox(bbox2);
    if (min.x < bbox2.min.x)
        newbbox.min.x = min.x;
    if (min.y < bbox2.min.y)
        newbbox.min.y = min.y;
    if (min.z < bbox2.min.z)
        newbbox.min.z = min.z;
    if (max.x > bbox2.max.x)
        newbbox.max.x = max.x;
    if (max.y > bbox2.max.y)
        newbbox.max.y = max.y;
    if (max.z > bbox2.max.z)
        newbbox.max.z = max.z;
    return newbbox;
}
inline BBOX& BBOX::operator|=(const BBOX& bbox2)
{
    if (min.x > bbox2.min.x)
        min.x = bbox2.min.x;
    if (min.y > bbox2.min.y)
        min.y = bbox2.min.y;
    if (min.z > bbox2.min.z)
        min.z = bbox2.min.z;
    if (max.x < bbox2.max.x)
        max.x = bbox2.max.x;
    if (max.y < bbox2.max.y)
        max.y = bbox2.max.y;
    if (max.z < bbox2.max.z)
        max.z = bbox2.max.z;
    return *this;
}
inline BBOX& BBOX::operator|=(const VECTOR3& v)
{
    if (v.x > max.x)
        max.x = v.x;
    if (v.y > max.y)
        max.y = v.y;
    if (v.z > max.z)
        max.z = v.z;
    if (v.x < min.x)
        min.x = v.x;
    if (v.y < min.y)
        min.y = v.y;
    if (v.z < min.z)
        min.z = v.z;
    return *this;
}
inline BBOX BBOX::operator|(const VECTOR3& v) const
{
    BBOX newbox(*this);
    if (v.x > newbox.max.x)
        newbox.max.x = v.x;
    if (v.y > newbox.max.y)
        newbox.max.y = v.y;
    if (v.z > newbox.max.z)
        newbox.max.z = v.z;
    if (v.x < newbox.min.x)
        newbox.min.x = v.x;
    if (v.y < newbox.min.y)
        newbox.min.y = v.y;
    if (v.z < newbox.min.z)
        newbox.min.z = v.z;
    return newbox;
}
inline BBOX BBOX::operator*(float S) const
{
    return BBOX(min * S, max * S);
}
inline BBOX BBOX::operator/(float S) const
{
    return BBOX(min / S, max / S);
}
inline BBOX& BBOX::operator*=(float S)
{
    min *= S;
    max *= S;
    return *this;
}
inline BBOX& BBOX::operator/=(float S)
{
    min /= S;
    max /= S;
    return *this;
}

inline BBOX& BBOX::operator+=(const VECTOR3& v)
{
    min += v;
    max += v;
    return *this;
}

inline BBOX& BBOX::operator-=(const VECTOR3& v)
{
    min -= v;
    max -= v;
    return *this;
}

inline float getWidth(const BBOX& bbox)
{
    return bbox.max.x - bbox.min.x;
}
inline float getHeight(const BBOX& bbox)
{
    return bbox.max.y - bbox.min.y;
}
inline float getDepth(const BBOX& bbox)
{
    return bbox.max.z - bbox.min.z;
}
inline VECTOR3 getCentre(const BBOX& bbox)
{
    return VECTOR3(bbox.min + bbox.max) * 0.5;
}

inline void resetBBox(BBOX& bbox)
{
    bbox.min = VECTOR3(999999, 999999, 999999);
    bbox.max = VECTOR3(-999999, -999999, -999999);
}
inline void getDimensions(const BBOX& bbox, VECTOR3& dim)
{
    dim = bbox.max - bbox.min;
}

inline float getBBoxRadius(const VECTOR3& dim, const VECTOR3& normal)
{
    return (fabsf(dim.x * normal.x) + fabsf(dim.y * normal.y) + fabsf(dim.z * normal.z)) * 0.5f;
}

inline float getBBoxRadius(const BBOX& box, const VECTOR3& normal)
{
    VECTOR3 dim = box.max - box.min;
    return getBBoxRadius(dim, normal);
}

inline bool BoundingBoxIntersect(const BBOX& bbox1, const BBOX& bbox2)
{
    if (bbox1.max.x < bbox2.min.x || bbox1.min.x > bbox2.max.x)
        return false;
    if (bbox1.max.y < bbox2.min.y || bbox1.min.y > bbox2.max.y)
        return false;
    if (bbox1.max.z < bbox2.min.z || bbox1.min.z > bbox2.max.z)
        return false;

    return true;
}

inline bool PointInBox(const VECTOR3& P, const BBOX& B)
{
    if (P.x < B.min.x || P.x > B.max.x || P.y < B.min.y || P.y > B.max.y || P.z < B.min.z || P.z > B.max.z)
        return false;
    return true;
}

inline void getCornerPoints(const BBOX& box, VECTOR3* v)
{
    v[NPP].x = v[NPN].x = v[NNP].x = v[NNN].x = box.min.x;
    v[PNP].y = v[PNN].y = v[NNP].y = v[NNN].y = box.min.y;
    v[PPN].z = v[PNN].z = v[NPN].z = v[NNN].z = box.min.z;

    v[PPP].x = v[PPN].x = v[PNP].x = v[PNN].x = box.max.x;
    v[PPP].y = v[PPN].y = v[NPP].y = v[NPN].y = box.max.y;
    v[PPP].z = v[PNP].z = v[NPP].z = v[NNP].z = box.max.z;
}

inline void getCornerPoints(const BBOX& box, VECTOR4* v)
{
    v[NPP].x = v[NPN].x = v[NNP].x = v[NNN].x = box.min.x;
    v[PNP].y = v[PNN].y = v[NNP].y = v[NNN].y = box.min.y;
    v[PPN].z = v[PNN].z = v[NPN].z = v[NNN].z = box.min.z;

    v[PPP].x = v[PPN].x = v[PNP].x = v[PNN].x = box.max.x;
    v[PPP].y = v[PPN].y = v[NPP].y = v[NPN].y = box.max.y;
    v[PPP].z = v[PNP].z = v[NPP].z = v[NNP].z = box.max.z;

    for (int i = 0; i < 8; ++i) {
        v[i].w = 1.0f;
    }
}

inline BBOX operator*(const MATRIX4& M, const BBOX& bbox)
{
    BBOX newbbox((VECTOR3&)M.a[3], (VECTOR3&)M.a[3]);

    const VECTOR3& min    = bbox.min;
    const VECTOR3& max    = bbox.max;
    VECTOR3&       newmin = newbbox.min;
    VECTOR3&       newmax = newbbox.max;
    float          a;
    float          b;
    for (int r = 0; r < 3; ++r) {
        for (int c = 0; c < 3; ++c) {
            a = M.a[c][r] * min[c];
            b = M.a[c][r] * max[c];
            if (a < b) {
                newmin[r] += a;
                newmax[r] += b;
            } else {
                newmin[r] += b;
                newmax[r] += a;
            }
        }
    }
    return newbbox;
}

inline bool BoxInFront(const BBOX& Box, const FRUSTUMPLANE& Plane)
{
    VECTOR3 v2;

    switch (Plane.signbits) {
    case NNN:
        return Box.max * Plane.n >= Plane.d;
    case PPP:
        return Box.min * Plane.n >= Plane.d;
    case NNP:
        v2.x = Box.max.x;
        v2.y = Box.max.y;
        v2.z = Box.min.z;
        return v2 * Plane.n >= Plane.d;
    case PPN:
        v2.x = Box.min.x;
        v2.y = Box.min.y;
        v2.z = Box.max.z;
        return (v2 * Plane.n >= Plane.d);
    case NPP:
        v2.x = Box.max.x;
        v2.y = Box.min.y;
        v2.z = Box.min.z;
        return (v2 * Plane.n >= Plane.d);
    case PNN:
        v2.x = Box.min.x;
        v2.y = Box.max.y;
        v2.z = Box.max.z;
        return (v2 * Plane.n >= Plane.d);
    case NPN:
        v2.x = Box.max.x;
        v2.y = Box.min.y;
        v2.z = Box.max.z;
        return (v2 * Plane.n >= Plane.d);
    case PNP:
        v2.x = Box.min.x;
        v2.y = Box.max.y;
        v2.z = Box.min.z;
        return (v2 * Plane.n >= Plane.d);
    }
    // FIXME throw exception here or calc signbits on the fly
    return false;
}

inline bool BoxBehind(const BBOX& Box, const FRUSTUMPLANE& Plane)
{
    VECTOR3 v2;

    switch (Plane.signbits) {
    case PPP:
        return (Box.max * Plane.n <= Plane.d);
    case NNN:
        return (Box.min * Plane.n <= Plane.d);
    case PPN:
        v2.x = Box.max.x;
        v2.y = Box.max.y;
        v2.z = Box.min.z;
        return (v2 * Plane.n <= Plane.d);
    case NNP:
        v2.x = Box.min.x;
        v2.y = Box.min.y;
        v2.z = Box.max.z;
        return (v2 * Plane.n <= Plane.d);
    case PNN:
        v2.x = Box.max.x;
        v2.y = Box.min.y;
        v2.z = Box.min.z;
        return (v2 * Plane.n <= Plane.d);
    case NPP:
        v2.x = Box.min.x;
        v2.y = Box.max.y;
        v2.z = Box.max.z;
        return (v2 * Plane.n <= Plane.d);
    case PNP:
        v2.x = Box.max.x;
        v2.y = Box.min.y;
        v2.z = Box.max.z;
        return (v2 * Plane.n <= Plane.d);
    case NPN:
        v2.x = Box.min.x;
        v2.y = Box.max.y;
        v2.z = Box.min.z;
        return (v2 * Plane.n <= Plane.d);
    }
    // FIXME throw exception here or calc signbits on the fly
    return false;
}

inline bool BoxBehind(const VECTOR3 boxcorners[8], const FRUSTUMPLANE& Plane)
{
    return (boxcorners[Plane.signbits] * Plane.n <= Plane.d);
}

inline int BoxOnPlaneSide(const BBOX& Box, const FRUSTUMPLANE& Plane)
{
    switch (Plane.signbits) {
    case PPP:
    case NNN:
        return PointOnPlaneSide(Box.min, Plane) | PointOnPlaneSide(Box.max, Plane);
    case PPN:
    case NNP: {
        VECTOR3 v1, v2;
        v1.x = Box.min.x;
        v1.y = Box.min.y;
        v1.z = Box.max.z;
        v2.x = Box.max.x;
        v2.y = Box.max.y;
        v2.z = Box.min.z;
        return PointOnPlaneSide(v1, Plane) | PointOnPlaneSide(v2, Plane);
    }
    case PNN:
    case NPP: {
        VECTOR3 v1, v2;
        v1.x = Box.max.x;
        v1.y = Box.min.y;
        v1.z = Box.min.z;
        v2.x = Box.min.x;
        v2.y = Box.max.y;
        v2.z = Box.max.z;
        return PointOnPlaneSide(v1, Plane) | PointOnPlaneSide(v2, Plane);
    }
    case PNP:
    case NPN: {
        VECTOR3 v1, v2;
        v1.x = Box.min.x;
        v1.y = Box.max.y;
        v1.z = Box.min.z;
        v2.x = Box.max.x;
        v2.y = Box.min.y;
        v2.z = Box.max.z;
        return PointOnPlaneSide(v1, Plane) | PointOnPlaneSide(v2, Plane);
    }
    }
    // FIXME throw exception here or calc signbits on the fly
    return 0;
}

inline int BoxOnPlaneSide(const VECTOR3 Box[8], const FRUSTUMPLANE& Plane)
{
    return PointOnPlaneSide(Box[Plane.signbits], Plane) | PointOnPlaneSide(Box[(~Plane.signbits) & 0x7], Plane);
}

inline float BoxDistance(const BBOX& Box, const FRUSTUMPLANE& Plane)
{
    VECTOR3 v2;

    switch (Plane.signbits) {
    case NNN:
        return PointDistance(Box.max, Plane);
    case PPP:
        return PointDistance(Box.min, Plane);
    case NNP:
        v2.x = Box.max.x;
        v2.y = Box.max.y;
        v2.z = Box.min.z;
        return PointDistance(v2, Plane);
    case PPN:
        v2.x = Box.min.x;
        v2.y = Box.min.y;
        v2.z = Box.max.z;
        return PointDistance(v2, Plane);
    case NPP:
        v2.x = Box.max.x;
        v2.y = Box.min.y;
        v2.z = Box.min.z;
        return PointDistance(v2, Plane);
    case PNN:
        v2.x = Box.min.x;
        v2.y = Box.max.y;
        v2.z = Box.max.z;
        return PointDistance(v2, Plane);
    case NPN:
        v2.x = Box.max.x;
        v2.y = Box.min.y;
        v2.z = Box.max.z;
        return PointDistance(v2, Plane);
    case PNP:
        v2.x = Box.min.x;
        v2.y = Box.max.y;
        v2.z = Box.min.z;
        return PointDistance(v2, Plane);
    }
    // FIXME throw exception here or calc signbits on the fly
    return 0;
}
#endif
