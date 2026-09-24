/*
This file is part of Diesel
(c) 2002-2026 by Mathias Heyer
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

#include "misc/Exception.h"
#include "misc/Timer.h"

#include <cassert>

bool             CTimer::s_timing_calibrated = false;
unsigned __int64 CTimer::s_frequency;

CTimer::CTimer()
{
    m_start.tics = 0;
    m_stop.tics  = 0;

    m_tics_elapsed = 0;
    m_secs_elapsed = 0.0;
    m_paused       = false;

    if (s_timing_calibrated)
        return;

    Tics freqency;
    if (!QueryPerformanceFrequency(&freqency.qpc))
        throw CException("CTimer::CTimer() \nHigh Resolution CTimer not supported?");

    s_frequency = freqency.tics;

    s_timing_calibrated = true;
}

CTimer::~CTimer()
{
}
