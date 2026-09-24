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
// ConCmd.h: Schnittstelle für die Klasse ConCmd.
//
//////////////////////////////////////////////////////////////////////

#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER > 1000

#include <map>
#include <string>

class ConCmd
{
    // FIXME: vielleicht eine statische Funktion einbauen, die ConCmd Objekte erzeugt und
    //  verhindert, dass mehrere Commandos gleichen Namens erzeugt werden
    //  + Konstruktor verstecken
public:
    struct myless
    {
        bool operator()(const std::string& s1, const std::string& s2) const;
    };

    typedef std::map<std::string, ConCmd*, myless> COMMANDMAP;
    typedef COMMANDMAP::iterator                   COMMANDITERATOR;

    typedef void (*CMDCALLBACKFUNC)(ConCmd& Command, const std::string& Arg);

    ConCmd(const std::string& Command = "", CMDCALLBACKFUNC CallbackFunc = NULL);
    virtual ~ConCmd();

    void        setCommand(const std::string& Command, CMDCALLBACKFUNC CallbackFunc);
    void        execute(const std::string& Arg = "");
    std::string getCommandName() const;

    static ConCmd* findCommand(const std::string& Command);
    static void    execute(const std::string& Command, const std::string& Arg);
    static void    executeCommandLine(const std::string& CommandLine);

    static const COMMANDMAP& getCommandList();

protected:
    std::string     command;
    CMDCALLBACKFUNC callback;

    static COMMANDMAP* CommandList;

    static ConCmd listcmd;
    static ConCmd aliascmd;
    static ConCmd echocmd;
    static ConCmd saycmd;

    static void ListCmd(ConCmd& Command, const std::string& Arg);
    static void AliasCmd(ConCmd& Command, const std::string& Arg);
    static void ExecCmd(ConCmd& Command, const std::string& Arg);
    static void EchoCmd(ConCmd& Command, const std::string& Arg);

private:
    ConCmd(const ConCmd&);
    ConCmd& operator=(const ConCmd&);
};

class CAliasCmd : public ConCmd
{

public:
    CAliasCmd(const std::string& Alias, const std::string& Commandline);
    virtual ~CAliasCmd();

    void        setCommandLine(const std::string& Commandline);
    std::string getCommandline() const;

protected:
    std::string commandline;

    static void AliasCallback(ConCmd& Command, const std::string& Arg);
};
