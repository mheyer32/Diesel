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

#include <file/File.h>
#include <file/Path.h>
#include <stack>
#include "Entity.h"

class LWOLoader
{
public:
    LWOLoader(void);
    ~LWOLoader(void);

    bool LoadLWO(const CPath& Filename, CEntity::ENTITYLIST& EntityList);

protected:
    struct CHUNK
    {
        union
        {
            int  id;
            char name[4];
        };
        unsigned int length;

        unsigned int pos;
        unsigned int maxseek;
    };

    CEntity* readLayer();
    void     readPoints(CVertexBuffer* vbuffer);
    void     readFaces(CVertexBuffer* vbuffer);
    void     readTexCoords(CVertexBuffer* vbuffer);

    void enterChunk();
    void exitChunk();

    void pushChunk();
    void popChunk();
    void getTopChunk();

    void readChunk();
    bool skipChunk();
    bool seekChunk(int Name);

    int   readInt();
    short readShort();
    float readFloat();
    int   readVariableIndex();

    CFile*            file;
    CHUNK             current_chunk;
    std::stack<CHUNK> chunkstack;
};
