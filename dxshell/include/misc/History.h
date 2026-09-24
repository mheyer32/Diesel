
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
// History.h: Schnittstelle für die Klasse CHistory.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HISTORY_H__324526A0_0B01_4674_B889_D14411D9C06A__INCLUDED_)
#define AFX_HISTORY_H__324526A0_0B01_4674_B889_D14411D9C06A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER > 1000

#include <deque>

/** maintaines a history of objects of type T */
template <typename T>
class CHistory
{
public:
    CHistory();
    virtual ~CHistory();

    void add(T elem);

    void setMaxHistoryLength(int new_max_size);
    int  getHistoryLength();

    inline const T& operator[](const int history_pos) const { return history[history_pos]; };

protected:
    std::deque<T> history;

    typename std::deque<T>::size_type max_size;
};

#include "history.hh"
#endif  // !defined(AFX_HISTORY_H__324526A0_0B01_4674_B889_D14411D9C06A__INCLUDED_)
