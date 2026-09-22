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
// Entity.h: Schnittstelle für die Klasse CEntity.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ENTITY_H__7F8B4521_D049_11D3_AF5D_0080AD17B2BB__INCLUDED_)
#define AFX_ENTITY_H__7F8B4521_D049_11D3_AF5D_0080AD17B2BB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER > 1000

#include <list>
#include <string>

#include <adt/NamedObject.h>
#include <adt/RefCount.h>
#include <adt/SmartPointer.h>
#include <geometry/AABB.h>

#include "Camera.h"  // only for ALL_PLANES - fix that ?
#include "Mesh.h"
#include "Transformation.h"

////////////////////////////////////////////////////////////////////////

class CEntity : public CRefCount, public CNamedObject
{

public:
    typedef SmartPointer<CEntity>  SMARTPTR;
    typedef std::list<SMARTPTR>    SMARTPTRLIST;
    typedef SMARTPTRLIST::iterator SMARTPTRITERATOR;

    typedef std::list<CEntity*>  ENTITYLIST;
    typedef ENTITYLIST::iterator ENTITYITERATOR;

    CEntity(CMesh* Mesh = NULL);
    virtual ~CEntity();

    // check entity against frustum, if visible add its faces to g_TNLStack
    virtual void cullEntity(int testplanes = ALLPLANES);

    CEntity*         getParent() const;
    int              getNumChildren() const;
    CEntity*         getChild(unsigned int childNum);
    virtual void     removeChild(CEntity* Entity);
    virtual void     addChild(CEntity* newEntity);
    virtual void     addChild(CEntity::ENTITYLIST& children);
    virtual CEntity* clone();

    void   addMesh(CMesh* newMesh);
    CMesh* getMesh(int lod = 0);

    // FIXME: does not belong to the "essence" of CEntity, move it outwards?
    void collapseChildren(VECTOR3 t = NullVector3);

    inline Transformation& getTransformation() { return m_transform; };

    // FIXME make virtual to allow different implementations that override
    // (slow) default behavior, or even better abstract enveloping objects
    // (spheres, OBBs, AABBs etc)
    const BBOX& getBoundingBox(int Frame = 0, int lod = 0);

    int isEntityInFrustum(int& testplanes);

    // don´t allow simple copying!
    CEntity& operator=(const CEntity& E);

    static CEntity*          findEntity(const std::string& Name);
    static const ENTITYLIST& GetEntityList();

    // FIXME: still public...
    int markentity;

protected:
    BBOX           m_bbox;
    int            m_earlyout;
    Transformation m_transform;

    std::vector<CMesh::SMARTPTR> m_meshes;  // all LOD levels, last mesh is smallest detail

    CEntity* m_parent;  // normal pointer, otherwise child and parent would
                        //  reference each other, as a result the child would
                        //  avoid the dstruction of the parent and vice versa

    SMARTPTRLIST m_children;

    // must not be a smartpointerlist, otherweise the entity will add a reference to itself
    // upon construction and this reference will prevent it from being ever deleted
    static ENTITYLIST s_allentities;
};

#endif  // !defined(AFX_ENTITY_H__7F8B4521_D049_11D3_AF5D_0080AD17B2BB__INCLUDED_)
