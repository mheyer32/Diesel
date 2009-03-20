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
#ifndef MATRIX34_H_INCLUDED
#define MATRIX34_H_INCLUDED

#include "Vector3.h"
#include "Vector4.h"

/*////////////////////////////////////////////////////////////////////
Function Prototypes
////////////////////////////////////////////////////////////////////*/

/*////////////////////////////////////////////////////////////////////
Types
////////////////////////////////////////////////////////////////////*/

union MATRIX3;
union MATRIX4;

// column-major order
union MATRIX34
{	
	float m[12];
	float a[4][3]; // [column][row]
	struct 
	{
		float m00,m10,m20;// +---> columns
		float m01,m11,m21;// |
		float m02,m12,m22;// |
		float m03,m13,m23;// v  rows
	};

	inline MATRIX34();
	inline MATRIX34(float *m2);
	inline float operator [] (const int aik) const;
	inline float& operator [] (const int aik);
	inline VECTOR4 operator * (const VECTOR4 &v) const;
	inline VECTOR3 operator * (const VECTOR3 &v) const;
	inline MATRIX34 operator * (const MATRIX34 &m2) const; 
	inline MATRIX34& operator = (const MATRIX3 &m2);
	inline MATRIX34& operator = (const MATRIX4 &m2);
	
	inline operator MATRIX3& () {return *(MATRIX3*)this;};
	inline operator float* () const {return (float*)m;};
};

static const MATRIX34 IdentityMatrix34;

/*////////////////////////////////////////////////////////////////////////////////////////////////////////
 Inline-Funktionen
///////////////////////////////////////////////////////////////////////////////////////////////////////*/

/** get a rotation matrix, angles are in radians*/
inline void RotationMatrix34X(MATRIX34 &m,const float Angle);
inline void RotationMatrix34Y(MATRIX34 &m,const float Angle);
inline void RotationMatrix34Z(MATRIX34 &m,const float Angle);

/** same as multiplying m with a rotation matrix, angle in radians*/
inline void Rotate34Z(MATRIX34 &m,const float Angle);
inline void Rotate34Y(MATRIX34 &m,const float Angle);
inline void Rotate34X(MATRIX34 &m,const float Angle);

/** get scaling matrix, overwrite m */
inline void ScaleMatrix34(MATRIX34 &m,const VECTOR3 &s);

/** same as multiplying m with a scaling matrix */
inline void Scale34(MATRIX34 &m,const VECTOR3 &s); // vielleicht einen Vector zum Rotieren nehmen?

/** get a 3D-homogenous translation matrix, overwriting m */
inline void TranslationMatrix34(MATRIX34 &m,const VECTOR3 &t);

/** same as multiplying m with a translational matrix */
inline void Translate34(MATRIX34 &m,const VECTOR3 &t);

/** get a rotation matrix, composed as  M*v= Ry*Rx*Rz*v */
inline void	YawPitchRollMatrix34(const VECTOR3 &angles, MATRIX34 &matrix);

/** create a rotational matrix from the given axis and rotation around it */
inline void AngleAxisMatrix34(MATRIX34& matrix,const VECTOR3 &axis, float angle);

/** transpose upper 3x3 part of m */
inline void Transpose3(MATRIX34 &m); // transponiert R in m

/** inverse m as  M^-1 = (R^T) * (T^-1)
	it only works for matrices that have an orthonormal rotational part */
inline void SimpleInverse34(MATRIX34 &m); // inverse inplace
inline void SimpleInverse34(const MATRIX34 &mat, MATRIX34& inv);

inline void Multiply(const MATRIX34 &m, const VECTOR3 &v, VECTOR3 &result)
{
	result.x=m.m00*v.x+m.m01*v.y+m.m02*v.z+m.m03;
	result.y=m.m10*v.x+m.m11*v.y+m.m12*v.z+m.m02;
	result.z=m.m20*v.x+m.m21*v.y+m.m22*v.z+m.m03;
}

#include "Matrix3.h"
#include "Matrix4.h"
#include "Matrix34.hh"
#endif