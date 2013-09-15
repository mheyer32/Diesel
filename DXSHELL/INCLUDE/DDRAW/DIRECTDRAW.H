
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
// DirectDraw.h: Schnittstelle für die Klasse CDirectDraw.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIRECTDRAW_H__C0693D80_4837_11D3_9743_0000E85E86C1__INCLUDED_)
#define AFX_DIRECTDRAW_H__C0693D80_4837_11D3_9743_0000E85E86C1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <win/appwindow.h>
#include <messaging/messagingobject.h>
#include <adt/singleton.h>
#include <vector>

#include <InitGuid.h>

#ifndef DIRECTDRAW_VERSION
#define DIRECTDRAW_VERSION 0x700
#endif

#include <ddraw.h>
#include <ddraw/dderr.h>

#pragma comment( lib, "d2d1.lib" )

typedef IDirectDraw7	DDRAW;
typedef LPDIRECTDRAW7	LPDDRAW;
#define DDINTERFACE		IID_IDirectDraw7

class CDirectDraw : public Msg::MessagingObject , public Singleton<CDirectDraw>
{

DECLARE_SINGLETON(CDirectDraw)

public:


	bool InitDirectDraw(GUID *lpGuid=NULL);

	bool InitDisplay(DWORD Width,DWORD Height,DWORD Depth,bool Fullscreen);
	bool InitDisplay();

	void ShutDownDirectDraw();

	inline LPDDRAW getLPDD() const {return lpDDx;};
	HRESULT getCaps(DDCAPS &caps) const;
	
	bool runsFullscreen() const {return run_fullscreen;};

	BOOL chooseDevice(GUID &Guid);

protected:

typedef std::vector<DDSURFACEDESC2> MODELIST;
typedef MODELIST::iterator		  MODEITERATOR;

	CDirectDraw();
	~CDirectDraw();

	virtual	 Msg::MSGRVAL handleMessage(Msg::MessagingObject *sender, Msg::MESSAGEID msgId, const Msg::Param &parameters=Msg::Param());


	bool	CheckCoopLevel();
	void	InitMisc();

	HRESULT enumModes(MODELIST &modelist, LPDDSURFACEDESC2 lpRefddsd=NULL);

	static BOOL WINAPI EnumDevicesCallback(GUID * lpGuid,LPSTR  lpDescription,LPSTR  lpName,LPVOID lpContext);
	static HRESULT WINAPI EnumModesCallback2(LPDDSURFACEDESC2 lpDDSurfaceDesc,LPVOID lpContext);


	LPDDRAW					lpDDx;
	CAppWindow				*appwindow;
	bool					run_fullscreen;

};

#endif // !defined(AFX_DIRECTDRAW_H__C0693D80_4837_11D3_9743_0000E85E86C1__INCLUDED_)
