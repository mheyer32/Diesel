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
#include "sound/SoundDevice.h"
#include "sound/SampleBuffer.h"
#include "sound/SoundSource.h"

#include <cassert>
#include <iostream>
#include <sstream>

using namespace std;

void ALErrorImpl(const char* text, const char* filename, int linenr)
{
    ALenum error = alGetError();
    if (error != AL_NO_ERROR) {
        ostringstream outtext;
        outtext << "OpenAL error:" << text << '(' << filename << ':' << linenr << ") : " << ALErrorToString(error);
        cerr << outtext.str() << endl;
#ifndef NDEBUG
        __debugbreak();
#endif
        throw ALException(outtext.str());
    }
}

void ALCErrorImpl(ALCdevice* device, const char* text, const char* filename, int linenr)
{
    assert(device);
    ALCenum error = alcGetError(device);
    if (error != ALC_NO_ERROR) {
        ostringstream outtext;
        outtext << "OpenAL error:" << text << '(' << filename << ':' << linenr << ") : " << ALCErrorToString(error);
        cerr << outtext.str() << endl;
#ifndef NDEBUG
        __debugbreak();
#endif
        throw ALCException(outtext.str(), error);
    }
}

const char* ALCErrorToString(ALCenum error)
{
    return alcGetString(NULL, error);
}

const char* ALErrorToString(ALenum error)
{
    return alGetString(error);
}

SoundDevice::SoundDevice(const std::string& devicename)
: m_aldevice(NULL)
, m_alcontext(NULL)
, m_listenervelocity(0.0f, 0.0f, 0.0f)
{
    cout << endl << ">>>>>>>> SoundDevice Initialization >>>>>>>>" << endl << endl;

    m_aldevice = alcOpenDevice(devicename.empty() ? NULL : devicename.c_str());
    if (!m_aldevice)
        throw CException("SoundDevice: could not open OpenAL device");

    m_alcontext = alcCreateContext(m_aldevice, NULL);
    if (!m_alcontext)
        throw ALCException("SoundDevice: could not create OpenAL context", m_aldevice);

    alcMakeContextCurrent(m_alcontext);
    ALCVERIFY(m_aldevice, "SoundDevice::SoundDevice() alcMakeContextCurrent()");

    cout << "OpenAL Renderer: " << alGetString(AL_RENDERER) << endl;
    cout << "OpenAL Vendor  : " << alGetString(AL_VENDOR) << endl;
    cout << "OpenAL Version : " << alGetString(AL_VERSION) << endl;
    ALVERIFY("SoundDevice::SoundDevice()");

    alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);
    alSpeedOfSound(343.3f);

    cout << "<<<<<<<< SoundDevice Initialization <<<<<<<<" << endl << endl;
}

SoundDevice::~SoundDevice()
{
    cout << endl << ">>>>>>>> SoundDevice Shutdown >>>>>>>>" << endl << endl;

    if (m_alcontext) {
        if (alcGetCurrentContext() == m_alcontext)
            alcMakeContextCurrent(NULL);
        alcDestroyContext(m_alcontext);
        m_alcontext = NULL;
    }
    if (m_aldevice) {
        alcCloseDevice(m_aldevice);
        m_aldevice = NULL;
    }

    cout << "<<<<<<<< SoundDevice Shutdown <<<<<<<<" << endl << endl;
}

SoundSourcePtr SoundDevice::createSource()
{
    return SoundSourcePtr(new SoundSource(*this));
}

SampleBufferPtr SoundDevice::createSampleBuffer()
{
    return SampleBufferPtr(new SampleBuffer(*this));
}

SampleBufferPtr SoundDevice::createSampleBuffer(ALenum format, const ALvoid* data, ALsizei size, ALsizei freq)
{
    SampleBufferPtr buffer = createSampleBuffer();
    buffer->setBufferData(format, data, size, freq);
    return buffer;
}

void SoundDevice::setListenerPosition(const MATRIX4& pos)
{
    m_listenerposition = pos;

    VECTOR3 meter_position = (VECTOR3&)pos.a[3];
    meter_position.x *= Q3_TO_METERS;
    meter_position.y *= Q3_TO_METERS;
    meter_position.z *= Q3_TO_METERS;
    alListenerfv(AL_POSITION, &meter_position.x);

    VECTOR3 forward = (VECTOR3&)pos.a[2];
    VECTOR3 up      = (VECTOR3&)pos.a[1];
    float   orientation[6] = {-forward.x, -forward.y, -forward.z, up.x, up.y, up.z};
    alListenerfv(AL_ORIENTATION, orientation);
    ALERROR("SoundDevice::setListenerPosition()");
}

const MATRIX4& SoundDevice::getListenerPosition() const
{
    return m_listenerposition;
}

void SoundDevice::setListenerVelocity(const VECTOR3& velocity)
{
    m_listenervelocity = velocity;
    VECTOR3 velocity_in_meter(velocity.x * Q3_TO_METERS, velocity.y * Q3_TO_METERS, velocity.z * Q3_TO_METERS);
    alListenerfv(AL_VELOCITY, &velocity_in_meter.x);
    ALERROR("SoundDevice::setListenerVelocity()");
}

const VECTOR3& SoundDevice::getListenerVelocity() const
{
    return m_listenervelocity;
}

void SoundDevice::setListenerParameter(ALenum param, ALfloat value)
{
    alListenerf(param, value);
    ALERROR("SoundDevice::setListenerParameter()");
}

ALfloat SoundDevice::getListenerParameterf(ALenum param) const
{
    ALfloat value = 0.0f;
    alGetListenerf(param, &value);
    ALERROR("SoundDevice::getListenerParameterf()");
    return value;
}

void SoundDevice::setListenerParameter(ALenum param, ALint value)
{
    alListeneri(param, value);
    ALERROR("SoundDevice::setListenerParameter()");
}

ALint SoundDevice::getListenerParameteri(ALenum param) const
{
    ALint value = 0;
    alGetListeneri(param, &value);
    ALERROR("SoundDevice::getListenerParameteri()");
    return value;
}

void SoundDevice::setVolume(float gain)
{
    setListenerParameter(AL_GAIN, gain);
}

void SoundDevice::process()
{
    alcProcessContext(m_alcontext);
}

void SoundDevice::suspend()
{
    alcSuspendContext(m_alcontext);
}
