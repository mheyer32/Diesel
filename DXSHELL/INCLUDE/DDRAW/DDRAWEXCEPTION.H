
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
// DDrawException.h: Schnittstelle für die Klasse CDDrawException.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CDDRAWEXCEPTION_H__4F2879B1_6293_4ABC_A963_D44039BBF6FF__INCLUDED_)
#define AFX_CDDRAWEXCEPTION_H__4F2879B1_6293_4ABC_A963_D44039BBF6FF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN
#include <misc/Exception.h>
#include <windows.h>

class CDDrawException : public CException
{
public:
    CDDrawException(const std::string& text, HRESULT ddrval);
    virtual ~CDDrawException();

    HRESULT     getErrorCode() const { return errorcode; };
    const char* getErrorText() const;

protected:
    HRESULT errorcode;
};

#endif  // !defined(AFX_DDRAWEXCEPTION_H__4F2879B1_6293_4ABC_A963_D44039BBF6FF__INCLUDED_)
