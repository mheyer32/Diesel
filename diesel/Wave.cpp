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
#include "Wave.h"

#include <defs.h>
#include <stdlib.h>
#include <string.h>

CWave::CWave(void)
: waveform(WF_SIN)
, wavefunc(&CWave::sin)
, base(0.0f)
, amplitude(0.0f)
, freq(0.0f)
{
}
CWave::CWave(WAVEFORM nform, float nbase, float namplitude, float nphase, float nfreq)
{
    setParams(nform, nbase, namplitude, nphase, nfreq);
}
CWave::~CWave(void)
{
}

void CWave::setParams(WAVEFORM form, float nbase, float namplitude, float nphase, float nfreq)
{
    wavefunc  = funcForWaveform(form);
    base      = nbase;
    amplitude = namplitude;
    phase     = nphase;
    freq      = nfreq;
}

float CWave::sin(float time) const
{
    time = phase + freq * time;
    time -= floorf(time);
    time = sinf(time * TWOPI);
    return base + amplitude * time;
}
float CWave::triangle(float time) const
{
    time = phase + freq * time;
    time -= floorf(time);
    time = (time < 0.5f) ? 4.0f * time - 1.0f : 3.0f - 4.0f * time;
    return base + amplitude * time;
}
float CWave::sawtooth(float time) const
{
    time = phase + freq * time;
    time -= floorf(time);
    return base + amplitude * time;
}
float CWave::invsawtooth(float time) const
{
    time = phase + freq * time;
    time -= floorf(time);
    time = 1.0f - time;
    return base + amplitude * time;
}
float CWave::square(float time) const
{
    time = phase + freq * time;
    time -= floorf(time);
    time = (time < 0.5) ? 1.0f : -1.0f;
    return base + amplitude * time;
}
float CWave::noise(float time) const
{
    time = phase + freq * time;
    time -= floorf(time);
    time = (float)rand() * (1.0f / (float)RAND_MAX);
    return base + amplitude * time;
}

CWave::WAVEFORM CWave::getWaveForm(const char* wavename)
{
    if (EQ(wavename, "sin"))
        return WF_SIN;
    else if (EQ(wavename, "square"))
        return WF_SQUARE;
    else if (EQ(wavename, "sawtooth"))
        return WF_SAWTOOTH;
    else if (EQ(wavename, "inversesawtooth"))
        return WF_INVERSESAWTOOTH;
    else if (EQ(wavename, "triangle"))
        return WF_TRIANGLE;
    else if (EQ(wavename, "noise"))
        return WF_NOISE;

    return WF_SIN;  // FIXME: give warning ?
}

CWave::WAVEFUNC CWave::funcForWaveform(WAVEFORM form)
{
    switch (form) {
    case WF_SIN:
        return &CWave::sin;
    case WF_SQUARE:
        return &CWave::square;
    case WF_SAWTOOTH:
        return &CWave::sawtooth;
    case WF_INVERSESAWTOOTH:
        return &CWave::invsawtooth;
    case WF_TRIANGLE:
        return &CWave::triangle;
    case WF_NOISE:
        return &CWave::noise;
    }
    return NULL;
}