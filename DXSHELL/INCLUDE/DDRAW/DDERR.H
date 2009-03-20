#ifndef _DDERR_H
#define _DDERR_H

#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#include <windows.h> // for HRESULT

const char* DDErrorToString(HRESULT error);

#endif