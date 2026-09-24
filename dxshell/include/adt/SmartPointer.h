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
// SmartPointer.h: Schnittstelle für die Klasse SmartPointer.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SMARTPOINTER_H__D4DEE4EB_F166_45BB_8B6B_AD217A4D7624__INCLUDED_)
#define AFX_SMARTPOINTER_H__D4DEE4EB_F166_45BB_8B6B_AD217A4D7624__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER > 1000

template <class _T>
class SmartPointer
{

public:
    SmartPointer(_T* pointer = 0)
    : ptr(pointer)
    {
        if (ptr)
            ptr->AddRef();
    }
    SmartPointer(const SmartPointer<_T>& sptr2)
    : ptr(0)
    {
        *this = sptr2;
    }
    ~SmartPointer()
    {
        if (ptr)
            ptr->Release();
    }

    inline SmartPointer<_T>& operator=(const SmartPointer<_T>& sptr)
    {
        *this = sptr.ptr;
        return *this;
    }

    inline SmartPointer<_T>& operator=(_T* newPtr)
    {
        if (newPtr)
            newPtr->AddRef();
        if (ptr)
            ptr->Release();
        ptr = newPtr;
        return *this;
    }

    inline _T* operator->() const { return ptr; }

    inline _T& operator*() const { return *ptr; }

    inline operator _T*() const { return ptr; }

    inline _T* getPointer() const { return ptr; };
    /*	inline bool operator == (const SmartPointer<_T> &sptr2)
        {
            return ptr==sptr2.ptr;
        }
    */

protected:
    _T* ptr;
};

#endif  // !defined(AFX_SMARTPOINTER_H__D4DEE4EB_F166_45BB_8B6B_AD217A4D7624__INCLUDED_)
