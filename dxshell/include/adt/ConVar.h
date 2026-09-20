
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
// ConVar.h: Schnittstelle für die Klasse ConVar.
//
//////////////////////////////////////////////////////////////////////

#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER > 1000

#include "ConCmd.h"

#include <adt/RefCount.h>
#include <adt/SmartPointer.h>

#include <list>
#include <string>

#define CVARFLAG_READONLY (1 << 0)
#define CVARFLAG_ARCHIVE (1 << 1)

class ConVar : public CRefCount, public ConCmd
{

public:
    typedef void (*VARCALLBACKFUNC)(ConVar& CVar);

    ConVar(const std::string& Command, const std::string& InitVal = "", int Flags = 0, VARCALLBACKFUNC cb = NULL);
    virtual ~ConVar();

    void setOnChangeCallback(VARCALLBACKFUNC changecallback);

    int  getFlags() const;
    void setFlags(int flags);

    inline std::string getString() const { return sval; };
    inline float       getFloat() const { return fval; };
    inline int         getInt() const { return ival; };

    inline void setString(const std::string& Value) { *this = Value; };
    inline void setFloat(float f) { *this = f; };
    inline void setInt(int i) { *this = i; };

    ConVar& operator=(const int i);
    ConVar& operator=(const float f);
    ConVar& operator=(const std::string& s);

    inline operator int() const { return ival; };
    inline operator float() const { return fval; };
    inline operator const std::string&() const { return sval; };
    inline operator bool() const { return ival ? true : false; };

    static ConVar* findCVar(const std::string& varname);

protected:
    // this function is called if you either type "varname value"
    // or "set varname value" into the console
    void setFromConsole(const std::string& args);

    // this function is used as callback if the variable name is directly typed in
    static void CVarCallback(ConCmd& Command, const std::string& Arg);

    // this function is the callback for the "set" command
    static void   set(ConCmd& Command, const std::string& Arg);
    static ConCmd cmd_set;
    static ConCmd cmd_seta;

    std::string sval;
    std::string defaultval;
    int         ival;
    float       fval;
    int         flags;

    VARCALLBACKFUNC changecallback;

    static std::list<SmartPointer<ConVar> > allocatedvars;
};
