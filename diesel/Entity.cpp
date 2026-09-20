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

#include "Entity.h"

#include <iostream>

#include <misc/Exception.h>

#include "Camera.h"
#include "Engine.h"
#include "Mesh.h"
#include "TNLStack.h"

#include <MemoryTracker.h>

CEntity::ENTITYLIST CEntity::s_allentities;

using namespace std;

CEntity::CEntity(CMesh* Mesh)
{
    m_name = string("unnamed_entity");

    m_parent = NULL;

    m_earlyout = 4;

    if (Mesh) {
        addMesh(Mesh);
        setName(Mesh->getName());
    }
    s_allentities.push_front(this);
}

CEntity::~CEntity()
{
#ifdef _DEBUG
    std::cout << "Entity: " << getName() << " deleted" << std::endl;
#endif
    // FIXME: cyclic references cause infinite loops and crashes
    // if (m_parent) m_parent->removeChild(this);
    s_allentities.remove(this);

    // m_children will be automatically released
}

void CEntity::cullEntity(int testplanes)
{
    if (isEntityInFrustum(testplanes)) {
        if (!m_transform.isIdentity()) {
            g_TNLStack->PushMatrix();
            g_TNLStack->MultMatrix(m_transform.getMatrix());
        }
        int lod = 0;
        if (!m_meshes.empty()) {
            MATRIX4 m;
            g_TNLStack->getMatrix(m);
            const VECTOR3& dist = (VECTOR3&)m.a[3];
            // quick´n dirty lod based on distance
            // looks quite good  though
            int num_lods = m_meshes.size();
            lod          = (int)fabsf(dist.z / 200.0f);
            if (lod > num_lods - 1)
                lod = num_lods - 1;
            m_meshes[lod]->batchFaces();
        }
        SMARTPTRLIST::iterator e = m_children.begin();
        for (; e != m_children.end(); ++e) {
            (*e)->cullEntity(testplanes);
        }
        if (!m_transform.isIdentity())
            g_TNLStack->PopMatrix();
    }
}

void CEntity::addMesh(CMesh* newMesh)
{
    if (!newMesh) {
        cout << "CEntity::addMesh() tried to set NULL mesh" << endl;
        return;
    }
    m_meshes.push_back(newMesh);
}

CMesh* CEntity::getMesh(int lod)
{
    if (m_meshes.empty())
        return NULL;
    return m_meshes[lod];
}

void CEntity::addChild(CEntity* child)
{
    m_children.push_back(child);

    CEntity* otherparent = child->getParent();

    if (otherparent) {
        otherparent->removeChild(child);
    }

    child->m_parent = this;
}

void CEntity::addChild(CEntity::ENTITYLIST& m_children)
{
    ENTITYITERATOR e = m_children.begin();

    for (; e != m_children.end(); ++e) {
        addChild(*e);
    }
}

void CEntity::removeChild(CEntity* child)
{
    m_children.remove(child);
    // FIXME: need to check if it really was this´ child ?
    child->m_parent = NULL;
}

CEntity* CEntity::getChild(unsigned int childNum)
{
    if (childNum > m_children.size())
        return NULL;
    SMARTPTRLIST::iterator eIt = m_children.begin();
    while (childNum) {
        ++eIt;
        --childNum;
    };  // clumsy
    return *eIt;
}

CEntity* CEntity::findEntity(const std::string& Name)
{
    ENTITYITERATOR e;

    for (e = s_allentities.begin(); e != s_allentities.end(); ++e) {
        if ((*e)->m_name == Name)
            return *e;
    }
    return NULL;
}

const BBOX& CEntity::getBoundingBox(int Frame, int lod)
{
    // FIXME: introduce caching for the bounding box
    if (m_meshes.size() || m_children.size()) {
        resetBBox(m_bbox);
        // sollten keine Children vorhanden sein, wird m_bbox nicht angrührt
        SMARTPTRLIST::const_iterator e = m_children.begin();
        for (; e != m_children.end(); ++e) {
            m_bbox |= (*e)->getBoundingBox(Frame, lod);
        }
        if (m_meshes.size()) {
            m_bbox |= m_meshes[lod]->getBoundingBox(Frame);
        }
        if (!m_transform.isIdentity()) {
            m_bbox = m_transform.getMatrix() * m_bbox;
        }
    } else {
        m_bbox.max = m_bbox.min = m_transform.getTranslation();
    }

    return m_bbox;
}

// FIXME: use full matrix transformation of m_children here
void CEntity::collapseChildren(VECTOR3 t)
{
    SMARTPTRLIST::iterator e;

    t += m_transform.getTranslation();

    int num_lods = m_meshes.size();

    if (m_children.size() == 0) {
        for (int l = 0; l < num_lods; ++l) {
            if (m_meshes[l] != NULL) {
                m_meshes[l]->translate(t);
            }
        }
        return;  // besteht nur aus einem/keinem Mesh;
    }

    for (e = m_children.begin(); e != m_children.end(); e++) {
        (*e)->collapseChildren(t);
    }

    if (m_meshes[0] == NULL)  // wahrsch. GruppenObjekt
    {
        m_meshes[0] = new CMesh;
        num_lods    = 1;
    } else {
        for (int l = 0; l < num_lods; l++) {
            m_meshes[l]->translate(t);
        }
    }

    for (e = m_children.begin(); e != m_children.end(); ++e) {
        if ((*e)->m_meshes.size() != m_meshes.size()) {
            throw CException("CEntity::collapseChildren()\n cannot combine entities with a different number of LODs");
        }
        for (int l = 0; l < num_lods; l++) {
            if ((*e)->m_meshes[l] != NULL) {
                *m_meshes[l] += *((*e)->m_meshes[l]);  // ==NULL sollte NIE eintreten (völlig leeres Entity??)
            }
        }
    }

    if (m_meshes[0]->getVBufferList().size() == 0)  // immernoch leeres Mesh
    {
        addMesh(NULL);
    }

    m_children.clear();

    getBoundingBox();
}

const CEntity::ENTITYLIST& CEntity::GetEntityList()
{
    return s_allentities;
}

inline int CEntity::isEntityInFrustum(int& testplanes)
{
    if (!testplanes)
        return true;

    MATRIX4 M;
    g_TNLStack->getMatrix(M);

    M            = g_Camera->getTransformation().getMatrix() * M;  // g_Camera-Matrix aus M=C^-1 * E herausrechnen
    BBOX testbox = M * getBoundingBox();
    return g_Camera->BoxInFrustum(testbox, testplanes);
    // return g_Camera->BoxOnFrustumSide(testbox,testplanes,m_earlyout)&FRONTSIDE;
}

int CEntity::getNumChildren() const
{
    return m_children.size();
}

CEntity* CEntity::getParent() const
{
    return m_parent;
}

CEntity* CEntity::clone()
{
    CEntity* E = new CEntity;
    *E         = *this;
    return E;
}

CEntity& CEntity::operator=(const CEntity& entity)
{
    if (&entity == this)
        return *this;
    m_bbox      = entity.m_bbox;
    m_transform = entity.m_transform;
    m_parent    = NULL;

    SMARTPTRLIST::const_iterator e;

    CEntity* child = NULL;

    // m_children of an entity can obly have one m_parent, so clone them
    for (e = entity.m_children.begin(); e != entity.m_children.end(); ++e) {
        child = (*e)->clone();
        addChild(child);
    }

    m_meshes.clear();

    for (std::vector<CMesh::SMARTPTR>::size_type l = 0; l < entity.m_meshes.size(); ++l) {
        addMesh(entity.m_meshes[l]);
    }

    m_name = entity.m_name;
    m_name += "_clone";
    return *this;
}