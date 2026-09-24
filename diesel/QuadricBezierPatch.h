/*
This file is part of Diesel
(c) 2002-2026 by Mathias Heyer
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
#ifndef QUADRICBEZIERPATCH_H
#define QUADRICBEZIERPATCH_H

#include "Color.h"

namespace Bezier
{
// FIXME: somday there will be a special bezier curve/surface class containing this
template <class CPTYPE>
CPTYPE evalQuadricBezier(float t, const CPTYPE& p0, const CPTYPE& p1, const CPTYPE& p2)
{
    const float qt = t * t;
    const float dt = 2.0f * t;
    return (p0 * (1.0f - dt + qt)) + (p1 * (dt - 2.0f * qt)) + (p2 * qt);

    /*	const float weight1= 1.0f-sinf(t*PI *0.5f);
        const float weight3= 1.0f-cosf(t*PI *0.5f);
        const float weight2= 1.0f-weight1-weight3;

        return (p0*weight1)+(p1*weight2)+(p2*weight3);*/
}

COLOR evalQuadricBezier(float t, const COLOR& c0, const COLOR& c1, const COLOR& c2)
{
    VECTOR4 p0;
    p0 = c0;
    VECTOR4 p1;
    p1 = c1;
    VECTOR4 p2;
    p2 = c2;

    return COLOR(evalQuadricBezier(t, p0, p1, p2));
}

template <class CPTYPE>
CPTYPE evalQuadricBezierDerivative(float t, const CPTYPE& p0, const CPTYPE& p1, const CPTYPE& p2)
{
    return p0 * (t - 1.0f) + p1 * (1.0f - 2.0f * t) + p2 * t;
}

template <class CPTYPE>
int flatnessTest(float maxflat, const CPTYPE& p0, const CPTYPE& p1, const CPTYPE& p2)
{

    CPTYPE d = p2 - p0;
    float  l = Length(d);
    if (l == 0)
        return 0;
    d /= l;
    CPTYPE w    = d * ((p1 - p0) * d);
    float  dist = Length(p1 - p0 - w);
    dist        = 10.f * dist * dist / Length(p2 - p0);
    if (fabs(dist) <= maxflat)
        return 0;

    CPTYPE v1 = (p1 + p0) * 0.5f;
    CPTYPE v2 = (p2 + p1) * 0.5f;
    CPTYPE v3 = (v1 + v2) * 0.5f;

    return 1 + __max(flatnessTest(maxflat, p0, v1, v3), flatnessTest(maxflat, v3, v2, p2));
}

template <class P>
void evalQuadricBezierPatchNormals(const P* ctrl_points, int num_cpx, int num_cpy, P* dest, int tess_x, int tess_y)
{
    assert(ctrl_points);
    assert(dest);
    assert(tess_x);
    assert(tess_y);

    int num_patches_x = num_cpx / 2;
    int num_patches_y = num_cpy / 2;

    int dstpitch = num_patches_x * tess_x + 1;

    float step_t = 1.0f / (float)(tess_y);
    float step_s = 1.0f / (float)(tess_x);

    for (int patch_y = 0; patch_y < num_patches_y; ++patch_y) {
        // last patch has one more row
        int num_tess_y = (patch_y < num_patches_y - 1) ? tess_y : tess_y + 1;

        for (int patch_x = 0; patch_x < num_patches_x; ++patch_x) {
            // last patch has one more column
            int num_tess_x = (patch_x < num_patches_x - 1) ? tess_x : tess_x + 1;

            int i11 = (patch_y * num_cpx + patch_x) * 2;
            int i12 = i11 + num_cpx;
            int i13 = i12 + num_cpx;
            // current 3x3 patch controlpoints
            const P& p11 = ctrl_points[i11];
            const P& p12 = ctrl_points[i12];
            const P& p13 = ctrl_points[i13];
            ++i11;
            ++i12;
            ++i13;
            const P& p21 = ctrl_points[i11];
            const P& p22 = ctrl_points[i12];
            const P& p23 = ctrl_points[i13];
            ++i11;
            ++i12;
            ++i13;
            const P& p31 = ctrl_points[i11];
            const P& p32 = ctrl_points[i12];
            const P& p33 = ctrl_points[i13];

            P* patchstart = dest + patch_y * tess_y * dstpitch + patch_x * tess_x;
            // first tangents in s-direction
            P*    tempdest = patchstart;
            float t        = 0.0f;
            for (int y = 0; y < num_tess_y; ++y, t += step_t) {

                P p1 = evalQuadricBezier(t, p11, p12, p13);
                P p2 = evalQuadricBezier(t, p21, p22, p23);
                P p3 = evalQuadricBezier(t, p31, p32, p33);

                float s = 0.0f;
                for (int x = 0; x < num_tess_x; ++x, s += step_s) {
                    tempdest[x] = evalQuadricBezierDerivative(s, p1, p2, p3);
                }
                tempdest += dstpitch;
            }
            // now in t=direction
            float s = 0.0f;
            for (int x = 0; x < num_tess_x; ++x, s += step_s) {

                P p1 = evalQuadricBezier(s, p11, p21, p31);
                P p2 = evalQuadricBezier(s, p12, p22, p32);
                P p3 = evalQuadricBezier(s, p13, p23, p33);

                P*    tempdest = patchstart + x;
                float t        = 0.0f;
                for (int y = 0; y < num_tess_y; ++y, t += step_t) {
                    *tempdest = *tempdest ^ evalQuadricBezierDerivative(t, p1, p2, p3);
                    // sometimes all three controlpoints are at the same coordiante, so no valid normal can be generated
                    if (*tempdest != NullVector3)
                        Normalize(*tempdest);
                    tempdest += dstpitch;
                }
            }
        }
    }
}

template <class P>
void evalQuadricBezierPatch(const P* ctrl_points, int num_cpx, int num_cpy, P* dest, int tess_x, int tess_y)
{
    assert(ctrl_points);
    //	assert(num_cpx%2);
    //	assert(num_cpy%2);
    assert(dest);
    assert(tess_x);
    assert(tess_y);

    int num_patches_x = num_cpx / 2;
    int num_patches_y = num_cpy / 2;

    int dstpitch = num_patches_x * tess_x + 1;

    float step_t = 1.0f / (float)(tess_y);
    float step_s = 1.0f / (float)(tess_x);

    for (int patch_y = 0; patch_y < num_patches_y; ++patch_y) {
        // last patch has one more row
        int num_tess_y = (patch_y < num_patches_y - 1) ? tess_y : tess_y + 1;

        for (int patch_x = 0; patch_x < num_patches_x; ++patch_x) {
            // last patch has one more column
            int num_tess_x = (patch_x < num_patches_x - 1) ? tess_x : tess_x + 1;

            int i11 = (patch_y * num_cpx + patch_x) * 2;
            int i12 = i11 + num_cpx;
            int i13 = i12 + num_cpx;
            // current 3x3 patch controlpoints
            const P& p11 = ctrl_points[i11];
            const P& p12 = ctrl_points[i12];
            const P& p13 = ctrl_points[i13];
            ++i11;
            ++i12;
            ++i13;
            const P& p21 = ctrl_points[i11];
            const P& p22 = ctrl_points[i12];
            const P& p23 = ctrl_points[i13];
            ++i11;
            ++i12;
            ++i13;
            const P& p31 = ctrl_points[i11];
            const P& p32 = ctrl_points[i12];
            const P& p33 = ctrl_points[i13];

            P*    tempdest = dest + patch_y * tess_y * dstpitch + patch_x * tess_x;
            float t        = 0.0f;
            for (int y = 0; y < num_tess_y; ++y, t += step_t) {

                P p1 = evalQuadricBezier(t, p11, p12, p13);
                P p2 = evalQuadricBezier(t, p21, p22, p23);
                P p3 = evalQuadricBezier(t, p31, p32, p33);

                float s = 0.0f;
                for (int x = 0; x < num_tess_x; ++x, s += step_s) {
                    tempdest[x] = evalQuadricBezier(s, p1, p2, p3);
                }
                tempdest += dstpitch;
            }
        }
    }
}
}  // namespace Bezier

#endif