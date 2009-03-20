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
#include <ostream>

__inline void sincos34(float in_angle, float &out_sin, float &out_cos) 
{ 
	__asm { 
			fld in_angle 
			fsincos  
			mov edi, out_cos 
			fstp dword ptr [edi] 
			mov edi, out_sin 
			fstp dword ptr [edi] 
	} 
} 

inline MATRIX34::MATRIX34()
{
	m[0]=m[4]=m[8]=1.0f;
	m[11]=m[10]=m[9]=m[7]=m[6]=m[5]=m[3]=m[2]=m[1]=0.0f;
}
inline MATRIX34::MATRIX34(float* m2)
{
	*this=*(MATRIX34*)m2;
}
inline float MATRIX34::operator [] (const int aik) const
{
	return m[aik];
}
inline float& MATRIX34::operator [] (const int aik)
{
	return m[aik];
}

inline MATRIX34& MATRIX34::operator = (const MATRIX3 &m2)
{
	(MATRIX3&)*this=m2;
	return *this;
}

inline MATRIX34& MATRIX34::operator = (const MATRIX4 &m2)
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
	m[9]=m2.m[12];
	m[10]=m2.m[13];
	m[11]=m2.m[14];
}


inline MATRIX34 MATRIX34::operator * (const MATRIX34 &m2) const
{
   MATRIX34 result;
   float ma0,ma1,ma2;
    for (int i = 0; i < 9; i+=3)
	{
        ma0 = m2.m[i];  ma1 = m2.m[i+1];
	    ma2 = m2.m[i+2];  
        result.m[i] = ma0 * m[0] + ma1 * m[3] + ma2 * m[6];
        result.m[i+1] = ma0 * m[1] + ma1 * m[4] + ma2 * m[7];
        result.m[i+2] = ma0 * m[2] + ma1 * m[5] + ma2 * m[8];
    }
	result.m[9] = m2[9] * m[0] + m2[10] * m[3] + m2[11] * m[6] + m[9];
	result.m[10] = m2[9] * m[1] + m2[10] * m[4] + m2[11] * m[7] + m[10];
	result.m[11] = m2[9] * m[2] + m2[10] * m[5] + m2[11] * m[8] + m[11];
	return result;
}
inline VECTOR4 MATRIX34::operator * (const VECTOR4 &v2) const
{
	VECTOR4 temp;
	temp.x=m[0]*v2.x+m[3]*v2.y+m[6]*v2.z+m[9]*v2.w;
	temp.y=m[1]*v2.x+m[4]*v2.y+m[7]*v2.z+m[10]*v2.w;
	temp.z=m[2]*v2.x+m[5]*v2.y+m[8]*v2.z+m[11]*v2.w;
	temp.w=v2.w;
	return temp;
}
inline VECTOR3 MATRIX34::operator * (const VECTOR3 &v2) const
{
	VECTOR3 temp;
	temp.x=m[0]*v2.x+m[3]*v2.y+m[6]*v2.z+m[9];
	temp.y=m[1]*v2.x+m[4]*v2.y+m[7]*v2.z+m[10];
	temp.z=m[2]*v2.x+m[5]*v2.y+m[8]*v2.z+m[11];
	return temp;
}
inline void RotationMatrix34X(MATRIX34 &m,const float angle) // Angle in Radians
{
	float sinus,cosinus;
	sincos34(angle,sinus,cosinus);

	m.m[0]=1.0f;
	m.m[11]=m.m[10]=m.m[9]=m.m[6]=m.m[3]=m.m[2]=m.m[1]=0.0f;

	m.m[4]=cosinus;
	m.m[5]=sinus;
	m.m[7]=-sinus;
	m.m[8]=cosinus;

}
inline void RotationMatrix34Y(MATRIX34 &m,const float angle) // Angle in Radians
{
	float sinus,cosinus;
	sincos34(angle,sinus,cosinus);

	m.m[0]=cosinus;
	m.m[2]=-sinus;
	m.m[6]=sinus;
	m.m[8]=cosinus;

	m.m[11]=m.m[10]=m.m[9]=m.m[7]=m.m[5]=m.m[3]=m.m[1]=0.0f;
	m.m[4]=1.0f;
}
inline void RotationMatrix34Z(MATRIX34 &m,const float angle) // Angle in Radians
{
	float sinus,cosinus;
	sincos34(angle,sinus,cosinus);

	m.m[0]=cosinus;
	m.m[1]=sinus;
	m.m[3]=-sinus;
	m.m[4]=cosinus;

	m.m[11]=m.m[10]=m.m[9]=m.m[7]=m.m[6]=m.m[5]=m.m[2]=0.0f;
	m.m[8]=1.0f;
}

inline void Rotate34Z(MATRIX34 &matrix,const float Angle)
{
	Rotate3Z((MATRIX3&)matrix,Angle);
}
inline void Rotate34Y(MATRIX34 &matrix,const float Angle)
{
	Rotate3Y((MATRIX3&)matrix,Angle);
}

inline void Rotate34X(MATRIX34 &matrix,const float Angle)
{
	Rotate3X((MATRIX3&)matrix,Angle);
}

inline void ScaleMatrix34(MATRIX34 &m,const VECTOR3 &s)
{
	m.m[0]=s.x;
	m.m[4]=s.y;
	m.m[8]=s.z;
	m.m[11]=m.m[10]=m.m[9]=m.m[7]=m.m[6]=m.m[5]=m.m[3]=m.m[2]=m.m[1]=0.0f;
}

inline void Scale34(MATRIX34 &m,const VECTOR3 &s)
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
inline void TranslationMatrix34(MATRIX34 &m,const VECTOR3 &t)
{
	m.m[0]=m.m[4]=m.m[8]=1.0f;
	m.m[7]=m.m[6]=m.m[5]=m.m[3]=m.m[2]=m.m[1]=0.0f;

	m.m[9]=t.x;
	m.m[10]=t.y;
	m.m[11]=t.z;
}
inline void Translate34(MATRIX34 &m,const VECTOR3 &t)
{
	m.m[9]=m.m[0]*t.x+m.m[3]*t.y+m.m[6]*t.z+m.m[9];	
	m.m[10]=m.m[1]*t.x+m.m[4]*t.y+m.m[7]*t.z+m.m[10];
	m.m[11]=m.m[2]*t.x+m.m[5]*t.y+m.m[8]*t.z+m.m[11];	
}

inline void Transpose3(MATRIX34 &m) // transponiert R in m
{
	float temp=m.m[1];
	m.m[1]=m.m[3];
	m.m[3]=temp;
	temp=m.m[2];
	m.m[2]=m.m[6];
	m.m[6]=temp;
	temp=m.m[5];
	m.m[5]=m.m[7];
	m.m[7]=temp;
}

/** invert matrix (containing rotation and translation only!)  inplace */
inline void SimpleInverse34(MATRIX34 &m)
{
	VECTOR3 trans=(VECTOR3&)m.a[3];
	m.m[9]=-trans*(VECTOR3&)m.a[0];
	m.m[10]=-trans*(VECTOR3&)m.a[1];
	m.m[11]=-trans*(VECTOR3&)m.a[2];

	float temp=m.m[1];
	m.m[1]=m.m[3];
	m.m[3]=temp;
	temp=m.m[2];
	m.m[2]=m.m[6];
	m.m[6]=temp;
	temp=m.m[5];
	m.m[5]=m.m[7];
	m.m[7]=temp;
}

inline void SimpleInverse34(const MATRIX34 &mat, MATRIX34& inv)
{
	inv.m[0]=mat.m[0];
	inv.m[1]=mat.m[3];
	inv.m[2]=mat.m[6];
	inv.m[3]=mat.m[1];
	inv.m[4]=mat.m[4];
	inv.m[5]=mat.m[7];
	inv.m[6]=mat.m[2];
	inv.m[7]=mat.m[5];
	inv.m[8]=mat.m[8];

	const VECTOR3 &trans=(VECTOR3&)mat.a[3];
	inv.m[12]=-trans*(VECTOR3&)mat.a[0];
	inv.m[13]=-trans*(VECTOR3&)mat.a[1];
	inv.m[14]=-trans*(VECTOR3&)mat.a[2];
}

inline void AngleAxisMatrix34(MATRIX34& matrix,const VECTOR3 &axis, float angle)
{
	AngleAxisMatrix3((MATRIX3)matrix,axis,angle);
	matrix.m[9]=matrix.m[10]=matrix.m[11]=0.0f;
}

/** angles.x=pitch, angles.y=yaw , angles.z = roll*/
inline void	YawPitchRollMatrix34(const VECTOR3 &angles, MATRIX34 &matrix)
{
	YawPitchRollMatrix3(angles,(MATRIX3)matrix);
	matrix.m[9]=matrix.m[10]=matrix.m[11]=0.0f;
}

inline std::ostream& operator << (std::ostream &stream, const MATRIX34& M)
{
	stream<<'('<<M.m00<<' '<<M.m01<<' '<<M.m02<<' '<<M.m03<<')'<<std::endl;
	stream<<'('<<M.m10<<' '<<M.m11<<' '<<M.m12<<' '<<M.m13<<')'<<std::endl;
	stream<<'('<<M.m20<<' '<<M.m21<<' '<<M.m22<<' '<<M.m23<<')'<<std::endl;
	return stream;
}


