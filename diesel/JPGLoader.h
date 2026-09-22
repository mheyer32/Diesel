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
// JPGLoader.h: Schnittstelle für die Klasse JPGLoader.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_JPGLOADER_H__10BF8C70_0E1D_4888_98A9_8D9E3E786A8B__INCLUDED_)
#define AFX_JPGLOADER_H__10BF8C70_0E1D_4888_98A9_8D9E3E786A8B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER > 1000

#include "ImageLoader.h"

class JPGLoader : public ImageLoader
{
public:
    JPGLoader();
    virtual ~JPGLoader();

    virtual Image* load(CFile& file);
};

#endif  // !defined(AFX_JPGLOADER_H__10BF8C70_0E1D_4888_98A9_8D9E3E786A8B__INCLUDED_)
