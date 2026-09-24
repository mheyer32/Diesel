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
#ifndef MATHLIB_H_INCLUDED
#define MATHLIB_H_INCLUDED

#include <cmath>

#include "Matrix4.h"
#include "Plane.h"
#include "Vector3.h"

#define VECDIR_X 0
#define VECDIR_Y 1
#define VECDIR_Z 2

/*////////////////////////////////////////////////////////////////////
Types
////////////////////////////////////////////////////////////////////*/

//////////////////////////////////////////////////////////////////////////////////////////////
// Funktions-Prototypen
//////////////////////////////////////////////////////////////////////////////////////////////
extern VECTOR3 PerpendicularVector(const VECTOR3& V);

extern void MatrixMultiply4(const float* m1, const float* m2, float* result);
extern void MatrixMultiply3(const float* m1, const float* m2, float* result);
extern void MatrixMultiply34(const float* m1, const float* m2, float* result);

extern void ConstructMatrix4X(const VECTOR3& xaxis, MATRIX4& M);
extern void ConstructMatrix4Z(const VECTOR3& zaxis, MATRIX4& M);

extern void FullInverse4(const MATRIX4& M, MATRIX4& Inv);
extern void FullInverse3(const MATRIX3& M, MATRIX3& Inv);

/*////////////////////////////////////////////////////////////////////////////////////////////////////////
Inline-Funktionen
///////////////////////////////////////////////////////////////////////////////////////////////////////*/

inline void SinCos(float in_angle, float& out_sin, float& out_cos)
{
    out_sin = sinf(in_angle);
    out_cos = cosf(in_angle);
}

inline float InvSqrt(float x)
{
    float xhalf = 0.5f * x;
    int   i     = *(int*)&x;
    i           = 0x5f3759df - (i >> 1);
    x           = *(float*)&i;
    x           = x * (1.5f - xhalf * x * x);
    return x;
}

// lat and lng obey Quakes lat/lng system
inline void getVecFromLatLong(float lat, float lng, VECTOR3& vec)
{
    float sinlng, coslng;
    float sinlat, coslat;
    SinCos(lng, sinlng, coslng);
    SinCos(lat, sinlat, coslat);
    vec.x = coslat * sinlng;
    vec.y = coslng;
    vec.z = -sinlat * sinlng;
}

inline void getLatLongFromVec(const VECTOR3& vec, float& lat, float& lng)
{
    lng = acosf(vec.y);
    lat = atan2f(-vec.z, vec.x);
}

inline bool checkColinear(const VECTOR3& p1, const VECTOR3& p2, const VECTOR3& p3)
{
    VECTOR3 d1(p2 - p1);
    VECTOR3 d2(p3 - p2);
    Normalize(d1);
    Normalize(d2);
    if (d1 == d2)
        return true;
    return false;
}

inline bool isPowerOfTwo(int x, int& lower, int& upper)
{
    if (!(x & (x - 1)))
        return true;
    int bits = 0;
    while (x) {
        x >>= 1;
        bits++;
    };

    upper = 1 << bits;
    lower = 1 << (bits - 1);

    return false;
}

inline bool isPowerOfTwo(int x)
{
    if (!(x & (x - 1)))
        return true;
    return false;
}

inline int getVectorDirection(const VECTOR3& vec)  // could be a template istead...
{
    if (vec.x > vec.y && vec.x > vec.z) {
        return VECDIR_X;
    } else if (vec.y > vec.x && vec.y > vec.z) {
        return VECDIR_Y;
    };

    return VECDIR_Z;
}

template <typename _V>
inline _V mirrorDirection(const PLANE& P, const _V& dir)
{
    return dir - P.n * ((P.n * dir) * 2.0f);
}
template <typename _V>
inline _V mirrorPoint(const PLANE& P, const _V& point)
{
    return point - P.n * (PointDistance(point, P) * 2.0f);
}

#endif