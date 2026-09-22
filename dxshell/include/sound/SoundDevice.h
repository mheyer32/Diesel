/*
This file is part of DXShell
(c) 2002-2026 by Mathias Heyer
*/
#pragma once

#include <al.h>
#include <alc.h>

#include <math/Matrix4.h>
#include <math/Vector3.h>
#include <misc/Exception.h>

#include "sound/SampleBuffer.h"
#include "sound/SoundSource.h"

#include <string>

// Quake / id Tech units (inches) to meters
const float Q3_TO_METERS   = 0.0254f;
const float METERS_TO_Q3   = (1.0f / Q3_TO_METERS);

// Quake3 snd_dma.c attenuation (game units)
const float SOUND_FULLVOLUME = 80.0f;
const float SOUND_ATTENUATE  = 0.0008f;
const float SOUND_MAXDIST    = (SOUND_FULLVOLUME + 1.0f / SOUND_ATTENUATE);  // ~1330
const float SOUND_LOOP_VOL   = (90.0f / 127.0f);

class SoundDevice
{
public:
    explicit SoundDevice(const std::string& devicename = std::string());
    ~SoundDevice();

    void process();
    void suspend();

    void               setListenerPosition(const MATRIX4& pos);
    const MATRIX4&     getListenerPosition() const;
    void               setListenerVelocity(const VECTOR3& velocity);
    const VECTOR3&     getListenerVelocity() const;

    void    setListenerParameter(ALenum param, ALfloat value);
    void    setListenerParameter(ALenum param, ALint value);
    ALfloat getListenerParameterf(ALenum param) const;
    ALint   getListenerParameteri(ALenum param) const;

    void setVolume(float gain);

    SoundSourcePtr  createSource();
    SampleBufferPtr createSampleBuffer();
    SampleBufferPtr createSampleBuffer(ALenum format, const ALvoid* data, ALsizei size, ALsizei freq);

    ALCdevice* getALCDevice() const { return m_aldevice; }

private:
    SoundDevice(const SoundDevice&);
    SoundDevice& operator=(const SoundDevice&);

    ALCdevice*  m_aldevice;
    ALCcontext* m_alcontext;

    MATRIX4 m_listenerposition;
    VECTOR3 m_listenervelocity;
};

const char* ALCErrorToString(ALCenum error);
const char* ALErrorToString(ALenum error);

class ALException : public CException
{
public:
    explicit ALException(const std::string& text)
    : CException(text)
    , m_error(AL_NO_ERROR)
    {
    }
    ALException(const std::string& text, ALenum error)
    : CException(text, ALErrorToString(error))
    , m_error(error)
    {
    }

    ALenum getError() const { return m_error; }

private:
    ALenum m_error;
};

class ALCException : public CException
{
public:
    explicit ALCException(const std::string& text)
    : CException(text)
    , m_error(ALC_NO_ERROR)
    {
    }
    ALCException(const std::string& text, ALCenum error)
    : CException(text, ALCErrorToString(error))
    , m_error(error)
    {
    }
    ALCException(const std::string& text, ALCdevice* device)
    : CException(text)
    , m_error(alcGetError(device))
    {
        addText(ALCErrorToString(m_error));
    }

    ALCenum getError() const { return m_error; }

private:
    ALCenum m_error;
};

void ALErrorImpl(const char* text, const char* filename, int linenr);
#define ALVERIFY(text) ALErrorImpl(text, __FILE__, __LINE__)
#ifndef NDEBUG
#define ALERROR(text) ALVERIFY(text)
#else
#define ALERROR(text)
#endif

void ALCErrorImpl(ALCdevice* device, const char* text, const char* filename, int linenr);
#define ALCVERIFY(device, text) ALCErrorImpl(device, text, __FILE__, __LINE__)
#ifndef NDEBUG
#define ALCERROR(device, text) ALCVERIFY(device, text)
#else
#define ALCERROR(device, text)
#endif
