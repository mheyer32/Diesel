
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
// Surface16.cpp: Implementierung der Klasse Surface16.
//
//////////////////////////////////////////////////////////////////////


#include "Surface16.h"

#include "ddrawexception.h"
#include <defs.h>

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

		DWORD CSurface16::RMask=0x0000f800,
			  CSurface16::GMask=0x000007e0,
			  CSurface16::BMask=0x0000001f;
	

CSurface16::CSurface16()
{

}

CSurface16::~CSurface16()
{
	
}


CSurface16::CSurface16(DWORD w,DWORD h,DWORD bc,DWORD caps)
{
// Surface anlegen /////////////////////////////////////////////////////////////////////////////
	ddsd.ddpfPixelFormat.dwSize=sizeof(DDPIXELFORMAT);
	ddsd.ddpfPixelFormat.dwFlags=DDPF_RGB;
	ddsd.ddpfPixelFormat.dwRGBBitCount=16;
	ddsd.ddpfPixelFormat.dwRBitMask=RMask;
	ddsd.ddpfPixelFormat.dwGBitMask=GMask;
	ddsd.ddpfPixelFormat.dwBBitMask=BMask;

    create(w,h,bc,caps);

	if (caps&DDSCAPS_PRIMARYSURFACE)
	{
		setBitMasks(Surface);
	}
}


CSurface16::CSurface16(LPDDSURFACE Srfc)
{
	Surface=Srfc;
	storeSurfaceData();
	HRESULT ddrval;
	if ((ddrval=lock())!=DD_OK)
	{
		throw CDDrawException("CSurface16::CSurface16() Initial Lock on Surface failed",ddrval); // Pointer holen
		return;
	}
	unlock();
}


CSurface16::CSurface16(const std::string &file)
{
	createFromFile(file);
	if (lock()!=DD_OK)
	{
		throw CException("CSurface16::CSurface16() Initial Lock on Surface failed"); // Pointer holen
		return;
	}
	unlock();
}

 
void CSurface16::setBitMasks(LPDDSURFACE From)
{
	DDPIXELFORMAT ddpf;

	DXINITSTRUCT(ddpf);

	From->GetPixelFormat(&ddpf);
	RMask=ddpf.dwRBitMask;
	GMask=ddpf.dwGBitMask;
	BMask=ddpf.dwBBitMask;
}


