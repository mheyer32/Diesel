
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
// CVar.cpp: Implementierung der Klasse ConVar.
//
//////////////////////////////////////////////////////////////////////

#include "ConVar.h"
#include <iostream>

#include <MemoryTracker.h>
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

ConCmd ConVar::cmd_set("set", ConVar::set);
ConCmd ConVar::cmd_seta("seta", ConVar::set);

std::list<SmartPointer<ConVar> > ConVar::allocatedvars;

using namespace std;

ConVar::ConVar(const std::string& Command, const std::string& InitVal, int Flags, VARCALLBACKFUNC cb)
: ConCmd(Command, &ConVar::CVarCallback)
{
    flags = Flags;
    setString(InitVal);
    defaultval     = InitVal;
    changecallback = cb;
}

ConVar::~ConVar()
{
}
ConVar& ConVar::operator=(const int i)
{
    ival = i;
    fval = (float)i;
    char st[32];
    _itoa(i, st, 10);
    sval = st;
    return *this;
};
ConVar& ConVar::operator=(const float f)
{
    ival = (int)f;
    fval = f;
    char st[32];
    sprintf(st, "%f", f);
    sval = st;
    return *this;
};
ConVar& ConVar::operator=(const std::string& s)
{
    ival = atoi(s.c_str());
    fval = (float)atof(s.c_str());
    sval = s;
    return *this;
};
void ConVar::setFlags(int newflags)
{
    flags = newflags;
}

int ConVar::getFlags() const
{
    return flags;
}

ConVar* ConVar::findCVar(const std::string& varname)
{
    return dynamic_cast<ConVar*>(ConCmd::findCommand(varname));
}

void ConVar::setOnChangeCallback(VARCALLBACKFUNC newchangecallback)
{
    changecallback = newchangecallback;
}

void ConVar::setFromConsole(const std::string& args)
{
    if (args.empty()) {
        std::cout << command << " is:\"" << sval << "\" default:\"" << defaultval << "\"" << std::endl;
        return;
    }

    if (!(flags & CVARFLAG_READONLY)) {
        setString(args);
        if (changecallback)
            changecallback(*this);
    } else {
        std::cout << command << " is write protected" << std::endl;
    }
}

void ConVar::CVarCallback(ConCmd& cmd, const std::string& Arg)
{
    ConVar* cvar = dynamic_cast<ConVar*>(&cmd);

    if (!cvar) {
        std::cout << "CVarCallback was called for non-cvar command" << std::endl;
        return;
    };
    cvar->setFromConsole(Arg);
}

void ConVar::set(ConCmd& Command, const std::string& Arg)
{

    char var[512];
    char val[512];

    int num_args = sscanf(Arg.c_str(), "%s %[^\n]s", var, val);
    if (num_args < 1) {
        std::cout << "not enough arguments" << std::endl;
        return;
    }

    ConCmd* cmd  = ConCmd::findCommand(var);
    ConVar* cvar = 0;

    if (cmd) {
        // the variable´s name is found
        if (!((cvar = dynamic_cast<ConVar*>(cmd)))) {
            // the found command is not a variable
            std::cout << "'" << var << "' is already defined as non-variable statement." << std::endl;
            return;
        }
        cvar->setFromConsole(val);
    } else {
        // the symbol is not found, a new variable will be created
        cvar = new ConVar(std::string(var), std::string(val));
        allocatedvars.push_back(cvar);  // destruction of the list will also destroy all variables
    }
    if (&Command == &ConVar::cmd_seta) {
        cvar->setFlags(cvar->getFlags() | CVARFLAG_ARCHIVE);
    }
}
