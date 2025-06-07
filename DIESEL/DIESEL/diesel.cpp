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
#include <iostream>

#include <dinput/joystick.h>
#include <dinput/mouse.h>
#include <dxshell.h>

#include <file/Path.h>
#include <file/ZipFile.h>
#include <file/filemanager.h>

#include <adt/ConCmd.h>
#include <misc/exception.h>
#include <win/appwindow.h>
#include <win/filerequester.h>

#include "Engine.h"
#include "camera.h"

#include "3dsloader.h"
#include "ASELoader.h"
#include "DoomLoader.h"
#include "LWOLoader.h"
#include "MS3DLoader.h"
#include "Q3BSPLoader.h"
#include "dofloader.h"
#include "md3loader.h"
#include "meshloader.h"

#include "glconsole.h"
#include "gltext.h"
#include "shader.h"
#include "shaderloader.h"

#include "Conkeymap.h"
#include "Kernel.h"
#include "SceneGraph.h"
#include "TNLStack.h"
#include "Transformation.h"
#include "sound.h"
#include "vbservices.h"

#include "ENTITY.H"
#include "MS3DEntity.h"

#include <memorytracker.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////
CKeyboard* Keyboard = NULL;
CMouse*    Mouse    = NULL;
CJoystick* Joystick = NULL;
COpenGL*   ogl;

LIGHT headlight;

WORD q3gamma[] = {
#include "gammatable.h"
};

bool game_initialized = false;
/////////////////////////////////////////////////////////////////////////////////////////////////////
void LoadModel(CPath file, CEntity::ENTITYLIST& entitylist);
void MouseHandler();
void JoystickHandler();

void Water(VECTOR3* current_buffer, VECTOR3* old_buffer, int width_shift);

void mapCommand(ConCmd& command, const std::string& args);
void lightCommand(ConCmd& command, const std::string& args);
void testmodel(ConCmd& command, const std::string& args);
void addmodel(ConCmd& command, const std::string& args);
void addanimation(ConCmd& command, const std::string& args);

ConCmd cmd_map("map", mapCommand);
ConCmd cmd_testmodel("testmodel", testmodel);
ConCmd cmd_addanimation("addanimation", addanimation);
ConCmd cmd_model("addmodel", addmodel);

ConVar r_lighting("r_lighting", "0");

extern ConVar cg_fov;

// Testing

FOGFEATURE      ff;
RENDERINFO      override;
CVertexBuffer*  fogsurf;
DLIGHTFEATURE   dlightfeature;
static VECTOR3* water1 = NULL;
static VECTOR3* water2 = NULL;

////////////////////////////////////////////////////////////////////////////////////////////////////

bool GameInit()
{

    CAppWindow::Instance()->setTitle("Diesel (c)1999-2003 by M.Heyer");
    //	SetCurrentDirectory("g:\\quake3"); // fuers Profiling

    CFileManager* fman = CFileManager::Instance();

    CPath basepath;
    fman->getCurrentDirectory(basepath);
    fs_basepath = basepath.getString();

    CWindow* dummywindow = new CWindow();
    dummywindow->open(SW_HIDE);

    COpenGL::GLPIXELFORMAT format;  // leave empty, require no special settings
    ogl = COpenGL::Instance();

    if (!ogl->InitOpenGL(dummywindow, 0, 0, false, format))
        throw CException("COpenGL::InitOpenGL() creating dummy-context failed");
    ogl->ShutDownOpenGL();

    dummywindow->close();
    KILLOBJECT(dummywindow);

    if (!ogl->InitOpenGL(CAppWindow::Instance(), format))
        throw CException("COpenGL()::InitOpenGL() failed");

    //	ogl->InfoBox();

    if (!initSoundSystem()) {
        throw CException("Could not initialize Soundsystem!");
    }

    InitEngine();

    headlight.ambient  = VECTOR4(0.1f, 0.1f, 0.1f, 1.0f);
    headlight.specular = VECTOR4(0.5f, 0.5f, 0.5f, 1.0f);
    headlight.diffuse  = VECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
    headlight.pos      = VECTOR4(0.0f, 0.0f, 1.0f, 0.0f);
    headlight.dir      = VECTOR4(0.0f, 0.0f, -1.0f, 1.0f);

    headlight.cutoff         = 180.0f;
    headlight.exponent       = 4.0f;
    headlight.attenuation[1] = 0.005f;
    headlight.used_attribs   = DIRECTIONAL_LIGHT;

    CDirectInput::Instance()->InitDirectInput();
    // CDirectInput::Instance()->CDirectInput::InitDirectInput(getInstanceHandle(), CAppWindow::Instance()->getHWND());

    Keyboard = CKeyboard::Instance();
    Keyboard->initialize();
    Mouse = CMouse::Instance();
    Mouse->initialize(DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);

    /*	Joystick= new CJoystick();
        Joystick->SetRange(DIJOFS_X,-1000,1000);
        Joystick->SetDeadzone(DIJOFS_X,-10,10);
        Joystick->SetRange(DIJOFS_Y,-1000,1000);
        Joystick->SetDeadzone(DIJOFS_Y,-10,10);
        Joystick->SetRange(DIJOFS_Z,-1000,1000);
        Joystick->SetDeadzone(DIJOFS_Z,-10,10);

        Joystick->SetRange(DIJOFS_RZ,-1000,1000);
        Joystick->SetDeadzone(DIJOFS_RZ,-10,10);
    */

    // enumerate an arbitrary device
    GUID          GUID_device;
    CDInputDevice device;
    /*	if (CDirectInput::Instance()->selectDevice(GUID_device))
        {
            device.InitDevice(GUID_device);
        }
    */

    CAppWindow* appwindow = CAppWindow::Instance();
    if (!appwindow->isActive()) {
        SetActiveWindow(appwindow->getHWND());
    }

    /*	the overriding-speciality of TNLStack is not waterproof implemented
        (a new setOverrideInfo() will override any former setOverrideInfo())
        maybe there should be also some kind of ovrride-stack ?!
    */

    /*  creates a fog override-info, the fogplane is constantly moved up-and down.
        It will create an additional fog-pass for every geometry rendered, be it inside the fog or not.
        Fogmapping will care about the sharp edge of the fog.
        To have a surface that covers the fog from above a plane grid is created that is always centered
        at the camera and moves with the fogplane up and down.
        Overall this is a nice effect but costs much fillrate
    */

    /*	ff.plane=PLANE (VECTOR3(0,1,0),200);
        ff.shader=CShader::findOrLoadShader("textures/sfx/kc_hellfog_1k");
        fogsurf=new CVertexBuffer;
        fogsurf->AddRef();
        fogsurf->setShader(ff.shader);
        VBServices::createCompleteGrid(*fogsurf,3000,3000,10,10);
        VBServices::reverseIndices(*fogsurf);	// so grid is visibly from below
        // you can also override the shader for all walls and objects
        //override.shader=CShader::findOrLoadShader("textures/skin/chapthroat2");;
        override.num_features=1;
        override.features[0].name=FEATURE_FOG;
        override.features[0].data=&ff;
    */

    game_initialized = true;

    return true;
}

int GameUpdate()
{
    BeginFrame();

    Timer5.start();
    Timer5.pause();
    Timer6.start();
    Timer6.pause();

    g_Scene->renderScene();

    Timer5.stop();
    Timer6.stop();

    CGLConsole::Instance()->execCommandBuffer();

    CConKeyMap::Instance()->Update();

    if (Joystick)
        JoystickHandler();

    MouseHandler();

    CKernel::Instance()->runTasks();

    g_Camera->setupCamera();
    g_TNLStack->setCamera(*g_Camera);

    if ((int)r_lighting) {
        // setup a directional headlight
        g_TNLStack->PushMatrix();
        g_TNLStack->LoadIdentity();
        g_TNLStack->addLight(headlight);
        g_TNLStack->PopMatrix();
    }

    // Full-Scene Fogging
    /*	g_TNLStack->setOverrideRenderInfo(override);
        float t=g_Time*0.05;
        t=t-floorf(t);
        ff.plane.d=100+100*sinf(t*2.0f*PI);
        MATRIX4	m;
        // place fogsurface so that the camera is in its centre
        TranslationMatrix4(m,g_Camera->getTransformation().getTranslation()-VECTOR3(1500,0,1500));
        m.m13=ff.plane.d;
        g_TNLStack->PushMatrix();
        g_TNLStack->MultMatrix(m);
        g_TNLStack->batch(fogsurf);
        g_TNLStack->PopMatrix();
    */

    /*	// Watertest
        static float amp=500.0f;
        static float phase=0.0f;
        static float freq=PI/15.0f;
        static float damp=0.99;
        static int   addr=33*16;
        static float fps=30;
        static float lasttime=0;
        // zeit für neuen tropfen?
        float timepassed=(g_Time-lasttime)*fps;
        if (timepassed>=1.0f)
        {
            lasttime=g_Time;
            if ((unsigned int)rand()<200)
            {
                addr=rand()%(64*64)+1;
                amp=700.0f;
            }
            // tropfen weiterschwingen lassen
            water1[addr].y=sinf(phase)*amp;
            phase+=freq;
            amp*=damp;
            // neues wasser berechnen
            Water(water1,water2,6);
            // puffer tauschen
            VECTOR3 *temp=water1;
            water1=water2;
            water2=temp;
            fogsurf->setActiveSet(fogsurf->getActiveSet()==0?1:0);
        }
    */

    Timer4.start();
    g_Scene->cullScene();
    Timer4.stop();

    EndFrame();

    ogl->swapBuffers();

    return 0;
}

void GameShutDown()
{
    // don´t shutdown twice FIXME: automate this somehow
    if (!game_initialized)
        return;

    if (fogsurf)
        fogsurf->Release();

    KILLOBJECT(g_Scene);

    shutdownSoundSystem();

    ShutDownEngine();
    if (ogl) {
        ogl->ShutDownOpenGL();
    }
    COpenGL::destroyInstance();

    KILLOBJECT(Joystick);

    CKeyboard::destroyInstance();
    CMouse::destroyInstance();
    CDirectInput::destroyInstance();

    CFileManager::destroyInstance();

    cout << endl;
    cout << "=========================================================" << endl;
    cout << "GameShutDown(): left-overs:" << endl;
    ConCmd::executeCommandLine("entitylist;meshlist;shaderlist;texturelist");

    game_initialized = false;
}

void MouseHandler()
{
    static float        filter[5][2] = {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}};
    static float        weights[5]   = {0.5f, 0.25f, 0.125f, 0.0625, 0.03125};
    static unsigned int latest       = 0;

    latest = PREVINDEX(latest, 5);

    Mouse->GetDeviceState();

    filter[latest][0] = (float)Mouse->MouseState.lX * 0.01f;
    filter[latest][1] = (float)Mouse->MouseState.lY * 0.01f;

    float x = 0.0f;
    float y = 0.0f;
    for (int f = 0; f < 5; f++) {
        x += filter[(latest + f) % 5][0] * weights[f];
        y += filter[(latest + f) % 5][1] * weights[f];
    }

    if (Mouse->MouseState.rgbButtons[1]) {
        g_Camera->getTransformation().setAngles(g_Camera->getTransformation().getAngles() + VECTOR3(0.0f, 0.0f, x));
    } else {
        g_Camera->getTransformation().setAngles(g_Camera->getTransformation().getAngles() + VECTOR3(-y, -x, 0.0f));
    }
}

void JoystickHandler()
{
    GLfloat x, y, z, az;
    Joystick->GetDeviceState();
    x = (GLfloat)Joystick->JoyState.lX;
    y = (GLfloat)Joystick->JoyState.lY;
    z = (GLfloat)Joystick->JoyState.lZ;

    az = (GLfloat)Joystick->JoyState.lRz / 100;

    // std::cout<<"x: "<<x<<" y: "<<y<<" z: "<<z<<" az: "<<az<<std::endl;

    Transformation& t = g_Camera->getTransformation();

    t.setTranslation(t.getTranslation() + t.getColumnVector(2) * (g_SecsPerFrame * y));
    // t.setAngles(t.getAngles()+VECTOR3(RAD(y)*g_SecsPerFrame,RAD(az)*g_SecsPerFrame,0));
}

void LoadModel(CPath file, CEntity::ENTITYLIST& entitylist)
{

    if (file.getString().empty()) {
        std::string    filename;
        CFileRequester requester;
        requester.openRequester(filename);

        if (CAppWindow::Instance()->isFullscreen()) {
            ogl->SwitchToFullscreen();  // wont work, because the filerequester already switched the window to
                                        // windowed-mode
        }
        file = filename;
    }

    CFileManager* fman = CFileManager::Instance();
    fman->setCurrentDirectory(file.getPath());

    std::string ext = file.getExtension();
    if (ext == "3ds") {
        C3DSLoader loader;
        loader.Load3DS(file, entitylist);
        std::cout << file.getString() << " loaded.\n" << std::endl;
    } else if (ext == "md3") {
        MD3Loader loader;
        CEntity*  newentity = loader.loadSimpleLODEntity(file.getString());
        if (newentity != NULL) {
            std::cout << file.getString() << " loaded.\n" << std::endl;
            entitylist.push_back(newentity);
        }
    } else if (ext == "dof") {
        DOFLoader loader;
        loader.LoadDOF(file.getString().c_str(), entitylist);
        g_Scene->addEntities(entitylist);
    } else if (ext == "mesh") {
        MESHLoader loader;
        CMesh*     mesh = loader.loadMESH(file.getString());
        if (mesh) {
            CEntity* ent = new CEntity(mesh);
            entitylist.push_back(ent);
        }
    } else if (ext == "ase") {
        ASELoader loader;
        loader.LoadASE(file.getString(), entitylist);
    } else if (ext == "lwo") {
        LWOLoader loader;
        loader.LoadLWO(file.getString(), entitylist);
    } else if (ext == "ms3d") {
        MS3DLoader loader;
        MS3DModel* msmodel = loader.loadMS3DModel_ASCII(file.getString());
        if (msmodel) {
            MS3DEntity* entity = new MS3DEntity(msmodel);
            entitylist.push_back(entity);
        }
    }
}

void mapCommand(ConCmd& command, const std::string& args)
{
    glFinish();

    KILLOBJECT(g_Scene)

    CPath file = args;

    if (!args.empty()) {
        freeBGMusic();
        if (file.getExtension().empty()) {
            file = CPath("maps") / (args + std::string(".bsp"));  // no means to add an extension yet
        }
    }

    std::string ext = file.getExtension();
    if (ext == "bsp") {
        KILLOBJECT(g_Scene);
        Q3BSPLoader loader;
        Q3Scene*    q3scene = loader.LoadQ3BSP(file.getString());
        if (q3scene) {
            g_Scene = q3scene;
        } else {
            g_Scene = new CSceneGraph;
        }
    } else if (ext == "proc") {
        KILLOBJECT(g_Scene);
        DoomLoader loader;
        g_Scene = loader.LoadPROC(file.getString());
        if (!g_Scene) {
            g_Scene = new CSceneGraph;
        }
    } else {
        KILLOBJECT(g_Scene);
        g_Scene = new CSceneGraph;

        CEntity::ENTITYLIST list;
        LoadModel(file, list);
        g_Scene->addEntities(list);
    }

    // find the first entity that is called "info_player_deathmatch" and place the camera there

    Q3Scene* q3scene = dynamic_cast<Q3Scene*>(g_Scene);

    if (q3scene) {

        Q3Entity::SMARTPTRLIST     list = q3scene->getEntityList();
        Q3Entity::SMARTPTRITERATOR eIt  = list.begin();

        for (; eIt != list.end(); ++eIt) {
            if ((*eIt)->getClassName() == "info_player_deathmatch" || (*eIt)->getClassName() == "info_player_start")

            {
                g_Camera->getTransformation() = (*eIt)->getTransformation();
                return;
            }
        }
        if (list.size())
            g_Camera->getTransformation() = list.front()->getTransformation();
        else
            g_Camera->getTransformation().setTranslation(NullVector3);
    }
}

void testmodel(ConCmd& command, const std::string& args)
{
    CEntity::ENTITYLIST list;
    std::string         model = args + ".md3";
    LoadModel(model.c_str(), list);
    g_Camera->getTransformation().setTranslation(NullVector3);
    g_Scene->addEntities(list);
}

void Water(VECTOR3* current_buffer, VECTOR3* old_buffer, int width_shift)
{
    const int width = 1 << width_shift;
    for (int y = 1; y < width - 1; ++y) {
        int ax = (y << width_shift) + 1;
        for (int x = 1; x < width - 1; ++x, ++ax) {
            float temp = (((  // current_buffer[ax-width-1].y
                               +current_buffer[ax - width].y
                               //+current_buffer[ax-width+1].y
                               //+current_buffer[ax+width-1].y
                               + current_buffer[ax + width].y
                               //+current_buffer[ax+width+1].y
                               + current_buffer[ax - 1].y + current_buffer[ax + 1].y) *
                           0.5f) -
                          old_buffer[ax].y);

            // dämpfung
            temp -= temp * 0.002f;

            old_buffer[ax].y = temp;
        }
    }
}

void addanimation(ConCmd& command, const std::string& args)
{
    char entname[256], animname[256];
    sscanf(args.c_str(), "%s %s", entname, animname);

    MS3DEntity* entity = dynamic_cast<MS3DEntity*>(CEntity::findEntity(entname));

    if (!entity) {
        std::cout << "either could not find entity with this name or it is not of type MS3DEntity" << std::endl;
        return;
    }

    MS3DLoader     loader;
    MS3DAnimation* anim = loader.loadMS3DAnimation_ASCII(animname);
    if (!anim) {
        std::cout << "could not load given animation" << std::endl;
        return;
    }

    entity->getAnimator().addAnimation(anim, Animator::REPEATING, CKernel::Instance()->getTime());
}

void addmodel(ConCmd& command, const std::string& args)
{
    CEntity::ENTITYLIST list;
    LoadModel(args, list);

    CEntity::ENTITYITERATOR eIt = list.begin();

    for (; eIt != list.end(); ++eIt) {
        (*eIt)->getTransformation() = g_Camera->getTransformation();
    }

    g_Scene->addEntities(list);
}