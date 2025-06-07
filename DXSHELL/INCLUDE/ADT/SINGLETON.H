
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
// Singleton.h: Schnittstelle für die Klasse Singleton.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SINGLETON_H__C7A159EF_174E_416E_9AA9_85D53EABC439__INCLUDED_)
#define AFX_SINGLETON_H__C7A159EF_174E_416E_9AA9_85D53EABC439__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER > 1000

#include <memory>

/* Singleton is thought to serve as baseclass for Singletons
   Otherwise Singleton<T>::Instance is not by defualt allowed to call the default-constructor of the
   derived class which usually especially protects it to serve the idea of the singleton-pattern.
   Thats why derived classes need to do the friend-declaration for Singleton::Instance()
*/

#define DECLARE_SINGLETON(classname)                                                                                   \
    friend classname* Singleton<classname>::Instance();                                                                \
    friend std::auto_ptr<classname>;

template <class T>
class Singleton
{
public:
    /** if no Instance exists yet, one is created and its pointer is returned */
    static T* Instance();

    /** return the current Instance
    if no Instance exists, NULL is returned */
    static T* hasInstance();

    /** we _need_ means to destroy the instance, otherwise the destruction of the singletons would be
        in no particular order, which may be catastrophic if some singletons depend on each other */
    static void destroyInstance();

protected:
    Singleton();
    Singleton(const Singleton&);
    Singleton& operator=(const Singleton&);
    virtual ~Singleton();

private:
    static std::auto_ptr<T> s_instance;
};

#include "Singleton.hh"

#endif  // !defined(AFX_SINGLETON_H__C7A159EF_174E_416E_9AA9_85D53EABC439__INCLUDED_)
