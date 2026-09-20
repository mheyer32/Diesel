
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
// Path.cpp: Implementierung der Klasse CPath.
//
//////////////////////////////////////////////////////////////////////

#include <algorithm>
#include <cassert>
#include "Path.h"

#include <misc/Exception.h>

#include <MemoryTracker.h>

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
#define MAX_PATH 1024

CPath::CPath()
{
    m_has_extension = false;
    m_type          = PT_RELATIVE;
}

CPath::~CPath()
{
}

CPath::CPath(const std::string& newpath)
{
    m_pathname = newpath;
    preparePath();
}

CPath::CPath(const char* newpath)
{
    m_pathname = newpath;
    preparePath();
}

// purify m_pathname to a valid string
// turn all capitals into lower letters,
// remove whitespaces in fron and behind the path (currently disabled),
// replace all occurrences of "/" with "\"
// remove leading and trailing delimiters,
// determine m_type of path
void CPath::preparePath()
{
    m_has_extension  = false;
    m_type           = PT_RELATIVE;
    m_startextension = std::string::npos;

    if (m_pathname.empty()) {
        return;
    }

    //	std::cout<<"CPath::preparePath() from: '"<<m_pathname<<"' to: '";

    // remove spaces in front of and at the end of the path
    std::string::size_type start = m_pathname.find_first_not_of(" \n\0x0A\t");
    std::string::size_type end   = m_pathname.find_last_not_of(" \n\0x0A\t");

    if (start == std::string::npos || end == std::string::npos) {
        m_pathname.clear();
        return;
    }

    m_pathname.assign(m_pathname, start, end - start + 1);

    // replace "/" with "\"
    std::replace(m_pathname.begin(), m_pathname.end(), '/', '\\');

    if (m_pathname.size() > 1) {
        // determine m_type of given path/filename
        if (m_pathname[1] == ':') {
            m_type = PT_ABSOLUTE;
        } else if (m_pathname[0] == '\\' && m_pathname[1] == '\\') {
            m_type = PT_NETWORK;
        } else {
            m_type = PT_RELATIVE;
        }

        // remove leading delimiter
        if (m_type == PT_RELATIVE && m_pathname[0] == '\\') {
            m_pathname.erase(m_pathname.begin());
        }

        // remove trailing delimiter
        while (*(m_pathname.end() - 1) == '\\') {
            m_pathname.erase(m_pathname.end() - 1);
        }

        // look for a valid extension

        std::string::size_type pos = m_pathname.find_last_of("\\.");
        if (pos != std::string::npos && m_pathname[pos] == '.') {
            m_has_extension  = true;
            m_startextension = pos + 1;
        }

        // finally lower all letters
        std::transform(m_pathname.begin(), m_pathname.end(), m_pathname.begin(), ::tolower);
    }

    //	std::cout<<m_pathname<<'\''<<std::endl;
}

// FIXME maybe returning a CPath would be better
std::string CPath::getPath() const
{
    if (!m_has_extension)
        return m_pathname;

    std::string::size_type pos = m_pathname.find_last_of('\\');
    if (pos == std::string::npos) {
        return "";
    }
    return m_pathname.substr(0, pos);
}

std::string CPath::getFilename() const
{
    std::string::size_type pos = m_pathname.find_last_of('\\');
    if (pos == std::string::npos) {
        return m_pathname;
    }
    return m_pathname.substr(pos + 1);
}

std::string CPath::getExtension() const
{
    if (!m_has_extension)
        return "";

    return m_pathname.substr(m_startextension);
}

void CPath::setExtension(const std::string& newext)
{
    if (!m_has_extension) {
        m_pathname.push_back('.');
        m_pathname.append(newext);
    } else {
        m_pathname.resize(m_startextension + newext.size());
        m_pathname.replace(m_startextension, newext.size(), newext);
    }
    preparePath();  // sanity check
}

void CPath::stripExtension()
{
    if (!m_has_extension)
        return;

    m_pathname.resize(m_startextension - 1);
    preparePath();  // sanity check
}

CPath& CPath::operator=(const std::string& newpathname)
{
    m_pathname = newpathname;
    preparePath();
    return *this;
}
CPath& CPath::operator=(const char* newpathname)
{
    m_pathname = newpathname;
    preparePath();
    return *this;
}
CPath& CPath::operator/=(const CPath& path2)
{
    if (m_pathname.empty()) {
        *this = path2;
        return *this;
    }
    if (path2.m_pathname.empty())
        return *this;
    if (m_has_extension || path2.m_type != PT_RELATIVE) {
        throw CException(std::string("CPath::operator +() impossible to combine: '") + m_pathname +
                         std::string("' and: '") + path2.m_pathname + std::string("'"));
        // std::cout<<"CPath::operator +() impossible to combine: '"<<m_pathname<<"' and:
        // '"<<path2.m_pathname<<"'"<<std::endl;
    }

    if (path2.m_has_extension) {
        m_startextension = m_pathname.size() + path2.m_startextension + 1;
    }
    m_has_extension = path2.m_has_extension;

    m_pathname.push_back('\\');
    m_pathname.append(path2.m_pathname);

    return *this;
}

CPath CPath::operator/(const CPath& path2) const
{
    CPath tpath(*this);
    tpath /= path2;
    return tpath;
}

bool CPath::operator==(const CPath& path2) const
{
    return m_pathname == path2.m_pathname;
}
bool CPath::operator!=(const CPath& path2) const
{
    return m_pathname != path2.m_pathname;
}
bool CPath::operator<(const CPath& path2) const
{
    return m_pathname < path2.m_pathname;
}

std::ostream& operator<<(std::ostream& stream, const CPath& path)
{
    return stream << path.getString();
};

std::string operator+(const std::string& str1, const CPath& path)
{
    return str1 + path.getString();
}
