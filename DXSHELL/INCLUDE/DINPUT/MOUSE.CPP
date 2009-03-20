
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
// Mouse.cpp: Implementierung der Klasse CMouse.
//
//////////////////////////////////////////////////////////////////////

#include "Mouse.h"
#include <misc/exception.h>
#include <iostream>
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CMouse::CMouse()
{

}

CMouse::~CMouse()
{

}

bool CMouse::initialize(DWORD cooplevel)
{

	std::cout<<"======== CMouse::initialize() ========"<<std::endl;
	if(InitDevice(GUID_SysMouse,(_DIDATAFORMAT *)&c_dfDIMouse,cooplevel)!=TRUE)
	{
		std::cout<<"CMouse::initialize() failed"<<std::endl;
		return false;
	}

	DeviceState=&MouseState;
	DeviceStateSize=sizeof(MouseState);
	Axes=&MouseState.lX;
	Buttons=MouseState.rgbButtons;
	POV=NULL;

	std::cout<<"CMouse::initialize() successful"<<std::endl;
	return true;
}