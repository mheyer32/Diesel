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
// Q3EntityFactory.cpp: Implementierung der Klasse Q3EntityFactory.
//
//////////////////////////////////////////////////////////////////////

#include "Q3EntityFactory.h"

#include <iostream>

#include <misc/Exception.h>

#include "Entity.h"
#include "MD3Loader.h"
#include "Q3BSPMesh.h"
// #include "Sound.h"
#include "Q3Entity.h"
#include "Shader.h"
#include "Sound.h"

#include <MemoryTracker.h>

#define DEFBOB 7.0f, 1.0f
#define DEFROT 180.0f
#define DEFSCALE 1.0f

/*QUAKED item_***** ( 0 0 0 ) (-16 -16 -16) (16 16 16) suspended
DO NOT USE THIS CLASS, IT JUST HOLDS GENERAL INFORMATION.
The suspended flag will allow items to hang in the air, otherwise they are dropped to the next surface.

If an item is the target of another entity, it will not spawn in until fired.

An item fires all of its targets when it is picked up.  If the toucher can't carry it, the targets won't be fired.

"notfree" if set to 1, don't spawn in free for all games
"notteam" if set to 1, don't spawn in team games
"notsingle" if set to 1, don't spawn in single player games
"wait"	override the default wait before respawning.  -1 = never respawn automatically, which can be used with targeted
spawning. "random" random number of plus or minus seconds varied from the respawn time "count" override quantity or
duration on most items.
*/

gitem_t Q3EntityFactory::bg_itemlist[] =
    {{NULL,
      NULL,
      {NULL, NULL, 0, 0},
      /* icon */ NULL,
      /* pickup */ NULL,
      0,
      0,
      0,
      /* precache */ "",
      /* sounds */ ""},  // leave index 0 alone

     //
     // ARMOR
     //

     /*QUAKED item_armor_shard (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
      */
     {"item_armor_shard",
      "sound/misc/ar1_pkup.wav",
      {"models/powerups/armor/shard.md3",
       0,  //"models/powerups/armor/shard_sphere.md3",
       0, 0},
      /* icon */ "icons/iconr_shard",
      /* pickup */ "Armor Shard",
      5,
      IT_ARMOR,
      0,
      /* precache */ "",
      /* sounds */ ""},

     /*QUAKED item_armor_combat (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
      */
     {"item_armor_combat",
      "sound/misc/ar2_pkup.wav",
      {"models/powerups/armor/armor_yel.md3", 0, 0, 0},
      /* icon */ "icons/iconr_yellow",
      /* pickup */ "Armor",
      50,
      IT_ARMOR,
      0,
      /* precache */ "",
      /* sounds */ ""},

     /*QUAKED item_armor_body (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
      */
     {"item_armor_body",
      "sound/misc/ar2_pkup.wav",
      {"models/powerups/armor/armor_red.md3", 0, 0, 0},
      /* icon */ "icons/iconr_red",
      /* pickup */ "Heavy Armor",
      100,
      IT_ARMOR,
      0,
      /* precache */ "",
      /* sounds */ ""},

     //
     // health
     //
     /*QUAKED item_health_small (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
      */
     {"item_health_small",
      "sound/items/s_health.wav",
      {"models/powerups/health/small_cross.md3", "models/powerups/health/small_sphere.md3", 0, 0},
      /* icon */ "icons/iconh_green",
      /* pickup */ "5 Health",
      5,
      IT_HEALTH,
      0,
      /* precache */ "",
      /* sounds */ ""},

     /*QUAKED item_health (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
      */
     {"item_health",
      "sound/items/n_health.wav",
      {"models/powerups/health/medium_cross.md3", "models/powerups/health/medium_sphere.md3", 0, 0},
      /* icon */ "icons/iconh_yellow",
      /* pickup */ "25 Health",
      25,
      IT_HEALTH,
      0,
      /* precache */ "",
      /* sounds */ ""},

     /*QUAKED item_health_large (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
      */
     {"item_health_large",
      "sound/items/l_health.wav",
      {"models/powerups/health/large_cross.md3", "models/powerups/health/large_sphere.md3", 0, 0},
      /* icon */ "icons/iconh_red",
      /* pickup */ "50 Health",
      50,
      IT_HEALTH,
      0,
      /* precache */ "",
      /* sounds */ ""},

     /*QUAKED item_health_mega (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
      */
     {"item_health_mega",
      "sound/items/m_health.wav",
      {"models/powerups/health/mega_cross.md3", "models/powerups/health/mega_sphere.md3", 0, 0},
      /* icon */ "icons/iconh_mega",
      /* pickup */ "Mega Health",
      100,
      IT_HEALTH,
      0,
      /* precache */ "",
      /* sounds */ ""},

     //
     // WEAPONS
     //

     /*QUAKED weapon_gauntlet (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
      */
     {"weapon_gauntlet",
      "sound/misc/w_pkup.wav",
      {"models/weapons2/gauntlet/gauntlet.md3", 0, 0, 0},
      /* icon */ "icons/iconw_gauntlet",
      /* pickup */ "Gauntlet",
      0,
      IT_WEAPON,
      WP_GAUNTLET,
      /* precache */ "",
      /* sounds */ ""},

     /*QUAKED weapon_shotgun (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
      */
     {"weapon_shotgun",
      "sound/misc/w_pkup.wav",
      {"models/weapons2/shotgun/shotgun.md3", 0, 0, 0},
      /* icon */ "icons/iconw_shotgun",
      /* pickup */ "Shotgun",
      10,
      IT_WEAPON,
      WP_SHOTGUN,
      /* precache */ "",
      /* sounds */ ""},

     /*QUAKED weapon_machinegun (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
      */
     {"weapon_machinegun",
      "sound/misc/w_pkup.wav",
      {"models/weapons2/machinegun/machinegun.md3", 0, 0, 0},
      /* icon */ "icons/iconw_machinegun",
      /* pickup */ "Machinegun",
      40,
      IT_WEAPON,
      WP_MACHINEGUN,
      /* precache */ "",
      /* sounds */ ""},

     /*QUAKED weapon_grenadelauncher (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
      */
     {"weapon_grenadelauncher",
      "sound/misc/w_pkup.wav",
      {"models/weapons2/grenadel/grenadel.md3", 0, 0, 0},
      /* icon */ "icons/iconw_grenade",
      /* pickup */ "Grenade Launcher",
      10,
      IT_WEAPON,
      WP_GRENADE_LAUNCHER,
      /* precache */ "",
      /* sounds */ "sound/weapons/grenade/hgrenb1a.wav sound/weapons/grenade/hgrenb2a.wav"},

     /*QUAKED weapon_rocketlauncher (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
      */
     {"weapon_rocketlauncher",
      "sound/misc/w_pkup.wav",
      {"models/weapons2/rocketl/rocketl.md3", 0, 0, 0},
      /* icon */ "icons/iconw_rocket",
      /* pickup */ "Rocket Launcher",
      10,
      IT_WEAPON,
      WP_ROCKET_LAUNCHER,
      /* precache */ "",
      /* sounds */ ""},

     /*QUAKED weapon_lightning (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
      */
     {"weapon_lightning",
      "sound/misc/w_pkup.wav",
      {"models/weapons2/lightning/lightning.md3", 0, 0, 0},
      /* icon */ "icons/iconw_lightning",
      /* pickup */ "Lightning Gun",
      100,
      IT_WEAPON,
      WP_LIGHTNING,
      /* precache */ "",
      /* sounds */ ""},

     /*QUAKED weapon_railgun (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
      */
     {"weapon_railgun",
      "sound/misc/w_pkup.wav",
      {"models/weapons2/railgun/railgun.md3", 0, 0, 0},
      /* icon */ "icons/iconw_railgun",
      /* pickup */ "Railgun",
      10,
      IT_WEAPON,
      WP_RAILGUN,
      /* precache */ "",
      /* sounds */ ""},

     /*QUAKED weapon_plasmagun (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
      */
     {"weapon_plasmagun",
      "sound/misc/w_pkup.wav",
      {"models/weapons2/plasma/plasma.md3", 0, 0, 0},
      /* icon */ "icons/iconw_plasma",
      /* pickup */ "Plasma Gun",
      50,
      IT_WEAPON,
      WP_PLASMAGUN,
      /* precache */ "",
      /* sounds */ ""},

     /*QUAKED weapon_bfg (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
      */
     {"weapon_bfg",
      "sound/misc/w_pkup.wav",
      {"models/weapons2/bfg/bfg.md3", 0, 0, 0},
      /* icon */ "icons/iconw_bfg",
      /* pickup */ "BFG10K",
      20,
      IT_WEAPON,
      WP_BFG,
      /* precache */ "",
      /* sounds */ ""},

     /*QUAKED weapon_grapplinghook (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
      */
     {"weapon_grapplinghook",
      "sound/misc/w_pkup.wav",
      {"models/weapons2/grapple/grapple.md3", 0, 0, 0},
      /* icon */ "icons/iconw_grapple",
      /* pickup */ "Grappling Hook",
      0,
      IT_WEAPON,
      WP_GRAPPLING_HOOK,
      /* precache */ "",
      /* sounds */ ""},

     //
     // AMMO ITEMS
     //

     /*QUAKED ammo_shells (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
      */
     {"ammo_shells",
      "sound/misc/am_pkup.wav",
      {"models/powerups/ammo/shotgunam.md3", 0, 0, 0},
      /* icon */ "icons/icona_shotgun",
      /* pickup */ "Shells",
      10,
      IT_AMMO,
      WP_SHOTGUN,
      /* precache */ "",
      /* sounds */ ""},

     /*QUAKED ammo_bullets (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
      */
     {"ammo_bullets",
      "sound/misc/am_pkup.wav",
      {"models/powerups/ammo/machinegunam.md3", 0, 0, 0},
      /* icon */ "icons/icona_machinegun",
      /* pickup */ "Bullets",
      50,
      IT_AMMO,
      WP_MACHINEGUN,
      /* precache */ "",
      /* sounds */ ""},

     /*QUAKED ammo_grenades (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
      */
     {"ammo_grenades",
      "sound/misc/am_pkup.wav",
      {"models/powerups/ammo/grenadeam.md3", 0, 0, 0},
      /* icon */ "icons/icona_grenade",
      /* pickup */ "Grenades",
      5,
      IT_AMMO,
      WP_GRENADE_LAUNCHER,
      /* precache */ "",
      /* sounds */ ""},

     /*QUAKED ammo_cells (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
      */
     {"ammo_cells",
      "sound/misc/am_pkup.wav",
      {"models/powerups/ammo/plasmaam.md3", 0, 0, 0},
      /* icon */ "icons/icona_plasma",
      /* pickup */ "Cells",
      30,
      IT_AMMO,
      WP_PLASMAGUN,
      /* precache */ "",
      /* sounds */ ""},

     /*QUAKED ammo_lightning (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
      */
     {"ammo_lightning",
      "sound/misc/am_pkup.wav",
      {"models/powerups/ammo/lightningam.md3", 0, 0, 0},
      /* icon */ "icons/icona_lightning",
      /* pickup */ "Lightning",
      60,
      IT_AMMO,
      WP_LIGHTNING,
      /* precache */ "",
      /* sounds */ ""},

     /*QUAKED ammo_rockets (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
      */
     {"ammo_rockets",
      "sound/misc/am_pkup.wav",
      {"models/powerups/ammo/rocketam.md3", 0, 0, 0},
      /* icon */ "icons/icona_rocket",
      /* pickup */ "Rockets",
      5,
      IT_AMMO,
      WP_ROCKET_LAUNCHER,
      /* precache */ "",
      /* sounds */ ""},

     /*QUAKED ammo_slugs (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
      */
     {"ammo_slugs",
      "sound/misc/am_pkup.wav",
      {"models/powerups/ammo/railgunam.md3", 0, 0, 0},
      /* icon */ "icons/icona_railgun",
      /* pickup */ "Slugs",
      10,
      IT_AMMO,
      WP_RAILGUN,
      /* precache */ "",
      /* sounds */ ""},

     /*QUAKED ammo_bfg (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
      */
     {"ammo_bfg",
      "sound/misc/am_pkup.wav",
      {"models/powerups/ammo/bfgam.md3", 0, 0, 0},
      /* icon */ "icons/icona_bfg",
      /* pickup */ "Bfg Ammo",
      15,
      IT_AMMO,
      WP_BFG,
      /* precache */ "",
      /* sounds */ ""},

     //
     // HOLDABLE ITEMS
     //
     /*QUAKED holdable_teleporter (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
      */
     {"holdable_teleporter",
      "sound/items/holdable.wav",
      {"models/powerups/holdable/teleporter.md3", 0, 0, 0},
      /* icon */ "icons/teleporter",
      /* pickup */ "Personal Teleporter",
      60,
      IT_HOLDABLE,
      HI_TELEPORTER,
      /* precache */ "",
      /* sounds */ ""},
     /*QUAKED holdable_medkit (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
      */
     {"holdable_medkit",
      "sound/items/holdable.wav",
      {"models/powerups/holdable/medkit.md3", "models/powerups/holdable/medkit_sphere.md3", 0, 0},
      /* icon */ "icons/medkit",
      /* pickup */ "Medkit",
      60,
      IT_HOLDABLE,
      HI_MEDKIT,
      /* precache */ "",
      /* sounds */ "sound/items/use_medkit.wav"},

     //
     // POWERUP ITEMS
     //
     /*QUAKED item_quad (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
      */
     {"item_quad",
      "sound/items/quaddamage.wav",
      {"models/powerups/instant/quad.md3", "models/powerups/instant/quad_ring.md3", 0, 0},
      /* icon */ "icons/quad",
      /* pickup */ "Quad Damage",
      30,
      IT_POWERUP,
      PW_QUAD,
      /* precache */ "",
      /* sounds */ "sound/items/damage2.wav sound/items/damage3.wav"},

     /*QUAKED item_enviro (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
      */
     {"item_enviro",
      "sound/items/protect.wav",
      {"models/powerups/instant/enviro.md3", "models/powerups/instant/enviro_ring.md3", 0, 0},
      /* icon */ "icons/envirosuit",
      /* pickup */ "Battle Suit",
      30,
      IT_POWERUP,
      PW_BATTLESUIT,
      /* precache */ "",
      /* sounds */ "sound/items/airout.wav sound/items/protect3.wav"},

     /*QUAKED item_haste (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
      */
     {"item_haste",
      "sound/items/haste.wav",
      {"models/powerups/instant/haste.md3", "models/powerups/instant/haste_ring.md3", 0, 0},
      /* icon */ "icons/haste",
      /* pickup */ "Speed",
      30,
      IT_POWERUP,
      PW_HASTE,
      /* precache */ "",
      /* sounds */ ""},

     /*QUAKED item_invis (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
      */
     {"item_invis",
      "sound/items/invisibility.wav",
      {"models/powerups/instant/invis.md3", "models/powerups/instant/invis_ring.md3", 0, 0},
      /* icon */ "icons/invis",
      /* pickup */ "Invisibility",
      30,
      IT_POWERUP,
      PW_INVIS,
      /* precache */ "",
      /* sounds */ ""},

     /*QUAKED item_regen (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
      */
     {"item_regen",
      "sound/items/regeneration.wav",
      {"models/powerups/instant/regen.md3", "models/powerups/instant/regen_ring.md3", 0, 0},
      /* icon */ "icons/regen",
      /* pickup */ "Regeneration",
      30,
      IT_POWERUP,
      PW_REGEN,
      /* precache */ "",
      /* sounds */ "sound/items/regen.wav"},

     /*QUAKED item_flight (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
      */
     {"item_flight",
      "sound/items/flight.wav",
      {"models/powerups/instant/flight.md3", "models/powerups/instant/flight_ring.md3", 0, 0},
      /* icon */ "icons/flight",
      /* pickup */ "Flight",
      60,
      IT_POWERUP,
      PW_FLIGHT,
      /* precache */ "",
      /* sounds */ "sound/items/flight.wav"},

     /*QUAKED team_CTF_redflag (1 0 0) (-16 -16 -16) (16 16 16)
     Only in CTF games
     */
     {"team_CTF_redflag",
      NULL,
      {"models/flags/r_flag.md3", 0, 0, 0},
      /* icon */ "icons/iconf_red1",
      /* pickup */ "Red Flag",
      0,
      IT_TEAM,
      PW_REDFLAG,
      /* precache */ "",
      /* sounds */ ""},

     /*QUAKED team_CTF_blueflag (0 0 1) (-16 -16 -16) (16 16 16)
     Only in CTF games
     */
     {"team_CTF_blueflag",
      NULL,
      {"models/flags/b_flag.md3", 0, 0, 0},
      /* icon */ "icons/iconf_blu1",
      /* pickup */ "Blue Flag",
      0,
      IT_TEAM,
      PW_BLUEFLAG,
      /* precache */ "",
      /* sounds */ ""},

     /*QUAKED holdable_kamikaze (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
      */
     {"holdable_kamikaze",
      "sound/items/holdable.wav",
      {"models/powerups/kamikazi.md3", 0, 0, 0},
      /* icon */ "icons/kamikaze",
      /* pickup */ "Kamikaze",
      60,
      IT_HOLDABLE,
      HI_KAMIKAZE,
      /* precache */ "",
      /* sounds */ "sound/items/kamikazerespawn.wav"},

     /*QUAKED holdable_portal (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
      */
     {"holdable_portal",
      "sound/items/holdable.wav",
      {"models/powerups/holdable/porter.md3", 0, 0, 0},
      /* icon */ "icons/portal",
      /* pickup */ "Portal",
      60,
      IT_HOLDABLE,
      HI_PORTAL,
      /* precache */ "",
      /* sounds */ ""},

     /*QUAKED holdable_invulnerability (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
      */
     {"holdable_invulnerability",
      "sound/items/holdable.wav",
      {"models/powerups/holdable/invulnerability.md3", 0, 0, 0},
      /* icon */ "icons/invulnerability",
      /* pickup */ "Invulnerability",
      60,
      IT_HOLDABLE,
      HI_INVULNERABILITY,
      /* precache */ "",
      /* sounds */ ""},

     /*QUAKED ammo_nails (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
      */
     {"ammo_nails",
      "sound/misc/am_pkup.wav",
      {"models/powerups/ammo/nailgunam.md3", 0, 0, 0},
      /* icon */ "icons/icona_nailgun",
      /* pickup */ "Nails",
      20,
      IT_AMMO,
      WP_NAILGUN,
      /* precache */ "",
      /* sounds */ ""},

     /*QUAKED ammo_mines (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
      */
     {"ammo_mines",
      "sound/misc/am_pkup.wav",
      {"models/powerups/ammo/proxmineam.md3", 0, 0, 0},
      /* icon */ "icons/icona_proxlauncher",
      /* pickup */ "Proximity Mines",
      10,
      IT_AMMO,
      WP_PROX_LAUNCHER,
      /* precache */ "",
      /* sounds */ ""},

     /*QUAKED ammo_belt (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
      */
     {"ammo_belt",
      "sound/misc/am_pkup.wav",
      {"models/powerups/ammo/chaingunam.md3", 0, 0, 0},
      /* icon */ "icons/icona_chaingun",
      /* pickup */ "Chaingun Belt",
      100,
      IT_AMMO,
      WP_CHAINGUN,
      /* precache */ "",
      /* sounds */ ""},

     //
     // PERSISTANT POWERUP ITEMS
     //
     /*QUAKED item_scout (.3 .3 1) (-16 -16 -16) (16 16 16) suspended redTeam blueTeam
      */
     {"item_scout",
      "sound/items/scout.wav",
      {"models/powerups/scout.md3", 0, 0, 0},
      /* icon */ "icons/scout",
      /* pickup */ "Scout",
      30,
      IT_PERSISTANT_POWERUP,
      PW_SCOUT,
      /* precache */ "",
      /* sounds */ ""},

     /*QUAKED item_guard (.3 .3 1) (-16 -16 -16) (16 16 16) suspended redTeam blueTeam
      */
     {"item_guard",
      "sound/items/guard.wav",
      {"models/powerups/guard.md3", 0, 0, 0},
      /* icon */ "icons/guard",
      /* pickup */ "Guard",
      30,
      IT_PERSISTANT_POWERUP,
      PW_GUARD,
      /* precache */ "",
      /* sounds */ ""},

     /*QUAKED item_doubler (.3 .3 1) (-16 -16 -16) (16 16 16) suspended redTeam blueTeam
      */
     {"item_doubler",
      "sound/items/doubler.wav",
      {"models/powerups/doubler.md3", 0, 0, 0},
      /* icon */ "icons/doubler",
      /* pickup */ "Doubler",
      30,
      IT_PERSISTANT_POWERUP,
      PW_DOUBLER,
      /* precache */ "",
      /* sounds */ ""},

     /*QUAKED item_doubler (.3 .3 1) (-16 -16 -16) (16 16 16) suspended redTeam blueTeam
      */
     {"item_ammoregen",
      "sound/items/ammoregen.wav",
      {"models/powerups/ammo.md3", 0, 0, 0},
      /* icon */ "icons/ammo_regen",
      /* pickup */ "Ammo Regen",
      30,
      IT_PERSISTANT_POWERUP,
      PW_AMMOREGEN,
      /* precache */ "",
      /* sounds */ ""},

     /*QUAKED team_CTF_neutralflag (0 0 1) (-16 -16 -16) (16 16 16)
 Only in One Flag CTF games
 */
     {"team_CTF_neutralflag",
      NULL,
      {"models/flags/n_flag.md3", 0, 0, 0},
      /* icon */ "icons/iconf_neutral1",
      /* pickup */ "Neutral Flag",
      0,
      IT_TEAM,
      PW_NEUTRALFLAG,
      /* precache */ "",
      /* sounds */ ""},

     {"item_redcube",
      "sound/misc/am_pkup.wav",
      {"models/powerups/orb/r_orb.md3", 0, 0, 0},
      /* icon */ "icons/iconh_rorb",
      /* pickup */ "Red Cube",
      0,
      IT_TEAM,
      0,
      /* precache */ "",
      /* sounds */ ""},

     {"item_bluecube",
      "sound/misc/am_pkup.wav",
      {"models/powerups/orb/b_orb.md3", 0, 0, 0},
      /* icon */ "icons/iconh_borb",
      /* pickup */ "Blue Cube",
      0,
      IT_TEAM,
      0,
      /* precache */ "",
      /* sounds */ ""},
     /*QUAKED weapon_nailgun (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
      */
     {"weapon_nailgun",
      "sound/misc/w_pkup.wav",
      {"models/weapons/nailgun/nailgun.md3", 0, 0, 0},
      /* icon */ "icons/iconw_nailgun",
      /* pickup */ "Nailgun",
      10,
      IT_WEAPON,
      WP_NAILGUN,
      /* precache */ "",
      /* sounds */ ""},

     /*QUAKED weapon_prox_launcher (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
      */
     {"weapon_prox_launcher",
      "sound/misc/w_pkup.wav",
      {"models/weapons/proxmine/proxmine.md3", 0, 0, 0},
      /* icon */ "icons/iconw_proxlauncher",
      /* pickup */ "Prox Launcher",
      5,
      IT_WEAPON,
      WP_PROX_LAUNCHER,
      /* precache */ "",
      /* sounds */
      "sound/weapons/proxmine/wstbtick.wav "
      "sound/weapons/proxmine/wstbactv.wav "
      "sound/weapons/proxmine/wstbimpl.wav "
      "sound/weapons/proxmine/wstbimpm.wav "
      "sound/weapons/proxmine/wstbimpd.wav "
      "sound/weapons/proxmine/wstbactv.wav"},

     /*QUAKED weapon_chaingun (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
      */
     {"weapon_chaingun",
      "sound/misc/w_pkup.wav",
      {"models/weapons/vulcan/vulcan.md3", 0, 0, 0},
      /* icon */ "icons/iconw_chaingun",
      /* pickup */ "Chaingun",
      80,
      IT_WEAPON,
      WP_CHAINGUN,
      /* precache */ "",
      /* sounds */ "sound/weapons/vulcan/wvulwind.wav"},

     // end of list marker
     {NULL}};

Q3EntityFactory::SPAWN Q3EntityFactory::spawns[] = {
    //	{"info_player_start", SP_info_player_start},
    {"info_player_deathmatch", &Q3EntityFactory::spawnInfoPlayerDeathMatch},
    //	{"info_player_intermission", SP_info_player_intermission},
    //	{"info_null", SP_info_null},
    {"info_notnull", &Q3EntityFactory::spawnGeneral},  // use target_position instead
                                                       //	{"info_camp", SP_info_camp},

    //	{"worldspawn", Q3EntityFactory::spawnWorldSpawn},

    {"func_plat", &Q3EntityFactory::spawnFunc},
    {"func_button", &Q3EntityFactory::spawnFunc},
    {"func_door", &Q3EntityFactory::spawnFunc},
    {"func_static", &Q3EntityFactory::spawnFunc},
    {"func_rotating", &Q3EntityFactory::spawnFunc},
    {"func_bobbing", &Q3EntityFactory::spawnFunc},
    {"func_pendulum", &Q3EntityFactory::spawnFunc},
    {"func_train", &Q3EntityFactory::spawnFunc},
    //	{"func_group", SP_info_null},
    //	{"func_timer", SP_func_timer},			// rename trigger_timer?

    // Triggers are brush objects that cause an effect when contacted
    // by a living player, usually involving firing targets.
    // While almost everything could be done with
    // a single trigger class and different targets, triggered effects
    // could not be client side predicted (push and teleport).
    //	{"trigger_always", SP_trigger_always},
    //	{"trigger_multiple", SP_trigger_multiple},
    //	{"trigger_push", SP_trigger_push},
    //	{"trigger_teleport", SP_trigger_teleport},
    //	{"trigger_hurt", SP_trigger_hurt},

    // targets perform no action by themselves, but must be triggered
    // by another entity
    //	{"target_give", SP_target_give},
    //	{"target_remove_powerups", SP_target_remove_powerups},
    //	{"target_delay", SP_target_delay},
    {"target_speaker", &Q3EntityFactory::spawnGeneral},
    //	{"target_print", SP_target_print},
    //	{"target_laser", SP_target_laser},
    //	{"target_score", SP_target_score},
    //	{"target_teleporter", SP_target_teleporter},
    //	{"target_relay", SP_target_relay},
    //	{"target_kill", SP_target_kill},
    {"target_position", &Q3EntityFactory::spawnTarget},
    {"target_location", &Q3EntityFactory::spawnTarget},
    //	{"target_push", SP_target_push},

    //	{"light", SP_light},
    {"path_corner", &Q3EntityFactory::spawnPathCorner},

    //	{"misc_teleporter_dest", SP_misc_teleporter_dest},
    //	{"misc_model", SP_misc_model},
    {"misc_portal_surface", &Q3EntityFactory::spawnPortal},
    {"misc_portal_camera", &Q3EntityFactory::spawnPortalCamera},

    //	{"shooter_rocket", SP_shooter_rocket},
    //	{"shooter_grenade", SP_shooter_grenade},
    //	{"shooter_plasma", SP_shooter_plasma},

    //	{"team_CTF_redplayer", SP_team_CTF_redplayer},
    //	{"team_CTF_blueplayer", SP_team_CTF_blueplayer},

    //	{"team_CTF_redspawn", SP_team_CTF_redspawn},
    //	{"team_CTF_bluespawn", SP_team_CTF_bluespawn},

    // #ifdef MISSIONPACK
    //	{"team_redobelisk", SP_team_redobelisk},
    //	{"team_blueobelisk", SP_team_blueobelisk},
    //	{"team_neutralobelisk", SP_team_neutralobelisk},
    // #endif
    //	{"item_botroam", SP_item_botroam},

    {0, 0}

};

using namespace std;

Q3EntityFactory::Q3EntityFactory()
{
}

Q3EntityFactory::~Q3EntityFactory()
{
}

void Q3EntityFactory::processEntities(char* entity_descriptions, Q3Scene* scene)
{
    m_lexer.setText(entity_descriptions);
    //	m_lexer.setCaseInsensitive();

    try {
        m_lexer.enterBlock();
        ProcessWorldSpawn(scene->getWorldModel());
        m_lexer.exitBlock();
    } catch (LexException&) {
    }

    std::list<Q3Entity::SMARTPTR> entlist;

    cout << "processing entities..." << endl;

    try {
        // parse all entity-descriptions, create the entities
        while (true) {
            m_lexer.enterBlock();
            Q3Entity* newentity = ProcessEntity(PROCENT_NOMISCMODELS);
            if (newentity) {
                // cout<<newentity->getName()<<endl;
                entlist.push_back(newentity);
            }
            m_lexer.exitBlock();
        }
    } catch (LexException&) {
    }
    // find the target for each entity and link it into the scene
    Q3Entity*                  target;
    Q3Entity::SMARTPTRITERATOR eIt = entlist.begin();
    while (eIt != entlist.end()) {
        if (!(*eIt)->targetname.empty()) {
            target = Q3Entity::findEntity((*eIt)->targetname);
            if (target) {
                (*eIt)->setTarget(target);
            } else {
                cout << "entity '" << (*eIt)->getName() << "' of class '" << (*eIt)->getClassName()
                     << "' could not find target '" << (*eIt)->targetname << "'" << endl;
            }
        }
        scene->addEntity(*eIt);
        if ((*eIt)->getType() == Q3Entity::ET_PORTAL) {
            // will link it into worldmodel
            (*eIt)->setPortalFace();
        }
        ++eIt;
    }
}

void Q3EntityFactory::ProcessWorldSpawn(Q3BSPMesh* worldmodel)
{

    cout << "processing worldspawn..." << endl;

    m_keyvalues.clear();
    readKeyValuePairs();

    std::string music;
    getValue("music", music);
    if (getValue("music", music)) {
        setBGMusic(music);
    }
    float gridsize[3];
    if (getValue("gridsize", gridsize)) {
        worldmodel->gridsize = VECTOR3(gridsize[0], gridsize[1], gridsize[2]);
    }
    cout << "lightvolsize: (" << worldmodel->gridsize.x << "," << worldmodel->gridsize.y << ","
         << worldmodel->gridsize.z << ")" << endl;

    std::string message;
    getValue("message", message);
    cout << endl << endl << "                 " << message << endl << endl;
}

Q3Entity* Q3EntityFactory::ProcessEntity(DWORD Options)
{
    Q3Entity* newentity = new Q3Entity;

    m_keyvalues.clear();
    readKeyValuePairs();

    getValue("classname", newentity->classname);

    int s = 0;
    while (spawns[s].classname) {
        if (EQ(newentity->classname.c_str(), spawns[s].classname)) {
            (this->*spawns[s].spawnfunc)(*newentity);
            return newentity;
        }
        s++;
    }

    if (spawnItem(*newentity)) {

        return newentity;
    }

    delete newentity;

    return NULL;
}

void Q3EntityFactory::readKeyValuePairs()
{
    try {
        while (1) {
            char key[512], value[512];
            int  charcount = m_lexer.readQuotedString(key);
            if (charcount == -1)
                break;

            m_lexer.readQuotedString(value);
            m_keyvalues[key] = value;
        }
    } catch (LexException&) {
    }
}

bool Q3EntityFactory::findKey(const std::string& key, const std::string*& value)
{
    std::map<std::string, std::string>::const_iterator kvIt = m_keyvalues.find(key);
    if (kvIt != m_keyvalues.end()) {
        value = &(kvIt->second);
        return true;
    }
    return false;
}

bool Q3EntityFactory::getValue(char* key, std::string& value)
{
    const std::string* valuestring;
    if (!findKey(key, valuestring))
        return false;
    value = *valuestring;
    return true;
}
bool Q3EntityFactory::getValue(char* key, float& value)
{
    const std::string* valuestring;
    if (!findKey(key, valuestring))
        return false;
    value = (float)atof(valuestring->c_str());
    return true;
}
bool Q3EntityFactory::getValue(char* key, int& value)
{
    const std::string* valuestring;
    if (!findKey(key, valuestring))
        return false;
    value = atoi(valuestring->c_str());
    return true;
}
bool Q3EntityFactory::getValue(char* key, float* value)
{
    const std::string* valuestring;
    if (!findKey(key, valuestring))
        return false;

    int pos = sscanf(valuestring->c_str(), "%f %f %f", &value[0], &value[1], &value[2]);
    if (pos == 3) {
        return true;
    }

    return false;
}

bool Q3EntityFactory::getValue(char* key, VECTOR3& value)
{
    const std::string* valuestring;
    if (!findKey(key, valuestring))
        return false;

    int pos = sscanf(valuestring->c_str(), "%f %f %f", &value.x, &value.z, &value.y);
    if (pos == 3) {
        value.z = -value.z;
        return true;
    }

    return false;
}
bool Q3EntityFactory::spawnGeneral(Q3Entity& Entity)
{
    getValue("spawnflags", Entity.spawnflags);
    getValue("targetname", Entity.m_name);

    getValue("target", Entity.targetname);
    getValue("wait", Entity.wait);
    getValue("random", Entity.random);
    getValue("count", Entity.count);

    getValue("angle", Entity.angles[1]);
    getValue("angles", Entity.angles);

    Entity.angles[1] = 180 - Entity.angles[1];  // fixme: is this needed ?

    getValue("origin", Entity.origin);
    return true;
}
bool Q3EntityFactory::spawnItem(Q3Entity& Entity)
{

    // std::cout<<"Q3EntityFactory::spawnItem() for: "<<Entity.classname<<std::endl;
    //  Find Item
    int c = 1;
    while (bg_itemlist[c].classname) {
        if (EQ(bg_itemlist[c].classname, Entity.classname.c_str()))
            break;
        c++;
    }

    if (bg_itemlist[c].classname == NULL)
        return false;

    spawnGeneral(Entity);

    Entity.getTransformation().setTranslation(Entity.origin);

    Entity.type     = Q3Entity::ET_ITEM;
    Entity.itemtype = bg_itemlist[c].giType;

    Entity.angles[0] = (float)rand() / (float)RAND_MAX;  // Start-Offset

    // Load Models
    MD3Loader loader;
    for (int m = 0; bg_itemlist[c].world_model[m]; m++) {
        CEntity* model = loader.loadSimpleLODEntity(bg_itemlist[c].world_model[m]);
        if (model) {
            Entity.addChild(model);
        } else {
            cout << "could not load model '" << bg_itemlist[c].world_model[m] << "' " << endl;
        }
    }

    return true;
};

bool Q3EntityFactory::spawnFunc(Q3Entity& Entity)
{

    spawnGeneral(Entity);

    std::string modelname;
    CEntity*    ent  = NULL;
    CMesh*      mesh = NULL;
    if (getValue("model", modelname)) {
        if (mesh = CMesh::FindMesh(modelname)) {
            ent = new CEntity(mesh);
            Entity.addChild(ent);
        } else {
            cout << "Q3EntityFactory::spawnFunc() could not find model: " << modelname << endl;
        }
    }
    if (getValue("model2", modelname)) {
        MD3Loader loader;
        ent = loader.loadSimpleLODEntity(modelname);
        if (ent) {
            Entity.addChild(ent);
        } else {
            cout << "Q3EntityFactory::spawnFunc() could not find model2: " << modelname << endl;
        }
    }
    if (Entity.classname == "func_rotating") {
        Entity.type = Q3Entity::ET_FUNCROTATING;
        if (!getValue("speed", Entity.speed))
            Entity.speed = 100;
    } else if (Entity.classname == "func_bobbing") {
        Entity.type = Q3Entity::ET_FUNCBOBBING;
        if (!getValue("phase", Entity.angles[1]))
            Entity.angles[1] = 0;
        if (!getValue("height", Entity.angles[2]))
            Entity.angles[2] = 32;
        if (!getValue("speed", Entity.speed))
            Entity.speed = 4;
    } else if (Entity.classname == "func_pendulum") {
        Entity.type = Q3Entity::ET_FUNCPENDULUM;
        getValue("phase", Entity.wait);
        if (!getValue("speed", Entity.speed))
            Entity.speed = 30;
    } else if (Entity.classname == "func_train") {
        Entity.type = Q3Entity::ET_FUNCTRAIN;
        if (!getValue("speed", Entity.speed))
            Entity.speed = 100;
        Entity.angles[0] = Entity.speed;
        Entity.wait      = 0;
    }
    Entity.getTransformation().setTranslation(Entity.origin);

    CKernel::Instance()->addTask(&Entity);
    return true;
}

bool Q3EntityFactory::spawnPortal(Q3Entity& Entity)
{
    spawnGeneral(Entity);
    cout << "spawned PORTAL " << Entity.m_name << endl;
    Entity.type = Q3Entity::ET_PORTAL;
    Entity.getTransformation().setTranslation(Entity.origin);

    // this is a speciality of Q3RocketArena - if a portal surface entity is associated with a model
    // FIXME: sometimes the referenced model is not yet instantiated
    std::string modelname;
    if (getValue("model", modelname)) {
        CEntity* ent = NULL;
        if (ent = CEntity::findEntity(modelname)) {
            Entity.model = ent;
        } else {
            cout << "Q3EntityFactory::spawnPortal() portal '" << Entity.getName() << "': could not find model: '"
                 << modelname << "'" << endl;
        }
    }
    return true;
}

bool Q3EntityFactory::spawnPortalCamera(Q3Entity& Entity)
{
    spawnGeneral(Entity);
    Entity.type = Q3Entity::ET_PORTALCAMERA;
    // probably only two values are expected to be in the description
    // FIXME: check this
    // getValue("angles",Entity.angles);  // done already in spawnGeneral()

    if (Entity.spawnflags & 1) {
        Entity.speed     = 25;  // deg/s ?
        Entity.angles[2] = 0;
    } else if (Entity.spawnflags & 2) {
        Entity.speed     = 75;
        Entity.angles[2] = 0;
    } else {
        Entity.speed = 45;
        if (!getValue("roll", Entity.angles[2]))
            Entity.angles[2] = 0;
    }

    Entity.getTransformation().setTranslation(Entity.origin);
    return true;
}

bool Q3EntityFactory::spawnInfoPlayerDeathMatch(Q3Entity& Entity)
{
    spawnGeneral(Entity);
    Entity.getTransformation().setTranslation(Entity.origin);
    return true;
}

bool Q3EntityFactory::spawnTarget(Q3Entity& Entity)
{
    spawnGeneral(Entity);
    Entity.getTransformation().setTranslation(Entity.origin);
    return true;
}

bool Q3EntityFactory::spawnPathCorner(Q3Entity& Entity)
{
    spawnGeneral(Entity);
    if (!getValue("speed", Entity.speed))
        Entity.speed = 0;
    getValue("wait", Entity.wait);
    Entity.getTransformation().setTranslation(Entity.origin);
    return true;
}
