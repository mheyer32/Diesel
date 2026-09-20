
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
// Surface8.h: Schnittstelle für die Klasse CSurface8.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SURFACE8_H__8F0B6427_1EA2_11D3_ADDC_0000E85E86C1__INCLUDED_)
#define AFX_SURFACE8_H__8F0B6427_1EA2_11D3_ADDC_0000E85E86C1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER > 1000

#include "Palette.h"
#include "Surface.h"

class CSurface8 : public CSurface
{

public:
    CSurface8();
    CSurface8(DWORD w, DWORD h, DWORD bc = 0, DWORD caps = 0);
    CSurface8(LPDDSURFACE Srfc);
    CSurface8(LPCSTR file);
    virtual ~CSurface8();

    virtual HRESULT create(DWORD width, DWORD height, DWORD backbuffer_count, DWORD caps);

    virtual void loadBitmap(const std::string& File);

    void      setPalette(const CPalette& newPalette);
    CPalette* getPalette() { return &Palette; };

protected:
    CPalette Palette;
};

#endif  // !defined(AFX_SURFACE8_H__8F0B6427_1EA2_11D3_ADDC_0000E85E86C1__INCLUDED_)
