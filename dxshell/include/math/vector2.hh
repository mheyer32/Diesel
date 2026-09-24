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
inline VECTOR2 VECTOR2::operator + (const VECTOR2 &v2) const
{
	return VECTOR2(x+v2.x,y+v2.y);
}
inline VECTOR2 VECTOR2::operator - (const VECTOR2 &v2) const
{
	return VECTOR2(x-v2.x,y-v2.y);
}
inline VECTOR2 VECTOR2::operator * (const float s) const
{
	return VECTOR2(x*s,y*s);
}

inline VECTOR2 VECTOR2::operator / (const float s) const 
{
	return VECTOR2(x/s,y/s);
}
inline VECTOR2& VECTOR2::operator += (const VECTOR2 &v2)
{	x+=v2.x;y+=v2.y;
	return *this;
}
inline VECTOR2& VECTOR2::operator -= (const VECTOR2 &v2)
{
	x-=v2.x;y-=v2.y;
	return *this;
}
inline VECTOR2& VECTOR2::operator *= (const float s)
{
	x*=s;y*=s;
	return *this;
}
inline VECTOR2& VECTOR2::operator /= (const float s)
{
	x/=s;y/=s;
	return *this;
}
inline VECTOR2 VECTOR2::operator -() const
{
	return VECTOR2(-x,-y);
}
inline float VECTOR2::operator * (const VECTOR2 &v2) const
{
	return x*v2.x+y*v2.y;
}
inline bool VECTOR2::operator == (const VECTOR2 &v2) const
{
//	if ((x==v2.x)&&(y==v2.y)) return true;
	if ((fabs(x-v2.x)<POINT_EPSILON)&&(fabs(y-v2.y)<POINT_EPSILON)) return true;

	return false;
}
inline bool VECTOR2::operator != (const VECTOR2 &v2) const
{
	//if ((x!=v2.x)||(y!=v2.y)) return true;
	if ((fabs(x-v2.x)>POINT_EPSILON)||(fabs(y-v2.y)>POINT_EPSILON)) return true;

	return false;
}

inline float VECTOR2::operator [] (const unsigned int k) const
{
	return ((float*)this)[k];
}
inline float& VECTOR2::operator [](const unsigned int k)
{
	return ((float*)this)[k];
}