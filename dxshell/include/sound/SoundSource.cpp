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
#include "sound/SoundSource.h"
#include "sound/SampleBuffer.h"
#include "sound/SoundDevice.h"

#include <cassert>

SoundSource::SoundSource(SoundDevice& device)
: m_device(device)
, m_alsource(0)
{
    alGenSources(1, &m_alsource);
    ALVERIFY("SoundSource::SoundSource() alGenSources()");

    set(AL_REFERENCE_DISTANCE, SOUND_FULLVOLUME * Q3_TO_METERS);
    set(AL_MAX_DISTANCE, SOUND_MAXDIST * Q3_TO_METERS);
    set(AL_ROLLOFF_FACTOR, 1.0f);
    set(AL_MAX_GAIN, 1.0f);
    set(AL_MIN_GAIN, 0.0f);
}

SoundSource::~SoundSource()
{
    stop();
    alSourcei(m_alsource, AL_BUFFER, AL_NONE);
    m_buffer = 0;
    m_queuedbuffers.clear();
    alDeleteSources(1, &m_alsource);
    ALVERIFY("SoundSource::~SoundSource");
}

void SoundSource::play()
{
    alSourcePlay(m_alsource);
    ALERROR("SoundSource::play()");
}

void SoundSource::stop()
{
    alSourceStop(m_alsource);
    ALERROR("SoundSource::stop()");
}

void SoundSource::pause()
{
    alSourcePause(m_alsource);
    ALERROR("SoundSource::pause()");
}

void SoundSource::rewind()
{
    alSourceRewind(m_alsource);
    ALERROR("SoundSource::rewind()");
}

void SoundSource::setBuffer(const SampleBufferPtr& buffer)
{
    stop();
    m_buffer = buffer;
    if (buffer) {
        alSourcei(m_alsource, AL_BUFFER, (ALint)buffer->getBufferId());
    } else {
        alSourcei(m_alsource, AL_BUFFER, AL_NONE);
    }
    ALERROR("SoundSource::setBuffer()");
}

void SoundSource::queueBuffer(const SampleBufferPtr& buffer)
{
    ALuint bufferid = buffer->getBufferId();
    alSourceQueueBuffers(m_alsource, 1, &bufferid);
    ALERROR("SoundSource::queueBuffer()");
    m_queuedbuffers.push_back(buffer);
}

void SoundSource::unqueueBuffer(const SampleBufferPtr& buffer)
{
    ALuint bufferid = buffer->getBufferId();
    alSourceUnqueueBuffers(m_alsource, 1, &bufferid);
    ALERROR("SoundSource::unqueueBuffer()");

    std::list<SampleBufferPtr>::iterator bIt = m_queuedbuffers.begin();
    for (; bIt != m_queuedbuffers.end(); ++bIt) {
        if ((*bIt).getPointer() == buffer.getPointer()) {
            m_queuedbuffers.erase(bIt);
            return;
        }
    }
    assert(0);
}

SampleBufferPtr SoundSource::getBuffer() const
{
    return m_buffer;
}

void SoundSource::set(ALenum param, ALint value)
{
    alSourcei(m_alsource, param, value);
    ALERROR("SoundSource::set()");
}

void SoundSource::set(ALenum param, ALfloat value)
{
    alSourcef(m_alsource, param, value);
    ALERROR("SoundSource::set()");
}

void SoundSource::set(ALenum param, const VECTOR3& value)
{
    alSourcefv(m_alsource, param, &value.x);
    ALERROR("SoundSource::set()");
}

ALint SoundSource::getState() const
{
    ALint state = 0;
    alGetSourcei(m_alsource, AL_SOURCE_TYPE, &state);
    ALERROR("SoundSource::getState()");
    return state;
}

ALint SoundSource::getPlayState() const
{
    ALint state = 0;
    alGetSourcei(m_alsource, AL_SOURCE_STATE, &state);
    ALERROR("SoundSource::getPlayState()");
    return state;
}

void SoundSource::setLooping(bool looping)
{
    alSourcei(m_alsource, AL_LOOPING, looping ? AL_TRUE : AL_FALSE);
    ALERROR("SoundSource::setLooping()");
}

bool SoundSource::isLooping() const
{
    ALint looping = 0;
    alGetSourcei(m_alsource, AL_LOOPING, &looping);
    ALERROR("SoundSource::isLooping()");
    return looping != 0;
}
