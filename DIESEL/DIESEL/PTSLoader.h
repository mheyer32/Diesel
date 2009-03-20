#pragma once

#include <file/file.h>
#include <file/path.h>

#include "entity.h"

class PTSLoader
{
public:
	PTSLoader(void);
	~PTSLoader(void);

	bool LoadPTS(const CPath &filename, CEntity::ENTITYLIST &entities);

protected:
	CFile	*file;
};
