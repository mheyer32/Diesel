
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
// History.cpp: Implementierung der Klasse CHistory.
//
//////////////////////////////////////////////////////////////////////

#include "History.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

template <class T>
CHistory<T>::CHistory()
{
	max_size=1;
}

template <class T>
CHistory<T>::~CHistory()
{

}

template <class T>
void CHistory<T>::add(T elem)
{
	history.push_front(elem);
	if (history.size()>max_size)
	{
		history.pop_back();
	}
}

template <class T>
void CHistory<T>::setMaxHistoryLength(int new_max_size)
{
	max_size=new_max_size;
	if (history.size()>max_size)
	{
		history.resize(max_size); //shrink history, if its already too large
	}
}

template <class T>
int CHistory<T>::getHistoryLength()
{
	return (int)history.size();
}