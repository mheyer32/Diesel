
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
// Joystick.cpp: Implementierung der Klasse CJoystick.
//
//////////////////////////////////////////////////////////////////////

#include "Joystick.h"

#include "DInputException.h"
#include <iostream>
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CJoystick::CJoystick(DWORD CoopLevel)
{
HRESULT dirval;
	GUID Joystick_GUID;

	std::cout<<"======== CJoystick::CJoystick() ========"<<std::endl;


	if (CDirectInput::Instance()->selectDevice(Joystick_GUID,DIEDFL_ATTACHEDONLY  ,DIDEVTYPE_JOYSTICK))
	{

		std::cout<<"found an attached joystick"<<std::endl;
#if DIRECTINPUT_VERSION==0x700
	dirval=InitDevice(Joystick_GUID,(_DIDATAFORMAT *)&c_dfDIJoystick,CoopLevel);
#else
	dirval=InitDevice(Joystick_GUID,NULL,CoopLevel);		
#endif

		SetRange(DIJOFS_X);
		SetRange(DIJOFS_Y);
		SetRange(DIJOFS_Z);
		SetDeadzone(DIJOFS_X);
		SetDeadzone(DIJOFS_Y);
		SetDeadzone(DIJOFS_Z);

		DeviceState=&JoyState;
		DeviceStateSize=sizeof(JoyState);
	}
	else
	{
		throw CException("CJoystick::CJoystick() no attached joystick found");
	}
	std::cout<<"CJoystick::CJoystick()  device successfully initialized"<<std::endl;

}

CJoystick::~CJoystick()
{

}

HRESULT CJoystick::GetDeviceState()
{
	Device->Poll();
	return CDInputDevice::GetDeviceState();
}
 

/////////////////////////////////////////////////////////////////////////////////////////
// Setzt die Range-Property
// Object ist zb: DIJOFS_X,DIJOFS_Y,DIJOFS_Z
/////////////////////////////////////////////////////////////////////////////////////////
HRESULT CJoystick::SetRange(DWORD Object, long Min, long Max)
{
	
   DIPROPRANGE diprg; 

   diprg.diph.dwSize       = sizeof(diprg); 
   diprg.diph.dwHeaderSize = sizeof(diprg.diph); 
   diprg.diph.dwObj        = Object; 
   diprg.diph.dwHow        = DIPH_BYOFFSET; 
   diprg.lMin              = Min; 
   diprg.lMax              = Max; 

   return Device->SetProperty(DIPROP_RANGE, &diprg.diph);
}
/////////////////////////////////////////////////////////////////////////////////////////
// Setzt die Deadzone-Property
// Object ist zb: DIJOFS_X,DIJOFS_Y,DIJOFS_Z
// Min und Max liegen im Bereich zwischen 0 und 10000
/////////////////////////////////////////////////////////////////////////////////////////

HRESULT CJoystick::SetDeadzone(DWORD Object,long Min,long Max)
{
   DIPROPRANGE diprg; 

   diprg.diph.dwSize       = sizeof(diprg); 
   diprg.diph.dwHeaderSize = sizeof(diprg.diph); 
   diprg.diph.dwObj        = Object; 
   diprg.diph.dwHow        = DIPH_BYOFFSET; 
   diprg.lMin              = Min; 
   diprg.lMax              = Max; 

   return Device->SetProperty(DIPROP_DEADZONE, &diprg.diph);
}
