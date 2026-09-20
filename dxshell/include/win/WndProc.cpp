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
// WndProc.cpp: Implementierung der Klasse CWndProc.
//
//////////////////////////////////////////////////////////////////////

#include "WndProc.h"

#include <iostream>
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CWndProc::CWndProc()
{
}

CWndProc::~CWndProc()
{
}

LRESULT CWndProc::callObjWndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
    CWndProc* object = reinterpret_cast<CWndProc*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    if (object)
        return object->WindowProc(hWnd, Message, wParam, lParam);
    return 0;
}

LRESULT CALLBACK CWndProc::MainWindowProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
    switch (Message) {
    case WM_CREATE: {
        SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)((CREATESTRUCT*)lParam)->lpCreateParams);
        callObjWndProc(hWnd, Message, wParam, lParam);
        return 0;
    } break;

    case WM_DESTROY:
        SetWindowLongPtr(hWnd, GWLP_USERDATA, 0);
        return TRUE;
        break;
    }
    HRESULT rval = callObjWndProc(hWnd, Message, wParam, lParam);
    if (rval)
        return rval;

    return DefWindowProc(hWnd, Message, wParam, lParam);
}

LRESULT CALLBACK CWndProc::MainDialogProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
    switch (Message) {
    case WM_INITDIALOG: {
        SetWindowLongPtr(hWnd, GWLP_USERDATA, lParam);
        callObjWndProc(hWnd, Message, wParam, lParam);
        return TRUE;
    } break;
    case WM_DESTROY:
        SetWindowLongPtr(hWnd, GWLP_USERDATA, 0);
        return TRUE;
        break;
    }

    return callObjWndProc(hWnd, Message, wParam, lParam);
}
