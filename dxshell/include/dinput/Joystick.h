
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
// Joystick.h: Schnittstelle für die Klasse CJoystick.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_JOYSTICK_H__713B6817_D2DD_4DAB_A552_C526620F9D91__INCLUDED_)
#define AFX_JOYSTICK_H__713B6817_D2DD_4DAB_A552_C526620F9D91__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER > 1000
#include "DInputDevice.h"

class CJoystick : public CDInputDevice
{
public:
    CJoystick(DWORD CoopLevel = DISCL_EXCLUSIVE | DISCL_FOREGROUND);
    virtual ~CJoystick();

    virtual HRESULT GetDeviceState();
    HRESULT         SetRange(DWORD Object, long Min = -1000, long Max = 1000);
    HRESULT         SetDeadzone(DWORD Object, long Min = -500, long Max = 500);

    DIJOYSTATE JoyState;
};

#endif  // !defined(AFX_JOYSTICK_H__713B6817_D2DD_4DAB_A552_C526620F9D91__INCLUDED_)
