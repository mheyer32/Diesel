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
// KeyMap.cpp: Implementierung der Klasse CKeyMap.
//
//////////////////////////////////////////////////////////////////////

#include <adt/ConCmd.h>
#include <dinput/keyboard.h>
#include <iostream>
#include "KeyMap.h"

#include <memorytracker.h>
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

using namespace std;

CKeyMap::CKeyMap()
{

    keyboard = CKeyboard::Instance();

    ZeroMemory(lastkbstate, sizeof(lastkbstate));

    SwitchKeymap(CKeyMap::KEYMAP1);
}

CKeyMap::~CKeyMap()
{
    SwitchKeymap(KEYMAP1);
    UnbindAll();
    SwitchKeymap(KEYMAP2);
    UnbindAll();
}

void CKeyMap::BindKey(int key, KEYCALLBACKFUNC pressCB, KEYCALLBACKFUNC releaseCB, CKeyMap::KEYOPTION keyopt,
                      void* userdata)
{
    keymap[key].options      = keyopt;
    keymap[key].pressCB      = pressCB;
    keymap[key].releaseCB    = releaseCB;
    keymap[key].userdata     = userdata;
    keymap[key].firstpressed = 0.0;
}

void CKeyMap::UnBindKey(int key)
{
    keymap[key].pressCB   = NULL;
    keymap[key].releaseCB = NULL;
}

void CKeyMap::UnbindAll()
{
    for (int i = 0; i < 256; i++) {
        UnBindKey(i);
    }
}

void CKeyMap::Update()
{
    BYTE oldks, newks;

    keyboard->GetDeviceState();
    const BYTE* keybuffer = keyboard->getBuffer();

    timer.stop();
    float time = timer.getElapsedSecs();

    for (int k = 0; k < 256; k++) {
        oldks          = lastkbstate[k];
        newks          = keybuffer[k];
        lastkbstate[k] = keybuffer[k];

        if (newks & 0x80) {
            if (keymap[k].firstpressed == 0.0)  // key was pressed for the first time, also used for TOGGLEONCE
            {
                PressKey(k);
                keymap[k].firstpressed = time + KEYREPEAT_DELAY - KEYREPEAT_PAUSE;  // initial delay for repeating keys
                continue;
            }

            switch (keymap[k].options) {
            case KEYOPT_CONTINUE:
                PressKey(k);
                break;
            case KEYOPT_REPEAT:
                if ((time - keymap[k].firstpressed) >= KEYREPEAT_PAUSE) {
                    PressKey(k);
                    keymap[k].firstpressed = time;
                }
                break;
            }
        } else {
            if (oldks & 0x80)  // key released
            {
                ReleaseKey(k);
                keymap[k].firstpressed = 0.0;
            }
        }
    }
}

void CKeyMap::PressKey(int key)
{
    // std::cout<<"CKeyMap::PressKey() "<<key<<"  "<<std::endl;
    if (keymap[key].pressCB)
        keymap[key].pressCB(key, keymap[key].userdata);
}

void CKeyMap::ReleaseKey(int key)
{
    // std::cout<<"CKeyMap::ReleaseKey() "<<key<<"  "<<std::endl;
    if (keymap[key].releaseCB)
        keymap[key].releaseCB(key, keymap[key].userdata);
}

void CKeyMap::SwitchKeymap(int map)
{
    keymapnum = map;
    keymap    = keymaps[keymapnum];
}

int CKeyMap::getKeymapNum() const
{
    return keymapnum;
}
