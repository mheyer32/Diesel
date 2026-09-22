
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
// Path.h: Schnittstelle für die Klasse CPath.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PATH_H__E3EF821F_0F3C_44B9_83F4_A7FCD9EC127A__INCLUDED_)
#define AFX_PATH_H__E3EF821F_0F3C_44B9_83F4_A7FCD9EC127A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER > 1000

#include <list>
#include <ostream>
#include <string>

class CPath
{
public:
    enum PATHTYPE
    {
        PT_ABSOLUTE,
        PT_RELATIVE,
        PT_NETWORK
    };

    CPath();
    CPath(const std::string& newpath);
    CPath(const char* newpath);
    virtual ~CPath();

    PATHTYPE getType() const { return m_type; };

    std::string getPath() const;
    std::string getFilename() const;
    std::string getExtension() const;
    void        setExtension(const std::string& newext);
    bool        hasExtension() const { return m_has_extension; };
    void        stripExtension();

    const std::string& getString() const { return m_pathname; };
    operator const std::string&() const { return m_pathname; };

    CPath& operator=(const std::string& newpath);
    CPath& operator=(const char* newpath);

    CPath  operator/(const CPath& path2) const;
    CPath& operator/=(const CPath& path2);

    // lexicographical order on complete pathstring
    bool operator==(const CPath& path2) const;
    bool operator!=(const CPath& path2) const;
    bool operator<(const CPath& path2) const;

protected:
    void setType();
    void preparePath();

    std::string            m_pathname;
    PATHTYPE               m_type;
    std::string::size_type m_startextension;
    bool                   m_has_extension;
};

typedef std::list<CPath>   PATHLIST;
typedef PATHLIST::iterator PATHITERATOR;

extern std::ostream& operator<<(std::ostream& stream, const CPath& path);
extern std::string   operator+(const std::string& str1, const CPath& path);

#endif  // !defined(AFX_PATH_H__E3EF821F_0F3C_44B9_83F4_A7FCD9EC127A__INCLUDED_)
