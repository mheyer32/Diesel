
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
// Surface16.h: Schnittstelle für die Klasse Surface16.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SURFACE16_H__C989F7A1_1C50_11D3_ADDC_0000E85E86C1__INCLUDED_)
#define AFX_SURFACE16_H__C989F7A1_1C50_11D3_ADDC_0000E85E86C1__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif  // _MSC_VER >= 1000

#include "Surface.h"

class CSurface16 : public CSurface
{
public:
    CSurface16();
    CSurface16(DWORD w, DWORD h, DWORD bc = 0, DWORD caps = 0);
    CSurface16(LPDDSURFACE Srfc);
    CSurface16(const std::string& file);
    virtual ~CSurface16();

    static void  setBitMasks(LPDDSURFACE From);
    static DWORD RMask, GMask, BMask;
};
#endif  // !defined(AFX_SURFACE16_H__C989F7A1_1C50_11D3_ADDC_0000E85E86C1__INCLUDED_)
