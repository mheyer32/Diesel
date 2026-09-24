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
#include <cassert>
#include <cmath>
#include <ostream>


__inline void sincos4(float in_angle, float &out_sin, float &out_cos)
{
    out_sin = sinf(in_angle);
    out_cos = cosf(in_angle);
} 

inline MATRIX4::MATRIX4()
{
	m[14]=m[13]=m[12]=m[11]=m[9]=m[8]=m[7]=m[6]=m[4]=m[3]=m[2]=m[1]=0.0f;
	m[15]=m[10]=m[5]=m[0]=1.0f;
}
inline MATRIX4::MATRIX4(float* m2)
{
	*this=*(MATRIX4*)m2;
}
inline float MATRIX4::operator [] (const int aik) const
{
	return m[aik];
}
inline float& MATRIX4::operator [] (const int aik)
{
	return m[aik];
}

inline MATRIX4& MATRIX4::operator = (const MATRIX3 &m2)
{
	m[0]=m2.m[0];
	m[1]=m2.m[1];
	m[2]=m2.m[2];

	m[4]=m2.m[3];
	m[5]=m2.m[4];
	m[6]=m2.m[5];

	m[8]=m2.m[6];
	m[9]=m2.m[7];
	m[10]=m2.m[8];

	return *this;
}

inline MATRIX4 MATRIX4::operator * (const MATRIX4 &m2) const
{
	MATRIX4 result(false);
   float ma0,ma1,ma2,ma3;
   int i;

    for (i = 0; i < 16; i+=4)
	{
        ma0 = m2.m[i];  ma1 = m2.m[i+1];
	    ma2 = m2.m[i+2];  ma3 = m2.m[i+3];
        result.m[i] = ma0 * m[0] + ma1 * m[4] + ma2 * m[8] + ma3 * m[12];
        result.m[i+1] = ma0 * m[1] + ma1 * m[5] + ma2 * m[9] + ma3 * m[13];
        result.m[i+2] = ma0 * m[2] + ma1 * m[6] + ma2 * m[10] + ma3 * m[14];
        result.m[i+3] = ma0 * m[3] + ma1 * m[7] + ma2 * m[11] + ma3 * m[15];
	
    }
	return result;
}
inline VECTOR4 MATRIX4::operator * (const VECTOR4 &v2) const
{
	VECTOR4 temp;
	temp.x=m[0]*v2.x+m[4]*v2.y+m[8]*v2.z+m[12]*v2.w;
	temp.y=m[1]*v2.x+m[5]*v2.y+m[9]*v2.z+m[13]*v2.w;
	temp.z=m[2]*v2.x+m[6]*v2.y+m[10]*v2.z+m[14]*v2.w;
	temp.w=m[3]*v2.x+m[7]*v2.y+m[11]*v2.z+m[15]*v2.w;
	return temp;
}
inline VECTOR3 MATRIX4::operator * (const VECTOR3 &v2) const
{
	VECTOR3 temp;
	temp.x=m[0]*v2.x+m[4]*v2.y+m[8]*v2.z+m[12];
	temp.y=m[1]*v2.x+m[5]*v2.y+m[9]*v2.z+m[13];
	temp.z=m[2]*v2.x+m[6]*v2.y+m[10]*v2.z+m[14];
	return temp;
}
inline void RotationMatrix4X(MATRIX4 &m,const float angle) // Angle in Radians
{
	m.m[14]=m.m[13]=m.m[12]=m.m[11]=m.m[8]=m.m[7]=m.m[4]=m.m[3]=m.m[2]=m.m[1]=0.0f;
	m.m[0]=m.m[15]=1.0f;
	float sinus,cosinus;
	sincos4(angle,sinus,cosinus);
	m.m[5]=cosinus;
	m.m[6]=sinus;
	m.m[9]=-sinus;
	m.m[10]=cosinus;
}
inline void RotationMatrix4Y(MATRIX4 &m,const float angle) // Angle in Radians
{
	m.m[14]=m.m[13]=m.m[12]=m.m[11]=m.m[9]=m.m[7]=m.m[6]=m.m[4]=m.m[3]=m.m[1]=0.0f;
	m.m[5]=m.m[15]=1.0f;
	float sinus,cosinus;
	sincos4(angle,sinus,cosinus);
	m.m[0]=cosinus;
	m.m[2]=-sinus;
	m.m[8]=sinus;
	m.m[10]=cosinus;
}
inline void RotationMatrix4Z(MATRIX4 &m,const float angle) // Angle in Radians
{
	m.m[14]=m.m[13]=m.m[12]=m.m[11]=m.m[9]=m.m[8]=m.m[7]=m.m[6]=m.m[3]=m.m[2]=0.0f;
	m.m[10]=m.m[15]=1.0f;
	float sinus,cosinus;
	sincos4(angle,sinus,cosinus);
	m.m[0]=cosinus;
	m.m[1]=sinus;
	m.m[4]=-sinus;
	m.m[5]=cosinus;
}

inline void Rotate4Z(MATRIX4 &m,const float angle)
{
	float sinus,cosinus;
	sincos4(angle,sinus,cosinus);
	float t1=m.m[0];
	float t2=m.m[4];
	m.m[0]=t1*cosinus+t2*sinus; m.m[4]=t2*cosinus-t1*sinus;
	t1=m.m[1];t2=m.m[5];
	m.m[1]=t1*cosinus+t2*sinus; m.m[5]=t2*cosinus-t1*sinus;
	t1=m.m[2];t2=m.m[6];
	m.m[2]=t1*cosinus+t2*sinus; m.m[6]=t2*cosinus-t1*sinus;
}
inline void Rotate4Y(MATRIX4 &m,const float angle)
{
	float sinus,cosinus;
	sincos4(angle,sinus,cosinus);
	float t1=m.m[0];
	float t2=m.m[8];
	m.m[0]=t1*cosinus-t2*sinus; m.m[8]=t2*cosinus+t1*sinus;
	t1=m.m[1];t2=m.m[9];
	m.m[1]=t1*cosinus-t2*sinus; m.m[9]=t2*cosinus+t1*sinus;
	t1=m.m[2];t2=m.m[10];
	m.m[2]=t1*cosinus-t2*sinus; m.m[10]=t2*cosinus+t1*sinus;
}

inline void Rotate4X(MATRIX4 &m,const float angle)
{
	float sinus,cosinus;
	sincos4(angle,sinus,cosinus);
	float t1=m.m[4];
	float t2=m.m[8];
	m.m[4]=t1*cosinus+t2*sinus; m.m[8]=t2*cosinus-t1*sinus;
	t1=m.m[5];t2=m.m[9];
	m.m[5]=t1*cosinus+t2*sinus; m.m[9]=t2*cosinus-t1*sinus;
	t1=m.m[6];t2=m.m[10];
	m.m[6]=t1*cosinus+t2*sinus; m.m[10]=t2*cosinus-t1*sinus;
}

inline void ScaleMatrix4(MATRIX4 &m,const VECTOR3 &s)
{
	m.m[14]=m.m[13]=m.m[12]=m.m[11]=m.m[9]=m.m[8]=m.m[7]=m.m[6]=m.m[4]=m.m[3]=m.m[2]=m.m[1]=0.0f;
	m.m[15]=1.0f;

	m.m[0]=s.x;
	m.m[5]=s.y;
	m.m[10]=s.z;
}

inline void Scale4(MATRIX4 &m,const VECTOR3 &s) // vielleicht einen Vector zum Rotieren nehmen?
{
	m.m[0]*=s.x;
	m.m[1]*=s.x;
	m.m[2]*=s.x;
	m.m[3]*=s.x;
	m.m[4]*=s.y;
	m.m[5]*=s.y;
	m.m[6]*=s.y;
	m.m[7]*=s.y;
	m.m[8]*=s.z;
	m.m[9]*=s.z;
	m.m[10]*=s.z;
	m.m[11]*=s.z;
}
inline void TranslationMatrix4(MATRIX4 &m,const VECTOR3 &t)
{
	m.m[11]=m.m[9]=m.m[8]=m.m[7]=m.m[6]=m.m[4]=m.m[3]=m.m[2]=m.m[1]=0.0f;
	m.m[0]=m.m[5]=m.m[10]=m.m[15]=1.0f;

	m.m[12]=t.x;
	m.m[13]=t.y;
	m.m[14]=t.z;
}
inline void Translate4(MATRIX4 &m,const VECTOR3 &t)
{
	m.m[12]+=m.m[0]*t.x+m.m[4]*t.y+m.m[8]*t.z;	
	m.m[13]+=m.m[1]*t.x+m.m[5]*t.y+m.m[9]*t.z;
	m.m[14]+=m.m[2]*t.x+m.m[6]*t.y+m.m[10]*t.z;	
	m.m[15]+=m.m[3]*t.x+m.m[7]*t.y+m.m[11]*t.z;	
}

inline void Transpose3(MATRIX4 &m) // transponiert R in m
{
	float temp;
	temp=m.m[1];
	m.m[1]=m.m[4];
	m.m[4]=temp;
	temp=m.m[2];
	m.m[2]=m.m[8];
	m.m[8]=temp;
	temp=m.m[6];
	m.m[6]=m.m[9];
	m.m[9]=temp;
}

inline void Transpose4(MATRIX4 &m)
{
	float temp;
	temp=m.m[1];
	m.m[1]=m.m[4];
	m.m[4]=temp;
	temp=m.m[2];
	m.m[2]=m.m[8];
	m.m[8]=temp;
	temp=m.m[3];
	m.m[3]=m.m[12];
	m.m[12]=temp;
	temp=m.m[6];
	m.m[6]=m.m[9];
	m.m[9]=temp;
	temp=m.m[7];
	m.m[7]=m.m[13];
	m.m[13]=temp;
	temp=m.m[11];
	m.m[11]=m.m[14];
	m.m[14]=temp;
}

/** invert matrix (containing rotation and translation only!)  inplace */
inline void SimpleInverse4(MATRIX4 &m)
{
	VECTOR3 trans((VECTOR3&)m.a[3]);
	m.m[12]=-trans*(VECTOR3&)m.a[0];
	m.m[13]=-trans*(VECTOR3&)m.a[1];
	m.m[14]=-trans*(VECTOR3&)m.a[2];

	float t=m.m[1];
	m.m[1]=m.m[4];
	m.m[4]=t;
	t=m.m[2];
	m.m[2]=m.m[8];
	m.m[8]=t;
	t=m.m[9];
	m.m[9]=m.m[6];
	m.m[6]=t;
}

inline void SimpleInverse4(const MATRIX4 &mat, MATRIX4& inv)
{
	inv.m[0]=mat.m[0];
	inv.m[1]=mat.m[4];
	inv.m[2]=mat.m[8];
	inv.m[3]=mat.m[3];
	inv.m[4]=mat.m[1];
	inv.m[5]=mat.m[5];
	inv.m[6]=mat.m[9];
	inv.m[7]=mat.m[7];
	inv.m[8]=mat.m[2];
	inv.m[9]=mat.m[6];
	inv.m[10]=mat.m[10];
	inv.m[11]=mat.m[11];

	const VECTOR3 &trans=(VECTOR3&)mat.a[3];
	inv.m[12]=-trans*(VECTOR3&)mat.a[0];
	inv.m[13]=-trans*(VECTOR3&)mat.a[1];
	inv.m[14]=-trans*(VECTOR3&)mat.a[2];
}

// FIXME: avoid copying by taking MATRIX4 &m as argument ?
inline void AngleAxisMatrix4(MATRIX4& matrix,const VECTOR3 &axis, float angle)
{
	float c,s;
	sincos4(angle,s,c);
	const float c2=1.0f-c;
	float temp;

	matrix.m[0]=c+c2*axis.x*axis.x;
	matrix.m[1]=matrix.m[4]=c2*axis.x*axis.y;
	temp=axis.z*s;
	matrix.m[1]+=temp;
	matrix.m[4]-=temp;
	matrix.m[2]=matrix.m[8]=c2*axis.x*axis.z;
	temp=axis.y*s;
	matrix.m[2]-=temp;
	matrix.m[8]+=temp;
	matrix.m[5]=c+c2*axis.y*axis.y;
	matrix.m[6]=matrix.m[9]=c2*axis.y*axis.z;
	temp=axis.x*s;
	matrix.m[6]+=temp;
	matrix.m[9]-=temp;
	matrix.m[10]=c+c2*axis.z*axis.z;

	//just for sure
	matrix.m[3]=matrix.m[7]=matrix.m[11]=matrix.m[12]=matrix.m[13]=matrix.m[14]=0.0f;
	matrix.m[15]=1.0f;
}

/** angles.x=pitch, angles.y=yaw , angles.z = roll*/
inline void	YawPitchRollMatrix4(const VECTOR3 &angles, MATRIX4 &matrix)
{
	// Reihenfolge der Rotationen : Z - X - Y  M*v= Ry*Rx*Rz
	float cz;
	float sz;
	sincos4(angles.z,sz,cz);
	float cy;
	float sy;
	sincos4(angles.y,sy,cy);
	float cx;
	float sx;
	sincos4(angles.x,sx,cx);

	matrix.m[8]=cx*sy;
	matrix.m[9]=-sx;
	matrix.m[10]=cx*cy;
	matrix.m[4]=-sz*cy+cz*sx*sy;
	matrix.m[5]=cz*cx;
	matrix.m[6]=+sz*sy+cz*sx*cy;

	matrix.m[0]=matrix.m[5]*matrix.m[10]-matrix.m[9]*matrix.m[6];
	matrix.m[1]=matrix.m[6]*matrix.m[8]-matrix.m[10]*matrix.m[4];
	matrix.m[2]=matrix.m[4]*matrix.m[9]-matrix.m[8]*matrix.m[5];
	//just for sure
	matrix.m[3]=matrix.m[7]=matrix.m[11]=matrix.m[12]=matrix.m[13]=matrix.m[14]=0.0f;
	matrix.m[15]=1.0f;
}

inline std::ostream& operator << (std::ostream &stream, const MATRIX4& M)
{
	stream<<'('<<M.m00<<' '<<M.m01<<' '<<M.m02<<' '<<M.m03<<')'<<std::endl;
	stream<<'('<<M.m10<<' '<<M.m11<<' '<<M.m12<<' '<<M.m13<<')'<<std::endl;
	stream<<'('<<M.m20<<' '<<M.m21<<' '<<M.m22<<' '<<M.m23<<')'<<std::endl;
	stream<<'('<<M.m30<<' '<<M.m31<<' '<<M.m32<<' '<<M.m33<<')'<<std::endl;
	return stream;
}

inline void PermutationMatrix4(MATRIX4 &m, int x, int y, int z, int w)
{
	for (int i=0; i<16; ++i) m.m[i]=0.0f;
	m.a[0][x]=1.0f;
	m.a[1][y]=1.0f;
	m.a[2][z]=1.0f;
	m.a[3][w]=1.0f;
}

