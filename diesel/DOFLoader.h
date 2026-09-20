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
// DOFLoader.h: Schnittstelle für die Klasse DOFLoader.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DOFLOADER_H__13CA2B03_1684_42D8_BE11_7165441902C2__INCLUDED_)
#define AFX_DOFLOADER_H__13CA2B03_1684_42D8_BE11_7165441902C2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER > 1000

#include <file/File.h>
#include <file/Path.h>
#include <list>
#include <string>
#include <vector>
#include "Entity.h"
#include "Shader.h"

class CMesh;

class DOFLoader
{
public:
    DOFLoader();
    virtual ~DOFLoader();

    bool LoadDOF(const CPath& filename, CEntity::ENTITYLIST& entities);

protected:
    struct CHUNK
    {
        union
        {
            int  name;
            char cname[4];
        };
        int length;
        int pos;
        int maxseek;
    };

    void readChunk();
    void pushChunk();
    void popChunk();
    void topChunk();
    void skipChunk();
    bool seekChunk(int chunkname);
    void enterSubChunk();
    void exitSubChunk();

    std::string readString();
    CShader*    readMaterial();
    void        readGeometryObject(CMesh* geob);
    void        read4Floats(float* dest);

    CFile* file;

    CHUNK            chunk;
    std::list<CHUNK> chunkstack;

    std::vector<CShader::SMARTPTR> materials;
};

#endif  // !defined(AFX_DOFLOADER_H__13CA2B03_1684_42D8_BE11_7165441902C2__INCLUDED_)
