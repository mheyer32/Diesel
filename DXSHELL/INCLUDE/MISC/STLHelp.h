
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
#ifndef STL_HELP_H
#define STL_HELP_H

#include <algorithm>
#include <map>


namespace STLHelp
{
/*
	struct _deleteObject
	{ 
		template <class T> void operator()(const T* ptr) const 
		{
			delete ptr;
		} 
		template <class _T1, class _T2> void operator()(std::pair<_T1,_T2> &ptr) const 
		{ 
			delete ptr.second; 
		}
	}; 

	template<class T>
	stlwipe (T& container)
	{
		std::for_each(container.begin(), container.end(), _deleteObject());
		container.clear();
	}
*/

	inline void tolower (std::string &s)
	{
		std::transform(s.begin(), s.end(),s.begin(), ::tolower);
	}

	inline void replace (std::string &s, std::string::value_type oldvalue, std::string::value_type newvalue)
	{
		std::replace(s.begin(), s.end(), oldvalue, newvalue);
	}
}
#endif