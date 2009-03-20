
/*
This file is part of DXShell
(c) 2002 by Mathias Heyer

DXShell is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

DXShell is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
// OpenGL.h: Schnittstelle für die Klasse COpenGL.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPENGL_H__1C0DD0C0_CB86_11D3_AF5D_0080AD17B2BB__INCLUDED_)
#define AFX_OPENGL_H__1C0DD0C0_CB86_11D3_AF5D_0080AD17B2BB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma comment( lib, "opengl32.lib" ) // bad if the dll should be manually loaded

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <GL/gl.h>

#include <adt/ConVar.h>
#include <adt/ConCmd.h>
#include <adt/Singleton.h>
#include <Messaging/MessagingObject.h>

#undef  GL_EXT_TYPED
#include "GLFuncs.h"
#include "OGLErrors.h"

#ifdef _DEBUG
extern void GLERROR(const char *text);
#else
#define GLERROR(text)
#endif


#define GLCAPS_COMPILED_VERTEX_ARRAY	(1<<0)
#define GLCAPS_MULTITEXTURE				(1<<1)
#define GLCAPS_POINT_PARAMETERS			(1<<2)
#define GLCAPS_SWAPINTERVAL				(1<<3)
#define GLCAPS_3DFX_GAMMA_CONTROL		(1<<4)
#define GLCAPS_DRAW_RANGE_ELEMENTS		(1<<5)
#define GLCAPS_TEXTURE_ENV_ADD			(1<<6)
#define GLCAPS_TEXTURE_ENV_COMBINE		(1<<7)
#define GLCAPS_TEXTURE_ENV_COMBINE4		(1<<8)
#define GLCAPS_GENERATE_MIPMAP			(1<<9)
#define GLCAPS_TEXTURE_COMPRESSION		(1<<10)
#define GLCAPS_VERTEX_ARRAY_RANGE		(1<<11)
#define GLCAPS_VERTEX_ARRAY_RANGE2		(1<<12)
#define GLCAPS_S3TC						(1<<13)


class RenderTarget;

class CWindow;

class COpenGL: protected Msg::MessagingObject, public Singleton<COpenGL>
{

DECLARE_SINGLETON(COpenGL)

public:

struct GLPIXELFORMAT
{
	int colorbits;
	int zbits;
	int stencilbits;
	int alphabits;
	int multisamples;	// number of samples if multisampling is required
	int hertz;
	GLPIXELFORMAT()
	{
		ZeroMemory(this,sizeof(GLPIXELFORMAT));
	}
};

	bool InitOpenGL(CWindow *window,int width,int height,bool fullscreen=false,const GLPIXELFORMAT &format=GLPIXELFORMAT());

	bool InitOpenGL(CWindow *window,GLPIXELFORMAT format=GLPIXELFORMAT());
	void ShutDownOpenGL();

	RenderTarget* getRenderTarget();
	bool makeCurrent(RenderTarget *read_target=0);

	bool EnableWindowed();
	bool EnableFullscreen();

	bool SwitchToWindowed();
	bool SwitchToFullscreen();

	void InfoBox();

	int getStencilBits() const;
	int getColorBits() const;
	int getScreenHeight() const;
	int getScreenWidth() const;

	bool isExtensionSupported(LPCSTR extension);
	bool isWGLExtensionSupported(LPCSTR extension);

	const char * getExtensions();
	const char * getVendor();
	const char * getRenderer();
	const char * getVersion();
	const char * getWGLExtensions(); // may return NULL !!!!
	 
	inline int getNumTextureUnits() const{return m_num_TextureUnits;};
	inline void swapBuffers() const{::SwapBuffers(m_hDC);};

	void setGamma(double gamma,int overbrightbits);
	void setGammaRamp(WORD *gammaramp);

	HDC	getDeviceContext() const {return m_hDC;};

	void checkCapabilities();
	inline int getCaps() const {return m_caps;};

	//FIXME: should these be real members?
	// making them static allows scripts to be executed and cvars to be set 
	//even before the opengl instance gets initialized
	static ConVar	gl_renderer;
	static ConVar	gl_vendor;
	static ConVar	gl_extensions;
	static ConVar	gl_version;

	static ConVar	r_colorbits;
	static ConVar	r_stencilbits;
	static ConVar	r_depthbits;
	static ConVar	r_fullscreen;
	static ConVar	r_customwidth;
	static ConVar	r_customheight;
	static ConVar	r_displayRefresh;

	static ConVar	r_overBrightBits;
	static ConVar	r_gamma;
	static ConVar	r_FSAA;
	static ConVar	r_ignore;

private:

	COpenGL();
	 ~COpenGL();

	bool setWGLPixelFormat(const GLPIXELFORMAT &format);
	bool setStandardPixelFormat(const GLPIXELFORMAT &format);

	bool getDeviceContext();		// get DC from window and use it for GL
	void releaseDeviceContext();

	bool createRenderContext();
	void releaseRenderingContext();

	void bindExtensions();

	virtual	 Msg::MSGRVAL handleMessage(Msg::MessagingObject *sender, Msg::MESSAGEID msgId, const Msg::Param &parameters);

	void getCurrentScreenMode();
	void saveCurrentGamma();
	void restoreGamma();
	
	static void vid_restartCB(ConCmd &Command,const std::string &Arg);
	static void changegamma(ConVar &cvar);
	static void changeFSAA(ConVar &cvar);

	int				m_num_TextureUnits;

	HDC				m_hDC;

	RenderTarget	*m_rendertarget;
	CWindow			*m_glwindow;

	GLPIXELFORMAT	m_pixelformat;

	int				m_screenwidth;
	int				m_screenheight;
	int				m_hertz;

	bool			m_run_fullscreen;

	unsigned short	m_old_gammaramp[3*256];
	unsigned short	m_gammaramp[3*256];

	int				m_caps;

	std::string		m_extensions;
	std::string		m_wglextensions;

	bool			m_fully_initialized;
};

#endif // !defined(AFX_OPENGL_H__1C0DD0C0_CB86_11D3_AF5D_0080AD17B2BB__INCLUDED_)
