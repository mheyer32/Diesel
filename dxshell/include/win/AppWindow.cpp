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
// AppWindow.cpp: Implementierung der Klasse CAppWindow.
//
//////////////////////////////////////////////////////////////////////

#include <win/WinMain.h>
#include "AppWindow.h"
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CAppWindow::CAppWindow()
{
    initialize();
}

CAppWindow::~CAppWindow()
{
}

void CAppWindow::GameActivation()
{
    if (bRunning == true) {
        // App-level deactivate / minimize: ignore transient WM_KILLFOCUS during mode switches
        if (!bActive || bMinimized || !bAppActive) {
            PostMessage(hwnd, Msg::GAME_DEACTIVATED, 0, 0);
        }
    } else {
        if (bActive && !bMinimized && bFocus && bAppActive) {
            PostMessage(hwnd, Msg::GAME_ACTIVATED, 0, 0);
        }
    }
}

LRESULT CAppWindow::WindowProc(HWND hWnd, UINT m, WPARAM w, LPARAM l)
{

    CWindow::WindowProc(hWnd, m, w, l);

    LRESULT rval = 1;
    MSG     msg;

    switch (m) {
    case WM_CLOSE:
        bRunning = bReady = false;
        PostQuitMessage(0);
        return 1;  // application will close the window itself!
        break;
    case WM_KEYDOWN:
        switch (w) {
        case VK_ESCAPE:
            bRunning = bReady = false;
            PostQuitMessage(0);
            return 1;
            break;
        }
        break;

    case WM_SIZE:
        if (w == SIZE_MINIMIZED) {
            bMinimized = true;
        } else if ((w == SIZE_RESTORED) || (w == SIZE_MAXIMIZED)) {
            bMinimized = false;
        }
        GameActivation();  // erst jetzt anspringen, damit das Rect richtig behandelt wird
        break;
    case WM_SETFOCUS:
        bFocus = true;
        GameActivation();
        break;
    case WM_KILLFOCUS:
        bFocus = false;
        // do not GameActivation() here — focus blips during CDS/setFullscreen must not pause the game
        break;

    case WM_ACTIVATE:
        bMinimized = HIWORD(w) ? true : false;
        if (LOWORD(w) == WA_INACTIVE) {
            bActive = false;
        } else {
            bActive = true;
        }
        GameActivation();
        break;

    case WM_ACTIVATEAPP:
        bAppActive = w ? true : false;
        GameActivation();
        break;
    case WM_SETCURSOR:
        if (bFullscreen || bMouseGrabbed) {
            SetCursor(NULL);
            return 1;
        }
        break;
    case Msg::GAME_DEACTIVATED:
        bRunning = false;
        releaseMouse();
        break;
    case Msg::GAME_ACTIVATED:
        bRunning = true;
        grabMouse();
        break;
    }

    msg.message = m;
    msg.wParam  = w;
    msg.lParam  = l;

    // spread message to all listeners
    // yeah, I know, this casts unknown message ID´s, but the windows Id´s are always
    // below WM_APP, so ambigous message IDs won´t be created
    sendMessage((Msg::MESSAGEID)msg.message, Msg::WINDOWMESSAGE_PARAM(msg));

    return 0;
}

bool CAppWindow::isActive() const
{
    return bRunning;
}

void CAppWindow::initialize()
{
    bAppActive   = true;
    bActive      = true;
    bMinimized   = false;
    bFocus       = true;
    bReady       = true;
    bRunning     = true;
    bCoopLevelOK = true;
}

bool CAppWindow::open(int nCmdShow)
{
    if (CWindow::open(nCmdShow)) {
        initialize();
        return true;
    }
    return false;
}

void CAppWindow::close()
{
    CWindow::close();

    bAppActive = false;
    bActive    = false;
    bMinimized = false;
    bFocus     = false;
    bRunning   = false;
}

void CAppWindow::setReady(bool ready)
{
    bReady = ready;
}
bool CAppWindow::isReady() const
{
    return bReady;
}