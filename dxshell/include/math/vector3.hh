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
inline VECTOR3 VECTOR3::operator + (const VECTOR3 &v2) const
{
	return VECTOR3(x+v2.x,y+v2.y,z+v2.z);
}
inline VECTOR3 VECTOR3::operator - (const VECTOR3 &v2) const
{
	return VECTOR3(x-v2.x,y-v2.y,z-v2.z);
}
inline VECTOR3 VECTOR3::operator * (const float s) const
{
	return VECTOR3(x*s,y*s,z*s);
}

inline VECTOR3 VECTOR3::operator / (float s) const
{
	s=1.0f/s;
	return VECTOR3(x*s,y*s,z*s);
}
inline VECTOR3& VECTOR3::operator /= (float s)
{
	s=1.0f/s;
	x*=s;y*=s;z*=s;
	return *this;
}

inline VECTOR3& VECTOR3::operator += (const VECTOR3 &v2)
{	x+=v2.x;y+=v2.y;z+=v2.z;
	return *this;
}
inline VECTOR3& VECTOR3::operator -= (const VECTOR3 &v2)
{
	x-=v2.x;y-=v2.y;z-=v2.z;
	return *this;
}
inline VECTOR3& VECTOR3::operator *= (const float s)
{
	x*=s;y*=s;z*=s;
	return *this;
}
inline VECTOR3	VECTOR3::operator ^ (const VECTOR3 &v2) const
{
	VECTOR3 n;
	n.x=(y*v2.z)-(v2.y*z);
	n.y=(z*v2.x)-(v2.z*x);
	n.z=(x*v2.y)-(v2.x*y);
	return n;
}
inline VECTOR3 VECTOR3::operator -() const
{
	return VECTOR3(-x,-y,-z);
}
inline float VECTOR3::operator * (const VECTOR3 &v) const
{
	return x*v.x+y*v.y+z*v.z;
}
inline VECTOR3& VECTOR3::operator = (const VECTOR4 &v)
{
	x=v.x;y=v.y;z=v.z;
	return *this;
}

inline bool VECTOR3::operator == (const VECTOR3 &v2) const
{
//	if ((x==v2.x)&&(y==v2.y)&&(z==v2.z)) return TRUE;
	if ((fabs(x-v2.x)<=POINT_EPSILON)&&(fabs(y-v2.y)<=POINT_EPSILON)&&(fabs(z-v2.z)<=POINT_EPSILON)) return true;
	return false;
}
inline bool VECTOR3::operator != (const VECTOR3 &v2) const
{
//	if ((x!=v2.x)||(y==v2.y)||(z==v2.z)) return TRUE;
	if ((fabs(x-v2.x)>POINT_EPSILON)||(fabs(y-v2.y)>POINT_EPSILON)||(fabs(z-v2.z)>POINT_EPSILON)) return true;
	return false;
}
inline float VECTOR3::operator [] (const unsigned int k) const
{
	return ((float*)this)[k];
}
inline float& VECTOR3::operator [](const unsigned int k)
{
	return ((float*)this)[k];
}
