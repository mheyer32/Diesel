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

#include <defs.h>
#include "Shader.h"
#include "Stripper.h"
#include "Texture.h"
#include "VertexBuffer.h"

#include <cassert>
#include <iostream>

#include <memorytracker.h>

using namespace std;

CVertexBuffer::CVertexBuffer()
{
    initVars();
}

CVertexBuffer::CVertexBuffer(MEMTYPE type, int Arrays, int num_Vertices, int num_Indices, int num_Vertexsets)
{
    initVars();

    AllocArrays(type, Arrays, num_Vertices, num_Indices, num_Vertexsets);
}

CVertexBuffer::~CVertexBuffer()
{
    FreeArrays();
}

void CVertexBuffer::initVars()
{
    mode = GL_TRIANGLES;  // indexd tris, quads, or whatever

    vertices  = NULL;
    normals   = NULL;
    texcoords = NULL;
    lmcoords  = NULL;
    colors    = NULL;
    indices   = NULL;

    max_num_vertices = 0;  // how many vertices the buffer can hold
    num_vertices     = 0;  // how many vertices it actually holds
    max_num_indices  = 0;  // how many indices the buffer can hold
    num_indices      = 0;  // how many indices its actually holds
    num_vertexsets   = 0;  // how many vertexsets, eg. meshframes

    used_arrays      = NO_ARRAY;  // arrays in use
    arrays_allocated = 0;
    activeset        = 0;

    sortkey = 0;
}
// does not free already allocated arrays!
// FIXME: should this give an error ?

bool CVertexBuffer::AllocArrays(MEMTYPE type, int Arrays, int num_Vertices, int num_Indices, int num_Vertexsets)
{
    Arrays &= ALL_ARRAYS;

    GeometryMemManager* mmgr = GeometryMemManager::Instance();

    if (Arrays & VERTEXARRAY) {
        assert(num_Vertices);
        assert(num_Vertexsets);
        vertices         = new VECTOR3*[num_Vertexsets];
        max_num_vertices = num_Vertices;
        num_vertexsets   = num_Vertexsets;
    }
    if (Arrays & NORMALARRAY) {
        assert(num_vertexsets);
        normals = new VECTOR3*[num_vertexsets];
    }

    if (Arrays & VERTEXARRAY || Arrays & NORMALARRAY) {
        assert(max_num_vertices);
        for (int i = 0; i < num_vertexsets; i++) {
            if (Arrays & VERTEXARRAY) {
                vertices[i] = (VECTOR3*)mmgr->alloc(type, max_num_vertices * sizeof(VECTOR3));
            }

            if (Arrays & NORMALARRAY) {
                normals[i] = (VECTOR3*)mmgr->alloc(type, max_num_vertices * sizeof(VECTOR3));
            }
        }
    }
    if (Arrays & TEXCOORDARRAY) {
        assert(max_num_vertices);
        texcoords = (VECTOR2*)mmgr->alloc(type, max_num_vertices * sizeof(VECTOR2));
    }
    if (Arrays & COLORARRAY) {
        assert(max_num_vertices);
        colors = (COLOR*)mmgr->alloc(type, max_num_vertices * sizeof(COLOR));
    }
    if (Arrays & LMCOORDARRAY) {
        assert(max_num_vertices);
        lmcoords = (VECTOR2*)mmgr->alloc(type, max_num_vertices * sizeof(VECTOR2));
    }
    if (Arrays & INDEXARRAY) {
        max_num_indices = num_Indices;
        assert(max_num_indices);
        // indices remain in system memory
        indices     = (INDEX*)mmgr->alloc(MT_SYSMEM, max_num_indices * sizeof(INDEX));
        num_indices = 0;
    }

    arrays_allocated |= Arrays;
    used_arrays |= Arrays;
    return true;
}

bool CVertexBuffer::AllocArrays(int arrays, int num_Vertices, int num_Indices, int num_Vertexsets)
{
    return AllocArrays(MT_DYNAMIC, arrays, num_Vertices, num_Indices, num_Vertexsets);
}

void CVertexBuffer::FreeArrays(int Arrays)
{
    GeometryMemManager* mmgr = GeometryMemManager::Instance();

    Arrays &= arrays_allocated;

    if ((Arrays & VERTEXARRAY) && vertices) {
        for (int i = 0; i < num_vertexsets; i++) {
            mmgr->free(vertices[i]);
        }
        KILLARRAY(vertices);
    }
    if ((Arrays & NORMALARRAY) && normals) {
        for (int i = 0; i < num_vertexsets; i++) {
            mmgr->free(normals[i]);
        }
        KILLARRAY(normals);
    }
    if (Arrays & COLORARRAY) {
        mmgr->free(colors);
    }

    if (Arrays & TEXCOORDARRAY) {
        mmgr->free(texcoords);
    }
    if (Arrays & LMCOORDARRAY) {
        mmgr->free(lmcoords);
    }

    if (Arrays & INDEXARRAY) {
        mmgr->free(indices);
    }

    arrays_allocated &= ~Arrays;
    used_arrays &= ~Arrays;
}
void CVertexBuffer::OptimizeVBuffer()
{
    int     i, i2, oldindex, newindex, index2;
    VECTOR3 tvert;
    VECTOR2 ttco;
    COLOR   tcol;

    if (num_indices < 3) {
        cout << "OptimizeVBuffer for an empty buffer???!!!!" << endl;
        return;
    }

    ArrangeForStrips(indices, num_indices, STRIPOPT_SORTONCE | STRIPOPT_SORTEVERYTIME);  //|STRIPOPT_SORTEVERYTIME

    // now optimize for linear access of the vertices
    for (i = newindex = 0; i < num_indices; i++) {
        oldindex = indices[i];
        if (oldindex > newindex) {
            int f;
            // oldindex ist noch "unbehandelt"
            for (f = 0; f < num_vertexsets; f++) {
                tvert                 = vertices[f][oldindex];
                vertices[f][oldindex] = vertices[f][newindex];
                vertices[f][newindex] = tvert;
            }
            if (normals != NULL) {
                for (f = 0; f < num_vertexsets; f++) {
                    tvert                = normals[f][oldindex];
                    normals[f][oldindex] = normals[f][newindex];
                    normals[f][newindex] = tvert;
                }
            }
            if (texcoords) {
                ttco                = texcoords[oldindex];
                texcoords[oldindex] = texcoords[newindex];
                texcoords[newindex] = ttco;
            }
            if (colors) {
                tcol             = colors[oldindex];
                colors[oldindex] = colors[newindex];
                colors[newindex] = tcol;
            }
            if (lmcoords) {
                ttco               = lmcoords[oldindex];
                lmcoords[oldindex] = lmcoords[newindex];
                lmcoords[newindex] = ttco;
            }

            for (i2 = i; i2 < num_indices; i2++) {
                index2 = indices[i2];
                if (index2 == newindex) {
                    indices[i2] = oldindex;
                } else if (index2 == oldindex) {
                    indices[i2] = newindex;
                }
            }
        }
        if (oldindex >= newindex)
            newindex++;  // in diesem Fall ist oldindex schon "richtig" und bleibt so
    }
}

void CVertexBuffer::setShader(CShader* newshader)
{
    shader = newshader;
    calcSortkey();
}

void CVertexBuffer::setLightmap(CTexture* Lightmap)
{
    lightmap = Lightmap;
    calcSortkey();
}

// FIXME: rethink combining of vbuffers with more than one vertexset
CVertexBuffer::VBCOMBINE_RVAL CVertexBuffer::combine(const CVertexBuffer& vb2, int arrays)
{
    if ((vb2.num_vertices + num_vertices > max_num_vertices) || (vb2.num_indices + num_indices > max_num_indices)) {
        return VBCOMBINE_TOOBIG;
    }
    if (vb2.mode != mode)
        return VBCOMBINE_MODE_MISMATCH;

    arrays &= arrays_allocated & vb2.arrays_allocated;
    if (!(arrays & ~INDEXARRAY))
        return VBCOMBINE_NOARRAYSCOPIED;

    if (arrays & VERTEXARRAY) {
        assert(vertices && vb2.vertices);
        assert(vertices[activeset] && vb2.vertices[vb2.activeset]);
        memcpy(vertices[activeset] + num_vertices, vb2.vertices[vb2.activeset], vb2.num_vertices * sizeof(VECTOR3));
    }
    if (arrays & NORMALARRAY) {
        assert(normals && vb2.normals);
        assert(normals[activeset] && vb2.normals[vb2.activeset]);
        memcpy(normals[activeset] + num_vertices, vb2.normals[vb2.activeset], vb2.num_vertices * sizeof(VECTOR3));
    }
    if (arrays & TEXCOORDARRAY) {
        assert(texcoords && vb2.texcoords);
        memcpy(texcoords + num_vertices, vb2.texcoords, vb2.num_vertices * sizeof(VECTOR2));
    }
    if (arrays & LMCOORDARRAY) {
        assert(lmcoords && vb2.lmcoords);
        memcpy(lmcoords + num_vertices, vb2.lmcoords, vb2.num_vertices * sizeof(VECTOR2));
    }
    if (arrays & COLORARRAY) {
        assert(colors && vb2.colors);
        memcpy(colors + num_vertices, vb2.colors, vb2.num_vertices * sizeof(COLOR));
    }

    if (arrays & INDEXARRAY) {
        assert(indices && vb2.indices);
        INDEX *tindices = indices + num_indices, *tindices2 = vb2.indices;
        for (int i = 0; i < vb2.num_indices; ++i) {
            tindices[i] = tindices2[i] + num_vertices;
        }
        num_indices += vb2.num_indices;
    }
    num_vertices += vb2.num_vertices;

    used_arrays |= arrays;

    return VBCOMBINE_OK;
}

void CVertexBuffer::calcSortkey()
{
    sortkey = 0;
    if (shader) {
        sortkey = ((int)shader->getSort());
        sortkey |= ((int)shader->getShaderId() & 0x3FF) << 18;
    }
    if (lightmap) {
        sortkey |= ((int)lightmap->getTextureId() & 0x3FF) << 8;
    }
}

void CVertexBuffer::createNormals()
{
    if (indices == NULL)
        return;

    if (normals == NULL) {
        GeometryMemManager* mmgr = GeometryMemManager::Instance();
        AllocArrays(mmgr->getType(vertices[0]), NORMALARRAY, num_vertices);
    }

    // Normalen erzeugen
    VECTOR3 v1, v2, v3;
    for (int f = 0; f < num_vertexsets; f++) {
        ZeroMemory(normals[f], num_vertices * sizeof(VECTOR3));
        for (int i = 0; i < num_indices; i += 3) {
            v1 = vertices[f][indices[i]];
            v2 = vertices[f][indices[i + 1]];
            v3 = vertices[f][indices[i + 2]];
            v2 -= v1;
            v3 -= v1;
            v1 = v2 ^ v3;
            normals[f][indices[i]] += v1;
            normals[f][indices[i + 1]] += v1;
            normals[f][indices[i + 2]] += v1;
        }
        for (int v = 0; v < num_vertices; v++) {
            Normalize(normals[f][v]);
        }
    }
}

void CVertexBuffer::setActiveSet(const int active)
{
    if (active >= num_vertexsets)
        throw CException("CVertexBuffer::setActiveSet()  invalid activeset");
    activeset = active;
}

void CVertexBuffer::setVertices(VECTOR3* new_vertices, int new_num_vertices)
{
    assert(new_vertices);
    assert(vertices);
    assert(vertices[activeset]);

    if (new_num_vertices > max_num_vertices)
        throw CException("CVertexBuffer::setVertices() given vertex count exceeds buffer");

    num_vertices = new_num_vertices;
    memcpy(vertices[activeset], new_vertices, num_vertices * sizeof(VECTOR3));
}

void CVertexBuffer::setNormals(VECTOR3* new_normals, int num_normals)
{
    assert(new_normals);
    assert(normals);
    assert(normals[activeset]);

    if (num_normals > max_num_vertices)
        throw CException("CVertexBuffer::setNormals() given normal count exceeds buffer");

    // FIXME: what if num_normals is different from num_vertices ?
    memcpy(normals[activeset], new_normals, num_normals * sizeof(VECTOR3));
}

void CVertexBuffer::setTexCoords(VECTOR2* new_texcoords, int num_texcoords)
{
    assert(new_texcoords);
    assert(texcoords);

    if (num_texcoords > max_num_vertices)
        throw CException("CVertexBuffer::setTexCoords() given texcoord count exceeds buffer");

    memcpy(texcoords, new_texcoords, num_texcoords * sizeof(VECTOR2));
}
void CVertexBuffer::setLMCoords(VECTOR2* new_lmcoords, int num_lmcoords)
{
    assert(new_lmcoords);
    assert(lmcoords);

    if (num_lmcoords > max_num_vertices)
        throw CException("CVertexBuffer::setLMCoords() given lmcoord count exceeds buffer");

    memcpy(lmcoords, new_lmcoords, num_lmcoords * sizeof(VECTOR2));
}

void CVertexBuffer::setColors(COLOR* new_colors, int num_colors)
{
    assert(new_colors);
    assert(colors);

    if (num_colors > max_num_vertices)
        throw CException("CVertexBuffer::setColors() given color count exceeds buffer");

    memcpy(colors, new_colors, num_colors * sizeof(COLOR));
}

void CVertexBuffer::setIndices(INDEX* new_indices, int new_num_indices)
{
    assert(new_indices);
    assert(indices);

    if (new_num_indices > max_num_indices)
        throw CException("CVertexBuffer::setIndices() given index count exceeds buffer");

    num_indices = new_num_indices;
    memcpy(indices, new_indices, num_indices * sizeof(INDEX));
}
