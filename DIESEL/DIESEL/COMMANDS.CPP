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

#include <file/FileManager.h>
#include "ShaderLoader.h"
#include <file/Path.h>
#include <win/appwindow.h>
#include <misc/exception.h>
#include <misc/parsehelper.h>
#include <dinput/directinput.h>
#include <dinput/mouse.h>
#include <dinput/keyboard.h>

#include "adt/ConCmd.h"
#include "adt/ConVar.h"
#include "Engine.h"
#include "camera.h"
#include "TextureManager.h"
#include <opengl/opengl.h>
#include "q3bspmesh.h"
#include "q3scene.h"
#include "renderer.h"
#include "vertexbuffer.h"
#include "entity.h"

#include <memorytracker.h>


void exec(ConCmd &command,const std::string &args);
void move(ConCmd &command,const std::string &args);
void fov(ConVar &command);
void vid_restart(ConCmd &command,const std::string &args);
void swapinterval(ConVar &command);
void lookbehind(ConCmd &command,const std::string &args);
void strip(ConCmd &command,const std::string &Args);
void entitylist(ConCmd &command,const std::string &Args);
void meshlist(ConCmd &command,const std::string &Args);
void vstr(ConCmd &command,const std::string &Args);
void sys_memoryReport(ConCmd &command,const std::string &Args);

void cb_addPaks(ConCmd &command,const std::string &args);
void cb_quit(ConCmd &command,const std::string &args);

ConCmd cmd_exec("exec",exec);
CAliasCmd cmd_seta("seta","set");
ConCmd	cmd_forward("forward",move);
ConCmd	cmd_back("back",move);
ConCmd	cmd_moveleft("moveleft",move);
ConCmd	cmd_moveright("moveright",move);
ConCmd	cmd_moveup("moveup",move);
ConCmd	cmd_movedown("movedown",move);
ConCmd cmd_vid_restart("vid_restart",vid_restart);
ConCmd	cmd_addPaks("addPaks",cb_addPaks);

ConCmd	cmd_lookbehind("lookbehind",lookbehind);

ConCmd	cmd_strip("mesh_optimize",strip);

ConCmd	cmd_entitylist("entitylist",entitylist);
ConCmd cmd_meshlist("meshlist",meshlist);

ConCmd cmd_vstr("vstr",vstr);

ConCmd cmd_sys_memoryReport("sys_memoryReport",sys_memoryReport);

ConCmd cmd_quit("quit",cb_quit);

ConVar	r_swapInterval("r_swapInterval","0",0,swapinterval);
ConVar   cg_fov("cg_fov","100",0,fov);
ConVar	speed("speed","300");
ConVar	noclip("noclip","0");

void exec(ConCmd &Command,const std::string &Arg)
{
	if (Arg.empty())
	{ 
		std::cout<<"no filename given"<<std::endl;
		return;
	}

	CFile *file=CFileManager::Instance()->open(Arg.c_str());
	if (!file)
	{
		std::cout<<"could not open file"<<std::endl;
		return;
	}

	std::cout<<"execing "<<Arg<<std::endl;

	int size=file->getSize();
	char *buffer=new char[size+1];
	file->readVOID(buffer,size);
	buffer[size]=0;
	char *pos=buffer;
	char *nextline;
	while (nextline=ParseHelper::FindNextLine(pos))
	{
		ConCmd::executeCommandLine(std::string(pos,nextline-pos));
		pos=nextline;
	}
	KILLARRAY(buffer);

	file->close();
	KILLOBJECT(file);
}
void move(ConCmd &command,const std::string &args)
{
	float mspeed=(float)speed;
	VECTOR3 pos=g_Camera->getTransformation().getTranslation();
	VECTOR3 move,newpos;
	if (&command==&cmd_forward)
	{
		move=-g_Camera->getTransformation().getColumnVector(2);
	}
	else if (&command==&cmd_back)
	{
		move=g_Camera->getTransformation().getColumnVector(2);
	}
	else if (&command==&cmd_moveleft)
	{
		move=-g_Camera->getTransformation().getColumnVector(0);
	}
	else if (&command==&cmd_moveright)
	{
		move=g_Camera->getTransformation().getColumnVector(0);
	}
	else if (&command==&cmd_moveup)
	{
		move=g_Camera->getTransformation().getColumnVector(1);
	}
	else if (&command==&cmd_movedown)
	{
		move=-g_Camera->getTransformation().getColumnVector(1);
	}

	move*=mspeed*g_SecsPerFrame;
	// move is the original unit-length vector that represents the wanted movement of the camera
	newpos=pos+move;

	Q3Scene *q3scene=dynamic_cast<Q3Scene*>(g_Scene);
	Q3BSPMesh *worldspawn=NULL;

	if (q3scene) worldspawn=q3scene->getWorldModel();

	if (worldspawn&&!(int)noclip)
	{
		Q3BSPMesh::TRACE trace;
		trace.contents=CONTENTS_PLAYERCLIP|CONTENTS_SOLID;
		trace.bevel=10.0f;
		int iterations=0;

		// test a sphere swept along the movement
		while (!worldspawn->RayTest(pos,newpos,trace))
		{
			if (trace.allsolid)
			{
				std::cout<<"allsolid\n ";
				break;
			}
			if (trace.startsolid)
			{
				std::cout<<"startsolid, ";
				break;
			}
			//std::cout<<"camera collided"<<std::endl;
			trace.contents=CONTENTS_SOLID|CONTENTS_PLAYERCLIP;
			const PLANE &plane=worldspawn->getPlane(worldspawn->getBrushside(trace.brushside).plane);
			move-=plane.n*((move*plane.n)*1.005f);

			if (Length(move)<PLANE_EPSILON) break;

			newpos=pos+move;	
/*			if (iterations>100)
			{
				std::cout<<"camera got stuck"<<std::endl;
				break;
			}*/
			iterations++;
		}
	}
	
	g_Camera->getTransformation().setTranslation(newpos);
}

void fov(ConVar &command)
{
	g_Camera->setupFrustum((float)command,g_Camera->Aspect,g_Camera->NearZ,g_Camera->FarZ);
}

// not working
void vid_restart(ConCmd &Command,const std::string &Arg)
{
	COpenGL *ogl=COpenGL::Instance();
	
	ogl->ShutDownOpenGL();

	CDirectInput::Instance()->ShutDownDirectInput();
	
	CAppWindow::Instance()->close();
	if (!CAppWindow::Instance()->open())
	{
		throw CException("vid_restart: could not reopen window");
	}

//	CDirectInput::Instance()->InitDirectInput();

	/*CKeyboard *kb=CKeyboard::hasInstance();
	if (kb) kb->reinitialize();
	CMouse	*mouse=CMouse::hasInstance();
	if (mouse) mouse->reinitialize();*/

	ConVar *r_customwidth=ConVar::findCVar("r_customwidth");
	ConVar *r_customheight=ConVar::findCVar("r_customheight");
	ConVar *r_fullscreen=ConVar::findCVar("r_fullscreen");
	ConVar *r_colorbits=ConVar::findCVar("r_colorbits");
	ConVar *r_stencilbits=ConVar::findCVar("r_stencilbits");

	COpenGL::GLPIXELFORMAT format;
	format.colorbits=(int)*r_colorbits;
	format.stencilbits=(int)*r_stencilbits;

	if (!ogl->InitOpenGL(CAppWindow::Instance(),ogl->getScreenWidth(),ogl->getScreenHeight(),(bool)*r_fullscreen,format))
	{
		throw CException("vid_restart: could not initialize OpenGL");
	}
	
	Renderer::Instance()->initialize();

	CTextureManager::Instance()->restoreAllTextures();
}

void swapinterval(ConVar &command)
{
	if (wglSwapIntervalEXT)
	{
		wglSwapIntervalEXT((int)command);
	}
}

void cb_addPaks(ConCmd &command,const std::string &args)
{
	CPath searchpath(args);
	CFileManager *fman=CFileManager::Instance();
	if (searchpath.getType()==CPath::PT_RELATIVE)
	{
		searchpath=CPath((std::string)fs_basepath)/searchpath;
	}
	fman->addSearchPath(searchpath);
	fman->clearPakFileCache();
	fman->cachePakFiles("");
	CShaderLoader::cacheDirectory("scripts");
}


void lookbehind(ConCmd &command,const std::string &args)
{
	glMatrixMode(GL_PROJECTION);
	glScalef(1,1,-1);
	glMatrixMode(GL_MODELVIEW);
}

void strip(ConCmd &command,const std::string &Args)
{
	CMesh::MESHLIST list;
	CMesh::MESHITERATOR m;

	list=CMesh::GetMeshList();
	for (m=list.begin();m!=list.end();m++)
	{
		(*m)-> optimize();
	}
}

void entitylist(ConCmd &command,const std::string &Args)
{
	std::cout<<"all currently instantiated entities"<<std::endl;
	CEntity::ENTITYLIST entlist=CEntity::GetEntityList();
	CEntity::ENTITYITERATOR e=entlist.begin();
	for (;e!=entlist.end();++e)
	{
		std::cout<<"    "<<(*e)->getName()<<"  rc: "<<(*e)->getRefCount()<<std::endl;
	}
	std::cout<<entlist.size()<<" entities"<<std::endl;
}
void meshlist(ConCmd &command,const std::string &Args)
{
	std::cout<<"all currently instantiated meshes"<<std::endl;
	CMesh::MESHLIST meshlist=CMesh::GetMeshList();
	CMesh::MESHITERATOR m=meshlist.begin();
	for (;m!=meshlist.end();++m)
	{
		std::cout<<"    "<<(*m)->getName()<<"  rc: "<<(*m)->getRefCount()<<std::endl;
	}
	std::cout<<meshlist.size()<<" meshes"<<std::endl;
}

void vstr(ConCmd &command,const std::string &Args)
{
	ConVar *var=ConVar::findCVar(Args);
	if (!var)
	{
		std::cout<<"'"<<Args<<"' is not a variable"<<std::endl;
		return;
	}
	ConCmd::executeCommandLine(var->getString());
}

void sys_memoryReport(ConCmd &command,const std::string &Args)
{
#ifdef	_H_MMGR
	sMStats	stats=m_getMemoryStatistics();
	// Report summary

	cout<<"--------------------------------------------------------"<<endl;
	cout<<"|                     T O T A L S                      |"<<endl;
	cout<<"--------------------------------------------------------"<<endl;
	cout<<"Allocation unit count: "<<stats.totalAllocUnitCount<<endl;
	cout<<"Reported to application: "<<stats.totalReportedMemory<<endl;
	cout<<"Actual total memory in use: "<<stats.totalActualMemory<<endl;
	cout<<"Memory tracking overhead: "<<stats.totalActualMemory - stats.totalReportedMemory<<endl;
	cout<<endl;

	cout<<"--------------------------------------------------------"<<endl;
	cout<<"|                      P E A K S                       |"<<endl;
	cout<<"--------------------------------------------------------"<<endl;
	cout<<"   Allocation unit count: "<<stats.peakAllocUnitCount<<endl;
	cout<<" Reported to application: "<<stats.peakReportedMemory<<endl;
	cout<<"                  Actual: "<<stats.peakActualMemory<<endl;
	cout<<"Memory tracking overhead: "<<stats.peakActualMemory - stats.peakReportedMemory<<endl;
	cout<<endl;

	cout<<"-------------------------------------------------------"<<endl;
	cout<<"|                  A C C U M U L A T E D              |"<<endl;
	cout<<"-------------------------------------------------------"<<endl;
	cout<<"  Allocation unit count: "<<stats.accumulatedAllocUnitCount<<endl;
	cout<<"Reported to application: "<<stats.accumulatedReportedMemory<<endl;
	cout<<"                 Actual: "<<stats.accumulatedActualMemory<<endl;
	cout<<endl;

	cout<<"-------------------------------------------------------"<<endl;
	cout<<"|                      U N U S E D                    |"<<endl;
	cout<<"-------------------------------------------------------"<<endl;
	cout<<"Memory allocated but not in use: "<< m_calcAllUnused()<<endl;
	cout<<endl;

	cout<<"writing memory report file..."<<endl;
	m_dumpMemoryReport("memory_report", true);

#endif

	cout<<endl;

	GeometryMemManager *mmgr=GeometryMemManager::Instance();

	cout<<"     free static memory : "<<mmgr->getStaticManager()->getFreeMemorySize()<<endl;
	cout<<"allocated static memory : "<<mmgr->getStaticManager()->getAllocatedMemorySize()<<endl;
	cout<<"alloacted memory blocks : "<<mmgr->getStaticManager()->getBlockCount()<<endl<<endl;
	cout<<"     free dynamic memory: "<<mmgr->getDynamicManager()->getFreeMemorySize()<<endl;
	cout<<"allocated dynamic memory: "<<mmgr->getDynamicManager()->getAllocatedMemorySize()<<endl;
	cout<<"alloacted memory blocks : "<<mmgr->getDynamicManager()->getBlockCount()<<endl<<endl;
	cout<<"     free system memory : "<<mmgr->getSysManager()->getFreeMemorySize()<<endl;
	cout<<"allocated system memory : "<<mmgr->getSysManager()->getAllocatedMemorySize()<<endl;
	cout<<"alloacted memory blocks : "<<mmgr->getSysManager()->getBlockCount()<<endl<<endl;
}

void cb_quit(ConCmd &command,const std::string &args)
{
	PostQuitMessage(0);
}