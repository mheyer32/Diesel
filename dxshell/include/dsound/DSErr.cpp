
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

#include <dsound.h>
#include "DSErr.h"

const char* DSErrorToString(HRESULT dsrval)
{
    switch (dsrval) {
    case DS_OK:
        return "The request completed successfully.";
    case DSERR_ALLOCATED:
        return "The request failed because resources, such as a priority level, were already in use by another caller.";
    case DSERR_ALREADYINITIALIZED:
        return "The object is already initialized.";
    case DSERR_BADFORMAT:
        return "The specified wave format is not supported.";
    case DSERR_BUFFERLOST:
        return "The buffer memory has been lost and must be restored.";
    case DSERR_CONTROLUNAVAIL:
        return "The control (volume, pan, and so forth) requested by the caller is not available.";
    case DSERR_GENERIC:
        return "An undetermined error occurred inside the DirectSound subsystem.";
    case DSERR_INVALIDCALL:
        return "This function is not valid for the current state of this object.";
    case DSERR_INVALIDPARAM:
        return "An invalid parameter was passed to the returning function.";
    case DSERR_NOAGGREGATION:
        return "The object does not support aggregation.";
    case DSERR_NODRIVER:
        return "No sound driver is available for use.";
    case DSERR_NOINTERFACE:
        return "The requested COM interface is not available.";
    case DSERR_OTHERAPPHASPRIO:
        return "Another application has a higher priority level, preventing this call from succeeding";
    case DSERR_OUTOFMEMORY:
        return "The DirectSound subsystem could not allocate sufficient memory to complete the caller's request.";
    case DSERR_PRIOLEVELNEEDED:
        return "The caller does not have the priority level required for the function to succeed.";
    case DSERR_UNINITIALIZED:
        return "The IDirectSound::Initialize method has not been called or has not been called successfully before "
               "other methods were called.";
    case DSERR_UNSUPPORTED:
        return "The function called is not supported at this time.";
    default:
        return "Unknown DirectSound errorcode";
    }
    return NULL;
}