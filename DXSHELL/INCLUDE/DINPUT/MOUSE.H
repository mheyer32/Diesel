
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
// Mouse.h: Schnittstelle für die Klasse CMouse.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOUSE_H__26F61E60_9F45_11D3_AF5D_0080AD17B2BB__INCLUDED_)
#define AFX_MOUSE_H__26F61E60_9F45_11D3_AF5D_0080AD17B2BB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER > 1000

#include <adt/singleton.h>
#include "DInputDevice.h"

#define MBUTTON1 CMouse::MouseState.rgbButtons[0]
#define MBUTTON2 CMouse::MouseState.rgbButtons[1]
#define MBUTTON3 CMouse::MouseState.rgbButtons[2]
#define MBUTTON4 CMouse::MouseState.rgbButtons[3]
#define MOUSEDX CMouse::MouseState.lX
#define MOUSEDY CMouse::MouseState.lY
#define MOUSEDZ CMouse::MouseState.lZ

class CMouse : public CDInputDevice, public Singleton<CMouse>
{
    DECLARE_SINGLETON(CMouse)
public:
    bool initialize(DWORD cooplevel = DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);

    DIMOUSESTATE MouseState;

protected:
    virtual ~CMouse();
    CMouse();
};

#endif  // !defined(AFX_MOUSE_H__26F61E60_9F45_11D3_AF5D_0080AD17B2BB__INCLUDED_)
