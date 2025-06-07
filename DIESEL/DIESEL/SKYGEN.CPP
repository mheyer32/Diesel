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
#include <cassert>
#include <iostream>
#include "skygen.h"

namespace SkyGen
{
/** generate the vertices for a skyside, forward,up and right must be normalized */
void genSkyVerts(VECTOR3 forward, VECTOR3 up, VECTOR3 right, int tess, float skyboxsize, VECTOR3* vertices)
{
    float   dist   = skyboxsize / (float)(tess - 1);
    VECTOR3 origin = (forward - up - right) * (skyboxsize * 0.5f);
    up *= dist;
    right *= dist;

    // we start in the lower left corner of the skyside
    for (int y = 0; y < tess; ++y) {
        VECTOR3 vert = origin;
        for (int x = 0; x < tess; ++x) {
            *(vertices++) = (VECTOR3)vert;
            vert += right;
        }
        origin += up;
    }
}

void genPlainSkyTexcoords(int tess, int texturesize, VECTOR2* texcoords)
{
    float texstep = 1.0f / (float)(tess - 1);
    for (int y = 0; y < tess; ++y) {
        for (int x = 0; x < tess; ++x) {
            *(texcoords++) = VECTOR2((float)x * texstep, (float)y * texstep);
        }
    }
}

#define SPHERE_RADIUS 6.0f

void genSphereTexcoords(float height, VECTOR3* vertices, int num_vertices, VECTOR2* texcoords)
{
    // for each vertex of the skycube a ray is shot through the sphere to the vertex
    // we calculate the point where the sphere got hit and use that point for calculating
    // texturcoordinates for the vertex
    float temp = height * height - SPHERE_RADIUS * SPHERE_RADIUS;
    for (int v = 0; v < num_vertices; ++v) {
        VECTOR3 dir = vertices[v];

        float a = dir * dir;
        float b = height * dir.y / a;
        float c = temp / a;
        float t = sqrtf(b * b - c) - b;

        assert(t > 0.0f);

        float h = (dir.y * t + height) / SPHERE_RADIUS;
        h       = fabsf(h);
        if (h > 1.0f)
            h = 1.0f;  // unfortunately this happens rather often
        float lat = (0.5f * PI - asinf(h)) / (0.5f * PI);

        t = dir.x * dir.x + dir.z * dir.z;
        if (t > 0.000001f) {
            t = 1.0f / sqrtf(t);
        }
        texcoords[v].x = 1.0f - (0.5f + dir.x * lat * t * 2.0f);  //  is repeating the sky 4 times in both direction
        texcoords[v].y = 0.5f + dir.z * lat * t * 2.0f;

        // texcoords[v].x=0.5f+dir.x*t/SPHERE_RADIUS;
        // texcoords[v].y=0.5f+dir.z*t/SPHERE_RADIUS;

        // std::cout<<v<<"  t.x: "<<texcoords[v].x<<" t.y: "<<texcoords[v].y<<std::endl;
    }
}
/* 'indices' will be changed */
int genSkyIndices(const float* minmax, int tess, INDEX*& indices, int baseindex)
{
    int   tessminusone = tess - 1;
    float temp         = 0.5f * (float)tessminusone;
    int   mins         = (int)floorf((minmax[SKY_MINS] + 1.0f) * temp);
    int   maxs         = (int)ceilf((minmax[SKY_MAXS] + 1.0f) * temp);
    int   mint         = (int)floorf((minmax[SKY_MINT] + 1.0f) * temp);
    int   maxt         = (int)ceilf((minmax[SKY_MAXT] + 1.0f) * temp);

    // just for sure bound the values
    if (maxs > tessminusone)
        maxs = tessminusone;
    if (maxt > tessminusone)
        maxt = tessminusone;
    if (mins < 0)
        mins = 0;
    if (mint < 0)
        mint = 0;

    baseindex += mint * tess + mins;

    for (int y = mint; y < maxt; ++y) {
        int rowindex = baseindex;
        for (int x = mins; x < maxs; ++x) {
            indices[0] = rowindex;
            indices[4] = indices[1] = rowindex + 1;
            indices[3] = indices[2] = rowindex + tess;
            indices[5]              = indices[3] + 1;
            indices += 6;
            rowindex++;
        }
        baseindex += tess;
    }
    return (maxt - mint) * (maxs - mins) * 6;
}

}  // namespace SkyGen