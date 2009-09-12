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
// ShaderLoader.cpp: Implementierung der Klasse CShaderLoader.
//
//////////////////////////////////////////////////////////////////////

#include "ShaderLoader.h"

#include <iostream>

#include <misc/ParseHelper.h>
#include <file/FileManager.h>

#include "Renderer.h"
#include "Shader.h"
#include "ShaderPass.h"
#include "VideoTexture.h"
#include "WaterTexture.h"
#include "Texture.h"
#include "TextureManager.h"

#include <memorytracker.h>


//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CShaderLoader::SCRIPTCACHE CShaderLoader::s_scriptcache;
std::vector<std::string>   CShaderLoader::s_scriptfiles;


ConVar  CShaderLoader::r_compress_sky("r_compress_sky","0");

using namespace std;

CShaderLoader::CShaderLoader()
{

}

CShaderLoader::~CShaderLoader()
{

}


void CShaderLoader::cacheDirectory(const CPath &directory)
{
	std::cout<<std::endl<<"CShaderLoader::cacheDirectory() for directory '"<<directory<<"' ..."<<std::endl<<std::endl;

	PATHLIST flist;
	CFileManager *fman=CFileManager::Instance();
	std::cout<<"CShaderLoader::cacheDirectory() getting filelist...";
	fman->getFileList(flist,directory,"*.shader",false);
	std::cout<<flist.size()<<" files"<<std::endl;

	CPath temp;

	PATHITERATOR fIt=flist.begin();
	for (;fIt!=flist.end();++fIt)
	{
		if ((*fIt).getType()==CPath::PT_RELATIVE)
		{
			temp=directory/(*fIt);		
		}
		else
		{
			temp=*fIt;
		}

		// FIXME: Filter doesn´t work for pk3-contained files yet
		// so do it here
		if (temp.getExtension()=="shader")
		{
			cacheFile(temp);
		}
	}
	std::cout<<s_scriptcache.size()<<" shaders cached"<<std::endl;
}

void CShaderLoader::cacheFile(const CPath &filename)
{
	std::cout<<"    loading "<<filename<<std::endl;

	CFile *file=NULL;
	if (!(file=CFileManager::Instance()->open(filename)))
	{
		std::cout<<"CShaderLoader::cacheFile() could not open: "<<filename<<std::endl;
		return;
	}
	int filesize=file->getSize();
	char *filecontents=new char[filesize+1];
	file->readVOID(filecontents,filesize);
	delete file;

	filecontents[filesize]=0;
	//FIXME: doubly loaded materialscripts will cause double entries
	int materialfilenum=s_scriptfiles.size();
	s_scriptfiles.push_back(filename);

	try
	{
		Lexer lex;

		lex.setText(filecontents, filesize);
		lex.setCaseSensitive(false);

		char temp[512];

		while (true)
		{
			lex.skipWhiteSpace();

			lex.readUnquotedString(temp);

			// remove any extension
			char *pos=strrchr(temp,'.');
			if (pos) *pos=0;
#ifdef _DEBUG
			std::cout<<"    found shader: "<<temp<<std::endl;
#endif
			s_scriptcache[temp]=CACHEENTRY(lex.getPosition(),materialfilenum);
			lex.skipNextBlock();

		}
	}
	catch (LexException &){}

	delete[] filecontents;
}

void CShaderLoader::clearShaderCache()
{
	s_scriptcache.clear();
	s_scriptfiles.clear();
}


CShader* CShaderLoader::loadShader(const std::string &shadername)
{
	

	SCRIPTITERATOR sIt=s_scriptcache.find(shadername);
	if (sIt==s_scriptcache.end())
	{
		return NULL;
	}

	CACHEENTRY entry=sIt->second;

	CPath filename=s_scriptfiles[entry.filenumber];
	CFile *file;
	if (!(file=CFileManager::Instance()->open(filename)))
	{
		std::cout<<"CShaderLoader::loadShader() could not open: "<<filename<<std::endl;
		return 0;
	}

	int filesize=file->getSize();
	char *filecontents=new char[filesize+1];
	file->readVOID(filecontents,filesize);
	delete file;

	filecontents[filesize]=0;
	CShader	*shader=NULL;

	try
	{
		lex.setText(filecontents, filesize);
		lex.setCaseSensitive(false);

		lex.setPosition(entry.position);
#ifdef _DEBUG
		std::cout<<"parsing shader: "<<shadername<<std::endl;;
#endif
		shader=new CShader;
		shader->setName(shadername);
		ParseShader(shader);
	}
	catch (LexException &){}

	delete[] filecontents;

	return shader;
}

bool CShaderLoader::ParseShader(CShader *newshader)
{
	char line[512],s1[64],s2[64],s3[64],s4[64],s5[64],s6[64],s7[64],s8[64],s9[64],s10[64];

	newshader->setSort(0);

	lex.enterBlock();

	try
	{
		while (1) 
		{
			lex.skipWhiteSpace();
				
			if (lex.match('{'))	// found start of first pass
			{
				break;
			}

			lex.readLine(line);

			int num_args=sscanf(line,"%s %s %s %s %s %s %s %s %s %s",s1,s2,s3,s4,s5,s6,s7,s8,s9,s10);
			
			if (!num_args) continue;
		
			if (EQ(s1,"cull"))
			{
				if (EQ(s2,"none")||EQ(s2,"disable")||EQ(s2,"twosided"))
				{
					newshader->setFlags(SHADER_NOCULL,SHADER_NOCULL);
				} 
				else if (EQ(s2,"back")) //Q3 has a STRANGE convention what to call front/backside of a polygon
				{
					newshader->setFlags(SHADER_CULLFRONT,SHADER_CULLFRONT);
				}
			}
			else if (EQ(s1,"sort"))
			{   if (EQ(s2,"portal")) newshader->setSort(SORT_PORTAL);
				else if (EQ(s2,"sky")) newshader->setSort(SORT_SKY);
				else if (EQ(s2,"opaque")) newshader->setSort(SORT_OPAQUE);
				else if (EQ(s2,"banner")) newshader->setSort(SORT_BANNER);
				else if (EQ(s2,"underwater")) newshader->setSort(SORT_UNDERWATER);
				else if (EQ(s2,"additive")) newshader->setSort(SORT_ADDITIVE);
				else if (EQ(s2,"nearest")) newshader->setSort(SORT_NEAREST);
				else newshader->setSort((atoi(s2)<<28));
			}
			else if (EQ(s1,"deformvertexes"))
			{
				if (EQ(s2,"wave"))
				{
					CWave wave;
					ParseWaveForm(s4,s5,s6,s7,s8,wave);
					newshader->addVertexDeformWave((float)atof(s3),wave);
				}
				else if (EQ(s2,"normal"))
				{
					newshader->addVertexDeformNormal((float)atof(s3),(float)atof(s4));
				}
				else if (EQ(s2,"bulge"))
				{
					newshader->addVertexDeformBulge((float)atof(s3),(float)atof(s4),(float)atof(s5));
				}
				else if (EQ(s2,"move"))
				{
					CWave wave;
					ParseWaveForm(s6,s7,s8,s9,s10,wave);
					newshader->addVertexDeformMove((float)atof(s3),(float)atof(s5),-(float)atof(s4),wave);
				}
				else if (EQ(s2,"autosprite"))
				{
					newshader->addVertexDeformAutosprite();	
				}
				else if (EQ(s2,"autosprite2"))
				{
					newshader->addVertexDeformAutosprite2();
				}

			}
			else if (EQ(s1,"nomipmap")||EQ(s1,"nomipmaps")) newshader->setFlags(SHADER_NOMIPMAP|SHADER_NOPICMIP,SHADER_NOMIPMAP|SHADER_NOPICMIP);
			else if (EQ(s1,"nopicmip")) newshader->setFlags(SHADER_NOPICMIP,SHADER_NOPICMIP);
			else if (EQ(s1,"fogparms"))
			{
				newshader->setFogParams((float)atof(s3),(float)atof(s4),(float)atof(s5),(float)atof(s7));
			}
			else if (EQ(s1,"polygonoffset"))
			{
				newshader->setFlags(SHADER_POLYGONOFFSET,SHADER_POLYGONOFFSET);
			}
			else if (EQ(s1,"portal"))
			{
				newshader->setFlags(SHADER_PORTAL,SHADER_PORTAL);
			}
			else if (EQ(s1,"skyparms"))
			{
				std::cout<<"skyparms: farbox: "<<s2<<" cloudheight: "<<s3<<" nearbox: "<<s4<<std::endl;
				CTexture *nb[6];
				CTexture **nearbox=NULL;
				CTexture *fb[6];
				CTexture **farbox=NULL;
				float skyheight=128;
				if (!EQ(s2,"-"))
				{
					loadSkyTextures(s2,fb);
					farbox=fb;
				}
				if (!EQ(s3,"-"))
				{
					skyheight=(float)atof(s3);
				}
				if (!EQ(s4,"-"))
				{
					loadSkyTextures(s4,nb);
					nearbox=nb;
				}
				newshader->setSkyParams(nearbox,farbox,skyheight);
			}
		}

		if (!newshader->getSort()) newshader->setSort(SORT_NEAREST);

		// now parse all passes
		while (1)
		{
			ParsePass(newshader);
		}
	}
	catch (LexException&) {};

	lex.exitBlock();

	if (newshader->getFlags()&SHADER_FOG)
	{
		newshader->setSort(SORT_ADDITIVE-SORT_ONE);
	}
	if (newshader->getFlags()&SHADER_SKY)
	{
		newshader->setUsedArrays(VERTEXARRAY|INDEXARRAY); // don´t need more for skysahded faces
		newshader->setSort(SORT_SKY);					  // overwrite any sortkey that may have been set
		for (int p=0;p<newshader->getNumPasses();++p)
		{
			newshader->getPass(p).setDepthWrite(false);
		}
	}
	
	return true;
}

	

bool CShaderLoader::ParsePass(CShader *shader)
{
char line[512];
char s[11][512];

int texopt=0;
int pass_usedarrays=0;

	CShaderPass pass;
	pass.setRGBGen(RGBGEN_NONE);
	if (shader->getFlags()&SHADER_NOMIPMAP) texopt|=TEX_NOMIPMAP;
	if (shader->getFlags()&SHADER_NOPICMIP) texopt|=TEX_NOPICMIP;
	if (shader->getFlags()&SHADER_SKY)
	{
		//texopt|=TEX_NOMIPMAP; // skies need no mipmapping at first glance, BUT some excessive tiled
		// skies textures caused huge speed-hits (texture cache?)
		if (!(int)r_compress_sky) texopt|=TEX_DONTCOMPRESS;
	}


	lex.enterBlock();
	
	try
	{
		while(true) 
		{
			lex.skipWhiteSpace();
			lex.readLine(line);
			int num_args=sscanf(line,"%s %s %s %s %s %s %s %s %s %s %s",s[0],s[1],s[2],s[3],s[4],s[5],s[6],s[7],s[8],s[9],s[10]);
			
			if (!num_args) continue;

			if (EQ(s[0],"map")||EQ(s[0],"clampmap"))
			{
				CTexture *tex=NULL;

				if (EQ(s[0],"clampmap")) {texopt|=TEX_CLAMP;}
				if (EQ(s[1],"$lightmap"))
				{
					pass.setTCGen(TCGEN_LIGHTMAP);
					pass.setLightmapPass();
				}
				else if (EQ(s[1],"*white")||EQ(s[1],"$whiteimage"))
				{
					tex=CTextureManager::Instance()->findOrLoadTexture("$whiteimage");
					if (tex)
					{
						pass.addTexture(tex);
					}
					else
					{
						std::cout<<"CShaderLoader::ParsePass() missing texture $whiteimage"<<std::endl;
					}
				}
				else
				{
					tex=CTextureManager::Instance()->findOrLoadTexture(s[1],texopt);
					if (tex)
					{
						pass.addTexture(tex);
					}
					else
					{
						std::cout<<"CShaderLoader::ParsePass() missing texture "<<s[1]<<std::endl;
					}		
				}
			}
			else if (EQ(s[0],"animmap"))
			{
				CTexture *tex=NULL;
				pass.setFPS((float)atof(s[1]));
				num_args-=2;
				for (int t=0;t<num_args;++t)
				{
					tex=CTextureManager::Instance()->findOrLoadTexture(s[t+2]);
					if (tex)
					{
						pass.addTexture(tex);
					}
					else
					{
						std::cout<<"CShaderLoader::ParsePass() missing texture "<<s[t+2]<<"\n in shader: "<<shader->getName()<<std::endl;
					}
				}
			}
			else if (EQ(s[0],"videomap"))
			{
				CVideoTexture *vidtex=CTextureManager::Instance()->findOrLoadVideoTexture(s[1]);
				if (vidtex)
				{
					pass.addTexture(vidtex);
					vidtex->play();
				}
			}
			else if (EQ(s[0],"watermap"))
			{
				CWaterTexture *tex=new CWaterTexture;
				if (CTextureManager::Instance()->loadTexture(tex,s[2],texopt))
				{
					pass.addTexture(tex);
				}
				else
				{
					KILLOBJECT(tex);
					std::cout<<"CShaderLoader::ParsePass() cannot open watertexture\n    "<<s[2]<<std::endl;
				}
			}
			else if(EQ(s[0],"blendfunc"))
			{	
				if(EQ(s[1],"add")) pass.setBlendFunc(GL_ONE,GL_ONE);
				else if(EQ(s[1],"filter")) pass.setBlendFunc(GL_ZERO,GL_SRC_COLOR);
				else if(EQ(s[1],"blend")) pass.setBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
				else
				{
					pass.setBlendFunc(ParseBlendFunc(s[1]), ParseBlendFunc(s[2]) );
				}
			}
			else if (EQ(s[0],"rgbgen"))
			{
				if(EQ(s[1],"identity"))					pass.setRGBGen(RGBGEN_IDENTITY);
				else if(EQ(s[1],"identitylighting"))	pass.setRGBGen(RGBGEN_IDENTITYLIGHTING);
				else if(EQ(s[1],"entity"))				pass.setRGBGen(RGBGEN_ENTITY); //FIXME: wie kann man das "Entity modulate field" einbauen?
				else if(EQ(s[1],"one_minus_entity"))	pass.setRGBGen(RGBGEN_ONEMINUSENTITY);
				else if(EQ(s[1],"vertex"))				pass.setRGBGen(RGBGEN_VERTEX);
				else if(EQ(s[1],"exactvertex"))			pass.setRGBGen(RGBGEN_EXACTVERTEX);
				else if(EQ(s[1],"one_minus_vertex"))	pass.setRGBGen(RGBGEN_ONEMINUSVERTEX);
				else if(EQ(s[1],"flare"))				pass.setRGBGen(RGBGEN_FLARE);
				else if(EQ(s[1],"lightingdiffuse"))		pass.setRGBGen(RGBGEN_LIGHTINGDIFFUSE);
				else if (EQ(s[1],"wave"))
					{
						CWave wave;
						ParseWaveForm(s[2],s[3],s[4],s[5],s[6],wave);
						pass.setRGBGenWave(wave);
					}
				else if (EQ(s[1],"const"))
					{
						pass.setRGBGenConst((float)atof(s[3]),(float)atof(s[4]),(float)atof(s[5]));
					}
			}
			else if (EQ(s[0],"alphagen"))
			{
				if(EQ(s[1],"identity"))					pass.setAlphaGen(ALPHAGEN_IDENTITY);
				else if(EQ(s[1],"identitylighting"))	pass.setAlphaGen(ALPHAGEN_IDENTITYLIGHTING);
				else if(EQ(s[1],"entity"))				pass.setAlphaGen(ALPHAGEN_ENTITY);
				else if(EQ(s[1],"one_minus_entity"))	pass.setAlphaGen(ALPHAGEN_ONEMINUSENTITY);
				else if(EQ(s[1],"vertex"))				pass.setAlphaGen(ALPHAGEN_VERTEX);
				else if(EQ(s[1],"one_minus_vertex"))	pass.setAlphaGen(ALPHAGEN_ONEMINUSVERTEX);
				else if(EQ(s[1],"lightingdiffuse"))		pass.setAlphaGen(ALPHAGEN_LIGHTINGDIFFUSE); 
				else if(EQ(s[1],"lightingspecular"))	pass.setAlphaGen(ALPHAGEN_LIGHTINGSPECULAR);
				else if (EQ(s[1],"wave"))
					{
						CWave wave;
						ParseWaveForm(s[2],s[3],s[4],s[5],s[6],wave);
						pass.setAlphaGenWave(wave);
					}
				else if(EQ(s[1],"portal"))
				{
					float range=256.0f;
					if (num_args>2) range=atof(s[2]);
					pass.setAlphaGenPortal(range);
				}
			}
			else if (EQ(s[0],"tcgen"))
			{
				if (EQ(s[1],"base"))				pass.setTCGen(TCGEN_NONE);
				else if (EQ(s[1],"environment"))	pass.setTCGen(TCGEN_ENVIRONMENT);
				else if (EQ(s[1],"lightmap"))		pass.setTCGen(TCGEN_LIGHTMAP);
				else if (EQ(s[1],"spheremap"))		pass.setTCGen(TCGEN_SPHEREMAP);
				else if (EQ(s[1],"vector"))
				{
					pass.setTCGenVector(VECTOR3(atof(s[3]),atof(s[5]),-atof(s[4])),
										VECTOR3(atof(s[8]),atof(s[10]),-atof(s[9])));
				}
		   	}
			else if (EQ(s[0],"tcmod"))
			{
				if (EQ(s[1],"rotate"))	pass.addTCModRotate((float)atof(s[2]));
				else if (EQ(s[1],"scale")) pass.addTCModScale((float)atof(s[2]),(float)atof(s[3]));
				else if (EQ(s[1],"scroll")) pass.addTCModScroll((float)atof(s[2]),(float)atof(s[3]));
				else if (EQ(s[1],"stretch"))
				{
					CWave wave;
					ParseWaveForm(s[2],s[3],s[4],s[5],s[6],wave);
					pass.addTCModStretch(wave);
				}
				else if (EQ(s[1],"transform"))
				{
					pass.addTCModTransform((float)atof(s[2]),(float)atof(s[3]),(float)atof(s[4]),(float)atof(s[5]),(float)atof(s[6]),(float)atof(s[7]));
				}
				else if (EQ(s[1],"turb"))
				{
					pass.addTCModTurb((float)atof(s[2]),(float)atof(s[3]),(float)atof(s[4]),(float)atof(s[5]));
				}
			}
			else if (EQ(s[0],"depthfunc"))
			{
				if (EQ(s[1],"equal"))
				{
					pass.setDepthFunc(GL_EQUAL);
				}
				else pass.setDepthFunc(GL_LEQUAL);
			}
			else if (EQ(s[0],"alphafunc"))
			{
				if (EQ(s[1],"gt0"))			pass.setAlphaFunc(GL_GREATER,0);
				else if (EQ(s[1],"lt128"))	pass.setAlphaFunc(GL_LESS,0.5);
				else if (EQ(s[1],"ge128"))	pass.setAlphaFunc(GL_GEQUAL,0.5);
			}
			else if (EQ(s[0],"depthwrite"))	pass.setDepthWrite(true);
			else if (EQ(s[0],"detail"))		pass.setDetailPass();
			else
			{
				std::cout<<"unknown shader keyword: "<<s[0]<<std::endl;
			}
		} 
	}
	catch (LexException&) {}

	shader->addPass(pass);

	lex.exitBlock();

	return true;	
}

GLenum CShaderLoader::ParseBlendFunc(const char* string)
{
	if (EQ(string,"gl_one")) return GL_ONE;
	if (EQ(string,"gl_zero")) return GL_ZERO;
	if (EQ(string,"gl_dst_color")) return GL_DST_COLOR;
	if (EQ(string,"gl_one_minus_dst_color")) return GL_ONE_MINUS_DST_COLOR;
	if (EQ(string,"gl_src_alpha")) return GL_SRC_ALPHA;
	if (EQ(string,"gl_one_minus_src_alpha")) return GL_ONE_MINUS_SRC_ALPHA;
	if (EQ(string,"gl_src_color")) return GL_SRC_COLOR;
	if (EQ(string,"gl_one_minus_src_color")) return GL_ONE_MINUS_SRC_COLOR;
	if (EQ(string,"gl_dst_alpha")) return GL_DST_ALPHA;
	if (EQ(string,"gl_one_minus_dst_alpha")) return GL_ONE_MINUS_DST_ALPHA;
	if (EQ(string,"gl_src_alpha_saturate")) return GL_SRC_ALPHA_SATURATE;
	return GL_ONE;
}

void CShaderLoader::ParseWaveForm(const char* func, const char* base, const char* amplitude, const char* phase, const char* freq, CWave &wavefunc)
{
	wavefunc.setParams(CWave::getWaveForm(func),(float)atof(base),(float)atof(amplitude),(float)atof(phase),(float)atof(freq));
}

void CShaderLoader::loadSkyTextures(const char *skybasename, CTexture  **textures)
{
	char extens[6][8]={"_lf.tga","_up.tga","_bk.tga","_rt.tga","_dn.tga","_ft.tga"};
	char filename[MAX_PATH];

	strcpy(filename,skybasename);
	char *ext=filename+strlen(filename);

	for (int t=0;t<6;t++)
	{
		strcpy(ext,extens[t]);
		int options=TEX_NOMIPMAP|TEX_CLAMP|TEX_FLIPY;
		if (!(int)r_compress_sky) options|=TEX_DONTCOMPRESS;
		textures[t]=CTextureManager::Instance()->findOrLoadTexture(filename,options);
	}
}
