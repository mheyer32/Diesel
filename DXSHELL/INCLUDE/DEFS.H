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
#ifndef DEFINES_H
#define DEFINES_H

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Defines
/////////////////////////////////////////////////////////////////////////////////////////////////////////
#define KILLOBJECT(Object)                                                                                             \
    {                                                                                                                  \
        delete Object;                                                                                                 \
        Object = NULL;                                                                                                 \
    };
#define KILLARRAY(Array)                                                                                               \
    {                                                                                                                  \
        delete[] Array;                                                                                                \
        Array = NULL;                                                                                                  \
    };

#define NEXTINDEX(i, max_i) ((i + 1) % max_i)
#define PREVINDEX(i, max_i) ((i + max_i - 1) % max_i)

#define EQ(s1, s2) (!(strcmp(s1, s2)))
typedef char CHARLINE[256];
const char   nl[] = {13, 10, 0};

#define SIGN(a) (fabs(a) == a ? 1.0f : -1.0f)

#define PI 3.14159265358979323846f
#define TWOPI (PI * 2.0f)
#define DEG(r) ((r) * (180.0f / PI))
#define RAD(d) ((d) * (PI / 180.0f))
#define NOFLAGS 0

#define DXINITSTRUCT(Struct)                                                                                           \
    ZeroMemory(&Struct, sizeof(Struct));                                                                               \
    Struct.dwSize = sizeof(Struct);
#define DXINITSTRUCTPTR(Struct)                                                                                        \
    ZeroMemory(Struct, sizeof(*Struct));                                                                               \
    Struct->dwSize = sizeof(*Struct);

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

#ifdef NDEBUG
#define BREAKIF(condition)
#else
#define BREAKIF(condition)                                                                                             \
    if (condition) {                                                                                                   \
        __asm int 3                                                                                                    \
    };
#endif

#endif