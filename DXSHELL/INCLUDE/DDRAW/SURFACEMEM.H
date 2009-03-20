
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
// SurfaceMem.h: Schnittstelle für die Klasse SurfaceMem.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CSURFACEMEM_H__312D5121_AE70_11D3_BD8A_0000E85E86C1__INCLUDED_)
#define AFX_CSURFACEMEM_H__312D5121_AE70_11D3_BD8A_0000E85E86C1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "surface.h"

template <class _P>
class CSurfaceMem 
{
public:
	CSurfaceMem(const CSurface &Surface):surface(&Surface){};

	/** set a point, check for boundaries */
	inline void setPixel(const unsigned int x, const unsigned int y, const _P &color) const
	{
		if ((x>=surface->getWidth())||(y>=surface->getHeight())) return; //FIXME: throw exception ?
		((_P*)surface->getPointer())[y*surface->getPitch()+x]=color;
	}
	
	/** get a Pixel, check for boundaries */
	inline _P getPixel(const unsigned int x, const unsigned int y) const
	{
		if ((x>=surface->getWidth())||(y>=surface->getHeight())) return 0;
		return ((_P*)surface->getPointer())[y*surface->getPitch()+x];
	}

	/** set a point in a quadratic power-of-two surface,
		coordinates greater than the boundaries wrap around */
	inline void setPixel2(const unsigned int x,const unsigned int y, const _P &color) const
	{
		((_P*)surface->getPointer())[((y&surface->getYMask())<<surface->getShift())+(x&surface->getXMask())]=color;
	}

	/** get a Pixel, for power-of-two surfaces only */
	inline _P getPixel2(const unsigned int x, const unsigned int y) const
	{
		return ((_P*)surface->getPointer())[((y&surface->getYMask())<<surface->getShift())+(x&surface->getXMask())];
	}

	/** set a point, don´t check for boundaries */
	inline void setPixel3(const unsigned int x, const unsigned int y, const _P &color) const
	{
		((_P*)surface->getPointer())[y*surface->getPitch()+x]=color;
	}

	/** get a Pixel, but don´t check for boundaries */
	inline _P getPixel3(const unsigned int x, const unsigned int y) const
	{
		return ((_P*)surface->getPointer())[y*surface->getPitch()+x];
	}
protected:
	const CSurface	*surface;
};

#endif // !defined(AFX_CSURFACEMEM_H__312D5121_AE70_11D3_BD8A_0000E85E86C1__INCLUDED_)
