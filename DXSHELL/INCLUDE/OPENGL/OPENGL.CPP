
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
// OpenGL.cpp: Implementierung der Klasse COpenGL.
//
//////////////////////////////////////////////////////////////////////

#include "OpenGL.h"

#include <misc/exception.h>
#include <misc/parsehelper.h>
#include <win/AppWindow.H>
#include <win/ScreenModeRequester.h>
#include <win/resource.h>
#include <win/winerr.h>
#include <win/winexception.h>

#undef min
#undef max

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <string>

/* create definitions of all functionpointers */
#undef GLFUNCS_H
#define GL_EXT_TYPED(type, name) DEFN_GL_EXT_TYPED(type, name)
#include "GLFuncs.h"
#include "RenderTarget.h"

#include <memorytracker.h>

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

ConVar COpenGL::gl_renderer("gl_renderer", "", CVARFLAG_READONLY);
ConVar COpenGL::gl_vendor("gl_vendor", "", CVARFLAG_READONLY);
ConVar COpenGL::gl_extensions("gl_extensions", "", CVARFLAG_READONLY);
ConVar COpenGL::gl_version("gl_version", "", CVARFLAG_READONLY);

ConVar COpenGL::r_colorbits("r_colorbits", "0");
ConVar COpenGL::r_stencilbits("r_stencilbits", "0");
ConVar COpenGL::r_depthbits("r_depthbits", "0");
ConVar COpenGL::r_fullscreen("r_fullscreen", "0");
ConVar COpenGL::r_displayRefresh("r_displayRefresh", "60");

ConVar COpenGL::r_customwidth("r_customwidth", "640");
ConVar COpenGL::r_customheight("r_customheight", "480");
ConVar COpenGL::r_overBrightBits("r_overBrightBits", "0", 0, COpenGL::changegamma);
ConVar COpenGL::r_gamma("r_gamma", "1.2125", 0, COpenGL::changegamma);
ConVar COpenGL::r_FSAA("r_FSAA", "0", 0, COpenGL::changeFSAA);
ConVar COpenGL::r_ignore("r_ignore", "1");

using namespace std;

#ifdef _DEBUG
void GLERROR(const char* text)
{
    int e = glGetError();
    if (e != GL_NO_ERROR) {
        std::cout << "^1GL_ERROR in " << text << ":\n^4" << OGLErrorToString(e) << std::endl;
        if (!(int)COpenGL::r_ignore)
            throw CException(std::string("GL_ERROR in:") + std::string(text), OGLErrorToString(e));
    }
};
#endif

COpenGL::COpenGL()
{
    m_glwindow     = NULL;
    m_rendertarget = NULL;
    m_hDC          = NULL;

    m_screenwidth  = 640;
    m_screenheight = 480;

    m_hertz = 0;

    m_fully_initialized = false;
    m_run_fullscreen    = false;

    m_num_TextureUnits = 0;
}

COpenGL::~COpenGL()
{
    ShutDownOpenGL();
}

bool COpenGL::InitOpenGL(CWindow* window, int width, int height, bool fullscreen, const GLPIXELFORMAT& format)
{
    cout << endl << endl << "========= COpenGL()::InitOpenGL() ========" << endl << endl;
    if (m_fully_initialized) {
        // std::cout<<"tried to initialize OpenGL twice"<<std::endl;
        throw CException("tried to initialize OpenGL twice");
        return true;
    }

    assert(window);
    m_glwindow    = window;
    m_pixelformat = format;

    m_run_fullscreen = false;

    // enable  OpenGL-capable Window
    DWORD WStyle;
    WStyle = GetWindowLong(m_glwindow->getHWND(), GWL_STYLE);
    WStyle |= (WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
    SetWindowLong(m_glwindow->getHWND(), GWL_STYLE, WStyle);

    // if fullscreen is requested, go into fullscreen mode
    if (fullscreen) {
        m_screenwidth  = width;
        m_screenheight = height;
        m_hertz        = m_pixelformat.hertz ? m_pixelformat.hertz : (int)r_displayRefresh;
        if (!EnableFullscreen()) {
            std::cout << "COpenGL::InitOpenGL()->EnableFullscreen() failed" << std::endl;
            return false;
        }
    } else {
        SetWindowPos(m_glwindow->getHWND(), NULL, 0, 0, width, height, 0);
        EnableWindowed();
    }

    // get Device Context
    if (!getDeviceContext()) {
        if (m_glwindow->isFullscreen())
            SwitchToWindowed();
        return false;
    }

    // set the Pixelformat
    if (wglChoosePixelFormatARB) {
        if (!setWGLPixelFormat(m_pixelformat)) {
            if (m_glwindow->isFullscreen())
                SwitchToWindowed();
            return false;
        }
    } else {
        if (!setStandardPixelFormat(m_pixelformat)) {
            if (m_glwindow->isFullscreen())
                SwitchToWindowed();
            return false;
        }
    }

    if (!createRenderContext()) {
        if (m_glwindow->isFullscreen())
            SwitchToWindowed();
        return false;
    }

    cout << getRenderer() << endl << getVendor() << endl << getVersion() << endl;

    char extens[32000];  // large enough :-)
    strcpy(extens, getExtensions());
    ParseHelper::ReplaceChar(extens, ' ', '\n');
    cout << extens << std::endl << std::endl;

    const char* wglext = getWGLExtensions();
    if (wglext) {
        strcpy(extens, wglext);
        ParseHelper::ReplaceChar(extens, ' ', '\n');
        cout << extens << std::endl << std::endl;
    }

    if ((strstr(getRenderer(), "Voodoo2") || strstr(getRenderer(), "VoodooGraphics")) && !m_run_fullscreen) {
        // FIXME: I never tested the code in a V3+, but I assume just checking for "Voodoo"
        //  will fail with it, forcing it to fullscreen although it doesn´t need to
        std::cout << "COpenGL::InitOpenGL() warning: Voodoo card detected, enforcing fullscreen..." << endl;
        // chop format to an acceptable value
        m_screenwidth              = 640;
        m_screenheight             = 480;
        m_pixelformat.colorbits    = 16;
        m_pixelformat.stencilbits  = 0;
        m_pixelformat.zbits        = 0;
        m_pixelformat.multisamples = 0;
        m_pixelformat.alphabits    = 0;

        if (EnableFullscreen() == false) {
            std::cout << "...failed" << endl;
            EnableWindowed();
            return false;
        }
    }

    m_rendertarget->setViewport(m_glwindow->getWindowRect());

    saveCurrentGamma();
    memcpy(m_gammaramp, m_old_gammaramp, 3 * 256 * sizeof(WORD));

    registerForMessage(m_glwindow, Msg::GAME_ACTIVATED, 60000);
    registerForMessage(m_glwindow, Msg::GAME_DEACTIVATED, 60000);
    registerForMessage(m_glwindow, (Msg::MESSAGEID)WM_SIZE, 60000);
    registerForMessage(m_glwindow, (Msg::MESSAGEID)WM_MOVE, 60000);
    registerForMessage(m_glwindow, (Msg::MESSAGEID)WM_COMMAND, 60000);

    m_fully_initialized = true;

    std::cout << std::endl << "initialization successful" << std::endl << std::endl;
    return true;
}

bool COpenGL::InitOpenGL(CWindow* window, GLPIXELFORMAT format)
{
    DEVMODE Modes[MAXMODES];
    DEVMODE Mode;

    CScreenModeRequester SMR;

    Mode.dmSize        = sizeof(DEVMODE);
    Mode.dmDriverExtra = 0;

    int  numModes = 0;
    BOOL Enum     = TRUE;

    for (int enumMode = 0; Enum && numModes < MAXMODES; ++enumMode) {
        Enum = EnumDisplaySettings(NULL, enumMode, &Mode);
        if (Mode.dmBitsPerPel > 8) {
            if (format.colorbits && format.colorbits > (int)Mode.dmBitsPerPel)
                continue;
            if (format.hertz && format.hertz > (int)Mode.dmDisplayFrequency)
                continue;

            Modes[numModes] = Mode;
            CHARLINE ModeString;
            sprintf(ModeString, "%d x %d x %d @ %dHz", Modes[numModes].dmPelsWidth, Modes[numModes].dmPelsHeight,
                    Modes[numModes].dmBitsPerPel, Modes[numModes].dmDisplayFrequency);
            SMR.AddScreenMode(ModeString);
            ++numModes;
        }
    }

    bool FS;
    int  selMode;
    int  AALevel;

    if (!isWGLExtensionSupported("WGL_ARB_multisample")) {
        SMR.DisableFSAA();
    }

    AALevel = 4;  // give maximum
    if (SMR.SelectScreenMode(selMode, FS, AALevel)) {
        format.colorbits    = Modes[selMode].dmBitsPerPel;
        format.multisamples = AALevel;  // don´t know if this is right
        format.hertz        = Modes[selMode].dmDisplayFrequency;
        return InitOpenGL(window, Modes[selMode].dmPelsWidth, Modes[selMode].dmPelsHeight, FS ? true : false, format);
    }
    return false;
}

bool COpenGL::setWGLPixelFormat(const GLPIXELFORMAT& format)
{
    assert(m_hDC);

    int iAttribs[32];
    int num_iAttribs = 0;

    ZeroMemory(iAttribs, 32 * sizeof(int));

    std::cout << "wglChoosePixelFormatARB()... ";

    // fill Attribute list
    iAttribs[num_iAttribs++] = WGL_SUPPORT_OPENGL_ARB;
    iAttribs[num_iAttribs++] = GL_TRUE;
    iAttribs[num_iAttribs++] = WGL_ACCELERATION_ARB;
    iAttribs[num_iAttribs++] = WGL_FULL_ACCELERATION_ARB;  // full hw-accleleration required
    iAttribs[num_iAttribs++] = WGL_DOUBLE_BUFFER_ARB;
    iAttribs[num_iAttribs++] = GL_TRUE;
    if (!m_run_fullscreen) {
        iAttribs[num_iAttribs++] = WGL_DRAW_TO_WINDOW_ARB;
        iAttribs[num_iAttribs++] = GL_TRUE;
    }
    if (format.colorbits) {
        // cout<<"colorbits: "<<format.colorbits<<endl;
        iAttribs[num_iAttribs++] = WGL_COLOR_BITS_ARB;
        iAttribs[num_iAttribs++] = format.colorbits;
    }
    if (format.zbits) {
        // cout<<"zbits: "<<format.zbits<<endl;
        iAttribs[num_iAttribs++] = WGL_DEPTH_BITS_ARB;
        iAttribs[num_iAttribs++] = format.zbits;
    }
    if (format.stencilbits) {
        // cout<<"sbits: "<<format.stencilbits<<endl;
        iAttribs[num_iAttribs++] = WGL_STENCIL_BITS_ARB;
        iAttribs[num_iAttribs++] = format.stencilbits;
    }
    if (format.alphabits) {
        // cout<<"abits: "<<format.alphabits<<endl;
        iAttribs[num_iAttribs++] = WGL_ALPHA_BITS_ARB;
        iAttribs[num_iAttribs++] = format.alphabits;
    }
    if (format.multisamples) {
        if (isWGLExtensionSupported("WGL_ARB_multisample")) {
            //			cout<<"requesting "<<format.multisamples<<" multisamples"<<endl;
            iAttribs[num_iAttribs++] = WGL_SAMPLE_BUFFERS_EXT;
            iAttribs[num_iAttribs++] = GL_TRUE;
            iAttribs[num_iAttribs++] = WGL_SAMPLES_EXT;
            iAttribs[num_iAttribs++] = format.multisamples;
        } else {
            cout << endl << "WARNING: WGL_ARB_multisample not supported, claiming no multisample buffer" << endl;
        }
    }
    iAttribs[num_iAttribs++] = 0;
    iAttribs[num_iAttribs++] = 0;  // end of attribute list

    // choose Format
    int          chosenformat = 0;
    unsigned int num_formats  = 0;
    BOOL         rval         = wglChoosePixelFormatARB(m_hDC, iAttribs, NULL, 1, &chosenformat, &num_formats);

    if (!num_formats) {
        cout << "failed" << endl;
        if (rval && !num_formats)
            cout << "no suitable pixelformat found" << endl;
        else {
            cout << WinErrorToString() << endl;
        }
        return false;
    }
    cout << "successful" << endl;
    cout << "    Format " << chosenformat << " chosen" << endl << endl;

    // set pixelformat
    std::cout << "SetPixelFormat()...";
    if (!SetPixelFormat(m_hDC, chosenformat, NULL)) {
        throw CWinException("SetPixelFormat() failed: ");
        cout << "failed" << endl;
        return false;
    }
    cout << "successful" << endl << endl;

    // query exact values;
    num_iAttribs             = 0;
    iAttribs[num_iAttribs++] = WGL_COLOR_BITS_ARB;
    iAttribs[num_iAttribs++] = WGL_DEPTH_BITS_ARB;
    iAttribs[num_iAttribs++] = WGL_STENCIL_BITS_ARB;
    iAttribs[num_iAttribs++] = WGL_ALPHA_BITS_ARB;
    if (isWGLExtensionSupported("WGL_ARB_multisample")) {
        iAttribs[num_iAttribs++] = WGL_SAMPLES_EXT;
    }

    int results[10];

    ZeroMemory(results, 10 * sizeof(int));

    cout << "wglGetPixelFormatAttribivARB()... ";
    if (wglGetPixelFormatAttribivARB(m_hDC, chosenformat, 0, num_iAttribs, iAttribs, results)) {
        m_pixelformat.colorbits = r_colorbits = results[0];
        m_pixelformat.zbits = r_depthbits = results[1];
        m_pixelformat.stencilbits = r_stencilbits = results[2];
        m_pixelformat.alphabits                   = results[3];
        m_pixelformat.multisamples                = results[4];
        cout << "successful" << endl;
        cout << "    RGB: " << m_pixelformat.colorbits << " A: " << m_pixelformat.alphabits
             << " S: " << m_pixelformat.stencilbits << " Z: " << m_pixelformat.zbits
             << " M: " << m_pixelformat.multisamples << endl
             << endl;
    } else {
        cout << "failed, because:" << endl << WinErrorToString() << endl;
    }

    return true;
}

bool COpenGL::setStandardPixelFormat(const GLPIXELFORMAT& format)
{
    assert(m_hDC);
    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),  // Size Of This Pixel Format Descriptor
        1,                              // Version Number (?)
        PFD_DRAW_TO_WINDOW |            // Format Must Support Window
            PFD_SUPPORT_OPENGL |        // Format Must Support OpenGL
            PFD_DOUBLEBUFFER,           // Must Support Double Buffering
        PFD_TYPE_RGBA,                  // Request An RGBA Format
        (BYTE)format.colorbits,         // Select Color Depth
        0,
        0,
        0,
        0,
        0,
        0,                       // Color Bits Ignored (?)
        (BYTE)format.alphabits,  // Alphabits
        0,                       // Shift Bit Ignored (?)
        0,                       // No Accumulation Buffer
        0,
        0,
        0,
        0,                         // Accumulation Bits Ignored (?)
        (BYTE)format.zbits,        // 16Bit Z-Buffer (Depth Buffer)
        (BYTE)format.stencilbits,  //
        0,                         // No Auxiliary Buffer (?)
        PFD_MAIN_PLANE,            // Main Drawing Layer
        0,                         // Reserved (?)
        0,
        0,
        0  // Layer Masks Ignored (?)
    };

    std::cout << "ChoosePixelFormat()... ";
    int iFormat = ChoosePixelFormat(m_hDC, &pfd);
    if (iFormat == 0) {
        cout << "failed" << endl;
        return false;
    }

    cout << "successful" << endl;
    cout << "Format " << iFormat << " chosen" << endl << endl;

    DescribePixelFormat(m_hDC, iFormat, sizeof(pfd), &pfd);

    if (pfd.dwFlags & PFD_GENERIC_FORMAT) {
        cout << "COpenGL::setStandardPixelFormat() chosen PixelFormat does not support hw-acceleration" << endl;
        return false;
    }

    std::cout << "SetPixelFormat()... ";
    if (!SetPixelFormat(m_hDC, iFormat, &pfd)) {
        cout << "failed" << endl;
        return false;
    }
    cout << "successful" << endl;
    cout << "    RGB: " << (int)pfd.cColorBits << " A: " << (int)pfd.cAlphaBits << " S: " << (int)pfd.cStencilBits
         << " Z: " << (int)pfd.cDepthBits << endl
         << endl;

    // adapt pixelformat to real format, you´ll never know, what you get
    m_pixelformat.colorbits = r_colorbits = pfd.cColorBits;
    m_pixelformat.stencilbits = r_stencilbits = pfd.cStencilBits;
    m_pixelformat.alphabits                   = pfd.cAlphaBits;
    m_pixelformat.zbits                       = pfd.cDepthBits;

    return true;
}

bool COpenGL::createRenderContext()
{
    assert(m_hDC);

    m_rendertarget = new RenderTarget;

    // create the Rendering Context
    if (!m_rendertarget->createRenderContext(m_hDC)) {
        if (m_glwindow->isFullscreen() == true)
            SwitchToWindowed();
        return false;
    }

    // and make it the current Context
    if (!m_rendertarget->makeCurrent()) {
        if (m_glwindow->isFullscreen() == true)
            SwitchToWindowed();
        return false;
    }

    checkCapabilities();

    gl_extensions = m_extensions = string(getExtensions());
    gl_vendor                    = string(getVendor());
    gl_renderer                  = string(getRenderer());
    gl_version                   = string(getVersion());

    m_wglextensions = getWGLExtensions();

    return true;
}
void COpenGL::releaseRenderingContext()
{
    if (m_rendertarget) {
        m_rendertarget->destroyRenderContext();
        delete m_rendertarget;
        m_rendertarget = 0;
    }
}

bool COpenGL::getDeviceContext()
{
    assert(m_glwindow);
    // now get DeviceContext
    cout << "GetDC()... ";
    m_hDC = GetDC(m_glwindow->getHWND());
    if (m_hDC == NULL) {
        cout << "failed" << endl;
        return false;
    }
    cout << "successful" << endl << endl;
    return true;
}
void COpenGL::releaseDeviceContext()
{
    assert(m_glwindow);
    if (m_hDC) {
        ReleaseDC(m_glwindow->getHWND(), m_hDC);
        m_hDC = NULL;
    }
}

void COpenGL::ShutDownOpenGL()
{
    if (!m_fully_initialized)
        return;
    // unregister Messages
    unregisterForMessage(m_glwindow, (Msg::MESSAGEID)WM_SIZE);
    unregisterForMessage(m_glwindow, (Msg::MESSAGEID)WM_MOVE);
    unregisterForMessage(m_glwindow, (Msg::MESSAGEID)WM_COMMAND);
    unregisterForMessage(m_glwindow, Msg::GAME_ACTIVATED);
    unregisterForMessage(m_glwindow, Msg::GAME_DEACTIVATED);

    // go back into windowed mode
    if (m_run_fullscreen) {
        EnableWindowed();
    }

    // restore old gamma settings
    restoreGamma();

    // destroy rendering context
    releaseRenderingContext();
    releaseDeviceContext();

    // restore Windowstyle
    DWORD WStyle;
    WStyle = GetWindowLong(m_glwindow->getHWND(), GWL_STYLE);
    WStyle &= ~(WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
    SetWindowLong(m_glwindow->getHWND(), GWL_STYLE, WStyle);

    m_glwindow = NULL;

    m_fully_initialized = false;
}

void COpenGL::getCurrentScreenMode()
{
    DEVMODE screenmode;
    ZeroMemory(&screenmode, sizeof(DEVMODE));
    screenmode.dmSize        = sizeof(DEVMODE);
    screenmode.dmDriverExtra = 0;

    EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &screenmode);

    r_customwidth = m_screenwidth = screenmode.dmPelsWidth;
    r_customheight = m_screenheight = screenmode.dmPelsHeight;
    r_colorbits                     = (int)screenmode.dmBitsPerPel;
    r_displayRefresh = m_hertz = screenmode.dmDisplayFrequency;
}

bool COpenGL::EnableFullscreen()
{
    if (!SwitchToFullscreen()) {
        EnableWindowed();
        return false;
    }

    m_run_fullscreen = true;
    r_fullscreen     = 1;
    return true;
}

bool COpenGL::SwitchToFullscreen()
{
    if (m_rendertarget && m_rendertarget->getRenderContext())
        glFinish();

    // get current screenmode
    DEVMODE screenmode;
    ZeroMemory(&screenmode, sizeof(DEVMODE));
    screenmode.dmSize        = sizeof(DEVMODE);
    screenmode.dmDriverExtra = 0;

    screenmode.dmPelsWidth        = m_screenwidth;   // Screen Width
    screenmode.dmPelsHeight       = m_screenheight;  // Screen Height
    screenmode.dmBitsPerPel       = m_pixelformat.colorbits;
    screenmode.dmDisplayFrequency = m_hertz;
    screenmode.dmFields           = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL | DM_DISPLAYFREQUENCY;  // Pixel Mode

    if (ChangeDisplaySettings(&screenmode, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL) {
        // throw CWinException("COpenGL::SwitchToFullscreen() -> ChangeDisplaySettings()");
        return false;
    }

    m_glwindow->setFullscreen(true);
    return true;
}

bool COpenGL::EnableWindowed()
{
    m_run_fullscreen = false;

    SwitchToWindowed();

    r_fullscreen = 0;
    return true;
}

bool COpenGL::SwitchToWindowed()
{
    if (m_rendertarget && m_rendertarget->getRenderContext())
        glFinish();

    ChangeDisplaySettings(NULL, 0);

    m_glwindow->setFullscreen(false);
    return true;
}

Msg::MSGRVAL COpenGL::handleMessage(Msg::MessagingObject* sender, Msg::MESSAGEID msgId, const Msg::Param& parameters)
{
    MSG_PARAMCAST(Msg::WINDOWMESSAGE, parameters, localparam);
    const MSG& message = localparam.p1();
    switch (msgId) {
    case Msg::GAME_ACTIVATED:
        if (m_run_fullscreen && CAppWindow::Instance()->isReady()) {
            SwitchToFullscreen();
        }
        setGammaRamp(m_gammaramp);
        break;
    case Msg::GAME_DEACTIVATED:
        if (m_run_fullscreen) {
            SwitchToWindowed();
            ShowWindow(m_glwindow->getHWND(), SW_SHOWMINNOACTIVE);
        }
        restoreGamma();
        break;
    case WM_SIZE:
    case WM_MOVE:
        if (m_rendertarget)
            m_rendertarget->setViewport(m_glwindow->getWindowRect());
        break;
    case WM_COMMAND:
        if (LOWORD(message.wParam) == IDA_TOGGLEFULLSCREEN) {
            if (m_run_fullscreen) {
                EnableWindowed();
            } else {
                EnableFullscreen();
            }
        }
        break;
    }

    return Msg::PASS_ON;
}

RenderTarget* COpenGL::getRenderTarget()
{
    return m_rendertarget;
}

bool COpenGL::makeCurrent(RenderTarget* read_target)
{
    bool rval = m_rendertarget->makeCurrent(read_target);
    m_rendertarget->setViewport(m_glwindow->getWindowRect());
    return rval;
}

const char* COpenGL::getExtensions()
{
    const char* Extensions = (const char*)glGetString(GL_EXTENSIONS);
    return Extensions;
}
const char* COpenGL::getVendor()
{
    const char* Vendor = (const char*)glGetString(GL_VENDOR);
    return Vendor;
}
const char* COpenGL::getRenderer()
{
    const char* Renderer = (const char*)glGetString(GL_RENDERER);
    return Renderer;
}
const char* COpenGL::getVersion()
{
    const char* Version = (const char*)glGetString(GL_VERSION);
    return Version;
}

const char* COpenGL::getWGLExtensions()
{
    assert(m_hDC);
    if (!wglGetExtensionsStringARB)
        return NULL;
    return wglGetExtensionsStringARB(m_hDC);
}

void COpenGL::InfoBox()
{
    CHAR infostring[4096];
    if (m_glwindow->isFullscreen()) {
        SwitchToWindowed();
    };

    ZeroMemory(infostring, sizeof(infostring));
    strcat(infostring, getRenderer());
    strcat(infostring, nl);
    strcat(infostring, getVendor());
    strcat(infostring, nl);
    strcat(infostring, getVersion());
    strcat(infostring, nl);
    strcat(infostring, getExtensions());
    MessageBox(m_glwindow->getHWND(), infostring, "OpenGL Information", MB_OK | MB_ICONINFORMATION | MB_APPLMODAL);
    if (m_glwindow->isFullscreen()) {
        SwitchToFullscreen();
    };
}
void COpenGL::checkCapabilities()
{
    bindExtensions();

    m_caps = 0;
    if (glLockArraysEXT)
        m_caps |= GLCAPS_COMPILED_VERTEX_ARRAY;
    if (glPointParameterfARB)
        m_caps |= GLCAPS_POINT_PARAMETERS;
    if (glMultiTexCoord1dARB)
        m_caps |= GLCAPS_MULTITEXTURE;
    if (wglSwapIntervalEXT)
        m_caps |= GLCAPS_SWAPINTERVAL;
    if (wglGetDeviceGammaRamp3DFX)
        m_caps |= GLCAPS_3DFX_GAMMA_CONTROL;
    if (glDrawRangeElementsEXT)
        m_caps |= GLCAPS_DRAW_RANGE_ELEMENTS;
    if (glVertexArrayRangeNV)
        m_caps |= GLCAPS_VERTEX_ARRAY_RANGE;

    if (isExtensionSupported("GL_NV_vertex_array_range2")) {
        m_caps |= GLCAPS_VERTEX_ARRAY_RANGE2;
    }
    if (isExtensionSupported("GL_SGIS_generate_mipmap")) {
        m_caps |= GLCAPS_GENERATE_MIPMAP;
    }
    if (isExtensionSupported("GL_ARB_texture_compression")) {
        m_caps |= GLCAPS_TEXTURE_COMPRESSION;
    }
    if (isExtensionSupported("GL_EXT_texture_env_combine")) {
        m_caps |= GLCAPS_TEXTURE_ENV_COMBINE;
    }
    if (isExtensionSupported("GL_NV_texture_env_combine4")) {
        m_caps |= GLCAPS_TEXTURE_ENV_COMBINE4;
    }
    if (isExtensionSupported("GL_EXT_texture_env_add")) {
        m_caps |= GLCAPS_TEXTURE_ENV_ADD;
    }
    if (isExtensionSupported("GL_EXT_texture_compression_s3tc")) {
        m_caps |= GLCAPS_S3TC;
    }

    glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &m_num_TextureUnits);
}

void COpenGL::bindExtensions()
{
#undef GLFUNCS_H
#define GL_EXT_TYPED(type, name) GPA_GL_EXT_TYPED(type, name)
#include "GLFuncs.h"
}

bool COpenGL::isExtensionSupported(LPCSTR extension)
{
    assert(extension);
    if (strstr(m_extensions.c_str(), extension))
        return true;
    return false;
}

bool COpenGL::isWGLExtensionSupported(LPCSTR extension)
{
    assert(extension);
    if (strstr(m_wglextensions.c_str(), extension))
        return true;
    return false;
}

void COpenGL::setGamma(double gamma, int overbrightbits)
{
    r_overBrightBits = overbrightbits;
    r_gamma          = (float)gamma;
    double div       = (double)(1 << overbrightbits) / 255.0;
    WORD   value;
    gamma = 1.0 / gamma;
    for (int i = 0; i < 256; i++) {
        value                = (WORD)std::min(65535.0, std::max(0.0, pow((double)i * div, gamma) * 65535.0));
        m_gammaramp[i]       = value;
        m_gammaramp[i + 256] = value;
        m_gammaramp[i + 512] = value;
    }

    setGammaRamp(m_gammaramp);
}

void COpenGL::changegamma(ConVar& cvar)
{
    //	std::cout<<"COpenGL::changegamma()"<<std::endl;
    double gamma      = (float)r_gamma;
    int    overbright = (int)r_overBrightBits;
    COpenGL::Instance()->setGamma(gamma, overbright);
}

void COpenGL::restoreGamma()
{
    setGammaRamp(m_old_gammaramp);
}

void COpenGL::saveCurrentGamma()
{
    if (!m_hDC)
        return;
    if (wglGetDeviceGammaRamp3DFX) {
        wglGetDeviceGammaRamp3DFX(m_hDC, m_old_gammaramp);
    } else {
        GetDeviceGammaRamp(m_hDC, m_old_gammaramp);
    }
}

void COpenGL::setGammaRamp(WORD* newgammaramp)
{
    if (newgammaramp != m_gammaramp && newgammaramp != m_old_gammaramp) {
        memcpy(m_gammaramp, newgammaramp, 3 * 256 * sizeof(WORD));
    }
    if (!m_hDC)
        return;
    if (wglSetDeviceGammaRamp3DFX) {
        wglSetDeviceGammaRamp3DFX(m_hDC, newgammaramp);
    } else {
        SetDeviceGammaRamp(m_hDC, newgammaramp);
    }
}

int COpenGL::getScreenHeight() const
{
    return m_screenheight;
}

int COpenGL::getScreenWidth() const
{
    return m_screenwidth;
}

int COpenGL::getStencilBits() const
{
    return m_pixelformat.stencilbits;
}
int COpenGL::getColorBits() const
{
    return m_pixelformat.colorbits;
}

void COpenGL::changeFSAA(ConVar& cvar)
{
    if ((int)cvar && COpenGL::Instance()->isWGLExtensionSupported("WGL_ARB_multisample")) {
        glEnable(GL_MULTISAMPLE_ARB);
        std::cout << "GL_MULTISAMPLE_ARB enabled, using ";
        if (COpenGL::Instance()->isExtensionSupported("GL_NV_multisample_filter_hint")) {
            switch ((int)cvar) {
            case 1:
                glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_FASTEST);
                std::cout << " fastest" << endl;
                break;
            case 2:
                glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST);
                std::cout << " nicest" << endl;
                break;
            default:
                std::cout << " default (setting not supported)" << std::endl;
                break;
            }
        } else {
            std::cout << " default (GL_NV_multisample_filter_hint not supported)" << std::endl;
        }
    } else {
        glDisable(GL_MULTISAMPLE_ARB);
        std::cout << "GL_MULTISAMPLE_ARB disabled" << endl;
    }
}
