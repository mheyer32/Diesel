
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
// DirectDraw.cpp: Implementierung der Klasse CDirectDraw.
//
//////////////////////////////////////////////////////////////////////

#include <win/DriverSelector.h>
#include <win/ScreenmodeRequester.h>
#include "DirectDraw.h"

#include "DDrawException.h"

#include <defs.h>
#include <stdio.h>
#include <cassert>
#include <iostream>

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CDirectDraw::CDirectDraw()
{
    lpDDx = NULL;

    appwindow      = NULL;
    run_fullscreen = false;
}

CDirectDraw::~CDirectDraw()
{
    ShutDownDirectDraw();
}

bool CDirectDraw::InitDisplay()
{
    assert(lpDDx);

    MODELIST modelist;
    enumModes(modelist);

    if (modelist.empty())
        return false;

    CScreenModeRequester SMR;

    MODEITERATOR mIt = modelist.begin();
    for (; mIt != modelist.end(); ++mIt) {
        CHARLINE ModeString;
        sprintf(ModeString, "%u x %u x %u @ %uHz", (unsigned)(*mIt).dwWidth, (unsigned)(*mIt).dwHeight,
                (unsigned)(*mIt).ddpfPixelFormat.dwRGBBitCount, (unsigned)(*mIt).dwRefreshRate);
        SMR.AddScreenMode(ModeString);
    }

    DDCAPS ddcaps;
    getCaps(ddcaps);
    // FIXME: detection seems to fail
    if (!(ddcaps.dwCaps2 & DDCAPS2_CANRENDERWINDOWED)) {
        SMR.DisableWindowed();
    }

    SMR.DisableFSAA();

    bool fullscreen;
    int  fsaa, selMode;

    if (SMR.SelectScreenMode(selMode, fullscreen, fsaa)) {
        return InitDisplay(modelist[selMode].dwWidth, modelist[selMode].dwHeight,
                           modelist[selMode].ddpfPixelFormat.dwRGBBitCount, fullscreen);
    } else
        return false;
}

bool CDirectDraw::InitDisplay(DWORD Width, DWORD Height, DWORD Depth, bool FS)
{
    HRESULT ddrval;

    std::cout << std::endl << std::endl << "======== CDirectDraw::InitDisplay()========" << std::endl;
    assert(lpDDx);
    assert(appwindow);

    run_fullscreen = FS;
    // Get exclusive mode

    std::cout << "SetCooperativeLevel() ...";
    if (run_fullscreen) {
        ddrval =
            lpDDx->SetCooperativeLevel(appwindow->getHWND(), DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN | DDSCL_ALLOWREBOOT);
    } else {
        ddrval = lpDDx->SetCooperativeLevel(appwindow->getHWND(), DDSCL_NORMAL);
    }

    if (ddrval != DD_OK) {
        std::cout << "failed, because: " << std::endl << DDErrorToString(ddrval) << std::endl;
        throw CDDrawException("CDirectDraw::InitDisplay() SetCooperativeLevel fehlgeschlagen", ddrval);
        return false;
    };

    std::cout << " successful" << std::endl;

    // Set the video mode
    if (run_fullscreen) {
        std::cout << "SetDisplayMode() ...";
        ddrval = lpDDx->SetDisplayMode(Width, Height, Depth, 0, 0);  // Std Refresh und keine extra Flags
        if (ddrval != DD_OK) {
            std::cout << "failed, because: " << std::endl << DDErrorToString(ddrval) << std::endl;
            throw CDDrawException("CDirectDraw::InitDisplay() -> SetDisplayMode() failed", ddrval);
            return false;
        }
        std::cout << "successful" << std::endl;
        appwindow->setFullscreen(run_fullscreen);  // should be self-detected
    };

    InitMisc();

    std::cout << "displayemode successfully set" << std::endl << std::endl;

    return true;
}

void CDirectDraw::InitMisc()
{
    // Messages, die abgefangen werden müssen
    if (run_fullscreen) {
        std::cout << "registering for window messages" << std::endl;
        registerForMessage(appwindow, Msg::GAME_ACTIVATED);
        registerForMessage(appwindow, Msg::GAME_DEACTIVATED);
        registerForMessage(appwindow, Msg::TESTCOOPLEVEL);
    }
}

bool CDirectDraw::InitDirectDraw(GUID FAR* lpGuid)
{
    HRESULT ddrval;

    ddrval = DirectDrawCreateEx(lpGuid, (LPVOID*)&lpDDx, DDINTERFACE, NULL);

    if (ddrval != DD_OK) {
        throw CDDrawException("CDirectDraw::InitDirectDraw() ->CreateDirectDrawEx failed", ddrval);
        return FALSE;
    }

    appwindow = CAppWindow::Instance();

    return true;
}

void CDirectDraw::ShutDownDirectDraw()
{
    if (CAppWindow::hasInstance()) {
        unregisterForMessage(appwindow, Msg::GAME_ACTIVATED);
        unregisterForMessage(appwindow, Msg::GAME_DEACTIVATED);
        unregisterForMessage(appwindow, Msg::TESTCOOPLEVEL);
    }

    if (lpDDx != NULL) {
        lpDDx->FlipToGDISurface();
        // InitDisplay(0,0,0,FALSE);
        lpDDx->RestoreDisplayMode();
        lpDDx->Release();
        lpDDx = NULL;
    };
}

Msg::MSGRVAL CDirectDraw::handleMessage(Msg::MessagingObject* sender, Msg::MESSAGEID msgId,
                                        const Msg::Param& parameters)
{
    // std::cout<<"CDirectDraw::MessageHandler()"<<std::endl;
    switch (msgId) {
    case Msg::GAME_DEACTIVATED:
        std::cout << "CDirectDraw::MessageHandler() Msg::GAME_DEACTIVATED" << std::endl;
        if (run_fullscreen) {
            appwindow->setReady(false);  // have to wait for correct CoopLevel first
            appwindow->setFullscreen(false);
            ShowWindow(appwindow->getHWND(), SW_MINIMIZE);
        }
        break;
    case Msg::GAME_ACTIVATED:
        std::cout << "CDirectDraw::MessageHandler() Msg::GAME_ACTIVATED" << std::endl;
        if (run_fullscreen) {
            if (CheckCoopLevel()) {
                appwindow->setReady(true);
                SendMessage(appwindow->getHWND(), Msg::RESTORE_SURFACES, 0,
                            0);  // Wenn das CoopLevel stimmt, SOFORT Surfaces wiederherstellen
            }
            appwindow->setFullscreen(true);
        }
        break;
    case Msg::TESTCOOPLEVEL:
        std::cout << "CDirectDraw::MessageHandler() Msg::TESTCOOPLEVEL" << std::endl;
        if (CheckCoopLevel()) {
            std::cout << "CDirectDraw::MessageHandler() set app ready!" << std::endl;
            appwindow->setReady(true);
            SendMessage(appwindow->getHWND(), Msg::RESTORE_SURFACES, 0,
                        0);  // Wenn das CoopLevel stimmt, SOFORT Surfaces wiederherstellen
        }
        break;
    }

    return Msg::PASS_ON;
}

bool CDirectDraw::CheckCoopLevel()
{
    std::cout << "CDirectDraw::CheckCoopLevel(): ";
    if (lpDDx->TestCooperativeLevel() != DD_OK) {
        std::cout << "have to check again..." << std::endl;
        Sleep(500);
        PostMessage(appwindow->getHWND(), Msg::TESTCOOPLEVEL, 0, 0);
        return false;
    }
    std::cout << "cooplevel ok." << std::endl;
    return true;
}

BOOL CDirectDraw::chooseDevice(GUID& Guid)
{
    CDriverSelector Selector;
    DirectDrawEnumerate(EnumDevicesCallback, (LPVOID)&Selector);
    return Selector.SelectDriver(Guid);
}

BOOL WINAPI CDirectDraw::EnumDevicesCallback(GUID* lpGuid, LPSTR lpDescription, LPSTR lpName, LPVOID lpContext)
{

    CDriverSelector* Selector;

    Selector = (CDriverSelector*)lpContext;

    /*	LPDIRECTDRAW7 lpDD;
        HRESULT ddrval=DirectDrawCreate(lpGuid,&lpDD,NULL);

        if (lpDD!=NULL)
        {
            DDDEVICEIDENTIFIER2 ddi;
            CHAR temp[256];
            lpDD->GetDeviceIdentifier(&ddi,0);
            sprintf(temp,"Description: %s%sDriver: %s%sDriver Version:
       %d",ddi.szDescription,nl,ddi.szDriver,nl,ddi.liDriverVersion); Selector->AddDriver(lpGuid,lpDescription,temp);
            Selector->AddDriver(lpGuid,lpDescription,temp);
            lpDD->Release();
            return DDENUMRET_OK; weitermachen
        };
        return DDENUMRET_CANCEL;  Enumeration stoppen*/

    Selector->AddDriver(lpGuid, lpDescription, NULL);
    return DDENUMRET_OK;
};

HRESULT WINAPI CDirectDraw::EnumModesCallback2(LPDDSURFACEDESC2 lpDDSurfaceDesc, LPVOID lpContext)
{
    MODELIST* modelist = reinterpret_cast<MODELIST*>(lpContext);

    modelist->push_back(*lpDDSurfaceDesc);
    return DDENUMRET_OK;
}

HRESULT CDirectDraw::enumModes(MODELIST& modelist, LPDDSURFACEDESC2 lpRefddsd)
{
    HRESULT ddrval;
    ddrval = lpDDx->EnumDisplayModes(0, lpRefddsd, &modelist, EnumModesCallback2);
    return ddrval;
}

HRESULT CDirectDraw::getCaps(DDCAPS& caps) const
{
    // Hardware-Caps holen und speichern
    assert(lpDDx);
    DXINITSTRUCT(caps);
    return lpDDx->GetCaps(&caps, NULL);
};