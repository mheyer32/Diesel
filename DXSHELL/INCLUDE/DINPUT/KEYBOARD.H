
/*
This file is part of DXShell
(c) 2002 by Mathias Heyer

DXShell is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

DXShell is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
// Keyboard.h: Schnittstelle für die Klasse CKeyboard.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KEYBOARD_H__31079B01_8D92_11D3_BD8A_0000E85E86C1__INCLUDED_)
#define AFX_KEYBOARD_H__31079B01_8D92_11D3_BD8A_0000E85E86C1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER > 1000

#include <adt/singleton.h>
#include <iostream>
#include <map>
#include <string>
#include "DInputDevice.h"

class CKeyboard : public CDInputDevice, public Singleton<CKeyboard>
{
    DECLARE_SINGLETON(CKeyboard)
public:
    bool initialize(DWORD cooplevel = DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

    inline bool isPressed(int key) { return (m_keybuffer[key] & 0x80) ? true : false; }

    const BYTE* getBuffer();

    bool getKeyforASCII(const std::string& keyname, DWORD& keycode);
    char getASCIIforKey(DWORD scancode);

protected:
    CKeyboard();
    virtual ~CKeyboard();

    void initTranslationMaps();

    BYTE m_keybuffer[256];

    std::map<std::string, int> ascii2dx;
    std::map<int, std::string> dx2ascii;
};

#endif  // !defined(AFX_KEYBOARD_H__31079B01_8D92_11D3_BD8A_0000E85E86C1__INCLUDED_)
