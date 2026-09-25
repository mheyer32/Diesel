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
// Window.cpp: Implementierung der Klasse Window.
//
//////////////////////////////////////////////////////////////////////

#include <win/resource.h>
#include <win/WinException.h>
#include <win/WinMain.h>
#include <cassert>
#include <iostream>
#include <string>
#include "Window.h"

#define NAME "DXShell"
#define TITLE "DXShell 0.83"
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CWindow::CWindow()
{
    hwnd          = NULL;
    hAccel        = NULL;
    bFullscreen   = false;
    bMouseGrabbed = false;
}

CWindow::~CWindow()
{
    close();
}

bool CWindow::open(int nCmdShow)
{
    POINT UL, LR;

    if (!hwnd)  // dont open more than once
    {

        WNDCLASS wc;
        /*
         * set up and register window class
         */
        wc.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        wc.lpfnWndProc   = CWndProc::MainWindowProc;
        wc.cbClsExtra    = 0;
        wc.cbWndExtra    = 0;
        wc.hInstance     = getInstanceHandle();
        wc.hIcon         = LoadIcon(getInstanceHandle(), MAKEINTRESOURCE(IDI_APP));
        wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
        wc.lpszMenuName  = TITLE;
        wc.lpszClassName = NAME;

        ATOM atom = RegisterClass(&wc);
        /*	if (!atom)
            {
                DWORD err=GetLastError();
                if (err!=ERROR_CLASS_ALREADY_EXISTS)
                {
                    throw CWinException("CWindow::open() RegisterClass failed: \n");
                    return false;
                }
            }
        */
        /*
         * create a window
         */
        hwnd = CreateWindowEx(0,
                              NAME,
                              TITLE,
                              WS_OVERLAPPEDWINDOW,
                              CW_USEDEFAULT,
                              CW_USEDEFAULT,
                              400,
                              300,
                              NULL,
                              NULL,
                              getInstanceHandle(),
                              (CWndProc*)this);  // this cast is very important!

        if (!hwnd) {
            throw CWinException("CWindow::open() CreateWindowEx failed:\n");
            return false;
        }

        hAccel = LoadAccelerators(getInstanceHandle(), MAKEINTRESOURCE(IDR_ACCELERATOR));
        if (!hAccel) {
            std::cout << "CWindow::open(): LoadAccelerators(IDR_ACCELERATOR) failed" << std::endl;
        }
    }
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    GetClientRect(hwnd, &window_rect);
    UL.x = window_rect.left;
    UL.y = window_rect.top;
    LR.x = window_rect.right + 1;
    LR.y = window_rect.bottom + 1;

    ClientToScreen(hwnd, &UL);
    ClientToScreen(hwnd, &LR);

    window_rect.left   = UL.x;
    window_rect.top    = UL.y;
    window_rect.right  = LR.x;
    window_rect.bottom = LR.y;

    return true;
}

void CWindow::close()
{
    releaseMouse();
    if (hwnd) {
        DestroyWindow(hwnd);
    }
    hwnd = NULL;
}

LRESULT CWindow::WindowProc(HWND hWnd, UINT m, WPARAM w, LPARAM l)
{
    switch (m) {
    case WM_SIZE:
    case WM_MOVE:
        GetClientRect(hwnd, &window_rect);
        ClientToScreen(hwnd, (LPPOINT)&window_rect.left);
        ClientToScreen(hwnd, (LPPOINT)&window_rect.right);
        if (bMouseGrabbed)
            grabMouse();
        break;
    }

    return 0;
}

const RECT& CWindow::getWindowRect() const
{
    return window_rect;
}

HWND CWindow::getHWND() const
{
    return hwnd;
}

HACCEL CWindow::getAcceleratorHandle() const
{
    return hAccel;
}

int CWindow::getWidth() const
{
    return window_rect.right - window_rect.left + 1;
}

int CWindow::getHeight() const
{
    return window_rect.bottom - window_rect.top + 1;
};

void CWindow::setTitle(const std::string& title)
{
    SetWindowText(hwnd, title.c_str());
}

bool CWindow::setFullscreen(bool fs)
{
    DWORD WStyle;

    if (fs) {
        WStyle = GetWindowLong(hwnd, GWL_STYLE);
        WStyle &= ~(WS_OVERLAPPEDWINDOW);
        WStyle |= WS_POPUP;
        SetWindowLong(hwnd, GWL_STYLE, WStyle);

        MONITORINFO mi;
        mi.cbSize = sizeof(mi);
        GetMonitorInfo(MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST), &mi);

        // Cover the full monitor — SW_SHOWMAXIMIZED only fills the work area
        // (taskbar stays visible/interactive after a focus loss/regain cycle).
        SetWindowPos(hwnd,
                     HWND_TOPMOST,
                     mi.rcMonitor.left,
                     mi.rcMonitor.top,
                     mi.rcMonitor.right - mi.rcMonitor.left,
                     mi.rcMonitor.bottom - mi.rcMonitor.top,
                     SWP_FRAMECHANGED | SWP_SHOWWINDOW);
        SetForegroundWindow(hwnd);
        SetFocus(hwnd);
        bFullscreen = true;
        if (bMouseGrabbed)
            grabMouse();
        return true;
    } else {
        WStyle = GetWindowLong(hwnd, GWL_STYLE);
        WStyle &= ~WS_POPUP;
        WStyle |= WS_OVERLAPPEDWINDOW;
        SetWindowLong(hwnd, GWL_STYLE, WStyle);
        SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED | SWP_NOACTIVATE);
        bFullscreen = false;
        // Avoid restore+focus when leaving exclusive mode due to focus loss —
        // caller may ShowWindow(SW_SHOWMINNOACTIVE) next. Stealing focus here
        // would immediately GAME_ACTIVATED → fullscreen again.
        if (GetForegroundWindow() == hwnd && !IsIconic(hwnd)) {
            ShowWindow(hwnd, SW_RESTORE);
            SetFocus(hwnd);
        }
        if (bMouseGrabbed)
            grabMouse();
        return true;
    }

    return false;
}

bool CWindow::isFullscreen() const
{
    return bFullscreen;
}

void CWindow::grabMouse()
{
    if (!hwnd)
        return;

    RECT rc;
    GetClientRect(hwnd, &rc);
    MapWindowPoints(hwnd, NULL, (POINT*)&rc, 2);
    ClipCursor(&rc);
    SetCapture(hwnd);
    SetCursor(NULL);
    bMouseGrabbed = true;
}

void CWindow::releaseMouse()
{
    if (!bMouseGrabbed)
        return;

    ClipCursor(NULL);
    if (hwnd && GetCapture() == hwnd)
        ReleaseCapture();
    SetCursor(LoadCursor(NULL, IDC_ARROW));
    bMouseGrabbed = false;
}
