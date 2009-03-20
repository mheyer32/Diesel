
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
// Soundbuffer.cpp: Implementierung der Klasse CSoundbuffer.
//
//////////////////////////////////////////////////////////////////////

#include "Soundbuffer.h"

#include "dsoundexception.h"
#include "dserr.h"
#include <defs.h>

#include <iostream>
#include <cassert>
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
const WAVEFORMATEX CSoundbuffer::wfx16bit44khzstereo={WAVE_FORMAT_PCM,2,44100,44100*4,4,16,0};
const WAVEFORMATEX CSoundbuffer::wfx16bit44khzmono={WAVE_FORMAT_PCM,1,44100,44100*2,2,16,0};
const WAVEFORMATEX CSoundbuffer::wfx8bit22khzstereo={WAVE_FORMAT_PCM,2,22050,22050*2,2,8,0};
const WAVEFORMATEX CSoundbuffer::wfx8bit22khzmono={WAVE_FORMAT_PCM,1,22050,22050,1,8,0};
const WAVEFORMATEX CSoundbuffer::wfx8bit44khzstereo={WAVE_FORMAT_PCM,2,44100,44100*2,2,8,0};
const WAVEFORMATEX CSoundbuffer::wfx8bit44khzmono={WAVE_FORMAT_PCM,1,44100,44100,1,8,0};


CSoundbuffer::CSoundbuffer()
{
	initVars();
}

CSoundbuffer::CSoundbuffer(LPCSTR filename,DWORD Flags)
{
	initVars();
	if (!loadWAV(filename,Flags))
	{
		throw CException("CSoundbuffer::CSoundbuffer(...) unable to setup streaming");
	}
}

CSoundbuffer::CSoundbuffer(DWORD buffersize,const WAVEFORMATEX &format,DWORD dsbcaps)
{
	initVars();
	if (!create(buffersize,format,dsbcaps))
	{
		throw CException("CSoundbuffer::CSoundbuffer() -> create(...) failed");
	}
}

CSoundbuffer::~CSoundbuffer()
{
DWORD signal;

	if ((StreamInfo.StreamEvents[2]!=NULL)&&(StreamThread!=NULL))
	{
		//SignalObjectAndWait(StreamInfo.StreamEvents[3],StreamThread,INFINITE,0);
		SetEvent(StreamInfo.StreamEvents[2]);
		signal=WaitForSingleObject(StreamThread,5000);
		if (signal==WAIT_TIMEOUT)
		{
			TerminateThread(StreamThread,0xFFFFFFFF);
		}
		CloseHandle(StreamThread);
	}

	if (lpdsNotify!=NULL) lpdsNotify->Release();

	for (int i=0;i<=2;i++)
	{
		if (StreamInfo.StreamEvents[i]!=NULL) CloseHandle(StreamInfo.StreamEvents[i]);
	}

	closeWAV();

	if (SoundBuffer!=NULL)
	{
		SoundBuffer->Release();
		SoundBuffer=NULL;
	}

}


void CSoundbuffer::initVars()
{
	DXINITSTRUCT(dsbdesc);
	dsbdesc.lpwfxFormat=&wfxFormat; // NULL für Primary Soundbuffer

	wfxFormat.cbSize=0;

	SoundBuffer=NULL;
	lpdsNotify=NULL;

	AudioPtr1=NULL;
	AudioBytes1=0;
	AudioPtr2=NULL;
	AudioBytes2=0;

	ZeroMemory(&StreamInfo,sizeof(StreamInfo));
}


bool CSoundbuffer::create(DWORD buffersize,const WAVEFORMATEX &format,DWORD dsbcaps)
{	
	DXINITSTRUCT(dsbdesc);

	wfxFormat=format;
	dsbdesc.dwFlags=dsbcaps;

	if (dsbcaps&DSBCAPS_PRIMARYBUFFER)
	{
		dsbdesc.dwBufferBytes=0;
		dsbdesc.lpwfxFormat=NULL;
	}
	else
	{	
		dsbdesc.dwBufferBytes=buffersize;
		dsbdesc.lpwfxFormat=&wfxFormat;
	}

	HRESULT dsrval=CDirectSound::Instance()->getLPDS()->CreateSoundBuffer(&dsbdesc,&SoundBuffer,NULL);
	if (dsrval!=DS_OK)
	{
		std::cout<<"CSoundbuffer::CreateBuffer() -> CreateDirectSoundBuffer failed, because:"<<std::endl<<DSErrorToString(dsrval)<<std::endl;
		throw CDSoundException("CSoundbuffer::CreateBuffer() -> CreateDirectSoundBuffer failed",dsrval);
		return false;
	}
	
	SoundBuffer->SetFormat(&wfxFormat);

	setVolume(1.0f);
	return true;
};

HRESULT CSoundbuffer::play(bool loop)
{
	assert(SoundBuffer);
	return SoundBuffer->Play(0,0,loop?DSBPLAY_LOOPING:0);
}

void CSoundbuffer::stop()
{
	assert(SoundBuffer);
	SoundBuffer->Stop();
}
HRESULT CSoundbuffer::setVolume(float volume)
{
	assert(SoundBuffer);
	float v=(float)(DSBVOLUME_MAX-DSBVOLUME_MIN)*volume+(float)DSBVOLUME_MIN;
	return SoundBuffer->SetVolume( (int)v);
}
HRESULT CSoundbuffer::lock(DWORD WriteCursor,DWORD Length,DWORD Flags)
{
	return SoundBuffer->Lock(WriteCursor,Length,&AudioPtr1,&AudioBytes1,&AudioPtr2,&AudioBytes2,Flags);
}


void CSoundbuffer::unlock()
{
	SoundBuffer->Unlock(AudioPtr1,AudioBytes1,AudioPtr2,AudioBytes2);
}


bool CSoundbuffer::loadWAV( LPCSTR filename,DWORD buffersize,DWORD Flags)
{
DWORD bytesread;

	if (StreamInfo.filehandle!=NULL) closeWAV();

	if (openWAV(filename))
	{
		if (buffersize!=0)
		{
			// real streaming
			Flags|=DSBCAPS_GETCURRENTPOSITION2|DSBCAPS_CTRLPOSITIONNOTIFY ;
			if (!create(dsbdesc.dwBufferBytes,wfxFormat,Flags))
			{
				throw CException("CSoundbuffer::LoadWAV() could not create SoundBuffer object");
			}
			if (!setupStreaming())
			{
				throw CException("CSoundbuffer::LoadWAV() SetupStreaming failed");
			}
		}
		else
		{
			// wav completely
			create(StreamInfo.datachunksize,wfxFormat,Flags);
		};
	
		HRESULT dsrval=lock(0,0,ENTIREBUFFER);
        
		if (dsrval!=DS_OK)
		{
			throw CDSoundException("CSoundbuffer::LoadWAV() initial Lock on SoundBuffer failed ",dsrval);	
		}

		ReadFile(StreamInfo.filehandle,AudioPtr1,dsbdesc.dwBufferBytes,&bytesread,NULL);
		StreamInfo.bytesleft-=dsbdesc.dwBufferBytes;
		unlock();
	}
	else
	{
		std::cout<<"CSoundbuffer::loadWAV() could not open WAV-File"<<std::endl;
		return false;
	}

	return true;
};



bool CSoundbuffer::openWAV(LPCSTR filename)
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

DWORD	bytesread;

	StreamInfo.filehandle=CreateFile(filename,GENERIC_READ,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);

	if (StreamInfo.filehandle==INVALID_HANDLE_VALUE)
	{
		throw CException("CSoundbuffer::OpenWAV() LoadWAV: could not open File: ",std::string(filename));
		return false;
	}

	ReadFile(StreamInfo.filehandle,&RIFFHeader,sizeof(RIFFHeader),&bytesread,NULL);

	if ((RIFFHeader.chunkname!=MAKEFOURCC('R','I','F','F'))||(RIFFHeader.wavefile!=MAKEFOURCC('W','A','V','E'))){
		throw CException("CSoundbuffer::OpenWAV() File ist kein RIFF/WAVE-File");
		return false;
	}
	ChunkHeader.chunksize=0;

	do
	{
		SetFilePointer(StreamInfo.filehandle,ChunkHeader.chunksize,NULL,FILE_CURRENT);
		ReadFile(StreamInfo.filehandle,&ChunkHeader,sizeof(ChunkHeader),&bytesread,NULL);
	} while (ChunkHeader.chunkname!=MAKEFOURCC('f','m','t',' '));
	

	ReadFile(StreamInfo.filehandle,&wfxFormat,ChunkHeader.chunksize,&bytesread,NULL);
	
	ChunkHeader.chunksize=0;
	do
	{
		SetFilePointer(StreamInfo.filehandle,ChunkHeader.chunksize,NULL,FILE_CURRENT);
		ReadFile(StreamInfo.filehandle,&ChunkHeader,sizeof(ChunkHeader),&bytesread,NULL);
	} while (ChunkHeader.chunkname!=MAKEFOURCC('d','a','t','a'));
	
	
	StreamInfo.datachunk=SetFilePointer(StreamInfo.filehandle,0,NULL,FILE_CURRENT);
	StreamInfo.datachunksize=ChunkHeader.chunksize;
	StreamInfo.bytesleft=ChunkHeader.chunksize;

	return true;
}

void CSoundbuffer::closeWAV()
{
	if (StreamInfo.filehandle!=NULL){
		CloseHandle(StreamInfo.filehandle);
		StreamInfo.filehandle=NULL;
	}
}

bool CSoundbuffer::setupStreaming()
{
DSBPOSITIONNOTIFY PositionNotify[2];	

    for (int i = 0; i < 4; i++)
    {
        StreamInfo.StreamEvents[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
        if (NULL == StreamInfo.StreamEvents[i]) return false;
    }

    PositionNotify[0].dwOffset = 0;
    PositionNotify[0].hEventNotify = StreamInfo.StreamEvents[0];
    PositionNotify[1].dwOffset = (dsbdesc.dwBufferBytes/2);
    PositionNotify[1].hEventNotify = StreamInfo.StreamEvents[1];

	
    if (SoundBuffer->QueryInterface(IID_IDirectSoundNotify, (LPVOID *)&lpdsNotify)!=DS_OK)
        return false; 
 
    if (lpdsNotify->SetNotificationPositions(2, PositionNotify)!=DS_OK)
    {
        lpdsNotify->Release();
        return false;
    }


	StreamInfo.bufferbytes=dsbdesc.dwBufferBytes;
	StreamInfo.halfbufferbytes=StreamInfo.bufferbytes>>1;
	StreamInfo.bytesleft=StreamInfo.datachunksize;
	StreamInfo.SB=this;

	DWORD ThreadId=NULL;
	StreamThread=CreateThread(	0,  // pointer to security attributes
								0,                         // initial thread stack size
								(LPTHREAD_START_ROUTINE)StreamingThread,     // pointer to thread function
								&StreamInfo,                        // argument for new thread
								0,                     // creation flags
								&ThreadId     // pointer to receive thread ID
							);
	DWORD error=GetLastError();
	if (StreamThread==0)
	{ 
		throw CException("CSoundbuffer::SetupStreaming() Streaming Thread Creation failed");
		return false;
	}
	return true;
}

DWORD WINAPI CSoundbuffer::StreamingThread(STREAMINFO *SI){
Loop:
		DWORD dwEvt = MsgWaitForMultipleObjects(
				3,      // How many possible events
				SI->StreamEvents,       // Location of handles
				FALSE,          // Wait for all?
				INFINITE,       // How long to wait
				QS_ALLEVENTS);   // Any message is an event
 
		// WAIT_OBJECT_0 == 0 but is properly treated as an arbitrary
		// index value assigned to the first event, therefore we subtract
		// it from dwEvt to get the zero-based index of the event.
 
		dwEvt -= WAIT_OBJECT_0;
 
		switch (dwEvt)
		{ 
		case 2:
			SI->SB->SoundBuffer->Stop();
			ExitThread(0);
		case 1:
			SI->SB->lock(0,SI->halfbufferbytes,0);
			break;
		case 0: 
			SI->SB->lock(SI->halfbufferbytes,SI->halfbufferbytes,0);
		};
	
		SI->SB->fillHalfStreamBuffer();
		SI->SB->unlock();

	goto Loop;

	return 0;
};


void CSoundbuffer::resetStreaming(DWORD Offset)
{
	SoundBuffer->Stop();
	SoundBuffer->SetCurrentPosition(0);

	SetFilePointer(StreamInfo.filehandle,StreamInfo.datachunk+Offset,NULL,FILE_BEGIN);

	StreamInfo.bytesleft=StreamInfo.datachunksize-Offset;

	lock(0,0,DSBLOCK_ENTIREBUFFER);
	fillHalfStreamBuffer();
	unlock();
}


void CSoundbuffer::fillHalfStreamBuffer()
{
LPVOID	temp;
DWORD	bytesread;
	if (StreamInfo.bytesleft<StreamInfo.halfbufferbytes)
	{
		ReadFile(StreamInfo.filehandle,AudioPtr1,StreamInfo.bytesleft,&bytesread,NULL);
		SetFilePointer(StreamInfo.filehandle,StreamInfo.datachunk,NULL,FILE_BEGIN);
		temp=(CHAR*)AudioPtr1+StreamInfo.bytesleft;
		ReadFile(StreamInfo.filehandle,temp,StreamInfo.halfbufferbytes-StreamInfo.bytesleft,&bytesread,NULL);
		StreamInfo.bytesleft=StreamInfo.datachunksize-StreamInfo.halfbufferbytes+StreamInfo.bytesleft;
	}
	else{
		ReadFile(StreamInfo.filehandle,AudioPtr1,StreamInfo.halfbufferbytes,&bytesread,NULL);
		StreamInfo.bytesleft-=StreamInfo.halfbufferbytes;
	}
}

		