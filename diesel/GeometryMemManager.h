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
#pragma once

#include <adt/Singleton.h>
#include <map>
#include "MemoryManager.h"

// for convenience this is put into the global namespace
enum MEMTYPE
{
    MT_UNKNOWN,
    MT_STATIC,
    MT_DYNAMIC,
    MT_SYSMEM
};
enum MANAGEMODE
{
    MM_SYSONLY,
    MM_VARANDSYS
};

class GeometryMemManager : public Singleton<GeometryMemManager>
{
    DECLARE_SINGLETON(GeometryMemManager)

public:
    bool initialize(MANAGEMODE mode, unsigned int staticMem, unsigned int dynamicMem, unsigned int sysMem);

    /** try to allocate the given amount of memory (in bytes) */
    void* alloc(MEMTYPE type, unsigned int size);
    /** free the memory */
    void free(void* pointer);

    MEMTYPE              getType(void* pointer);
    const MemoryManager* getDynamicManager() const { return dynamicMMGR; };
    const MemoryManager* getStaticManager() const { return staticMMGR; };
    const MemoryManager* getSysManager() const { return sysMMGR; };

protected:
    typedef std::map<void*, MEMTYPE> MTMAP;
    typedef MTMAP::iterator          MTITERATOR;

    GeometryMemManager(void);  // hide constructor (Singleton)
    virtual ~GeometryMemManager();

    void cleanUp();

    MemoryManager* staticMMGR;   // manages static geometry memory
    MemoryManager* dynamicMMGR;  // manages dynamic geometry memory
    MemoryManager* sysMMGR;      // manages geometry in system memory;

    MTMAP mtmap;  // can assign a pointer to its memorymanager

    MANAGEMODE manageMode;
};
