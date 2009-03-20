
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
// Surface8.cpp: Implementierung der Klasse CCSurface8.
//
//////////////////////////////////////////////////////////////////////

#include "Surface8.h"
#include <cassert>
#include <defs.h>
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////


CSurface8::CSurface8()
{
}


CSurface8::~CSurface8()
{

}


CSurface8::CSurface8(DWORD w, DWORD h,DWORD bc,DWORD caps)
{
	if (!create(w,h,bc,caps)) throw CException("CSurface8::CSurface8() could not create surface");
}


CSurface8::CSurface8(LPDDSURFACE Srfc)
{
 	Surface=Srfc;
	storeSurfaceData();
	Palette.setFromSurface(Surface);
}


CSurface8::CSurface8(LPCSTR file)
{
	createFromFile(file);
	Palette.setFromSurface(Surface);
}

HRESULT CSurface8::create(DWORD w, DWORD h,DWORD bc,DWORD caps)
{
	DDPIXELFORMAT ddpf;
	DXINITSTRUCT(ddpf);
	ddpf.dwFlags=DDPF_RGB|DDPF_PALETTEINDEXED8;
	ddpf.dwRGBBitCount=8;
	HRESULT ddrval=CSurface::create(w,h,bc,caps,&ddpf);
	if (ddrval!=DD_OK) return ddrval;
	assert(Surface); // pointer should be set now
	Palette.setFromSurface(Surface);
	return DD_OK;
}
void CSurface8::loadBitmap(const std::string &File)
{
	CSurface::loadBitmap(File);
	Palette.setFromSurface(Surface);
}

void CSurface8::setPalette(const CPalette &newPalette)
{
	Palette.setFrom(newPalette.getLPDDP());
	Surface->SetPalette(Palette.getLPDDP());
}