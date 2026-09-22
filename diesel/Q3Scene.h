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
// Q3Scene.h: Schnittstelle für die Klasse Q3Scene.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Q3SCENE_H__9A5CFB0F_1CFB_4834_9EF7_0F400C4FB71F__INCLUDED_)
#define AFX_Q3SCENE_H__9A5CFB0F_1CFB_4834_9EF7_0F400C4FB71F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER > 1000

#define MAX_NUM_PORTALS 4

#include <adt/RefCount.h>
#include "BaseScene.h"
#include "Q3BSPMesh.h"
#include "Q3Entity.h"

class Q3Scene : public CBaseScene, public CRefCount
{

    friend class Q3BSPLoader;

public:
    Q3Scene();

    // fulfill CBaseScene´s abstract interface
    virtual void cullScene();
    virtual void renderScene();
    virtual void addEntity(CEntity* newEntity);
    virtual void addEntities(CEntity::ENTITYLIST& list);
    virtual bool initialize();

    void addEntity(Q3Entity* newEntity);

    Q3BSPMesh* getWorldModel() { return worldmodel; };

    void     setWeaponModel(CEntity* weapon);
    CEntity* getWeaponModel() const { return weaponmodel; };

    const Q3Entity::SMARTPTRLIST& getEntityList() { return sceneentities; };

protected:
    virtual ~Q3Scene();

    void renderGun();
    void setupCamera();
    void renderPortals();
    void cullPortals();

    Q3BSPMesh*        worldmodel;
    CEntity::SMARTPTR weaponmodel;

    // the handling of Q3-Portals is not very mature
    Q3Entity::PORTALINFO g_Portals[MAX_NUM_PORTALS];
    int                  g_num_Portals;
    TNLStack*            g_PortalStacks[MAX_NUM_PORTALS];

    Q3Entity::SMARTPTRLIST sceneentities;

    static void weapon_CB(ConVar& var);

    static ConVar r_portalOnly;
    static ConVar cg_drawGun;
    static ConVar raytest;
    static ConVar weapon;
};

#endif  // !defined(AFX_Q3SCENE_H__9A5CFB0F_1CFB_4834_9EF7_0F400C4FB71F__INCLUDED_)
