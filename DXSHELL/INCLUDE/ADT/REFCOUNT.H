
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
// RefCount.h: Schnittstelle für die Klasse CRefCount.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REFCOUNT_H__A5FB04BE_72DA_44B7_83DF_E8A47431A21D__INCLUDED_)
#define AFX_REFCOUNT_H__A5FB04BE_72DA_44B7_83DF_E8A47431A21D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER > 1000

class CRefCount
{
public:
    virtual ~CRefCount();  // GAAAANZ _WiChTiG_, dass das virtual ist!!
    CRefCount();

    inline void Release()
    {
        m_RefCount--;
        if (m_RefCount == 0)
            delete this;
    }

    inline void CRefCount::AddRef()
    {
        m_RefCount++;
        return;
    }

    int getRefCount() const { return m_RefCount; }

private:
    // dont allow copies of reference count
    CRefCount(const CRefCount&);
    CRefCount& operator=(const CRefCount&);

    int m_RefCount;
};

#endif  // !defined(AFX_REFCOUNT_H__A5FB04BE_72DA_44B7_83DF_E8A47431A21D__INCLUDED_)
