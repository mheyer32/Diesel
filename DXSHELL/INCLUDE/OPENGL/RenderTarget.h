#pragma once

#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#include <windows.h>

class RenderTarget
{
public:
	RenderTarget(void);
	virtual ~RenderTarget(void);

	bool makeCurrent(RenderTarget *read_target=0);

	bool isCurrent();
	bool isCurrentRead();

	void getViewport(RECT &rect);
	void setViewport(const RECT &rect);

	bool createRenderContext(HDC devicecontext);
	void destroyRenderContext();

	HDC		getDeviceContext() const {return m_hDC;}
	HGLRC	getRenderContext() const {return m_hRC;}

private:

	HGLRC	m_hRC;
	HDC		m_hDC;
	bool	m_borrowedRC;
};
