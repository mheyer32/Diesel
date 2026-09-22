
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
// WinException.h: Schnittstelle für die Klasse CWinException.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WINEXCEPTION_H__D6C8A5CD_9162_42DE_92F4_A4849CC4FE38__INCLUDED_)
#define AFX_WINEXCEPTION_H__D6C8A5CD_9162_42DE_92F4_A4849CC4FE38__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER > 1000

#include <misc\Exception.h>

class CWinException : public CException
{
public:
    CWinException();
    CWinException(std::string errortext);
    virtual ~CWinException();
};

#endif  // !defined(AFX_WINEXCEPTION_H__D6C8A5CD_9162_42DE_92F4_A4849CC4FE38__INCLUDED_)
