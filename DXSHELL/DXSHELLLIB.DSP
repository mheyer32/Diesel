# Microsoft Developer Studio Project File - Name="DXShellLib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=DXShellLib - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "DXSHELLLIB.MAK".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "DXSHELLLIB.MAK" CFG="DXShellLib - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "DXShellLib - Win32 Release" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE "DXShellLib - Win32 Debug" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/DXShellLib", BAAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DXShellLib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release_VC6"
# PROP Intermediate_Dir "Release_VC6"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /W3 /GR /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /fo"lib\DXShelllib.res" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"LIB\DXShell_vc6.lib"

!ELSEIF  "$(CFG)" == "DXShellLib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug_VC6"
# PROP Intermediate_Dir "Debug_VC6"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GR /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /fo"lib\DXShellLib_vc6.res" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"LIB\DXShelld_vc6.lib"

!ENDIF 

# Begin Target

# Name "DXShellLib - Win32 Release"
# Name "DXShellLib - Win32 Debug"
# Begin Group "ADT"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\INCLUDE\ADT\CONCMD.CPP
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\ADT\CONCMD.H
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\ADT\ConVar.cpp
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\ADT\ConVar.h
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\ADT\NAMEDOBJECT.CPP
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\ADT\NAMEDOBJECT.H
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\ADT\REFCOUNT.CPP
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\ADT\REFCOUNT.H
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\ADT\SINGLETON.H
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\ADT\SmartPointer.h
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\ADT\StrideArray.h
# End Source File
# End Group
# Begin Group "Direct3D"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\INCLUDE\D3D\DIRECT3D.CPP
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\D3D\DIRECT3D.H
# End Source File
# End Group
# Begin Group "DirectDraw"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\INCLUDE\DDRAW\DDERR.CPP
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\DDRAW\DDERR.H
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\DDRAW\DDrawException.cpp
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\DDRAW\DDrawException.h
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\DDRAW\DDUTIL.CPP
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\DDRAW\DDUTIL.H
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\DDRAW\DIRECTDRAW.CPP
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\DDRAW\DIRECTDRAW.H
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\DDRAW\OVERLAY.CPP
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\DDRAW\OVERLAY.H
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\DDRAW\PALETTE.CPP
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\DDRAW\PALETTE.H
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\DDRAW\SURFACE.CPP
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\DDRAW\SURFACE.H
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\DDRAW\SURFACE16.CPP
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\DDRAW\SURFACE16.H
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\DDRAW\SURFACE8.CPP
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\DDRAW\SURFACE8.H
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\DDRAW\SurfaceMem.cpp
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\DDRAW\SurfaceMem.h
# End Source File
# End Group
# Begin Group "DirectInput"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\INCLUDE\DINPUT\dierr.cpp
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\DINPUT\dierr.h
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\DINPUT\DINPUTDEVICE.CPP
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\DINPUT\DINPUTDEVICE.H
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\DINPUT\DinputException.cpp
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\DINPUT\DinputException.h
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\DINPUT\DIRECTINPUT.CPP
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\DINPUT\DIRECTINPUT.H
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\DINPUT\JOYSTICK.CPP
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\DINPUT\JOYSTICK.H
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\DINPUT\KEYBOARD.CPP
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\DINPUT\KEYBOARD.H
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\DINPUT\MOUSE.CPP
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\DINPUT\MOUSE.H
# End Source File
# End Group
# Begin Group "DirectSound"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\INCLUDE\DSOUND\DIRECTSOUND.CPP
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\DSOUND\DIRECTSOUND.H
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\DSOUND\dserr.cpp
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\DSOUND\dserr.h
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\DSOUND\DSoundException.cpp
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\DSOUND\DSoundException.h
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\DSOUND\SOUNDBUFFER.CPP
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\DSOUND\SOUNDBUFFER.H
# End Source File
# End Group
# Begin Group "File"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\INCLUDE\FILE\DIRTREE.H
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\FILE\dirtree.hh
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\FILE\FILE.CPP
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\FILE\file.h
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\FILE\FILEMANAGER.CPP
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\FILE\FILEMANAGER.H
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\FILE\PATH.CPP
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\FILE\PATH.H
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\FILE\RAMFILE.CPP
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\FILE\RAMFILE.H
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\FILE\ZIPFILE.CPP
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\FILE\ZIPFILE.H
# End Source File
# End Group
# Begin Group "Geometry"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\INCLUDE\GEOMETRY\AABB.CPP
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\GEOMETRY\AABB.H
# End Source File
# End Group
# Begin Group "Math"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\INCLUDE\MATH\MATHDEFS.H
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\MATH\MATHLIB.CPP
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\MATH\MathLib.h
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\MATH\MATRIX3.H
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\MATH\matrix3.hh
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\MATH\MATRIX4.H
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\MATH\matrix4.hh
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\MATH\PLANE.CPP
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\MATH\PLANE.H
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\MATH\VECTOR2.H
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\MATH\vector2.hh
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\MATH\VECTOR3.H
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\MATH\vector3.hh
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\MATH\VECTOR4.H
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\MATH\vector4.hh
# End Source File
# End Group
# Begin Group "Misc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\INCLUDE\MISC\EXCEPTION.CPP
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\MISC\Exception.h
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\MISC\HISTORY.H
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\MISC\history.hh
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\MISC\Lexer.cpp
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\MISC\Lexer.h
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\MISC\PARSEHELPER.CPP
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\MISC\PARSEHELPER.H
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\MISC\QSort.h
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\MISC\STLHelp.h
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\MISC\STREAMBUF_HISTORY.CPP
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\MISC\STREAMBUF_HISTORY.H
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\MISC\TIMER.CPP
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\MISC\TIMER.H
# End Source File
# End Group
# Begin Group "OpenGL"

# PROP Default_Filter ""
# Begin Group "Extensions"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\INCLUDE\GL\glATI.h
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\GL\GLEXT.H
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\GL\NewExt.h
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\GL\wglATI.h
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\GL\WGLEXT.H
# End Source File
# End Group
# Begin Source File

SOURCE=.\INCLUDE\OPENGL\GAMMATABLE.H
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\OPENGL\GLFuncs.h
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\OPENGL\OGLERRORS.CPP
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\OPENGL\OGLERRORS.H
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\OPENGL\OPENGL.CPP
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\OPENGL\OPENGL.H
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\OPENGL\RenderTarget.cpp
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\OPENGL\RenderTarget.h
# End Source File
# End Group
# Begin Group "Windows"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\INCLUDE\WIN\APPWINDOW.CPP
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\WIN\AppWindow.h
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\WIN\DEBUG.CPP
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\WIN\DEBUG.H
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\WIN\DRIVERSELECTOR.CPP
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\WIN\DRIVERSELECTOR.H
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\WIN\FILEREQUESTER.CPP
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\WIN\FILEREQUESTER.H
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\WIN\RESOURCE.H
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\WIN\SCREENMODEREQUESTER.CPP
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\WIN\SCREENMODEREQUESTER.H
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\WIN\WINDOW.CPP
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\WIN\Window.h
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\WIN\WinErr.cpp
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\WIN\WinErr.h
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\WIN\WINEXCEPTION.CPP
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\WIN\WinException.h
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\WIN\WINMAIN.CPP
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\WIN\WINMAIN.H
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\WIN\WNDPROC.CPP
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\WIN\WndProc.h
# End Source File
# End Group
# Begin Group "Ressourcen"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\INCLUDE\WIN\appicon.ico
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\WIN\Audio.ico
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\WIN\Device2.ico
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\WIN\DirectX.ico
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\WIN\DXShellLib.rc
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\WIN\SCREENMODE.ICO
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\WIN\SKULL_FACE.ICO
# End Source File
# End Group
# Begin Group "Messaging"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\INCLUDE\MESSAGING\Messages.h
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\MESSAGING\MessagingObject.cpp
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\MESSAGING\MessagingObject.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\INCLUDE\DEFS.H
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\DXSHELL.H
# End Source File
# End Target
# End Project
