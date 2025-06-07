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
#include "geometrymemmanager.h"

#include <cassert>

#include <misc/exception.h>
#include <opengl/opengl.h>
// #include <MemoryTracker.h>

GeometryMemManager::GeometryMemManager(void)
{
    staticMMGR  = NULL;  // manages static geometry memory
    dynamicMMGR = NULL;  // manages dynamic geometry memory
    sysMMGR     = NULL;  // manages geometry in system memory;

    manageMode = MM_SYSONLY;
}

GeometryMemManager::~GeometryMemManager(void)
{
    cleanUp();
}

bool GeometryMemManager::initialize(MANAGEMODE mode, unsigned int staticMemSize, unsigned int dynamicMemSize,
                                    unsigned int sysMemSize)
{
    assert(staticMemSize);
    assert(dynamicMemSize);
    assert(sysMemSize);

    manageMode = mode;

    switch (manageMode) {
    case MM_SYSONLY: {
        unsigned int size   = staticMemSize + dynamicMemSize + sysMemSize;
        char*        sysMem = new char[size];
        sysMMGR             = new MemoryManager;
        sysMMGR->setAlignment(64);
        sysMMGR->setBasePointer(sysMem, size);

        staticMMGR  = sysMMGR;
        dynamicMMGR = sysMMGR;
    } break;
    case MM_VARANDSYS: {
        COpenGL* ogl = COpenGL::Instance();
        if (!(ogl->getCaps() & GLCAPS_VERTEX_ARRAY_RANGE))
            throw CException("GL_NV_vertex_array_range not supported");

        int   size = dynamicMemSize + staticMemSize;
        void* ptr  = wglAllocateMemoryNV(size, 0.2f, 1.0f, 0.5f);
        if (!ptr)
            throw CException("wglAllocateMemoryNV() returned NULL");

        glVertexArrayRangeNV(size, ptr);
        glEnableClientState(GL_VERTEX_ARRAY_RANGE_NV);

        dynamicMMGR = new MemoryManager;
        dynamicMMGR->setAlignment(64);
        dynamicMMGR->setBasePointer((char*)ptr, size);

        staticMMGR = dynamicMMGR;

        char* sysMem = new char[sysMemSize];
        sysMMGR      = new MemoryManager;
        sysMMGR->setAlignment(32);
        sysMMGR->setBasePointer(sysMem, sysMemSize);

    } break;
    default:
        throw CException("GeometryMemManager::initialize() wrong managing mode");
        return false;
    }
    return true;
}

/** try to allocate the given amount of memory (in bytes) */
void* GeometryMemManager::alloc(MEMTYPE type, unsigned int size)
{
    assert(size);

    void* mem = NULL;
    switch (type) {
    case MT_STATIC:
        assert(staticMMGR);
        mem = staticMMGR->allocMem(size);
        break;
    case MT_DYNAMIC:
        assert(dynamicMMGR);
        mem = dynamicMMGR->allocMem(size);
        break;
    case MT_SYSMEM:
        assert(sysMMGR);
        mem = sysMMGR->allocMem(size);
        break;
    default:
        throw CException("GeometryMemManager::alloc() unknown memory type requested");
    }

    if (mem) {
        mtmap[mem] = type;
    } else {
        throw CException("GeometryMemManager::alloc() run out of memory");
    }
    return mem;
}

MEMTYPE GeometryMemManager::getType(void* pointer)
{
    MTITERATOR mIt = mtmap.find(pointer);
    if (mIt == mtmap.end()) {
        throw CException("GeometryMemManager::free() tried to reqeust type of invalid memory");
        return MT_UNKNOWN;
    }
    return (*mIt).second;
}

/** free the memory */
void GeometryMemManager::free(void* pointer)
{
    MTITERATOR mIt = mtmap.find(pointer);
    if (mIt == mtmap.end()) {
        throw CException("GeometryMemManager::free() tried to free invalid memory");
        return;
    }

    MEMTYPE type = (*mIt).second;

    switch (type) {
    case MT_STATIC:
        assert(staticMMGR);
        staticMMGR->freeMem((char*)pointer);
        break;
    case MT_DYNAMIC:
        assert(dynamicMMGR);
        dynamicMMGR->freeMem((char*)pointer);
        break;
    case MT_SYSMEM:
        assert(sysMMGR);
        sysMMGR->freeMem((char*)pointer);
        break;
    default:
        throw CException("GeometryMemManager::free() tried to free memory of unknown type ");
    }

    mtmap.erase(mIt);
}

void GeometryMemManager::cleanUp()
{
    switch (manageMode) {
    case MM_SYSONLY:
        if (sysMMGR) {
            char* sysMem = (char*)sysMMGR->getBasePointer();
            delete[] sysMem;

            delete sysMMGR;
            staticMMGR  = NULL;
            dynamicMMGR = NULL;
        }
        break;
    case MM_VARANDSYS:
        if (dynamicMMGR) {
            glFlushVertexArrayRangeNV();
            glDisableClientState(GL_VERTEX_ARRAY_RANGE_NV);
            wglFreeMemoryNV(dynamicMMGR->getBasePointer());
            delete dynamicMMGR;
            staticMMGR = NULL;

            char* sysMem = (char*)sysMMGR->getBasePointer();
            delete[] sysMem;
            delete sysMMGR;
        }
    }
}