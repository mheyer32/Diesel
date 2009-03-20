
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
// Keyboard.cpp: Implementierung der Klasse CKeyboard.
//
//////////////////////////////////////////////////////////////////////

#include "Keyboard.h"
#include <misc/exception.h>
#include <iostream>

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CKeyboard::CKeyboard()
{
	initTranslationMaps();
}

CKeyboard::~CKeyboard()
{
}

bool CKeyboard::initialize(DWORD cooplevel)
{
	std::cout<<"======== CKeyboard::initialize() ========"<<std::endl;

	ZeroMemory(m_keybuffer,sizeof(m_keybuffer));
	if(InitDevice(GUID_SysKeyboard,(_DIDATAFORMAT *)&c_dfDIKeyboard,cooplevel)!=TRUE)
	{
		std::cout<<"CKeyboard::initialize() failed"<<std::endl;
		return false;
	}

	DeviceState=&m_keybuffer;
	DeviceStateSize=sizeof(m_keybuffer);
	
	Buttons=m_keybuffer;
	Axes=NULL;
	POV=NULL;

	std::cout<<"CKeyboard::initialize() successful"<<std::endl;

	return true;
}

bool CKeyboard::getKeyforASCII(const std::string &keyname,DWORD &keycode)
{
	std::map<std::string,int>::iterator found=ascii2dx.find(keyname);
	//std::cout<<"\""<<keyname<<"\""<<std::endl;
	if (found!=ascii2dx.end())
	{
		keycode=(*found).second;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

char CKeyboard::getASCIIforKey(DWORD scancode)
{
	HKL layout=GetKeyboardLayout(0);
	BYTE state[256];
	GetKeyboardState(state);
	UINT vk=MapVirtualKeyEx(scancode,1,layout);
	WORD result[4]; // 4 should be enough
	if (ToAsciiEx(vk,scancode,state,result,0,layout)) return (char)result[0];
	return 0;
}

void	CKeyboard::initTranslationMaps()
{
	HKL layout=GetKeyboardLayout(0);
	char  keyname[256];	

	for (int k=0;k<256;k++)
	{
		GetKeyNameText(k<<16,keyname,sizeof(keyname));
		dx2ascii[k]=keyname;

	//	std::cout<<keyname<<std::endl;
	}
 
	ascii2dx["a"]=DIK_A;
	ascii2dx["b"]=DIK_B;
	ascii2dx["c"]=DIK_C;
	ascii2dx["d"]=DIK_D;
	ascii2dx["e"]=DIK_E;
	ascii2dx["f"]=DIK_F;
	ascii2dx["g"]=DIK_G;
	ascii2dx["h"]=DIK_H;
	ascii2dx["i"]=DIK_I;
	ascii2dx["j"]=DIK_J;
	ascii2dx["k"]=DIK_K;
	ascii2dx["l"]=DIK_L;
	ascii2dx["m"]=DIK_M;
	ascii2dx["n"]=DIK_N;
	ascii2dx["o"]=DIK_O;
	ascii2dx["p"]=DIK_P;
	ascii2dx["q"]=DIK_Q;
	ascii2dx["r"]=DIK_R;
	ascii2dx["s"]=DIK_S;
	ascii2dx["t"]=DIK_T;
	ascii2dx["u"]=DIK_U;
	ascii2dx["v"]=DIK_V;
	ascii2dx["w"]=DIK_W;
	ascii2dx["x"]=DIK_X;
	ascii2dx["y"]=DIK_Y;
	ascii2dx["z"]=DIK_Z;

	ascii2dx["A"]=DIK_A;
	ascii2dx["B"]=DIK_B;
	ascii2dx["C"]=DIK_C;
	ascii2dx["D"]=DIK_D;
	ascii2dx["E"]=DIK_E;
	ascii2dx["F"]=DIK_F;
	ascii2dx["G"]=DIK_G;
	ascii2dx["H"]=DIK_H;
	ascii2dx["I"]=DIK_I;
	ascii2dx["J"]=DIK_J;
	ascii2dx["K"]=DIK_K;
	ascii2dx["L"]=DIK_L;
	ascii2dx["M"]=DIK_M;
	ascii2dx["N"]=DIK_N;
	ascii2dx["O"]=DIK_O;
	ascii2dx["P"]=DIK_P;
	ascii2dx["Q"]=DIK_Q;
	ascii2dx["R"]=DIK_R;
	ascii2dx["S"]=DIK_S;
	ascii2dx["T"]=DIK_T;
	ascii2dx["U"]=DIK_U;
	ascii2dx["V"]=DIK_V;
	ascii2dx["W"]=DIK_W;
	ascii2dx["X"]=DIK_X;
	ascii2dx["Y"]=DIK_Y;
	ascii2dx["Z"]=DIK_Z;

	ascii2dx["\n"]=DIK_RETURN;
	ascii2dx["ENTER"]=DIK_RETURN;
	ascii2dx[" "]=DIK_SPACE;
	ascii2dx["SPACE"]=DIK_SPACE;
	ascii2dx[";"]=DIK_SEMICOLON;
	ascii2dx["SEMICOLON"]=DIK_SEMICOLON;
	ascii2dx[","]=DIK_COMMA;
	ascii2dx["COMMA"]=DIK_COMMA;
	ascii2dx["."]=DIK_PERIOD;
	ascii2dx["PERIOD"]=DIK_PERIOD;
	ascii2dx["/"]=DIK_SLASH;
	ascii2dx["SLASH"]=DIK_SLASH;
	ascii2dx["\\"]=DIK_BACKSLASH;
	ascii2dx["BACKSLASH"]=DIK_BACKSLASH;
	ascii2dx["_"]=DIK_UNDERLINE;
	
	ascii2dx["*"]=DIK_MULTIPLY;
	ascii2dx["MULTIPLY"]=DIK_MULTIPLY;
	ascii2dx["+"]=DIK_ADD;
	ascii2dx["ADD"]=DIK_ADD;
	ascii2dx["-"]=DIK_MINUS;
	ascii2dx["MINUS"]=DIK_MINUS;

	ascii2dx["["]=DIK_LBRACKET;
	ascii2dx["LBRACKET"]=DIK_LBRACKET;
	ascii2dx["]"]=DIK_Z;
	ascii2dx["RBRACKET"]=DIK_RBRACKET;


	ascii2dx["="]=DIK_EQUALS;
	ascii2dx["EQUALS"]=DIK_EQUALS;
	ascii2dx["BACKSLASH"]=DIK_BACKSLASH;
	ascii2dx["UPARROW"]=DIK_UP;
	ascii2dx["DOWNARROW"]=DIK_DOWN;
	ascii2dx["LEFTARROW"]=DIK_LEFT;
	ascii2dx["RIGHTARROW"]=DIK_RIGHT;
	ascii2dx["PGUP"]=DIK_PGUP;
	ascii2dx["PGDN"]=DIK_PGDN;
	ascii2dx["HOME"]=DIK_HOME;
	ascii2dx["INSERT"]=DIK_INSERT;
	ascii2dx["DELETE"]=DIK_DELETE;
	ascii2dx["INS"]=DIK_INSERT;
	ascii2dx["DEL"]=DIK_DELETE;
	ascii2dx["END"]=DIK_END;
	ascii2dx["TAB"]=DIK_TAB;

	ascii2dx["^"]=DIK_GRAVE;			// is this really right ?
	ascii2dx["GRAVE"]=DIK_GRAVE;
	ascii2dx["~"]=DIK_GRAVE;
	ascii2dx["`"]=DIK_GRAVE;

	ascii2dx["CTRL"]=DIK_LCONTROL;
	ascii2dx["LCONTROL"]=DIK_LCONTROL;
	ascii2dx["RCONTROL"]=DIK_RCONTROL;
	ascii2dx["SHIFT"]=DIK_LSHIFT;
	ascii2dx["LSHIFT"]=DIK_LSHIFT;
	ascii2dx["RSHIFT"]=DIK_RSHIFT;
	ascii2dx["CAPSLOCK"]=DIK_CAPITAL;
	ascii2dx["ALT"]=DIK_LALT;		// both ALT keys are meant, though
	ascii2dx["LALT"]=DIK_LALT;
	ascii2dx["RALT"]=DIK_RALT;

	ascii2dx["0"]=DIK_0;
	ascii2dx["1"]=DIK_1;
	ascii2dx["2"]=DIK_2;
	ascii2dx["3"]=DIK_3;
	ascii2dx["4"]=DIK_4;
	ascii2dx["5"]=DIK_5;
	ascii2dx["6"]=DIK_6;
	ascii2dx["7"]=DIK_7;
	ascii2dx["8"]=DIK_8;
	ascii2dx["9"]=DIK_9;

	ascii2dx["KP_0"]=DIK_NUMPAD0;
	ascii2dx["KP_1"]=DIK_NUMPAD1;
	ascii2dx["KP_2"]=DIK_NUMPAD2;
	ascii2dx["KP_3"]=DIK_NUMPAD3;
	ascii2dx["KP_4"]=DIK_NUMPAD4;
	ascii2dx["KP_5"]=DIK_NUMPAD5;
	ascii2dx["KP_6"]=DIK_NUMPAD6;
	ascii2dx["KP_7"]=DIK_NUMPAD7;
	ascii2dx["KP_8"]=DIK_NUMPAD8;
	ascii2dx["KP_9"]=DIK_NUMPAD9;

	ascii2dx["F1"]=DIK_F1;
	ascii2dx["F2"]=DIK_F2;
	ascii2dx["F3"]=DIK_F3;
	ascii2dx["F4"]=DIK_F4;
	ascii2dx["F5"]=DIK_F5;
	ascii2dx["F6"]=DIK_F6;
	ascii2dx["F7"]=DIK_F7;
	ascii2dx["F8"]=DIK_F8;
	ascii2dx["F9"]=DIK_F9;
	ascii2dx["F10"]=DIK_F10;
	ascii2dx["F11"]=DIK_F11;
	ascii2dx["F12"]=DIK_F12;

	ascii2dx["PAUSE"]=DIK_PAUSE;
	ascii2dx["ESCAPE"]=DIK_ESCAPE;

}

const BYTE* CKeyboard::getBuffer()
{
	return m_keybuffer; // FIXME: do a getDeviceState before ?
}