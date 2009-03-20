#include ".\rendertarget.h"

#include <iostream>
#include <cassert>

#include <OpenGL/OpenGL.H>
#include <win/WinException.H>
#include <win/WinErr.h>

RenderTarget::RenderTarget(void)
{
	m_hRC=0;
	m_hDC=0;
	m_borrowedRC=false;
}

RenderTarget::~RenderTarget(void)
{
	destroyRenderContext();
}

bool RenderTarget::createRenderContext(HDC devicecontext)
{
	assert(devicecontext);
	std::cout<<"creating rendering context...";

	if (m_hDC || m_hRC)
	{
		throw CException("RenderTarget::createRenderContext() tried to create rendercontext more than once");
	}

	m_hDC=devicecontext;
	
	m_borrowedRC=true;
	HGLRC glRC=wglGetCurrentContext();	// in this case, just reuse the existing context

	if (!glRC)
	{
		m_hRC=wglCreateContext(m_hDC);	// create a new one
		m_borrowedRC=false;
	}
	else
	{
		m_hRC=glRC;
	}

	if (!m_hRC)
	{
		std::cout<<"failed"<<std::endl;
		throw CWinException("RenderTarget::createRenderContext() wglCreateContext() failed");
	}
	std::cout<<"successful"<<std::endl;

	return true;
}

void RenderTarget::destroyRenderContext()
{
	if (m_hRC)
	{
		if (isCurrent())
		{
			wglMakeCurrent( NULL, NULL );
		}
		if (!m_borrowedRC)	wglDeleteContext(m_hRC);
	}
	m_hRC=0;
	m_hDC=0;
}

bool RenderTarget::makeCurrent(RenderTarget *read_target)
{
	BOOL rval;
	if (!read_target)
	{
		if (wglMakeContextCurrentARB && wglGetCurrentContext())
		{
			rval=wglMakeContextCurrentARB(m_hDC, m_hDC, m_hRC);
		}
		else
		{
			rval=wglMakeCurrent(m_hDC,m_hRC);
		}
	}
	else
	{
		if (!wglMakeContextCurrentARB) throw CException("RenderTarget::makeCurrent() WGL_ARB_make_current_read not supported");
		rval=wglMakeContextCurrentARB(m_hDC,read_target->getDeviceContext(),m_hRC);
	}
	if (!rval)
	{
		throw CWinException("RenderTarget::makeCurrent() failed, because: \n");
		std::cout<<WinErrorToString()<<std::endl;
	}
	return (bool) rval;
}


bool RenderTarget::isCurrent()
{
	HDC hDC=wglGetCurrentDC();
	return hDC==m_hDC;
}

bool RenderTarget::isCurrentRead()
{
	if (!wglGetCurrentReadDCARB) throw CException("RenderTarget::makeCurrent() WGL_ARB_make_current_read not supported");
	HDC hDC=wglGetCurrentReadDCARB();
	return hDC==m_hDC;
}

void RenderTarget::getViewport(RECT &rectangle)
{
	assert(0);
}

void RenderTarget::setViewport(const RECT &rectangle)
{
	DWORD W,H;

	W=rectangle.right-rectangle.left+1;
	H=rectangle.bottom-rectangle.top+1;

	glViewport(0, 0, W, H);	
}