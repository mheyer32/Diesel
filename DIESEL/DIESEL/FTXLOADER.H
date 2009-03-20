/*
This file is part of Diesel
(c) 2002 by Mathias Heyer
email: sonode@gmx.de

Diesel is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Diesel is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
// FTXLoader.h: Schnittstelle für die Klasse FTXLoader.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FTXLOADER_H__B01A7C3C_FFB9_4285_BDA2_410808C1C433__INCLUDED_)
#define AFX_FTXLOADER_H__B01A7C3C_FFB9_4285_BDA2_410808C1C433__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "imageloader.h"

class FTXLoader : public ImageLoader
{
public:
	FTXLoader();
	virtual ~FTXLoader();
	
	//
	virtual Image* load(CFile &file);

};

#endif // !defined(AFX_FTXLOADER_H__B01A7C3C_FFB9_4285_BDA2_410808C1C433__INCLUDED_)
