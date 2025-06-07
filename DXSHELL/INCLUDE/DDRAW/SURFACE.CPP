
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
// Surface.cpp: Implementierung der Klasse CSurface.
//
//////////////////////////////////////////////////////////////////////

#include "Surface.h"

#include <ddraw/ddrawexception.h>
#include <defs.h>
#include <win/appwindow.h>
#include <cassert>
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CSurface* CSurface::Primary = NULL;

CSurface::CSurface()
{
    DXINITSTRUCT(ddsd);
    ZeroMemory(&ddscaps, sizeof(ddscaps));  // has no dwSize member

    Surface = NULL;
    Clipper = NULL;

    File_loaded   = false;
    Is_BackBuffer = false;
}

CSurface::~CSurface()
{
    if (Clipper != NULL) {
        Clipper->Release();
    };

    if ((Surface != NULL) && (!Is_BackBuffer)) {
        Surface->Release();
        Surface = NULL;
    };

    unregisterForMessage(CAppWindow::Instance(), Msg::RESTORE_SURFACES);
}

CSurface::CSurface(DWORD w, DWORD h, DWORD bc, DWORD caps, LPDDPIXELFORMAT pf)
{
    DXINITSTRUCT(ddsd);
    ZeroMemory(&ddscaps, sizeof(ddscaps));  // has no dwSize member

    Surface = NULL;
    Clipper = NULL;

    File_loaded   = false;
    Is_BackBuffer = false;

    if (pf == NULL && !(caps & DDSCAPS_PRIMARYSURFACE)) {
        throw CException("CSurface::CSurface() pixelformat needed for non-primary surface!");
    }

    HRESULT ddrval = create(w, h, bc, caps, pf);
    if (ddrval != DD_OK) {
        throw CDDrawException("CSurface::CSurface() failed: ", ddrval);
    }
}

void CSurface::createFromFile(const std::string& File)
{
    LPDDRAW lpDDx = CDirectDraw::Instance()->getLPDD();

    Surface = DDLoadBitmap(lpDDx, File.c_str(), 0, 0);
    if (Surface != NULL) {
        Filename    = File;
        File_loaded = true;
        storeSurfaceData();
    };

    registerForMessage(CAppWindow::Instance(), Msg::RESTORE_SURFACES, 16786);
}

// Diese Surface wird Backbuffer von der übergebenen Surface

void CSurface::createAsBackbuffer(CSurface& From)
{
    ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
    HRESULT ddrval = From.Surface->GetAttachedSurface(&ddscaps, &Surface);
    if (ddrval != DD_OK) {
        throw CDDrawException("CSurface::createAsBackbuffer() -> GetAttachedSurface() failed ", ddrval);
        return;
    }
    Is_BackBuffer = true;
    storeSurfaceData();
}

HRESULT CSurface::create(DWORD w, DWORD h, DWORD bc, DWORD caps, LPDDPIXELFORMAT lpDDPixFmt)
{
    // ein gwünschtes PixelFormat  wurde übergeben
    if (lpDDPixFmt != NULL) {
        ddsd.ddpfPixelFormat = *lpDDPixFmt;
    }

    fillSurfaceDesc(w, h, bc, caps);

    LPDDRAW lpDDx  = CDirectDraw::Instance()->getLPDD();
    HRESULT ddrval = lpDDx->CreateSurface(&ddsd, &Surface, NULL);

    if (ddrval != DD_OK) {
        // throw CDDrawException("CSurface::create() failed: ",ddrval);
        return ddrval;
    }

    if (caps & DDSCAPS_PRIMARYSURFACE) {
        Primary               = this;
        CAppWindow* appwindow = CAppWindow::Instance();
        if (!appwindow->isFullscreen()) {
            ddrval = createClipper(appwindow->getHWND());
            if (ddrval != DD_OK) {
                // throw CDDrawException("CSurface::create(): could not create clipper  object",ddrval);
                return ddrval;
            }
        }
    }

    storeSurfaceData();

    if ((ddrval = lock()) != DD_OK) {
        throw CDDrawException("CSurface::create(): initial lock on surface failed", ddrval);
    }
    unlock();

    File_loaded   = false;
    Is_BackBuffer = true;

    registerForMessage(CAppWindow::Instance(), Msg::RESTORE_SURFACES, 16786);

    return DD_OK;
}
/////////////////////////////////////////////////////////////////////////////////////////
// belegt die ddsd.ddsCaps und ddsd.dwFlags entsprechend der angeforderten Caps und Flags
// mit sinnvollen Werten und/oder fügt weitere caps hinzu, je nachdem, was nötig ist
// bzw in Abhängigkeit steht
/////////////////////////////////////////////////////////////////////////////////////////
void CSurface::fillSurfaceDesc(DWORD w, DWORD h, DWORD bc, DWORD caps)
{

    if (caps)
        ddsd.dwFlags = DDSD_CAPS;
    else
        ddsd.dwFlags = 0;

    ddsd.ddsCaps.dwCaps = caps;

    if (bc != 0) {
        ddsd.dwBackBufferCount = bc;
        ddsd.dwFlags |= DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
        ddsd.ddsCaps.dwCaps |= DDSCAPS_FLIP | DDSCAPS_COMPLEX;
    }

    if (caps & (DDSCAPS_LOCALVIDMEM | DDSCAPS_NONLOCALVIDMEM)) {
        ddsd.ddsCaps.dwCaps |= DDSCAPS_VIDEOMEMORY;
    }

    if (caps & DDSCAPS_TEXTURE) {
        ddsd.ddsCaps.dwCaps &=
            ~(DDSCAPS_VIDEOMEMORY | DDSCAPS_SYSTEMMEMORY | DDSCAPS_LOCALVIDMEM | DDSCAPS_NONLOCALVIDMEM);
        ddsd.ddsCaps.dwCaps2 |= DDSCAPS2_TEXTUREMANAGE | DDSCAPS2_HINTDYNAMIC;
    }

    if (!(caps & DDSCAPS_PRIMARYSURFACE)) {
        ddsd.dwFlags |= DDSD_WIDTH | DDSD_HEIGHT;
        ddsd.dwHeight = h;
        ddsd.dwWidth  = w;
        if (!(caps & DDSCAPS_VIDEOMEMORY) || (caps & DDSCAPS_OVERLAY)) {
            ddsd.dwFlags |= DDSD_PIXELFORMAT;
            if (ddsd.ddpfPixelFormat.dwFlags & (DDPF_PALETTEINDEXED1 | DDPF_PALETTEINDEXED2 | DDPF_PALETTEINDEXED4 |
                                                DDPF_PALETTEINDEXED8 | DDPF_PALETTEINDEXEDTO8)) {
                // ddsd.ddsCaps.dwCaps|=DDSCAPS_PALETTE; // funktioniert wohl nicht
            }
            if (ddsd.ddpfPixelFormat.dwFlags & DDPF_ZBUFFER) {
                ddsd.ddsCaps.dwCaps |= DDSCAPS_ZBUFFER;
            }
            if (ddsd.ddpfPixelFormat.dwFlags & DDPF_ALPHA) {
                ddsd.ddsCaps.dwCaps |= DDSCAPS_ALPHA;
            }
        }
    }
}

// Alle wichtigen Daten der Surface holen und damit die Member-Variablen initialisieren

void CSurface::storeSurfaceData()
{
    assert(Surface);

    unsigned long temp;

    HRESULT ddrval = Surface->GetSurfaceDesc(&ddsd);
    if (ddrval != DD_OK) {
        throw CDDrawException("CSurface::storeSurfaceData(): GetSurfaceDesc() failed", ddrval);
        return;
    }

    XMask = temp = ddsd.dwWidth - 1;
    YMask        = ddsd.dwHeight - 1;
    Shift        = 0;
    while (temp > 0) {
        temp >>= 1;
        Shift++;
    }  // Shift,XMask,YMask ist nur für quadratische 2^x Surfaces nützlich

    Surface->GetPixelFormat(&ddsd.ddpfPixelFormat);

    lock();
    unlock();
}

// der Surface einen Clipper zum Fenster hwnd verschaffen
HRESULT CSurface::createClipper(HWND hwnd)
{
    LPDDRAW lpDDx = CDirectDraw::Instance()->getLPDD();

    assert(lpDDx);
    assert(Surface);

    HRESULT ddrval = lpDDx->CreateClipper(NULL, &Clipper, NULL);
    if (ddrval == DD_OK) {
        if (hwnd != NULL) {
            ddrval = Clipper->SetHWnd(NULL, hwnd);
            if (ddrval == DD_OK) {
                ddrval = Surface->SetClipper(Clipper);
            }
        }
    }
    return ddrval;
}

// Bitmap in Surface laden, wird automatisch auf Surface-Größe angepasst
void CSurface::loadBitmap(const std::string& File)
{
    Filename     = File;
    File_loaded  = true;
    HRESULT rval = DDReLoadBitmap(Surface, File.c_str());
    if (rval != S_OK) {
        throw CException("CSurface::loadBitmap() bitmap loading failed for file: ", File);
    }
    registerForMessage(CAppWindow::Instance(), Msg::RESTORE_SURFACES, 16786);
}

// Auto-Restore
Msg::MSGRVAL CSurface::handleMessage(Msg::MessagingObject* sender, Msg::MESSAGEID msgId, const Msg::Param& parameters)
{
    if (msgId == Msg::RESTORE_SURFACES) {
        if (Surface->IsLost() == DDERR_SURFACELOST) {
            HRESULT ddrval = Surface->Restore();
            if (ddrval != DD_OK) {
                throw CDDrawException("CSurface::MessageHandler() Surface-Auto-Restore failed: ", ddrval);
                return Msg::PASS_ON;
            }
            if ((File_loaded == TRUE)) {
                DDReLoadBitmap(Surface, Filename.c_str());
                return Msg::PASS_ON;
            }
        }
    }
    return Msg::PASS_ON;
}
HRESULT CSurface::lock()
{
    assert(Surface);
    HRESULT ddrval = Surface->Lock(NULL, &ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL);
    // ddsd.lPitch ist in Bytes gegeben, muss in das Pixelformat (byte/word/dword) umgerechnet werden.
    Pitch = ddsd.lPitch / (ddsd.ddpfPixelFormat.dwRGBBitCount >> 3);

    return ddrval;
}
// Surface unlocken;
void CSurface::unlock() const
{
    Surface->Unlock(NULL);
}

void CSurface::getRect(RECT& rect) const
{
    rect.left   = 0;
    rect.top    = 0;
    rect.right  = ddsd.dwWidth - 1;
    rect.bottom = ddsd.dwHeight - 1;
    return;
}

void CSurface::clear() const
{
    DDBLTFX ddbltfx = {sizeof(ddbltfx), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    Surface->Blt(NULL, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx);
}

void CSurface::getSurfaceDesc(DDSURFACEDESC2& surfacedesc) const
{
    assert(Surface);
    // DXINITSTRUCT(ddsd);
    // Surface->GetSurfaceDesc(&ddsd);
    surfacedesc = ddsd;
}
void CSurface::getCaps(DDSCAPS2& ddscaps) const
{
    assert(Surface);
    Surface->GetCaps(&ddscaps);
}
void CSurface::getPixelFormat(DDPIXELFORMAT& pixelformat) const
{
    assert(Surface);
    DXINITSTRUCT(pixelformat);
    Surface->GetPixelFormat(&pixelformat);
}

void CSurface::Blit(const CSurface& Source, RECT* dst_rect, RECT* src_rect, DWORD ddbltflags, DDBLTFX* ddbltfx)
{
    assert(Surface);
    assert(Source.Surface);

    Surface->Blt(dst_rect, Source.Surface, src_rect, ddbltflags, ddbltfx);
}
