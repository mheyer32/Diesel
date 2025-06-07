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
// Mesh.h: Schnittstelle für die Klasse CMesh.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESH_H__7F8B4520_D049_11D3_AF5D_0080AD17B2BB__INCLUDED_)
#define AFX_MESH_H__7F8B4520_D049_11D3_AF5D_0080AD17B2BB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER > 1000

#include <list>
#include <string>

#include <adt/NamedObject.h>
#include <adt/RefCount.h>
#include <adt/smartpointer.h>

#include <defs.h>

#include "VertexBuffer.h"

/** CMesh collects multiple vertexbuffers into one geometrical object.
    A mesh can be referenced an arbitrary number of entities.
    Still unsolved: how to proper animate such a mesh (different referencing entities,
    that all have different states of animation) */

class CMesh : public CRefCount, public CNamedObject
{

    friend class C3DSLoader;

public:
    typedef SmartPointer<CMesh>    SMARTPTR;
    typedef std::list<SMARTPTR>    SMARTPTRLIST;
    typedef SMARTPTRLIST::iterator SMARTPTRITERATOR;

    typedef std::list<CMesh*>  MESHLIST;
    typedef MESHLIST::iterator MESHITERATOR;

    CMesh();

    // batch all Faces into g_TNLStack
    virtual void batchFaces(int Frame = 0);

    void addVertexBuffer(CVertexBuffer* VBuffer);

    const CVertexBuffer::SMARTPTRARRAY& getVBufferList() const;
    void                                sortVertexBuffers();
    void                                mergeVertexBuffers();
    void                                optimize();

    // transform the vertices by m
    void transform(const MATRIX4& m, int Frame = 0);

    // FIXME: why virtual ?
    virtual void    translate(const VECTOR3& T, int Frame = 0);
    virtual VECTOR3 center(const VECTOR3& Centre = NullVector3, int Frame = 0);

    virtual void calcBoundingBox(int Frame = 0);
    virtual void setBoundingBox(const BBOX& newbox, int Frame = 0);
    // a virtual call may become a bottleneck in future, especially in large scenegraphs
    virtual const BBOX& getBoundingBox(int Frame = 0) const;

    // FIXME: use function instead of operator
    CMesh& operator+=(const CMesh& M);

    void freeMeshData();

    static const MESHLIST& GetMeshList();
    static CMesh*          FindMesh(const std::string& Name);

protected:
    virtual ~CMesh();

    CVertexBuffer::SMARTPTRARRAY m_vbuffers;
    BBOX                         m_bbox;

    // must not be a smartpointerlist!!!
    static MESHLIST MeshList;

private:
    /** currently undefined  - should vbuffers be cloned or AddRef´d ? */
    CMesh& operator=(const CMesh& m2);
};

#endif  // !defined(AFX_MESH_H__7F8B4520_D049_11D3_AF5D_0080AD17B2BB__INCLUDED_)
