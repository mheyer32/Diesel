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

#include <iostream>
#include "VBServices.h"
#include "VertexBuffer.h"
#include "Shader.h"

void VBServices::createGridIndices(CVertexBuffer& vbuffer, int xdim, int ydim)
{
    int indices_per_row = 6 * (xdim - 1);

    vbuffer.AllocArrays(INDEXARRAY, 0, indices_per_row * (ydim - 1));

    INDEX index  = 0;
    INDEX vertex = xdim;  // in erster Zeile anfangen

    for (int y = 1; y < ydim; y++) {
        for (int x = 0; x < xdim - 1; x++) {
            vbuffer.indices[index++] = vertex;
            vbuffer.indices[index++] = vertex + 1;
            vbuffer.indices[index++] = vertex - xdim;

            vbuffer.indices[index++] = vertex - xdim;
            vbuffer.indices[index++] = vertex + 1;
            vbuffer.indices[index++] = vertex + 1 - xdim;

            vertex++;
        }
        vertex++;
    }

    vbuffer.num_indices = index;
    vbuffer.mode        = GL_TRIANGLES;
}

void VBServices::preTransformBillboard(CVertexBuffer& vb)
{
    VECTOR3* vertices = vb.vertices[0];
    VECTOR3  t, t2, t3, t4;
    MATRIX3  M = IdentityMatrix3;

    for (int v = 0; v < vb.num_indices; v += 6) {
        t  = vertices[vb.indices[v]];
        t2 = vertices[vb.indices[v + 1]];
        t3 = vertices[vb.indices[v + 2]];
        // t4=vertices[vb.indices[v+5]];
        // t4=(t+t2+t3+t4)*0.25f;
        t4 = (t2 + t3) * 0.5f;

        t2 = t2 - t;
        Normalize(t2);
        t3 = t - t3;
        Normalize(t3);
        t = t2 ^ t3;

        (VECTOR3&)M.a[0] = t2;
        (VECTOR3&)M.a[1] = t3;
        (VECTOR3&)M.a[2] = t;
        Transpose3(M);

        vertices[vb.indices[v]]     = t4 + (M * (vertices[vb.indices[v]] - t4));
        vertices[vb.indices[v + 1]] = t4 + (M * (vertices[vb.indices[v + 1]] - t4));
        vertices[vb.indices[v + 2]] = t4 + (M * (vertices[vb.indices[v + 2]] - t4));
        vertices[vb.indices[v + 5]] = t4 + (M * (vertices[vb.indices[v + 5]] - t4));
    }
}

void VBServices::preTransformAxisBillboard(CVertexBuffer& vb)
{
    VECTOR3  t1, t2, t3;
    VECTOR3* vertices = vb.vertices[0];
    INDEX*   indices  = vb.indices;

    int   i;
    int   vert = 0;
    float l1, l2, l3;
    for (int v = 0; v < vb.num_indices; v += 6) {
        // lengthes of all edges
        l1 = Length(vertices[vb.indices[v + 1]] - vertices[vb.indices[v]]);
        l2 = Length(vertices[vb.indices[v + 2]] - vertices[vb.indices[v]]);
        l3 = Length(vertices[vb.indices[v + 2]] - vertices[vb.indices[v + 1]]);

        // find the longest edge (diagonal)
        int rotate = 0;
        if ((l3 > l2 && l3 > l1)) {
            if (l1 > l2)
                rotate = 1;
        } else if ((l2 > l3 && l2 > l1)) {
            std::cout << "found axis billboard with wrong index-orientation1" << std::endl;
            rotate = 1;
            if (l3 > l1)
                rotate = 2;
        } else if ((l1 > l3 && l1 > l2)) {
            std::cout << "found axis billboard with wrong index-orientation2" << std::endl;
            rotate = 1;
            if (l2 > l3)
                rotate = 2;
        }

        for (int r = 0; r < rotate; ++r) {
            i              = indices[v];
            indices[v]     = indices[v + 2];
            indices[v + 2] = indices[v + 3] = indices[v + 5];
            indices[v + 5]                  = indices[v + 1];
            indices[v + 1] = indices[v + 4] = i;
        }
    }
}

void VBServices::createCompleteGrid(CVertexBuffer& vbuffer, float xsize, float ysize, int xdim, int ydim,
                                    int num_vertexsets)
{

    xsize /= (float)xdim - 1;
    ysize /= (float)ydim - 1;
    float txsize = 1.0f / ((float)xdim - 1);
    float tysize = 1.0f / ((float)ydim - 1);

    int arrays;
    if (vbuffer.getShader()) {
        arrays = vbuffer.getShader()->getUsedArrays() & ~(INDEXARRAY | LMCOORDARRAY | COLORARRAY);
    } else {
        arrays = VERTEXARRAY;
    }

    vbuffer.AllocArrays(arrays, xdim * ydim, 0, num_vertexsets);
    vbuffer.num_vertices = xdim * ydim;

    for (int set = 0; set < num_vertexsets; ++set) {
        int   elem;
        float vy = 0;
        for (int y = 0; y < ydim; ++y) {
            float vx = 0;

            for (int x = 0; x < xdim; ++x) {
                elem = y * xdim + x;
                if (arrays & VERTEXARRAY) {
                    vbuffer.vertices[set][elem] = VECTOR3(vx, 0, vy);
                }

                if (arrays & NORMALARRAY) {
                    vbuffer.normals[set][elem] = VECTOR3(0, 1, 0);
                }
                vx += xsize;
            }
            vy += ysize;
        }
    }

    if (arrays & TEXCOORDARRAY) {
        int   elem;
        float ty = 0;
        for (int y = 0; y < ydim; ++y) {
            float tx = 0;
            for (int x = 0; x < xdim; ++x) {
                elem = y * xdim + x;

                if (arrays & TEXCOORDARRAY) {
                    vbuffer.texcoords[elem] = VECTOR2(tx, ty);
                }
                tx += txsize;
            }
            ty += tysize;
        }
    }

    createGridIndices(vbuffer, xdim, ydim);
}

extern void VBServices::reverseIndices(CVertexBuffer& vbuffer)
{
    INDEX temp;
    for (int i = 0, j = vbuffer.num_indices - 1; i < j; i++, j--) {
        temp               = vbuffer.indices[i];
        vbuffer.indices[i] = vbuffer.indices[j];
        vbuffer.indices[j] = temp;
    }
}