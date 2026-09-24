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
#include "Engine.h"

#include <iostream>

#include <file/FileManager.h>
#include <misc/Timer.h>
#include <misc/Exception.h>
#include <win/AppWindow.h>
#include <win/WinMain.h>

#include "Camera.h"
#include "Entity.h"
#include "Mesh.h"
#include "Shader.h"
#include "ShaderLoader.h"
#include "Texture.h"

#include "Camera.h"
#include "ConKeyMap.h"
#include "GLConsole.h"
#include "GLText.h"
#include "GeometryMemManager.h"
#include "Renderer.h"
#include "SceneGraph.h"
#include "TNLStack.h"

#include <MemoryTracker.h>
//////////////////////////////////////////////////////////////////
// Globals
//////////////////////////////////////////////////////////////////
CBaseScene* g_Scene    = NULL;
CCamera*    g_Camera   = NULL;
TNLStack*   g_TNLStack = NULL;

float g_FPS;
float g_SecsPerFrame;
float g_Time;
int   g_FrameCounter = 0;

CTimer Timer1;  // mißt die Zeit seit Start
CTimer Timer2;  // mißt 1 Frame;
CTimer Timer3;  // mißt die g_FPS
CTimer Timer4;  // traversing scenegraph, collecting all visible faces, doing all the fancy culling stuff
CTimer Timer5;  // rendering g_TNLStack
CTimer Timer6;  // sorting the g_TNLStack for shadersort,shaderref,lightmap,lighting and transformation (in that order)
CTimer Timer7;

// also global, but should be discarded some day
DWORD drawnnodes, skippednodes, seenboxes, unseenboxes, gl_apicalls, triangles, strippedtris, num_framevbuffers;

bool already_updating = false;

CGLConsole* Console = NULL;
CGLText*    g_Text  = NULL;
CConKeyMap* KeyMap  = NULL;

Renderer* renderer = NULL;

// console Variables
ConVar r_clear("r_clear", "0");
ConVar fs_basepath("fs_basepath", "", CVARFLAG_READONLY);
void   change_fs_game(ConVar& cvar);
ConVar fs_game("fs_game", "baseq3", 0, change_fs_game);

//////////////////////////////////////////////////////////////////

using namespace std;

BOOL InitEngine()
{

    cout << endl << "======== InitEngine() ========" << endl << endl;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////

    cout << "initializing filesystem..." << endl;
    CPath basepath = (string)fs_basepath;

    cout << "basepath: " << basepath.getString() << endl;

    SetCurrentDirectory(basepath.getString().c_str());

    CFileManager* fman = CFileManager::Instance();

    fman->clearPakFileCache();
    fman->clearSearchPaths();

    fman->addSearchPath(basepath);  // working directory
    fman->addSearchPath("baseq3");  // standard quake3 searchpath
    fman->addSearchPath("base");    // standard doom3 searchpath

    CPath gamedir = (string)fs_game;  // it may be that the gamepath is different
    if (gamedir != CPath("baseq3")) {
        fman->addSearchPath(gamedir);
    }

    fman->cachePakFiles("");  // find all pk3 files in all searchpaths

    cout << "initializing shaders..." << endl;

    CShaderLoader::clearShaderCache();
    CShaderLoader::cacheDirectory("scripts");  // scriptfiles cachen

    cout << "checking required OpenGL extensions...";
    COpenGL* ogl  = COpenGL::Instance();
    int      caps = ogl->getCaps();
    if (!(caps & GLCAPS_COMPILED_VERTEX_ARRAY))
        throw CException("EXT_compiled_vertex_arrays not supported");
    cout << " successful" << endl;

    cout << "initializing GeometryMemoryManager...";
    GeometryMemManager::Instance()->initialize(MM_SYSONLY, 20 * 1024 * 1024, 20 * 1024 * 1024, 20 * 1024 * 1024);
    cout << "successful" << endl;

    cout << "initializing Renderer";
    renderer = Renderer::Instance();
    renderer->initialize();
    cout << "done." << endl;

    cout << "creating default shader...";
    CShader::createDefaultShader();
    cout << "done." << endl;

    cout << "creating g_TNLStack...";
    g_TNLStack = new TNLStack;
    cout << "done." << endl;

    cout << "creating g_Camera...";
    g_Camera = new CCamera();
    g_Camera->setupFrustum(100.0f,
                           (float)CAppWindow::Instance()->getWidth() / (float)CAppWindow::Instance()->getHeight(), 4.0f,
                           8000.0f);
    g_Camera->setupCamera();
    cout << " done." << endl;

    cout << "creating g_Scene...";
    g_Scene = new CSceneGraph();
    g_Scene->initialize();

    cout << " done." << endl;

    cout << "starting timers...";
    Timer1.start();
    Timer2.start();
    Timer3.start();

    Timer4.start();
    Timer5.start();
    Timer6.start();
    Timer7.start();
    cout << " done." << endl;

    KeyMap = CConKeyMap::Instance();

    g_Text = new CGLText();
    g_Text->SetBufsize(64, 32);
    g_Text->AttachToWindow();
    g_Text->SetColor(COLOR(255, 255, 248, 196));

    Console = CGLConsole::Instance();

    Console->initialize(&getHistory());

    ConCmd::executeCommandLine("exec q3config.cfg");
    ConCmd::executeCommandLine("exec autoexec.cfg");

    COpenGL::Instance()->resolveIgnoreHwGamma();
    COpenGL::Instance()->setGamma((float)COpenGL::r_gamma, (int)COpenGL::r_overBrightBits);

    return true;
}
//////////////////////////////////////////////////////////////////
void change_fs_game(ConVar& cvar)
{
    ShutDownEngine();
    InitEngine();
}
//////////////////////////////////////////////////////////////////
void ShutDownEngine()
{
    KILLOBJECT(g_Text)
    KILLOBJECT(g_Scene);
    KILLOBJECT(g_TNLStack);
    KILLOBJECT(g_Camera);

    CGLConsole::destroyInstance();
    Renderer::destroyInstance();
}

#define NUM_FILTERFRAMES 10

void BeginFrame()
{
    static float        count       = 0;
    static float        filter[10]  = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    static float        weights[10] = {0.5f,      0.25f,      0.125f,      0.0625f,      0.03125f,
                                       0.015625f, 0.0078125f, 0.00390625f, 0.001953125f, 0.000976562f};
    static unsigned int latest      = 0;

    if (already_updating)
        return;  // no recursive update allowed
    already_updating = true;

    Timer1.stop();  // g_Time From the Beginning of the Game
    Timer2.stop();  // g_Time needed for the last Frame
    g_Time = (float)Timer1.getElapsedSecs();

    latest         = PREVINDEX(latest, NUM_FILTERFRAMES);
    filter[latest] = (float)Timer2.getElapsedSecs();

    g_SecsPerFrame = 0.0f;

    for (int f = 0; f < NUM_FILTERFRAMES; ++f) {
        g_SecsPerFrame += filter[(latest + f) % NUM_FILTERFRAMES] * weights[f];
    }

    ++count;
    if (count >= 10.0f) {
        float fpstime;
        Timer3.stop();
        fpstime = (float)Timer3.getElapsedSecs();
        g_FPS   = 10.0f / (fpstime);
        count   = 0;
        Timer3.start();
    }

    Timer2.start();  // lets measure this Frame

    skippednodes = drawnnodes = seenboxes = unseenboxes = gl_apicalls = triangles = strippedtris = num_framevbuffers =
        0;

    g_Text->Clr();

    // FIXME: get all this stuff into one place one day!
    glPushAttrib(GL_DEPTH_BUFFER_BIT);
    glDepthMask(GL_TRUE);
    int mask = GL_DEPTH_BUFFER_BIT;
    if ((int)r_clear == 1)
        mask |= GL_COLOR_BUFFER_BIT;
    glClear(mask);
    glPopAttrib();

    Renderer::Instance()->updateIdentityLighting();

    ++g_FrameCounter;
}
void EndFrame()
{
    char text[256];

    Console->Render();

    sprintf(text, "cull: %5.2f ms\nsort: %5.2f ms\ndraw: %5.2f ms", Timer4.getElapsedSecs() * 1000.0f,
            Timer6.getElapsedSecs() * 1000.0f, Timer5.getElapsedSecs() * 1000.0f);
    g_Text->PrintAt(text, 0, 0);

    sprintf(text, "%5.2f ms %5.2f fps", g_SecsPerFrame * 1000.0f, g_FPS);
    g_Text->PrintAt(text, 42, 0);
    sprintf(text, "%2d:%2d", (int)g_Time / 60, (int)fmod(g_Time, 60));
    g_Text->PrintAt(text, 58, 1);
    sprintf(text, "%3d drawnnodes %3d skippednodes\n%3d seenboxes %3d unseenboxes\n", drawnnodes, skippednodes,
            seenboxes, unseenboxes);
    g_Text->PrintAt(text, 0, 29);
    sprintf(text, "%6d gl_apicalls %6d tris %5d strips %5d vbuffers", gl_apicalls, triangles, strippedtris,
            num_framevbuffers);
    g_Text->PrintAt(text, 0, 30);

    g_Text->RenderBuffer();

    Renderer::Instance()->applyOverBright();

    already_updating = false;
}
