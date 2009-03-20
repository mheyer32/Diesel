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
// Mesh.cpp: Implementierung der Klasse CMesh.
//
//////////////////////////////////////////////////////////////////////

#include "Mesh.h"
#include "Engine.h"
#include "Stripper.h"
#include "TNLStack.h"
#include <iostream>
#include <algorithm>

#include <cassert>

#include <memorytracker.h>
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CMesh::MESHLIST	CMesh::MeshList;

using namespace std;

CMesh::CMesh()
{
	MeshList.push_front(this);	
}

CMesh::~CMesh()
{	
#ifdef _DEBUG
	cout<<"Mesh: "<<getName()<<" deleted"<<endl;
#endif
	freeMeshData();
	MeshList.remove(this);	
}

void CMesh::batchFaces(int Frame)
{
	int num_vbuffers=m_vbuffers.size();

	for (int vb=0; vb<num_vbuffers; ++vb)
	{
		CVertexBuffer *vbuffer=m_vbuffers[vb];
		assert(vbuffer->num_vertices);
		g_TNLStack->batch(vbuffer);
	}
	return;
}

VECTOR3 CMesh::center(const VECTOR3 &Centre,int Frame)
{
VECTOR3 t;
	t=-getCentre(m_bbox)+Centre;
	translate(t,Frame);
	return t;
}

void CMesh::translate(const VECTOR3 &T,int Frame)
{
	int num_vbuffers=m_vbuffers.size();

	for (int vb=0; vb<num_vbuffers; ++vb)
	{
		CVertexBuffer *vbuffer=m_vbuffers[vb];
		for (int v=0;v<vbuffer->num_vertices;v++)
		{
			vbuffer->vertices[Frame][v]+=T;
		}
	}
	m_bbox.min+=T;
	m_bbox.max+=T;
}
void CMesh::transform(const MATRIX4 &m,int Frame)
{
	MATRIX4 mn;
	FullInverse4(m,mn);
	Transpose4(mn);

	int num_vbuffers=m_vbuffers.size();

	for (int vb=0; vb<num_vbuffers; ++vb)
	{
		CVertexBuffer *vbuffer=m_vbuffers[vb];
		VECTOR3 *vertices=vbuffer->vertices[Frame];
		for (int v=0;v<vbuffer->num_vertices;++v)
		{
			vertices[v]=m*vertices[v];
		}
		if (vbuffer->used_arrays&NORMALARRAY)
		{
			VECTOR3 *normals=vbuffer->normals[Frame];
			for (int v=0;v<vbuffer->num_vertices;++v)
			{
				normals[v]=mn*normals[v];
			}
		}
	}
	m_bbox=m*m_bbox;
}
// FIXME: support for more than one box is missing
const BBOX& CMesh::getBoundingBox(int Frame) const
{
	return m_bbox;
}

CMesh* CMesh::FindMesh(const std::string &Name)
{
MESHITERATOR m=MeshList.begin();

	for (; m!=MeshList.end(); ++m)
	{
		if ((*m)->m_name==Name) return *m;
	}
	return NULL;
}

CMesh& CMesh::operator += (const CMesh &M)
{
	int num_vbuffers=M.m_vbuffers.size();

	for (int vb=0; vb<num_vbuffers; ++vb)
	{	
		m_vbuffers.push_back(M.m_vbuffers[vb]);
	}
	m_name+="+"+M.m_name;
	return *this;
}
 
void CMesh::freeMeshData()
{
	m_vbuffers.clear();
}


void CMesh::optimize()
{

	int num_vbuffers=m_vbuffers.size();

	for (int vb=0; vb<num_vbuffers; ++vb)
	{
		m_vbuffers[vb]->OptimizeVBuffer();
	}	

	sortVertexBuffers();
}


const CMesh::MESHLIST& CMesh::GetMeshList()
{
	return MeshList;
}


void CMesh::addVertexBuffer(CVertexBuffer *VBuffer)
{
	assert(VBuffer);
	assert(VBuffer->num_vertices);
	assert(VBuffer->getShader());
	m_vbuffers.push_back(VBuffer);
}

const CVertexBuffer::SMARTPTRARRAY& CMesh::getVBufferList() const
{
	return m_vbuffers;
}

void CMesh::mergeVertexBuffers()
{
	sortVertexBuffers();

	CVertexBuffer::SMARTPTRARRAY temparray;

	int num_vbuffers=m_vbuffers.size();

	for (int vb=0; vb<num_vbuffers;)
	{
		CVertexBuffer *vbuffer=m_vbuffers[vb];
		CShader *shader=vbuffer->getShader();
		CTexture *lightmap=vbuffer->getLightmap();
		int	num_vsets=vbuffer->getNumVertexSets();

		int	num_vertices=0;
		int num_indices=0;
		int vb2=vb;

		for (; vb2!=num_vbuffers; ++vb2)
		{
			CVertexBuffer *vbuffer2=m_vbuffers[vb2];

			if ((  shader==vbuffer2->getShader())
				&&(lightmap==vbuffer2->getLightmap())
				&&(num_vsets==vbuffer2->getNumVertexSets())
				)
			{
				num_vertices+=vbuffer2->num_vertices;
				num_indices+=vbuffer2->num_indices;
			}
			else
			{
				break;
			}
		}

		if (vb2>vb+1)
		{
			// buffers can be combined

			CVertexBuffer *newbuffer=new CVertexBuffer; //FIXME: VBuffers werden wohl kaum zur Laufzeit gemerged
			// also VB-Erweiterung in CVertexbuffer stecken und nach
			// gemeinsamen vertices suchen lassen!?
			newbuffer->setShader(shader);
			newbuffer->setLightmap(lightmap);
			int arrays=vbuffer->used_arrays;
			newbuffer->AllocArrays(arrays,num_vertices,num_indices,num_vsets);

			for (int vb3=vb; vb3<vb2; ++vb3)
			{
				newbuffer->combine(*m_vbuffers[vb3]);
			}

			temparray.push_back(newbuffer);				
		}
		else
		{
			temparray.push_back(vbuffer);
		}
		vb=vb2;
	}

	m_vbuffers.swap(temparray);
}

void CMesh::calcBoundingBox(int Frame) 
{
	m_bbox.min=VECTOR3(99999,99999,99999);
	m_bbox.max=VECTOR3(-99999,-99999,-99999);

	int num_vbuffers=m_vbuffers.size();

	for (int vb=0; vb<num_vbuffers; ++vb)
	{
		CVertexBuffer *vbuffer=m_vbuffers[vb];
		for (int v=0; v<vbuffer->num_vertices; ++v)
		{
			m_bbox|=vbuffer->vertices[Frame][v];
		}
	}
}

bool _cmp_vbuffer(const CVertexBuffer::SMARTPTR &vb1, const CVertexBuffer::SMARTPTR &vb2)
{
		return vb1->getSortkey()<vb2->getSortkey();
}

void CMesh::sortVertexBuffers()
{
	std::sort(m_vbuffers.begin(), m_vbuffers.end(), _cmp_vbuffer);
}

void CMesh::setBoundingBox(const BBOX &newbox, int Frame)
{
	m_bbox=newbox;
}


