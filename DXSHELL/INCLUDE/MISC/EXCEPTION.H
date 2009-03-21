
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
// Exception.h: Schnittstelle für die Klasse CException.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EXCEPTION_H__B1C8F0B0_3782_4C4A_87BB_247BF3C230FC__INCLUDED_)
#define AFX_EXCEPTION_H__B1C8F0B0_3782_4C4A_87BB_247BF3C230FC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <list>
#include <string>
#include <stdexcept>

class CException  : public std::exception
{
public:
	CException();
	CException(const std::string &error_text);
	CException(const std::string &error_text,const std::string &error_text2);
	virtual ~CException();

	void addText(const std::string &error_text);
	CException& operator << (const std::string &error_text);

	std::string getCompleteText();

protected:

	typedef std::list<std::string> ERRLIST;
	typedef ERRLIST::iterator ERRLISTITERATOR;

	ERRLIST errorlist;
};

#endif // !defined(AFX_EXCEPTION_H__B1C8F0B0_3782_4C4A_87BB_247BF3C230FC__INCLUDED_)
