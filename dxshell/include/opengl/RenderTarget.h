/*
This file is part of Diesel
(c) 2002-2026 by Mathias Heyer
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
#pragma once

#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#include <windows.h>

class RenderTarget
{
public:
    RenderTarget(void);
    virtual ~RenderTarget(void);

    bool makeCurrent(RenderTarget* read_target = 0);

    bool isCurrent();
    bool isCurrentRead();

    void getViewport(RECT& rect);
    void setViewport(const RECT& rect);

    bool createRenderContext(HDC devicecontext);
    void destroyRenderContext();

    HDC   getDeviceContext() const { return m_hDC; }
    HGLRC getRenderContext() const { return m_hRC; }

private:
    HGLRC m_hRC;
    HDC   m_hDC;
    bool  m_borrowedRC;
};
