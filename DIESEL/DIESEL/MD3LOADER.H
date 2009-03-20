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
// MD3Loader.h: Schnittstelle für die Klasse MD3Loader.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MD3LOADER_H__34BB8F68_7A5B_4573_9971_B287EE653935__INCLUDED_)
#define AFX_MD3LOADER_H__34BB8F68_7A5B_4573_9971_B287EE653935__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <math/vector3.h>
#include <math/vector2.h>
#include <math/matrix3.h>
#include <file/path.h>

#include <string>

class CVertexBuffer;
class CEntity;
class CMesh;
class CShader;
class CFile;

class MD3Loader  
{
public:
	MD3Loader();
	virtual ~MD3Loader();

	CEntity*	loadEntity(const CPath &Filename);
	CMesh*		loadMesh(const CPath &Filename);
	CEntity*	loadSimpleEntity(const CPath &Filename);
	CEntity*    loadSimpleLODEntity(const CPath &Filename);

protected:


struct MD3HEADER
{
	char ID[4];         //id of file, always "IDP3" 
	int  Version;       //i suspect this is a version //number, always 15
	char FileName[68];  //sometimes left Blank...65 chars, 32bit aligned == 68 chars 
	int  BoneFrame_num; //number of BoneFrames
	int  Tag_num;       //number of 'tags' per BoneFrame
	int  Mesh_num;      //number of meshes/skins
	int  MaxSkin_num;	//maximum number of unique skins used in md3 file 
	int  HeaderLength;  //always equal to the length of this header
	int  Tag_Start;     //starting position of tag-structures
	int  Tag_End;       //ending position of  tag-structures, starting position of mesh-structures 
	int  FileSize;      //size of file     
};

struct MD3TAG
{
	char Name[64];    //name of 'tag' as it's usually called in the md3 files try to 
					//see it as a sub-mesh/seperate   mesh-part. 
                  //sometimes this 64 string may  contain some garbage, but  
					//i've been told this is because
                   //some tools leave garbage in those strings, but they ARE 
					//strings...
	VECTOR3 Position;   //relative position of tag 
	MATRIX3 Rotation;   //the direction the tag is facing relative to the rest of the model
};

struct MD3BONEFRAME
{
	float  Mins[3];
	float  Maxs[3];
	float  Position[3];
	float  scale;
	char   Creator[16];  
};

struct MD3MESH
{
	char ID[4];          //id, must be IDP3 
	char Name[68];       //name of mesh  65 chars, 32 bit aligned == 68 chars
	int  MeshFrame_num;  //number of meshframes in mesh 
	int  Skin_num;       //number of skins in mesh 
	int  Vertex_num;     //number of vertices 
	int  Triangle_num;   //number of Triangles 
	int  Triangle_Start; //starting position of Triangle data, relative to start of Mesh_Header
	int  HeaderSize;     //size of header 
	int  TexVec_Start;   //starting position of texvector data, relative  to start of Mesh_Header 
	int  Vertex_Start;   //starting position of vertex data,relative to start of Mesh_Header 
	int  MeshSize;       //size of mesh 
};


	CVertexBuffer*	loadMD3Mesh();
	CShader* findOrLoadShader(char *shadername);
	CShader* loadSkin(char *meshname);

	MD3HEADER header;
	MD3MESH   meshheader;

	CFile	*file;

	std::string filename;
	//char	filename[MAX_PATH];

};

#endif // !defined(AFX_MD3LOADER_H__34BB8F68_7A5B_4573_9971_B287EE653935__INCLUDED_)
