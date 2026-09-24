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
// Q3EntityFactory.h: Schnittstelle für die Klasse Q3EntityFactory.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Q3ENTITYFACTORY_H__86524019_BF91_4B95_93C1_870EE36641D7__INCLUDED_)
#define AFX_Q3ENTITYFACTORY_H__86524019_BF91_4B95_93C1_870EE36641D7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>

#include <misc/Lexer.h>
#include "Q3Entity.h"
#include "Q3Scene.h"

// NOTE: may not have more than 16
typedef enum
{
    PW_NONE,

    PW_QUAD,
    PW_BATTLESUIT,
    PW_HASTE,
    PW_INVIS,
    PW_REGEN,
    PW_FLIGHT,

    PW_REDFLAG,
    PW_BLUEFLAG,
    PW_NEUTRALFLAG,

    PW_SCOUT,
    PW_GUARD,
    PW_DOUBLER,
    PW_AMMOREGEN,
    PW_INVULNERABILITY,

    PW_NUM_POWERUPS

} powerup_t;

typedef enum
{
    HI_NONE,

    HI_TELEPORTER,
    HI_MEDKIT,
    HI_KAMIKAZE,
    HI_PORTAL,
    HI_INVULNERABILITY,

    HI_NUM_HOLDABLE
} holdable_t;

typedef enum
{
    WP_NONE,

    WP_GAUNTLET,
    WP_MACHINEGUN,
    WP_SHOTGUN,
    WP_GRENADE_LAUNCHER,
    WP_ROCKET_LAUNCHER,
    WP_LIGHTNING,
    WP_RAILGUN,
    WP_PLASMAGUN,
    WP_BFG,
    WP_GRAPPLING_HOOK,
    WP_NAILGUN,
    WP_PROX_LAUNCHER,
    WP_CHAINGUN,

    WP_NUM_WEAPONS
} weapon_t;

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

    int quantity;  // for ammo how much, or duration of powerup
    int giType;    // IT_* flags

    int giTag;

    char* precaches;  // string of all models and images this item will use
    char* sounds;     // string of all sounds this item will use
};

#define PROCENT_NOMISCMODELS (1 << 0)

class CEntity;

class Q3EntityFactory
{
public:
    Q3EntityFactory();
    ~Q3EntityFactory();

    void      processEntities(char* entity_descriptions, Q3Scene* scene);
    Q3Entity* ProcessEntity(DWORD Options = 0);
    void      ProcessWorldSpawn(Q3BSPMesh* worldmodel);

protected:
    typedef bool (Q3EntityFactory::*SPAWNFUNC)(Q3Entity& entity);

    struct SPAWN
    {
        char*     classname;
        SPAWNFUNC spawnfunc;
    };

    void readKeyValuePairs();
    bool findKey(const std::string& key, const std::string*& value);

    bool getValue(char* key, std::string& value);
    bool getValue(char* key, float& value);
    bool getValue(char* key, int& value);
    bool getValue(char* key, float* value);
    bool getValue(char* key, VECTOR3& value);

    Q3Entity* curr_entity;

    Lexer m_lexer;

    std::map<std::string, std::string> m_keyvalues;

    bool spawnGeneral(Q3Entity& Entity);
    bool spawnItem(Q3Entity& Entity);
    bool spawnFunc(Q3Entity& Entity);
    bool spawnWorldSpawn(Q3Entity& Entity);
    bool spawnPortal(Q3Entity& Entity);
    bool spawnPortalCamera(Q3Entity& Entity);
    bool spawnInfoPlayerDeathMatch(Q3Entity& Entity);
    bool spawnTarget(Q3Entity& Entity);
    bool spawnPathCorner(Q3Entity& Entity);
    bool spawnTargetSpeaker(Q3Entity& Entity);

    void closeStartClosedMoverPortals(Q3Scene* scene);

    static gitem_t bg_itemlist[];
    static SPAWN   spawns[];
};

#endif  // !defined(AFX_Q3ENTITYFACTORY_H__86524019_BF91_4B95_93C1_870EE36641D7__INCLUDED_)
