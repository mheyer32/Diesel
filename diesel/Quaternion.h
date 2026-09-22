/*
This file is part of Diesel
(c) 2002 by Mathias Heyer
email: sonode@gmx.de

Diesel is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Diesel is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#ifndef QUATERNION_H
#define QUATERNION_H

#include <defs.h>
#include <math.h>
#include <math/MathLib.h>
#include <math/Matrix3.h>
#include <math/Matrix4.h>
#include <math/Vector3.h>

struct QUATERNION
{
    VECTOR3 q;  // axis, followed by angle
    float   qw;

    inline QUATERNION()
    : q(0, 0, 0)
    , qw(1) {};
    inline QUATERNION(const VECTOR3& axis, const float angle);

    inline QUATERNION operator*(const QUATERNION& r) const;
    inline QUATERNION operator*(const float s) const;
    inline QUATERNION operator+(const QUATERNION& r) const;
    inline QUATERNION operator-(const QUATERNION& r) const;
    inline QUATERNION operator-() const;

    inline VECTOR3 operator*(const VECTOR3& p) const;

protected:
    // no normalization is done here!!!
    inline QUATERNION(const float nqx, const float nqy, const float nqz, const float nqw);
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline QUATERNION::QUATERNION(const VECTOR3& axis, float angle)
{
    q = axis;  // a unit quaternion is defined as q= ( sin(a)*uq , cos(a))
    Normalize(q);
    angle *= 0.5f;
    q *= sinf(angle);
    qw = cosf(angle);
}

inline QUATERNION::QUATERNION(const float nqx, const float nqy, const float nqz, const float nqw)
: q(nqx, nqy, nqz)
, qw(nqw)
{
}

inline QUATERNION QUATERNION::operator*(const QUATERNION& r) const
{
    QUATERNION nq;
    nq.q  = (q ^ r.q) + (q * r.qw) + (r.q * qw);
    nq.qw = (qw * r.qw) - (q * r.q);
    return nq;
}

inline QUATERNION QUATERNION::operator+(const QUATERNION& r) const
{
    return QUATERNION(q.x + r.q.x, q.y + r.q.y, q.z + r.q.z, qw + r.qw);
}

inline QUATERNION QUATERNION::operator-(const QUATERNION& r) const
{
    return QUATERNION(q.x - r.q.x, q.y - r.q.y, q.z - r.q.z, qw - r.qw);
}

inline QUATERNION QUATERNION::operator-() const  // FIXME this doesn´t look right
{
    return QUATERNION(-q.x, -q.y, -q.z, qw);
}

inline QUATERNION QUATERNION::operator*(const float s) const
{
    return QUATERNION(q.x * s, q.y * s, q.z * s, qw * s);
}
inline float getNorm(const QUATERNION& q)
{
    return q.q.x * q.q.x + q.q.y * q.q.y + q.q.z * q.q.z + q.qw * q.qw;
}

inline void getAxisAngle(const QUATERNION& q, VECTOR3& axis, float& angle)
{
    angle       = 2.0f * acosf(q.qw);
    float sinus = 1.0f / (1.0f - q.qw * q.qw);  // FIXME:  normalization of axis neccessary ?
    axis        = q.q * sinus;
}

inline void setAxisAngle(QUATERNION& q, const VECTOR3& axis, float angle)
{
    q.q = axis;
    Normalize(q.q);
    angle *= 0.5f;
    q.q *= sinf(angle);
    q.qw = cosf(angle);
}

inline void getMatrix3(const QUATERNION& q, MATRIX3& mat)  // again, for unit quaternions only
{
    float t1 = 2.0f * q.q.x * q.q.x;
    float t2 = 2.0f * q.q.y * q.q.y;
    float t3 = 2.0f * q.q.z * q.q.z;

    mat.m[0] = 1.0f - t2 - t3;  // 1-2*(qy*qy + qz*qz)
    mat.m[4] = 1.0f - t1 - t3;
    mat.m[8] = 1.0f - t1 - t2;

    t1 = 2.0f * q.qw * q.q.z;
    t2 = 2.0f * q.q.x * q.q.y;

    mat.m[1] = t2 + t1;
    mat.m[3] = t2 - t1;

    t1 = 2.0f * q.qw * q.q.y;
    t2 = 2.0f * q.q.x * q.q.z;

    mat.m[2] = t2 - t1;
    mat.m[6] = t2 + t1;

    t1 = 2.0f * q.qw * q.q.x;
    t2 = 2.0f * q.q.y * q.q.z;

    mat.m[5] = t2 + t1;
    mat.m[7] = t2 - t1;
}
inline void getMatrix4(const QUATERNION& q, MATRIX4& mat)  // again, for unit quaternions only
{
    float t1 = 2.0f * q.q.x * q.q.x;
    float t2 = 2.0f * q.q.y * q.q.y;
    float t3 = 2.0f * q.q.z * q.q.z;

    mat.m[0]  = 1.0f - t2 - t3;  // 1-2*(qy*qy + qz*qz)
    mat.m[5]  = 1.0f - t1 - t3;
    mat.m[10] = 1.0f - t1 - t2;

    t1 = 2.0f * q.qw * q.q.z;
    t2 = 2.0f * q.q.x * q.q.y;

    mat.m[1] = t2 + t1;
    mat.m[4] = t2 - t1;

    t1 = 2.0f * q.qw * q.q.y;
    t2 = 2.0f * q.q.x * q.q.z;

    mat.m[2] = t2 - t1;
    mat.m[8] = t2 + t1;

    t1 = 2.0f * q.qw * q.q.x;
    t2 = 2.0f * q.q.y * q.q.z;

    mat.m[6] = t2 + t1;
    mat.m[9] = t2 - t1;
}

inline void getQuaternion(const MATRIX3& M, QUATERNION& q)
{
    float t = M.m[0] + M.m[4] + M.m[8];  // trace of matrix m

    q.qw = t = 0.5f * sqrtf(t + 1.0f);
    t        = 0.25f / t;

    q.q.x = (M.m[5] - M.m[7]) * t;
    q.q.y = (M.m[6] - M.m[2]) * t;
    q.q.z = (M.m[1] - M.m[3]) * t;
}

inline VECTOR3 QUATERNION::operator*(const VECTOR3& p) const
{
    // FIXME: there are other, faster ways
    MATRIX3 M;
    getMatrix3(*this, M);
    return M * p;
}

inline void slerpQuaternion(float t, const QUATERNION& q, const QUATERNION& r, QUATERNION& slerp)
{
    float sinalpha = (VECTOR4&)q * (VECTOR4&)r;  // dot = cos(alpha)

    if (sinalpha >= 1.0f) {
        slerp = q;
        return;
    }

    bool neg = sinalpha < 0.0f;

    sinalpha = sqrtf(1.0f - (sinalpha * sinalpha));

    float alpha = asinf(sinalpha);

    sinalpha = 1.0f / sinalpha;

    slerp = q * (sinf(alpha * (1.0f - t)) * sinalpha);

    if (!neg) {
        slerp = slerp + r * (sinf(alpha * t) * sinalpha);
    } else {
        slerp = slerp - r * (sinf(alpha * t) * sinalpha);
    }
}

inline void RotationQuaternionX(QUATERNION& q, float angle)
{
    angle *= 0.5f;
    q.q.x = sinf(angle);
    q.q.y = 0;
    q.q.z = 0;
    q.qw  = cosf(angle);
}
inline void RotationQuaternionY(QUATERNION& q, float angle)
{
    angle *= 0.5f;
    q.q.x = 0;
    q.q.y = sinf(angle);
    q.q.z = 0;
    q.qw  = cosf(angle);
}

inline void RotationQuaternionZ(QUATERNION& q, float angle)
{
    angle *= 0.5f;
    q.q.x = 0;
    q.q.y = 0;
    q.q.z = sinf(angle);
    q.qw  = cosf(angle);
}

inline void QuaternionFromAngles(QUATERNION& q, const VECTOR3& angles)
{
    QUATERNION temp;
    RotationQuaternionZ(q, angles.z);
    RotationQuaternionY(temp, angles.y);
    q = q * temp;
    RotationQuaternionX(temp, angles.x);
    q = q * temp;
}

#endif