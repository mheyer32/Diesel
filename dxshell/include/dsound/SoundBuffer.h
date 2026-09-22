
/*
This file is part of DXShell
(c) 2002 by Mathias Heyer

DXShell is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

DXShell is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
// Soundbuffer.h: Schnittstelle für die Klasse CSoundbuffer.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SOUNDBUFFER_H__6B77BCCD_79BF_11D3_BD8A_0000E85E86C1__INCLUDED_)
#define AFX_SOUNDBUFFER_H__6B77BCCD_79BF_11D3_BD8A_0000E85E86C1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER > 1000
#include "DirectSound.h"

#define ENTIREBUFFER DSBLOCK_ENTIREBUFFER
#define FROMWRITECURSOR DSBLOCK_FROMWRITECURSOR

#define PRIMARYBUFFER DSBCAPS_PRIMARYBUFFER
#define STATICBUFFER DSBCAPS_STATIC
#define STICKYBUFFER DSBCAPS_STICKYFOCUS
#define LOOPING DSBPLAY_LOOPING

// #define WM_DSSTREAM_DONE	WM_USER + 0x100	/* Make our own app messages */
// #define WM_DSSTREAM_DEBUG	WM_USER + 0x101
// #define WM_DSSTREAM_PROGRESS	WM_USER + 0x102

class CSoundbuffer
{
public:
    CSoundbuffer();
    CSoundbuffer(LPCSTR filename, DWORD Flags = 0);
    CSoundbuffer(DWORD buffersize, const WAVEFORMATEX& format,
                 DWORD dsbcaps = DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_CTRLVOLUME);
    virtual ~CSoundbuffer();

    bool create(DWORD buffersize, const WAVEFORMATEX& format,
                DWORD dsbcaps = DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_CTRLVOLUME);

    HRESULT play(bool loop);
    void    stop();
    HRESULT setVolume(float volume);

    HRESULT lock(DWORD WriteCursor = 0, DWORD Length = 0, DWORD Flags = DSBLOCK_ENTIREBUFFER);
    void    unlock();

    void* getPointer1() const { return AudioPtr1; };
    void* getPointer2() const { return AudioPtr2; };
    DWORD getLength1() const { return AudioBytes1; };
    DWORD getLength2() const { return AudioBytes2; };

    LPDIRECTSOUNDBUFFER getDSBuffer() const { return SoundBuffer; };

    bool loadWAV(LPCSTR filename, DWORD buffersize = 0, DWORD Flags = 0);
    void resetStreaming(DWORD Offset = 0);

    static const WAVEFORMATEX wfx16bit44khzstereo;
    static const WAVEFORMATEX wfx16bit44khzmono;
    static const WAVEFORMATEX wfx8bit22khzstereo;
    static const WAVEFORMATEX wfx8bit22khzmono;
    static const WAVEFORMATEX wfx8bit44khzstereo;
    static const WAVEFORMATEX wfx8bit44khzmono;

protected:
    struct STREAMINFO
    {
        HANDLE        filehandle;
        DWORD         datachunk;
        DWORD         datachunksize;
        DWORD         bufferbytes;
        DWORD         halfbufferbytes;
        DWORD         bytesleft;
        HANDLE        StreamEvents[3];
        CSoundbuffer* SB;
        BOOL          Looping;
    };

    void                initVars();
    bool                setupStreaming();
    static DWORD WINAPI StreamingThread(STREAMINFO* SI);
    void                fillHalfStreamBuffer();

    bool openWAV(LPCSTR file);
    void closeWAV();

    LPDIRECTSOUNDBUFFER SoundBuffer;

    DSBUFFERDESC dsbdesc;
    WAVEFORMATEX wfxFormat;

    LPVOID AudioPtr1, AudioPtr2;  // Für Lock
    DWORD  AudioBytes1, AudioBytes2;

    HANDLE     StreamThread;
    STREAMINFO StreamInfo;

    LPDIRECTSOUNDNOTIFY lpdsNotify;
};

#endif  // !defined(AFX_SOUNDBUFFER_H__6B77BCCD_79BF_11D3_BD8A_0000E85E86C1__INCLUDED_)
