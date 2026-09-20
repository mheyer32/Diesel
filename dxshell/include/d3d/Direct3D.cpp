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
// Direct3D.cpp: Implementierung der Klasse CDirect3D.
//
//////////////////////////////////////////////////////////////////////

#include <ddraw/DDrawException.h>
#include <win/AppWindow.h>
#include "Direct3D.h"

#include <cassert>
#include <iostream>
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CDirect3D::CDirect3D()
{
    lpD3D        = NULL;
    D3DDevice    = NULL;
    RenderTarget = NULL;
    ddraw        = NULL;
}

CDirect3D::~CDirect3D()
{
}

bool CDirect3D::InitDirect3D(CDirectDraw* DDraw)
{
    assert(DDraw);

    ddraw         = DDraw;
    LPDDRAW lpDDx = ddraw->getLPDD();

    HRESULT ddrval = lpDDx->QueryInterface(D3DINTERFACE, (VOID**)&lpD3D);
    if (ddrval != DD_OK) {
        std::cout << "CDirect3D::InitDirect3D(): Failed to create D3DObject: " << DDErrorToString(ddrval) << std::endl;
        return false;
    }
    return true;
}

void CDirect3D::ShutDownDirect3D()
{

    CAppWindow* appwindow = CAppWindow::Instance();

    unregisterForMessage(appwindow, (Msg::MESSAGEID)WM_EXITSIZEMOVE);

    if (D3DDevice != NULL) {
        D3DDevice->Release();
        D3DDevice = NULL;
    }
    if (lpD3D != NULL) {
        lpD3D->Release();
        lpD3D = NULL;
    }
}

HRESULT CDirect3D::setRenderDevice(CSurface* RT, const GUID& guid)
{
    RenderTarget   = RT;
    HRESULT ddrval = lpD3D->CreateDevice(guid, RenderTarget->getSurface(), &D3DDevice);
    if (ddrval != DD_OK) {
        throw CDDrawException("SetRenderDevice fehlgeschlagen:", ddrval);
    }
    return ddrval;
}

HRESULT CDirect3D::setViewport(const RECT& vprect, D3DVALUE minz, D3DVALUE maxz)
{
    DWORD width, height;

    width           = vprect.right - vprect.left;
    height          = vprect.bottom - vprect.top;
    D3DVIEWPORT7 vp = {vprect.left, vprect.top, width, height, minz, maxz};
    return D3DDevice->SetViewport(&vp);
}

void CDirect3D::attachViewport2Window()
{
    D3DVIEWPORT7 vp;

    CAppWindow* appwindow = CAppWindow::Instance();

    registerForMessage(appwindow, (Msg::MESSAGEID)WM_EXITSIZEMOVE);

    D3DDevice->GetViewport(&vp);
    setViewport(appwindow->getWindowRect(), vp.dvMinZ, vp.dvMaxZ);
}

Msg::MSGRVAL CDirect3D::handleMessage(Msg::MessagingObject* sender, Msg::MESSAGEID msgId, const Msg::Param& parameters)
{
    D3DVIEWPORT7 vp;

    switch (msgId) {
    case WM_EXITSIZEMOVE: {
        D3DDevice->GetViewport(&vp);
        CAppWindow* appwindow = CAppWindow::Instance();
        setViewport(appwindow->getWindowRect(), vp.dvMinZ, vp.dvMaxZ);
    } break;
    }

    return Msg::PASS_ON;
}

bool CDirect3D::EnumDevices(GUID& Guid)
{
    CDriverSelector Selector;

    lpD3D->EnumDevices(EnumDevicesCallback, (LPVOID)&Selector);
    return Selector.SelectDriver(Guid);
}

HRESULT CALLBACK CDirect3D::EnumDevicesCallback(LPSTR lpDeviceDescription, LPSTR lpDeviceName,
                                                LPD3DDEVICEDESC7 lpD3DDeviceDesc, LPVOID lpContext)
{
    CDriverSelector* Selector;

    Selector = (CDriverSelector*)lpContext;
    Selector->AddDriver(&lpD3DDeviceDesc->deviceGUID, lpDeviceName, lpDeviceDescription);
    return D3DENUMRET_OK;
}

BOOL CDirect3D::findTextureFormat(LPDDPIXELFORMAT lpPixelformat, DWORD Flags, DWORD FirstBitdepth, DWORD SecondBitdepth)
{
    TEXENUMINFO TexEnumInfo;

    TexEnumInfo.lpDDPixFmt     = lpPixelformat;
    TexEnumInfo.Flags          = Flags;
    TexEnumInfo.FirstBitdepth  = FirstBitdepth;
    TexEnumInfo.SecondBitdepth = SecondBitdepth;
    TexEnumInfo.Found          = FALSE;

    HRESULT ddrval = D3DDevice->EnumTextureFormats(EnumPixelFormatsCallback, (LPVOID)&TexEnumInfo);
    return TexEnumInfo.Found;
};

HRESULT CALLBACK CDirect3D::EnumPixelFormatsCallback(LPDDPIXELFORMAT lpDDPixFmt, LPVOID lpContext)
{
    DWORD        bitcount = 0, secondbits;
    TEXENUMINFO* TexEnumInfo;

    TexEnumInfo = (TEXENUMINFO*)lpContext;

    if ((lpDDPixFmt->dwFlags & TexEnumInfo->Flags) != (TexEnumInfo->Flags))
        return D3DENUMRET_OK;

    if (TexEnumInfo->FirstBitdepth != 0) {
        if (lpDDPixFmt->dwRGBBitCount != TexEnumInfo->FirstBitdepth)
            return D3DENUMRET_OK;
    }

    secondbits = lpDDPixFmt->dwRGBAlphaBitMask;

    if (TexEnumInfo->SecondBitdepth != 0) {
        while (!(secondbits & 1)) {
            secondbits >>= 1;
        }
        while (secondbits) {
            secondbits >>= 1;
            bitcount++;
        }
        if (bitcount < TexEnumInfo->SecondBitdepth)
            return D3DENUMRET_OK;
    }

    // alle Tests bestanden!
    *TexEnumInfo->lpDDPixFmt = *lpDDPixFmt;
    TexEnumInfo->Found       = TRUE;
    return D3DENUMRET_CANCEL;
};
