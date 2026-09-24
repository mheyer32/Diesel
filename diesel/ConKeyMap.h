/*
This file is part of Diesel
(c) 2002-2026 by Mathias Heyer
email: sonode@gmx.de

Diesel is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Diesel is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
// ConKeyMap.h: Schnittstelle für die Klasse CConKeyMap.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONKEYMAP_H__61D4CFE0_3E45_11D5_99C9_0000E85E86C1__INCLUDED_)
#define AFX_CONKEYMAP_H__61D4CFE0_3E45_11D5_99C9_0000E85E86C1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER > 1000

#include <adt/Singleton.h>
#include "KeyMap.h"

class ConCmd;

class CConKeyMap : public CKeyMap, public Singleton<CConKeyMap>
{

    DECLARE_SINGLETON(CConKeyMap)

public:
    void UnbindAllUserKey();
    void UnbindUserKey(int key);
    void BindUserKey(int key, char* commandline);

    static void PrintBindings(int key);
    static int  StringToKey(const std::string& Keyname);

protected:
    virtual ~CConKeyMap();
    CConKeyMap();

    ConCmd* cmd_bind;
    ConCmd* cmd_unbind;
    ConCmd* cmd_unbindall;

    std::map<int, std::string> dx2ascii;

    static void bind(ConCmd& Command, const std::string& Arg);  // callback for "bind" and "unbind" console commands
    static void ExecBinding(int key, void* userdata);           // called if a console-bound key was pressed

    friend static void bind(ConCmd& Command, const std::string& Arg);
    friend static void PrintBindings(int key);
};

#endif  // !defined(AFX_CONKEYMAP_H__61D4CFE0_3E45_11D5_99C9_0000E85E86C1__INCLUDED_)
