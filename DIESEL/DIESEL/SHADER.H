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
// Shader.h: Schnittstelle für die Klasse CShader.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHADER_H__380B9F95_8B56_4B1B_8E36_6633DD189314__INCLUDED_)
#define AFX_SHADER_H__380B9F95_8B56_4B1B_8E36_6633DD189314__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <list> 
#include <string>

#include <adt/Refcount.h>
#include <adt/NamedObject.h>
#include <adt/smartpointer.h>

#include <adt/ConVar.h>
#include <opengl/opengl.h>
#include "glState.h"
#include "ShaderPass.h"

#define SHADER_MAX_NUM_PASSES			8
#define SHADER_MAX_NUM_DEFORMVERTEX		5

/* Shader flags */
#define	SHADER_NOCULL         GLSTATE_NOCULL
#define SHADER_CULLFRONT      GLSTATE_CULLFRONT
#define SHADER_POLYGONOFFSET  GLSTATE_POLYGONOFFSET
#define SHADER_FOG			  GLSTATE_FOG
#define SHADER_GLSTATE_MASK (SHADER_NOCULL|SHADER_CULLFRONT|SHADER_POLYGONOFFSET|SHADER_FOG)

#define	SHADER_SKY            (1<< 16)
#define	SHADER_NOMIPMAP	      (1<< 17)
#define SHADER_NOPICMIP		  (1<< 18)
#define SHADER_USENORMALS     (1<< 20) // used to see, if normals need to be sent to ogl
#define SHADER_PORTAL		  (1<< 21)
#define SHADER_NEEDSLIGHTING  ((1<< 22)|SHADER_USENORMALS)  // one of the passes uses lighting
#define SHADER_TAGGED		  (1<< 31) // this is a one-bit Information, not used by the shaders itself

/* default shader sort keys*/
#define SORT_ONE		(1<<28)
#define SORT_PORTAL		(1<<28)		// place the key into the 4 msb
#define SORT_SKY		(7<<28)
#define SORT_OPAQUE		(3<<28)
#define SORT_BANNER		(6<<28)
#define SORT_UNDERWATER (8<<28)
#define SORT_ADDITIVE	(9<<28)
#define SORT_NEAREST	(15<<28)

#define SORT_DONTBATCH	0x80


/* Vertex Deformation Funcs */
enum VERTEXDEFORM
{
	VDEF_NONE,
	VDEF_WAVE,
	VDEF_NORMAL,
	VDEF_MOVE,
	VDEF_BULGE,
	VDEF_AUTOSPRITE,
	VDEF_AUTOSPRITE2
};

//////////////////////////////////////////////////////////////////////////////////////////////

class CShader : public CRefCount, public CNamedObject
{

friend class Renderer;

public:

typedef SmartPointer<CShader>	SMARTPTR;
typedef std::list<SMARTPTR>		SMARTPTRLIST;
typedef SMARTPTRLIST::iterator	SMARTPTRITERATOR;

typedef std::list<CShader*>		SHADERLIST;
typedef SHADERLIST::iterator	SHADERITERATOR;

	CShader();
	virtual ~CShader();

	/** returns the id this shader has - not much of interest since you cannot make a reverse mapping from
		id to a shader-object */
	inline int getShaderId() const {return shaderId;}

	/** custom setName() in order to force lower-case names 
		FIXME: add this feature to CNamedObject directly?*/
	virtual void setName(const std::string &newname); // should overwrite CNamedObject::setName()

	/** add a pass to this shader */
	void addPass(const CShaderPass &newPass);

	void addVertexDeformWave(float div, const CWave &wave);
	void addVertexDeformBulge(float width, float height, float speed);
	void addVertexDeformMove(float x, float y, float z,  CWave &wave);
	void addVertexDeformAutosprite();
	void addVertexDeformAutosprite2();
	void addVertexDeformNormal(float amplitude, float freq);
	int getNumVertexDeform() const {return num_deformv;};
	VERTEXDEFORM getVertexDeform(int vdef_num);

	void setFogParams(float r, float g, float b, float fogdistance);
	void setSkyParams(CTexture **nearboxtextures, CTexture **farboxtextures, float cloudheight);

	inline int getFlags() const {return shaderflags;};
	void setFlags(int mask, int flags);

	inline int getSort() const  {return sort;};
	void setSort(int newsort) {sort=newsort;};

	inline int getUsedArrays() const {return usedarrays;};
	void setUsedArrays(int new_usedarrays) {usedarrays=new_usedarrays;};

	inline int getNumPasses() const {return num_passes;};
	inline CShaderPass& getPass(int passnum) {return passes[passnum];};

	/** go through all passes, rethink flags */
	void validate();

	static CShader* findShader(const std::string &Shadername);
	static CShader* findOrLoadShader(const std::string &Shadername, bool genLightmapStage=false, bool genGenericShader=true);

	/** called once when the renderer is initialized */
	static void createDefaultShader();

	/** generate a generic shader either from a given file or texture */
	static CShader* genShaderFromTexture(const std::string &texfile,bool genLightmapStage=false);
	static CShader* genShaderFromTexture(CTexture *texture,bool genLightmapStage=false);
	static SHADERLIST getShaderList();

	// pointer to default-shader, will be initialized at the start of the renderer
	static	SMARTPTR DefaultShader;
	
protected:

	void validatePass(int num_Pass);

	int			num_passes;						// number of shader stages
    CShaderPass	passes[SHADER_MAX_NUM_PASSES];	// the passes themselves

	int				usedarrays;		// which arrays have to be supplied to use this shader correctly
	int				shaderflags;	// global shader settings	
	unsigned int	sort;		// coarse sortkey for this shader (0-15)

	// vertexdeformation related
	int			num_deformv;	//TODO vertexdeform-objects could make this more flexible
	VERTEXDEFORM deformv[SHADER_MAX_NUM_DEFORMVERTEX];
	CWave		deformv_wave[SHADER_MAX_NUM_DEFORMVERTEX];
	float		deformv_param[SHADER_MAX_NUM_DEFORMVERTEX][3]; 

	// for fogshaders
	float		fogcolor[3];
	float		fogdistance;

	// for skyshaders
	CTexture::SMARTPTR nearbox[6];	// for skyshaders 
	CTexture::SMARTPTR farbox[6];
	float		cloudheight;

	int shaderId;

	static int max_shaderId;
	static SHADERLIST ShaderList;

	static ConCmd shaderlist;
	static void cmd_shaderlist(ConCmd &command, const std::string &args);
};

#endif // !defined(AFX_SHADER_H__380B9F95_8B56_4B1B_8E36_6633DD189314__INCLUDED_)
