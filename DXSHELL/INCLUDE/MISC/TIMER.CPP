
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
// Timer.cpp: Implementierung der Klasse CTimer.
//
//////////////////////////////////////////////////////////////////////

#include "Timer.h"

#include <misc/exception.h>

#define WIN32_LEAN_AND_MEAN
#define VC_EXTRA_LEAN
#include <Windows.h>
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
bool   CTimer::timing_calibrated=false;
double CTimer::sec_tic_ratio=1.0f;
double CTimer::tic_sec_ratio=1.0f;


CTimer::CTimer()
{
	start_tics=0;
	stop_tics=0;
	
	tics_passed=0;
	secs_passed=0.0;
	paused=false;

	if (timing_calibrated) return;


#ifdef _WIN32

	LARGE_INTEGER performanceCount1;
	if (!QueryPerformanceFrequency(&performanceCount1))
		throw CException("CTimer::CTimer() \nHigh Resolution Timer not supported?");

	QueryPerformanceCounter(&performanceCount1);
	TICKCOUNT tc1 = readTSC();
	Sleep(1000);		// sleep for 1000ms
	TICKCOUNT tc2 = readTSC();
	LARGE_INTEGER performanceCount2;
	QueryPerformanceCounter(&performanceCount2);
	performanceCount2.QuadPart -= performanceCount1.QuadPart;
	QueryPerformanceFrequency(&performanceCount1);
	double delta = (double)performanceCount2.QuadPart/(double)performanceCount1.QuadPart;

	sec_tic_ratio = delta / (tc2-tc1);
#else
	tc1 = readTSC();
	usleep (1000000);			// sleep for 1s
	tc2 = readTSC();
	sec_tic_ratio = 1.0 / double(tc2 - tc1);
#endif

	tic_sec_ratio=1.0f/sec_tic_ratio;

	timing_calibrated=true;

}

CTimer::~CTimer()
{

}

