/*
This file is part of DXShell
(c) 2002-2026 by Mathias Heyer
*/
#pragma once

#include <al.h>

#include <adt/RefCount.h>
#include <adt/SmartPointer.h>
#include <math/Vector3.h>

#include "sound/SampleBuffer.h"

#include <list>

class SoundDevice;

class SoundSource : public CRefCount
{
public:
    typedef SmartPointer<SoundSource> SMARTPTR;

    void play();
    void stop();
    void pause();
    void rewind();

    void setLooping(bool looping);
    bool isLooping() const;

    void            setBuffer(const SampleBufferPtr& buffer);
    SampleBufferPtr getBuffer() const;

    void queueBuffer(const SampleBufferPtr& buffer);
    void unqueueBuffer(const SampleBufferPtr& buffer);

    void set(ALenum param, ALint value);
    void set(ALenum param, ALfloat value);
    void set(ALenum param, const VECTOR3& value);

    ALint getState() const;      // AL_UNDETERMINED, AL_STATIC, AL_STREAMING
    ALint getPlayState() const;  // AL_INITIAL, AL_PLAYING, AL_PAUSED, AL_STOPPED

    SoundDevice& getSoundDevice() const { return m_device; }
    ALuint       getSourceId() const { return m_alsource; }

protected:
    virtual ~SoundSource();

private:
    friend class SoundDevice;

    explicit SoundSource(SoundDevice& device);

    SoundSource(const SoundSource&);
    SoundSource& operator=(const SoundSource&);

    SoundDevice& m_device;
    ALuint       m_alsource;

    SampleBufferPtr            m_buffer;
    std::list<SampleBufferPtr> m_queuedbuffers;
};

typedef SoundSource::SMARTPTR SoundSourcePtr;
