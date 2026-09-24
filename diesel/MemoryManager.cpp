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

#include "MemoryManager.h"

#include <assert.h>
#include <iostream>
#include <strstream>

#include <misc/Exception.h>
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

MemoryManager::MemoryManager()
{
    baseptr     = 0;
    memsize     = 0;
    realbaseptr = 0;
    realmemsize = 0;

    align = 0;
}

MemoryManager::~MemoryManager()
{
    freeAll();
}

char* MemoryManager::allocMem(unsigned int size)
{
    MCB mcb;
    if (!size)
        throw CException("MemoryManager::allocMem() tried to allocate 0-sized block");
    mcb.size = (size + align + 1);
    if ((mcb.index = memmap.getIndex(mcb.size)) != -1) {
        // align index
        unsigned int useindex = (mcb.index + align) & (~align);

        char* ptr = baseptr + useindex;  // cheat a bit to allow pointer-arithmetic

        // memorize the memory control block
        mcbmap[ptr] = mcb;
        return ptr;
    } else {
        std::cout << std::endl;

        std::strstream stream;
        stream << "MemoryManager::allocMem() failed, tried to allocate " << size << " bytes";
        throw CException(stream.str());
        return 0;  // FIXME: throw exception here ?
    }
}

void MemoryManager::freeMem(char* ptr)
{
    MCBITERATOR mIt = mcbmap.find(ptr);
    if (mIt != mcbmap.end()) {
        memmap.freeIndex((*mIt).second.index, (*mIt).second.size);
        mcbmap.erase(mIt);
    } else {
        throw CException("MemoryManager::freeMem invalid pointer given!");
    }
}

void MemoryManager::setBasePointer(char* ptr, unsigned int size)
{
    assert(ptr);
    assert(size);

    freeAll();

    realmemsize = size;
    realbaseptr = ptr;

    // align base
    // FIXME: this method contradicts the possibility of using templates!
    baseptr = (char*)((ptrdiff_t)(realbaseptr + (ptrdiff_t)align - 1) & ~(ptrdiff_t)align);
    memsize = realmemsize & ~align;

    memmap.initialize(memsize);
}

void MemoryManager::freeAll()
{
    memmap.freeAll();
    mcbmap.clear();
    memmap.initialize(memsize);
}

unsigned int MemoryManager::getFreeMemorySize() const
{
    return memmap.getFreeIndexCount();
}

unsigned int MemoryManager::getAllocatedMemorySize() const
{
    return memsize - memmap.getFreeIndexCount();
}

void MemoryManager::setAlignment(unsigned int alignment)
{
    if (baseptr)
        throw CException("MemoryManager::setAlignment() change of alignment not allowed after setting the basepointer");
    if (!alignment)
        throw CException("MemoryManager::setAlignment() alignment of 0 is not allowed!");

    align = alignment - 1;
}