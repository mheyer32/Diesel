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

#include <math.h>

class CWave
{
public:
    enum WAVEFORM
    {
        WF_SIN,
        WF_TRIANGLE,
        WF_SQUARE,
        WF_SAWTOOTH,
        WF_INVERSESAWTOOTH,
        WF_NOISE
    };

    CWave(void);
    CWave(WAVEFORM form, float base, float amplitude, float phase, float freq);
    ~CWave(void);

    inline float evaluate(float time) const { return (this->*wavefunc)(time); }

    inline float operator()(float time) const { return (this->*wavefunc)(time); }

    void setParams(WAVEFORM form, float base, float amplitude, float phase, float freq);

    static WAVEFORM getWaveForm(const char* wavename);

protected:
    typedef float (CWave::*WAVEFUNC)(float time) const;
    WAVEFUNC funcForWaveform(WAVEFORM form);

    float sin(float time) const;
    float triangle(float time) const;
    float sawtooth(float time) const;
    float invsawtooth(float time) const;
    float square(float time) const;
    float noise(float time) const;

    WAVEFORM waveform;
    WAVEFUNC wavefunc;
    float    base;
    float    amplitude;
    float    phase;
    float    freq;
};
