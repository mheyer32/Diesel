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
#ifndef ENGINEHEADER_INCLUDED
#define ENGINEHEADER_INCLUDED

/*////////////////////////////////////////////////////////////////////
Includes
////////////////////////////////////////////////////////////////////*/
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <adt/ConVar.h>

class CCamera;
class CTimer;
class CGLText;
class TNLStack;
class CBaseScene;
/*////////////////////////////////////////////////////////////////////
Defines
////////////////////////////////////////////////////////////////////*/

/*////////////////////////////////////////////////////////////////////
Variables
////////////////////////////////////////////////////////////////////*/

extern CBaseScene* g_Scene;     // global SceneObject
extern TNLStack*   g_TNLStack;  // global TNLStack (may be overwritten by another)
extern CCamera*    g_Camera;    // global currently used Camera Object

extern CGLText* g_Text;  // global TextOut (mostly for debug output)

extern float g_Time;  // gloabal time sice startup
extern int   g_FrameCounter;
extern float g_SecsPerFrame;  //
extern float g_FPS;

extern CTimer Timer1;  // mißt die Zeit seit Start
extern CTimer Timer2;  // mißt 1 Frame;
extern CTimer Timer3;  // mißt die FPS

extern CTimer Timer4;  // extra timers
extern CTimer Timer5;
extern CTimer Timer6;
extern CTimer Timer7;

// these will disappear some beautiful day
// the numbers are generated in Q3BSPMesh,CCamera and Renderer
extern DWORD drawnnodes, skippednodes, seenboxes, unseenboxes, gl_apicalls, triangles, strippedtris, num_framevbuffers;

extern ConVar fs_basepath;
/*////////////////////////////////////////////////////////////////////
Function Prototypes
////////////////////////////////////////////////////////////////////*/

extern BOOL InitEngine();
extern void ShutDownEngine();
extern void BeginFrame();
extern void EndFrame();

#endif