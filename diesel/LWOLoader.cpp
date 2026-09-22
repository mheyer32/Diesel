#include "lwoloader.h"
#include <cassert>
#include <iostream>

#include <file/FileManager.h>
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "VertexBuffer.h"

#define CHKNAME(a, b, c, d) ((a << 24) | (b << 16) | (c << 8) | (d))

#define FORM CHKNAME('F', 'O', 'R', 'M')
#define LWO2 CHKNAME('L', 'W', 'O', '2')
#define LAYR CHKNAME('L', 'A', 'Y', 'R')
#define PNTS CHKNAME('P', 'N', 'T', 'S')
#define POLS CHKNAME('P', 'O', 'L', 'S')
#define FACE CHKNAME('F', 'A', 'C', 'E')
#define BONE CHKNAME('B', 'O', 'N', 'E')
#define TAGS CHKNAME('T', 'A', 'G', 'S')
#define PTAG CHKNAME('P', 'T', 'A', 'G')
#define SURF CHKNAME('S', 'U', 'R', 'F')
#define BBOX CHKNAME('B', 'B', 'O', 'X')
#define PART CHKNAME('P', 'A', 'R', 'T')
#define VMAP CHKNAME('V', 'M', 'A', 'P')
#define VMAD CHKNAME('V', 'M', 'A', 'D')
#define COLR CHKNAME('C', 'O', 'L', 'R')
#define TXUV CHKNAME('T', 'X', 'U', 'V')

using namespace std;

LWOLoader::LWOLoader(void)
{
    file = NULL;
}

LWOLoader::~LWOLoader(void)
{
    delete file;
}

bool LWOLoader::LoadLWO(const CPath& Filename, CEntity::ENTITYLIST& EntityList)
{
    file = NULL;

    if (!(file = CFileManager::Instance()->open(Filename))) {
        cout << "LWOLoader::LoadLWO could not open file '" << Filename << '\'' << endl;
        return false;
    }

    CPath directory;
    CFileManager::Instance()->getCurrentDirectory(directory);
    CFileManager::Instance()->setCurrentDirectory(CPath(file->getFilename()).getPath());

    readChunk();
    enterChunk();
    if (current_chunk.id != FORM) {
        cout << "LWOLoader::LoadLWO() not a IFF file, expected FORM chunk" << endl;
        return NULL;
    }
    int type = readInt();
    if (type != LWO2) {
        cout << "LWOLoader::LoadLWO() not a LWO2 file" << endl;
        return false;
    }

    readChunk();
    if (!seekChunk(PNTS)) {
        cout << "LWOLoader::LoadLWO() could not find PNTS chunk" << endl;
        return false;
    }

    CVertexBuffer* vbuffer = new CVertexBuffer();
    vbuffer->setShader(CShader::DefaultShader);

    readPoints(vbuffer);

    if (!seekChunk(POLS)) {
        cout << "LWOLoader::LoadLWO() could not find POLS chunk" << endl;
        return false;
    }
    type = readInt();
    if (type == FACE)
        readFaces(vbuffer);

    pushChunk();
    if (!seekChunk(VMAD)) {
        popChunk();
        if (!seekChunk(VMAP)) {
            cout << "LWOLoader::LoadLWO() could not find VMAD nor VMAP chunk in file \4" << Filename << endl;
        } else {
            type = readInt();
            if (type == TXUV)
                readTexCoords(vbuffer);
        }
    } else {
        type = readInt();
        if (type == TXUV)
            readTexCoords(vbuffer);
        popChunk();
        skipChunk();
    }

    CMesh* mesh = new CMesh();
    mesh->setName(Filename.getString());
    mesh->addVertexBuffer(vbuffer);
    mesh->calcBoundingBox();

    CEntity* entity = new CEntity();
    entity->addMesh(mesh);
    entity->setName(mesh->getName());

    EntityList.push_back(entity);

    CFileManager::Instance()->setCurrentDirectory(directory);
    KILLOBJECT(file);

    return true;
}

void LWOLoader::readTexCoords(CVertexBuffer* vbuffer)
{
    short dim = readShort();  // skip
    assert(dim == 2);

    char temp[256];
    file->readSTRING(temp);
    if (!(strlen(temp) & 1))
        file->readBYTE();  // skip pad byte if necessary

    vbuffer->AllocArrays(MT_STATIC, TEXCOORDARRAY, vbuffer->num_vertices);

    for (int v = 0; v < vbuffer->num_vertices; ++v) {
        int vertex = readVariableIndex();
        readVariableIndex();  // skip polygon index;
        vbuffer->texcoords[vertex].x = readFloat();
        vbuffer->texcoords[vertex].y = readFloat();
    }

    skipChunk();
}
void LWOLoader::readPoints(CVertexBuffer* vbuffer)
{
    int num_vertices      = current_chunk.length / 12;
    vbuffer->num_vertices = num_vertices;
    vbuffer->AllocArrays(MT_STATIC, VERTEXARRAY, num_vertices);
    for (int v = 0; v < num_vertices; ++v) {
        vbuffer->vertices[0][v].x = -readFloat();
        vbuffer->vertices[0][v].y = readFloat();
        vbuffer->vertices[0][v].z = readFloat();
    }
    skipChunk();
}

void LWOLoader::readFaces(CVertexBuffer* vbuffer)
{
    int num_faces   = current_chunk.length / (2 * 4);
    int num_indices = num_faces * 3;
    vbuffer->AllocArrays(MT_STATIC, INDEXARRAY, 0, num_indices);
    vbuffer->num_indices = num_indices;
    for (int i = 0; i < num_indices; i += 3) {
        short num_points = readShort();
        assert(num_points == 3);
        vbuffer->indices[i + 2] = readVariableIndex();  // reverse order of points into counter-clockwise mode
        vbuffer->indices[i + 1] = readVariableIndex();
        vbuffer->indices[i]     = readVariableIndex();
    }
    skipChunk();
}

void LWOLoader::enterChunk()
{
    pushChunk();                                                       // oberen Chunk auf Stack
    current_chunk.maxseek = current_chunk.pos + current_chunk.length;  // Seek nur innerhalb dieses Chunks zulassen
}

void LWOLoader::exitChunk()
{
    popChunk();   // wieder an den Anfang des Parent-Chunks
    skipChunk();  // und gesamten Chunk überspringen
}

void LWOLoader::pushChunk()
{
    chunkstack.push(current_chunk);
}

void LWOLoader::popChunk()
{
    getTopChunk();
    chunkstack.pop();
}

void LWOLoader::getTopChunk()
{
    current_chunk = chunkstack.top();
    file->setPosition(current_chunk.pos);
}

void LWOLoader::readChunk()
{
    current_chunk.id     = readInt();
    current_chunk.length = readInt();
    current_chunk.pos    = file->getPosition();
}

bool LWOLoader::skipChunk()
{
    current_chunk.pos += current_chunk.length + (current_chunk.length & 1);
    file->setPosition(current_chunk.pos);

    if (current_chunk.pos < current_chunk.maxseek)
        readChunk();  // falls ExitSubChunk ganz am Ende des Files aufgerufen wird
    return true;
}

bool LWOLoader::seekChunk(int Name)
{
    pushChunk();
    unsigned int maxseek = current_chunk.maxseek;

    while ((current_chunk.pos < maxseek) && (current_chunk.id != Name)) {
        skipChunk();
    }
    if (current_chunk.pos >= maxseek) {
        popChunk();
        return false;
    }
    if (current_chunk.id == Name) {
        chunkstack.pop();  // nur oberstes Element löschen, nicht aber current_chunk überschreiben
        return true;
    }
    return false;
}
int LWOLoader::readInt()
{
    int t = file->readDWORD();
    return ((t & 0xFF) << 24) | ((t & 0xFF00) << 8) | ((t & 0xFF0000) >> 8) | ((t & 0xFF000000) >> 24);
}
short LWOLoader::readShort()
{
    short t = file->readWORD();
    return ((t & 0xFF00) >> 8) | ((t & 0xFF) << 8);
}
float LWOLoader::readFloat()
{
    int t = file->readDWORD();
    t     = (((t & 0xFF) << 24) | ((t & 0xFF00) << 8) | ((t & 0xFF0000) >> 8) | ((t & 0xFF000000) >> 24));
    return (float&)t;
}
int LWOLoader::readVariableIndex()
{
    int t = 0;
    t     = file->readBYTE();
    if (t != 0xFF) {
        t <<= 8;
        t |= file->readBYTE();
    } else {
        t = file->readWORD() << 8;
        t |= file->readBYTE();
    }
    return t;
}