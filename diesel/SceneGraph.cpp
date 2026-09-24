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
// SceneGraph.cpp: Implementierung der Klasse CSceneGraph.
//
//////////////////////////////////////////////////////////////////////

#include "SceneGraph.h"

#include "Engine.h"
#include "Entity.h"
#include "TNLStack.h"

#include <cassert>

#include <MemoryTracker.h>
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CSceneGraph::CSceneGraph()
{
    sceneroot = NULL;
    initialize();
}

CSceneGraph::~CSceneGraph()
{
    if (sceneroot) {
        // should kill everything below
        sceneroot->Release();
    }
}

void CSceneGraph::cullScene()
{
    sceneroot->cullEntity();
}

void CSceneGraph::renderScene()
{
    g_TNLStack->execute();
    g_TNLStack->clearStack();
}

bool CSceneGraph::initialize()
{
    if (sceneroot) {
        sceneroot->Release();
    }

    sceneroot = new CEntity();
    sceneroot->AddRef();
    sceneroot->setName("SceneRoot");

    return true;
}

void CSceneGraph::addEntity(CEntity* newEntity)
{
    assert(sceneroot);

    sceneroot->addChild(newEntity);
}

void CSceneGraph::addEntities(CEntity::ENTITYLIST& list)
{
    assert(sceneroot);

    sceneroot->addChild(list);
}