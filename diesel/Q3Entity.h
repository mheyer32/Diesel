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
// Q3Entity.h: Schnittstelle für die Klasse Q3Entity.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Q3ENTITY_H__FC8A0FF1_4B6C_432E_A3DF_78E6EBAACB83__INCLUDED_)
#define AFX_Q3ENTITY_H__FC8A0FF1_4B6C_432E_A3DF_78E6EBAACB83__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER > 1000

#include <list>
#include <map>
#include <string>

#include <adt/SmartPointer.h>

#include "Entity.h"
#include "rect.h"
#include "Task.h"

#define MAX_PORTALS 4;

class CEntity;
class Q3Scene;
class Q3BSPMesh;
class TNLStack;

class Q3Entity : public CEntity, protected CTask
{
    // FIXME: reduce friendships
    friend class Q3BSPLoader;
    friend class Q3EntityFactory;
    friend class Q3BSPMesh;
    friend class Q3Scene;

public:
    typedef SmartPointer<Q3Entity> SMARTPTR;
    typedef std::list<SMARTPTR>    SMARTPTRLIST;
    typedef SMARTPTRLIST::iterator SMARTPTRITERATOR;

    typedef std::list<Q3Entity*>   Q3ENTITYLIST;
    typedef Q3ENTITYLIST::iterator Q3ENTITYITERATOR;

    struct PORTALINFO
    {
        PLANE     portalplane;
        TNLStack* portalstack;
        int       cullfront;
        RECTANGLE portalrect;
    };

    // taken from Quake3 GameSource
    //
    // entityState_t->eType
    //
    enum ENTTYPE
    {
        ET_GENERAL,
        ET_ITEM,
        ET_FUNCBOBBING,
        ET_FUNCROTATING,
        ET_FUNCPENDULUM,
        ET_FUNCTRAIN,
        ET_PORTAL,
        ET_PORTALCAMERA,
        ET_SPEAKER
    };
    // gitem_t->type
    typedef enum
    {
        IT_BAD,
        IT_WEAPON,    // EFX: rotate + upscale + minlight
        IT_AMMO,      // EFX: rotate
        IT_ARMOR,     // EFX: rotate + minlight
        IT_HEALTH,    // EFX: static external sphere + rotating internal
        IT_POWERUP,   // instant on, timer based
                      // EFX: rotate + external ring that rotates
        IT_HOLDABLE,  // single use, holdable item
                      // EFX: rotate + bob
        IT_PERSISTANT_POWERUP,
        IT_TEAM
    } itemType_t;

#define MAX_ITEM_MODELS 4

    struct gitem_t
    {
        char* classname;  // spawning name
        char* pickup_sound;
        char* world_model[MAX_ITEM_MODELS];

        char* icon;
        char* pickup_name;  // for printing on pickup

        int        quantity;  // for ammo how much, or duration of powerup
        itemType_t giType;    // IT_* flags

        int giTag;

        char* precaches;  // string of all models and images this item will use
        char* sounds;     // string of all sounds this item will use
    };

    Q3Entity();

    virtual void cullEntity(int testplanes = ALLPLANES);
    virtual void runTask();

    const std::string& getClassName() const { return classname; };

    Q3Entity* getTarget() const { return target; };
    void      setTarget(Q3Entity* newTarget);

    void     setModel(CEntity* mewModel);
    CEntity* getModel() const { return model; };

    ENTTYPE getType() const { return type; };

    void       setWorldModel(Q3BSPMesh* model);
    Q3BSPMesh* getWorldModel() const { return worldmodel; };

    void assignEntity(CEntity* entity);

    static Q3Entity* findEntity(const std::string& entityname);

protected:
    virtual ~Q3Entity();

    void setPortalFace();

    void updateFuncRotating();
    void updateFuncBobbing();
    void updateFuncPendulum();
    void updateFuncTrain();
    void updateItem();

    void RenderPortal(PORTALINFO& portalinfo);

    CEntity::SMARTPTR model;

    // these fields are taken over from quake3 gamecode, not all are used at present
    // q3 reuses the different fields for different classes in different ways
    // deriving the entity-subclasses may be the better way.
    /////////////////////////////////////////////////////////////////////////////////////////
    SMARTPTR    target;
    std::string classname;
    std::string
        targetname;  // name of target, NOT own name, will be used to resolve target after loading in all entities

    std::string message;
    int         spawnflags;
    float       speed;
    float       wait;
    float       random;
    int         count;
    int         health;
    int         dmg;
    float       angles[3];
    ENTTYPE     type;
    int         itemtype;

    VECTOR3 origin;  // this is usually the base-position for bobbing/rotating entities

    /////////////////////////////////////////////////////////////////////////////////////////
    std::list<int> leafs;       // list of leaves this entity is linked into
    Q3BSPMesh*     worldmodel;  // bsp model
    int            fog;         // index of fogeffect this entity is in

    // portal traversal stuff
    static int                  cull_front;
    static Q3Entity*            current_portal;
    static std::list<Q3Entity*> seen_portals;

    // list of all instantiated q3 entities
    static Q3ENTITYLIST q3entities;

private:
    Q3Entity& operator=(const Q3Entity&);
    Q3Entity(const Q3Entity& ent2);
};

#endif  // !defined(AFX_Q3ENTITY_H__FC8A0FF1_4B6C_432E_A3DF_78E6EBAACB83__INCLUDED_)
