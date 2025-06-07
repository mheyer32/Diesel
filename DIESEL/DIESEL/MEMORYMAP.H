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
// MemoryMap.h: Schnittstelle für die Klasse MemoryMap.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MEMORYMAP_H__97DB83D6_F320_4F00_ACE9_48CE7834D8D2__INCLUDED_)
#define AFX_MEMORYMAP_H__97DB83D6_F320_4F00_ACE9_48CE7834D8D2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER > 1000

class MemoryMap
{
public:
    MemoryMap();
    virtual ~MemoryMap();

    /** free all previously allocated blocks, set new amount of managed elements to
        num_elems*/
    void initialize(unsigned int num_elems);

    /** free all allocated blocks */
    void freeAll();

    /** allocate num_elems elements, the startindex is returned
        if allocation failed, 0xFFFFFFFF is returned
    */
    unsigned int getIndex(const unsigned int num_elems = 1);

    /** frees num_elems elements starting at 'index';
        MUST match the size allocated! */
    void freeIndex(unsigned int index, const unsigned int num_elems = 1);

    /** return count of free idices */
    unsigned int getFreeIndexCount() const;

protected:
    struct BLOCK
    {
        unsigned int start;
        unsigned int num_elements;
        BLOCK*       next;
    };

    BLOCK* freeblocks;  // essentially a list
};

#endif  // !defined(AFX_MEMORYMAP_H__97DB83D6_F320_4F00_ACE9_48CE7834D8D2__INCLUDED_)
