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
// ScreenModeRequester.cpp: Implementierung der Klasse CScreenModeRequester.
//
//////////////////////////////////////////////////////////////////////

#include "ScreenmodeRequester.h"

#include <win/AppWindow.h>
#include <win/resource.h>
#include <win/WinException.h>
#include <win/WinMain.h>

#include <commctrl.h>

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CScreenModeRequester::CScreenModeRequester()
{
    numModes   = 0;
    Fullscreen = true;
    FSAALevel  = 0;

    WindowedEnabled = true;
    FSAAEnabled     = true;
}

CScreenModeRequester::~CScreenModeRequester()
{
}

bool CScreenModeRequester::SelectScreenMode(int& Mode, bool& FS, int& FSAA)
{
    int rval;

    maxFSAALevel = FSAA;  // use as max

    rval = DialogBoxParam(getInstanceHandle(), MAKEINTRESOURCE(IDD_SCREENMODE), CAppWindow::Instance()->getHWND(),
                          (DLGPROC)MainDialogProc, (LPARAM)((CWndProc*)this));
    if (rval == -1) {
        throw CWinException("CScreenModeRequester::SelectScreenMode() could not open Dialog:\n");
        return false;
    }

    Mode = selMode;
    FS   = Fullscreen;
    FSAA = FSAALevel;
    return rval;
}

void CScreenModeRequester::OnInitDialog()
{
    HWND list, chkbox;

    INITCOMMONCONTROLSEX icc  = {sizeof(INITCOMMONCONTROLSEX), ICC_WIN95_CLASSES};
    BOOL                 rval = InitCommonControlsEx(&icc);

    if (!rval)
        throw CWinException("CScreenModeRequester::OnInitDialog()->InitCommonControlsEx(&icc)");

    chkbox = GetDlgItem(hDlg, IDC_FULLSCREEN);
    list   = GetDlgItem(hDlg, IDC_MODELIST);

    for (int i = 0; i < numModes; i++) {
        SendMessage(list, LB_ADDSTRING, 0, (LPARAM)Modes[i]);
    }

    SendMessage(list, LB_SETCURSEL, 0, 0);
    SetFocus(list);
    selMode = 0;

    SendMessage(chkbox, BM_SETCHECK, BST_CHECKED, 0);

    if (WindowedEnabled == FALSE) {
        EnableWindow(chkbox, FALSE);
    }

    // place Icon
    Icon = CreateWindowEx(WS_EX_TOPMOST, "static", NULL, WS_VISIBLE | WS_CHILD | SS_CENTER | SS_ICON, 25, 180, 32, 32,
                          hDlg, 0, getInstanceHandle(), 0);
    SendMessage(Icon, STM_SETICON, (WPARAM)LoadIcon(getInstanceHandle(), MAKEINTRESOURCE(IDI_SCREENMODE)), 0);

    // init FSAA slider
    HWND fsaatrkbar = GetDlgItem(hDlg, IDC_FSAASLIDER);
    SendMessage(fsaatrkbar, TBM_SETRANGE, TRUE,
                MAKELONG(0, maxFSAALevel));  // initially set to the value given to SelectScreenMode()
    if (!FSAAEnabled) {
        EnableWindow(fsaatrkbar, FALSE);
    }
}

void CScreenModeRequester::OnSelectionChange()
{
    HWND list;

    list    = GetDlgItem(hDlg, IDC_MODELIST);
    selMode = SendMessage(list, LB_GETCURSEL, 0, 0);
}

void CScreenModeRequester::OnFullscreen()
{
    HWND  chkbox, list;
    DWORD state;
    chkbox = GetDlgItem(hDlg, IDC_FULLSCREEN);
    list   = GetDlgItem(hDlg, IDC_MODELIST);
    state  = SendMessage(chkbox, BM_GETCHECK, 0, 0);
    if (state == BST_CHECKED) {
        EnableWindow(list, TRUE);
        Fullscreen = TRUE;
    } else if (state == BST_UNCHECKED) {
        EnableWindow(list, FALSE);
        Fullscreen = FALSE;
    }
}
void CScreenModeRequester::OnTrackbarChange()
{
    HWND trkbar = GetDlgItem(hDlg, IDC_FSAASLIDER);

    FSAALevel = SendMessage(trkbar, TBM_GETPOS, 0, 0);
}
LRESULT CScreenModeRequester::WindowProc(HWND hDialog, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
            case IDC_FULLSCREEN:
                OnFullscreen();
                return TRUE;
                break;
            case IDOK:
                OnTrackbarChange();  // get slider position
                EndDialog(hDlg, TRUE);
                return TRUE;
                break;
            case IDCANCEL:
                OnTrackbarChange();  // get slider position
                EndDialog(hDlg, FALSE);
                return TRUE;
                break;
            }
            break;
        case LBN_DBLCLK:
            OnTrackbarChange();  // get slider position
            EndDialog(hDlg, TRUE);
            return TRUE;
            break;
        case LBN_SELCHANGE:
            OnSelectionChange();
            return TRUE;
            break;
        }
        break;
    }
    return FALSE;  // DefWindowProc (hDialog,uMsg,wParam,lParam);
}

void CScreenModeRequester::AddScreenMode(const char* ModeString)
{
    if (numModes < MAXMODES) {
        strcpy(Modes[numModes], ModeString);
        ++numModes;
    }
}

void CScreenModeRequester::DisableWindowed()
{
    WindowedEnabled = FALSE;
}

void CScreenModeRequester::DisableFSAA()
{
    FSAAEnabled = FALSE;
}