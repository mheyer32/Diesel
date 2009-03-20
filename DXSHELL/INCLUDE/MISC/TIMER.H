
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
// Timer.h: Schnittstelle für die Klasse CTimer.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIMER_H__4D600921_2475_11D4_BD8A_0000E85E86C1__INCLUDED_)
#define AFX_TIMER_H__4D600921_2475_11D4_BD8A_0000E85E86C1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/** uses Win32 QueryPerformanceCounter to emulate some kind of stopwatch. 
use start() to start the counting, stop() to stop. Then get the seconds or tics that passed between
Start() and Stop() via getSECPassed() or getTICPassed()
*/
class CTimer  
{
public:

#if defined(_MSC_VER)
	typedef __int64 TICKCOUNT;
#elif defined(__linux)
	typedef unsigned long long TICKCOUNT;
#endif

	CTimer();
	~CTimer();


	inline void CTimer::start()
	{
		// QueryPerformanceCounter(&start_tics);
		start_tics=readTSC();
		 tics_passed=0;
		 paused=false;
	}

	inline void CTimer::stop()
	{
	  if (!paused)
	  {
		 // QueryPerformanceCounter(&stop_tics);
		  stop_tics=readTSC();
		  tics_passed+=stop_tics-start_tics;
		  start_tics=stop_tics;
	  }
	  secs_passed=(double)tics_passed*sec_tic_ratio;
	  paused=false;
	}

	inline void CTimer::pause()
	{
		if (!paused)
		{
			//QueryPerformanceCounter(&stop_tics);
			stop_tics=readTSC();
			tics_passed+=stop_tics-start_tics;
			secs_passed=(double)tics_passed*sec_tic_ratio;
			paused=true;
		}
	}

	inline void CTimer::resume()
	{
		if (paused)
		{
			//QueryPerformanceCounter(&start_tics);	
			start_tics=readTSC();
			paused=false;
		}
	}

	inline double CTimer::getSECPassed()
	{
	  return secs_passed;
	}

	inline TICKCOUNT CTimer::getTICPassed()
	{
		return tics_passed;
	}

	inline TICKCOUNT CTimer::SECStoTICS(double secs)
	{
		return (TICKCOUNT)(sec_tic_ratio*secs);
	}

	inline double CTimer::TICStoSECS(TICKCOUNT tics)
	{
		return tic_sec_ratio*(double)tics;
	}

private:

	TICKCOUNT start_tics,stop_tics;
	TICKCOUNT tics_passed;
	double		 secs_passed;
	bool		paused;

	static bool	  timing_calibrated;
	static double sec_tic_ratio;
	static double tic_sec_ratio;

	inline TICKCOUNT readTSC( void ) const
	{
		TICKCOUNT volatile tsc;

#	if defined(_MSC_VER)
		_asm
		{
			xor eax, eax
				xor edx, edx
				///\todo do we need this
				// cpuid
				rdtsc				// returns the time stamp counter in edx:eax
				mov dword ptr tsc, eax
				mov dword ptr tsc+4, edx
		}
#	elif defined(__linux)
		__asm__ volatile (".byte 0x0f, 0x31" : "=A" (tsc) );
#	endif

		return tsc;
	}

};

#endif // !defined(AFX_TIMER_H__4D600921_2475_11D4_BD8A_0000E85E86C1__INCLUDED_)
