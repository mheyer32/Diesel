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
#ifndef COLOR_H
#define COLOR_H

#include <math/Vector3.h>
#include <math/Vector4.h>

struct COLOR
{
    unsigned char r, g, b, alpha;
    inline COLOR() {};
    inline COLOR(unsigned char nr, unsigned char ng, unsigned char nb, unsigned char nalpha = 255)
    : r(nr)
    , g(ng)
    , b(nb)
    , alpha(nalpha) {};
    inline COLOR(unsigned char* c) { *(__int32*)this = *(__int32*)c; };
    inline COLOR(float* c)
    : r((unsigned char)c[0])
    , g((unsigned char)c[1])
    , b((unsigned char)c[2])
    , alpha((unsigned char)c[3]) {};
    inline COLOR(const VECTOR3& v)
    : r((unsigned char)v.x)
    , g((unsigned char)v.y)
    , b((unsigned char)v.z) {};
    inline COLOR(const VECTOR4& v)
    : r((unsigned char)v.x)
    , g((unsigned char)v.y)
    , b((unsigned char)v.z)
    , alpha((unsigned char)v.w) {};
    inline COLOR(const COLOR& col2) { *(int*)this = *(int*)&col2; };

    inline COLOR& operator=(const COLOR& col2)
    {
        *(int*)this = *(int*)&col2;
        return *this;
    }
    inline COLOR& operator=(const VECTOR4& v)
    {
        r     = (unsigned char)v.x;
        g     = (unsigned char)v.y;
        b     = (unsigned char)v.z;
        alpha = (unsigned char)v.w;
        return *this;
    }

    inline COLOR& operator=(const VECTOR3& v)
    {
        r = (unsigned char)v.x;
        g = (unsigned char)v.y;
        b = (unsigned char)v.z;
        return *this;
    }

    // FIXME: ist das so gewollt? Keine Umrechnung in 0.0-1.0
    inline                operator VECTOR3() const { return VECTOR3((float)r, (float)g, (float)b); };
    inline unsigned char  operator[](const int i) const { return ((unsigned char*)this)[i]; };
    inline unsigned char& operator[](const int i) { return ((unsigned char*)this)[i]; };
};

inline VECTOR4& VECTOR4::operator=(const COLOR& col)
{
    x = col.r;
    y = col.g;
    z = col.b;
    w = col.alpha;
    return *this;
}

struct COLOR3
{
    unsigned char r, g, b;
    inline COLOR3() {};
    inline COLOR3(unsigned char nr, unsigned char ng, unsigned char nb)
    : r(nr)
    , g(ng)
    , b(nb) {};
    inline COLOR3(unsigned char* c)
    : r(c[0])
    , g(c[1])
    , b(c[2]) {};
    inline COLOR3(float* c)
    : r((unsigned char)c[0])
    , g((unsigned char)c[1])
    , b((unsigned char)c[2]) {};
    inline COLOR3(const VECTOR3& v)
    : r((unsigned char)v.x)
    , g((unsigned char)v.y)
    , b((unsigned char)v.z) {};

    inline COLOR3& operator=(const VECTOR3& v)
    {
        r = (unsigned char)v.x;
        g = (unsigned char)v.y;
        b = (unsigned char)v.z;
        return *this;
    }

    inline operator VECTOR3() const { return VECTOR3((float)r, (float)g, (float)b); };
};

inline VECTOR3& VECTOR3::operator=(const COLOR3& col)
{
    x = col.r;
    y = col.g;
    z = col.b;
    return *this;
}

#endif