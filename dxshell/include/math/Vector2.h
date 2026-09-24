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
#ifndef VECTOR2_H_INCLUDED
#define VECTOR2_H_INCLUDED

#include <math.h>
#include <iostream>
#include "MathDefs.h"

/*////////////////////////////////////////////////////////////////////
Types
////////////////////////////////////////////////////////////////////*/
struct VECTOR3;

struct VECTOR2
{
    float x, y;
    inline VECTOR2() {};
    inline VECTOR2(float nx, float ny)
    : x(nx)
    , y(ny) {};
    inline VECTOR2(float* v)
    : x(v[0])
    , y(v[1]) {};
    inline VECTOR2  operator+(const VECTOR2& t2) const;
    inline VECTOR2  operator-(const VECTOR2& t2) const;
    inline VECTOR2  operator*(const float s) const;
    inline VECTOR2  operator/(const float s) const;
    inline VECTOR2  operator-() const;
    inline VECTOR2& operator+=(const VECTOR2& t2);
    inline VECTOR2& operator-=(const VECTOR2& t2);
    inline VECTOR2& operator*=(const float s);
    inline VECTOR2& operator/=(const float s);
    inline float    operator[](const unsigned int k) const;
    inline float&   operator[](const unsigned int k);
    inline float    operator*(const VECTOR2& t2) const;
    inline bool     operator==(const VECTOR2& t2) const;
    inline bool     operator!=(const VECTOR2& t2) const;

    inline VECTOR2& operator=(const VECTOR3& v2);
};

inline std::ostream& operator<<(std::ostream& stream, const VECTOR2& vec)
{
    stream << '(' << vec.x << ", " << vec.y << ')';
    return stream;
}

static const VECTOR2 NullVector2 = VECTOR2(0, 0);
/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Inline-Funktionen
/////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "Vector3.h"

inline void Normalize(VECTOR2& v)
{
    float l = 1.0f / (float)sqrtf(v.x * v.x + v.y * v.y);
    v *= l;
}

inline float Length(const VECTOR2& v)
{
    return sqrtf(v.x * v.x + v.y * v.y);
}
inline float Distance(const VECTOR2& v1, const VECTOR2& v2)
{
    VECTOR2 d = v2 - v1;
    return Length(d);
}
inline float DistanceSquared(const VECTOR2& v1, const VECTOR2& v2)
{
    VECTOR2 d = v2 - v1;
    return d * d;
}
inline VECTOR2 PerpendicularVector(const VECTOR2& v)
{
    return VECTOR2(-v.y, v.x);
}

inline VECTOR2& VECTOR2::operator=(const VECTOR3& v2)
{
    x = v2.x;
    y = v2.y;
    return *this;
}
#include "vector2.hh"

#endif