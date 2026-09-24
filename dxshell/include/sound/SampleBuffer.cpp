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
#include "sound/SampleBuffer.h"
#include "sound/SoundDevice.h"

#include <iostream>

SampleBuffer::SampleBuffer(SoundDevice& /*device*/)
: m_buffer(0)
, m_format(0)
, m_size(0)
, m_frequency(0)
{
    ALERROR("SampleBuffer::SampleBuffer() dangling error");
    alGenBuffers(1, &m_buffer);
    ALVERIFY("SampleBuffer::SampleBuffer");
}

SampleBuffer::~SampleBuffer()
{
#ifndef NDEBUG
    ALERROR("SampleBuffer::~SampleBuffer() dangling error");
    std::cout << "deleting SampleBuffer " << m_buffer << std::endl;
#endif
    alDeleteBuffers(1, &m_buffer);
    ALVERIFY("SampleBuffer::~SampleBuffer");
}

void SampleBuffer::setBufferData(ALenum format, const ALvoid* data, ALsizei size, ALsizei freq)
{
    m_format    = format;
    m_size      = size;
    m_frequency = freq;
    alBufferData(m_buffer, format, data, size, freq);
    ALVERIFY("SampleBuffer::setBufferData");
}

unsigned SampleBuffer::getBits() const
{
    ALint bits = 0;
    alGetBufferi(m_buffer, AL_BITS, &bits);
    ALERROR("SampleBuffer::getBits()");
    return bits;
}

unsigned SampleBuffer::getChannels() const
{
    ALint channels = 0;
    alGetBufferi(m_buffer, AL_CHANNELS, &channels);
    ALERROR("SampleBuffer::getChannels()");
    return channels;
}

float SampleBuffer::getDuration() const
{
    switch (m_format) {
    case AL_FORMAT_MONO8:
        return (float)m_size / float(m_frequency);
    case AL_FORMAT_MONO16:
        return (float)(m_size / 2) / float(m_frequency);
    case AL_FORMAT_STEREO8:
        return (float)(m_size / 2) / float(m_frequency);
    case AL_FORMAT_STEREO16:
        return (float)(m_size / 4) / float(m_frequency);
    }

    throw CException("SampleBuffer::getDuration() unsupported buffer format.");
}
