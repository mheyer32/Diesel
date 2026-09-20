#pragma once

#include <file/File.h>
#include <file/Path.h>

#include "Entity.h"

class PTSLoader
{
public:
    PTSLoader(void);
    ~PTSLoader(void);

    bool LoadPTS(const CPath& filename, CEntity::ENTITYLIST& entities);

protected:
    CFile* file;
};
