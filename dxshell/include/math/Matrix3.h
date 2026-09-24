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
#ifndef MATRIX3_H_INCLUDED
#define MATRIX3_H_INCLUDED

#include "Vector2.h"
#include "Vector3.h"

/*////////////////////////////////////////////////////////////////////
Function Prototypes
////////////////////////////////////////////////////////////////////*/

/*////////////////////////////////////////////////////////////////////
Types
////////////////////////////////////////////////////////////////////*/

union MATRIX4;

// column-major order
union MATRIX3
{
    float m[9];
    float a[3][3];  // [column][row]
    struct
    {
        float m00, m10, m20;  // --> rows
        float m01, m11, m21;  // |
        float m02, m12, m22;  // v  columns
    };

    inline MATRIX3();  // constructs identity matrix;
    inline MATRIX3(float* m2);
    inline MATRIX3(const MATRIX4& m2);
    inline float    operator[](const int aik) const;  // accessor for m
    inline float&   operator[](const int aik);
    inline VECTOR3  operator*(const VECTOR3& v) const;
    inline VECTOR2  operator*(const VECTOR2& v) const;
    inline MATRIX3  operator*(const MATRIX3& m2) const;
    inline MATRIX3& operator=(const MATRIX4& m2);  // extracts rotational part of m2

    inline operator float*() const { return (float*)m; };
};

/** use assignment  to clear a matrix to identity */
static const MATRIX3 IdentityMatrix3;

/*////////////////////////////////////////////////////////////////////////////////////////////////////////
 Prototypes, defined in matrix3.hh
///////////////////////////////////////////////////////////////////////////////////////////////////////*/

/** get scaling matrix, overwrite m */
inline void ScaleMatrix3(MATRIX3& m, const VECTOR3& s);

/** same as multiplying m with a scaling matrix */
inline void Scale3(MATRIX3& m, const VECTOR3& s);

/** get a rotation matrix, angle are in radians*/
inline void RotationMatrix3X(MATRIX3& m, const float Angle);
inline void RotationMatrix3Y(MATRIX3& m, const float Angle);
inline void RotationMatrix3Z(MATRIX3& m, const float Angle);

/** same as multiplying m with a rotation matrix, angle in radians*/
inline void Rotate3Z(MATRIX3& m, const float Angle);
inline void Rotate3Y(MATRIX3& m, const float Angle);
inline void Rotate3X(MATRIX3& m, const float Angle);

/** get a 2D-homogenous translation matrix, overwriting m */
inline void TranslationMatrix3(MATRIX3& m, const VECTOR2& t);

/** same as multiplying m with a translational matrix */
inline void Translate3(MATRIX3& m, const VECTOR2& t);

/** transpose m inplace */
inline void Transpose3(MATRIX3& m);

/** get a rotation matrix, composed as  M*v= Ry*Rx*Rz*v */
inline void YawPitchRollMatrix3(const VECTOR3& angles, MATRIX3& matrix);

/** get a rotation matrix that rotates around the given axis */
inline void AngleAxisMatrix3(MATRIX3& m, const VECTOR3& axis, const float angle);

/** get full blown inverse*/
// extern void FullInverse3(const MATRIX3 &m, MATRIX3 &inv);

inline void Multiply(const MATRIX3& m, const VECTOR3& v, VECTOR3& result)
{
    result.x = m.m00 * v.x + m.m01 * v.y + m.m02 * v.z;
    result.y = m.m10 * v.x + m.m11 * v.y + m.m12 * v.z;
    result.z = m.m20 * v.x + m.m21 * v.y + m.m22 * v.z;
}
extern void FullInverse3(const MATRIX3& M, MATRIX3& Inv);

#include "Matrix4.h"

#include "matrix3.hh"

#endif