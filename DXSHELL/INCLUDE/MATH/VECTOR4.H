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
#ifndef VECTOR4_H_INCLUDED
#define VECTOR4_H_INCLUDED

#include "mathdefs.h"
#include <math.h>
#include <iostream>

/*////////////////////////////////////////////////////////////////////
Types
////////////////////////////////////////////////////////////////////*/

struct VECTOR3;
struct COLOR;

struct VECTOR4
{ 
	float x,y,z,w;
	inline VECTOR4(){};
	inline VECTOR4(float nx,float ny,float nz,float nw=1.0f):x(nx),y(ny),z(nz),w(nw){};
	inline VECTOR4(const float *v):x(v[0]),y(v[1]),z(v[2]),w(v[3]){};
	inline VECTOR4(const VECTOR3 &v, float w=1.0f); 
	inline VECTOR4 operator + (const VECTOR4 &v2) const;
	inline VECTOR4 operator - (const VECTOR4 &v2) const;
	inline VECTOR4 operator - () const;
	inline VECTOR4 operator * (const float s) const;
	inline VECTOR4 operator / (const float s) const;
	inline VECTOR4& operator += (const VECTOR4 &v2);
	inline VECTOR4& operator -= (const VECTOR4 &v2);
	inline VECTOR4& operator *= (const float s);
	inline VECTOR4& operator /= (const float s);
	inline VECTOR4 operator ^ (const VECTOR4 &v2) const;
	inline VECTOR4& operator = (const VECTOR3 &v2);

	inline VECTOR4& operator=(const COLOR &col);

	inline bool operator == (const VECTOR4 &v2) const;
	inline bool operator != (const VECTOR4 &v2) const;
	float operator * (const VECTOR4 &v) const;
	inline float operator [] (const unsigned int k) const;
	inline float& operator [](const unsigned int k);

};

static const VECTOR4 NullVector4(0,0,0,0);
/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Inline-Funktionen
/////////////////////////////////////////////////////////////////////////////////////////////////////////

inline std::ostream& operator << (std::ostream& stream, const VECTOR4 &vec)
{
	stream<<'('<<vec.x<<", "<<vec.y<<", "<<vec.z<<", "<<vec.w<<')';
	return stream;
}

inline void Normalize (VECTOR4 &v)
{
	v/=(float)sqrtf(v.x*v.x+v.y*v.y+v.z*v.z);
}

inline float Length(const VECTOR4& v)
{
	return sqrtf(v.x*v.x+v.y*v.y+v.z*v.z);
}

#include "Vector3.h"
#include "Vector4.hh"

#endif
