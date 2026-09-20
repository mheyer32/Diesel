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
// VertexBuffer.h: Schnittstelle für die Klasse CVertexBuffer.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VERTEXBUFFER_H__0D3B72AF_A047_4C1B_9F12_1DC6D14ACDD4__INCLUDED_)
#define AFX_VERTEXBUFFER_H__0D3B72AF_A047_4C1B_9F12_1DC6D14ACDD4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER > 1000

#include <adt/RefCount.h>
#include <math/Vector2.h>
#include <math/Vector3.h>
#include <math/Vector4.h>
#include <list>
#include <vector>

#include <adt/SmartPointer.h>
#include "EngineTypes.h"
#include "GeometryMemManager.h"
#include "VertexArrays.h"
#include "Shader.h"
#include "Texture.h"

// CVertexBuffers primary goal is to store equally textured primitives in a compact fashion
// the member variables are intentionally made public to allow maximum flexible access to data
// never mess with the pointers! always use Alloc() for allocating data!!!!

class CVertexBuffer : public CRefCount  // Vertexbuffers can be referenced by more than just one mesh
{

public:
    typedef SmartPointer<CVertexBuffer> SMARTPTR;
    typedef std::list<SMARTPTR>         SMARTPTRLIST;
    typedef SMARTPTRLIST::iterator      SMARTPTRITERATOR;

    typedef std::vector<SMARTPTR> SMARTPTRARRAY;

    typedef std::list<CVertexBuffer*> VBUFLIST;
    typedef VBUFLIST::iterator        VBUFITERATOR;

    typedef std::vector<CVertexBuffer*> VBUFARRAY;

    enum VBCOMBINE_RVAL
    {
        VBCOMBINE_OK,             // call succeeded
        VBCOMBINE_MODE_MISMATCH,  // mismatching MODE of both vbuffers
        VBCOMBINE_TOOBIG,         // "this" has not enough space left
        VBCOMBINE_NOARRAYSCOPIED  // nothing was copied due to arrays-masking
    };

    CVertexBuffer();

    CVertexBuffer(MEMTYPE type, int arrays, int num_vertices, int num_indices = 0, int num_vertexsets = 1);

    /** Allocates the given arrays (bitcombination in arays, see SHADER.H)
        num_vertexsets	is only used if new vertices are allocated,
        num_indices is only used if a new indexarray is allocated
        */
    bool AllocArrays(MEMTYPE type, int arrays, int num_vertices, int num_indices = 0, int num_vertexsets = 1);

    // for compatibility , memtype will default to MT_DYNAMIC
    bool AllocArrays(int arrays, int num_vertices, int num_indices = 0, int num_vertexsets = 1);

    /* free the given arrays, but only thos which were actually allocated with AllocArrays */
    void FreeArrays(int Arrays = ALL_ARRAYS);

    /* get/set the curent set of vertices and normals
        will be used one day for meshanimations */
    int  getActiveSet() const { return activeset; };
    void setActiveSet(const int active);
    int  getNumVertexSets() const { return num_vertexsets; }

    /** only use these if you already have allocated memory with AllocArrays() */
    void setIndices(INDEX* new_indices, int new_num_indices);
    void setLMCoords(VECTOR2* new_lmcoords, int num_lmcoords);
    void setColors(COLOR* new_colors, int num_colors);
    void setTexCoords(VECTOR2* new_texcoords, int num_texcoords);
    void setNormals(VECTOR3* new_normals, int num_normals);
    void setVertices(VECTOR3* new_vertices, int new_num_vertices);

    /** create normals by averraging all normales aof all (triangle!) faces the vertices belong to
    FIXME: throw into VBServices namespace ? */
    void createNormals();

    /** try to reorder the indices into strip-order, the vertices will be reordered in a way
        they are indexed in a more linear fashion (cache friendly) */
    void OptimizeVBuffer();

    /** set the shader that is used for rendering */
    void setShader(CShader* newshader = NULL);

    /** set the lightmap that is used for rendering */
    void setLightmap(CTexture* Lightmap = NULL);

    /** concat "vb2" on the end of "this"
        in order to be succesful, "this" must have got enough space left and both vbuffers
        must have the same mode
        FIXME: interface is somewhat cumbersome and error prone
    */
    VBCOMBINE_RVAL combine(const CVertexBuffer& vb2, int arrays = ALL_ARRAYS);

    inline VBCOMBINE_RVAL canCombine(const CVertexBuffer& vb2) const
    {
        if (num_vertices + vb2.num_vertices > max_num_vertices || num_indices + vb2.num_indices > max_num_indices)
            return VBCOMBINE_TOOBIG;  // "this" has not enough space left
        // FIXME: need to check mode ?
        return VBCOMBINE_OK;
    }

    inline void fillRenderInfo(RENDERINFO& info) const
    {
        info.vbuffer      = (CVertexBuffer*)this;
        info.shader       = shader;
        info.sortkey      = sortkey;
        info.num_features = 0;
    }

    inline int       getSortkey() const { return sortkey; }
    inline CShader*  getShader() const { return shader; }
    inline CTexture* getLightmap() const { return lightmap; }

    inline int getMaxNumVertices() const { return max_num_vertices; };
    inline int getMaxNumIndices() const { return max_num_indices; };

    /* left public for fast access
       don´t mess with it! Never replace any arrays that were previously allocated with
       "AllocArrays()" but weren´t given free with "FreeArrays()"
       FIXME: still need for public access? introduce accessors ? */
    int mode;  // indexd tris, quads, or whatever

    VECTOR3** vertices;
    VECTOR3** normals;
    VECTOR2*  texcoords;
    VECTOR2*  lmcoords;
    COLOR*    colors;
    INDEX*    indices;

    int used_arrays;
    int num_vertices;  // how many vertices it actually holds
    int num_indices;   // how many indices its actually holds

protected:
    // only allow destruction via refcounting, later there will be shared vertexbuffer
    virtual ~CVertexBuffer();  // virtual is very important, because of RefCount´ing

    // hide copy operator
    // FIXME: also hide copy constructor ? (applies to some other classes, too)
    CVertexBuffer& operator=(const CVertexBuffer& vb2);

    void calcSortkey();
    void initVars();

    int arrays_allocated;  // Arrays were allocated with AllocArrays(), thus can be deleted in dtor

    int num_vertexsets;    // how many vertexsets, eg. meshframes
    int max_num_vertices;  // how many vertices the buffer can hold
    int max_num_indices;   // how many indices the buffer can hold

    CShader::SMARTPTR  shader;
    CTexture::SMARTPTR lightmap;

    int sortkey;
    int activeset;  // which vertexset is used for combining and filling renderinfos
};

#endif  // !defined(AFX_VERTEXBUFFER_H__0D3B72AF_A047_4C1B_9F12_1DC6D14ACDD4__INCLUDED_)
