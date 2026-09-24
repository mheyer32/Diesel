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
#include "sound/WavLoader.h"
#include "sound/SampleBuffer.h"

#include <file/FileManager.h>

#include <iostream>
#include <vector>

#include <windows.h>
#include <mmsystem.h>

#ifndef MAKEFOURCC
#define MAKEFOURCC(ch0, ch1, ch2, ch3)                                                                                 \
    ((DWORD)(BYTE)(ch0) | ((DWORD)(BYTE)(ch1) << 8) | ((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24))
#endif

using namespace std;

SampleBufferPtr loadWAV(SoundDevice& device, const std::string& filename)
{
    struct
    {
        DWORD chunkname;
        DWORD chunksize;
        DWORD wavefile;
    } RIFFHeader;

    struct
    {
        DWORD chunkname;
        DWORD chunksize;
    } ChunkHeader;

    CFile* file = CFileManager::Instance()->open(filename);
    if (!file)
        return SampleBufferPtr();

    file->readVOID(&RIFFHeader, sizeof(RIFFHeader));

    if ((RIFFHeader.chunkname != MAKEFOURCC('R', 'I', 'F', 'F')) ||
        (RIFFHeader.wavefile != MAKEFOURCC('W', 'A', 'V', 'E'))) {
        cout << "WARNING: loadWAV() file not a RIFF/WAVE-File: " << filename << endl;
        file->close();
        delete file;
        return SampleBufferPtr();
    }

    ChunkHeader.chunksize = 0;
    do {
        file->setPosition(file->getPosition() + ChunkHeader.chunksize);
        file->readVOID(&ChunkHeader, sizeof(ChunkHeader));
    } while (ChunkHeader.chunkname != MAKEFOURCC('f', 'm', 't', ' '));

    WAVEFORMATEX wfxFormat;
    ZeroMemory(&wfxFormat, sizeof(wfxFormat));
    if (ChunkHeader.chunksize > sizeof(wfxFormat)) {
        file->readVOID(&wfxFormat, sizeof(wfxFormat));
        file->setPosition(file->getPosition() + (ChunkHeader.chunksize - sizeof(wfxFormat)));
    } else {
        file->readVOID(&wfxFormat, ChunkHeader.chunksize);
    }

    if (wfxFormat.nChannels > 2 || (wfxFormat.wBitsPerSample != 8 && wfxFormat.wBitsPerSample != 16)) {
        cout << "WARNING: loadWAV() unsupported sample format: " << filename << endl;
        file->close();
        delete file;
        return SampleBufferPtr();
    }

    ALenum format = 0;
    if (wfxFormat.nChannels == 1) {
        format = (wfxFormat.wBitsPerSample == 8) ? AL_FORMAT_MONO8 : AL_FORMAT_MONO16;
    } else {
        format = (wfxFormat.wBitsPerSample == 8) ? AL_FORMAT_STEREO8 : AL_FORMAT_STEREO16;
    }

    ChunkHeader.chunksize = 0;
    do {
        file->setPosition(file->getPosition() + ChunkHeader.chunksize);
        file->readVOID(&ChunkHeader, sizeof(ChunkHeader));
    } while (ChunkHeader.chunkname != MAKEFOURCC('d', 'a', 't', 'a'));

    vector<char> wavdata(ChunkHeader.chunksize);
    if (ChunkHeader.chunksize)
        file->readVOID(&wavdata[0], ChunkHeader.chunksize);

    file->close();
    delete file;

    return device.createSampleBuffer(format, wavdata.empty() ? NULL : &wavdata[0], (ALsizei)wavdata.size(),
                                     wfxFormat.nSamplesPerSec);
}
