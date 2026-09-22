
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
// NamedObject.h: Schnittstelle für die Klasse CNamedObject.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NAMEDOBJECT_H__8ADDEF6A_E915_403B_A660_7D70C2FDFADF__INCLUDED_)
#define AFX_NAMEDOBJECT_H__8ADDEF6A_E915_403B_A660_7D70C2FDFADF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER > 1000

#include <string>

/** CNamedObject offers an interface for objects that need to be named.*/

// FIXME: maintain a list of all instantiated objects and a possibility for ensuring unique
// names ?

class CNamedObject
{
public:
    CNamedObject();
    virtual ~CNamedObject();

    virtual void               setName(const std::string& Name);
    virtual const std::string& getName() const;

protected:
    std::string m_name;
};

#endif  // !defined(AFX_NAMEDOBJECT_H__8ADDEF6A_E915_403B_A660_7D70C2FDFADF__INCLUDED_)
