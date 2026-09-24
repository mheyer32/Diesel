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
// GLConsole.cpp: Implementierung der Klasse CGLConsole.
//
//////////////////////////////////////////////////////////////////////

#include "GLConsole.h"
#include "Shader.h"

#include <adt/ConCmd.h>
#include <misc/ParseHelper.h>
#include <win/AppWindow.h>
#include <iostream>

#include <MemoryTracker.h>
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
using namespace std;

std::string CGLConsole::commandbuffer;

// ConVar	CGLConsole::scr_conspeed("scr_conspeed","3");

CGLConsole::CGLConsole()
: scr_conspeed("scr_conspeed", "3")
{
    history     = NULL;
    history_pos = 0;

    cmd_history_pos = -1;
}

bool CGLConsole::initialize(CHistory<std::string>* newhistory)
{
    std::cout << std::endl << "======== CGLConsole::initialize() ========" << std::endl << std::endl;

    cmdline.clear();

    // FIXME: rethink this
    history = newhistory;
    history->setMaxHistoryLength(HISTORY_LENGTH);
    history_pos = 0;

    cmd_history.setMaxHistoryLength(CMD_HISTORY_LENGTH);
    cmd_history_pos = -1;

    setConSize(80, 25);

    // FIXME: make this more flexible
    CShader* background = CShader::findOrLoadShader("console");
    if (!background) {
        std::cout << "CGLConsole::initialize(): could not find background shader \"console\"" << std::endl;
    }

    gltext.SetBackgroundShader(background);  // supposed to catch a nullpointer
    RECT border = {4, 4, 4, 4};
    gltext.SetBorder(border);

    // bind "static" keys
    CKeyMap* km = CConKeyMap::Instance();
    km->SwitchKeymap(CKeyMap::KEYMAP1);
    km->BindKey(DIK_GRAVE, KeymapCB, NULL, CKeyMap::KEYOPT_TRIGGERONCE, this);
    km->SwitchKeymap(CKeyMap::KEYMAP2);
    for (int i = 0; i < 256; i++)
        km->BindKey(i, KeymapCB, NULL, CKeyMap::KEYOPT_REPEAT, this);
    km->BindKey(DIK_GRAVE, KeymapCB, NULL, CKeyMap::KEYOPT_TRIGGERONCE, this);

    cmd_toggleconsole.setCommand("toggleconsole", toggleconsole);

    // start timer for last update
    timer.start();

    show = newshow = SHOW_NOT;
    Show(SHOW_HALF);

    std::cout << "... successful" << std::endl;

    return true;
}

CGLConsole::~CGLConsole()
{
}

void CGLConsole::Render()
{
    setConsoleDestrect();

    // either the console is visible or it is going to be visible
    if (show != SHOW_NOT || newshow != SHOW_NOT) {
        gltext.Clr();
        int h    = history_pos + rows - 2;
        int maxh = history->getHistoryLength();
        for (int r = 0; r < rows - 1 && h < maxh; ++r) {
            gltext.Print((*history)[h--].c_str());
        }

        gltext.Locate(0, rows - 1);
        gltext.Print("]");
        std::string commandline = cmdline.getText();
        commandline.push_back(0x20);
        commandline.at(cmdline.getCursorPos()) |= 128;
        gltext.Print(commandline.c_str());

        gltext.RenderBuffer();
    } else {
        RECT destrect;
        destrect.left = destrect.top = 0;
        destrect.right               = CAppWindow::Instance()->getWidth();
        destrect.bottom              = CAppWindow::Instance()->getHeight() / 40 * 5;
        gltext2.SetDestRect(destrect);

        gltext2.RenderBuffer();
    }

    timer.stop();
    if (timer.getElapsedSecs() > 1.0) {
        gltext2.Print("\n");
        timer.start();
    }
}

void CGLConsole::Show(CGLConsole::CON_SHOW showconsole)
{
    if (show != showconsole) {
        if (show == newshow)
            animtimer.start();
        newshow = showconsole;
        if (newshow == SHOW_NOT) {
            CConKeyMap::Instance()->SwitchKeymap(CKeyMap::KEYMAP1);
        } else {
            CConKeyMap::Instance()->SwitchKeymap(CKeyMap::KEYMAP2);
            setConsoleDestrect();
        }
    }
}

CGLConsole::CON_SHOW CGLConsole::ToggleConsole()
{

    switch (show) {
    case SHOW_FULL:
    case SHOW_HALF:
        Show(SHOW_NOT);
        break;
    case SHOW_NOT:
        Show(SHOW_HALF);  // make it more flexible
        break;
    }
    return newshow;
}

CGLConsole::CON_SHOW CGLConsole::GetShow()
{
    return show;
}

void CGLConsole::toggleconsole(ConCmd& command, const std::string& args)
{
    CGLConsole::Instance()->ToggleConsole();
}

void CGLConsole::KeymapCB(int key, void* userdata)
{
    CGLConsole* console = (CGLConsole*)userdata;
    console->handleKey(key);
}

void CGLConsole::handleKey(int key)
{

    // std::map<int,std::string>::iterator foundkey=dx2ascii.find(key);

    switch (key) {
    case DIK_RETURN:
        std::cout << cmdline.getText() << std::endl;
        if (cmdline.getText().size()) {
            commandbuffer += cmdline;
            cmd_history.add(cmdline);
            cmd_history_pos = -1;
        }
        cmdline.clear();
        return;
        break;
    case DIK_BACKSPACE:
        if (cmdline.getCursorPos() > 0) {
            cmdline.cursorLeft();
            cmdline.deleteCharacter();
        }
        break;
    case DIK_GRAVE:
        CGLConsole::ToggleConsole();
        break;
    case DIK_UP:
        if (cmd_history_pos < cmd_history.getHistoryLength() - 1) {
            ++cmd_history_pos;
            cmdline.setText(cmd_history[cmd_history_pos]);
            cmdline.setCursorPos(300);
        }
        break;
    case DIK_DOWN:
        if (cmd_history_pos >= 0) {
            cmdline.setText(cmd_history[cmd_history_pos]);
            cmdline.setCursorPos(300);
            --cmd_history_pos;
        } else {
            cmdline.clear();  // clear command line
        }
        break;
    case DIK_LEFT:
        cmdline.cursorLeft();
        break;
    case DIK_RIGHT:
        cmdline.cursorRight();
        break;
    case DIK_DELETE:
        cmdline.deleteCharacter();
        break;
    case DIK_PGUP:
        if (history_pos < history->getHistoryLength() - rows - 1)
            history_pos++;
        break;
    case DIK_PGDN:
        if (history_pos > 0)
            history_pos--;
        break;
    case DIK_HOME:
        if (history_pos < history->getHistoryLength() - 2 * rows - 1)
            history_pos += rows;
        else
            history_pos = history->getHistoryLength() - rows - 1;
        break;
    case DIK_END:
        if (history_pos > rows)
            history_pos -= rows;
        else
            history_pos = 0;
        break;
    case DIK_TAB: {
        char                    temp[256];
        int                     size  = cmdline.getText().size();
        ConCmd::COMMANDMAP      list  = ConCmd::getCommandList();
        ConCmd::COMMANDITERATOR cmdIt = list.begin();
        std::list<std::string>  commands;
        // find all commands that start with the commandline-text
        for (; cmdIt != list.end(); ++cmdIt) {
            if (!strncmp(cmdline.getText().c_str(), (*cmdIt).second->getCommandName().c_str(), size))
                commands.push_front((*cmdIt).second->getCommandName());
        }
        if (commands.empty())
            break;
        strcpy(temp, commands.front().c_str());
        size = strlen(temp);
        if (commands.size() > 1) {
            std::list<std::string>::iterator cIt = commands.begin();
            // find the "common denominator" of all found commands
            for (; cIt != commands.end(); ++cIt) {
                std::cout << "    " << (*cIt) << std::endl;
                size       = ParseHelper::strcmp(temp, (*cIt).c_str(), size);
                temp[size] = 0;
            }
        }
        cmdline.setText(temp);
        cmdline.setCursorPos(300);
    } break;
    default:
        char character = CKeyboard::Instance()->getASCIIforKey(key);
        if (character <= 0x7E && character >= 0x20 && character != '^') {
            cmdline.insertCharacter(character);
        }
        break;
    }
}

void CGLConsole::setConSize(int Columns, int Rows)
{
    cols = Columns;
    rows = Rows;

    history_pos = 0;

    gltext.SetBufsize(cols, rows);
    gltext2.SetBufsize(cols, 5);
}

void CGLConsole::setConsoleDestrect()
{
    RECT destrect = {0, 0, 0, 0};

    destrect.right  = CAppWindow::Instance()->getWidth();
    destrect.bottom = CAppWindow::Instance()->getHeight();

    if (show == SHOW_HALF || newshow == SHOW_HALF) {
        destrect.bottom >>= 1;
    }

    // FIXME: currently only a half console is really working
    if (show != newshow) {
        animtimer.stop();
        float time = (float)animtimer.getElapsedSecs();
        time *= (float)scr_conspeed;

        if (time > 1.0f) {
            show = newshow;
        } else {
            if (newshow == SHOW_NOT) {
                time = 1.0f - time;
            }

            destrect.top    = -(float)destrect.bottom * (1.0f - time);
            destrect.bottom = (float)destrect.bottom * time;
        }
    }

    gltext.SetDestRect(destrect);
}

void CGLConsole::execCommandBuffer()
{
    if (!commandbuffer.empty()) {
        // FIXME this is not the right place for executing
        //  vid_restart needs to shut down almost every subsystem
        CKeyMap* keymap = CConKeyMap::Instance();
        int      km     = keymap->getKeymapNum();
        keymap->SwitchKeymap(CKeyMap::KEYMAP1);
        ConCmd::executeCommandLine(commandbuffer);
        keymap = CConKeyMap::Instance();
        keymap->SwitchKeymap(km);
        // executeCommandline should clear the buffer itself
        // to allow commands like "wait"
        commandbuffer.clear();
    }
}
