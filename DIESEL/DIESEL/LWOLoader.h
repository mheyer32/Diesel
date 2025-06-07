#pragma once

#include <file/file.h>
#include <file/path.h>
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
