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

#pragma once

#include <map>
#include "MemoryMap.h"

class MemoryManager
{
public:
    MemoryManager();
    virtual ~MemoryManager();

    /** try to allocate the given amount of memory (in units of char) */
    char* allocMem(unsigned int size);
    /** free the memory */
    void freeMem(char* pointer);

    /** set needed Alignment, set before calling setBasePointer() */
    void setAlignment(unsigned int alignment);

    /** set continues block of memory to be managed */
    void setBasePointer(char* ptr, unsigned int size);

    char*        getBasePointer() const { return realbaseptr; };
    unsigned int getSize() const { return realmemsize; };

    /** return remaining free memory */
    unsigned int getFreeMemorySize() const;

    /** return allocated memory */
    unsigned int getAllocatedMemorySize() const;

    /** return number of allocated memory blocks */
    unsigned int getBlockCount() const { return (unsigned int)mcbmap.size(); }

    const MemoryMap& getMemoryMap() const { return memmap; }

    void freeAll();

protected:
    struct MCB
    {
        unsigned int index;
        unsigned int size;
    };

    typedef std::map<char*, MCB> MCBMAP;
    typedef MCBMAP::iterator     MCBITERATOR;

    MCBMAP       mcbmap;   // fast assignment of memoryptr and index-range in memorymap
    MemoryMap    memmap;   // manages free and allocated blocks
    char*        baseptr;  // aligned base pointer in memory
    unsigned int memsize;  // aligned size of the continues block of memory to be managed

    char*        realbaseptr;
    unsigned int realmemsize;

    unsigned int align;
};
