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

#include <cassert>
#include <cmath>
#include <ostream>

__inline void sincos3(float in_angle, float &out_sin, float &out_cos)
{
    out_sin = sinf(in_angle);
    out_cos = cosf(in_angle);
} 

inline MATRIX3::MATRIX3()
{
	m[8]=m[4]=m[0]=1.0f;
	m[7]=m[6]=m[5]=m[3]=m[2]=m[1]=0.0f;
}
inline MATRIX3::MATRIX3(float* m2)
{
	*this=*(MATRIX3*)m2;
}
inline float MATRIX3::operator [] (const int aik) const
{
	return m[aik];
}
inline float& MATRIX3::operator [] (const int aik)
{
	return m[aik];
}
inline MATRIX3& MATRIX3::operator = (const MATRIX4 &m2)
{
	m[0]=m2.m[0];
	m[1]=m2.m[1];
	m[2]=m2.m[2];

	m[3]=m2.m[4];
	m[4]=m2.m[5];
	m[5]=m2.m[6];

	m[6]=m2.m[8];
	m[7]=m2.m[9];
	m[8]=m2.m[10];
	return *this;
}
inline MATRIX3::MATRIX3(const MATRIX4 &m2)
{
	*this=m2;
}
inline MATRIX3 MATRIX3::operator * (const MATRIX3 &m2) const
{
MATRIX3 result;
   float ma0,ma1,ma2;
   int i;

    for (i = 0; i < 9; i+=3)
	{
        ma0 = m2.m[i];  ma1 = m2.m[i+1];
	    ma2 = m2.m[i+2];  
        result.m[i] = ma0 * m[0] + ma1 * m[3] + ma2 * m[6];
        result.m[i+1] = ma0 * m[1] + ma1 * m[4] + ma2 * m[7];
        result.m[i+2] = ma0 * m[2] + ma1 * m[5] + ma2 * m[8];
    }
	return result;
}
inline VECTOR3 MATRIX3::operator * (const VECTOR3 &v2) const
{
//	return (VECTOR3&)a[0]*v2.x+(VECTOR3&)a[1]*v2.y+(VECTOR3&)a[2]*v2.z;
	VECTOR3 temp;
	temp.x=m[0]*v2.x+m[3]*v2.y+m[6]*v2.z;
	temp.y=m[1]*v2.x+m[4]*v2.y+m[7]*v2.z;
	temp.z=m[2]*v2.x+m[5]*v2.y+m[8]*v2.z;
	return temp;
}
inline VECTOR2 MATRIX3::operator * (const VECTOR2 &v2) const
{
	//return (VECTOR2&)a[0]*v2.x+(VECTOR2&)a[1]*v2.y+(VECTOR2&)a[2];
	VECTOR2 temp;
	temp.x=m[0]*v2.x+m[3]*v2.y+m[6];
	temp.y=m[1]*v2.x+m[4]*v2.y+m[7];
	return temp;
}

inline void ScaleMatrix3(MATRIX3 &m, const VECTOR3 &s)
{	
	m.m[7]=m.m[6]=m.m[5]=m.m[3]=m.m[2]=m.m[1]=0.0f;
	m.m[0]=s.x;
	m.m[4]=s.y;
	m.m[8]=s.z;
}

inline void Scale3(MATRIX3 &m,const VECTOR3 &s)
{
	m.m[0]*=s.x;
	m.m[1]*=s.x;
	m.m[2]*=s.x;
	m.m[3]*=s.y;
	m.m[4]*=s.y;
	m.m[5]*=s.y;
	m.m[6]*=s.z;
	m.m[7]*=s.z;
	m.m[8]*=s.z;
}
inline void RotationMatrix3X(MATRIX3& m,const float angle) // Angle in Radians
{
	float sinus,cosinus;
	sincos3(angle,sinus,cosinus);
	
	m.m[0]=1.0f;
	m.m[6]=m.m[3]=m.m[2]=m.m[1]=0.0f;

	m.m[4]=cosinus;
	m.m[5]=sinus;
	m.m[7]=-sinus;
	m.m[8]=cosinus;
	
}
inline void RotationMatrix3Y(MATRIX3& m,const float angle) // Angle in Radians
{
	float sinus,cosinus;
	sincos3(angle,sinus,cosinus);

	m.m[7]=m.m[5]=m.m[3]=m.m[1]=0.0f;
	m.m[4]=1.0f;

	m.m[0]=cosinus;
	m.m[2]=-sinus;
	m.m[6]=sinus;
	m.m[8]=cosinus;
}
inline void RotationMatrix3Z(MATRIX3 &m,const float angle) // Angle in Radians
{
	float sinus,cosinus;
	sincos3(angle,sinus,cosinus);

	m.m[7]=m.m[6]=m.m[5]=m.m[2]=0.0f;
	m.m[8]=1.0f;

	m.m[0]=cosinus;
	m.m[1]=sinus;
	m.m[3]=-sinus;
	m.m[4]=cosinus;
}

inline void Rotate3Z(MATRIX3 &m,const float angle)
{
	float sinus,cosinus;
	sincos3(angle,sinus,cosinus);
	float t1=m.m[0];
	float t2=m.m[3];
	m.m[0]=t1*cosinus+t2*sinus;
	m.m[3]=t2*cosinus-t1*sinus;
	t1=m.m[1];
	t2=m.m[4];
	m.m[1]=t1*cosinus+t2*sinus;
	m.m[4]=t2*cosinus-t1*sinus;
	t1=m.m[2];
	t2=m.m[5];
	m.m[2]=t1*cosinus+t2*sinus;
	m.m[5]=t2*cosinus-t1*sinus;
}
inline void Rotate3Y(MATRIX3 &m,const float angle)
{
	float sinus,cosinus;
	sincos3(angle,sinus,cosinus);
	float t1=m.m[0];
	float t2=m.m[6];
	m.m[0]=t1*cosinus-t2*sinus;
	m.m[6]=t2*cosinus+t1*sinus;
	t1=m.m[1];
	t2=m.m[7];
	m.m[1]=t1*cosinus-t2*sinus;
	m.m[7]=t2*cosinus+t1*sinus;
	t1=m.m[2];
	t2=m.m[8];
	m.m[2]=t1*cosinus-t2*sinus;
	m.m[8]=t2*cosinus+t1*sinus;
}

inline void Rotate3X(MATRIX3 &m,const float angle)
{
	float sinus,cosinus;
	sincos3(angle,sinus,cosinus);
	float t1=m.m[3];
	float t2=m.m[6];
	m.m[3]=t1*cosinus+t2*sinus;
	m.m[6]=t2*cosinus-t1*sinus;
	t1=m.m[4];
	t2=m.m[7];
	m.m[4]=t1*cosinus+t2*sinus;
	m.m[7]=t2*cosinus-t1*sinus;
	t1=m.m[5];
	t2=m.m[8];
	m.m[5]=t1*cosinus+t2*sinus;
	m.m[8]=t2*cosinus-t1*sinus;
}

inline void TranslationMatrix3(MATRIX3 &m,const VECTOR2 &t)
{
	m.m[8]=m.m[4]=m.m[0]=1.0f;
	m.m[5]=m.m[3]=m.m[2]=m.m[1]=0.0f;

	m.m[6]=t.x;
	m.m[7]=t.y;
}
inline void Translate3(MATRIX3 &m,const VECTOR2 &t)
{
	m.m[6]+=m.m[0]*t.x+m.m[3]*t.y;	
	m.m[7]+=m.m[1]*t.x+m.m[4]*t.y;
	m.m[8]+=m.m[2]*t.x+m.m[5]*t.y;
}

inline void Transpose3(MATRIX3 &m)
{
	float temp;
	temp=m.m[1];
	m.m[1]=m.m[3];
	m.m[3]=temp;
	temp=m.m[2];
	m.m[2]=m.m[6];
	m.m[6]=temp;
	temp=m.m[5];
	m.m[5]=m.m[7];
	m.m[7]=temp;
}

inline void AngleAxisMatrix3(MATRIX3 &m,const VECTOR3 &axis,const float angle)
{
	float c,s;
	sincos3(angle,s,c);
	const float c2=1.0f-c;
	float temp;

	m.m[0]=c+c2*axis.x*axis.x;
	m.m[1]=m.m[3]=c2*axis.x*axis.y;
	temp=axis.z*s;
	m.m[1]+=temp;
	m.m[3]-=temp;
	m.m[2]=m.m[6]=c2*axis.x*axis.z;
	temp=axis.y*s;
	m.m[2]-=temp;
	m.m[6]+=temp;
	m.m[4]=c+c2*axis.y*axis.y;
	m.m[5]=m.m[7]=c2*axis.y*axis.z;
	temp=axis.x*s;
	m.m[5]+=temp;
	m.m[7]-=temp;
	m.m[8]=c+c2*axis.z*axis.z;
}

/** angles.x=pitch, angles.y=yaw , angles.z = roll*/
inline void	YawPitchRollMatrix3(const VECTOR3 &angles, MATRIX3 &matrix)
{
	// Reihenfolge der Rotationen : Z - X - Y  M*v= Ry*Rx*Rz
	float sx,cx;
	sincos3(angles.x,sx,cx);
	float sy,cy;
	sincos3(angles.y,sy,cy);
	float sz,cz;
	sincos3(angles.z,sz,cz);

	matrix.m[6]=cx*sy;
	matrix.m[7]=-sx;
	matrix.m[8]=cx*cy;
	matrix.m[3]=-sz*cy+cz*sx*sy;
	matrix.m[4]=cz*cx;
	matrix.m[5]=+sz*sy+cz*sx*cy;

	matrix.m[0]=matrix.m[4]*matrix.m[8]-matrix.m[7]*matrix.m[5];
	matrix.m[1]=matrix.m[5]*matrix.m[6]-matrix.m[8]*matrix.m[3];
	matrix.m[2]=matrix.m[3]*matrix.m[7]-matrix.m[6]*matrix.m[4];
}

inline std::ostream& operator << (std::ostream &stream, const MATRIX3& M)
{
	stream<<'('<<M.m00<<' '<<M.m01<<' '<<M.m02<<')'<<std::endl;
	stream<<'('<<M.m10<<' '<<M.m11<<' '<<M.m12<<')'<<std::endl;
	stream<<'('<<M.m20<<' '<<M.m21<<' '<<M.m22<<')'<<std::endl;
	return stream;
}