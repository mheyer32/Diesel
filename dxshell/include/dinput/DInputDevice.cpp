
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
// DInputDevice.cpp: Implementierung der Klasse CDInputDevice.
//
//////////////////////////////////////////////////////////////////////

#include <defs.h>
#include <misc/Exception.h>
#include <win/AppWindow.h>
#include <iostream>
#include "DInputDevice.h"
#include "DIErr.h"
#include "DInputException.h"

#include <MemoryTracker.h>
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CDInputDevice::CDInputDevice()
{

    Device          = NULL;
    DeviceEvents[0] = NULL;
    DeviceEvents[1] = NULL;
    ThreadHandle    = NULL;

    num_Axes    = 0;
    num_Buttons = 0;
    num_POV     = 0;
    Axes        = NULL;
    Buttons     = NULL;
    POV         = NULL;

    Format_autodetected = false;
}

CDInputDevice::~CDInputDevice()
{
    CAppWindow* appwindow = CAppWindow::Instance();
    unregisterForMessage(appwindow, Msg::GAME_ACTIVATED);
    unregisterForMessage(appwindow, Msg::AUTOACQUIRE);

    releaseDevice();
}

void CDInputDevice::releaseDevice()
{
    StopDeviceThread();

    if (Device != NULL) {
        Device->Unacquire();
        Device->Release();
        Device = NULL;
    };

    if (Format_autodetected) {
        KILLARRAY(DeviceDataFormat->rgodf)
        KILLOBJECT(DeviceDataFormat)
        KILLOBJECT(DeviceState)
    }
}

bool CDInputDevice::InitDevice(REFGUID DeviceGUID, DIDATAFORMAT* DataFormat, DWORD Cooplevel)
{
    HRESULT dirval;
    // DInput-Device  öffnen

    std::cout << std::endl << "======== CDInputDevice::InitDevice() ========" << std::endl << std::endl;

    if (CDirectInput::Instance()->getlpDI() == NULL) {
        if (!CDirectInput::Instance()->InitDirectInput()) {
            return false;
        }
    }

    guid      = DeviceGUID;
    cooplevel = Cooplevel;

    std::cout << "CreateDeviceEx().... ";
    dirval = CDirectInput::Instance()->getlpDI()->CreateDevice(DeviceGUID, &Device, NULL);

    if (dirval != DI_OK) {
        std::cout << "failed, because" << DIErrorToString(dirval) << std::flush;
        throw CDInputException("CDInputDevice::InitDevice() lpDI->CreateDevice() failed", dirval);
        return false;
    }

    std::cout << "successful" << std::endl;

    std::cout << "get device capablilities.... " << std::endl;
    DIDEVCAPS caps;
    DXINITSTRUCT(caps);
    Device->GetCapabilities(&caps);
    num_Axes    = caps.dwAxes;
    num_Buttons = caps.dwButtons;
    num_POV     = caps.dwPOVs;

    std::cout << num_Axes << " axes, " << num_Buttons << " buttons, " << num_POV << " povs" << std::endl;

    // set the cooperative level
    std::cout << "SetCooperativeLevel().... ";
    dirval = Device->SetCooperativeLevel(CAppWindow::Instance()->getHWND(), cooplevel);

    if (dirval != DI_OK) {
        std::cout << " failed,because " << DIErrorToString(dirval) << std::endl;
        return false;
    }
    std::cout << " successful" << std::endl;

    // set dataformat
    if (DataFormat != NULL) {
        std::cout << "SetDataFormat()... ";
        dirval = Device->SetDataFormat(DataFormat);

        if (dirval != DI_OK) {
            std::cout << "failed,because " << DIErrorToString(dirval) << std::endl;
            return false;
        }
        DeviceDataFormat = DataFormat;
        std::cout << "successful" << std::endl;
    } else {
        dirval = AutoDetectDataFormat();
    }

    CAppWindow* appwindow = CAppWindow::Instance();

    std::cout << "registering for window messages" << std::endl;
    if (cooplevel & (DISCL_EXCLUSIVE | DISCL_FOREGROUND)) {
        registerForMessage(appwindow, Msg::GAME_ACTIVATED);
        registerForMessage(appwindow, Msg::AUTOACQUIRE);
    }

    // acquire device
    std::cout << "Acquire()... ";
    dirval = Device->Acquire();

    if (dirval != DI_OK) {
        std::cout << "failed,because:\n" << DIErrorToString(dirval) << "\n will try again" << std::endl;
        PostMessage(appwindow->getHWND(), Msg::AUTOACQUIRE, 0, 0);
        return true;  // mal sehen, ob das eine gute Idee ist
    }

    std::cout << "successful" << std::endl;
    std::cout << std::endl << "device successfully initialized" << std::endl << std::endl;
    return true;
}

Msg::MSGRVAL CDInputDevice::handleMessage(Msg::MessagingObject* sender,
                                          Msg::MESSAGEID        msgId,
                                          const Msg::Param&     parameters)
{
    HRESULT dirval;

    switch (msgId) {
    case Msg::GAME_ACTIVATED:
    case Msg::AUTOACQUIRE:
        if (Device != NULL) {
            // dirval = Device->SetDataFormat(DeviceDataFormat); //FIXME: ever needed?
            dirval = Device->Acquire();
            // if (((dirval!=DI_OK)||(WinStart::bCoopLevelOK==FALSE))&&(dirval!=S_FALSE))
            // FIXME: check for Cooplevel still needed?
            if ((dirval != DI_OK) && (dirval != S_FALSE)) {
                // this would really block the app, need something else here
                // PostMessage(CAppWindow::Instance()->getHWND(),WM_AUTOACQUIRE,0,0);
            }
        }
        break;
    }

    return Msg::PASS_ON;
}

HRESULT CDInputDevice::GetDeviceState()
{
    return Device->GetDeviceState(DeviceStateSize, DeviceState);
}

bool CDInputDevice::SetupDeviceThread(void (*Handler)())
{
    HRESULT dirval;

    DeviceEvents[0] = CreateEvent(0, 0, 0, 0);
    DeviceEvents[1] = CreateEvent(0, 0, 0, 0);

    DeviceHandler = Handler;

    DWORD ThreadId = NULL;

    ThreadHandle = CreateThread(0,                                     // pointer to security attributes
                                0,                                     // initial thread stack size
                                (LPTHREAD_START_ROUTINE)DeviceThread,  // pointer to thread function
                                this,                                  // argument for new thread
                                0,                                     // creation flags
                                &ThreadId                              // pointer to receive thread ID
    );
    if (ThreadHandle == NULL) {
        throw CException("DInputDevice->DeviceThread could not created");
        return false;
    }
    SetThreadPriority(ThreadHandle, THREAD_PRIORITY_ABOVE_NORMAL);

    Device->Unacquire();
    dirval = Device->SetEventNotification(DeviceEvents[0]);

    if (dirval != DI_OK) {
        throw CException("DInputDevice->SetEventNotification failed");
        return false;
    }
    Device->Acquire();
    return true;
}

void CDInputDevice::StopDeviceThread()
{
    DWORD signal;
    if (Device != NULL) {
        Device->SetEventNotification(NULL);
    }
    if ((ThreadHandle != NULL) && (DeviceEvents[1] != NULL)) {
        SetEvent(DeviceEvents[1]);
        signal = WaitForSingleObject(ThreadHandle, 5000);
        if (signal == WAIT_TIMEOUT) {
            TerminateThread(ThreadHandle, -1);
        }
        CloseHandle(ThreadHandle);
    };
    if (DeviceEvents[0] != NULL)
        CloseHandle(DeviceEvents[0]);
    if (DeviceEvents[1] != NULL)
        CloseHandle(DeviceEvents[1]);
}

DWORD WINAPI CDInputDevice::DeviceThread(CDInputDevice* DeviceObject)
{
    HRESULT dirval;

LOOP:

    DWORD dwEvt = MsgWaitForMultipleObjects(2,                           // How many possible events
                                            DeviceObject->DeviceEvents,  // Location of handles
                                            FALSE,                       // Wait for all?
                                            INFINITE,                    // How long to wait
                                            QS_ALLEVENTS);               // Any message is an event

    dwEvt -= WAIT_OBJECT_0;

    switch (dwEvt) {
    case 0:
        dirval = DeviceObject->Device->GetDeviceState(DeviceObject->DeviceStateSize, DeviceObject->DeviceState);
        if ((dirval == DI_OK) && (DeviceObject->DeviceHandler != NULL)) {
            DeviceObject->DeviceHandler();
        }
        /*	else if(dirval==DIERR_NOTACQUIRED)
            {
                DeviceObject->MessageHandler();
            }
            */
        break;
    case 1:
        ExitThread(0);
        break;
    }
    goto LOOP;
}

HRESULT CDInputDevice::AutoDetectDataFormat()
{
    HRESULT dirval;
    DWORD   act_axis, act_button, act_pov, act_object, num_objects;

    dirval = Device->EnumObjects(CDInputDevice::DIEnumDeviceObjectsCallback,
                                 (LPVOID)&ObjectList,
                                 DIDFT_AXIS | DIDFT_BUTTON | DIDFT_POV);
    if (dirval != DI_OK) {
        throw CDInputException("CDInputDevice::AutoDetectDataFormat() fehlgeschlagen", dirval);
        return dirval;
    }

    num_objects = num_Axes + num_Buttons + num_POV;

    DIOBJECTDATAFORMAT* objectdataformat = new DIOBJECTDATAFORMAT[num_objects];

    DeviceStateSize = num_Axes * sizeof(long) + num_Buttons + num_POV * sizeof(DWORD);
    // datenbytes auf 4-Byte-Boundary bringen
    if (DeviceStateSize & 0x00000003) {
        DeviceStateSize = (DeviceStateSize & 0xFFFFFFFC) + 4;
    }

    // "Emfpangspuffer" anlegen
    DeviceState = new BYTE[DeviceStateSize];

    act_axis = act_button = act_pov = act_object = 0;

    int startPOVs    = sizeof(long) * num_Axes;
    int startButtons = sizeof(long) * num_Axes + sizeof(DWORD) * num_POV;

    // OBJECTDATAFORMATs festlegen: Axen:4 Bytes, Buttons 1 Byte, POVs 4 bytes (?)
    DIDEVICEOBJECTINSTANCE object;
    DIDOBJITERATOR         dIt = ObjectList.begin();

    for (; dIt != ObjectList.end(); ++dIt) {
        object                               = *dIt;
        objectdataformat[act_object].dwFlags = 0;
        objectdataformat[act_object].dwType  = object.dwType;
        objectdataformat[act_object].pguid   = &object.guidType;

        if (object.dwType & DIDFT_AXIS) {
            objectdataformat[act_object].dwOfs = sizeof(long) * act_axis++;
        } else if (object.dwType & DIDFT_POV) {
            objectdataformat[act_object].dwOfs = startPOVs + sizeof(DWORD) * act_pov++;
        } else if (object.dwType & DIDFT_BUTTON) {
            objectdataformat[act_object].dwOfs = startButtons + act_button++;
        };

        act_object++;
    }

    DeviceDataFormat = new DIDATAFORMAT;

    DXINITSTRUCTPTR(DeviceDataFormat);

    DeviceDataFormat->dwObjSize  = sizeof(DIOBJECTDATAFORMAT);
    DeviceDataFormat->dwDataSize = DeviceStateSize;
    DeviceDataFormat->dwFlags    = DIDF_ABSAXIS;
    DeviceDataFormat->dwNumObjs  = num_objects;
    DeviceDataFormat->rgodf      = objectdataformat;

    dirval = Device->SetDataFormat(DeviceDataFormat);
    if (dirval != DI_OK) {
        throw CDInputException("Device->SetDataFormat fehlgeschlagen", dirval);
        return dirval;
    }

    Axes    = (long*)DeviceState;
    POV     = (DWORD*)((BYTE*)DeviceState + num_Axes * sizeof(long));
    Buttons = (BYTE*)((BYTE*)DeviceState + num_Axes * sizeof(long) + num_POV * sizeof(DWORD));

    return DI_OK;
}

BOOL CALLBACK CDInputDevice::DIEnumDeviceObjectsCallback(LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID pvRef)
{
    DIDOBJLIST*            ObjectList;
    DIDEVICEOBJECTINSTANCE object;

    ObjectList = (DIDOBJLIST*)pvRef;
    object     = *lpddoi;
    ObjectList->push_back(object);
    return DIENUM_CONTINUE;
}

bool CDInputDevice::reinitialize()
{
    return InitDevice(guid, DeviceDataFormat, cooplevel);
}

HRESULT CDInputDevice::acquire()
{
    return Device->Acquire();
}

int CDInputDevice::getNumAxes() const
{
    return num_Axes;
}
int CDInputDevice::getNumButtons() const
{
    return num_Buttons;
}

int CDInputDevice::getNumPOVs() const
{
    return num_POV;
}

const long* CDInputDevice::getAxes() const
{
    return Axes;
}
const DWORD* CDInputDevice::getPOVs() const
{
    return POV;
}
const BYTE* CDInputDevice::getButtons() const
{
    return Buttons;
}
