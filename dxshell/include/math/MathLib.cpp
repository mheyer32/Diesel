
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
#include <math\Plane.h>
#include "MathLib.h"

void MatrixMultiply4(const float* m1, const float* m2, float* result)
{
    assert(result != m1);
    // M1*M2=RESULT
    float ma0, ma1, ma2, ma3;
    int   i;

    for (i = 0; i < 16; i += 4) {
        ma0           = m2[i];
        ma1           = m2[i + 1];
        ma2           = m2[i + 2];
        ma3           = m2[i + 3];
        result[i]     = ma0 * m1[0] + ma1 * m1[4] + ma2 * m1[8] + ma3 * m1[12];
        result[i + 1] = ma0 * m1[1] + ma1 * m1[5] + ma2 * m1[9] + ma3 * m1[13];
        result[i + 2] = ma0 * m1[2] + ma1 * m1[6] + ma2 * m1[10] + ma3 * m1[14];
        result[i + 3] = ma0 * m1[3] + ma1 * m1[7] + ma2 * m1[11] + ma3 * m1[15];
    }
}
void MatrixMultiply3(const float* m1, const float* m2, float* result)
{
    assert(result != m1);
    // M1*M2=RESULT
    float ma0, ma1, ma2;
    int   i;

    for (i = 0; i < 9; i += 3) {
        ma0           = m2[i];
        ma1           = m2[i + 1];
        ma2           = m2[i + 2];
        result[i]     = ma0 * m1[0] + ma1 * m1[3] + ma2 * m1[6];
        result[i + 1] = ma0 * m1[1] + ma1 * m1[4] + ma2 * m1[7];
        result[i + 2] = ma0 * m1[2] + ma1 * m1[5] + ma2 * m1[8];
    }
}

void MatrixMultiply34(const float* m1, const float* m2, float* result)
{
    assert(result != m1);
    float ma0, ma1, ma2;
    for (int i = 0; i < 9; i += 3) {
        ma0           = m2[i];
        ma1           = m2[i + 1];
        ma2           = m2[i + 2];
        result[i]     = ma0 * m1[0] + ma1 * m1[3] + ma2 * m1[6];
        result[i + 1] = ma0 * m1[1] + ma1 * m1[4] + ma2 * m1[7];
        result[i + 2] = ma0 * m1[2] + ma1 * m1[5] + ma2 * m1[8];
    }
    ma0        = m2[9];
    ma1        = m2[10];
    ma2        = m2[11];
    result[9]  = ma0 * m1[0] + ma1 * m1[3] + ma2 * m1[6] + m1[9];
    result[10] = ma0 * m1[1] + ma1 * m1[4] + ma2 * m1[7] + m1[10];
    result[11] = ma0 * m1[2] + ma1 * m1[5] + ma2 * m1[8] + m1[11];
}

VECTOR3 PerpendicularVector(const VECTOR3& V)
{
    VECTOR3 Perp;

    Perp.x = (float)fabs(V.x);
    Perp.y = (float)fabs(V.y);
    Perp.z = (float)fabs(V.z);

    if (Perp.x <= Perp.y && Perp.x <= Perp.z) {
        Perp = VECTOR3(1, 0, 0);
    } else {
        if (Perp.y <= Perp.x && Perp.y <= Perp.z) {
            Perp = VECTOR3(0, 1, 0);
        } else {
            Perp = VECTOR3(0, 0, 1);
        }
    }
    Perp = Perp - (V * (Perp * V));  // lay Perp into the plane perpendicular to V
    return Perp;
}

void ConstructMatrix4X(const VECTOR3& xaxis, MATRIX4& M)
{
    (VECTOR3&)M.a[0] = xaxis;
    Normalize((VECTOR3&)M.a[0]);
    (VECTOR3&)M.a[1] = PerpendicularVector((VECTOR3&)M.a[0]);
    Normalize((VECTOR3&)M.a[1]);
    (VECTOR3&)M.a[2] = (VECTOR3&)M.a[0] ^ (VECTOR3&)M.a[1];
}

void ConstructMatrix4Z(const VECTOR3& zaxis, MATRIX4& M)
{
    (VECTOR3&)M.a[2] = zaxis;
    Normalize((VECTOR3&)M.a[2]);

    const VECTOR3 up = (zaxis != VECTOR3(0, 1, 0)) ? VECTOR3(0, 1, 0) : VECTOR3(0, 0, 1);

    (VECTOR3&)M.a[0] = up ^ zaxis;
    Normalize((VECTOR3&)M.a[0]);

    (VECTOR3&)M.a[1] = (VECTOR3&)M.a[2] ^ (VECTOR3&)M.a[0];
    Normalize((VECTOR3&)M.a[1]);
}

// Full Matrix inverse from NVidia

inline float det2x2(float a1, float a2, float b1, float b2)
{
    return a1 * b2 - a2 * b1;
}

/*
    calculate the determinent of a 3x3 matrix in the from

    | a1 a2 a3 |
    | b1 b2 b3 |
    | c1 c2 c3 |

*/
inline float det3x3(float a1, float a2, float a3, float b1, float b2, float b3, float c1, float c2, float c3)
{
    return a1 * b2 * c3 + a2 * b3 * c1 + a3 * b1 * c2 - a3 * b2 * c1 - a1 * b3 * c2 - a2 * b1 * c3;
}

inline void FullInverse4(const MATRIX4& M, MATRIX4& Inv)
{

    assert(&M != &Inv);  // must be different matrices or you´ll get bogus results

    float det, oodet;

    Inv.m00 = det3x3(M.m11, M.m21, M.m31, M.m12, M.m22, M.m32, M.m13, M.m23, M.m33);
    Inv.m10 = -det3x3(M.m10, M.m20, M.m30, M.m12, M.m22, M.m32, M.m13, M.m23, M.m33);
    Inv.m20 = det3x3(M.m10, M.m20, M.m30, M.m11, M.m21, M.m31, M.m13, M.m23, M.m33);
    Inv.m30 = -det3x3(M.m10, M.m20, M.m30, M.m11, M.m21, M.m31, M.m12, M.m22, M.m32);

    Inv.m01 = -det3x3(M.m01, M.m21, M.m31, M.m02, M.m22, M.m32, M.m03, M.m23, M.m33);
    Inv.m11 = det3x3(M.m00, M.m20, M.m30, M.m02, M.m22, M.m32, M.m03, M.m23, M.m33);
    Inv.m21 = -det3x3(M.m00, M.m20, M.m30, M.m01, M.m21, M.m31, M.m03, M.m23, M.m33);
    Inv.m31 = det3x3(M.m00, M.m20, M.m30, M.m01, M.m21, M.m31, M.m02, M.m22, M.m32);

    Inv.m02 = det3x3(M.m01, M.m11, M.m31, M.m02, M.m12, M.m32, M.m03, M.m13, M.m33);
    Inv.m12 = -det3x3(M.m00, M.m10, M.m30, M.m02, M.m12, M.m32, M.m03, M.m13, M.m33);
    Inv.m22 = det3x3(M.m00, M.m10, M.m30, M.m01, M.m11, M.m31, M.m03, M.m13, M.m33);
    Inv.m32 = -det3x3(M.m00, M.m10, M.m30, M.m01, M.m11, M.m31, M.m02, M.m12, M.m32);

    Inv.m03 = -det3x3(M.m01, M.m11, M.m21, M.m02, M.m12, M.m22, M.m03, M.m13, M.m23);
    Inv.m13 = det3x3(M.m00, M.m10, M.m20, M.m02, M.m12, M.m22, M.m03, M.m13, M.m23);
    Inv.m23 = -det3x3(M.m00, M.m10, M.m20, M.m01, M.m11, M.m21, M.m03, M.m13, M.m23);
    Inv.m33 = det3x3(M.m00, M.m10, M.m20, M.m01, M.m11, M.m21, M.m02, M.m12, M.m22);

    det = (M.m00 * Inv.m00) + (M.m01 * Inv.m10) + (M.m02 * Inv.m20) + (M.m03 * Inv.m30);
    //  det = (M.m00 * Inv.m00) + (M.m10 * Inv.m10) + (M.m20 * Inv.m20) + (M.m30 * Inv.m30);

    oodet = 1.0f / det;

    for (int i = 0; i < 16; ++i)
        Inv[i] *= oodet;
}

inline void FullInverse3(const MATRIX3& m, MATRIX3& inv)
{
    assert(&m != &inv);  // must be different matrices or you´ll get bogus results

    inv.m00 = det2x2(m.m11, m.m12, m.m21, m.m22);
    inv.m10 = -det2x2(m.m01, m.m02, m.m21, m.m22);
    inv.m20 = det2x2(m.m01, m.m02, m.m11, m.m12);

    inv.m01 = -det2x2(m.m10, m.m12, m.m20, m.m22);
    inv.m11 = det2x2(m.m00, m.m02, m.m20, m.m22);
    inv.m21 = -det2x2(m.m00, m.m02, m.m10, m.m12);

    inv.m02 = det2x2(m.m10, m.m11, m.m20, m.m21);
    inv.m12 = -det2x2(m.m00, m.m01, m.m20, m.m21);
    inv.m22 = det2x2(m.m00, m.m01, m.m10, m.m11);

    // determinant of 3x3 matrix
    // float d=1.0f/(m.m00*inv.m00 + m.m10*inv.m10 + m.m20*inv.m20);
    const float d = 1.0f / det3x3(m.m00, m.m01, m.m02, m.m10, m.m11, m.m12, m.m20, m.m21, m.m22);

    for (int i = 0; i < 9; ++i)
        inv[i] *= d;
}