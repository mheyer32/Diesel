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
// Direct3D.h: Schnittstelle für die Klasse CDirect3D.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIRECT3D_H__23A3C600_B0D3_11D3_BD8A_0000E85E86C1__INCLUDED_)
#define AFX_DIRECT3D_H__23A3C600_B0D3_11D3_BD8A_0000E85E86C1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER > 1000

#include <InitGuid.h>

#pragma comment(lib, "d3d8.lib")

#define D3D_OVERLOADS

#include <adt/singleton.h>
#include <d3d.h>
#include <ddraw/DirectDraw.h>
#include <ddraw/Surface.h>
#include <messaging/messagingobject.h>
#include <win/DriverSelector.h>

#define D3DINTERFACE IID_IDirect3D7
typedef LPDIRECT3D7       LPD3D;
typedef LPDIRECT3DDEVICE7 LPD3DDEVICE;

// #define ENABLE_BILINEAR(Stage)
// D3DDEV->SetTextureStageState(Stage,D3DTSS_MAGFILTER,D3DTFG_LINEAR);D3DDEV->SetTextureStageState(0,D3DTSS_MINFILTER,D3DTFG_LINEAR);
// #define COPY_TEXTURE(From,To) D3DDEV->Load(To,NULL,From,NULL,0)

#define RGB8 DDPF_RGB
#define RGB16 DDPF_RGB
#define RGB32 DDPF_RGB
#define RGBPAL8 DDPF_RGB | DDPF_PALETTEINDEXED8
#define LUMINANCE DDPF_LUMINANCE
#define ALPHA DDPF_APLHA
#define ALPHAPIXELS DDPF_ALPHAPIXELS

class CDirect3D : public Msg::MessagingObject, public Singleton<CDirect3D>
{

    DECLARE_SINGLETON(CDirect3D)

public:
    bool InitDirect3D(CDirectDraw* DDraw);
    bool EnumDevices(GUID& Guid);
    void ShutDownDirect3D();

    HRESULT setRenderDevice(CSurface* RT, const GUID& guid = IID_IDirect3DHALDevice);
    HRESULT setViewport(const RECT& vprect, D3DVALUE minz = 0.0, D3DVALUE maxz = 1.0);
    void    attachViewport2Window();

    BOOL findTextureFormat(LPDDPIXELFORMAT lpPixelFormat, DWORD Flags, DWORD Colorbits = 0, DWORD Alphabits = 0);

    inline LPD3D       getLPD3D() const { return lpD3D; };
    inline LPD3DDEVICE getDevice() const { return D3DDevice; };

protected:
    struct TEXENUMINFO
    {
        LPDDPIXELFORMAT lpDDPixFmt;
        DWORD           Flags;
        DWORD           FirstBitdepth;
        DWORD           SecondBitdepth;
        BOOL            Found;
    };

    CDirect3D();
    virtual ~CDirect3D();

    virtual Msg::MSGRVAL handleMessage(Msg::MessagingObject* sender, Msg::MESSAGEID msgId,
                                       const Msg::Param& parameters = Msg::Param());

    static HRESULT CALLBACK EnumDevicesCallback(LPSTR lpDeviceDescription, LPSTR lpDeviceName,
                                                LPD3DDEVICEDESC7 lpD3DDeviceDesc, LPVOID lpContext);
    static HRESULT CALLBACK EnumPixelFormatsCallback(LPDDPIXELFORMAT lpDDPixFmt, LPVOID lpContext);

    LPD3D        lpD3D;
    LPD3DDEVICE  D3DDevice;
    CSurface*    RenderTarget;
    CDirectDraw* ddraw;
};

#endif  // !defined(AFX_DIRECT3D_H__23A3C600_B0D3_11D3_BD8A_0000E85E86C1__INCLUDED_)
