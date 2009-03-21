
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
// DirectInput.h: Schnittstelle für die Klasse CDirectInput.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIRECTINPUT_H__23864E60_8980_11D3_AF5D_0080AD17B2BB__INCLUDED_)
#define AFX_DIRECTINPUT_H__23864E60_8980_11D3_AF5D_0080AD17B2BB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef DIRECTINPUT_VERSION
#undef DIRECTINPUT_VERSION
#endif

#define DIRECTINPUT_VERSION 0x0800

#include <dinput.h>

typedef LPDIRECTINPUT8			LPDINPUT;
typedef LPDIRECTINPUTDEVICE8 	LPDINPUTDEVICE;

#pragma comment( lib, "dinput8.lib" )
#pragma comment( lib, "dxguid.lib" )

#include <adt/Singleton.h>

class CDirectInput: public Singleton<CDirectInput>
{
DECLARE_SINGLETON(CDirectInput)

public:

	bool InitDirectInput();
	void ShutDownDirectInput();

	LPDINPUT getlpDI() const;	

	//opens a requester where all devices specified with "Flags" are listed
	//if a device was successfully selected, its GUID is return in guid and
	// the function returns true
	bool selectDevice(GUID &Guid,DWORD Flags=DIEDFL_ALLDEVICES,DWORD devType=0);

protected:
	CDirectInput();	
	virtual ~CDirectInput();

	static BOOL CALLBACK EnumDevicesCallback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef);

	LPDINPUT	lpDI;
};

#endif // !defined(AFX_DIRECTINPUT_H__23864E60_8980_11D3_AF5D_0080AD17B2BB__INCLUDED_)
