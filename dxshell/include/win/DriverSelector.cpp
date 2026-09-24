
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
// DriverSelector.cpp: Implementierung der Klasse CDriverSelector.
//
//////////////////////////////////////////////////////////////////////

#include "DriverSelector.h"

#include <win/AppWindow.h>
#include <win/WinException.h>
#include <win/WinMain.h>
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CDriverSelector::CDriverSelector()
{
    ZeroMemory(&Devices, sizeof(Devices));
    numDevices = NODEVICES;
    selDevice  = 0;
    hDlg       = NULL;
    Icon       = NULL;
}

CDriverSelector::~CDriverSelector()
{
}

void CDriverSelector::AddDriver(const GUID* lpGuid, LPCSTR lpName, LPCSTR lpDescription)
{
    if (numDevices < MAXDEVICES) {
        if (lpGuid != 0)
            Devices[numDevices].Guid = *lpGuid;
        strcpy(Devices[numDevices].Name, lpName);
        if (lpDescription) {
            strcpy(Devices[numDevices].Description, lpDescription);
        }
        ++numDevices;
    };
}

bool CDriverSelector::SelectDriver(GUID& Guid)
{
    BOOL rval;

    if (numDevices == NODEVICES)
        return false;

    rval = DialogBoxParam(getInstanceHandle(), MAKEINTRESOURCE(IDD_DRIVERSELECTOR), CAppWindow::Instance()->getHWND(),
                          (DLGPROC)MainWindowProc, (LPARAM)((CWndProc*)this));

    if (rval == TRUE) {
        Guid = Devices[selDevice].Guid;
        return true;
    } else if (rval == -1) {
        throw CWinException("CDriverSelector::SelectDriver()");
        return false;
    }
    return false;
}

LRESULT CDriverSelector::WindowProc(HWND hDialog, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
    case WM_INITDIALOG:
        hDlg = hDialog;
        // Setup the first devices text
        OnInitDialog();
        return TRUE;

    case WM_COMMAND:
        switch (HIWORD(wParam)) {
        case BN_CLICKED:
            switch (LOWORD(wParam)) {
            case IDOK:
                EndDialog(hDlg, TRUE);
                return TRUE;
                break;
            case IDCANCEL:
                EndDialog(hDlg, FALSE);
                return TRUE;
                break;
            }
            break;

        case CBN_SELCHANGE:
            OnSelectionChange();
            return TRUE;
            break;
        }
        break;
    }
    return FALSE;
}

void CDriverSelector::OnInitDialog()
{
    HWND cb;
    cb = GetDlgItem(hDlg, IDC_DEVICE);

    for (DWORD i = 0; i < numDevices; i++) {
        SendMessage(cb, CB_ADDSTRING, 0, (LPARAM)Devices[i].Name);
    }

    SendMessage(cb, CB_SETCURSEL, 0, 0);
    OnSelectionChange();
    SetFocus(cb);

    Icon = CreateWindowEx(WS_EX_TOPMOST, "static", NULL, WS_VISIBLE | WS_CHILD | SS_CENTER | SS_ICON, 16, 23, 32, 32,
                          hDlg, 0, getInstanceHandle(), 0);
    SendMessage(Icon, STM_SETICON, (WPARAM)LoadIcon(getInstanceHandle(), MAKEINTRESOURCE(IDI_DEVICE)), 0);
}

void CDriverSelector::OnSelectionChange()
{
    LRESULT index;
    HWND    edit, cb;

    cb   = GetDlgItem(hDlg, IDC_DEVICE);
    edit = GetDlgItem(hDlg, IDC_DESCRIPTION);

    index = SendMessage(cb, CB_GETCURSEL, 0, 0);

    selDevice = index;

    SetWindowText(edit, (CHAR*)&Devices[index].Description);
}