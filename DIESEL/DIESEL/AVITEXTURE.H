/*
This file is part of Diesel
(c) 2002 by Mathias Heyer
email: sonode@gmx.de

Diesel is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Diesel is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
// AVITexture.h: Schnittstelle für die Klasse CAVITexture.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AVITEXTURE_H__EE82D77B_DBE6_47AF_BBD2_D4A2C217BEEF__INCLUDED_)
#define AFX_AVITEXTURE_H__EE82D77B_DBE6_47AF_BBD2_D4A2C217BEEF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER > 1000

#define VC_EXTRA_LEAN
#include <vfw.h>
#include <windows.h>
#include <string>
#include "VideoTexture.h"

#pragma comment(lib, "vfw32.lib")  // Search For VFW32.lib While Linking

class CAVITexture : public CVideoTexture
{
public:
    CAVITexture();
    virtual ~CAVITexture();

    virtual void bindTexture();

    virtual void play();
    virtual void stop();  // fixme rewind, fforward etc missing

    virtual bool open(const std::string& file);
    virtual void close();

protected:
    void grabAVIFrame(int frame);

    int frame;  // Frame Counter

    AVISTREAMINFO    psi;         // Pointer To A Structure Containing Stream Info
    PAVISTREAM       pavi;        // Handle To An Open Stream
    PGETFRAME        pgf;         // Pointer To A GetFrame Object
    BITMAPINFOHEADER bmih;        // Header Information For DrawDibDraw Decoding
    long             num_frames;  // Last Frame Of The Stream
    int              aviwidth;    // Video Width
    int              aviheight;   // Video Height
    float            fps;
    HDRAWDIB         hdd;      // Handle For Our Dib
    HBITMAP          hBitmap;  // Handle To A Device Dependant Bitmap
    HDC              hdc;

    float          starttime;
    bool           playing;
    bool           colorswap_needed;
    unsigned char* pdata;

    static ConVar r_maxVideoSize;
};

#endif  // !defined(AFX_AVITEXTURE_H__EE82D77B_DBE6_47AF_BBD2_D4A2C217BEEF__INCLUDED_)
