/*
This file is part of DXShell
(c) 2002-2026 by Mathias Heyer
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
