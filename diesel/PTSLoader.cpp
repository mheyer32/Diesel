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
#include "ptsloader.h"

#include <fstream>
#include "Shader.h"

PTSLoader::PTSLoader(void)
{
}

PTSLoader::~PTSLoader(void)
{
}

bool PTSLoader::LoadPTS(const CPath& Filename, CEntity::ENTITYLIST& entities)
{
    std::ifstream file;

    CPath filename(Filename);

    file.open(filename.getString().c_str());

    int num_vertices;

    file >> num_vertices;

    CVertexBuffer* vbuffer = new CVertexBuffer(MT_STATIC, VERTEXARRAY, num_vertices);

    for (int v = 0; v < num_vertices; ++v) {
        file >> vbuffer->vertices[0][v].x >> vbuffer->vertices[0][v].y >> vbuffer->vertices[0][v].z;
    }
    file.close();
    filename.setExtension("tri");
    file.open(filename.getString().c_str());
    int num_indices;

    file >> num_indices;

    vbuffer->AllocArrays(INDEXARRAY, 0, num_indices);

    for (int i = 0; i < num_indices; ++i) {
        file >> vbuffer->indices[i];
    }
    file.close();

    vbuffer->num_vertices = num_vertices;
    vbuffer->num_indices  = num_indices;

    vbuffer->createNormals();

    CTexture* material = new CTexture();

    material->setName("BoingAussenhaut");

    material->Diffuse[0] = 0.8;
    material->Diffuse[1] = 0.85;
    material->Diffuse[2] = 0.99;
    material->setTexParameter(TEX_USEMATERIAL, TEX_USEMATERIAL);

    CShader* shader = CShader::genShaderFromTexture(material);
    shader->getPass(0).setRGBGen(RGBGEN_MATERIAL);

    vbuffer->setShader(shader);

    CMesh* mesh = new CMesh;
    mesh->setName("boing_mesh");
    mesh->addVertexBuffer(vbuffer);

    CEntity* entity = new CEntity;
    entity->setName("Boing");

    entity->setMesh(mesh);

    entities.push_back(entity);

    return true;
}