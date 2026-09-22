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
#include "winerr.h"

#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#include <windows.h>

const char* WinErrorToString()
{
    return WinErrorToString(GetLastError());
}

const char* WinErrorToString(int errornum)
{
    static char errortext[1024];

    char* textptr;
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
                  errornum, 0, (char*)&textptr, 0, 0);
    if (textptr) {
        strcpy(errortext, textptr);
        LocalFree(textptr);  // so we don´t get leaks
    } else {
        strcpy(errortext, "no error text: WinErrorToString(): FormatMessage() returned NULL");
    }

    return errortext;
}