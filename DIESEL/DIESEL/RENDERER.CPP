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
// Renderer.cpp: Implementierung der Klasse Renderer.
//
//////////////////////////////////////////////////////////////////////

#include "Renderer.h"
#include "Camera.h"
#include <opengl/opengl.h>
#include "Texture.h"
#include "specialtextures.h"
#include "geometrymemmanager.h"
#include "light.h"
#include "skygen.h"
#include <math/mathlib.h>

#include <iostream>
#include <cassert>

//#include <memorytracker.h>

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

ConVar Renderer::r_detailtextures("r_detailtextures","1");
ConVar Renderer::r_ext_texture_env_add("r_ext_texture_env_add","1");
ConVar Renderer::r_ext_multitexture("r_ext_multitexture","1",0,Renderer::mtexCB);
ConVar Renderer::r_finish("r_finish","0");
ConVar Renderer::r_primitives("r_primitives","0");
ConVar Renderer::r_shownormals("r_shownormals","0");
ConVar Renderer::r_showtris("r_showtris","0");
ConVar Renderer::r_showsky("r_showsky","1");
ConVar Renderer::r_lightmap("r_lightmap","0");
ConVar Renderer::r_maxPasses("r_maxPasses","10");
ConVar Renderer::r_fixfirstunit("r_fixfirstunit","0");
ConVar Renderer::r_maxUsedTexunit("r_maxUsedTexunit","0");

Renderer::Renderer()
{
	r_maxPasses=SHADER_MAX_NUM_PASSES;

	skyverts=NULL;
	skytexcoords=NULL;
	sphereskytexcoords=NULL;
	skyindices=NULL;

	Vertices=NULL;
	Normals=NULL;
	Colors=NULL;
	for (int t=0;t<NUM_TCARRAYS;++t)
	{
		TexCoords[t]=NULL;
	}	
}

Renderer::~Renderer()
{
	GeometryMemManager *mmgr=GeometryMemManager::Instance();
	if (skyverts) mmgr->free(skyverts);
	if (skytexcoords) mmgr->free(skytexcoords);
	if (sphereskytexcoords) mmgr->free(sphereskytexcoords);
	if (skyindices) mmgr->free(skyindices);

	if (Vertices) mmgr->free(Vertices);
	if (Normals) mmgr->free(Normals);
	if (Colors) mmgr->free(Colors);

	for (int t=0;t<NUM_TCARRAYS;++t)
	{
		if (TexCoords[t]) mmgr->free(TexCoords[t]);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Setup & Render 
/////////////////////////////////////////////////////////////////////////////////////////////////////////

void Renderer::Render(const RENDERINFO &Info)
{
	info=Info;
	sourcebuffer=info.vbuffer;
	int arrays=info.shader->usedarrays&sourcebuffer->used_arrays;

	org_arrays.vertices=arrays&VERTEXARRAY ? sourcebuffer->vertices[sourcebuffer->getActiveSet()] : NULL;
	org_arrays.normals=arrays&NORMALARRAY ? sourcebuffer->normals[sourcebuffer->getActiveSet()] : NULL;
	org_arrays.indices=arrays&INDEXARRAY ? sourcebuffer->indices : NULL;
	org_arrays.texcoords=arrays&TEXCOORDARRAY ? sourcebuffer->texcoords : NULL;
	org_arrays.colors=arrays&COLORARRAY ? sourcebuffer->colors : NULL;
	org_arrays.num_vertices=sourcebuffer->num_vertices;
	org_arrays.num_indices=sourcebuffer->num_indices;

	src_arrays=org_arrays;

	glPushMatrix();

	// after each rendered pass remember the flags it used
	// done here, so fogged and skyshaded polygons get drawn correctly
	previousflags=0;

	// sky-shaders need to be prepared completely differently
	if (info.shader->shaderflags&SHADER_SKY)
	{
		if ((int)r_showsky)
		{
			Render_Sky(); 
		}
		goto SKIP_RENDERING;
	}

	// decide what gl-function to use for rendering the primitives
	if (src_arrays.indices)
	{
		switch ((int)r_primitives)
		{
		case 1:
			if (sourcebuffer->mode==GL_TRIANGLES)
			{
				flushfunc=&Renderer::Render_ArrayElements;
				break;
			} // else and break intentionally left away, in case that r_primitives is set to 1, only
			  // triangles can be rendered. if not triangles are to be rendered, switch back to drawelements
		case 0:
			flushfunc=&Renderer::Render_DrawElements;
			break;
		case 2:
			flushfunc=&Renderer::Render_DrawRangeElements;
			break;
		}
	}
	else
	{
		flushfunc=&Renderer::Render_DrawArrays;
	}
		
	// do all the stuff that affects all passes the same way,
	// deform vertices and setup the static arrays (vertices and normals)
	SetupShader();

	glMatrixMode(GL_TEXTURE);


	// for testing purposes
	/*for (int i=0;i<info.num_features;++i)
	{
		if (info.features[i].name==FEATURE_DLIGHT)
		{
			Render_DLight(*((DLIGHTFEATURE*)info.features[i].data));
			goto SKIP_RENDERING;
		}
	}*/

	// decide, what and how to render
	if (!(int)r_lightmap)
	{
		if ((int)r_ext_multitexture&&(ogl->getCaps()&GLCAPS_MULTITEXTURE))
		{	
			prepare_MTex();
			Render_MTex(0,__min(info.shader->num_passes,(int)r_maxPasses));	// spread passes over as many texture units as available
		}
		else
		{
			Render_Generic(0,__min(info.shader->num_passes,(int)r_maxPasses)); // do not make use of singlepass-multitexturing, just use GL´s plain BlendFunc
		}	
	}
	else
	{
		Render_LightmapOnly();	
	}


	{
		for (int i=0;i<info.num_features;++i)
		{
			switch (info.features[i].name)
			{
			case FEATURE_FOG:
				Render_FogPass(*((FOGFEATURE*)info.features[i].data));
				break;
			case FEATURE_DLIGHT:
				Render_DLight2(*((DLIGHTFEATURE*)info.features[i].data));
				break;
			}
		}
	}

SKIP_RENDERING:

	if ((int)r_showtris) ShowTris();
	if (((int)r_shownormals)&&(info.shader->usedarrays&NORMALARRAY)) ShowNormals();

	// unlock all locked arrays
	glUnlockArraysEXT();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}



void Renderer::SetupShader()
{
	if (info.shader->num_deformv) DeformVerts(); 

	SetupStaticArrays();

	int diff_glstate=glstate^info.shader->shaderflags;

	if (diff_glstate&SHADER_NOCULL)
	{
		if (info.shader->shaderflags&GLSTATE_NOCULL)
		{
			glstate|=GLSTATE_NOCULL;
			glDisable(GL_CULL_FACE);
		}
		else
		{
			glstate&=~GLSTATE_NOCULL;
			glEnable(GL_CULL_FACE);
         }
	}

	if (diff_glstate&SHADER_CULLFRONT)
	{
		if (info.shader->shaderflags&SHADER_CULLFRONT)
		{
			glstate|=GLSTATE_CULLFRONT;
			glCullFace(GL_FRONT);
		}
		else
		{
			glstate&=~GLSTATE_CULLFRONT;
			glCullFace(GL_BACK);
		}
	}
	
	if (diff_glstate&SHADER_POLYGONOFFSET)
	{
		if (info.shader->shaderflags&SHADER_POLYGONOFFSET)
		{
			glstate|=GLSTATE_POLYGONOFFSET;
			glEnable(GL_POLYGON_OFFSET_FILL);
		}
		else
		{
			glstate&=~GLSTATE_POLYGONOFFSET;
			glDisable(GL_POLYGON_OFFSET_FILL);
		}
	}
	return;
}



// Vertex- and Normal-Deformation is done here
void Renderer::DeformVerts()
{
VECTOR3 def;
float w;
int v;

	for (int d=0;d<info.shader->num_deformv;d++)
	{
		switch (info.shader->deformv[d])
		{
		case VDEF_MOVE:
			def=(VECTOR3&)info.shader->deformv_param[d];
			def*=info.shader->deformv_wave[d](g_Time);
			glTranslatef(def.x,def.y,def.z);
			break;
		case VDEF_BULGE: 
			{
				if (src_arrays.normals)
				{
					int f;
					for (f=0;f<info.num_features;++f)
					{
						if (info.features[f].name==FEATURE_PATCHDIM) break;
					}
					if (f==info.num_features) continue; // no patchdim feature found

					int *patchdim=(int*)info.features[f].data;
					int xdim,ydim;
					xdim=patchdim[0];ydim=patchdim[1];
		
					float width=info.shader->deformv_param[d][0]; // I think this means how many bulges should be visible along the patch
					float height=info.shader->deformv_param[d][1];// maximum deformation
					float speed=info.shader->deformv_param[d][2];// I dont know exactly, but could mean, how fast should the "width" bulges pass the whole patch

					speed=g_Time/(speed*width);
					speed-=floorf(speed);


					// the patch is now given as a grid of discrete vertices that build the curved surface in 3d-space
					// xdim tells in how many vertices the patch was tesseletated in u direction (columns)
					// ydim tells the many rows there are
     
					width/=(float)ydim; // calculate the "portion" of one bulge each vertex gets
					int	elem=0;
					VECTOR3 *vertices=src_arrays.vertices;
					VECTOR3 *normals=src_arrays.normals;
					// Note that I let the wave go along the y-direction. This fits for q3dm4, but may go wrong with
					// other patches. In this case we would have to choose the direction dependend on some criteria
					for (int y=0;y<ydim;++y)
					{
						w=sinf(((float)y*width+speed)*TWOPI)*height;
						for (v=0;v<xdim;++v)
						{	
							 def=normals[elem]*w;
							Vertices[elem]=vertices[elem]+def;
							elem++;
						}
					}
					src_arrays.vertices=Vertices;
				}
			}
			break;
		case VDEF_WAVE:
			if (src_arrays.normals)
			{
				VECTOR3 *src_vertices=src_arrays.vertices;
				VECTOR3 *src_normals=src_arrays.normals;
				const CWave &vdef=info.shader->deformv_wave[d];

				float t=1.0f/info.shader->deformv_param[d][0];
				for (v=0;v<src_arrays.num_vertices;++v)
				{	
					// calling evaluate for every vertex may waste time
					 w=vdef(g_Time+t*(src_vertices[v].x+src_vertices[v].y+src_vertices[v].z));
					 def=src_normals[v]*w;
					 Vertices[v]=src_vertices[v]+def;
				}
				src_arrays.vertices=Vertices;
			}
			break;
		case VDEF_NORMAL: 
			if (src_arrays.normals)
			{
				VECTOR3 *src_normals=src_arrays.normals;
				VECTOR3 *src_vertices=src_arrays.vertices;
                float t=g_Time*info.shader->deformv_param[d][1];
				t-=floorf(t);
				t*=TWOPI;
				float lat,lng;
				float amp=info.shader->deformv_param[d][0];
				for (v=0; v<src_arrays.num_vertices; ++v)
				{
					getLatLongFromVec(src_normals[v],lat,lng);
					lat+=(sinf(t+src_vertices[v].x+src_vertices[v].y)*amp);
					lng+=(sinf(t+src_vertices[v].z+src_vertices[v].y)*amp);
					getVecFromLatLong(lat,lng,Normals[v]);
				}
				src_arrays.normals=Normals;
			}

			break;
		case VDEF_AUTOSPRITE: 
			{
				VECTOR3 t;
				MATRIX4 W;
				getModelview(W);
				

				MATRIX3 M(W);// Rotational Part of Worldmatrix
				Transpose3(M);	// invert it

				VECTOR3 *src_vertices=src_arrays.vertices;
				INDEX  *src_indices=src_arrays.indices;
				
				for (v=0;v<src_arrays.num_indices;v+=6) 
				{	
					t=(src_vertices[src_indices[v+1]]+src_vertices[src_indices[v+2]])*0.5; // the middle of the quad
					// move the middle of the quad to the origin, rotate it so it faces the camera and move it back 
					Vertices[src_indices[v]]=t+(M*(src_vertices[src_indices[v]]-t));
					Vertices[src_indices[v+1]]=t+(M*(src_vertices[src_indices[v+1]]-t));
					Vertices[src_indices[v+2]]=t+(M*(src_vertices[src_indices[v+2]]-t));
					Vertices[src_indices[v+5]]=t+(M*(src_vertices[src_indices[v+5]]-t));
				}

				src_arrays.vertices=Vertices;
			}
			break;
		case VDEF_AUTOSPRITE2: // mhhm, this one is a bit more tricky than the one above
			{
				MATRIX3 M,M3,M4;
				VECTOR3	t,t2,t3;
			
				VECTOR3 *src_vertices=src_arrays.vertices;
				INDEX  *src_indices=src_arrays.indices;
				// axis-billboards are pretransformed so always the 0-2 axis is the longest
				for (v=0;v<src_arrays.num_indices;v+=6) 
				{	
					// build a matrix were the longest axis of the billboard is the Y-Axis
					// It represents the system of the billboard
					// FIXME: this assumes that the billboard itself is not transformed and given in world-coordinates
					// it should not be possible to attach an axis-billboard to an object by now
					// (I should move the camera from worldspace into modelspace first to accomplish this)
					// The matrix gets orthonormalized, to prevent non-orthogonal quads from screwing up
					// everything. Thanks go to Vic for this hint!
					(VECTOR3&)M3.a[1]=src_vertices[src_indices[v+2]]-src_vertices[src_indices[v]];
					Normalize((VECTOR3&)M3.a[1]);
					(VECTOR3&)M3.a[0]=src_vertices[src_indices[v+1]]-src_vertices[src_indices[v]];
					(VECTOR3&)M3.a[2]=(VECTOR3&)M3.a[0]^(VECTOR3&)M3.a[1];
					Normalize((VECTOR3&)M3.a[2]);
					(VECTOR3&)M3.a[0]=(VECTOR3&)M3.a[1]^(VECTOR3&)M3.a[2];
					
	
					t=(src_vertices[src_indices[v+1]]+src_vertices[src_indices[v+2]])*0.5; // the middle of the quad
					// mid-of-quad to camera vector
					t3=g_Camera->getTransformation().getTranslation()-t;
					// filter any longest-axis-parts off the camera-direction
					t2=t3-((VECTOR3&)M3.a[1]*(t3*(VECTOR3&)M3.a[1]));
					Normalize(t2);

					// the system of the billboard as it should be to face the camera
					(VECTOR3&)M4.a[2]=t2;
					(VECTOR3&)M4.a[1]=(VECTOR3&)M3.a[1];
					(VECTOR3&)M4.a[0]=(VECTOR3&)M4.a[1]^(VECTOR3&)M4.a[2];
				
					// this is a bit hard to understand, but works as follows:
					// M3 is the current local system of the billboard
					// M4 is the system as it should be to let the billboard look at the camera
					// it should be M4=M3*M, where M is the matrix that is neccessary to rotate the billboard from M3 to M4
					// so M=M3^-1*M4, _but_
					// the geometry is given in world-coordinates (but it´s centre moved to the origin), so it first has to be converted back to 
					// the billboards local system (thereby aligning the y-Axis with the axis the billboard should rotate around),
					// then transformed with M and then transformed back into the
					// world-coordinate system.
					// the whole Transformation therefore equals to
					// M= M3 * (M3^-1*M4) * M3^-1, which is M4*M3^-1
					Transpose3(M3);
					M=M4*M3;

					// move the quad to origin, rotate it so it faces the camera and move it back 
					Vertices[src_indices[v]]=t+(M*(src_vertices[src_indices[v]]-t));
					Vertices[src_indices[v+1]]=t+(M*(src_vertices[src_indices[v+1]]-t));
					Vertices[src_indices[v+2]]=t+(M*(src_vertices[src_indices[v+2]]-t));
					Vertices[src_indices[v+5]]=t+(M*(src_vertices[src_indices[v+5]]-t));
				}

				src_arrays.vertices=Vertices;
			}
			break;
		}
	}
}


// "StaticArrays", because they are setup once for the whole shader
void Renderer::SetupStaticArrays()
{

static const int vertexstride = sizeof(VECTOR3)-3*sizeof(float) ? sizeof(VECTOR3) : 0;

	// vertexarray is always on
	glVertexPointer(3,GL_FLOAT,vertexstride,src_arrays.vertices);

	if (src_arrays.normals!=NULL&&(info.shader->shaderflags&SHADER_USENORMALS))
	{
		
		if (!(glstate&GLSTATE_NORMALARRAY))
		{
			glEnableClientState(GL_NORMAL_ARRAY);
			glstate|=GLSTATE_NORMALARRAY;
		}
		glNormalPointer(GL_FLOAT,vertexstride,src_arrays.normals);
	} 
	else 
	{
		if (glstate&GLSTATE_NORMALARRAY)
		{
			glDisableClientState(GL_NORMAL_ARRAY);
			glstate&=~GLSTATE_NORMALARRAY;
		}
	}
}

// called every pass
void Renderer::SetupDynamicArrays()
{
	if (src_arrays.texcoords!=NULL)
	{
		if (!(tustate[texunit].flags&TEXUNIT_TCARRAY))
		{
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			tustate[texunit].flags|=TEXUNIT_TCARRAY;
		}
		glTexCoordPointer(2,GL_FLOAT,0,src_arrays.texcoords);
	}
	else
	{
		if (tustate[texunit].flags&TEXUNIT_TCARRAY)
		{
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			tustate[texunit].flags&=~TEXUNIT_TCARRAY;
		}
	}

	if (texunit) return;
	
	if (src_arrays.colors!=NULL)
	{
		if (!(glstate&GLSTATE_COLORARRAY))
		{
			glEnableClientState(GL_COLOR_ARRAY);
			glstate|=GLSTATE_COLORARRAY;
		}
		glColorPointer(4,GL_UNSIGNED_BYTE,0,src_arrays.colors);
	}
	else
	{
		if (glstate&GLSTATE_COLORARRAY)
		{
			glDisableClientState(GL_COLOR_ARRAY);
			glstate&=~GLSTATE_COLORARRAY;
		}
	}
}

void Renderer::prepare_MTex()
{
	// switch off the colorarray and all texturecoord-arrays

	if (glstate&GLSTATE_COLORARRAY)
	{
		glstate&=~GLSTATE_COLORARRAY;
		glDisableClientState(GL_COLOR_ARRAY);
	}

	int num_units=ogl->getNumTextureUnits();

	for (texunit=0;texunit<num_units;++texunit)
	{
		if (tustate[texunit].flags&TEXUNIT_TCARRAY)
		{
			tustate[texunit].flags&=~TEXUNIT_TCARRAY;
			glClientActiveTextureARB(GL_TEXTURE0_ARB+texunit);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		}
	}

	// now lock the vertexbuffer and possibly also the normalarray
	glLockArraysEXT(0,src_arrays.num_vertices);
}

// Render all passes by trying to spread them over all textureunits
void Renderer::Render_MTex(int passnum, int num_passes)
{
	int  rval;

	lastpass=&info.shader->passes[passnum];

	int last_passnum=passnum+num_passes;

	while (passnum!=last_passnum)
	{
		for (texunit=0;(texunit<ogl->getNumTextureUnits())&&(passnum!=last_passnum);++passnum, restoreSourcePointers())
		{
			if (texunit&&(info.shader->passes[passnum].passflags&PASS_FIRSTUNIT)) break;
			if ((info.shader->passes[passnum].passflags&PASS_DETAIL)&&(!(int)r_detailtextures)) continue;

			glActiveTextureARB(GL_TEXTURE0_ARB+texunit);
		
			rval=SetupPass(info.shader->passes[passnum]);
			if (rval==RVAL_FIRSTUNIT)
			{
				if ((int)r_fixfirstunit) info.shader->passes[passnum].passflags|=PASS_FIRSTUNIT;
				restoreSourcePointers();
				break;
			}
			else if(rval==RVAL_SKIP) continue;

			if (!(tustate[texunit].flags&TEXUNIT_ENABLED))
			{
				tustate[texunit].flags|=TEXUNIT_ENABLED;
				glEnable(GL_TEXTURE_2D);
			}	

			SetupPassTexture(info.shader->passes[passnum]);
	
			glClientActiveTextureARB(GL_TEXTURE0_ARB+texunit);
			SetupDynamicArrays();

			lastpass=&info.shader->passes[passnum];
			previousflags|=info.shader->passes[passnum].passflags;
			++texunit;			
		}
		if (texunit>0) // ==0 -> all remaining passes skipped
		{
#ifdef _DEBUG
			if (texunit>(int)r_maxUsedTexunit) r_maxUsedTexunit=texunit;
#endif

			// disable not needed textureunits
			for (;texunit<ogl->getNumTextureUnits();texunit++)
			{
			  if (tustate[texunit].flags&TEXUNIT_ENABLED)
			  {
					tustate[texunit].flags&=~TEXUNIT_ENABLED;
					glActiveTextureARB(GL_TEXTURE0_ARB+texunit);				
					glDisable(GL_TEXTURE_2D);
			  }
			  else	break; // if the current unitis not enabled, upper units won´t be enabled either
			  
			}

			(this->*flushfunc)();
		}
	}

}


// use standard-blending
void Renderer::Render_Generic(int passnum, int num_passes)
{
int rval;

	texunit=0;

	lastpass=&info.shader->passes[passnum];

	// before locking we have to disable all arrays we might change
	if (glstate&GLSTATE_COLORARRAY)
	{
		glstate&=~GLSTATE_COLORARRAY;
		glDisableClientState(GL_COLOR_ARRAY);
	}
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	tustate[0].flags&=~TEXUNIT_TCARRAY;

    glLockArraysEXT(0,src_arrays.num_vertices);

	int last_passnum=passnum+num_passes;

	for (; passnum!=last_passnum; ++passnum,restoreSourcePointers())
	{	
		if ((info.shader->passes[passnum].passflags&PASS_DETAIL)&&(!(int)r_detailtextures)) continue;
		rval=SetupPass(info.shader->passes[passnum]);
		if (rval==RVAL_SKIP) continue;

		SetupPassTexture(info.shader->passes[passnum]);
		SetupDynamicArrays();
		
		(this->*flushfunc)(); 
		lastpass=&info.shader->passes[passnum];
		previousflags|=info.shader->passes[passnum].passflags;
	}
}

// find the lightmap-pass and render it separately
void Renderer::Render_LightmapOnly()
{
	int p;
	for (p=0;p<info.shader->num_passes;p++)
	{
		if ((info.shader->passes[p].passflags&(PASS_LIGHTMAP|PASS_LIGHTING))
			||info.shader->passes[p].rgbgen==RGBGEN_LIGHTINGDIFFUSE
			||info.shader->passes[p].rgbgen==RGBGEN_VERTEX
			||info.shader->passes[p].rgbgen==RGBGEN_EXACTVERTEX) goto GOON;
	}
	// Shader has no explicit Lightmap Stage
	if ((int)r_ext_multitexture&&ogl->getNumTextureUnits())
	{	
		prepare_MTex();
		Render_MTex(0,info.shader->num_passes);
	}
	else
	{
		Render_Generic(0,info.shader->num_passes);
	}
	return;

GOON:

	disableMTex();

	int  oldflags=info.shader->passes[p].passflags;
	info.shader->passes[p].passflags&=~PASS_BLENDFUNC;
	info.shader->passes[p].passflags|=PASS_DEPTHWRITE;

	texunit=0;
	int passnum=p;
	lastpass=&info.shader->passes[p];

	int temp=(int)r_ext_multitexture;
	r_ext_multitexture=0;
	SetupPass(info.shader->passes[p]);
	SetupDynamicArrays();
	if (info.shader->passes[p].passflags&PASS_LIGHTMAP)
	{
		SetupPassTexture(info.shader->passes[p]);
	}
	else
	{
		glDisable(GL_TEXTURE_2D);
	}
	glLockArraysEXT(0,src_arrays.num_vertices);

	(this->*flushfunc)();

	glEnable(GL_TEXTURE_2D);
	r_ext_multitexture=temp;
	info.shader->passes[p].passflags=oldflags;

}

void Renderer::Render_FogPass(const FOGFEATURE &fogfeature)
{
	disableMTex();
	
	setTexture(fogtexture);

	if (previousflags&PASS_DEPTHWRITE)
	{
		fogpass.passflags|=PASS_DEPTHFUNC;
	}
	else
	{
		fogpass.passflags&=~PASS_DEPTHFUNC;
	}

	SetupPass(fogpass);
	for (int i=0;i<3;++i) curr_color[i]=fogfeature.shader->fogcolor[i]*identitylighting[i];
	glColor3fv(curr_color);

	TCGEN_Fog(fogfeature.plane,fogfeature.shader->fogdistance);

	SetupDynamicArrays();
	
	(this->*flushfunc)();
}

// setup blending, alphatesting, depthtesting and so on....
Renderer::PASS_RVAL Renderer::SetupPass(CShaderPass &pass)
{
float t1,t2;
int diff_glflags,diff_tuflags;
PASS_RVAL	rval;

	// glstate and texture unit-states are defined in a way that they both
	// can be used together in one int (passflags in this case)
    diff_tuflags=tustate[texunit].flags^pass.passflags;
	diff_glflags=glstate^pass.passflags;


	rval=SetupColors(pass);
	if(rval!=RVAL_OK) return rval;

	if ((int)r_ext_multitexture==2)
		rval=SetupBlendingCombine4(pass);	// use NV_texture_combine4 and ARB_texture_combine
	else
		rval=SetupBlendingGenericMtex(pass); // use standard GL texture environments only

    if (rval!=RVAL_OK) return rval;

	if (pass.passflags&PASS_ALPHAFUNC)
	{
		if (texunit) {return RVAL_FIRSTUNIT;}
		glAlphaFunc(pass.alphafunc,pass.alphafuncref);

		if (diff_glflags&GLSTATE_ALPHATEST)
		{
			glstate|=GLSTATE_ALPHATEST;
			glEnable(GL_ALPHA_TEST);
		}
	}
	else
	{
		if ((!texunit)&&(diff_glflags&GLSTATE_ALPHATEST))
		{
			glstate&=~GLSTATE_ALPHATEST;
			glDisable(GL_ALPHA_TEST);
		}
	}

	if (diff_glflags&GLSTATE_DEPTHFUNC)
	{
		// we won´t change the current setting in upper texture units
		if (texunit) { return RVAL_FIRSTUNIT;}
		if (pass.passflags&PASS_DEPTHFUNC)
		{
			glstate|=GLSTATE_DEPTHFUNC;
			glDepthFunc(pass.depthfunc);
		}
		else
		{
			glstate&=~GLSTATE_DEPTHFUNC;
			glDepthFunc(GL_LEQUAL);
		}
	}
	if (diff_glflags&GLSTATE_NODEPTHTEST)
	{
		if (pass.passflags&PASS_NODEPTHTEST)
		{
			if (texunit) { return RVAL_FIRSTUNIT;}
			glstate|=GLSTATE_NODEPTHTEST;
			glDisable(GL_DEPTH_TEST);
 		}
		else 
		{
			if (!texunit)
			{
				glstate&=~GLSTATE_NODEPTHTEST;
				glEnable(GL_DEPTH_TEST);
			}
		}
	}

	if (diff_glflags&GLSTATE_DEPTHWRITE)
	{
		if (pass.passflags&PASS_DEPTHWRITE)
		{
			glstate|=GLSTATE_DEPTHWRITE;
			glDepthMask(GL_TRUE);
		
 		}
		else 
		{
			if (!texunit)
			{
				glstate&=~GLSTATE_DEPTHWRITE;
				glDepthMask(GL_FALSE);
			}
		}
	}

	if (diff_glflags&GLSTATE_LIGHTING)
	{
		if (pass.passflags&PASS_LIGHTING)
		{
			glstate|=GLSTATE_LIGHTING;
			glEnable(GL_LIGHTING);
		}
		else
		{
			if (!texunit)
			{
				glstate&=~GLSTATE_LIGHTING;
				glDisable(GL_LIGHTING);
			}
		}
	}

	glLoadIdentity();
	
	if (pass.tcgen!=TCGEN_NONE)
	{
		switch (pass.tcgen)
		{
		case TCGEN_ENVIRONMENT:
		{
			if (src_arrays.normals)
			{
				VECTOR3 dir;
				MATRIX4 M;
				getModelview(M);
				MATRIX4 M2=g_Camera->getTransformation().getMatrix()*M;
				MATRIX3 R(M);
				
				SimpleInverse4(M);
				const VECTOR3 &pos=(VECTOR3&)M.a[3];
				VECTOR2 *texcoords=TexCoords[texunit];
				VECTOR3 *vertices=src_arrays.vertices;
				VECTOR3 *normals=src_arrays.normals;				
				//Aftershocks method
				for(int v=0; v<src_arrays.num_vertices; ++v)
				{
					dir=pos-vertices[v];
					Normalize(dir);
					texcoords[v].x=dir.z-normals[v].z;
					texcoords[v].y=dir.y-normals[v].y;
				}
				src_arrays.texcoords=texcoords;
			}
		}
		break;
		case TCGEN_VECTOR: 
		{
			VECTOR4 plane;
			plane=pass.tcgen_vector[0]; plane.w=0;
			glTexGeni(GL_S,GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
			glTexGenfv(GL_S,GL_OBJECT_PLANE, (float*)&plane);
			plane=pass.tcgen_vector[1]; plane.w=0;
			glTexGeni(GL_T,GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
			glTexGenfv(GL_T,GL_OBJECT_PLANE, (float*)&plane);
			src_arrays.texcoords=NULL;
		}
		break;
		case TCGEN_LIGHTMAP: 
			src_arrays.texcoords=sourcebuffer->lmcoords;
			break;
		case TCGEN_FOG:
			{
				//FIXME: plane should be determined by a fogfeature
				PLANE fp(src_arrays.vertices[src_arrays.indices[1]],src_arrays.vertices[src_arrays.indices[0]],src_arrays.vertices[src_arrays.indices[2]]);
				TCGEN_Fog(fp,info.shader->fogdistance);
			}
			break;
		case TCGEN_SPHEREMAP:
			glTexGeni(GL_S,GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
			glTexGeni(GL_T,GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
			break;
		}
	}

	
	if (diff_tuflags&TEXUNIT_TCGEN)
	{
		if (pass.passflags&PASS_GLTCGEN)
		{	
			tustate[texunit].flags|=TEXUNIT_TCGEN;
			glEnable(GL_TEXTURE_GEN_S);
			glEnable(GL_TEXTURE_GEN_T);
		}
		else
		{	
			tustate[texunit].flags&=~TEXUNIT_TCGEN;
			glDisable(GL_TEXTURE_GEN_S);
			glDisable(GL_TEXTURE_GEN_T);
		}
	}
	

	for (int i=pass.tcmod_num-1;i>=0;--i)
	{
		switch (pass.tcmod_order[i])
		{
		case TCMOD_SCALE:
			glScalef(pass.tcmod_scale[0],pass.tcmod_scale[1],1.0f);
			break;
		case TCMOD_ROTATE:
			t1=fmodf(pass.tcmod_rotate*g_Time,360.0);
			glTranslatef(0.5f,0.5f,0);
			glRotatef(t1,0,0,-1.0f);
			glTranslatef(-0.5f,-0.5f,0);
			break;
		case TCMOD_SCROLL:
			t1=pass.tcmod_scroll[0]*g_Time;
			t2=pass.tcmod_scroll[1]*g_Time;
			glTranslatef(t1,t2,0.0f);
			break;
		case TCMOD_STRETCH:
			{
				t1=1.0f/pass.tcmod_stretch(g_Time);
				glTranslatef(0.5f,0.5f,0);
				glScalef(t1,t1,1.0f);
				glTranslatef(-0.5f,-0.5f,0);
			}
			break;
		case TCMOD_TURB:
			{					
				if (!src_arrays.texcoords) break; // FIXME: replace with assert ?
				VECTOR2 *src_texcoords=src_arrays.texcoords;
				VECTOR2 *texcoords=TexCoords[texunit]; 
				
				t1=pass.tcmod_turb[3]*g_Time+pass.tcmod_turb[2];
				t1-=floorf(t1);
				t1*=TWOPI;
				const float &t2=pass.tcmod_turb[1];
				for (int v=0;v<src_arrays.num_vertices;++v)
				{
					texcoords[v].x=src_texcoords[v].x+sinf(t1+src_texcoords[v].x)*t2;
					texcoords[v].y=src_texcoords[v].y+sinf(t1+src_texcoords[v].y)*t2;
				}
				src_arrays.texcoords=texcoords;
			}
			break;
		case TCMOD_TRANSFORM:
			float m[16]={pass.tcmod_transform[0],pass.tcmod_transform[1],0,0,
						 pass.tcmod_transform[2],pass.tcmod_transform[3],0,0,
						 0,0,1,0,
						 pass.tcmod_transform[4],pass.tcmod_transform[5],0,1};
			glMultMatrixf(m);
			break;
		}
	}

	//to effect the coordinates first it has to be passed last
	if (pass.tcgen==TCGEN_ENVIRONMENT)
	{
		glTranslatef(0.5f,0.5f,0.0f);
		glScalef(0.5f,0.5f,1.0f);
	}

	return RVAL_OK;
}
Renderer::PASS_RVAL Renderer::SetupBlendingGenericMtex(CShaderPass &pass)
{
int diff_glflags;

	diff_glflags=glstate^(pass.passflags&PASS_GLSTATE_MASK);

	if (texunit&&(pass.passflags&PASS_TEXENV))
	{
		if ((pass.texenv==GL_DECAL)||  // alphachannels get combined wrong with GL_DECAL
			((glstate&GLSTATE_BLENDING)&&(lastpass->texenv!=pass.texenv)))
		   return RVAL_FIRSTUNIT;
		if (pass.texenv!=tustate[texunit].texenv)
		{
			tustate[texunit].texenv=pass.texenv;
			glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, pass.texenv);
		}
		
	}
	else
	{
		if (pass.passflags&PASS_BLENDFUNC)
		{
			if (texunit){return RVAL_FIRSTUNIT;} // Blendfunc nur für erste TexUnit

			if (tustate[0].blendsrc!=pass.blendsrc||tustate[0].blenddest!=pass.blenddest)
			{
				tustate[0].blendsrc=pass.blendsrc;
				tustate[0].blenddest=pass.blenddest;
				tustate[0].texenv=pass.texenv;
				glBlendFunc(pass.blendsrc,pass.blenddest);
			}
			if (diff_glflags&GLSTATE_BLENDING)
			{
				glEnable(GL_BLEND);
				glstate|=GLSTATE_BLENDING;
			}
		}
		else
		{
			if ((!texunit)&&(diff_glflags&GLSTATE_BLENDING))
			{
				glDisable(GL_BLEND);
				glstate&=~GLSTATE_BLENDING;
			}
		}
	}

	return RVAL_OK;
}

Renderer::PASS_RVAL Renderer::SetupBlendingCombine4(CShaderPass &pass)
{

	if (texunit&&(pass.passflags&PASS_BLENDFUNC))
	{
		// pass needs blending
		if (pass.passflags&PASS_TEXENV)
		{
			if	((glstate&GLSTATE_BLENDING)&&
				(lastpass->texenv!=pass.texenv)) return RVAL_FIRSTUNIT;
			
			if (tustate[texunit].texenv==pass.texenv) return RVAL_OK;
			// blending can be expressed with means of texture environments
			switch (pass.texenv)
			{
			case GL_MODULATE:
			case GL_ADD:
			case GL_BLEND:
			case GL_REPLACE:
				// these modes are best set with TexEnv, Combine4 would need much more setup
				glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, pass.texenv);
				break;
			case GL_DECAL:
				// mimics Alpha-Blending in upper texture stage, but instead of multiplying the alpha-channel, they´re added
				// this way it can be possible to use GL_DECAL in both texture-units, while still looking good
				// normal mutlitexturing would multiply the alpha-channel which looks ugly
				glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
				glTexEnvi (GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_INTERPOLATE_EXT);
				glTexEnvi (GL_TEXTURE_ENV, GL_COMBINE_ALPHA_EXT, GL_ADD);

				glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE0_RGB_EXT, GL_TEXTURE);
				glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND0_RGB_EXT, GL_SRC_COLOR);
				glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_EXT, GL_TEXTURE);
				glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND0_ALPHA_EXT, GL_SRC_ALPHA);
		
				glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE1_RGB_EXT, GL_PREVIOUS_EXT);
				glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND1_RGB_EXT, GL_SRC_COLOR);
				glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE1_ALPHA_EXT, GL_PREVIOUS_EXT);
				glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND1_ALPHA_EXT, GL_SRC_ALPHA);

				glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE2_RGB_EXT, GL_TEXTURE);
				glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND2_RGB_EXT, GL_SRC_ALPHA);
			}
			tustate[texunit].texenv=pass.texenv;
			tustate[texunit].blendsrc=pass.blendsrc;
			tustate[texunit].blenddest=pass.blenddest;
			return RVAL_OK;
		
		}
		else 
		{
			// mimics ogl´s full blendfunc in upper texture unit
			// since you cannot know, how this blending interacts with previous blendmodes
			// it can only be used if currently no blending is enabled  
			if	(glstate&GLSTATE_BLENDING) return RVAL_FIRSTUNIT;

			tustate[texunit].texenv=-1;

			glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE4_NV);	
			glTexEnvi (GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_ADD);
			glTexEnvi (GL_TEXTURE_ENV, GL_COMBINE_ALPHA_EXT, GL_ADD);
			

			glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE0_RGB_EXT, GL_TEXTURE);
			glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND0_RGB_EXT, GL_SRC_COLOR);
			glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_EXT, GL_TEXTURE);
			glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND0_ALPHA_EXT, GL_SRC_ALPHA);

			if (tustate[texunit].blendsrc!=pass.blendsrc)
			{
				tustate[texunit].blendsrc=pass.blendsrc;
				switch (pass.blendsrc)
				{
				case GL_ONE:
					glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE1_RGB_EXT, GL_ZERO);
					glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND1_RGB_EXT, GL_ONE_MINUS_SRC_COLOR);
					glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE1_ALPHA_EXT, GL_ZERO);
					glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND1_ALPHA_EXT, GL_ONE_MINUS_SRC_ALPHA);
					break;
				case GL_ZERO:
					glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE1_RGB_EXT, GL_ZERO);
					glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND1_RGB_EXT, GL_SRC_COLOR);
					glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE1_ALPHA_EXT, GL_ZERO);
					glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND1_ALPHA_EXT, GL_SRC_ALPHA);
					break;
				case GL_DST_COLOR:
					glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE1_RGB_EXT, GL_PREVIOUS_EXT);
					glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND1_RGB_EXT, GL_SRC_COLOR);
					glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE1_ALPHA_EXT, GL_PREVIOUS_EXT);
					glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND1_ALPHA_EXT, GL_SRC_ALPHA);
					break;
				case GL_ONE_MINUS_DST_COLOR:
					glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE1_RGB_EXT, GL_PREVIOUS_EXT);
					glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND1_RGB_EXT, GL_ONE_MINUS_SRC_COLOR);
					glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE1_ALPHA_EXT, GL_PREVIOUS_EXT);
					glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND1_ALPHA_EXT, GL_ONE_MINUS_SRC_ALPHA);
					break;
				case GL_SRC_ALPHA:
					glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE1_RGB_EXT, GL_TEXTURE);
					glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND1_RGB_EXT, GL_SRC_ALPHA);
					glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE1_ALPHA_EXT, GL_TEXTURE);
					glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND1_ALPHA_EXT, GL_SRC_ALPHA);
					break;
				case GL_ONE_MINUS_SRC_ALPHA:
					glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE1_RGB_EXT, GL_TEXTURE);
					glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND1_RGB_EXT, GL_ONE_MINUS_SRC_ALPHA);
					glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE1_ALPHA_EXT, GL_TEXTURE);
					glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND1_ALPHA_EXT, GL_ONE_MINUS_SRC_ALPHA);
					break;
				case GL_DST_ALPHA:
					
					glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE1_RGB_EXT, GL_PREVIOUS_EXT);
					glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE1_ALPHA_EXT, GL_PREVIOUS_EXT);
					glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND1_RGB_EXT, GL_SRC_ALPHA);
					glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND1_ALPHA_EXT, GL_SRC_ALPHA);

					break;
				case GL_ONE_MINUS_DST_ALPHA:
					glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE1_RGB_EXT, GL_PREVIOUS_EXT);
					glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE1_ALPHA_EXT, GL_PREVIOUS_EXT);
					glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND1_RGB_EXT, GL_ONE_MINUS_SRC_ALPHA);
					glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND1_ALPHA_EXT, GL_ONE_MINUS_SRC_ALPHA);

					break;
				}
			}

			glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE2_RGB_EXT, GL_PREVIOUS_EXT);
			glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND2_RGB_EXT, GL_SRC_COLOR);
			glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE2_ALPHA_EXT, GL_PREVIOUS_EXT);	
			glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND2_ALPHA_EXT, GL_SRC_ALPHA);

			if (tustate[texunit].blenddest!=pass.blenddest)
			{
				tustate[texunit].blenddest=pass.blenddest;
				switch (pass.blenddest)
				{
				case GL_ONE:
					glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE3_RGB_NV, GL_ZERO);
					glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND3_RGB_NV, GL_ONE_MINUS_SRC_COLOR);
					glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE3_ALPHA_NV, GL_ZERO);
					glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND3_ALPHA_NV, GL_ONE_MINUS_SRC_ALPHA);
					break;
				case GL_ZERO:
					glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE3_RGB_NV, GL_ZERO);
					glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND3_RGB_NV, GL_SRC_COLOR);
					glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE3_ALPHA_NV, GL_ZERO);
					glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND3_ALPHA_NV, GL_SRC_ALPHA);
					break;
				case GL_SRC_COLOR:
					glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE3_RGB_NV, GL_TEXTURE);
					glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND3_RGB_NV, GL_SRC_COLOR);
					glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE3_ALPHA_NV, GL_TEXTURE);
					glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND3_ALPHA_NV, GL_SRC_ALPHA);
					break;
				case GL_ONE_MINUS_SRC_COLOR:
					glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE3_RGB_NV, GL_TEXTURE);
					glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND3_RGB_NV, GL_ONE_MINUS_SRC_COLOR);
					glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE3_ALPHA_NV, GL_TEXTURE);
					glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND3_ALPHA_NV, GL_ONE_MINUS_SRC_ALPHA);
					break;
				case GL_SRC_ALPHA:
					glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE3_RGB_NV, GL_TEXTURE);
					glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND3_RGB_NV, GL_SRC_ALPHA);
					glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE3_ALPHA_NV, GL_TEXTURE);
					glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND3_ALPHA_NV, GL_SRC_ALPHA);
					break;
				case GL_ONE_MINUS_SRC_ALPHA:
					glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE3_RGB_NV, GL_TEXTURE);
					glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND3_RGB_NV, GL_ONE_MINUS_SRC_ALPHA);
					glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE3_ALPHA_NV, GL_TEXTURE);
					glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND3_ALPHA_NV, GL_ONE_MINUS_SRC_ALPHA);
					break;
				case GL_DST_ALPHA:
					glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE3_RGB_NV, GL_PREVIOUS_EXT);
					glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND3_RGB_NV, GL_SRC_ALPHA);
					glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE3_ALPHA_NV, GL_PREVIOUS_EXT);
					glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND3_ALPHA_NV, GL_SRC_ALPHA);
					break;
				case GL_ONE_MINUS_DST_ALPHA:
					glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE3_RGB_NV, GL_PREVIOUS_EXT);
					glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND3_RGB_NV, GL_ONE_MINUS_SRC_ALPHA);
					glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE3_ALPHA_NV, GL_PREVIOUS_EXT);
					glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND3_ALPHA_NV, GL_ONE_MINUS_SRC_ALPHA);
					break;
				}
			}
			return RVAL_OK;
		}		
	}

	int diff_glflags=glstate^pass.passflags;
	if (pass.passflags&PASS_BLENDFUNC)
	{
		if (texunit){return RVAL_FIRSTUNIT;} // Blendfunc nur für erste TexUnit
		if (tustate[0].blendsrc!=pass.blendsrc||tustate[0].blenddest!=pass.blenddest)
		{
			tustate[0].blendsrc=pass.blendsrc;
			tustate[0].blenddest=pass.blenddest;
			tustate[0].texenv=pass.texenv;
			glBlendFunc(pass.blendsrc,pass.blenddest);
		}
		if (diff_glflags&GLSTATE_BLENDING)
		{
			glstate|=GLSTATE_BLENDING;
			glEnable(GL_BLEND);
		}
	}
	else
	{
		if ((!texunit)&&(diff_glflags&GLSTATE_BLENDING))
		{
			glstate&=~GLSTATE_BLENDING;
			glDisable(GL_BLEND);
		}
	}
	return RVAL_OK;
}


Renderer::PASS_RVAL Renderer::SetupColors(CShaderPass &pass)
{

	// Only rgbGen identiy is allowed in upper texture units	
	if (texunit)
	{
		if (pass.rgbgen==RGBGEN_IDENTITY&&pass.alphagen==ALPHAGEN_IDENTITY) { return RVAL_OK;}
		return RVAL_FIRSTUNIT;
	}

	float t1;
	curr_color[3]=curr_color[2]=curr_color[1]=curr_color[0]=1.0f;

	src_arrays.colors=NULL; 

	switch (pass.rgbgen)
	{
	case RGBGEN_IDENTITYLIGHTING:
		{
			for (int i=0;i<3;++i)curr_color[i]=identitylighting[i];
		}
		break;
	case RGBGEN_WAVE:
			t1=pass.rgbgen_func(g_Time);
			if ((t1<=0.01f)&&(pass.texenv==GL_ADD)) 
			{
				return RVAL_SKIP;
			}
			curr_color[0]=curr_color[1]=curr_color[2]=t1;
		break;
	case RGBGEN_EXACTVERTEX:
	case RGBGEN_VERTEX: 
		src_arrays.colors=org_arrays.colors;
		break;
	case RGBGEN_ONEMINUSVERTEX:
		RGBGEN_OneMinusVertex();
		break;
	case RGBGEN_ENTITY: 
		if (org_arrays.colors)
		{
			curr_color[0]=(1.0f/255.0f)*(float)org_arrays.colors[0].r;
			curr_color[1]=(1.0f/255.0f)*(float)org_arrays.colors[0].g;
			curr_color[2]=(1.0f/255.0f)*(float)org_arrays.colors[0].b;
		}
		break;
	case RGBGEN_ONEMINUSENTITY:
		if (org_arrays.colors)
		{
			curr_color[0]=1.0f-(1.0f/255.0f)*(float)org_arrays.colors[0].r;
			curr_color[1]=1.0f-(1.0f/255.0f)*(float)org_arrays.colors[0].g;
			curr_color[2]=1.0f-(1.0f/255.0f)*(float)org_arrays.colors[0].b;
		}
		break;
	case RGBGEN_LIGHTINGDIFFUSE: 
		src_arrays.colors=org_arrays.colors;
		break;
	case RGBGEN_FLARE:
		RGBGEN_Flare();
		break;
	case RGBGEN_FOG:
		{
			for (int i=0;i<3;++i) curr_color[i]=info.shader->fogcolor[i]*identitylighting[i];
		}
		break;
	case RGBGEN_CONST:
		{
			for (int i=0;i<3;++i) curr_color[i]=pass.rgbgen_const[i]*identitylighting[i];
		}
		break;
	case RGBGEN_MATERIAL: // texture sets diffuse color in bind()
		return RVAL_OK;			  // no animated alpha possible (needed anyway?)
	}

	if (pass.alphagen!=ALPHAGEN_IDENTITY)
	{
		switch (pass.alphagen)
		{
		case ALPHAGEN_WAVE:
		
			if (!src_arrays.colors)
			{
				t1=pass.alphagen_func(g_Time);
				curr_color[3]=t1;
			}
			else
			{
				ALPHAGEN_Wave(pass.alphagen_func);
			}

			break;
		case ALPHAGEN_PORTAL:
			ALPHAGEN_Portal(pass.alphagen_portalrange);
			break;
		case ALPHAGEN_VERTEX:
			ALPHAGEN_Vertex();
			break;
		case ALPHAGEN_ONEMINUSVERTEX:
			ALPHAGEN_OneMinusVertex();
			break;
		case ALPHAGEN_ENTITY: 
			if (src_arrays.colors) curr_color[3]=(1.0f/255.0f)*(float)src_arrays.colors[0].alpha;
			break;
		case ALPHAGEN_ONEMINUSENTITY: 
			if (src_arrays.colors) curr_color[3]=1.0f-(1.0f/255.0f)*(float)src_arrays.colors[0].alpha;
			break;
		case ALPHAGEN_LIGHTINGDIFFUSE: 
			break;
		case ALPHAGEN_LIGHTINGSPECULAR: 
			ALPHAGEN_LightingSpecular();
		}
	}
	if (!src_arrays.colors)
	{
		glColor4fv(curr_color);
	}
	return RVAL_OK;
}

Renderer::PASS_RVAL Renderer::SetupPassTexture(CShaderPass &pass)
{
	if (!(pass.passflags&PASS_LIGHTMAP))
	{   
		int frame=0;
		if (pass.passflags&PASS_ANIMMAP)
		{
			frame=(int)fmodf(pass.anim_fps*g_Time,(float)pass.anim_numframes);
		}
		setTexture(pass.anim_frames[frame]);
	}
	else
	{
		setTexture(sourcebuffer->getLightmap());
	}
	return RVAL_OK;
}

void Renderer::RGBGEN_OneMinusVertex()
{
	if (!org_arrays.colors) return;
	COLOR *colors=org_arrays.colors;
	for (int v=0;v<src_arrays.num_vertices;v++)
	{	
		Colors[v].r=255-colors[v].r;	
		Colors[v].g=255-colors[v].g;
		Colors[v].b=255-colors[v].b;
		Colors[v].alpha=255;// alphagen vertex macht dann: originalinfo.colors[v].alpha;
	}
	src_arrays.colors=Colors;
}	

void Renderer::RGBGEN_Flare()
{
	const VECTOR3 &campos=g_Camera->getTransformation().getTranslation();
	float d;
	VECTOR3 *vertices=src_arrays.vertices;
	for (int v=0;v<src_arrays.num_vertices;v++)
	{
		d=Distance(campos,vertices[v]);
		d*=0.5;
		if (d>255) d=255.0;
		Colors[v].b=Colors[v].g=Colors[v].r=(BYTE)(d*identitylighting[0]);
	}
	src_arrays.colors=Colors;
}

void Renderer::ALPHAGEN_Vertex()
{
	if (!org_arrays.colors) return;
	if (!src_arrays.colors) // rgbvalues are determined by curr_color
	{
		const COLOR c(((VECTOR3&)curr_color)*255.0f);
		for (int v=0;v<src_arrays.num_vertices;++v)
		{
			Colors[v].r=c.r;	
			Colors[v].g=c.g;
			Colors[v].b=c.b;
			Colors[v].alpha=org_arrays.colors[v].alpha;
		}
		src_arrays.colors=Colors;
	}
	else if (src_arrays.colors==Colors) // rgb values are already calculated somehow
	{
		for (int v=0;v<src_arrays.num_vertices;++v)
		{
			Colors[v].alpha=org_arrays.colors[v].alpha;
		}	
	}
	// if info.colors==originalinfo.colors then we don´t need to do anything
}
void Renderer::ALPHAGEN_OneMinusVertex()
{
	if (!org_arrays.colors) return;

	if (!src_arrays.colors)
	{
		const COLOR c(VECTOR3(curr_color)*255.0f);
		for (int v=0;v<src_arrays.num_vertices;++v)
		{
			Colors[v].r=c.r;	
			Colors[v].g=c.g;
			Colors[v].b=c.b;
			Colors[v].alpha=255-org_arrays.colors[v].alpha;
		}
		src_arrays.colors=Colors;
		return;
	}
	else if (src_arrays.colors!=Colors)
	{
		memcpy(Colors,org_arrays.colors,(src_arrays.num_vertices)*sizeof(COLOR));
	}
	for (int v=0;v<src_arrays.num_vertices;v++)
	{
		Colors[v].alpha=255-Colors[v].alpha;
	}
	src_arrays.colors=Colors;
}
void Renderer::ALPHAGEN_LightingSpecular()
{
	if (!src_arrays.normals) return;
	if (!src_arrays.colors)
	{
		const COLOR c(VECTOR3(curr_color)*255.0f);
		for (int v=0;v<src_arrays.num_vertices;++v)
		{
			Colors[v].r=c.r;	
			Colors[v].g=c.g;
			Colors[v].b=c.b;
		}
	}
	else if (src_arrays.colors!=Colors)
	{
		memcpy(Colors,org_arrays.colors,(src_arrays.num_vertices)*sizeof(COLOR));
	}
	src_arrays.colors=Colors;

	MATRIX4 M;
	getModelview(M);
	SimpleInverse4(M);
	const VECTOR3 &pos=(VECTOR3&)M.a[3]; 
	VECTOR3 *vertices=src_arrays.vertices;
	VECTOR3 *normals=src_arrays.normals;
	VECTOR3 diff;
	float alpha;
	for (int v=0;v<src_arrays.num_vertices;v++)
	{
		diff=pos-(VECTOR3&)vertices[v];
		Normalize(diff);
        alpha=fabsf(diff*normals[v]);

		alpha*=alpha*alpha*alpha*alpha*255.0f;
		Colors[v].alpha=(unsigned char)(alpha);
	}
}
void Renderer::ALPHAGEN_Portal(float range)
{
	// would it be sufficiant to just use one alpha value for all vertices?
	// you hardly notice the difference....
	if (!src_arrays.colors)
	{
		const COLOR c(VECTOR3(curr_color)*255.0f);
		for (int v=0;v<src_arrays.num_vertices;++v)
		{
			Colors[v].r=c.r;	
			Colors[v].g=c.g;
			Colors[v].b=c.b;
		}
	}
	else if (src_arrays.colors!=Colors)
	{
		memcpy(Colors,org_arrays.colors,(src_arrays.num_vertices)*sizeof(COLOR));
	}

	src_arrays.colors=Colors;

	MATRIX4 M;
	getModelview(M);
	SimpleInverse4(M);
	const VECTOR3 &campos=(VECTOR3&)M.a[3]; 
	VECTOR3 *vertices=src_arrays.vertices;
	float alpha;
	range=256.0f/range;
	for (int v=0;v<src_arrays.num_vertices;++v)
	{
		alpha=Distance(campos,vertices[v])*range;
		if (alpha>255) alpha=255.0;
		Colors[v].alpha=(BYTE)alpha;
	}
}

void Renderer::ALPHAGEN_Wave(const CWave &wave)
{
	if (!src_arrays.colors)
	{
		const COLOR c(VECTOR3(curr_color)*255.0f);
		for (int v=0;v<src_arrays.num_vertices;++v)
		{
			Colors[v].r=c.r;	
			Colors[v].g=c.g;
			Colors[v].b=c.b;
		}
	}
	else if (src_arrays.colors!=Colors)
	{
		memcpy(Colors,org_arrays.colors,(src_arrays.num_vertices)*sizeof(COLOR));
	}
	src_arrays.colors=Colors;

	float t1=wave(g_Time);
	if (t1<0) t1=0.0f;		// man könnte hier auf PASS_SKIP entscheiden....
	else if (t1>1.0f) t1=1.0f;

	unsigned char t2=(unsigned char)(t1*255.0f);
	for (int v=0;v<src_arrays.num_vertices;v++)
	{	
		Colors[v].alpha=t2;	
	}
}

// 3rd FOG-approach:
// use an alpha-texture that contains "translucy dependend on distance", were "distance" is represented as texture coodinates
// scaling from [0.0, fogdistance] to [0.0, 1.0] is done with glScale()
// This seems to be the best approach, since I´m able to do more complex calculations, were glTexGen cannot help out
// There´s no conversion to byte, no clamping, just pure floating-point calculations
// the second texture-coordinate is set to the distance of the vertex to the fog-plane.
// 
void Renderer::TCGEN_Fog(PLANE fogplane,float fogdistance)
{
	MATRIX4 M;
	getModelview(M);

	float d1=PointDistance(g_Camera->getTransformation().getTranslation(),fogplane); 

    // to transform the world-space fogplane into model-space 
	//(X^T)^-1 is needed which is (X^-1)^T ==  (C*M)^T
	MATRIX4 P=g_Camera->getTransformation().getMatrix()*M;	// strip off camera-matrix, P then contains transformation of model- into worldspace (M^-1)
	Transpose4(P);		

	fogplane=P*fogplane; // transform fogplane into modelspace
	PLANE camplane=P*g_Camera->getClipPlane(CLIP_NEAR); // worldspace-nearplane into modelspace

	VECTOR2 *texcoords=TexCoords[texunit];
	VECTOR3	 *vertices=src_arrays.vertices;

	float d2,d3;

	if (d1<0)
	{
		// the viewer is  below the fog-surface,e.g. inside the fogvolume
		for (int v=0;v<src_arrays.num_vertices;++v)
		{
			// just plain distance to camera-plane, look most original to quake in combination with squared fogmap 
			texcoords[v].x=PointDistance(vertices[v],camplane); 
		
			/*VECTOR3 dist=src_vertices[v]-campos;
			//texcoords[v].x=Distance(src_vertices[v],campos);  //radial distance with squared fogtexture
			texcoords[v].x=dist*dist;  //squared radial distance with linear fog
			*/
			texcoords[v].y=-PointDistance(vertices[v],fogplane);
         }
	}
	else
	{
		//the viewer is above the fogsurface looking through it
		for (int v=0;v<src_arrays.num_vertices;++v)
		{
			d2=PointDistance((VECTOR3&)vertices[v],fogplane);
			texcoords[v].y=-d2;
			if (d2<0.0f)
			{
				d3=d2/(d2-d1); // measure the way that the ray traveled through the fog to the vertex
				//this is eyeplane-vertex distance
				d3*=PointDistance(vertices[v],camplane);

				texcoords[v].x=d3;
			}
			else
			{
				texcoords[v].x=0.0f;
			}	
		}
	}
	src_arrays.texcoords=texcoords;

	// manipulate the distances to the fogplane just so that a fogplane-distance of 0
	// falls still inside the fog, the corresponding texture coordinate hits 
	// a textureline>0
	// this is done so that objects that are only partly inside the fog are shown correctly
	// modify the y-size of the texture in order to 
	// change the edge-sharpness of the fog (due to filtering)

	glTranslatef(0,1.5f/(float)fogtexture->getHeight(),0);
	glScalef(1.0f/fogdistance,1.0f/fogdistance,1.0f);	// map [0.0 ,fogdistance] to [0.0 ,1.0]

}


// code ripped out of Aftershock
// (c) Steven Taylor
void Renderer::Render_ArrayElements()
{
INDEX	index=0;
INDEX	i1=0,i2=0;
bool	mode;
	
	INDEX *indices=src_arrays.indices;

	while (index < src_arrays.num_indices)
	{
		if ((i1 != indices[index]) || (i2 != indices[index+1]))
		{
			mode=TRUE;
			glEnd();
			glBegin(GL_TRIANGLE_STRIP);
			++strippedtris;
			glColor3ub(255^i1,255^i2,index); // makes strips visible
			glArrayElement(indices[index++]);
			i2=indices[index++];
			glArrayElement(i2);
			i1=indices[index++];
			glArrayElement(i1);
			gl_apicalls+=3;
			continue;
		}

		gl_apicalls++;
		if (mode)
		{
			i2 = indices[index+2];
			glArrayElement(i2);
		}
		else
		{
			i1 = indices[index+2];
			glArrayElement(i1);
		}
		index += 3;
		mode = !mode;
	}
	glEnd();
	triangles+=src_arrays.num_indices/3;
	if ((int)r_finish) glFinish();
}

void Renderer::Render_DrawElements()
{
	glDrawElements(sourcebuffer->mode,src_arrays.num_indices,GL_INDEX,src_arrays.indices);
	triangles+=src_arrays.num_indices/3; // fixme optimize this away
	if ((int)r_finish) glFinish();
	gl_apicalls++;
}

void Renderer::Render_DrawRangeElements()
{
	glDrawRangeElementsEXT(sourcebuffer->mode,0,src_arrays.num_vertices-1,src_arrays.num_indices,GL_INDEX,src_arrays.indices);
	triangles+=src_arrays.num_indices/3;
	gl_apicalls++;	
	if ((int)r_finish) glFinish();
}

void Renderer::Render_DrawArrays()
{
	
	glDrawArrays(sourcebuffer->mode,0,src_arrays.num_vertices);
	triangles+=src_arrays.num_indices/3;
	gl_apicalls++;
	if ((int)r_finish) glFinish();
}

void Renderer::ShowTris()
{
	disableMTex();

	glPushAttrib(GL_ENABLE_BIT|GL_DEPTH_BUFFER_BIT);
	glColor3ub(255,255,255);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	//glDepthFunc(GL_LEQUAL);

	if (glstate&GLSTATE_COLORARRAY)
	{
		glDisableClientState(GL_COLOR_ARRAY_EXT);
		glstate&=~GLSTATE_COLORARRAY;
	}

	glEnable(GL_POLYGON_OFFSET_LINE);
	
	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

	(this->*flushfunc)();

	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);


/*
	for (int i=0;i<src_arrays.num_indices;i+=3)
	{
		glBegin(GL_LINE_STRIP);
		glArrayElement(src_arrays.indices[i]);
		glArrayElement(src_arrays.indices[i+1]);
		glArrayElement(src_arrays.indices[i+2]);
		glArrayElement(src_arrays.indices[i]);
		glEnd();
	}
*/
	glPopAttrib();
}

void Renderer::ShowNormals()
{

	disableMTex();

	glPushAttrib(GL_ENABLE_BIT|GL_DEPTH_BUFFER_BIT);
	glColor3ub(255,255,255);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
	glDepthFunc(GL_LEQUAL);

	glBegin(GL_LINES);
	VECTOR4 v;
	VECTOR3 *vertices=src_arrays.vertices;
	VECTOR3 *normals=src_arrays.normals;

	for (int i=0;i<src_arrays.num_vertices;i++)
	{
		glArrayElement(i);
		v=vertices[i]+normals[i];
		glVertex3fv((float*)&v);
	}
	glEnd();
	glPopAttrib();
}


void Renderer::getIdentityLighting(float il[4])
{
	for(int  i=0;i<4;++i) il[i]=identitylighting[i];
}

inline void Renderer::setTexture(CTexture *texture)
{
	if (!texture) texture=whitemap;
	if (tustate[texunit].texture!=texture)
	{
		tustate[texunit].texture=texture;
		texture->bindTexture();
	}
}
inline void  Renderer::getModelview(MATRIX4 &m)
{
	glGetFloatv(GL_MODELVIEW_MATRIX,(GLfloat*)&m.m);
}

inline void Renderer::restoreSourcePointers()
{
	// these pointers can change from pass to pass
	src_arrays.texcoords=org_arrays.texcoords;
	src_arrays.colors=org_arrays.colors;
}

// disable all upper textureunits
void Renderer::disableMTex()
{
	int num_units;
	if (num_units=ogl->getNumTextureUnits())
	{
		for (int u=1;u<num_units;++u)
		{
			if (tustate[u].flags&TEXUNIT_ENABLED)
			{
				tustate[u].flags&=~TEXUNIT_ENABLED;
				glActiveTextureARB(GL_TEXTURE0_ARB+u);
				glDisable(GL_TEXTURE_2D);
			}else break;
		}
		glActiveTextureARB(GL_TEXTURE0_ARB);
		glClientActiveTextureARB(GL_TEXTURE0_ARB);
		texunit=0;
	}
}

void Renderer::mtexCB(ConVar &var)
{
	switch ((int) r_ext_multitexture)
	{
	case 2:
		if (COpenGL::Instance()->getCaps()& GLCAPS_TEXTURE_ENV_COMBINE4)
		{
			std::cout<<"using NV_texture_env_combine4"<<std::endl;
			break;
		}
		else
		{
			r_ext_multitexture=1;
			// intentional fall-through
		}
	case 1:
		if (COpenGL::Instance()->getCaps()&GLCAPS_MULTITEXTURE)
		{
			std::cout<<"using ARB_multitexture"<<std::endl;
			break;
		}
		else
		{
			std::cout<<"your gfx-card is not multitexture-capable"<<std::endl;
			r_ext_multitexture=0;
			// intentional fall-through
		}
	case 0:
		std::cout<<"multitexturing disabled"<<std::endl;
		Renderer::Instance()->disableMTex();
		break;
	}
}

void Renderer::initialize()
{

	std::cout<<std::endl<<std::endl<<"======== Renderer::initialize() ========"<<std::endl<<std::endl;
	ogl=COpenGL::Instance();

	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnableClientState(GL_VERTEX_ARRAY);

	glClearColor(0.0f, 0.0f, 0.5f, 0.0f);		
	glClearDepth(1.0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ogl->swapBuffers();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glColor4ub(255,255,255,255);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);					
	glDepthRange(0,1);

	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);

	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);

	glEnable(GL_NORMALIZE);

	glDisable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(-1,-2);

	glDisable(GL_SCISSOR_TEST);

	glDisable(GL_CLIP_PLANE0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glDisable(GL_LIGHTING);

	GLfloat scene_ambient[] =  {0.01f, 0.01f, 0.01f, 1.0f};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, scene_ambient);

	glShadeModel(GL_SMOOTH);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 0);


	glColorMaterial(GL_FRONT_AND_BACK,GL_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);

	GLfloat color_ambient[] =  {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat color_specular[] = {0.0f, 0.0f, 0.0f, 1.0f};
	GLfloat color_diffuse[] =  {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat color_shininess = 64;

	glMaterialfv(GL_FRONT, GL_AMBIENT, color_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, color_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, color_specular);
	glMaterialf(GL_FRONT, GL_SHININESS, color_shininess);

	int caps=ogl->getCaps();

	Renderer::r_ext_multitexture=0;
	if (caps&GLCAPS_MULTITEXTURE)
	{
		Renderer::r_ext_multitexture=1;
		std::cout<<"using ARB_multitexture extension"<<std::endl;
	}
	
	if (caps&GLCAPS_TEXTURE_ENV_COMBINE4)
	{
		Renderer::r_ext_multitexture=2;
		std::cout<<"using NV_texture_env_combine4 extension"<<std::endl;
	}
	Renderer::r_ext_texture_env_add=0;
	if (caps&GLCAPS_TEXTURE_ENV_ADD)
	{
		Renderer::r_ext_texture_env_add=1;
		std::cout<<"using EXT_texture_env_add extension"<<std::endl;
	}

	if (caps&GLCAPS_SWAPINTERVAL) 
	{
		std::cout<<"using WGL_EXT_swapinterval extension"<<std::endl;
	}

	Renderer::r_primitives=0;
	if (caps&GLCAPS_DRAW_RANGE_ELEMENTS)
	{
		std::cout<<"using EXT_draw_range_elements extension"<<std::endl;
		Renderer::r_primitives=2;
	}
	CTexture::r_ext_compressed_textures=0;
	if (caps&GLCAPS_S3TC)
	{
		CTexture::r_ext_compressed_textures=2;
		std::cout<<"using EXT_texture_compression_s3tc extension"<<std::endl;		
	}
	if (caps&GLCAPS_TEXTURE_COMPRESSION)
	{
		CTexture::r_ext_compressed_textures=1;
		std::cout<<"using ARB_texture_compression extension"<<std::endl;
		glHint(GL_TEXTURE_COMPRESSION_HINT_ARB,GL_NICEST);
	}

	if (caps&GLCAPS_GENERATE_MIPMAP)
	{
		std::cout<<"using SGIS_generate_mipmap extension"<<std::endl;
	}

	lastpass=NULL;
	texunit=0;
	glstate=0;

	if (caps&GLCAPS_MULTITEXTURE)
	{
		assert(MAX_TEXTURE_UNITS >= ogl->getNumTextureUnits());
		for (int u=1;u<ogl->getNumTextureUnits();u++)
		{
			glActiveTextureARB(GL_TEXTURE0_ARB+u);
			glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			glDisable(GL_TEXTURE_2D);
			tustate[0].texenv=GL_MODULATE;
			tustate[u].flags=0;
		}
		glActiveTextureARB(GL_TEXTURE0_ARB);
	}

	glEnable(GL_TEXTURE_2D);
	glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	tustate[0].texenv=GL_MODULATE;
	tustate[0].flags=TEXUNIT_ENABLED;

	// fixme: set to real overbrightsetting
	identitylighting[0]=identitylighting[1]=identitylighting[2]=0.5f;
	identitylighting[3]=1.0f;

	createWhitemap();
	prepareDLight();
	prepareSky();
	prepareFog();

	GeometryMemManager *mmgr=GeometryMemManager::Instance();
	
	Vertices=(VECTOR3*)mmgr->alloc(MT_DYNAMIC,MAX_RENDERER_VERTICES*sizeof(VECTOR3));
	Normals=(VECTOR3*)mmgr->alloc(MT_DYNAMIC,MAX_RENDERER_VERTICES*sizeof(VECTOR3));
	Colors=(COLOR*)mmgr->alloc(MT_DYNAMIC,MAX_RENDERER_VERTICES*sizeof(COLOR));
	for (int t=0;t<NUM_TCARRAYS;++t)
	{
		TexCoords[t]=(VECTOR2*)mmgr->alloc(MT_DYNAMIC,MAX_RENDERER_VERTICES*sizeof(VECTOR2));
	}
	std::cout<<std::endl<<"initialization successful"<<std::endl<<std::endl;
}

void Renderer::prepareFog()
{
	fogtexture=SpecialTextures::createFogTexture();

	fogpass.passflags=PASS_BLENDFUNC|PASS_TEXENV|PASS_FIRSTUNIT|PASS_ALPHAFUNC;
	fogpass.rgbgen=RGBGEN_NONE;
	fogpass.texenv=GL_DECAL;
	fogpass.blendsrc=GL_SRC_ALPHA;
	fogpass.blenddest=GL_ONE_MINUS_SRC_ALPHA;
	fogpass.depthfunc=GL_EQUAL;
	fogpass.anim_numframes=1;
	fogpass.anim_frames[0]=fogtexture;
	fogpass.alphafunc=GL_GREATER;
	fogpass.alphafuncref=0.05f;
}
void Renderer::createWhitemap()
{
	whitemap=SpecialTextures::createWhitemap();
}

void Renderer::prepareDLight()
{
	dlighttexture=SpecialTextures::createDLightTexture();

	dlightpass.passflags|=PASS_BLENDFUNC|PASS_DEPTHFUNC;
	dlightpass.blenddest=GL_ONE;	
	dlightpass.blendsrc=GL_DST_COLOR;
	dlightpass.depthfunc=GL_EQUAL;
	dlightpass.anim_numframes=1;
	dlightpass.anim_frames[0]=dlighttexture;
}

void Renderer::Render_DLight2(const DLIGHTFEATURE &dlightfeature)
{
	disableMTex();
	SetupPass(dlightpass);
	setTexture(dlighttexture);

	MATRIX4 M(dlightfeature.tangentspace);
	float sscale=1.0f/Length((VECTOR3&)M.a[0]);
	float tscale=1.0f/Length((VECTOR3&)M.a[1]);
	(VECTOR3&)M.a[0]*=sscale;
	(VECTOR3&)M.a[1]*=tscale;

	PLANE plane;
	
	// set two planes at the position of the light
	// one with normal= tangent T
	// one with normal= tangent B

	plane.n=(VECTOR3&)M.a[0];
	plane.d=-((VECTOR3&)M.a[0]*(VECTOR3&)dlightfeature.light->pos);
	glTexGeni(GL_S,GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGenfv(GL_S,GL_OBJECT_PLANE, (float*)&plane);
	plane.n=(VECTOR3&)M.a[1];
	plane.d=-((VECTOR3&)M.a[1]*(VECTOR3&)dlightfeature.light->pos);
	glTexGeni(GL_T,GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGenfv(GL_T,GL_OBJECT_PLANE, (float*)&plane);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	tustate[0].flags|=TEXUNIT_TCGEN;
	src_arrays.texcoords=NULL;

	SetupDynamicArrays();

	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();

	float distance=(((VECTOR3&)dlightfeature.light->pos-(VECTOR3&)M.a[3])*(VECTOR3&)M.a[2]);
	distance/=dlightfeature.light->range; // map to 0...1
	
	sscale=distance;
	if(sscale>1.0f) sscale=1.0f;
	sscale=sqrtf(1.0f-sscale*sscale)*dlightfeature.light->range;
	glTranslatef(0.5f,0.5f,0);
	glScalef(1.0f/sscale,1.0f/sscale,1.0f);

	if (distance<0.0f)
	{
		distance=0.0f; // no light, if lightsource is behind face
									  //-> this face should not have been equipped with  FEATURE_DLIGHT
		//return;
	}
	if (distance>1.0f)
	{
		distance=1.0f; // don´light if too far away
						//-> this face should not have been equipped with  FEATURE_DLIGHT
		return;
	}

	distance=1.0f-distance;
	distance*=distance; // a pointlight-source attenuates with squared distance

	for (int i=0;i<3;++i) curr_color[i]=dlightfeature.light->diffuse[i]*distance;
	//curr_color[3]=distance*0.75f;

	glColor3fv(curr_color);

	(this->*flushfunc)();
}

#define SKYBOX_TESS	9
#define SKYBOX_SIZE 2048.0f

#define MAX_NUM_SKYVERTS	 600
#define MAX_NUM_CLIPSKYVERTS 32

//order of theskybox-sides: right,up,back,left,down,front
void Renderer::prepareSky()
{
	// direction, right and up
static const VECTOR3 baseaxis[18]=
	{
		VECTOR3(1,0,0), VECTOR3(0,0,1),VECTOR3(0,1,0),			//right
		VECTOR3(0,1,0), VECTOR3(0,0,-1),VECTOR3(1,0,0), 		//top
		VECTOR3(0,0,1), VECTOR3(-1,0,0),VECTOR3(0,1,0), 		// back
		VECTOR3(-1,0,0), VECTOR3(0,0,-1),VECTOR3(0,1,0), 		//left
		VECTOR3(0,-1,0),  VECTOR3(0,0,-1),VECTOR3(-1,0,0),		//bottom
		VECTOR3(0,0,-1), VECTOR3(1,0,0)	,VECTOR3(0,1,0) 	//front
	};

	GeometryMemManager *mmgr=GeometryMemManager::Instance();
	if (skyverts) mmgr->free(skyverts);
	if (skytexcoords) mmgr->free(skytexcoords);
	if (sphereskytexcoords) mmgr->free(sphereskytexcoords);
	if (skyindices) mmgr->free(skyindices);

	const int verts_per_side=SKYBOX_TESS*SKYBOX_TESS;
	const int indices_per_side=6*(SKYBOX_TESS-1)*(SKYBOX_TESS-1);

	skyverts=(VECTOR3*)mmgr->alloc(MT_STATIC,6*verts_per_side*sizeof(VECTOR3));
	skytexcoords=(VECTOR2*)mmgr->alloc(MT_STATIC,6*verts_per_side*sizeof(VECTOR2));
	sphereskytexcoords=(VECTOR2*)mmgr->alloc(MT_STATIC,6*verts_per_side*sizeof(VECTOR2));
	skyindices=(INDEX*)mmgr->alloc(MT_SYSMEM,6*indices_per_side*sizeof(INDEX));


	for (int s=0;s<6;++s)
	{
		SkyGen::genSkyVerts(baseaxis[s*3],baseaxis[s*3+2],baseaxis[s*3+1],SKYBOX_TESS,SKYBOX_SIZE,skyverts+s*verts_per_side);
		SkyGen::genPlainSkyTexcoords(SKYBOX_TESS,256,skytexcoords+s*verts_per_side);
	}
	SkyGen::genSphereTexcoords(5,skyverts,6*verts_per_side,sphereskytexcoords);

	farboxpass.rgbgen=RGBGEN_IDENTITYLIGHTING;
	nearboxpass.rgbgen=RGBGEN_IDENTITYLIGHTING;
	nearboxpass.passflags|=PASS_BLENDFUNC|PASS_TEXENV;
	nearboxpass.blendsrc=GL_SRC_ALPHA;
	nearboxpass.blenddest=GL_ONE_MINUS_SRC_ALPHA;
	nearboxpass.texenv=GL_DECAL;
}

void Renderer::determineMinMax(VECTOR3 *verts, int num_verts)
{
// which component of the vector make up the s and t axis, the third component is used for normalizing
// -value is encoded as ~value
static const int staxis[6][3]={{2,1,0},{~2,0,1},{~0,1,2},{~2,1,0},{~2,~0,1},{0,1,2}}; 

	VECTOR3 absfacedir;
	VECTOR3 facedir=*verts;
    int side,skyside;

	int v;
	for (v=0;v<num_verts;++v) // only allow triangles
	{
		facedir+=verts[v];
	}
	
	absfacedir.x=fabsf(facedir.x);
	absfacedir.y=fabsf(facedir.y);
	absfacedir.z=fabsf(facedir.z);
	if ((absfacedir.y>absfacedir.x)&&(absfacedir.y>absfacedir.z))
	{
		side=SkyGen::SKY_TOP;
	}
	else if((absfacedir.x>absfacedir.y)&&(absfacedir.x>absfacedir.z))
	{
		side=SkyGen::SKY_RIGHT;
	}
	else side=SkyGen::SKY_BACK;

	skyside= facedir[side]>=0.0f ? side : side+3;
	usedskysides[skyside]=true;


	for (v=0;v<num_verts;++v)
	{
		VECTOR3 corner=verts[v];

		float scale=fabsf(corner[staxis[skyside][2]]);
		if (scale>0.0f) scale=1.0f/scale;
	
		side=staxis[skyside][0];
		float s=side>=0.0f?corner[side]:-corner[~side];
		s*=scale;
		side=staxis[skyside][1];
		float t=side>=0.0f?corner[side]:-corner[~side];
		t*=scale;

		if (s<skyminmax[skyside][SkyGen::SKY_MINS]) skyminmax[skyside][SkyGen::SKY_MINS]=s;
		if (s>skyminmax[skyside][SkyGen::SKY_MAXS]) skyminmax[skyside][SkyGen::SKY_MAXS]=s;
		if (t<skyminmax[skyside][SkyGen::SKY_MINT]) skyminmax[skyside][SkyGen::SKY_MINT]=t;
		if (t>skyminmax[skyside][SkyGen::SKY_MAXT]) skyminmax[skyside][SkyGen::SKY_MAXT]=t;
	}

}


// this is actually taken from Q2 source, it clips the "skypolygons" in a way they that don´t span two or more skysides

void Renderer::clipSkyPolygon(VECTOR3 *verts, int num_verts, int planenumber)
{
static const VECTOR3 clipplanes[]={VECTOR3(1,1,0),VECTOR3(1,-1,0),VECTOR3(0,-1,1),VECTOR3(0,1,1),VECTOR3(1,0,1),VECTOR3(-1,0,1)};

	if (planenumber==6)
	{
		determineMinMax(verts,num_verts);
		return;
	}

	if (num_verts>=MAX_NUM_CLIPSKYVERTS)
	{
		std::cout<<"Renderer::clipSkyPolygon() number of clipvertices>"<<MAX_NUM_SKYVERTS<<std::endl;
		return;
	}
	int	  sides[MAX_NUM_CLIPSKYVERTS+1];
	float dists[MAX_NUM_CLIPSKYVERTS+1];
	int num_backside=0;
	int num_frontside=0;

	const VECTOR3 &plane=clipplanes[planenumber];
	int v;
    for (v=0;v<num_verts;++v)
	{
		dists[v]=verts[v]*plane;
		if (dists[v]>0.1f)
		{
			sides[v]=FRONTSIDE;
			num_frontside=1;	// abuse as boolean :-)
		}
		else if (dists[v]<-0.1f)
		{
			sides[v]=BACKSIDE;
			num_backside=1;
		}
		else 
		{
			sides[v]=ON_PLANE;
		}
	}
	if (num_backside^num_frontside)
	{
		clipSkyPolygon(verts,num_verts,planenumber+1);
		return;
	}

	num_frontside=num_backside=0;

	VECTOR3 backside[MAX_NUM_CLIPSKYVERTS+2];
	VECTOR3 frontside[MAX_NUM_CLIPSKYVERTS+2];
	sides[v]=sides[0]; // clone vertex so we always have a valid v+1 vertex
	dists[v]=dists[0];
	verts[v]=verts[0];

	for (v=0;v<num_verts;++v)
	{
		switch (sides[v])
		{
		case FRONTSIDE:
			frontside[num_frontside++]=verts[v];
			break;
		case BACKSIDE:
			backside[num_backside++]=verts[v];
			break;
		case ON_PLANE:
			frontside[num_frontside++]=verts[v];
			backside[num_backside++]=verts[v];
			continue;
		}
		if ((sides[v]|sides[v+1])!=SPANNING) continue;

		float t=dists[v]/(dists[v]-dists[v+1]);
		VECTOR3 newvert=verts[v]*(1.0f-t)+verts[v+1]*t;
		frontside[num_frontside++]=newvert;
		backside[num_backside++]=newvert;

	}

	clipSkyPolygon(frontside,num_frontside,planenumber+1);
	clipSkyPolygon(backside,num_backside,planenumber+1);
}

void Renderer::Render_Sky()
{
static ConVar r_showskycoords("r_showskycoords","0");

	if (src_arrays.num_vertices>MAX_NUM_SKYVERTS)
	{
		std::cout<<"Renderer::Render_Sky() number of skyvertices: "<<src_arrays.num_vertices<<">"<<MAX_NUM_SKYVERTS<<std::endl;
		return;
	}

	// clear old bounds
	for (int i=0;i<6;++i)
	{
		skyminmax[i][SkyGen::SKY_MINS]=skyminmax[i][SkyGen::SKY_MINT]=99999.0f;
		skyminmax[i][SkyGen::SKY_MAXS]=skyminmax[i][SkyGen::SKY_MAXT]=-99999.0f;
		usedskysides[i]=false;
	}
	
	// better use the modelview matrix here!!! ( skypolygons on models... nice :-)
	const VECTOR3 &origin=g_Camera->getTransformation().getTranslation();

	VECTOR3	skyboxrays[MAX_NUM_SKYVERTS];
	VECTOR3  *vertices=src_arrays.vertices;
	for (int v=0;v<src_arrays.num_vertices;++v)
	{
		skyboxrays[v]=vertices[v]-origin;
	}
	INDEX *indices=src_arrays.indices;
	VECTOR3  skypolygon[MAX_NUM_CLIPSKYVERTS];
	for (int i=0;i<src_arrays.num_indices;i+=3)
	{
		skypolygon[0]=skyboxrays[indices[i]];
		skypolygon[1]=skyboxrays[indices[i+1]];
		skypolygon[2]=skyboxrays[indices[i+2]];
		clipSkyPolygon(skypolygon,3,0);
	}

	int all_indices=0;
	int baseindex[6][2];
	src_arrays.indices=skyindices;
	// dynamically generate indices for the currently visible sky
	for (int s=0;s<6;++s)
	{
		if (usedskysides[s])
		{
			baseindex[s][0]=all_indices;
			baseindex[s][1]=SkyGen::genSkyIndices(skyminmax[s],SKYBOX_TESS,src_arrays.indices,s*SKYBOX_TESS*SKYBOX_TESS);
			all_indices+=baseindex[s][1];		
		}
	}

	if ((int)r_showskycoords)
	{
		char names[][10]={"right","top","back","left","bottom","front"};
		for (int i=0;i<6;++i)
		{
			std::cout<<names[i]<<":"<<std::endl;
			std::cout<<"MINS: "<<skyminmax[i][SkyGen::SKY_MINS]<<" MAXS: "<<skyminmax[i][SkyGen::SKY_MAXS]<<std::endl;
			std::cout<<"MINT: "<<skyminmax[i][SkyGen::SKY_MINT]<<" MAXT: "<<skyminmax[i][SkyGen::SKY_MAXT]<<std::endl;
		}
	}

	// disable all upper textureunits
	if (ogl->getCaps()&GLCAPS_MULTITEXTURE)
	{
		for (int t=1;t<ogl->getNumTextureUnits();++t)
		{		
			if (tustate[t].flags&TEXUNIT_TCARRAY)
			{
				glClientActiveTextureARB(GL_TEXTURE0_ARB+t);
				glDisableClientState(GL_TEXTURE_COORD_ARRAY);
				tustate[t].flags&=~TEXUNIT_TCARRAY;
			}
			if (tustate[t].flags&TEXUNIT_ENABLED)
			{
				glActiveTextureARB(GL_TEXTURE0_ARB+t);
				glDisable(GL_TEXTURE_2D);
				tustate[t].flags&=~TEXUNIT_ENABLED;
			}
		}
		glClientActiveTextureARB(GL_TEXTURE0_ARB);
		glActiveTextureARB(GL_TEXTURE0_ARB);
		texunit=0;
	}

	// disable dynamic arrays
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	tustate[0].flags&=~TEXUNIT_TCARRAY;
	glDisableClientState(GL_COLOR_ARRAY);
	glstate&=~GLSTATE_COLORARRAY;

	// setup and lock vertices
	src_arrays.vertices=skyverts;
	src_arrays.normals=NULL;
	src_arrays.num_vertices=6*SKYBOX_TESS*SKYBOX_TESS;

	SetupStaticArrays();
	glLockArraysEXT(0,src_arrays.num_vertices);

	// remove translational part of view-matrix, so the skybox is always centered around the camera
	MATRIX4 M;
	getModelview(M);
	(VECTOR3&)M.a[3]=NullVector3;
		
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf((float*)&M);

	glMatrixMode(GL_TEXTURE);

	glPushAttrib(GL_VIEWPORT_BIT); // save original depthrange
	glDepthRange(1.0,1.0);	// fool GL into generating highest possible depthvalues 

	// render the farbox
	if (info.shader->farbox[0])
	{
		// setup dynamic arrays
		src_arrays.texcoords=skytexcoords;
		src_arrays.colors=NULL;	
		org_arrays=src_arrays;
        SetupDynamicArrays();
		SetupPass(farboxpass);
		
		// avoid bilerp seam
	/*	glTranslatef(0.5f,0.5f,0.0f);
		float texwidth=(float)info.shader->farbox[0]->getWidth();
		texwidth=(texwidth-1.0f)/texwidth;
		glScalef(texwidth,texwidth,1.0f);
		glTranslatef(-0.5f,-0.5f,0.0f);*/
		for (int s=0;s<6;++s)
		{
			if (usedskysides[s])
			{
				setTexture(info.shader->farbox[s]);
				src_arrays.indices=skyindices+baseindex[s][0];
				src_arrays.num_indices=baseindex[s][1];
				Render_DrawElements();
			}
		}
	}

	// because the shader needs no indices, Render() assumes it should render the vertices with glDrawArrays(),
	// it doesnt know that we create the indices on the fly.
	flushfunc=&Renderer::Render_DrawElements;

	// render the cloudlayer(s)
	if (info.shader->num_passes)
	{	
		glLoadIdentity();
		src_arrays.texcoords=sphereskytexcoords;
		src_arrays.indices=skyindices;
		src_arrays.num_indices=all_indices;
		org_arrays=src_arrays;
		if ((int)r_ext_multitexture&&(ogl->getCaps()&GLCAPS_MULTITEXTURE))
		{	
			Render_MTex(0,__min(info.shader->num_passes,(int)r_maxPasses));	// spread passes over as many texture units as available
		}
		else
		{
			Render_Generic(0,__min(info.shader->num_passes,(int)r_maxPasses)); // do not make use of singlepass-multitexturing, just use GL´s plain BlendFunc
		}
	}

	// blend the nearbox on top of the clouds
	if (info.shader->nearbox[0])
	{

		disableMTex();	
		src_arrays.texcoords=skytexcoords;
		src_arrays.colors=NULL;
		SetupDynamicArrays();
		SetupPass(nearboxpass);	

		// avoid bilerp seam
		/*glTranslatef(0.5f,0.5f,0.0f);
		float texwidth=(float)info.shader->nearbox[0]->getWidth();
		texwidth=(texwidth-1.0f)/texwidth;
		glScalef(texwidth,texwidth,1.0f);
		glTranslatef(-0.5f,-0.5f,0.0f);*/

		for (int s=0;s<6;++s)
		{
			if (usedskysides[s])
			{
				setTexture(info.shader->nearbox[s]);
				src_arrays.indices=skyindices+baseindex[s][0];
				src_arrays.num_indices=baseindex[s][1];
				Render_DrawElements();
			}
		}
	}
	
	{
		for (int i=0;i<info.num_features;++i)
		{
			if (info.features[i].name==FEATURE_FOG)
			{
				src_arrays.texcoords=sphereskytexcoords;
				src_arrays.indices=skyindices;
				src_arrays.num_indices=all_indices;
				org_arrays=src_arrays;
				Render_FogPass(*((FOGFEATURE*)info.features[i].data));
				break;
			}
		}
	}

	glPopAttrib();
}