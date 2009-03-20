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
// Renderer.h: Schnittstelle für die Klasse Renderer.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RENDERER_H__6113ED7A_869D_406A_9507_F5D9F2EBAAAA__INCLUDED_)
#define AFX_RENDERER_H__6113ED7A_869D_406A_9507_F5D9F2EBAAAA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_TEXTURE_UNITS 8 // right enough for RADEON9700
#define NUM_TCARRAYS (MAX_TEXTURE_UNITS)
/* for the arrays that temporarily hold modified normals/texcoords/vertices */
#define MAX_RENDERER_VERTICES 4096
#define MAX_RENDERER_INDICES  65536


#include "Features.h"
#include "engine.h"
#include "enginetypes.h"
#include "vertexbuffer.h"
#include "shader.h"
#include <adt/singleton.h>

class CVertexBuffer;
class CTexture;

class Renderer : public Singleton<Renderer>
{
DECLARE_SINGLETON(Renderer)

public:

	void Render(const RENDERINFO &Info);
	void initialize();

	inline int getGLState(){return glstate;};
	void getIdentityLighting(float il[4]);

	static ConVar r_detailtextures;
	static ConVar r_ext_texture_env_add;
	static ConVar r_ext_multitexture;
	static ConVar r_finish;
	static ConVar r_primitives;
	static ConVar r_shownormals;
	static ConVar r_showtris;
	static ConVar r_showsky;
	static ConVar r_lightmap;
	static ConVar r_maxPasses;
	static ConVar r_fixfirstunit;
	static ConVar r_maxUsedTexunit;

protected:


struct TUSTATE
{
	unsigned int flags;
	GLenum		 texenv;
	GLenum		 blendsrc,blenddest;
	CTexture	 *texture;
	TUSTATE():flags(0),texenv(0),blendsrc(0),blenddest(0),texture(NULL){};
};

struct GLARRAYS
{
	VECTOR3			*vertices;
	VECTOR3			*normals;
	VECTOR2			*texcoords;
	COLOR			*colors;
	INDEX			*indices;
	int				num_indices;
	int				num_vertices;
};

/* Setuppass return values */
enum PASS_RVAL
{
	RVAL_OK			=0,
	RVAL_FIRSTUNIT	=1,
	RVAL_SKIP		=2
};

typedef void (Renderer::*FLUSHFUNC)(void);
typedef void (Renderer::*RENDERFUNC)(int,int);
typedef PASS_RVAL (Renderer::SETUPPASSFUNC)(CShaderPass &pass);


	Renderer();
	virtual ~Renderer();

	void SetupShader();
	void DeformVerts();
	void SetupStaticArrays();
	void SetupDynamicArrays();

	void Render_Generic(int firstpass, int num_passes);
	void Render_MTex(int firstpass, int num_passes);
	void Render_LightmapOnly();
	void Render_FogPass(const FOGFEATURE &fogfeature);
	void Render_DLight2(const DLIGHTFEATURE &dlight);
	void Render_Sky();

	void clipSkyPolygon(VECTOR3 *verts, int num_verts, int planenumber);
	void determineMinMax(VECTOR3 *verts, int num_verts);

	PASS_RVAL SetupPass(CShaderPass &pass);
	PASS_RVAL SetupPassTexture(CShaderPass &pass);
	PASS_RVAL SetupBlendingGenericMtex(CShaderPass &pass);
	PASS_RVAL SetupBlendingCombine4(CShaderPass &pass);
	PASS_RVAL SetupColors(CShaderPass &pass);
	inline void setTexture(CTexture *texture);

	void RGBGEN_OneMinusVertex();
	void RGBGEN_Wave(const CWave &wave);
	void RGBGEN_Flare();
	void ALPHAGEN_OneMinusVertex();
	void ALPHAGEN_Portal(float range);
	void ALPHAGEN_Wave(const CWave &wave);
	void ALPHAGEN_LightingSpecular();
	void ALPHAGEN_Vertex();

	void TCGEN_Fog(PLANE fogplane,float fogdistance);
	
	void Render_DrawRangeElements();
	void Render_DrawElements();
	void Render_ArrayElements();
	void Render_DrawArrays();

	void ShowTris();
	void ShowNormals();

	void prepare_MTex();
	void disableMTex();
	static void mtexCB(ConVar &var);

	void prepareFog();
	void prepareSky();
	void prepareDLight();
	void createWhitemap();

	void getModelview(MATRIX4 &m);
	void restoreSourcePointers();

	// texture unit state related
	int	texunit; // aktuell genutze Texture-Einheit
	TUSTATE tustate[MAX_TEXTURE_UNITS]; 

	// gl state related
	COpenGL *ogl;
	int glstate;

	FLUSHFUNC		flushfunc;	// 

	CShaderPass *curr_pass;
	CShaderPass *lastpass;

	int	previousflags;	// save passflags of previously rendered passed

	RENDERINFO info;	// current renderinfo

	float curr_color[4];			// if no per-vertex colors are used in the current pass, use this as color
	float identitylighting[4];		// white, adopted to r_overBrightBits

	// whitemap texture
	CTexture::SMARTPTR whitemap;

	//fog related stuff
	CShaderPass	fogpass;
	CTexture::SMARTPTR fogtexture;

	// dlight related stuff
	CShaderPass dlightpass;
	CTexture::SMARTPTR dlighttexture;

	// temporarily store new data here
	VECTOR3			*Vertices;
	VECTOR2			*TexCoords[NUM_TCARRAYS];
	VECTOR3			*Normals;
	COLOR			*Colors;

	GLARRAYS		src_arrays;		// here are the pointers to the data used in this pass
	GLARRAYS		org_arrays;		// pointer to the original arrays

	CVertexBuffer	*sourcebuffer;

	// everything sky related
	VECTOR3			*skyverts;
	VECTOR2			*skytexcoords;
	VECTOR2			*sphereskytexcoords;
	INDEX			*skyindices;
	float			skyminmax[6][4];
	bool			usedskysides[6];
	CShaderPass		farboxpass;
	CShaderPass		nearboxpass;

	const static float turbsin[];
};

#endif // !defined(AFX_RENDERER_H__6113ED7A_869D_406A_9507_F5D9F2EBAAAA__INCLUDED_)
