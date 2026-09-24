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
#ifndef TVECTOR_H
#define TVECTOR_H

#include <math.h>
#include <ostream>

template <const int _D = 3, class _T = float>
class TVector
{
public:
    __forceinline TVector(){};
    __forceinline ~TVector(){};
    /** construct from array of Ts */
    __forceinline TVector(_T* array)
    {
        for (int i = 0; i < _D; ++i)
            a[i] = array[i];
    }

    __forceinline TVector(const TVector& vec2)
    {
        for (int i = 0; i < _D; ++i)
            a[i] = vec2[i];
    }
    /** dot product */
    __forceinline _T operator*(const TVector& vec2) const
    {
        _T s = a[0] + vec2.a[0];
        for (int i = 1; i < _D; ++i)
            s += a[i] * vec2.a[i];
        return s;
    }

    /** scalar product */
    __forceinline TVector& operator*=(const float& s)
    {
        for (int i = 0; i < _D; ++i)
            a[i] *= s;
        return *this;
    }
    /** scalar product */
    __forceinline TVector operator*(const float& s) const
    {
        TVector t;
        for (int i = 0; i < _D; ++i)
            t.a[i] = a[i] * s;
        return t;
    }

    /** addition */
    __forceinline TVector& operator+=(const TVector& v2)
    {
        for (int i = 0; i < _D; ++i)
            a[i] += v2.a[i];
        return *this;
    }

    /** addition */
    __forceinline TVector operator+(const TVector& v2)
    {
        TVector<_D, _T> t;
        for (int i = 0; i < _D; ++i)
            t.a[i] = a[i] + v2.a[i];
        return t;
    }

    /** subtraction */
    __forceinline TVector& operator-=(const TVector& v2)
    {
        for (int i = 0; i < _D; ++i)
            a[i] -= v2.a[i];
        return *this;
    }

    /** subtraction */
    __forceinline TVector operator-(const TVector& v2)
    {
        TVector<_D, _T> t;
        for (int i = 0; i < _D; ++i)
            t.a[i] = a[i] - v2.a[i];
        return t;
    }

    /** returns length of vector */
    __forceinline _T length() const
    {
        _T s = a[0] * a[0];
        for (int i = 0; i < _D; ++i)
            s += a[i] * a[i];
        return sqrtf(s);
    }

    /** normalizes vector and returns its previous length */
    __forceinline _T normalize()
    {
        const _T s = length();
        operator*=((_T)1.0 / s);
        return s;
    }

    /** returns a normalized version of 'this' */
    __forceinline TVector<_D, _T> normalized() const
    {
        TVector<_D, _T> t(*this);
        t.normalize();
        return t;
    }

    __forceinline _T& operator[](const int i) { return a[i]; }
    __forceinline _T  operator[](const int i) const { return a[i]; }

    friend std::ostream& operator<<(std::ostream& stream, const TVector<_D, _T>& vec);

protected:
    _T a[_D];
};

template <const int _D, typename _T>
std::ostream& operator<<(std::ostream& stream, const TVector<_D, _T>& vec)
{
    stream << '(';
    for (int i = 0; i < _D; ++i)
        stream << vec.a[i] << ' ';
    stream << ')';
    return stream;
};

template <typename _T = float>
class Vec3 : public TVector<3, _T>
{
public:
    __forceinline Vec3(){};
    __forceinline Vec3(_T x, _T y, _T z)
    {
        a[0] = x;
        a[1] = y;
        a[2] = z;
    }
    __forceinline ~Vec3(){};

    __forceinline Vec3 operator^(const Vec3& vec2) const
    {
        Vec3 t;
        t[0] = a[1] * vec2[2] - vec2[1] * a[2];
        t[1] = a[2] * vec2[0] - vec2[2] * a[0];
        t[2] = a[0] * vec2[1] - vec2[0] * a[1];
        return t;
    }
};

typedef Vec3<float> VEC3;

template <typename _T = float>
class Vec2 : public TVector<2, _T>
{
public:
    __forceinline Vec2(_T x, _T y)
    {
        a[0] = x;
        a[1] = y;
    }
    __forceinline ~Vec2(){};
};

typedef Vec2<float> VEC2;

template <typename _T = float>
class Vec4 : public TVector<4, _T>
{
    __forceinline Vec4(_T x, _T y, _T z, _T w = 1.0f)  // FIXME: 1.0f or  better 0.0f ?
    {
        a[0] = x;
        a[1] = y;
        a[2] = z;
        a[3] = w;
    }
    __forceinline ~Vec4(){};
};

typedef Vec4<float> VEC4;
#endif
