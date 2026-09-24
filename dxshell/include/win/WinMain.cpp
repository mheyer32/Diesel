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

#include "WinMain.h"

#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>

#include <adt/ConVar.h>
#include <defs.h>
#include <misc/streambuf_history.h>

#include "AppWindow.h"
#include "WinException.h"

HINSTANCE hinstance = NULL;
LPSTR     lpcmdline = NULL;

CAppWindow* appwindow = NULL;

// cout will be rerouted to my own streambuffer which has a history of all printouts
// and can log to a file at the same time
streambuf_history streambuffer(256);
std::streambuf*   oldbuf = NULL;

void          CB_app_boostPriority(ConVar& var);
void          CB_app_log(ConVar& var);
static ConVar app_boostPriority("app_boostPriority", "0", 0, CB_app_boostPriority);
static ConVar app_log("app_log", "1", 0, CB_app_log);
static ConVar app_logfile("app_logfile", "logfile.log");

void MessageBox(std::string text);
int  MainEventLoop();

///////////////////////////////////////////////////////////////////////////////////////////////
// Application Entry
///////////////////////////////////////////////////////////////////////////////////////////////
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

    int return_value = 0;

    hinstance = hInstance;
    lpcmdline = lpCmdLine;

    try {
        // reroute COUT to Console
        oldbuf = std::cout.rdbuf();
        std::cout.rdbuf(&streambuffer);

        streambuffer.setMaxHistoryLength(3000);
        streambuffer.enableLogging((bool)app_log, (std::string)app_logfile);
        std::cout << "logging enabled" << std::endl;

        std::cout.setf(std::ios_base::showbase);

        appwindow = CAppWindow::Instance();
        if (!appwindow->open()) {
            MessageBox("WinMain()  could not open application window");
            return_value = -1;
        } else {
            if (!GameInit()) {
                GameShutDown();  // try to free all allocated resources till then
                MessageBox("WinMain() GameInit() failed");
                return_value = -1;
            } else {
                return_value = MainEventLoop();
            }
        }
    } catch (CException& e) {
        std::cout << "================================" << std::endl;
        std::cout << "******  EXCEPTION OCCURED ******" << std::endl;
        std::cout << "================================" << std::endl;
        std::cout << e.getCompleteText() << std::endl;
        std::cout << "================================" << std::endl << std::flush;

        MessageBox(e.getCompleteText());

#ifndef _DEBUG
        GameShutDown();
#else
        throw;
#endif
        return_value = -1;
    } catch (...) {
        std::cout << "=======================================" << std::endl;
        std::cout << "****** UNKNOWN EXCEPTION OCCURED ******" << std::endl;
        std::cout << "=======================================" << std::endl;
        std::cout << std::flush;

#ifndef _DEBUG
        MessageBox("an unhandled exception occurred");
#endif
        appwindow->setReady(false);
        SendMessage(appwindow->getHWND(), Msg::GAME_DEACTIVATED, 0, 0);

#ifndef _DEBUG
        GameShutDown();
#endif
#ifdef _DEBUG
        throw;
#endif
        return_value = -1;
    }

    if (!return_value) {
        // assume no problem occured, application quits normally
        appwindow->setReady(false);
        SendMessage(appwindow->getHWND(), Msg::GAME_DEACTIVATED, 0, 0);
        GameShutDown();
    }

    if (appwindow) {
        appwindow->close();
        CAppWindow::destroyInstance();
    }

    if (oldbuf) {
        std::cout.rdbuf(oldbuf);
    }

    return return_value;
}

void MessageBox(std::string text)
{
    MessageBox(NULL, text.c_str(), "DXShell Fatal Error", MB_OK);
    return;
}

int MainEventLoop()
{
    MSG msg;

    do {
        if (appwindow->isActive() && appwindow->isReady()) {
            if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
                TranslateAccelerator(appwindow->getHWND(), appwindow->getAcceleratorHandle(), &msg);
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            // Situation kann sich durch die Messages geändert haben
            if (appwindow->isActive() && appwindow->isReady())
                GameUpdate();
        } else {
            GetMessage(&msg, 0, 0, 0);
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    } while (msg.message != WM_QUIT);

    return 0;
}

//////////////////////////////////////////////////////////////////////////////////

HINSTANCE getInstanceHandle()
{
    return hinstance;
}

LPSTR getCommandLine()
{
    return lpcmdline;
}

CHistory<std::string>& getHistory()
{
    return streambuffer;
}

void CB_app_boostPriority(ConVar& var)
{
    if ((bool)var) {
        SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);
    } else {
        SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
    }
}

void CB_app_log(ConVar& var)
{
    streambuffer.enableLogging((bool)var, (std::string)app_logfile);
}
