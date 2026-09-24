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
// ConKeyMap.cpp: Implementierung der Klasse CConKeyMap.
//
//////////////////////////////////////////////////////////////////////

#include <adt/ConCmd.h>
#include <defs.h>
#include <dinput/Keyboard.h>
#include "ConKeyMap.h"

#include <MemoryTracker.h>
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CConKeyMap::CConKeyMap()
{
    cmd_bind      = new ConCmd("bind", bind);
    cmd_unbind    = new ConCmd("unbind", bind);
    cmd_unbindall = new ConCmd("unbindall", bind);

    // this table would be used if I´d not use windows´  WM_CHAR
    dx2ascii[DIK_A]         = 'a';
    dx2ascii[DIK_B]         = 'b';
    dx2ascii[DIK_C]         = 'c';
    dx2ascii[DIK_D]         = 'd';
    dx2ascii[DIK_E]         = 'e';
    dx2ascii[DIK_F]         = 'f';
    dx2ascii[DIK_G]         = 'g';
    dx2ascii[DIK_H]         = 'h';
    dx2ascii[DIK_I]         = 'i';
    dx2ascii[DIK_J]         = 'j';
    dx2ascii[DIK_K]         = 'k';
    dx2ascii[DIK_L]         = 'l';
    dx2ascii[DIK_M]         = 'm';
    dx2ascii[DIK_N]         = 'n';
    dx2ascii[DIK_O]         = 'o';
    dx2ascii[DIK_P]         = 'p';
    dx2ascii[DIK_Q]         = 'q';
    dx2ascii[DIK_R]         = 'r';
    dx2ascii[DIK_S]         = 's';
    dx2ascii[DIK_T]         = 't';
    dx2ascii[DIK_U]         = 'u';
    dx2ascii[DIK_V]         = 'v';
    dx2ascii[DIK_W]         = 'w';
    dx2ascii[DIK_X]         = 'x';
    dx2ascii[DIK_Y]         = 'y';
    dx2ascii[DIK_Z]         = 'z';
    dx2ascii[DIK_0]         = '0';
    dx2ascii[DIK_9]         = '9';
    dx2ascii[DIK_8]         = '8';
    dx2ascii[DIK_7]         = '7';
    dx2ascii[DIK_6]         = '6';
    dx2ascii[DIK_5]         = '5';
    dx2ascii[DIK_4]         = '4';
    dx2ascii[DIK_3]         = '3';
    dx2ascii[DIK_2]         = '2';
    dx2ascii[DIK_1]         = '1';
    dx2ascii[DIK_SPACE]     = ' ';
    dx2ascii[DIK_SEMICOLON] = ';';
    dx2ascii[DIK_COMMA]     = ',';
    dx2ascii[DIK_PERIOD]    = '.';
    dx2ascii[DIK_DECIMAL]   = '.';
    dx2ascii[DIK_SLASH]     = '/';
    dx2ascii[DIK_MULTIPLY]  = '*';
    dx2ascii[DIK_LBRACKET]  = '[';
    dx2ascii[DIK_RBRACKET]  = ']';
    dx2ascii[DIK_MINUS]     = '-';
    dx2ascii[DIK_SUBTRACT]  = '-';
    dx2ascii[DIK_EQUALS]    = '=';
    dx2ascii[DIK_BACKSLASH] = '\\';
    dx2ascii[DIK_ADD]       = '+';
}

CConKeyMap::~CConKeyMap()
{
    SwitchKeymap(KEYMAP1);
    UnbindAllUserKey();  // damit die gespeicherten userdata-Commandozeilen gelöscht werden

    KILLOBJECT(cmd_bind)
    KILLOBJECT(cmd_unbind)
    KILLOBJECT(cmd_unbindall)
}

void CConKeyMap::bind(ConCmd& Command, const std::string& Arg)
{
    CConKeyMap* kmap = CConKeyMap::Instance();

    if (&Command == kmap->cmd_bind)  // bind
    {
        char keyname[256];
        // FIXME: do better handling of cmdline
        char* cmdline  = new char[Arg.size()];  // wird von unbind freigegeben!
        int   num_args = sscanf(Arg.c_str(), "%s %[^\n]s", keyname, cmdline);
        if (num_args < 2) {
            KILLARRAY(cmdline)
            PrintBindings(0);
            return;
        }
        DWORD key;
        if (!CKeyboard::Instance()->getKeyforASCII(keyname, key)) {
            KILLARRAY(cmdline)
            std::cout << "tried to bind unknown key '" << keyname << "'" << std::endl;
            return;
        }
        kmap->BindUserKey(key, cmdline);

    } else if (&Command == kmap->cmd_unbind)  // unbind
    {
        DWORD key;
        if (!CKeyboard::Instance()->getKeyforASCII(Arg.c_str(), key)) {
            std::cout << "tried to unbind unknown key" << std::endl;
            return;
        }
        kmap->UnbindUserKey(key);

    } else  // unbindall
    {
        kmap->UnbindAllUserKey();
    }
}

void CConKeyMap::ExecBinding(int key, void* userdata)
{
    ConCmd::executeCommandLine((char*)userdata);
}

void CConKeyMap::PrintBindings(int key)
{
    CConKeyMap* kmap = Instance();

    if (key) {
        if (kmap->keymap[key].pressCB != NULL && kmap->keymap[key].pressCB == &ExecBinding) {
            std::cout << "'" << kmap->dx2ascii[key] << "' is bound to '" << (char*)kmap->keymap[key].userdata << "'"
                      << std::endl;
        } else {
            std::cout << "'" << kmap->dx2ascii[key] << "' is not bound to a console command" << std::endl;
        }
        return;
    }
    for (key = 0; key < 255; ++key) {
        if (kmap->keymap[key].pressCB != NULL && kmap->keymap[key].pressCB == &ExecBinding) {
            std::cout << "'" << kmap->dx2ascii[key] << "' is bound to '" << (char*)kmap->keymap[key].userdata << "'"
                      << std::endl;
        }
    }
    return;
}

// bind the key to execute a shell-command
// the User can change this key from the console
void CConKeyMap::BindUserKey(int key, char* commandline)
{
    int oldmapnum = keymapnum;
    SwitchKeymap(0);

    if (keymap[key].pressCB == NULL || keymap[key].pressCB == ExecBinding || keymap[key].releaseCB == ExecBinding) {
        if (keymap[key].userdata)
            delete[] (char*)(keymap[key].userdata);
        if (commandline[0] == '+') {
            commandline[0] = ' ';
            BindKey(key, ExecBinding, NULL, KEYOPT_CONTINUE, commandline);
        } else if (commandline[0] == '-') {
            commandline[0] = ' ';
            BindKey(key, NULL, ExecBinding, KEYOPT_CONTINUE, commandline);
        } else {
            BindKey(key, ExecBinding, NULL, KEYOPT_TRIGGERONCE, commandline);
        }
        // std::cout<<"key: '"<<key<<"' bound to: '"<<commandline<<"'"<<std::endl;
    } else {
        std::cout << "this key cannot be bound with the 'bind' command" << std::endl;
    }
    SwitchKeymap(oldmapnum);
}

void CConKeyMap::UnbindUserKey(int key)
{
    int oldmapnum = keymapnum;
    SwitchKeymap(0);

    if (keymap[key].pressCB == ExecBinding || keymap[key].releaseCB == ExecBinding) {
        if (keymap[key].userdata)
            delete[] (char*)(keymap[key].userdata);
        UnBindKey(key);
    } else {
        std::cout << "you cant unbind a key that was not bound with the 'bind' command" << std::endl;
    }
    SwitchKeymap(oldmapnum);
}

void CConKeyMap::UnbindAllUserKey()
{
    int oldmapnum = keymapnum;
    SwitchKeymap(0);

    for (int key = 0; key < 256; key++) {
        if (keymap[key].pressCB == ExecBinding || keymap[key].releaseCB == ExecBinding) {
            if (keymap[key].userdata)
                delete[] (char*)(keymap[key].userdata);
            UnBindKey(key);
        }
    }

    SwitchKeymap(oldmapnum);
}