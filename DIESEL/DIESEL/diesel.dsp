# Microsoft Developer Studio Project File - Name="Diesel" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Diesel - WIN32 RELEASE
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "diesel.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "diesel.mak" CFG="Diesel - WIN32 RELEASE"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "Diesel - Win32 Release" (basierend auf  "Win32 (x86) Application")
!MESSAGE "Diesel - Win32 Debug" (basierend auf  "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Diesel", EAAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Diesel - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release_VC6"
# PROP Intermediate_Dir "Release_VC6"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /Gi /GR /GX /Ox /Ot /Oa /Oi /Op- /Oy /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FAs /YX /FD /c
# SUBTRACT CPP /Og /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /i "f:\myprojects\includes\dxshell\include\win" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 dxshelllib.res kernel32.lib user32.lib gdi32.lib comdlg32.lib shell32.lib advapi32.lib /nologo /subsystem:windows /profile /map /debug /machine:I386 /nodefaultlib:"libcmt"
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "Diesel - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug_VC6"
# PROP Intermediate_Dir "Debug_VC6"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /Gi /GR /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 dxshelllib.res kernel32.lib user32.lib gdi32.lib comdlg32.lib shell32.lib advapi32.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libcmt"
# SUBTRACT LINK32 /profile /nodefaultlib

!ENDIF 

# Begin Target

# Name "Diesel - Win32 Release"
# Name "Diesel - Win32 Debug"
# Begin Group "Quellcodedateien"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Color.cpp
# End Source File
# Begin Source File

SOURCE=.\diesel.cpp
# End Source File
# Begin Source File

SOURCE=.\Diesel.rc
# End Source File
# Begin Source File

SOURCE=.\Kernel.cpp
# End Source File
# Begin Source File

SOURCE=.\Quaternion.cpp
# End Source File
# Begin Source File

SOURCE=.\Sound.cpp
# End Source File
# Begin Source File

SOURCE=.\Task.cpp
# End Source File
# End Group
# Begin Group "Header-Dateien"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Color.h
# End Source File
# Begin Source File

SOURCE=.\diesel.h
# End Source File
# Begin Source File

SOURCE=.\Features.h
# End Source File
# Begin Source File

SOURCE=.\Kernel.h
# End Source File
# Begin Source File

SOURCE=.\Quaternion.h
# End Source File
# Begin Source File

SOURCE=.\Sound.h
# End Source File
# Begin Source File

SOURCE=.\Task.h
# End Source File
# Begin Source File

SOURCE=.\Transformation.h
# End Source File
# End Group
# Begin Group "Ressourcendateien"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "Loader"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\3DSLoader.cpp
# End Source File
# Begin Source File

SOURCE=.\3DSLoader.h
# End Source File
# Begin Source File

SOURCE=.\ASELoader.cpp
# End Source File
# Begin Source File

SOURCE=.\ASELoader.h
# End Source File
# Begin Source File

SOURCE=.\DOFLoader.cpp
# End Source File
# Begin Source File

SOURCE=.\DOFLoader.h
# End Source File
# Begin Source File

SOURCE=.\LWOLoader.cpp
# End Source File
# Begin Source File

SOURCE=.\LWOLoader.h
# End Source File
# Begin Source File

SOURCE=.\MD3Loader.cpp
# End Source File
# Begin Source File

SOURCE=.\MD3Loader.h
# End Source File
# Begin Source File

SOURCE=.\MESHLoader.cpp
# End Source File
# Begin Source File

SOURCE=.\MESHLoader.h
# End Source File
# Begin Source File

SOURCE=.\ShaderLoader.cpp
# End Source File
# Begin Source File

SOURCE=.\ShaderLoader.h
# End Source File
# End Group
# Begin Group "Engine"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Camera.cpp
# End Source File
# Begin Source File

SOURCE=.\Camera.h
# End Source File
# Begin Source File

SOURCE=.\Commands.cpp
# End Source File
# Begin Source File

SOURCE=.\Commands.h
# End Source File
# Begin Source File

SOURCE=.\Engine.cpp
# End Source File
# Begin Source File

SOURCE=.\Engine.h
# End Source File
# Begin Source File

SOURCE=.\EngineTypes.h
# End Source File
# Begin Source File

SOURCE=.\Frustum.cpp
# End Source File
# Begin Source File

SOURCE=.\Frustum.h
# End Source File
# End Group
# Begin Group "MISC"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\MEMCPY.CPP
# End Source File
# Begin Source File

SOURCE=.\memcpy.h
# End Source File
# Begin Source File

SOURCE=.\Stripper.cpp
# End Source File
# Begin Source File

SOURCE=.\Stripper.h
# End Source File
# Begin Source File

SOURCE=.\VBServices.cpp
# End Source File
# Begin Source File

SOURCE=.\VBServices.h
# End Source File
# End Group
# Begin Group "Console"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ConKeyMap.cpp
# End Source File
# Begin Source File

SOURCE=.\ConKeyMap.h
# End Source File
# Begin Source File

SOURCE=.\GLConsole.cpp
# End Source File
# Begin Source File

SOURCE=.\GLConsole.h
# End Source File
# Begin Source File

SOURCE=.\GLText.cpp
# End Source File
# Begin Source File

SOURCE=.\GLText.h
# End Source File
# Begin Source File

SOURCE=.\KeyMap.cpp
# End Source File
# Begin Source File

SOURCE=.\KeyMap.h
# End Source File
# Begin Source File

SOURCE=.\LineEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\LineEdit.h
# End Source File
# End Group
# Begin Group "Quake3"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Q3BSPLoader.cpp
# End Source File
# Begin Source File

SOURCE=.\Q3BSPLoader.h
# End Source File
# Begin Source File

SOURCE=.\Q3BSPMesh.cpp
# End Source File
# Begin Source File

SOURCE=.\Q3BSPMesh.h
# End Source File
# Begin Source File

SOURCE=.\Q3Entity.cpp
# End Source File
# Begin Source File

SOURCE=.\Q3Entity.h
# End Source File
# Begin Source File

SOURCE=.\Q3EntityFactory.cpp
# End Source File
# Begin Source File

SOURCE=.\Q3EntityFactory.h
# End Source File
# Begin Source File

SOURCE=.\Q3Scene.cpp
# End Source File
# Begin Source File

SOURCE=.\Q3Scene.h
# End Source File
# End Group
# Begin Group "GeometryMemManager"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\GeometryMemManager.cpp
# End Source File
# Begin Source File

SOURCE=.\GeometryMemManager.h
# End Source File
# Begin Source File

SOURCE=.\MemoryManager.cpp
# End Source File
# Begin Source File

SOURCE=.\MemoryManager.h
# End Source File
# Begin Source File

SOURCE=.\MemoryMap.cpp
# End Source File
# Begin Source File

SOURCE=.\MemoryMap.h
# End Source File
# End Group
# Begin Group "SceneObjects"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Animator.cpp
# End Source File
# Begin Source File

SOURCE=.\Animator.h
# End Source File
# Begin Source File

SOURCE=.\BaseScene.cpp
# End Source File
# Begin Source File

SOURCE=.\BaseScene.h
# End Source File
# Begin Source File

SOURCE=.\Entity.cpp
# End Source File
# Begin Source File

SOURCE=.\Entity.h
# End Source File
# Begin Source File

SOURCE=.\Mesh.cpp
# End Source File
# Begin Source File

SOURCE=.\Mesh.h
# End Source File
# Begin Source File

SOURCE=.\MS3DAnimation.cpp
# End Source File
# Begin Source File

SOURCE=.\MS3DAnimation.h
# End Source File
# Begin Source File

SOURCE=.\MS3DEntity.cpp
# End Source File
# Begin Source File

SOURCE=.\MS3DEntity.h
# End Source File
# Begin Source File

SOURCE=.\MS3DLoader.cpp
# End Source File
# Begin Source File

SOURCE=.\MS3DLoader.h
# End Source File
# Begin Source File

SOURCE=.\MS3DModel.cpp
# End Source File
# Begin Source File

SOURCE=.\MS3DModel.h
# End Source File
# Begin Source File

SOURCE=.\SceneGraph.cpp
# End Source File
# Begin Source File

SOURCE=.\SceneGraph.h
# End Source File
# Begin Source File

SOURCE=.\Transformation.cpp
# End Source File
# Begin Source File

SOURCE=.\VertexBuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\VertexBuffer.h
# End Source File
# End Group
# Begin Group "Renderer"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\glState.h
# End Source File
# Begin Source File

SOURCE=.\Renderer.cpp
# End Source File
# Begin Source File

SOURCE=.\Renderer.h
# End Source File
# Begin Source File

SOURCE=.\Shader.cpp
# End Source File
# Begin Source File

SOURCE=.\Shader.h
# End Source File
# Begin Source File

SOURCE=.\ShaderPass.cpp
# End Source File
# Begin Source File

SOURCE=.\ShaderPass.h
# End Source File
# Begin Source File

SOURCE=.\SkyGen.cpp
# End Source File
# Begin Source File

SOURCE=.\SkyGen.h
# End Source File
# Begin Source File

SOURCE=.\TNLStack.cpp
# End Source File
# Begin Source File

SOURCE=.\TNLStack.h
# End Source File
# Begin Source File

SOURCE=.\Wave.cpp
# End Source File
# Begin Source File

SOURCE=.\Wave.h
# End Source File
# End Group
# Begin Group "Texture"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\AVITexture.cpp
# End Source File
# Begin Source File

SOURCE=.\AVITexture.h
# End Source File
# Begin Source File

SOURCE=.\ROQTexture.cpp
# End Source File
# Begin Source File

SOURCE=.\ROQTexture.h
# End Source File
# Begin Source File

SOURCE=.\SpecialTextures.cpp
# End Source File
# Begin Source File

SOURCE=.\SpecialTextures.h
# End Source File
# Begin Source File

SOURCE=.\Texture.cpp
# End Source File
# Begin Source File

SOURCE=.\Texture.h
# End Source File
# Begin Source File

SOURCE=.\TextureManager.cpp
# End Source File
# Begin Source File

SOURCE=.\TextureManager.h
# End Source File
# Begin Source File

SOURCE=.\VideoTexture.cpp
# End Source File
# Begin Source File

SOURCE=.\VideoTexture.h
# End Source File
# Begin Source File

SOURCE=.\WaterTexture.cpp
# End Source File
# Begin Source File

SOURCE=.\WaterTexture.h
# End Source File
# End Group
# Begin Group "Image"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\BMPLoader.cpp
# End Source File
# Begin Source File

SOURCE=.\BMPLoader.h
# End Source File
# Begin Source File

SOURCE=.\FTXLoader.cpp
# End Source File
# Begin Source File

SOURCE=.\FTXLoader.h
# End Source File
# Begin Source File

SOURCE=.\Image.cpp
# End Source File
# Begin Source File

SOURCE=.\Image.h
# End Source File
# Begin Source File

SOURCE=.\ImageLoader.cpp
# End Source File
# Begin Source File

SOURCE=.\ImageLoader.h
# End Source File
# Begin Source File

SOURCE=.\JPGLoader.cpp
# End Source File
# Begin Source File

SOURCE=.\JPGLoader.h
# End Source File
# Begin Source File

SOURCE=.\TGALoader.cpp
# End Source File
# Begin Source File

SOURCE=.\TGALoader.h
# End Source File
# End Group
# Begin Group "ROQ"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ROQPlayer\roq.h
# End Source File
# Begin Source File

SOURCE=.\ROQPlayer\roq_read.c
# End Source File
# End Group
# Begin Group "Doom3"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\D3Entity.cpp
# End Source File
# Begin Source File

SOURCE=.\D3Entity.h
# End Source File
# Begin Source File

SOURCE=.\DoomLoader.cpp
# End Source File
# Begin Source File

SOURCE=.\DoomLoader.h
# End Source File
# Begin Source File

SOURCE=.\DoomMap.cpp
# End Source File
# Begin Source File

SOURCE=.\DoomMap.h
# End Source File
# Begin Source File

SOURCE=.\DoomScene.cpp
# End Source File
# Begin Source File

SOURCE=.\DoomScene.h
# End Source File
# End Group
# End Target
# End Project
