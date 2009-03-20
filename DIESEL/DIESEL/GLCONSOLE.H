/*
This file is part of Diesel
(c) 2002 by Mathias Heyer
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
// GLConsole.h: Schnittstelle für die Klasse CGLConsole.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GLCONSOLE_H__01706115_AC44_4E3B_BE76_13F8F97C79B9__INCLUDED_)
#define AFX_GLCONSOLE_H__01706115_AC44_4E3B_BE76_13F8F97C79B9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#pragma warning ( disable : 4786 )

#include <string>
#include <map>
#include <deque>

#include "GLText.h"
#include "ConKeyMap.h"
#include <misc/Timer.h>
#include <misc/History.h>
#include "LineEdit.h"
#include <adt/ConCmd.h>
#include <adt/ConVar.h>
#include <adt/Singleton.h>
#include <misc/history.h>

#define MAX_CMDLINE_LENGTH	1024
#define CMD_HISTORY_LENGTH	30
#define HISTORY_LENGTH		1000

class CGLConsole : public Singleton<CGLConsole>
{
	
DECLARE_SINGLETON(CGLConsole)

public:

enum CON_SHOW
{
	SHOW_NOT=0,
	SHOW_FULL,
	SHOW_HALF,

};

	bool initialize(CHistory<std::string> *history);

	CON_SHOW GetShow();
	CON_SHOW ToggleConsole();

	void Show(CON_SHOW showconsole);
	void Render();
	void setConSize(int Columns,int Rows);

	void execCommandBuffer();

protected:

	virtual ~CGLConsole();
	CGLConsole();

	void handleKey(int key);
	void setConsoleDestrect();

	/** handles the editable commandline */
	CLineEdit cmdline;

	/** the last 30 commandlines are saved here */
	CHistory<std::string> cmd_history;
	int cmd_history_pos;

	/** pointer to a history object that receives all console printouts
		usually this is the history object of the streambuf_history object
		that will be bound to cout at the beginning of the program */
	CHistory<std::string> *history;
	int	history_pos;

	/** how many textlines  and columns are visible */
	int cols,rows;

	/** current state of the console */
	CON_SHOW show;
	/** future state */
	CON_SHOW newshow;

	/** textbuffer that receives the printouts as seen on the screen*/
	CGLText gltext;

	/** if the console is up this textbuffer should print all console printouts
		on top of the screen*/
	CGLText	gltext2;

	CTimer timer;		// should time blinking cursor
	CTimer animtimer;	// time scrolling of console

	//FIXME: rethink this
	static std::string commandbuffer;

	ConCmd	cmd_toggleconsole;
	ConVar	scr_conspeed;

	static void KeymapCB(int key,void *userdata);
	friend static void KeymapCB(int key,void *userdata);

	static void toggleconsole(ConCmd &command,const std::string &args);
};

#endif // !defined(AFX_GLCONSOLE_H__01706115_AC44_4E3B_BE76_13F8F97C79B9__INCLUDED_)
