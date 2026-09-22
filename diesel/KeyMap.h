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
// KeyMap.h: Schnittstelle für die Klasse CKeyMap.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KEYMAP_H__BC7F021D_59E0_497A_8785_775ADA58D2AF__INCLUDED_)
#define AFX_KEYMAP_H__BC7F021D_59E0_497A_8785_775ADA58D2AF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER > 1000

#include <dinput/Keyboard.h>
#include <misc/Timer.h>  // for the timers
#include <map>
#include <string>

#define KEYREPEAT_PAUSE 0.024f
#define KEYREPEAT_DELAY 0.250f

class CKeyMap
{

public:
    typedef void (*KEYCALLBACKFUNC)(int key, void* userdata);

    enum KEYOPTION
    {
        KEYOPT_REPEAT,       // trigger callback every 250 msecs
        KEYOPT_TRIGGERONCE,  // trigger callback just once
        KEYOPT_CONTINUE      // trigger callback every update
    };
    enum KEYMAP
    {
        KEYMAP1 = 0,
        KEYMAP2 = 1
    };

    CKeyMap();
    virtual ~CKeyMap();

    void Update();
    void BindKey(int key, KEYCALLBACKFUNC pressCB, KEYCALLBACKFUNC releaseCB = NULL, KEYOPTION keyopt = KEYOPT_REPEAT,
                 void* userdata = NULL);
    void UnBindKey(int key);
    void UnbindAll();

    void SwitchKeymap(int map);
    int  getKeymapNum() const;

protected:
    struct KEYCALLBACK
    {
        DWORD           options;
        KEYCALLBACKFUNC pressCB;
        KEYCALLBACKFUNC releaseCB;
        void*           userdata;
        float           firstpressed;

        KEYCALLBACK()
        {
            options      = 0;
            pressCB      = NULL;
            releaseCB    = NULL;
            userdata     = NULL;
            firstpressed = 0;
        }
    };

    void ReleaseKey(int key);
    void PressKey(int key);

    CKeyboard* keyboard;

    // FIXME: better use a stack of keymaps here, with puhs/pop semantic
    KEYCALLBACK  keymaps[2][256];  // console-keymap
    KEYCALLBACK* keymap;           // current keymap
    int          keymapnum;

    BYTE lastkbstate[256];

    CTimer timer;
};

#endif  // !defined(AFX_KEYMAP_H__BC7F021D_59E0_497A_8785_775ADA58D2AF__INCLUDED_)
