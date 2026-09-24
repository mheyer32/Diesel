
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
// WndProc.h: Schnittstelle für die Klasse CWndProc.
//
//////////////////////////////////////////////////////////////////////

#pragma warning(disable : 4786)

#if !defined(AFX_WNDPROC_H__425A4060_ABCE_11D3_BD8A_0000E85E86C1__INCLUDED_)
#define AFX_WNDPROC_H__425A4060_ABCE_11D3_BD8A_0000E85E86C1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER > 1000

#pragma warning(disable : 4786)  // STL-überlängen-Meldungen unterdruecken

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

class CWndProc
{
protected:
    CWndProc();
    virtual ~CWndProc();

    // will be called if a message arrives
    virtual LRESULT WindowProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam) = NULL;

    // _must_ be used as WindowProcedure
    static LRESULT CALLBACK MainWindowProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK MainDialogProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

private:
    // will distribute Message to correct Object
    static LRESULT callObjWndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
};

#endif  // !defined(AFX_WNDPROC_H__425A4060_ABCE_11D3_BD8A_0000E85E86C1__INCLUDED_)
