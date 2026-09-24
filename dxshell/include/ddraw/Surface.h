
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
// Surface.h: Schnittstelle für die Klasse CSurface.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SURFACE_H__8F0B6425_1EA2_11D3_ADDC_0000E85E86C1__INCLUDED_)
#define AFX_SURFACE_H__8F0B6425_1EA2_11D3_ADDC_0000E85E86C1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER > 1000

// FIXME: get rid of these old dirty defines
#define INSYSMEM DDSCAPS_SYSTEMMEMORY
#define INVIDMEM DDSCAPS_VIDEOMEMORY
#define INLOCALVIDMEM DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM
#define OFFSCREEN DDSCAPS_OFFSCREENPLAIN
#define PRIMARY DDSCAPS_PRIMARYSURFACE
#define D3D DDSCAPS_3DDEVICE
#define TEXTURE DDSCAPS_TEXTURE

#include "ddraw/DirectDraw.h"
#include "ddraw/DDUtil.h"
#include "messaging/MessagingObject.h"

#include <string>

typedef IDirectDrawSurface7  DDSURFACE;
typedef LPDIRECTDRAWSURFACE7 LPDDSURFACE;

class CSurface : public Msg::MessagingObject
{
public:
    CSurface(DWORD w, DWORD h, DWORD num_backbuffers, DWORD caps, LPDDPIXELFORMAT pf = NULL);
    CSurface();
    virtual ~CSurface();

    HRESULT lock();
    void    unlock() const;

    virtual HRESULT create(DWORD w, DWORD h, DWORD num_backbuffers, DWORD caps, LPDDPIXELFORMAT lpDDPixFmt = NULL);
    virtual void    createFromFile(const std::string& File);
    virtual void    createAsBackbuffer(CSurface& From);

    virtual void loadBitmap(const std::string& File);

    void getRect(RECT& rect) const;
    void getSurfaceDesc(DDSURFACEDESC2& ddsd) const;
    void getCaps(DDSCAPS2& ddscaps) const;
    void getPixelFormat(DDPIXELFORMAT& pixelformat) const;

    inline LPDDSURFACE getSurface() const { return Surface; };
    inline DWORD       getWidth() const { return ddsd.dwWidth; };
    inline DWORD       getHeight() const { return ddsd.dwHeight; };
    inline DWORD       getPitch() const { return Pitch; };  // in Pixel!
    inline void*       getPointer() const { return ddsd.lpSurface; };
    inline DWORD       getXMask() const { return XMask; };
    inline DWORD       getYMask() const { return YMask; };
    inline DWORD       getShift() const { return Shift; };

    void clear() const;
    void Blit(const CSurface& Source, RECT* dst_rect = NULL, RECT* src_rect = NULL, DWORD ddbltflags = DDBLT_WAIT,
              DDBLTFX* ddbltfx = NULL);

    static CSurface* getPrimary() { return Primary; };

protected:
    void fillSurfaceDesc(DWORD w, DWORD h, DWORD bc, DWORD caps);
    void storeSurfaceData();

    virtual Msg::MSGRVAL handleMessage(Msg::MessagingObject* sender, Msg::MESSAGEID msgId,
                                       const Msg::Param& parameters = Msg::Param());

    HRESULT createClipper(HWND hwnd = NULL);

    DDSURFACEDESC2      ddsd;
    DDSCAPS2            ddscaps;
    DWORD               XMask;
    DWORD               YMask;
    DWORD               Shift;
    DWORD               Pitch;
    DDSURFACE*          Surface;
    IDirectDrawClipper* Clipper;
    bool                File_loaded;
    bool                Is_BackBuffer;
    std::string         Filename;

    static CSurface* Primary;
};

#endif  // !defined(AFX_SURFACE_H__8F0B6425_1EA2_11D3_ADDC_0000E85E86C1__INCLUDED_)
