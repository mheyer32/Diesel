
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
// DInputDevice.h: Schnittstelle für die Klasse CDInputDevice.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DINPUTDEVICE_H__31079B00_8D92_11D3_BD8A_0000E85E86C1__INCLUDED_)
#define AFX_DINPUTDEVICE_H__31079B00_8D92_11D3_BD8A_0000E85E86C1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DirectInput.h"
#include <messaging/MessagingObject.h>
#include <list>

class CDInputDevice :protected Msg::MessagingObject
{

public:

	CDInputDevice();
	virtual ~CDInputDevice();

	bool InitDevice(REFGUID DeviceGUID, DIDATAFORMAT *DataFormat=NULL,DWORD cooplevel=DISCL_EXCLUSIVE | DISCL_FOREGROUND);
	bool reinitialize();

	void releaseDevice();

	virtual HRESULT GetDeviceState();

	HRESULT acquire();

	bool SetupDeviceThread(void (*Handler)()=NULL);
	void StopDeviceThread();

	int getNumAxes() const;
	int getNumButtons() const;
	int getNumPOVs() const;

	const long*	 getAxes() const;
	const DWORD* getPOVs() const;
	const BYTE*	 getButtons() const;

protected:

	HRESULT AutoDetectDataFormat();
	static BOOL CALLBACK DIEnumDeviceObjectsCallback(LPCDIDEVICEOBJECTINSTANCE lpddoi,LPVOID pvRef);
	static DWORD WINAPI DeviceThread(CDInputDevice *DeviceObject);

	virtual	 Msg::MSGRVAL handleMessage(Msg::MessagingObject *sender, Msg::MESSAGEID msgId, const Msg::Param &parameters=Msg::Param());

	void (*DeviceHandler)();

	HANDLE				DeviceEvents[2];
	HANDLE				ThreadHandle;

	bool Format_autodetected;
	typedef std::list<DIDEVICEOBJECTINSTANCE> DIDOBJLIST;
	typedef DIDOBJLIST::iterator DIDOBJITERATOR;

	DIDOBJLIST ObjectList;

	LPDINPUTDEVICE		Device;

	LPVOID				DeviceState;
	DWORD				DeviceStateSize;
	DIDATAFORMAT		*DeviceDataFormat;
	GUID				guid;
	DWORD				cooplevel;

	//FIXME: more privacy
	DWORD				num_Axes;
	DWORD				num_Buttons;
	DWORD				num_POV;
	long				*Axes;
	DWORD				*POV;
	BYTE				*Buttons;
};

#endif // !defined(AFX_DINPUTDEVICE_H__31079B00_8D92_11D3_BD8A_0000E85E86C1__INCLUDED_)
