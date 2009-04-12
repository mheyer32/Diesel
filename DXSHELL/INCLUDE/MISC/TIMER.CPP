/*
(c) 2002-2006 by Mathias Heyer
*/

#include "misc/Timer.h"
#include "misc/Exception.h"

#include <cassert>


bool				CTimer::s_timing_calibrated=false;
unsigned __int64	CTimer::s_frequency;

CTimer::CTimer()
{
	m_start.tics=0;
	m_stop.tics=0;
	
	m_tics_elapsed=0;
	m_secs_elapsed=0.0;
	m_paused=false;

	if (s_timing_calibrated) return;

	Tics freqency;
	if (!QueryPerformanceFrequency(&freqency.qpc))
		throw CException("CTimer::CTimer() \nHigh Resolution CTimer not supported?");

	s_frequency=freqency.tics;

	s_timing_calibrated=true;
}

CTimer::~CTimer()
{

}



