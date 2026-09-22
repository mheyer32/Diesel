
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
// DriverSelector.h: Schnittstelle für die Klasse CDriverSelector.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRIVERSELECTOR_H__FA8DAF80_A68F_11D3_BD8A_0000E85E86C1__INCLUDED_)
#define AFX_DRIVERSELECTOR_H__FA8DAF80_A68F_11D3_BD8A_0000E85E86C1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER > 1000

#include <windows.h>
#include "WinMain.h"
#include "WndProc.h"
#include "resource.h"

#define MAXDEVICES 10

#define NOGUID (GUID*)-1
#define NODEVICES (DWORD)0

class CDriverSlector;

class CDriverSelector : public CWndProc
{
public:
    CDriverSelector();
    virtual ~CDriverSelector();
    void AddDriver(const GUID* lpGuid, LPCSTR lpName, LPCSTR lpDescription);
    bool SelectDriver(GUID& Guid);

    // friend CWndProc;
protected:
    void            OnInitDialog();
    void            OnSelectionChange();
    virtual LRESULT WindowProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

    struct DEVICE
    {
        CHAR Name[256], Description[256];
        GUID Guid;
    };
    DWORD  selDevice;
    HWND   hDlg, Icon;
    DEVICE Devices[MAXDEVICES];
    DWORD  numDevices;
};

#endif  // !defined(AFX_DRIVERSELECTOR_H__FA8DAF80_A68F_11D3_BD8A_0000E85E86C1__INCLUDED_)
