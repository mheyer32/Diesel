
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
// Palette.h: Schnittstelle für die Klasse CPalette.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PALETTE_H__8F0B6429_1EA2_11D3_ADDC_0000E85E86C1__INCLUDED_)
#define AFX_PALETTE_H__8F0B6429_1EA2_11D3_ADDC_0000E85E86C1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER > 1000

#include <cassert>
#include "Surface.h"

class CPalette
{
public:
    CPalette();
    CPalette(LPCSTR szPalette);
    CPalette(LPDIRECTDRAWPALETTE Palette);
    virtual ~CPalette();

    LPDIRECTDRAWPALETTE getLPDDP() const { return DDPalette; };

    bool                setFromFile(LPCSTR PalFile);
    bool                setFrom(LPDIRECTDRAWPALETTE Palette);
    bool                setFromSurface(LPDDSURFACE Surface);
    const PALETTEENTRY* getEntries() const { return pe; };

    void CreateConvertTableRGB(LPDDSURFACE DestSurface);
    void CreateConvertTableYUV(LPDDSURFACE DestSurface);

    int getBestColor(float r, float g, float b);
    // int  getShadeOfColor(float red, float green, float blue, float shade, float specular);

    void createShadeTable(int threshold);
    // void CreateFogTable(float rF,float F,float bF, float density, float distance);

    inline BYTE getShade(int color, int shade) const
    {
        assert(ShadeTable);
        assert(color < 256 && color >= 0);
        assert(shade < 256 && shade >= 0);

        return ShadeTable[(color << 8) + shade];
    }

protected:
    inline void Initialize();

    LPDIRECTDRAWPALETTE DDPalette;
    PALETTEENTRY        pe[256];

    DWORD* ConvertTableRGB;
    DWORD* ConvertTableYUV;
    BYTE * ShadeTable, *FogTable;
};

#endif  // !defined(AFX_PALETTE_H__8F0B6429_1EA2_11D3_ADDC_0000E85E86C1__INCLUDED_)
