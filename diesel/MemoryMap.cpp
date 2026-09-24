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
// MemoryMap.cpp: Implementierung der Klasse MemoryMap.
//
//////////////////////////////////////////////////////////////////////

#include "MemoryMap.h"

#include <MemoryTracker.h>
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

#ifndef NULL
#define NULL 0
#endif

MemoryMap::MemoryMap()
{
    freeblocks = NULL;
}

MemoryMap::~MemoryMap()
{
    freeAll();
}

void MemoryMap::initialize(unsigned int num_elems)
{
    freeAll();
    freeblocks               = new BLOCK;
    freeblocks->start        = 0;
    freeblocks->num_elements = num_elems;
    freeblocks->next         = NULL;
}

void MemoryMap::freeAll()
{
    BLOCK* free     = freeblocks;
    BLOCK* lastfree = NULL;
    while (free) {
        lastfree = free;
        free     = free->next;
        delete lastfree;
    }
    freeblocks = NULL;
}

unsigned int MemoryMap::getIndex(const unsigned int num_elems)
{
    BLOCK* free     = freeblocks;
    BLOCK* lastfree = NULL;
    // Finde block mit genügend Platz
    while (free && free->num_elements < num_elems) {
        lastfree = free;
        free     = free->next;
    }
    // kein Block gefunden
    if (!free)
        return -1;

    // der neue Block wird vom Anfang des freien Stücks weggenommen
    unsigned int idx = free->start;
    free->num_elements -= num_elems;
    free->start += num_elems;

    // freier Block verbraucht?
    if (free->num_elements == 0) {
        if (lastfree)
            lastfree->next = free->next;
        else
            freeblocks = free->next;
        // entferne freien Block
        delete free;
    }
    return idx;
}

void MemoryMap::freeIndex(unsigned int index, const unsigned int num_elems)
{
    BLOCK* free     = freeblocks;
    BLOCK* lastfree = NULL;
    // der erste freie Block, dessen Start grösser ist als index
    // und lastfree begrenzen den belegten Bereich, wo das freizugebende Element liegt
    while (free && free->start < index) {
        lastfree = free;
        free     = free->next;
    }
    bool createnew = true;
    if (free && free->start == (index + num_elems))  // freizugebender Block schliesst sich an nächsten freien block an
    {
        free->start = index;
        free->num_elements += num_elems;
        createnew = false;
    }
    if (lastfree)  // index liegt zwischen zwei freien Blöcken, nicht am start
    {
        if (lastfree->start + lastfree->num_elements ==
            index)  // freizugebender Block beginnt am Ende des letzten freien Blocks
        {
            if (createnew)  // wurde Block nicht mit nächstem verbunden?
            {
                // verbinde in mit letztem
                lastfree->num_elements += num_elems;
                return;
            } else {
                // verbinde diesen und nächsten Block
                lastfree->next = free->next;
                lastfree->num_elements += free->num_elements;
                delete free;
                return;
            }
        }
    }
    if (createnew) {
        BLOCK* newblock        = new BLOCK;
        newblock->start        = index;
        newblock->num_elements = num_elems;
        newblock->next         = free;
        if (lastfree)
            lastfree->next = newblock;
        else
            freeblocks = newblock;
    }
}

unsigned int MemoryMap::getFreeIndexCount() const
{
    unsigned int sum  = 0;
    BLOCK*       free = freeblocks;
    while (free) {
        sum += free->num_elements;
        free = free->next;
    }
    return sum;
}
