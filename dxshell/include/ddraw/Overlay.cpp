
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
// Overlay.cpp: Implementierung der Klasse COverlay.
//
//////////////////////////////////////////////////////////////////////

#include "Overlay.h"

#include <ddraw/DDrawException.h>
#include <defs.h>
#include <win/AppWindow.h>

#include <iostream>

#include <MemoryTracker.h>
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

COverlay::COverlay()
{
    OverlayFlags = 0;
    OverlayFX    = NULL;
    KeepAspect   = FALSE;
    ShowOverlay  = FALSE;

    appwindow = CAppWindow::Instance();
    destRect  = appwindow->getWindowRect();
}

COverlay::COverlay(DWORD w, DWORD h, DWORD bc, DWORD caps, DDPIXELFORMAT* pf)
{
    OverlayFlags = 0;
    OverlayFX    = NULL;
    KeepAspect   = FALSE;
    ShowOverlay  = FALSE;

    appwindow = CAppWindow::Instance();
    destRect  = appwindow->getWindowRect();

    if (this->create(w, h, bc, caps, pf) != DD_OK) {
        throw CException("COverlay::COverlay(...) failed");
    }
}

COverlay::~COverlay()
{
    unregisterForMessage(appwindow, Msg::GAME_ACTIVATED);
    unregisterForMessage(appwindow, Msg::GAME_DEACTIVATED);
    unregisterForMessage(appwindow, (Msg::MESSAGEID)WM_SIZE);
    unregisterForMessage(appwindow, (Msg::MESSAGEID)WM_MOVE);

    if (Surface != NULL) {
        Hide();
    }

    KILLOBJECT(OverlayFX)
}

HRESULT COverlay::create(DWORD w, DWORD h, DWORD bc, DWORD caps, DDPIXELFORMAT* pf)
{
    HRESULT ddrval = DD_OK;

    OverlayFlags = DDOVER_HIDE;
    OverlayFX    = NULL;
    KeepAspect   = FALSE;
    ShowOverlay  = FALSE;

    if (!OverlaySupport()) {
        throw CException("Overlay not supported by Hardware");
    }

    caps |= DDSCAPS_OVERLAY | DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM;
    if (pf != NULL) {
        ddrval = CSurface::create(w, h, bc, caps, pf);
        if (ddrval != DD_OK) {
            throw CDDrawException("COverlay::Overlay() Failed to create Overlay-Surface: ", ddrval);
            return ddrval;
        }
    } else {
        /*	ddsd.ddpfPixelFormat.dwFlags=DDPF_RGB;//DDPF_YUV ;
            ddsd.ddpfPixelFormat.dwRGBBitCount=16;
            ddsd.ddpfPixelFormat.dwRBitMask=RMask;
            ddsd.ddpfPixelFormat.dwGBitMask=GMask;
            ddsd.ddpfPixelFormat.dwBBitMask=BMask;
            */

        std::cout << "trying to use primary surface´s pixelformat... ";
        Primary->getPixelFormat(ddsd.ddpfPixelFormat);
        ddrval = CSurface::create(w, h, bc, caps);

        if (ddrval != DD_OK) {
            std::cout << "failed" << std::endl;
            std::cout << "trying FOURCC pixelformats:" << std::endl;

            DWORD   NumCodes;
            LPDDRAW lpDDx = CDirectDraw::Instance()->getLPDD();
            lpDDx->GetFourCCCodes(&NumCodes, NULL);

            std::cout << NumCodes << " available FOURCC codes" << std::endl;

            union FOURCC
            {
                DWORD code;
                char  name[4];
            };

            FOURCC* FourCCCodes = new FOURCC[NumCodes];
            lpDDx->GetFourCCCodes(&NumCodes, &FourCCCodes[0].code);
            ddsd.ddpfPixelFormat.dwFlags = DDPF_FOURCC;  //|DDPF_RGBTOYUV;
            // Try to create an overlay surface using one of the pixel formats in our
            // global list.

            DWORD i = 0;
            do {
                std::cout << FourCCCodes[i].name << "... ";
                ddsd.ddpfPixelFormat.dwFourCC = FourCCCodes[i++].code;
                ddrval                        = CSurface::create(w, h, bc, caps);
                if (ddrval != DD_OK) {
                    std::cout << "failed" << std::endl;
                } else {
                    std::cout << "succeeded" << std::endl;
                }
            } while ((ddrval != DD_OK) && (i < NumCodes));

            delete[] FourCCCodes;

            if (ddrval != DD_OK) {
                throw CDDrawException("COverlay::Overlay16(...) Failed to create Overlay-Surface", ddrval);
                return ddrval;
            }
        }
    }

    ddrval = GetOverlayCaps();

    if (ddrval != DD_OK) {
        throw CDDrawException("COverlay::Overlay16() failed to verify overlay capablilities: ", ddrval);
        return ddrval;
    }
    registerForMessage(appwindow, Msg::GAME_ACTIVATED);
    registerForMessage(appwindow, Msg::GAME_DEACTIVATED);
    return DD_OK;
}

BOOL COverlay::OverlaySupport()
{
    // Get driver capabilities to determine Overlay support.
    /*  ZeroMemory(&ddcaps, sizeof(ddcaps));
        ddcaps.dwSize = sizeof(ddcaps);

        ddrval = lpDDx->GetCaps(&ddcaps, NULL);
        if (ddrval!=DD_OK)
            return FALSE;
     */
    // Does the driver support overlays in the current mode?
    // (Currently the DirectDraw emulation layer does not support overlays.
    // Overlay related APIs will fail without hardware support).

    DDCAPS ddcaps;
    CDirectDraw::Instance()->getCaps(ddcaps);

    if (!(ddcaps.dwCaps & DDCAPS_OVERLAY))
        return FALSE;

    return TRUE;
}

Msg::MSGRVAL COverlay::handleMessage(Msg::MessagingObject* sender, Msg::MESSAGEID msgId, const Msg::Param& parameters)
{
    CSurface::handleMessage(sender, msgId, parameters);

    switch (msgId) {
    case WM_SIZE:
    case WM_MOVE:
        if (appwindow->isActive()) {
            HRESULT ddrval = UpdateOverlay(&appwindow->getWindowRect());
            if (ddrval != DD_OK) {
                throw CException(std::string("COverlay::UpdateOverlay() failed:\n ") +
                                 std::string(DDErrorToString(ddrval)));
            }
        }
        break;
    case Msg::GAME_ACTIVATED:
        if (ShowOverlay)
            Show();
        break;
    case Msg::GAME_DEACTIVATED:
        BOOL s = ShowOverlay;
        Hide();
        ShowOverlay = s;
        break;
    }

    return Msg::PASS_ON;
}

HRESULT COverlay::GetOverlayCaps()
{
    HRESULT ddrval;

    DDCAPS ddcaps;
    // Get driver capabilities
    ddrval = CDirectDraw::Instance()->getCaps(ddcaps);  // nur Hardware-Caps holen
    if (ddrval != DD_OK)
        return ddrval;

    // Check the minimum stretch and set the local variable accordingly.
    if (ddcaps.dwCaps & DDCAPS_OVERLAYSTRETCH) {
        if (ddcaps.dwMinOverlayStretch != 0)
            minStretch = ddcaps.dwMinOverlayStretch;
        else
            minStretch = 1000;
        if (ddcaps.dwMaxOverlayStretch != 0)
            maxStretch = ddcaps.dwMaxOverlayStretch;
        else
            maxStretch = 5000;  // Soll Stretchen können, aber keine Min/Max-Werte angegeben?
    } else {
        minStretch = 1000;
        maxStretch = 1000;
    }
    return DD_OK;
}

HRESULT COverlay::UpdateOverlay(const RECT* TargetRect)
{
    DWORD xScale, yScale, destWidth, destHeight, srcWidth, srcHeight, temp;
    BOOL  AspectSet = FALSE;

    // Set initial values in the source RECT.

    if (TargetRect == NULL) {
        // OverlayFlags|=DDOVER_SHOW;
        HRESULT ddrval = Surface->UpdateOverlay(&srcRect, Primary->getSurface(), &destRect, OverlayFlags, OverlayFX);
        return ddrval;
    }

    destRect       = *TargetRect;
    srcRect.left   = 0;
    srcRect.top    = 0;
    srcRect.right  = ddsd.dwWidth - 1;
    srcRect.bottom = ddsd.dwHeight - 1;

    DDCAPS ddcaps;
    CDirectDraw::Instance()->getCaps(ddcaps);

SetAspect:

    destWidth  = destRect.right - destRect.left + 1;
    destHeight = destRect.bottom - destRect.top + 1;
    srcWidth   = srcRect.right - srcRect.left + 1;
    srcHeight  = srcRect.bottom - srcRect.top + 1;

    xScale = (destWidth * 1000) / srcWidth;
    yScale = (destHeight * 1000) / srcHeight;

    // min/max - Stretch Werte einhalten.
    if (xScale > maxStretch) {
        destRect.right = destRect.left + (ddsd.dwWidth * maxStretch + 999) / 1000;
        goto SetAspect;
    } else {
        if (xScale < minStretch) {
            srcRect.right -= (ddsd.dwWidth * minStretch - destWidth * 1000 + 999) / 1000;
            goto SetAspect;
        }
    }
    if (yScale > maxStretch) {
        destRect.bottom = destRect.top + (ddsd.dwHeight * maxStretch + 999) / 1000;
        goto SetAspect;
    } else {
        if (yScale < minStretch) {
            srcRect.bottom -= (ddsd.dwHeight * minStretch - destHeight * 1000 + 999) / 1000;
            goto SetAspect;
        }
    }
    // auf Aspect-Ratio acht geben (oder nicht :-)

    if ((KeepAspect == TRUE) && (!AspectSet)) {
        AspectSet = TRUE;  // Nur 1 (!!) mal korrigieren!
        if (yScale < xScale) {
            xScale         = yScale;
            destRect.right = destRect.left + (srcWidth * xScale + 999) / 1000;
            goto SetAspect;
        } else {
            yScale          = xScale;
            destRect.bottom = destRect.top + (srcHeight * yScale + 999) / 1000;
            goto SetAspect;
        }
    }

    // Clipping auf Ränder der Primary-Surface, wenn es nicht
    // durch Hardware unterstützt wird.
    if (ddcaps.dwCaps & DDCAPS_OVERLAYCANTCLIP) {
        int height = Primary->getHeight();
        int width  = Primary->getWidth();
        if (destRect.right > width - 1) {
            temp           = destRect.right - width + 1;
            destRect.right = width - 1;
            temp           = (temp * 1000 + 999) / xScale;
            srcRect.right -= temp;
            srcWidth -= temp;
        }
        if (destRect.left < 0) {
            temp         = (abs(destRect.left) * 1000 + 999) / xScale;
            srcRect.left = temp;
            srcWidth -= temp;
            destRect.left = 0;
        }

        if (destRect.bottom > height - 1) {
            temp            = destRect.bottom - height + 1;
            destRect.bottom = height - 1;
            temp            = (temp * 1000 + 999) / yScale;
            srcRect.bottom -= temp;
        }

        if (destRect.top < 0) {
            temp        = (abs(destRect.left) * 1000 + 999) / xScale;
            srcRect.top = temp;
            srcHeight -= temp;
            destRect.top = 0;
        }
        destHeight = destRect.bottom - destRect.top;
        destWidth  = destRect.right - destRect.left;
    }

    // und auf Alignments achten.
    if (ddcaps.dwCaps & DDCAPS_ALIGNBOUNDARYSRC) {
        srcRect.left -= srcRect.left % ddcaps.dwAlignBoundarySrc;
        if (ddcaps.dwCaps & DDCAPS_ALIGNSIZESRC)
            srcRect.right = srcRect.left + srcWidth - (srcWidth % ddcaps.dwAlignSizeSrc);
        else
            srcRect.right = srcRect.left + srcWidth;
    }
    if (ddcaps.dwCaps & DDCAPS_ALIGNBOUNDARYDEST) {
        destRect.left -= destRect.left % ddcaps.dwAlignBoundaryDest;
        if (ddcaps.dwCaps & DDCAPS_ALIGNSIZEDEST)
            destRect.right = destRect.left + destWidth - (destWidth % ddcaps.dwAlignSizeDest);
        else
            destRect.right = destRect.left + destWidth;
    }

    HRESULT ddrval = Surface->UpdateOverlay(&srcRect, Primary->getSurface(), &destRect, OverlayFlags, OverlayFX);
    //	debug("Overlay16::UpdateOverlay()  %s\n", DDErrorToString(ddrval));
    return ddrval;
}
HRESULT COverlay::Show()
{
    ShowOverlay = TRUE;
    OverlayFlags &= ~DDOVER_HIDE;
    OverlayFlags |= DDOVER_SHOW;
    return Surface->UpdateOverlay(NULL, Primary->getSurface(), &destRect, OverlayFlags, OverlayFX);
}

HRESULT COverlay::Hide()
{
    ShowOverlay = FALSE;
    OverlayFlags &= ~DDOVER_SHOW;
    OverlayFlags |= DDOVER_HIDE;
    return Surface->UpdateOverlay(NULL, Primary->getSurface(), NULL, DDOVER_HIDE, NULL);
}

HRESULT COverlay::Screensize()
{
    destRect.left   = 0;
    destRect.top    = 0;
    destRect.right  = Primary->getWidth() - 1;
    destRect.bottom = Primary->getHeight() - 1;
    return Surface->UpdateOverlay(NULL, Primary->getSurface(), &destRect, OverlayFlags, OverlayFX);
}

HRESULT COverlay::DestAlpha(DWORD AlphaValue)
{
    DDCAPS ddcaps;
    CDirectDraw::Instance()->getCaps(ddcaps);

    if (ddcaps.dwFXAlphaCaps & DDFXALPHACAPS_OVERLAYALPHAPIXELS) {
        CreateOverlayFX();
        OverlayFlags |= DDOVER_ALPHADESTCONSTOVERRIDE;
        OverlayFX->dwAlphaDestConst = AlphaValue;
        return UpdateOverlay();
    }
    return DDERR_NOALPHAHW;
}
HRESULT COverlay::SourceAlpha(DWORD AlphaValue)
{
    DDCAPS ddcaps;
    CDirectDraw::Instance()->getCaps(ddcaps);
    if (ddcaps.dwFXAlphaCaps & DDFXALPHACAPS_OVERLAYALPHAPIXELS) {
        CreateOverlayFX();
        OverlayFlags |= DDOVER_ALPHASRCCONSTOVERRIDE;
        OverlayFX->dwAlphaSrcConst = AlphaValue;
        return UpdateOverlay();
    }
    return DDERR_NOALPHAHW;
}

HRESULT COverlay::SourceColorKey(DWORD ColorKey)
{
    DDCAPS ddcaps;
    CDirectDraw::Instance()->getCaps(ddcaps);
    if (ddcaps.dwCKeyCaps & DDCKEYCAPS_SRCOVERLAY) {
        CreateOverlayFX();
        OverlayFlags |= DDOVER_KEYSRCOVERRIDE;
        OverlayFX->dckSrcColorkey.dwColorSpaceLowValue  = ColorKey;
        OverlayFX->dckSrcColorkey.dwColorSpaceHighValue = ColorKey;
        return UpdateOverlay();
    }
    return DDERR_NOCOLORKEYHW;
}

HRESULT COverlay::DestColorKey(DWORD ColorKey)
{
    DDCAPS ddcaps;
    CDirectDraw::Instance()->getCaps(ddcaps);
    if (ddcaps.dwCKeyCaps & DDCKEYCAPS_DESTOVERLAY) {
        CreateOverlayFX();
        OverlayFlags |= DDOVER_KEYDESTOVERRIDE;
        OverlayFX->dckDestColorkey.dwColorSpaceLowValue  = ColorKey;
        OverlayFX->dckDestColorkey.dwColorSpaceHighValue = ColorKey;
        return UpdateOverlay();
    }
    return DDERR_NOCOLORKEYHW;
}

void COverlay::CreateOverlayFX()
{
    if (OverlayFX == NULL) {
        OverlayFX = new (DDOVERLAYFX);
        ZeroMemory(OverlayFX, sizeof(DDOVERLAYFX));
        OverlayFX->dwSize = sizeof(DDOVERLAYFX);
    }
}

void COverlay::AttachToWindow(BOOL KA)
{
    KeepAspect = KA;
    UpdateOverlay(&appwindow->getWindowRect());
    registerForMessage(appwindow, (Msg::MESSAGEID)WM_SIZE);
    registerForMessage(appwindow, (Msg::MESSAGEID)WM_MOVE);
}
