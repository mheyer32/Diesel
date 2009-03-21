
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
// DirectInput.cpp: Implementierung der Klasse CDirectInput.
//
//////////////////////////////////////////////////////////////////////

#include "DirectInput.h"
#include "dierr.h"
#include <misc/exception.h>
#include <defs.h>

#include <win/WinMain.h>
#include <win/DriverSelector.h>

#include <iostream>

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CDirectInput::CDirectInput()
{
	lpDI=NULL;
}

CDirectInput::~CDirectInput()
{
	ShutDownDirectInput();
}

bool CDirectInput::InitDirectInput()
{
HRESULT dirval;

	std::cout<<std::endl<<"======== CDirectInput::initialize() ========"<<std::endl<<std::endl;

	std::cout<<"DirectInputCreate().... ";
	dirval=DirectInput8Create(getInstanceHandle(),DIRECTINPUT_VERSION, IID_IDirectInput8, (LPVOID*)&lpDI, NULL); 
	
	if (dirval!=DI_OK) 
	{ 
		// DirectInput not available; take appropriate action 
		lpDI=NULL;
		std::cout<<"failed, because\n"<<DIErrorToString(dirval)<<std::endl;
		return false;
	}
	std::cout<<"successful"<<std::endl;

	std::cout<<"DirectInput successfully intialized"<<std::endl<<std::endl;

	return true;
}

void CDirectInput::ShutDownDirectInput()
{
	if (lpDI!=NULL)
	{
		lpDI->Release();
		lpDI=NULL;
	}
}

bool CDirectInput::selectDevice(GUID &Guid,DWORD Flags,DWORD devType)
{
CDriverSelector Selector;
	lpDI->EnumDevices(devType,EnumDevicesCallback, (LPVOID) &Selector,Flags);
	return Selector.SelectDriver(Guid);
}
 
BOOL CALLBACK CDirectInput::EnumDevicesCallback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef)
{
CDriverSelector *Selector;
CHAR temp[255];

	Selector=(CDriverSelector*)pvRef;
	strcpy(temp,"Devicename: ");
	strcat(temp,lpddi->tszProductName);
	strcat(temp,nl);
	strcat(temp,nl);
	switch(GET_DIDEVICE_TYPE(lpddi->dwDevType))
	{
	case DI8DEVTYPE_MOUSE:
		strcat(temp,"Devicetype:  Mouse");
		strcat(temp,nl);
		strcat(temp,nl);
		switch(GET_DIDEVICE_SUBTYPE(lpddi->dwDevType))
		{
		case DI8DEVTYPEMOUSE_TRADITIONAL:
			strcat(temp,"The device is a traditional mouse.");
			break;
		case DI8DEVTYPEMOUSE_FINGERSTICK:
			strcat(temp,"The device is a fingerstick.");
			break;
		case DI8DEVTYPEMOUSE_TOUCHPAD:
			strcat(temp,"The Device is a touchpad.");
			break;
		case DI8DEVTYPEMOUSE_TRACKBALL:
			strcat(temp,"The device is a trackball.");
			break;
		default:
			strcat(temp,"The subtype could not be determined.");
			break;
		}
		strcat(temp,nl);
		break;
	case DI8DEVTYPE_KEYBOARD:
		strcat(temp,"Devicetype:  Keyboard");
		strcat(temp,nl);
		break;
	case DI8DEVTYPE_JOYSTICK:
		strcat(temp,"Devicetype:  Joystick");
		strcat(temp,nl);
		strcat(temp,nl);
		switch(GET_DIDEVICE_SUBTYPE(lpddi->dwDevType))
		{
		case DI8DEVTYPEJOYSTICK_LIMITED:
			strcat(temp,"Joystick that does not provide the minimum number of device objects for action mapping.");
			break;
		case DI8DEVTYPEJOYSTICK_STANDARD:
			strcat(temp,"Standard joystick that provides the minimum number of device objects for action mapping.");
			break;
		default:
			strcat(temp,"The subtype could not be determined.");
			break;
		}
		break;
	default:
		strcat(temp,"Devicetype: unknown");
		break;
	}
	strcat(temp,nl);
	if (HIWORD(lpddi->dwDevType)==DIDEVTYPE_HID)
	{
		strcat(temp,"The device uses the Human Interface Device (HID) protocol.");
	}
	Selector->AddDriver(&lpddi->guidInstance,lpddi->tszInstanceName,temp);
	return DIENUM_CONTINUE;
}





LPDINPUT CDirectInput::getlpDI() const
{
	return lpDI;
}
