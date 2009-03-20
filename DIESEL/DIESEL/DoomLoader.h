#pragma once

#include <iostream>

#include <file/path.h>
#include <misc/Lexer.h>

#include "DoomMap.h"
#include "DoomScene.h"
#include "Mesh.h"


class DoomLoader
{
public:
	DoomLoader(void);
	~DoomLoader(void);

	DoomScene* LoadPROC(const CPath &filename);

protected:

	void LoadMAP(const CPath &filename, DoomMap *map);

	CMesh* readModel();
	CVertexBuffer* readSurface();
	void readInterAreaPortals(DoomMap *doomap);
	void readNodes(DoomMap *dmap);

	void  readVec(float *coordinates, int dim);
	void convertVec(VECTOR3 &vec);

	Lexer lexer;

	CMesh::SMARTPTRLIST	models;
};
