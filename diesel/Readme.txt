[Build instructions for current CMake setups: see BUILDING.md]

Diesel, a Quake3 Arena Map Viewer
==================================

This project started as a simple first-try program to get used to OpenGL, almost 3 years ago.
It rather evolved since then and became a fully featured Quake3 map viewer.
It is one of the most (graphical) feature complete Q3 viewers freely available and was (at least as far as I know)
the first one that featured volumetric fog and mirrors/portals. Be aware that it is _not_ a full 
game engine (although it already contains some of the needed subsystems), it was never planned to be one. 
While developing it I was mainly interested in enhancing its look, speed and supported features.

How to use it
=============

Just compile the executable and put it together with default.jpg it into your quake3 basedirectory.
In fact the executable is only dependend on a shader named "gfx/2d/bigchars",
to be found inside a subdirectory named "scripts" and a texture called "default.jpg" that will be used
when a texture could not be found. Due to its origin, Diesel will automatically try to add
all pk3-files inside a subdirectory named "baseq3" and to execute "q3config.cfg" and "autoexec.cfg"
(basically for binding keys)
You can add new searchpaths (and pk3-files inside them) at runtime using the "addPaks  directoryname" command.


The Source
==========

For current CMake build steps, see BUILDING.md.

You will notice that the source is not very consistent in respect to design rules and styleguide.
This is due to the long development time and also reflects my own learning process.
Some parts of the code were not touched for a long period of time (especially the DirectDraw/Direct3D/
DirectSound parts in DXShell), so they are are not guaranteed to do their job flawless. Be prepared for surprises :-)
The code is mainly split into some third-party libraries (ZLib, JPEGLib, MemoryTracker), the main project
"Diesel" and a library called "DXShell".
DXShell is a bunch of classes that were originally meant to wrap
DirectX for easy (in my opinion) use. But it soon became some sort of small framework for my own projects.
It still grows from time to time whenever I add a new class that I consider useful and reusable for future projects.
I even put all initialization code, the mainloop and messagepump into it. So be aware of that if you try to link
against DXShell, you'll most probably get doubly defined symbols, missing symbols and the like.
In fact a new project only needs to define 3 functions,

bool GameInit()		// called after creating the appwindow
int GameUpdate()		// called every frame, as long as the application is active and has the focus
void GameShutDown()	// called either when the appwindow gets closed, the mainloop breaks out
				// (GameUpdate() returns a value != 0) or some exception causes the app to close

and link to DXShell.lib in order to create a fully functional Win32 application with an open window.

(These paragraphs describe the original VC6 / PropertySheets workflow.)

Since ZLib, JPEGLib and Paul Nettle's Memory Manager are independend libraries that are distributed on their own
I didn't want to put their sourcefiles into my project directly but left them as separate projects. This way you could
(in theory) easily replace those libs with newer versions, but also means some more work for you if you want to get
Diesel compiled.

ZLib:		make "ZLIB" a global include and library path. I made a projectfile that builds a static library named "ZLib.lib".
		Since Diesel is dependend on the ZLib project, it will automatically link against ZLib.lib
JPEGLib: 	make "JPEGLIB" a global include and library path. I made a projectfile that builds a static library named "libjpeg.lib".
		Since Diesel is dependend on the JPEGLib project, it will automatically link against libjpeg.lib.
		The source files inside the JPEGLib project are only those ones needed to decompress jpeg files.
PN_MMGR: make "PN_MMGR" a global include and library path. MMGR usually "works" just by adding mmgr.cpp to your
		project. I decided it would be a good idea to be able to leave it out by putting that objectfile into a
		static lib that gets linked in as needed. "MemoryTracker.h" was added by me to be able to influence the inclusion		of mmgr.h a bit better ( I never directly include mmgr.h, but memorytracker.h instead)
FMOD:		download the FMOD SDK (http://www.fmod.org) first. Make FMOD/API/INCLUDE a global include directory and
		FMOD/API/LIB a global library path.
STLPort:	I recommend using STLport (http://www.stlport.org) together with VC6 (problems with std::auto_ptr). VC7 users don't need STLPort,
		but it can significantly speed up MMGR , while writing out the list of memory leaks (there are plenty :-)
		Built the lib-files as described in STLport's documentation, make STLport-4.5.3/stlport a global include
		path and STLport-4.5.3/lib a global library path. Be sure to place both paths above VC's standard searchpaths.

dxshell:	make dxshell/include a global include path and add dxshell/lib to the library searchpaths

You also need the DirectX SDK (should go well with Version 6 and above).
Additionally VC6 users either have to install the SP5 processor pack (VCPP5.exe)
or have to remove "memcpy.cpp" from the project and comment-out "#define AMD_MEMCOPY" in VertexBuffer.cpp.

You should now be able to compile Diesel.exe. I cannot promise there won't be problems, but since you got all files needed,
I'm sure you'll fix it somehow :-) 


Greets & Credits (in no particular order)
=========================================

Credits go to:

-id Software for releasing their Quake and QuakeII source code.
-the Aftershock project
-Tim Ferguson for his ROQ Decoder
-Paul Nettle for his great Memory Manager
-Jean-loup Gailly, Mark Adler, and Gilles Vollant (zlib / minizip)
-the Independent JPEG Group and the libjpeg-turbo Project
-Chris Robinson and contributors (OpenAL Soft)


Greets go to:

Victor 'Vic' Luchits , visit http://hkitchen.quakesrc.org/
Martin 'Killerkraus' Kraus
Dawid 'Rockfor' Kozlowski

and all my friends

Contact
=======

sonode@gmx.de
ICQ 77402067



Diesel (c) 2002,2003-2026 by Mathias Heyer
