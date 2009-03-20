// -----------------------------------------------------------------------------
//  fmod_dyn.h -- Include header for run-time dynamic linking FMOD.
//  Version 3.5.0002, release by Serac.
//
//  This file was created by Aaron 'Serac' Hill <serac@hillvisions.com> and is
//  NOT a part of the FMOD Sound System.  Please contact the author if you have
//  questions or comments about this file.  For information about FMOD, please
//  visit http://www.fmod.org/.
//
//  FMOD is Copyright (c), Firelight Technologies Pty, Ltd 1999-2002.
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//  Warnings
//  --------
//
//  Thread Safety
//      This code currently is NOT thread safe.
//
//  Non-Win32 Platforms
//      This code currently does NOT support non-Win32 platforms.
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//  Using "fmod_dyn.h"
//  ------------------
//
//  This header is a full replacement for "fmod.h", so be sure to only include
//  "fmod_dyn.h" in your source files.  "fmod_mini.h" is a smaller version of
//  "fmod.h" with everything but the function prototypes.  You do not need to
//  manually include this file as "fmod_dyn.h" does it for you.  "fmod_funcs.h"
//  is a special header with all of the function prototypes.  As with the other
//  header, you do not use this file directly.
//
//  It should also be noted that you do not link with "fmod*.lib".
//
//  Before you include "fmod_dyn.h" in your source files, you can define a few
//  symbols to control the header file.  Here is a list of symbols:
//
//  FMOD_DYN_IMPL
//      This is the most important symbol.  It needs to be defined in one and
//      ONLY one of your source files.  It causes the declarations to become
//      definitions for the actual implementation.
//
//  FMOD_DYN_NOSAFETY
//      Functions pointers are unsafe, because their value can be NULL.  By
//      creating dummy functions, we can ensure that the function pointer is
//      never invalid.  The header generates these "safe" functions, but they
//      can be disabled with this symbol.
//
//  FMOD_DYN_NOASSERT
//      It may be desired during the debugging phase to be alerted when a
//      "safe" function is called instead of the real one.  By default, all
//      of the safety functions force a false assertion.  Define this symbol
//      to disable this behavior.  NOTE: Without the assertion, the "safe"
//      functions will gracefully fail, returning a benign zero.
//
//  FMOD_DYN_NOCLASSES
//      If you are using a C++ compiler, you can define this to prevent the
//      fmod_dyn C++ wrappers from being made available.  This cannot be
//      defined, if you intend on using the following C++ features:
//
//      FMOD_DYN_AUTOLOAD
//          Define this symbol to the desired library you wish to have auto-
//          loaded for you.  This behavior is done with a static object that
//          loads the API during its constructor and unloads the API during
//          its destructor.  Being a static object, this functionality will
//          occur directly before and after main().  (See fmod_dyn.cpp for
//          an example of using auto-load.)
//
//  Now that you have configured "fmod_dyn.h", you can include it.  As a full
//  replacement, the API function names have not been changed.  Even though
//  they are actually function pointers, you still invoke them the same way.
//
//  If you are not using the auto-load feature, you must manually load the
//  FMOD library before you can use the methods.  To load the library, just
//  call the function, LoadFMOD(), passing it FMOD_DLL.  (FMOD_DLL is pre-
//  defined to be the string "fmod.dll".)  Unless the name of the library
//  changes, you should not need to specify any other value but FMOD_DLL.
//
//  The return value for LoadFMOD() is the same value you get by using the
//  FMOD_DYN_FULLYLOADED macro.  Here is a summary of the macros:
//
//  FMOD_DYN_GETVERSION
//      This value is a float representing the version of the library
//      loaded.  0.0f is returned if no library is loaded.
//
//  FMOD_DYN_LOADED
//      This value indicates that a library was loaded.
//
//  FMOD_DYN_PARTIAL
//      This value indicates that some functions were not found.
//
//  FMOD_DYN_BADVERSION
//      This value indicates that the library is not the same version as
//      given by FMOD_VERSION.  (Versions newer than FMOD_VERSION do not
//      count as bad.)
//
//  FMOD_DYN_FULLYLOADED
//      This value indicates that the library was loaded, all functions
//      were found, and the version is valid.
//
//  FMOD_DYN_SOFTGUARD, FMOD_DYN_GUARD, FMOD_DYN_UNGUARD
//      These macros are meant to be used together to guard sections of
//      calls to the FMOD API.  Each use of FMOD_DYN_SOFTGUARD or
//      FMOD_DYN_GUARD must be paired with an FMOD_DYN_UNGUARD at the
//      same bracing level.
//
//      "Soft" guarding simply checks that a library was loaded.  This
//      does not protect against bad versions or partial loads.  If you
//      are using FMOD_DYN_NOSAFETY, then be especially careful as some
//      function pointers may be NULL.
//
//      Example:
//
//      FMOD_DYN_GUARD
//          FSOUND_Init(44100,32,0);
//          [ . . . ]
//          FSOUND_Close();
//      FMOD_DYN_UNGUARD
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//  Having trouble?
//  ---------------
//
//  If your compiler gives you warnings about "redefinition", then you either
//  have inadvertantly included "fmod.h" in one of your source files or have
//  defined FMOD_DYN_IMPL in more than one of your source files.
//
//  Forgetting to "unguard" will cause unmatched braces.  When coding, it is
//  good practice to type both the guard and unguard at the same time with the
//  same indent.  Indenting the guarded code will make your code more readable.
//
//  Auto-load not working?  Make sure you have not defined FMOD_DYN_NOCLASSES.
//  The auto-load feature is only available to C++ compilers.  Also, make sure
//  that you do not just define FMOD_DYN_AUTOLOAD.  It needs to be defined as
//  a string with the name of the library to load.
//
//  Any other problems?  First check to see if you are having a problem with
//  FMOD itself.  Check its documentation, or check the forum online.  When
//  all else fails or you know it's a problem with fmod_dyn, send me an email
//  describing the problem.  Please do not send me attachments with source
//  code or binary, unless I agree to look at it.  Thanks.
// -----------------------------------------------------------------------------

#ifndef _FMOD_DYN_H_
#define _FMOD_DYN_H_

// Use the "mini" version.
#include "fmod_mini.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

#ifdef WIN32

// We need the _ASSERT macro and some Win32 APIs.
#include <windows.h>

// The name of the library.
#define FMOD_DLL "fmod.dll"

// Our safety function.
#if !defined(FMOD_DYN_NOASSERT) && defined(_ASSERT_BASE)
#define FMOD_DYN_SAFETY(func) _ASSERT_BASE(0,#func)
#else /* !defined(FMOD_DYN_NOASSERT) && defined(_ASSERT_BASE) */
#define FMOD_DYN_SAFETY(func) ((void)0)
#endif /* !defined(FMOD_DYN_NOASSERT) && defined(_ASSERT_BASE) */

// Type for loaded modules.
typedef HMODULE FMOD_DYN_MODULE;

// Platform-specific methods.
FMOD_DYN_MODULE myLoadLibrary(LPCSTR library)
	{ return LoadLibraryA(library); }
void myFreeLibrary(FMOD_DYN_MODULE module)
	{ FreeLibrary(module); }
void *myGetProcAddress(FMOD_DYN_MODULE module, LPCSTR symbol)
	{ return GetProcAddress(module, symbol); }

#else /* WIN32 */

#error Run-time dynamic linking on non-Win32 platforms is not currently supported.

#endif /* WIN32 */

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

#ifndef FMOD_DYN_NOSAFETY

#ifdef FMOD_DYN_IMPL
#define FMOD_DYN_FUNC(sym,func,ret,val,args) ret F_API safe##func args { FMOD_DYN_SAFETY(func); return val; }
#else /* FMOD_DYN_IMPL */
#define FMOD_DYN_FUNC(sym,func,ret,val,args) ret F_API safe##func args;
#endif /* FMOD_DYN_IMPL */

#include "fmod_funcs.h"
#undef FMOD_DYN_FUNC

#endif /* FMOD_DYN_NOSAFETY */

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

#ifdef FMOD_DYN_IMPL

#ifndef FMOD_DYN_NOSAFETY
#define FMOD_DYN_FUNC(sym,func,ret,val,args) ret (F_API *func) args = safe##func;
#else /* FMOD_DYN_NOSAFETY */
#define FMOD_DYN_FUNC(sym,func,ret,val,args) ret (F_API *func) args = NULL;
#endif /* FMOD_DYN_NOSAFETY */

#else /* FMOD_DYN_IMPL */

#define FMOD_DYN_FUNC(sym,func,ret,val,args) extern ret (F_API *func) args;

#endif /* FMOD_DYN_IMPL */

#include "fmod_funcs.h"
#undef FMOD_DYN_FUNC

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

#define FMOD_DYN_GETVERSION		((NULL != FSOUND_GetVersion) ? FSOUND_GetVersion() : 0.0f)

#define FMOD_DYN_LOADED			(NULL != g_fmod)
#define FMOD_DYN_PARTIAL		(0 != g_fmodPartial)
#define FMOD_DYN_BADVERSION		(FMOD_VERSION > FMOD_DYN_GETVERSION)

#define FMOD_DYN_FULLYLOADED	(FMOD_DYN_LOADED && !FMOD_DYN_PARTIAL && !FMOD_DYN_BADVERSION)

#define FMOD_DYN_SOFTGUARD		if (FMOD_DYN_LOADED) {
#define FMOD_DYN_GUARD			if (FMOD_DYN_FULLYLOADED) {

#define FMOD_DYN_UNGUARD		}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

#ifndef FMOD_DYN_IMPL

extern FMOD_DYN_MODULE	g_fmod;
extern signed char		g_fmodPartial;

signed char LoadFMOD(LPCSTR library);
void UnloadFMOD();

#else /* FMOD_DYN_IMPL */

FMOD_DYN_MODULE	g_fmod			= NULL;
signed char		g_fmodPartial	= 0;

signed char LoadFMOD(LPCSTR library)
{
	// Ignore multiple calls.
	if (FMOD_DYN_LOADED)
		return 0;

	// Load the library.	
	if (NULL == (g_fmod = myLoadLibrary(library)))
		return 0;
	
	// Get the procedure addresses.
	g_fmodPartial = 0;
	void *temp = NULL;
	
	#define FMOD_DYN_FUNC(sym,func,ret,val,args)			\
		if (NULL != (temp = myGetProcAddress(g_fmod,sym)))	\
			func = (ret (F_API *)args)temp;					\
		else												\
			g_fmodPartial = 1;
	#include "fmod_funcs.h"
	#undef FMOD_DYN_FUNC

	// Finished.
	return FMOD_DYN_FULLYLOADED;
}
void UnloadFMOD()
{
	// Free the library, if needed.
	if (FMOD_DYN_LOADED)
	{
		myFreeLibrary(g_fmod);
		g_fmod = NULL;
	}

	// Reset the function pointers.
	#ifndef FMOD_DYN_NOSAFETY
	#define FMOD_DYN_FUNC(sym,func,ret,val,args) func = safe##func;
	#else /* FMOD_DYN_NOSAFETY */
	#define FMOD_DYN_FUNC(sym,func,ret,val,args) func = NULL;
	#endif /* FMOD_DYN_NOSAFETY */
	
	#include "fmod_funcs.h"
	#undef FMOD_DYN_FUNC
}

#endif /* FMOD_DYN_IMPL */

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

#if defined(__cplusplus) && !defined(FMOD_DYN_NOCLASSES)

#ifdef FMOD_DYN_AUTOLOAD

class AutoLoadFmod
{
	public:
		AutoLoadFmod() { LoadFMOD(FMOD_DYN_AUTOLOAD); }
		~AutoLoadFmod() { UnloadFMOD(); }
};

#ifdef FMOD_DYN_IMPL
static AutoLoadFmod g_autoLoadFmod;
#endif /* FMOD_DYN_IMPL */

#endif /* FMOD_DYN_AUTOLOAD */

#endif /* defined(__cplusplus) && !defined(FMOD_DYN_NOCLASSES) */

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

#endif /* _FMOD_DYN_H_ */