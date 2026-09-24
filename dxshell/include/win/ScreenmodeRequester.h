
/*
This file is part of DXShell
(c) 2002-2026 by Mathias Heyer

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
// ScreenModeRequester.h: Schnittstelle für die Klasse CScreenModeRequester.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCREENMODEREQUESTER_H__957945E0_AD9D_11D3_BD8A_0000E85E86C1__INCLUDED_)
#define AFX_SCREENMODEREQUESTER_H__957945E0_AD9D_11D3_BD8A_0000E85E86C1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER > 1000

#pragma comment(lib, "Comctl32.lib")

#include <defs.h>
#include "WndProc.h"

#define MAXMODES 300

class CScreenModeRequester : public CWndProc
{
    friend CWndProc;

public:
    CScreenModeRequester();
    virtual ~CScreenModeRequester();

    bool SelectScreenMode(int& Mode, bool& fullscreen, int& FSAA);
    void DisableWindowed();
    void DisableFSAA();
    void AddScreenMode(const char* ModeString);

private:
    char Modes[MAXMODES][256];
    int  numModes, selMode;
    bool Fullscreen;
    int  FSAALevel;
    int  maxFSAALevel;

    bool WindowedEnabled;
    bool FSAAEnabled;
    HWND hDlg, Icon;

    void OnInitDialog();
    void OnSelectionChange();
    void OnFullscreen();
    void OnTrackbarChange();

    virtual LRESULT WindowProc(HWND hDialog, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

#endif  // !defined(AFX_SCREENMODEREQUESTER_H__957945E0_AD9D_11D3_BD8A_0000E85E86C1__INCLUDED_)
