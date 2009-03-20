
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
// DirectSound.cpp: Implementierung der Klasse CDirectSound.
//
//////////////////////////////////////////////////////////////////////

#include "DirectSound.h"

#include <win/appwindow.h>
#include <win/DriverSelector.h>
#include "dsoundexception.h"
#include "dserr.h"

#include <iostream>
#include <stdio.h> //sprintf

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////


CDirectSound::CDirectSound()
{
	lpDS=NULL;
}

CDirectSound::~CDirectSound()
{
	ShutDownDirectSound();
}

bool CDirectSound::InitDirectSound(DWORD cooplevel, GUID * lpGuid)
{
	std::cout<<std::endl<<"========  CDirectSound::InitDirectSound() ========"<<std::endl<<std::endl;

	std::cout<<"DirectSoundCreate() ...";
	HRESULT dsrval = DirectSoundCreate(lpGuid, &lpDS, NULL);

	if	(dsrval!=DS_OK)
	{
		std::cout<<"failed, because:"<<std::endl<<DSErrorToString(dsrval)<<std::endl;
		throw CDSoundException("CDirectSound::InitDirectSound() -> DirectSoundCreate failed",dsrval);
		return false;
	}
	std::cout<<"successful"<<std::endl;

	std::cout<<"SetCooperativeLevel() ...";
	dsrval = lpDS->SetCooperativeLevel(CAppWindow::Instance()->getHWND(),cooplevel);

	if (dsrval!=DS_OK)
	{
		std::cout<<"failed, because:"<<std::endl<<DSErrorToString(dsrval)<<std::endl;
		throw CException("CDirectSound::InitDirectSound() -> DirectSound SetCooperative fehlgeschlagen");
		return false;
	}
	std::cout<<"successfull"<<std::endl;

	std::cout<<"DirectSound successfully initialized"<<std::endl<<std::endl;
	return true;
}

void CDirectSound::ShutDownDirectSound()
{
	if (lpDS!=NULL)
	{
		lpDS->Release();
		lpDS=NULL;
	}
}

bool CDirectSound::chooseDevice(GUID &Guid)
{
	CDriverSelector Selector;
	DirectSoundEnumerate(EnumDevicesCallback, (LPVOID) &Selector);
	return 	Selector.SelectDriver(Guid);
}

BOOL CALLBACK CDirectSound::EnumDevicesCallback(GUID * lpGuid,LPCSTR lpDescription,LPCSTR  lpModule,LPVOID lpContext)
{
const CHAR nl[]={13, 10, 0};
CHAR temp[256];
CDriverSelector *Selector;

	Selector=(CDriverSelector*)lpContext;
	sprintf(temp,"Module: %s",lpModule);
	Selector->AddDriver(lpGuid,lpDescription,temp);
	return TRUE; //weitermachen
};



 


