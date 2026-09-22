
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
// Window.h: Schnittstelle für die Klasse Window.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CWINDOW_H__003F5ABE_752A_41E3_9B59_C010E303BADA__INCLUDED_)
#define AFX_CWINDOW_H__003F5ABE_752A_41E3_9B59_C010E303BADA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN
#include <messaging/messagingobject.h>
#include <win/WndProc.h>
#include <windows.h>
#include <string>

class CWindow : public Msg::MessagingObject, protected CWndProc
{
public:
    CWindow();
    virtual ~CWindow();

    virtual bool open(int nCmdShow = SW_SHOWNORMAL);
    virtual void close();

    virtual void setTitle(const std::string& title);

    bool isFullscreen() const;
    bool setFullscreen(bool fs);

    HACCEL getAcceleratorHandle() const;

    HWND getHWND() const;
    int  getWidth() const;
    int  getHeight() const;

    const RECT& getWindowRect() const;

protected:
    virtual LRESULT WindowProc(HWND hWnd, UINT m, WPARAM w, LPARAM l);

    bool   bFullscreen;
    RECT   window_rect;
    HWND   hwnd;  // windows handle
    HACCEL hAccel;

private:
    /** forbid copying */
    CWindow& operator=(const CWindow& win);
};

#endif  // !defined(AFX_WINDOW_H__003F5ABE_752A_41E3_9B59_C010E303BADA__INCLUDED_)
