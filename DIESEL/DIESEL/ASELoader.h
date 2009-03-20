#pragma once

#include <vector>

#include <file/file.h>
#include <file/path.h>
#include <misc/Lexer.h>

#include "Shader.h"
#include "Entity.h"

class ASELoader
{
public:
	ASELoader(void);
	~ASELoader(void);

    bool LoadASE(const CPath &Filename, CEntity::ENTITYLIST &EntityList);

protected:

	void readMaterialList();
	CShader* readMaterial();
	CEntity* readGeomObject();
	CMesh* readMesh();

	void readVertices(VECTOR3 *verts, int num_vertices);
	void readFaces(INDEX *indices, int num_indices);
	void readTVerts(VECTOR2 *tverts, int num_tverts);
	void readTFaces(INDEX *indices, int num_tfaces);

	CVertexBuffer* buildVBuffer(VECTOR3 *verts, VECTOR2* tverts, INDEX *faces, INDEX* tfaces, int num_faces, int num_verts);


	Lexer m_lexer;

	std::vector<CShader::SMARTPTR> materials;
};
