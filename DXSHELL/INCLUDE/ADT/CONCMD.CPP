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
// ConCmd.cpp: Implementierung der Klasse ConCmd.
//
//////////////////////////////////////////////////////////////////////

#include "ConCmd.h"
#include <defs.h>
#include <iostream>
#include <fstream>

#include <memorytracker.h>
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

ConCmd::COMMANDMAP* ConCmd::CommandList=NULL;

ConCmd ConCmd::listcmd("list",ConCmd::ListCmd);
ConCmd ConCmd::aliascmd("alias",ConCmd::AliasCmd);
ConCmd ConCmd::echocmd("echo",ConCmd::EchoCmd);
ConCmd ConCmd::saycmd("say",ConCmd::EchoCmd);

using namespace std;

ConCmd::ConCmd(const std::string &Command,ConCmd::CALLBACKFUNC CallbackFunc)
{
	setCommand(Command,CallbackFunc);
}

ConCmd::~ConCmd()
{
	if (!CommandList) return;

	COMMANDITERATOR c;

	if ((c=CommandList->find(command))!=CommandList->end())
	{
		 if ((*c).second==this)
		 {
			CommandList->erase(c);
		 }
		 else
		 {
			 std::cout<<"ConCmd::~ConCmd() command found but mapped to different object"<<std::endl;
		 }
	}
	if (CommandList->empty()) KILLOBJECT(CommandList);
}

ConCmd* ConCmd::findCommand(const std::string &Command)
{
COMMANDITERATOR c;

	if ((c=CommandList->find(Command))!=CommandList->end())
	{
		return (*c).second;
	}

	return NULL;
}

void ConCmd::setCommand(const std::string &Command, ConCmd::CALLBACKFUNC CallbackFunc)
{
	if (!CommandList) CommandList=new COMMANDMAP;

	command=Command;
	(*CommandList)[command]=this;
	callback=CallbackFunc;
}

std::string ConCmd::getCommandName() const
{
	return command;
}

void ConCmd::execute(const std::string &Arg)
{
	if (callback) (*callback)(*this,Arg);
}


void ConCmd::execute(const std::string &Command,const std::string &Arg)
{
ConCmd *cobj;

	if (cobj=findCommand(Command)) cobj->execute(Arg);
	else std::cout<<"no such command \""<<Command<<"\""<<endl;
}


void ConCmd::executeCommandLine(const std::string &CommandLine)
{
	// CLI parsen und rekursiv ausfuehren
	char *cmdline,*temp,*temp4,*lineend;
	char command[256],args[256],temp2[256],temp3[1024];

	strcpy(temp3,CommandLine.c_str());
	temp=cmdline=temp3;
	lineend=cmdline+strlen(cmdline);

	do 
	{
		if (temp=strpbrk(cmdline,"\";"))
		{
			if (*temp=='\"')
			{
				*temp=' ';
				if (!(temp=strchr(temp,'\"')))
				{
					std::cout<<"no closing \" found"<<std::endl;
					return;
				}
				*temp=' ';
			}
			temp4=temp;
			if (!(temp=strchr(temp,';'))) temp=temp4;
		}
		else
		{
			temp=cmdline+strlen(cmdline);
		}
		memcpy(temp2,cmdline,temp-cmdline);
		temp2[temp-cmdline]=0;

		int num_args=sscanf(temp2,"%s %[^\n]s",command,args);
		if (num_args<1)
		{
			//cout<<"could not parse commandline: "<<CommandLine<<endl;
			return;
		}
		if (num_args==1)
		{
			ConCmd::execute(command,"");
		}
		else
		{
			ConCmd::execute(command,args);
		}

		cmdline=temp+1;

	} while (cmdline<lineend);
}

void ConCmd::ListCmd(ConCmd &Command,const std::string &Arg)
{
COMMANDITERATOR c;
	for (c=CommandList->begin();c!=CommandList->end();c++)
	{
		cout<<(*c).second->command<<endl;
	}
}

#pragma optimize ("g", off) // it seems that this is causing my problems
void ConCmd::AliasCmd(ConCmd &Command,const std::string &Arg)
{CAliasCmd *alias;
ConCmd *cmd;
char aliasname[256] ;
char cmdline[1024] ;

	if (Arg.empty())
	{
		COMMANDITERATOR c;
		for (c=CommandList->begin();c!=CommandList->end();c++)
		{
			if (alias=dynamic_cast<CAliasCmd*>((*c).second))
			{
				std::cout<<alias->getCommandName()<<": "<<alias->getCommandline()<<std::endl;
			}
		}
		return;
	}

	int num_args=sscanf(Arg.c_str(),"%s %[^\n]s",aliasname,cmdline);
	if (num_args<2)
	{
		std::cout<<"too few arguments"<<std::endl;
		return;
	}
	if (cmd=findCommand(aliasname))
	{
		if (!(alias=dynamic_cast<CAliasCmd*>(cmd)))
		{
			std::cout<<"aliasname is already used by non-alias"<<std::endl;
			return;
		}
		alias->setCommandLine(cmdline);
	}
	else
	{
		alias=new CAliasCmd(aliasname,cmdline);
	}

}
#pragma optimize ("", on)

void ConCmd::EchoCmd(ConCmd &Command,const std::string &Arg)
{
	std::cout<<Arg<<std::endl;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////

CAliasCmd::CAliasCmd(const std::string &Alias,const std::string &Commandline): ConCmd(Alias,CAliasCmd::AliasCallback)
{
	setCommandLine(Commandline);	
}


CAliasCmd::~CAliasCmd()
{

}


void CAliasCmd::setCommandLine(const std::string &Commandline)
{
	commandline=Commandline;
}


std::string CAliasCmd::getCommandline() const
{
	return commandline;
}


void CAliasCmd::AliasCallback(ConCmd &Command,const std::string &Arg)
{
	CAliasCmd *alias;

	if (!(alias=dynamic_cast<CAliasCmd*>(&Command)))
	{
		std::cout<<"AliasCallback on a non-alias command"<<std::endl;
	}

	ConCmd::executeCommandLine(alias->commandline+" "+Arg);
}



const ConCmd::COMMANDMAP& ConCmd::getCommandList()
{
	return *CommandList;
}
