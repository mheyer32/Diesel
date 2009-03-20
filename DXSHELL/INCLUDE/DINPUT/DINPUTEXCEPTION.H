
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
// DinputException.h: Schnittstelle für die Klasse CDinputException.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DINPUTEXCEPTION_H__9A2BC8D8_64F7_4F92_B8B3_47BD1366B098__INCLUDED_)
#define AFX_DINPUTEXCEPTION_H__9A2BC8D8_64F7_4F92_B8B3_47BD1366B098__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#include <windows.h>

#include "MISC\Exception.h"

#include <iostream>
#include <string>

class CDInputException : public CException  
{
public:
	CDInputException(const std::string &text,HRESULT dirval);
	virtual ~CDInputException();

};

#endif // !defined(AFX_DINPUTEXCEPTION_H__9A2BC8D8_64F7_4F92_B8B3_47BD1366B098__INCLUDED_)
