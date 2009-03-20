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
// Singleton.cpp: Implementierung der Klasse Singleton.
//
//////////////////////////////////////////////////////////////////////

#include <misc/exception.h>
#include <typeinfo.h>

#include <memorytracker.h>
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
template<typename _T>
std::auto_ptr<_T> Singleton<_T>::s_instance;


template<class T>
Singleton<T>::Singleton()
{
	if (s_instance.get()!=NULL)
	{
		throw CException("Singleton<T>::Singleton(): created more than one instance of class\n"+std::string(typeid(T).name()));
	}
}

template<class T>
Singleton<T>::~Singleton()
{

}

template<class T>
T* Singleton<T>::Instance()
{
	if (!s_instance.get()) s_instance.reset(new T);
	return s_instance.get();
}

template<class T>
T* Singleton<T>::hasInstance()
{
	return s_instance.get();
}
template<class T>
void Singleton<T>::destroyInstance()
{
	s_instance.reset(NULL);
}
