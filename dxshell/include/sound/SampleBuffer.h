/*
This file is part of DXShell
(c) 2002-2026 by Mathias Heyer
*/
#pragma once

#include <al.h>

#include <adt/RefCount.h>
#include <adt/SmartPointer.h>
#include <misc/Exception.h>

class SoundDevice;

class SampleBuffer : public CRefCount
{
public:
    typedef SmartPointer<SampleBuffer> SMARTPTR;

    void setBufferData(ALenum format, const ALvoid* data, ALsizei size, ALsizei freq);

    size_t   getSize() const { return m_size; }
    float    getDuration() const;
    ALenum   getFormat() const { return m_format; }
    unsigned getFrequency() const { return m_frequency; }
    unsigned getBits() const;
    unsigned getChannels() const;

    ALuint getBufferId() const { return m_buffer; }

protected:
    virtual ~SampleBuffer();

private:
    friend class SoundDevice;

    explicit SampleBuffer(SoundDevice& device);

    SampleBuffer(const SampleBuffer&);
    SampleBuffer& operator=(const SampleBuffer&);

    ALuint  m_buffer;
    ALenum  m_format;
    ALsizei m_size;
    ALsizei m_frequency;
};

typedef SampleBuffer::SMARTPTR SampleBufferPtr;
