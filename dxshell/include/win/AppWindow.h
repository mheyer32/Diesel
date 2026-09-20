
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
// AppWindow.h: Schnittstelle für die Klasse CAppWindow.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_APPWINDOW_H__BAF238FF_E8D2_4B85_A252_49ECB330B0C3__INCLUDED_)
#define AFX_APPWINDOW_H__BAF238FF_E8D2_4B85_A252_49ECB330B0C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER > 1000

#include "Window.h"

#include <messaging/MessagingObject.h>
#include <adt/Singleton.h>

/*//////////////////////////////////////////////////////////////
User-defined Messages
//////////////////////////////////////////////////////////////*/

class CAppWindow : public CWindow, public Singleton<CAppWindow>
{

    DECLARE_SINGLETON(CAppWindow)

public:
    bool isActive() const;

    void setReady(bool ready);
    bool isReady() const;

    virtual bool open(int nCmdShow = SW_SHOWNORMAL);
    virtual void close();

protected:
    CAppWindow();
    virtual ~CAppWindow();

    void initialize();
    void GameActivation();
    // will be called if a message arrives
    // implements CWndProc::WindowProc
    virtual LRESULT WindowProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

    bool bAppActive;
    bool bActive;     // is application active?
    bool bMinimized;  // minimiert?
    bool bFocus;      // Focus?
    bool bReady;      // darf gerendert werden ?
    bool bRunning;
    bool bCoopLevelOK;  // TestCooperativeLevel() ging gut oder schlug fehl
};

#endif  // !defined(AFX_APPWINDOW_H__BAF238FF_E8D2_4B85_A252_49ECB330B0C3__INCLUDED_)
