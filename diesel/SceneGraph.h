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
// SceneGraph.h: Schnittstelle für die Klasse CSceneGraph.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCENEGRAPH_H__D9FC738E_CACF_4FD8_96D1_9E58EB45F8E0__INCLUDED_)
#define AFX_SCENEGRAPH_H__D9FC738E_CACF_4FD8_96D1_9E58EB45F8E0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER > 1000

#include "BaseScene.h"

class CEntity;

class CSceneGraph : public CBaseScene
{
public:
    CSceneGraph();
    virtual ~CSceneGraph();

    // fulfill CBaseScene´s abstract interface
    virtual void cullScene();
    virtual void renderScene();
    virtual void addEntity(CEntity* newEntity);
    virtual void addEntities(CEntity::ENTITYLIST& list);
    virtual bool initialize();

    CEntity* getSceneRoot() const { return sceneroot; };

protected:
    CEntity* sceneroot;
};

#endif  // !defined(AFX_SCENEGRAPH_H__D9FC738E_CACF_4FD8_96D1_9E58EB45F8E0__INCLUDED_)
