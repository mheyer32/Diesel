
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
// Overlay16.h: Schnittstelle für die Klasse COverlay.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OVERLAY_H__29627520_8ECB_11D3_AF5D_0080AD17B2BB__INCLUDED_)
#define AFX_OVERLAY_H__29627520_8ECB_11D3_AF5D_0080AD17B2BB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <ddraw/Surface.h>

class CAppWindow;

class COverlay : public CSurface
{

public:

	COverlay();
	COverlay(DWORD w,DWORD h,DWORD bc=0,DWORD caps=0,LPDDPIXELFORMAT pf=NULL);
	virtual ~COverlay();

	virtual HRESULT create(DWORD w,DWORD h,DWORD bc=0,DWORD caps=0,LPDDPIXELFORMAT pf=NULL);

	BOOL OverlaySupport();
	HRESULT GetOverlayCaps();
	HRESULT UpdateOverlay(const RECT *destRect=NULL);
	HRESULT Show();
	HRESULT Hide();
	HRESULT Screensize();

	HRESULT SourceAlpha(DWORD AlphaValue);
	HRESULT DestAlpha(DWORD AlphaValue);
	HRESULT SourceColorKey(DWORD ColorKey);
	HRESULT DestColorKey(DWORD ColorKey);
	void AttachToWindow(BOOL KA=FALSE);

protected:
	void CreateOverlayFX();

	virtual	 Msg::MSGRVAL handleMessage(Msg::MessagingObject *sender, Msg::MESSAGEID msgId, const Msg::Param &parameters=Msg::Param());

	RECT	srcRect,destRect;
	DWORD	minStretch,maxStretch;
	DWORD	OverlayFlags;
	LPDDOVERLAYFX OverlayFX;
	BOOL	KeepAspect;
	BOOL	ShowOverlay;

	CAppWindow	*appwindow;
};

#endif // !defined(AFX_OVERLAY_H__29627520_8ECB_11D3_AF5D_0080AD17B2BB__INCLUDED_)
