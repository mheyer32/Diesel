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

#pragma once
#ifndef TIMER_H_INCLUDED
#define TIMER_H_INCLUDED

#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#include <windows.h>

/** emulates a stopwatch.
use start() to start the counting, stop() to stop. Then get the seconds or tics that passed between
Start() and Stop() via getElapsedSecs() or getElapsedTics()
*/
class CTimer
{
public:
    CTimer();
    ~CTimer();

    inline void CTimer::start()
    {
        QueryPerformanceCounter(&m_start.qpc);
        m_tics_elapsed = 0;
        m_paused       = false;
    }

    inline void CTimer::stop()
    {
        if (!m_paused) {
            QueryPerformanceCounter(&m_stop.qpc);
            m_tics_elapsed += m_stop.tics - m_start.tics;
            m_start = m_stop;
        }
        m_secs_elapsed = ((m_tics_elapsed << 14) / s_frequency) / 16384.0f;
        m_paused       = false;
    }

    inline void CTimer::pause()
    {
        if (!m_paused) {
            QueryPerformanceCounter(&m_stop.qpc);
            m_tics_elapsed += m_stop.tics - m_start.tics;
            m_secs_elapsed = ((m_tics_elapsed << 14) / s_frequency) / 16384.0f;
            m_paused       = true;
        }
    }

    inline void CTimer::resume()
    {
        if (m_paused) {
            QueryPerformanceCounter(&m_start.qpc);
            m_paused = false;
        }
    }

    inline double getElapsedSecs() const { return m_secs_elapsed; }

    inline unsigned __int64 getElapsedTics() const { return m_tics_elapsed; }

private:
    union Tics
    {
        LARGE_INTEGER    qpc;
        unsigned __int64 tics;
    };

    Tics             m_start, m_stop;
    unsigned __int64 m_tics_elapsed;
    float            m_secs_elapsed;
    bool             m_paused;

    static bool             s_timing_calibrated;
    static unsigned __int64 s_frequency;
};

#endif
