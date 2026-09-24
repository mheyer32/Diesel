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
#include "SpeakerEmitter.h"

#include <sound/SoundSource.h>

SpeakerEmitter::SpeakerEmitter()
: m_position(0, 0, 0)
, m_flags(0)
, m_maxDistance(SOUND_MAXDIST)
, m_refDistance(SOUND_FULLVOLUME)
, m_state(STOP)
, m_areaLinks(0)
, m_cullFrame(-1)
{
}

SpeakerEmitter::~SpeakerEmitter()
{
    detachSource();
}

void SpeakerEmitter::play()
{
    m_state = PLAYING;
}

void SpeakerEmitter::stop()
{
    m_state = STOP;
    detachSource();
}

float SpeakerEmitter::channelGain() const
{
    // Q3 loop "sphere" channels use master_vol 90 vs 127 for oneshots
    if (m_flags & LOOPING)
        return SOUND_LOOP_VOL;
    return 1.0f;
}

bool SpeakerEmitter::isAudible(const VECTOR3& listenerPos)
{
    if (m_state != PLAYING || !m_buffer)
        return false;

    // oneshot finished on its OpenAL source — stay stopped so we don't retrigger
    if (!(m_flags & LOOPING) && m_source && m_source->getPlayState() == AL_STOPPED) {
        m_state = STOP;
        return false;
    }

    if (m_flags & GLOBAL)
        return true;

    VECTOR3 d      = listenerPos - m_position;
    float   distSq = d.x * d.x + d.y * d.y + d.z * d.z;
    return distSq <= m_maxDistance * m_maxDistance;
}

void SpeakerEmitter::applyGain(float masterVolume)
{
    if (m_source)
        m_source->set(AL_GAIN, channelGain() * masterVolume);
}

void SpeakerEmitter::attachSource(const SoundSourcePtr& source)
{
    if (m_source.getPointer() == source.getPointer())
        return;

    detachSource();
    m_source = source;
    if (!m_source || !m_buffer)
        return;

    m_source->setBuffer(m_buffer);
    m_source->setLooping((m_flags & LOOPING) != 0);
    m_source->set(AL_ROLLOFF_FACTOR, 1.0f);

    if (m_flags & GLOBAL) {
        m_source->set(AL_SOURCE_RELATIVE, AL_TRUE);
        m_source->set(AL_POSITION, VECTOR3(0, 0, 0));
        m_source->set(AL_REFERENCE_DISTANCE, 1.0f);
        m_source->set(AL_MAX_DISTANCE, 100000.0f);
        m_source->set(AL_ROLLOFF_FACTOR, 0.0f);
    } else {
        // AL_LINEAR_DISTANCE_CLAMPED with ref=80, max=1330, rolloff=1
        // matches Q3: gain = 1 - (dist - 80) * 0.0008
        m_source->set(AL_SOURCE_RELATIVE, AL_FALSE);
        VECTOR3 meterPos(m_position.x * Q3_TO_METERS, m_position.y * Q3_TO_METERS,
                         m_position.z * Q3_TO_METERS);
        m_source->set(AL_POSITION, meterPos);
        m_source->set(AL_REFERENCE_DISTANCE, m_refDistance * Q3_TO_METERS);
        m_source->set(AL_MAX_DISTANCE, m_maxDistance * Q3_TO_METERS);
    }

    m_source->play();
}

void SpeakerEmitter::detachSource()
{
    if (m_source) {
        m_source->stop();
        m_source->setBuffer(SampleBufferPtr());
        m_source = 0;
    }
}
