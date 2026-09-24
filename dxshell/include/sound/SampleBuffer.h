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
