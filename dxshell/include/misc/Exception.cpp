
/*
This file is part of DXShell
(c) 2002-2026 by Mathias Heyer

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
// Exception.cpp: Implementierung der Klasse CException.
//
//////////////////////////////////////////////////////////////////////
#include "Exception.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

using namespace std;

CException::CException()
{
}

CException::~CException()
{
}

CException::CException(const std::string& error_text)
{
    addText(error_text);
}
CException::CException(const std::string& error_text, const std::string& error_text2)
{
    addText(error_text + error_text2);
}

void CException::addText(const std::string& error_text)
{
    errorlist.push_front(error_text);
}
CException& CException::operator<<(const std::string& error_text)
{
    CException::addText(error_text);
    return *this;
}

std::string CException::getCompleteText()
{
    std::string text;

    ERRLISTITERATOR eIt = errorlist.begin();

    for (; eIt != errorlist.end(); ++eIt) {
        text += *eIt + "->";
    }

    return text;
}
