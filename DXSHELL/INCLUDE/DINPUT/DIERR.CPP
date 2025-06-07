
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
#include "dierr.h"

#include "directinput.h"  // important to include the dinput.h ONLY in directinput.h

const char* DIErrorToString(HRESULT dirval)
{
    switch (dirval) {
    case DI_BUFFEROVERFLOW:
        return "The device buffer overflowed and some input was lost. This value is equal to the S_FALSE standard COM "
               "return value.";
    case DI_DOWNLOADSKIPPED:
        return "The parameters of the effect were successfully updated, but the effect could not be downloaded because "
               "the associated device was not acquired in exclusive mode.";
    case DI_EFFECTRESTARTED:
        return "The effect was stopped, the parameters were updated, and the effect was restarted.";
        //	case DI_NOEFFECT:
        //		return "The operation had no effect. This value is equal to the S_FALSE standard COM return value.";
        //	case DI_NOTATTACHED:
        //		return "The device exists but is not currently attached. This value is equal to the S_FALSE standard COM
        //return value.";
    case DI_OK:
        return "The operation completed successfully. This value is equal to the S_OK standard COM return value.";
    case DI_POLLEDDEVICE:
        return "The device is a polled device. As a result, device buffering does not collect any data and event "
               "notifications is not signaled until the IDirectInputDevice8::Poll method is called.";
        //	case DI_PROPNOEFFECT:
        //		return "The change in device properties had no effect. This value is equal to the S_FALSE standard COM
        //return value.";
    case DI_SETTINGSNOTSAVED:
        return "The action map was applied to the device, but the settings could not be saved.";
    case DI_TRUNCATED:
        return "The parameters of the effect were successfully updated, but some of them were beyond the capabilities "
               "of the device and were truncated to the nearest supported value.";
    case DI_TRUNCATEDANDRESTARTED:
        return "Equal to DI_EFFECTRESTARTED | DI_TRUNCATED.";
    case DI_WRITEPROTECT:
        return "A SUCCESS code indicating that settings cannot be modified.";
    case DIERR_ACQUIRED:
        return "The operation cannot be performed while the device is acquired.";
    case DIERR_ALREADYINITIALIZED:
        return "This object is already initialized";
    case DIERR_BADDRIVERVER:
        return "The object could not be created due to an incompatible driver version or mismatched or incomplete "
               "driver components.";
    case DIERR_BETADIRECTINPUTVERSION:
        return "The application was written for an unsupported prerelease version of DirectInput.";
    case DIERR_DEVICEFULL:
        return "The device is full.";
    case DIERR_DEVICENOTREG:
        return "The device or device instance is not registered with DirectInput. This value is equal to the "
               "REGDB_E_CLASSNOTREG standard COM return value.";
    case DIERR_EFFECTPLAYING:
        return "The parameters were updated in memory but were not downloaded to the device because the device does "
               "not support updating an effect while it is still playing.";
    case DIERR_GENERIC:
        return "An undetermined error occurred inside the DirectInput subsystem. This value is equal to the E_FAIL "
               "standard COM return value.";
    case DIERR_HANDLEEXISTS:
        return "The device already has an event notification associated with it. This value is equal to the "
               "E_ACCESSDENIED standard COM return value.";
    case DIERR_HASEFFECTS:
        return "The device cannot be reinitialized because effects are attached to it.";
    case DIERR_INCOMPLETEEFFECT:
        return "The effect could not be downloaded because essential information is missing. For example, no axes have "
               "been associated with the effect, or no type-specific information has been supplied.";
    case DIERR_INPUTLOST:
        return "Access to the input device has been lost. It must be reacquired.";
    case DIERR_INVALIDPARAM:
        return "An invalid parameter was passed to the returning function, or the object was not in a state that "
               "permitted the function to be called. This value is equal to the E_INVALIDARG standard COM return "
               "value.";
    case DIERR_MAPFILEFAIL:
        return "An error has occurred either reading the vendor-supplied action-mapping file for the device or reading "
               "or writing the user configuration mapping file for the device.";
    case DIERR_MOREDATA:
        return "Not all the requested information fit into the buffer.";
    case DIERR_NOAGGREGATION:
        return "This object does not support aggregation.";
    case DIERR_NOINTERFACE:
        return "The object does not support the specified interface. This value is equal to the E_NOINTERFACE standard "
               "COM return value.";
    case DIERR_NOTACQUIRED:
        return "The operation cannot be performed unless the device is acquired.";
    case DIERR_NOTBUFFERED:
        return "The device is not buffered. Set the DIPROP_BUFFERSIZE property to enable buffering.";
    case DIERR_NOTDOWNLOADED:
        return "The effect is not downloaded.";
    case DIERR_NOTEXCLUSIVEACQUIRED:
        return "The operation cannot be performed unless the device is acquired in DISCL_EXCLUSIVE mode.";
    case DIERR_NOTFOUND:
        return "The requested object does not exist.";
    case DIERR_NOTINITIALIZED:
        return "This object has not been initialized.";
        //	case DIERR_OBJECTNOTFOUND:
        //		return "The requested object does not exist.";
    case DIERR_OLDDIRECTINPUTVERSION:
        return "The application requires a newer version of DirectInput.";
        //	case DIERR_OTHERAPPHASPRIO:
        //		return "Another application has a higher priority level, preventing this call from succeeding. This
        //value is equal to the E_ACCESSDENIED standard COM return value. This error can be returned when an application
        //has only foreground access to a device but is attempting to acquire the device while in the background.";
    case DIERR_OUTOFMEMORY:
        return "The DirectInput subsystem could not allocate sufficient memory to complete the call. This value is "
               "equal to the E_OUTOFMEMORY standard COM return value.";
        //	case DIERR_READONLY:
        //		return "The specified property cannot be changed. This value is equal to the E_ACCESSDENIED standard COM
        //return value.";
    case DIERR_REPORTFULL:
        return "More information was requested to be sent than can be sent to the device.";
    case DIERR_UNPLUGGED:
        return "The operation could not be completed because the device is not plugged in.";
    case DIERR_UNSUPPORTED:
        return "The function called is not supported at this time. This value is equal to the E_NOTIMPL standard COM "
               "return value.";
    case E_HANDLE:
        return "The HWND parameter is not a valid top-level window that belongs to the process.";
    case E_PENDING:
        return "Data is not yet available.";
    case E_POINTER:
        return "An invalid pointer, usually NULL, was passed as a parameter.";
    }
    return "unknown DirectInput Error";
}