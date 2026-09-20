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
#ifndef MATRIX4_H_INCLUDED
#define MATRIX4_H_INCLUDED

#include "Vector4.h"

/*////////////////////////////////////////////////////////////////////
Function Prototypes
////////////////////////////////////////////////////////////////////*/

/*////////////////////////////////////////////////////////////////////
Types
////////////////////////////////////////////////////////////////////*/

union MATRIX3;

// column-major order
union MATRIX4
{
    float m[16];
    float a[4][4];  // [column][row]
    struct
    {
        float m00, m10, m20, m30;  // +---> columns
        float m01, m11, m21, m31;  // |
        float m02, m12, m22, m32;  // |
        float m03, m13, m23, m33;  // v  rows
    };

    inline MATRIX4();
    inline MATRIX4(float* m2);
    inline MATRIX4(bool dontinitialize) {};  ///< this constructor  doesn´t initialize the members, use with care

    inline float    operator[](const int aik) const;
    inline float&   operator[](const int aik);
    inline VECTOR4  operator*(const VECTOR4& v) const;
    inline VECTOR3  operator*(const VECTOR3& v) const;
    inline MATRIX4  operator*(const MATRIX4& m2) const;
    inline MATRIX4& operator=(const MATRIX3& m2);

    inline operator float*() const { return (float*)m; };

protected:
};

static const MATRIX4 IdentityMatrix4;

/*////////////////////////////////////////////////////////////////////////////////////////////////////////
 Inline-Funktionen
///////////////////////////////////////////////////////////////////////////////////////////////////////*/

/** get a rotation matrix, angles are in radians*/
inline void RotationMatrix4X(MATRIX4& m, const float Angle);
inline void RotationMatrix4Y(MATRIX4& m, const float Angle);
inline void RotationMatrix4Z(MATRIX4& m, const float Angle);

/** same as multiplying m with a rotation matrix, angle in radians*/
inline void Rotate4Z(MATRIX4& m, const float Angle);
inline void Rotate4Y(MATRIX4& m, const float Angle);
inline void Rotate4X(MATRIX4& m, const float Angle);

/** get scaling matrix, overwrite m */
inline void ScaleMatrix4(MATRIX4& m, const VECTOR3& s);

/** same as multiplying m with a scaling matrix */
inline void Scale4(MATRIX4& m, const VECTOR3& s);  // vielleicht einen Vector zum Rotieren nehmen?

/** get a 3D-homogenous translation matrix, overwriting m */
inline void TranslationMatrix4(MATRIX4& m, const VECTOR3& t);

/** same as multiplying m with a translational matrix */
inline void Translate4(MATRIX4& m, const VECTOR3& t);

/** get a rotation matrix, composed as  M*v= Ry*Rx*Rz*v */
inline void YawPitchRollMatrix4(const VECTOR3& angles, MATRIX4& matrix);

/** create a rotational matrix from the given axis and rotation around it */
inline void AngleAxisMatrix4(MATRIX4& matrix, const VECTOR3& axis, float angle);

/** transpose upper 3x3 part of m */
inline void Transpose3(MATRIX4& m);  // transponiert R in m

/** fully transpose m */
inline void Transpose4(MATRIX4& m);

/** inverse m as  M^-1 = (R^T) * (T^-1)
    it only works for matrices that have an orthonormal rotational part */
inline void SimpleInverse4(MATRIX4& m);  // inverse inplace
inline void SimpleInverse4(const MATRIX4& mat, MATRIX4& inv);

/** full blown inverse for regular 4x4 matrices */
extern void FullInverse4(const MATRIX4& M, MATRIX4& Inv);

inline void Multiply(const MATRIX4& m, const VECTOR3& v, VECTOR3& result)
{
    result.x = m.m00 * v.x + m.m01 * v.y + m.m02 * v.z + m.m03;
    result.y = m.m10 * v.x + m.m11 * v.y + m.m12 * v.z + m.m02;
    result.z = m.m20 * v.x + m.m21 * v.y + m.m22 * v.z + m.m03;
}

inline void PermutationMatrix4(MATRIX4& m, int x, int y, int z, int w);

#include "Matrix3.h"

#include "Matrix4.hh"
#endif