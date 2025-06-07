/*
This file is part of DXShell
(c) 2002 by Mathias Heyer

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
#ifndef VECTOR3_H_INCLUDED
#define VECTOR3_H_INCLUDED

#include <math.h>
#include <iostream>

#include "mathdefs.h"

/*////////////////////////////////////////////////////////////////////
Types
////////////////////////////////////////////////////////////////////*/
struct VECTOR4;
struct COLOR3;  // fowarddeclarations THAT forward?

enum VECDIR
{
    NNN,
    NNP,
    NPN,
    NPP,
    PNN,
    PNP,
    PPN,
    PPP,
    UNDEFINED_VECDIR
};

struct VECTOR3
{
    float x;
    float y;
    float z;

    inline VECTOR3() {};
    inline VECTOR3(float nx, float ny, float nz)
    : x(nx)
    , y(ny)
    , z(nz) {};
    inline VECTOR3(const float* v)
    : x(v[0])
    , y(v[1])
    , z(v[2]) {};
    inline VECTOR3  operator+(const VECTOR3& v2) const;
    inline VECTOR3  operator-(const VECTOR3& v2) const;
    inline VECTOR3  operator-() const;
    inline VECTOR3  operator*(const float s) const;
    inline VECTOR3  operator/(const float s) const;
    inline VECTOR3& operator+=(const VECTOR3& v2);
    inline VECTOR3& operator-=(const VECTOR3& v2);
    inline VECTOR3& operator*=(const float s);
    inline VECTOR3& operator/=(const float s);
    inline VECTOR3  operator^(const VECTOR3& v2) const;
    inline VECTOR3& operator=(const VECTOR4& v);
    inline VECTOR3& operator=(const COLOR3& col);  // this is just a prototype
    inline float    operator*(const VECTOR3& v) const;
    inline bool     operator==(const VECTOR3& v2) const;
    inline bool     operator!=(const VECTOR3& v2) const;
    inline float    operator[](const unsigned int k) const;
    inline float&   operator[](const unsigned int k);
};
inline std::ostream& operator<<(std::ostream& stream, const VECTOR3& vec)
{
    stream << '(' << vec.x << ", " << vec.y << ", " << vec.z << ')';
    return stream;
}
static const VECTOR3 NullVector3  = VECTOR3(0, 0, 0);
static const VECTOR3 UnitVector3X = VECTOR3(1.0f, 0, 0);
static const VECTOR3 UnitVector3Y = VECTOR3(0, 1.0f, 0);
static const VECTOR3 UnitVector3Z = VECTOR3(0, 0, 1.0f);
/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Inline-Funktionen
/////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void Normalize(VECTOR3& v)
{
    float l;
    l = 1.0f / (float)sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
    v.x *= l;
    v.y *= l;
    v.z *= l;
}

inline void Normalize(const VECTOR3& v, VECTOR3& result)
{
    float l;
    l        = 1.0f / (float)sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
    result.x = v.x * l;
    result.y = v.y * l;
    result.z = v.z * l;
}

inline float Distance(const VECTOR3& v1, const VECTOR3& v2)
{
    VECTOR3 dist = v2 - v1;
    return sqrtf(dist.x * dist.x + dist.y * dist.y + dist.z * dist.z);
}
inline float Length(const VECTOR3& v)
{
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

inline float SquaredLength(const VECTOR3& v)
{
    return v.x * v.x + v.y * v.y + v.z * v.z;
}

// some functions if its getting time-critical and temporaries must be avoided
inline void Add(const VECTOR3& v1, const VECTOR3& v2, VECTOR3& result)
{
    result.x = v1.x + v2.x;
    result.y = v1.y + v2.y;
    result.z = v1.z + v2.z;
}

inline void Sub(const VECTOR3& v1, const VECTOR3& v2, VECTOR3& result)
{
    result.x = v1.x - v2.x;
    result.y = v1.y - v2.y;
    result.z = v1.z - v2.z;
}

inline void Mul(const VECTOR3& v1, const float s, VECTOR3& result)
{
    result.x = v1.x * s;
    result.y = v1.y * s;
    result.z = v1.z * s;
}

inline float DotProduct(const VECTOR3& v1, const VECTOR3& v2)
{
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

// compute result = v1*s + v2
inline void MultiplyAdd(const VECTOR3& v1, const float s, const VECTOR3& v2, VECTOR3& result)
{
    result.x = v1.x * s + v2.x;
    result.y = v1.y * s + v2.y;
    result.z = v1.z * s + v2.z;
}

inline void CrossProduct(const VECTOR3& v1, const VECTOR3& v2, VECTOR3& result)
{
    result.x = v1.y * v2.z - v2.y * v1.z;
    result.y = v1.z * v2.x - v2.z * v1.x;
    result.z = v1.x * v2.y - v2.x * v1.y;
}

inline VECDIR getVecDir(const VECTOR3& vec)
{
    int dir = (vec.x > 0.0f) ? 4 : 0;
    dir |= (vec.y > 0.0) << 1;
    dir |= (vec.z > 0.0);
    return static_cast<VECDIR>(dir);
}

#include "Vector4.h"

#include "Vector3.hh"

#endif