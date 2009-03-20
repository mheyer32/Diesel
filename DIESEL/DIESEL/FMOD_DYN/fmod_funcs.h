// -----------------------------------------------------------------------------
//  fmod_funcs.h -- Include header for use with "fmod_dyn.h".
//  Version 3.5.0002, release by Serac.
//
//  This file was created by Aaron 'Serac' Hill <serac@hillvisions.com> and is
//  NOT a part of the FMOD Sound System.  Please contact the author if you have
//  questions or comments about this file.  For information about FMOD, please
//  visit http://www.fmod.org/.
//
//  FMOD is Copyright (c), Firelight Technologies Pty, Ltd 1999-2002.
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//  DO NOT INCLUDE THIS FILE DIRECTLY.  ALREADY INCLUDED VIA "FMOD_DYN.H".
// -----------------------------------------------------------------------------

FMOD_DYN_FUNC("_FSOUND_SetOutput@4",FSOUND_SetOutput,signed char,0,(int))
FMOD_DYN_FUNC("_FSOUND_SetDriver@4",FSOUND_SetDriver,signed char,0,(int))
FMOD_DYN_FUNC("_FSOUND_SetMixer@4",FSOUND_SetMixer,signed char,0,(int))
FMOD_DYN_FUNC("_FSOUND_SetBufferSize@4",FSOUND_SetBufferSize,signed char,0,(int))
FMOD_DYN_FUNC("_FSOUND_SetHWND@4",FSOUND_SetHWND,signed char,0,(void*))
FMOD_DYN_FUNC("_FSOUND_SetMinHardwareChannels@4",FSOUND_SetMinHardwareChannels,signed char,0,(int))
FMOD_DYN_FUNC("_FSOUND_SetMaxHardwareChannels@4",FSOUND_SetMaxHardwareChannels,signed char,0,(int))
FMOD_DYN_FUNC("_FSOUND_SetMemorySystem@20",FSOUND_SetMemorySystem,signed char,0,(void*,int,FSOUND_ALLOCCALLBACK,FSOUND_REALLOCCALLBACK,FSOUND_FREECALLBACK))

FMOD_DYN_FUNC("_FSOUND_Init@12",FSOUND_Init,signed char,0,(int,int,unsigned int))
FMOD_DYN_FUNC("_FSOUND_Close@0",FSOUND_Close,void,;,())

FMOD_DYN_FUNC("_FSOUND_SetSpeakerMode@4",FSOUND_SetSpeakerMode,void,;,(unsigned int))
FMOD_DYN_FUNC("_FSOUND_SetSFXMasterVolume@4",FSOUND_SetSFXMasterVolume,void,;,(int))
FMOD_DYN_FUNC("_FSOUND_SetPanSeperation@4",FSOUND_SetPanSeperation,void,;,(float))
FMOD_DYN_FUNC("_FSOUND_File_SetCallbacks@20",FSOUND_File_SetCallbacks,void,;,(FSOUND_OPENCALLBACK,FSOUND_CLOSECALLBACK,FSOUND_READCALLBACK,FSOUND_SEEKCALLBACK,FSOUND_TELLCALLBACK))

FMOD_DYN_FUNC("_FSOUND_GetError@0",FSOUND_GetError,int,0,())
FMOD_DYN_FUNC("_FSOUND_GetVersion@0",FSOUND_GetVersion,float,0.0f,())
FMOD_DYN_FUNC("_FSOUND_GetOutput@0",FSOUND_GetOutput,int,0,())
FMOD_DYN_FUNC("_FSOUND_GetOutputHandle@0",FSOUND_GetOutputHandle,void*,0,())
FMOD_DYN_FUNC("_FSOUND_GetDriver@0",FSOUND_GetDriver,int,0,())
FMOD_DYN_FUNC("_FSOUND_GetMixer@0",FSOUND_GetMixer,int,0,())
FMOD_DYN_FUNC("_FSOUND_GetNumDrivers@0",FSOUND_GetNumDrivers,int,0,())
FMOD_DYN_FUNC("_FSOUND_GetDriverName@4",FSOUND_GetDriverName,signed char*,0,(int))
FMOD_DYN_FUNC("_FSOUND_GetDriverCaps@8",FSOUND_GetDriverCaps,signed char,0,(int,unsigned int*))
FMOD_DYN_FUNC("_FSOUND_GetOutputRate@0",FSOUND_GetOutputRate,int,0,())
FMOD_DYN_FUNC("_FSOUND_GetMaxChannels@0",FSOUND_GetMaxChannels,int,0,())
FMOD_DYN_FUNC("_FSOUND_GetMaxSamples@0",FSOUND_GetMaxSamples,int,0,())
FMOD_DYN_FUNC("_FSOUND_GetSFXMasterVolume@0",FSOUND_GetSFXMasterVolume,int,0,())
FMOD_DYN_FUNC("_FSOUND_GetNumHardwareChannels@0",FSOUND_GetNumHardwareChannels,int,0,())
FMOD_DYN_FUNC("_FSOUND_GetChannelsPlaying@0",FSOUND_GetChannelsPlaying,int,0,())
FMOD_DYN_FUNC("_FSOUND_GetCPUUsage@0",FSOUND_GetCPUUsage,float,0.0f,())
FMOD_DYN_FUNC("_FSOUND_GetMemoryStats@8",FSOUND_GetMemoryStats,void,;,(unsigned int*,unsigned int*))

FMOD_DYN_FUNC("_FSOUND_Sample_Load@16",FSOUND_Sample_Load,FSOUND_SAMPLE*,0,(int,const char *,unsigned int,int))
FMOD_DYN_FUNC("_FSOUND_Sample_Alloc@28",FSOUND_Sample_Alloc,FSOUND_SAMPLE*,0,(int,int,unsigned int,int,int,int,int))
FMOD_DYN_FUNC("_FSOUND_Sample_Free@4",FSOUND_Sample_Free,void,;,(FSOUND_SAMPLE*))
FMOD_DYN_FUNC("_FSOUND_Sample_Upload@12",FSOUND_Sample_Upload,signed char,0,(FSOUND_SAMPLE*,void*,unsigned int))
FMOD_DYN_FUNC("_FSOUND_Sample_Lock@28",FSOUND_Sample_Lock,signed char,0,(FSOUND_SAMPLE*,int,int,void**,void**,unsigned int*,unsigned int*))
FMOD_DYN_FUNC("_FSOUND_Sample_Unlock@20",FSOUND_Sample_Unlock,signed char,0,(FSOUND_SAMPLE*,void*,void*,unsigned int,unsigned int))

FMOD_DYN_FUNC("_FSOUND_Sample_SetLoopMode@8",FSOUND_Sample_SetLoopMode,signed char,0,(FSOUND_SAMPLE*,unsigned int))
FMOD_DYN_FUNC("_FSOUND_Sample_SetLoopPoints@12",FSOUND_Sample_SetLoopPoints,signed char,0,(FSOUND_SAMPLE*,int,int))
FMOD_DYN_FUNC("_FSOUND_Sample_SetDefaults@20",FSOUND_Sample_SetDefaults,signed char,0,(FSOUND_SAMPLE*,int,int,int,int))
FMOD_DYN_FUNC("_FSOUND_Sample_SetMinMaxDistance@12",FSOUND_Sample_SetMinMaxDistance,signed char,0,(FSOUND_SAMPLE*, float, float))

FMOD_DYN_FUNC("_FSOUND_Sample_Get@4",FSOUND_Sample_Get,FSOUND_SAMPLE*,0,(int))
FMOD_DYN_FUNC("_FSOUND_Sample_GetName@4",FSOUND_Sample_GetName,char*,0,(FSOUND_SAMPLE*))
FMOD_DYN_FUNC("_FSOUND_Sample_GetLength@4",FSOUND_Sample_GetLength,unsigned int,0,(FSOUND_SAMPLE*))
FMOD_DYN_FUNC("_FSOUND_Sample_GetLoopPoints@12",FSOUND_Sample_GetLoopPoints,signed char,0,(FSOUND_SAMPLE*,int*,int*))
FMOD_DYN_FUNC("_FSOUND_Sample_GetDefaults@20",FSOUND_Sample_GetDefaults,signed char,0,(FSOUND_SAMPLE*,int*,int*,int*,int*))
FMOD_DYN_FUNC("_FSOUND_Sample_GetMode@4",FSOUND_Sample_GetMode,unsigned int,0,(FSOUND_SAMPLE*))

FMOD_DYN_FUNC("_FSOUND_PlaySound@8",FSOUND_PlaySound,int,0,(int,FSOUND_SAMPLE*))
FMOD_DYN_FUNC("_FSOUND_PlaySoundEx@16",FSOUND_PlaySoundEx,int,0,(int,FSOUND_SAMPLE*,FSOUND_DSPUNIT*,signed char))
FMOD_DYN_FUNC("_FSOUND_StopSound@4",FSOUND_StopSound,signed char,0,(int))

FMOD_DYN_FUNC("_FSOUND_SetFrequency@8",FSOUND_SetFrequency,signed char,0,(int,int))
FMOD_DYN_FUNC("_FSOUND_SetVolume@8",FSOUND_SetVolume,signed char,0,(int,int))
FMOD_DYN_FUNC("_FSOUND_SetVolumeAbsolute@8",FSOUND_SetVolumeAbsolute,signed char,0,(int,int))
FMOD_DYN_FUNC("_FSOUND_SetPan@8",FSOUND_SetPan,signed char,0,(int,int))
FMOD_DYN_FUNC("_FSOUND_SetSurround@8",FSOUND_SetSurround,signed char,0,(int,signed char))
FMOD_DYN_FUNC("_FSOUND_SetMute@8",FSOUND_SetMute,signed char,0,(int,signed char))
FMOD_DYN_FUNC("_FSOUND_SetPriority@8",FSOUND_SetPriority,signed char,0,(int,int))
FMOD_DYN_FUNC("_FSOUND_SetReserved@8",FSOUND_SetReserved,signed char,0,(int,signed char))
FMOD_DYN_FUNC("_FSOUND_SetPaused@8",FSOUND_SetPaused,signed char,0,(int,signed char))
FMOD_DYN_FUNC("_FSOUND_SetLoopMode@8",FSOUND_SetLoopMode,signed char,0,(int,unsigned int))
FMOD_DYN_FUNC("_FSOUND_SetCurrentPosition@8",FSOUND_SetCurrentPosition,signed char,0,(int,unsigned int))

FMOD_DYN_FUNC("_FSOUND_FX_Enable@8",FSOUND_FX_Enable,int,0,(int,unsigned int))

FMOD_DYN_FUNC("_FSOUND_FX_SetChorus@32",FSOUND_FX_SetChorus,signed char,0,(int,float,float,float,float,int,float,int))
FMOD_DYN_FUNC("_FSOUND_FX_SetCompressor@28",FSOUND_FX_SetCompressor,signed char,0,(int,float,float,float,float,float,float))
FMOD_DYN_FUNC("_FSOUND_FX_SetDistortion@24",FSOUND_FX_SetDistortion,signed char,0,(int,float,float,float,float,float))
FMOD_DYN_FUNC("_FSOUND_FX_SetEcho@24",FSOUND_FX_SetEcho,signed char,0,(int,float,float,float,float,int))
FMOD_DYN_FUNC("_FSOUND_FX_SetFlanger@32",FSOUND_FX_SetFlanger,signed char,0,(int,float,float,float,float,int,float,int))
FMOD_DYN_FUNC("_FSOUND_FX_SetGargle@12",FSOUND_FX_SetGargle,signed char,0,(int,int,int))
FMOD_DYN_FUNC("_FSOUND_FX_SetI3DL2Reverb@52",FSOUND_FX_SetI3DL2Reverb,signed char,0,(int,int,int,float,float,float,int,float,int,float,float,float,float))
FMOD_DYN_FUNC("_FSOUND_FX_SetParamEQ@16",FSOUND_FX_SetParamEQ,signed char,0,(int,float,float,float))
FMOD_DYN_FUNC("_FSOUND_FX_SetWavesReverb@20",FSOUND_FX_SetWavesReverb,signed char,0,(int,float,float,float,float))

FMOD_DYN_FUNC("_FSOUND_IsPlaying@4",FSOUND_IsPlaying,signed char,0,(int))
FMOD_DYN_FUNC("_FSOUND_GetFrequency@4",FSOUND_GetFrequency,int,0,(int))
FMOD_DYN_FUNC("_FSOUND_GetVolume@4",FSOUND_GetVolume,int,0,(int))
FMOD_DYN_FUNC("_FSOUND_GetPan@4",FSOUND_GetPan,int,0,(int))
FMOD_DYN_FUNC("_FSOUND_GetSurround@4",FSOUND_GetSurround,signed char,0,(int))
FMOD_DYN_FUNC("_FSOUND_GetMute@4",FSOUND_GetMute,signed char,0,(int))
FMOD_DYN_FUNC("_FSOUND_GetPriority@4",FSOUND_GetPriority,int,0,(int))
FMOD_DYN_FUNC("_FSOUND_GetReserved@4",FSOUND_GetReserved,signed char,0,(int))
FMOD_DYN_FUNC("_FSOUND_GetPaused@4",FSOUND_GetPaused,signed char,0,(int))
FMOD_DYN_FUNC("_FSOUND_GetLoopMode@4",FSOUND_GetLoopMode,unsigned int,0,(int))
FMOD_DYN_FUNC("_FSOUND_GetCurrentPosition@4",FSOUND_GetCurrentPosition,unsigned int,0,(int))
FMOD_DYN_FUNC("_FSOUND_GetCurrentSample@4",FSOUND_GetCurrentSample,FSOUND_SAMPLE*,0,(int))
FMOD_DYN_FUNC("_FSOUND_GetCurrentVU@4",FSOUND_GetCurrentVU,float,0.0f,(int))

FMOD_DYN_FUNC("_FSOUND_3D_Update@0",FSOUND_3D_Update,void,;,())
FMOD_DYN_FUNC("_FSOUND_3D_SetAttributes@12",FSOUND_3D_SetAttributes,signed char,0,(int,float*,float*))
FMOD_DYN_FUNC("_FSOUND_3D_GetAttributes@12",FSOUND_3D_GetAttributes,signed char,0,(int,float*,float*))
FMOD_DYN_FUNC("_FSOUND_3D_Listener_SetAttributes@32",FSOUND_3D_Listener_SetAttributes,void,;,(float*,float*,float,float,float,float,float,float))
FMOD_DYN_FUNC("_FSOUND_3D_Listener_GetAttributes@32",FSOUND_3D_Listener_GetAttributes,void,;,(float*,float*,float*,float*,float*,float*,float*,float*))
FMOD_DYN_FUNC("_FSOUND_3D_Listener_SetDopplerFactor@4",FSOUND_3D_Listener_SetDopplerFactor,void,;,(float))
FMOD_DYN_FUNC("_FSOUND_3D_Listener_SetDistanceFactor@4",FSOUND_3D_Listener_SetDistanceFactor,void,;,(float))
FMOD_DYN_FUNC("_FSOUND_3D_Listener_SetRolloffFactor@4",FSOUND_3D_Listener_SetRolloffFactor,void,;,(float))
 
FMOD_DYN_FUNC("_FSOUND_Stream_OpenFile@12",FSOUND_Stream_OpenFile,FSOUND_STREAM*,0,(const char*,unsigned int,int))
FMOD_DYN_FUNC("_FSOUND_Stream_Create@20",FSOUND_Stream_Create,FSOUND_STREAM*,0,(FSOUND_STREAMCALLBACK,int,unsigned int,int,int))
FMOD_DYN_FUNC("_FSOUND_Stream_Play@8",FSOUND_Stream_Play,int,0,(int, FSOUND_STREAM*))
FMOD_DYN_FUNC("_FSOUND_Stream_PlayEx@16",FSOUND_Stream_PlayEx,int,0,(int,FSOUND_STREAM*,FSOUND_DSPUNIT*,signed char))

FMOD_DYN_FUNC("_FSOUND_Stream_Stop@4",FSOUND_Stream_Stop,signed char,0,(FSOUND_STREAM*))
FMOD_DYN_FUNC("_FSOUND_Stream_Close@4",FSOUND_Stream_Close,signed char,0,(FSOUND_STREAM*))
FMOD_DYN_FUNC("_FSOUND_Stream_SetEndCallback@12",FSOUND_Stream_SetEndCallback,signed char,0,(FSOUND_STREAM*,FSOUND_STREAMCALLBACK,int))
FMOD_DYN_FUNC("_FSOUND_Stream_SetSynchCallback@12",FSOUND_Stream_SetSynchCallback,signed char,0,(FSOUND_STREAM*,FSOUND_STREAMCALLBACK,int))
FMOD_DYN_FUNC("_FSOUND_Stream_GetSample@4",FSOUND_Stream_GetSample,FSOUND_SAMPLE*,0,(FSOUND_STREAM*))
FMOD_DYN_FUNC("_FSOUND_Stream_CreateDSP@16",FSOUND_Stream_CreateDSP,FSOUND_DSPUNIT*,0,(FSOUND_STREAM*,FSOUND_DSPCALLBACK,int,int))
FMOD_DYN_FUNC("_FSOUND_Stream_SetBufferSize@4",FSOUND_Stream_SetBufferSize,signed char,0,(int))

FMOD_DYN_FUNC("_FSOUND_Stream_SetPosition@8",FSOUND_Stream_SetPosition,signed char,0,(FSOUND_STREAM*,unsigned int))
FMOD_DYN_FUNC("_FSOUND_Stream_GetPosition@4",FSOUND_Stream_GetPosition,unsigned int,0,(FSOUND_STREAM*))
FMOD_DYN_FUNC("_FSOUND_Stream_SetTime@8",FSOUND_Stream_SetTime,signed char,0,(FSOUND_STREAM*,int))
FMOD_DYN_FUNC("_FSOUND_Stream_GetTime@4",FSOUND_Stream_GetTime,int,0,(FSOUND_STREAM*))
FMOD_DYN_FUNC("_FSOUND_Stream_GetLength@4",FSOUND_Stream_GetLength,int,0,(FSOUND_STREAM*))
FMOD_DYN_FUNC("_FSOUND_Stream_GetLengthMs@4",FSOUND_Stream_GetLengthMs,int,0,(FSOUND_STREAM*))

FMOD_DYN_FUNC("_FSOUND_CD_Play@8",FSOUND_CD_Play,signed char,0,(char,int))
FMOD_DYN_FUNC("_FSOUND_CD_SetPlayMode@8",FSOUND_CD_SetPlayMode,void,;,(char,signed char))
FMOD_DYN_FUNC("_FSOUND_CD_Stop@4",FSOUND_CD_Stop,signed char,0,(char))
FMOD_DYN_FUNC("_FSOUND_CD_SetPaused@8",FSOUND_CD_SetPaused,signed char,0,(char,signed char))
FMOD_DYN_FUNC("_FSOUND_CD_SetVolume@8",FSOUND_CD_SetVolume,signed char,0,(char,int))
FMOD_DYN_FUNC("_FSOUND_CD_Eject@4",FSOUND_CD_Eject,signed char,0,(char))

FMOD_DYN_FUNC("_FSOUND_CD_GetPaused@4",FSOUND_CD_GetPaused,signed char,0,(char))
FMOD_DYN_FUNC("_FSOUND_CD_GetTrack@4",FSOUND_CD_GetTrack,int,0,(char))
FMOD_DYN_FUNC("_FSOUND_CD_GetNumTracks@4",FSOUND_CD_GetNumTracks,int,0,(char))
FMOD_DYN_FUNC("_FSOUND_CD_GetVolume@4",FSOUND_CD_GetVolume,int,0,(char))
FMOD_DYN_FUNC("_FSOUND_CD_GetTrackLength@8",FSOUND_CD_GetTrackLength,int,0,(char,int))
FMOD_DYN_FUNC("_FSOUND_CD_GetTrackTime@4",FSOUND_CD_GetTrackTime,int,0,(char))

FMOD_DYN_FUNC("_FSOUND_DSP_Create@12",FSOUND_DSP_Create,FSOUND_DSPUNIT*,0,(FSOUND_DSPCALLBACK,int,int))
FMOD_DYN_FUNC("_FSOUND_DSP_Free@4",FSOUND_DSP_Free,void,;,(FSOUND_DSPUNIT*))
FMOD_DYN_FUNC("_FSOUND_DSP_SetPriority@8",FSOUND_DSP_SetPriority,void,;,(FSOUND_DSPUNIT*,int))
FMOD_DYN_FUNC("_FSOUND_DSP_GetPriority@4",FSOUND_DSP_GetPriority,int,0,(FSOUND_DSPUNIT*))
FMOD_DYN_FUNC("_FSOUND_DSP_SetActive@8",FSOUND_DSP_SetActive,void,;,(FSOUND_DSPUNIT*,signed char))
FMOD_DYN_FUNC("_FSOUND_DSP_GetActive@4",FSOUND_DSP_GetActive,signed char,0,(FSOUND_DSPUNIT*))

FMOD_DYN_FUNC("_FSOUND_DSP_GetClearUnit@0",FSOUND_DSP_GetClearUnit,FSOUND_DSPUNIT*,0,())
FMOD_DYN_FUNC("_FSOUND_DSP_GetSFXUnit@0",FSOUND_DSP_GetSFXUnit,FSOUND_DSPUNIT*,0,())
FMOD_DYN_FUNC("_FSOUND_DSP_GetMusicUnit@0",FSOUND_DSP_GetMusicUnit,FSOUND_DSPUNIT*,0,())
FMOD_DYN_FUNC("_FSOUND_DSP_GetFFTUnit@0",FSOUND_DSP_GetFFTUnit,FSOUND_DSPUNIT*,0,())
FMOD_DYN_FUNC("_FSOUND_DSP_GetClipAndCopyUnit@0",FSOUND_DSP_GetClipAndCopyUnit,FSOUND_DSPUNIT*,0,())

FMOD_DYN_FUNC("_FSOUND_DSP_MixBuffers@28",FSOUND_DSP_MixBuffers,signed char,0,(void*,void*,int,int,int,int,unsigned int))
FMOD_DYN_FUNC("_FSOUND_DSP_ClearMixBuffer@0",FSOUND_DSP_ClearMixBuffer,void,;,())
FMOD_DYN_FUNC("_FSOUND_DSP_GetBufferLength@0",FSOUND_DSP_GetBufferLength,int,0,())
FMOD_DYN_FUNC("_FSOUND_DSP_GetBufferLengthTotal@0",FSOUND_DSP_GetBufferLengthTotal,int,0,())
FMOD_DYN_FUNC("_FSOUND_DSP_GetSpectrum@0",FSOUND_DSP_GetSpectrum,float*,0,())

FMOD_DYN_FUNC("_FSOUND_Geometry_AddPolygon@28",FSOUND_Geometry_AddPolygon,signed char,0,(float*,float*,float*,float*,float*,unsigned int,float*))
FMOD_DYN_FUNC("_FSOUND_Geometry_AddList@4",FSOUND_Geometry_AddList,int,0,(FSOUND_GEOMLIST*))

FMOD_DYN_FUNC("_FSOUND_Geometry_List_Create@4",FSOUND_Geometry_List_Create,FSOUND_GEOMLIST*,0,(signed char))
FMOD_DYN_FUNC("_FSOUND_Geometry_List_Free@4",FSOUND_Geometry_List_Free,signed char,0,(FSOUND_GEOMLIST*))
FMOD_DYN_FUNC("_FSOUND_Geometry_List_Begin@4",FSOUND_Geometry_List_Begin,signed char,0,(FSOUND_GEOMLIST*))
FMOD_DYN_FUNC("_FSOUND_Geometry_List_End@4",FSOUND_Geometry_List_End,signed char,0,(FSOUND_GEOMLIST*))

FMOD_DYN_FUNC("_FSOUND_Geometry_Material_Create@0",FSOUND_Geometry_Material_Create,FSOUND_MATERIAL*,0,())
FMOD_DYN_FUNC("_FSOUND_Geometry_Material_Free@4",FSOUND_Geometry_Material_Free,signed char,0,(FSOUND_MATERIAL*))
FMOD_DYN_FUNC("_FSOUND_Geometry_Material_SetAttributes@20",FSOUND_Geometry_Material_SetAttributes,signed char,0,(FSOUND_MATERIAL*,float,float,float,float))
FMOD_DYN_FUNC("_FSOUND_Geometry_Material_GetAttributes@20",FSOUND_Geometry_Material_GetAttributes,signed char,0,(FSOUND_MATERIAL*,float*,float*,float*,float*))
FMOD_DYN_FUNC("_FSOUND_Geometry_Material_Set@4",FSOUND_Geometry_Material_Set,signed char,0,(FSOUND_MATERIAL*))

FMOD_DYN_FUNC("_FSOUND_Reverb_SetProperties@4",FSOUND_Reverb_SetProperties,signed char,0,(FSOUND_REVERB_PROPERTIES*))
FMOD_DYN_FUNC("_FSOUND_Reverb_GetProperties@4",FSOUND_Reverb_GetProperties,signed char,0,(FSOUND_REVERB_PROPERTIES*))
FMOD_DYN_FUNC("_FSOUND_Reverb_SetChannelProperties@8",FSOUND_Reverb_SetChannelProperties,signed char,0,(int,FSOUND_REVERB_CHANNELPROPERTIES*))
FMOD_DYN_FUNC("_FSOUND_Reverb_GetChannelProperties@8",FSOUND_Reverb_GetChannelProperties,signed char,0,(int,FSOUND_REVERB_CHANNELPROPERTIES*))

FMOD_DYN_FUNC("_FSOUND_Record_SetDriver@4",FSOUND_Record_SetDriver,signed char,0,(int))
FMOD_DYN_FUNC("_FSOUND_Record_GetNumDrivers@0",FSOUND_Record_GetNumDrivers,int,0,())
FMOD_DYN_FUNC("_FSOUND_Record_GetDriverName@4",FSOUND_Record_GetDriverName,signed char*,0,(int))
FMOD_DYN_FUNC("_FSOUND_Record_GetDriver@0",FSOUND_Record_GetDriver,int,0,())

FMOD_DYN_FUNC("_FSOUND_Record_StartSample@8",FSOUND_Record_StartSample,signed char,0,(FSOUND_SAMPLE*,signed char))
FMOD_DYN_FUNC("_FSOUND_Record_Stop@0",FSOUND_Record_Stop,signed char,0,())
FMOD_DYN_FUNC("_FSOUND_Record_GetPosition@0",FSOUND_Record_GetPosition,int,0,())

FMOD_DYN_FUNC("_FMUSIC_LoadSong@4",FMUSIC_LoadSong,FMUSIC_MODULE*,0,(const char*))
FMOD_DYN_FUNC("_FMUSIC_LoadSongMemory@8",FMUSIC_LoadSongMemory,FMUSIC_MODULE *,0,(void*,int))
FMOD_DYN_FUNC("_FMUSIC_FreeSong@4",FMUSIC_FreeSong,signed char,0,(FMUSIC_MODULE*))
FMOD_DYN_FUNC("_FMUSIC_PlaySong@4",FMUSIC_PlaySong,signed char,0,(FMUSIC_MODULE*))
FMOD_DYN_FUNC("_FMUSIC_StopSong@4",FMUSIC_StopSong,signed char,0,(FMUSIC_MODULE*))
FMOD_DYN_FUNC("_FMUSIC_StopAllSongs@0",FMUSIC_StopAllSongs,void,;,())

FMOD_DYN_FUNC("_FMUSIC_SetZxxCallback@8",FMUSIC_SetZxxCallback,signed char,0,(FMUSIC_MODULE*,FMUSIC_CALLBACK))
FMOD_DYN_FUNC("_FMUSIC_SetRowCallback@12",FMUSIC_SetRowCallback,signed char,0,(FMUSIC_MODULE*,FMUSIC_CALLBACK,int))
FMOD_DYN_FUNC("_FMUSIC_SetOrderCallback@12",FMUSIC_SetOrderCallback,signed char,0,(FMUSIC_MODULE*,FMUSIC_CALLBACK,int))
FMOD_DYN_FUNC("_FMUSIC_SetInstCallback@12",FMUSIC_SetInstCallback,signed char,0,(FMUSIC_MODULE*,FMUSIC_CALLBACK,int))

FMOD_DYN_FUNC("_FMUSIC_SetSample@12",FMUSIC_SetSample,signed char,0,(FMUSIC_MODULE*,int,FSOUND_SAMPLE*))
FMOD_DYN_FUNC("_FMUSIC_OptimizeChannels@12",FMUSIC_OptimizeChannels,signed char,0,(FMUSIC_MODULE*,int,int))

FMOD_DYN_FUNC("_FMUSIC_SetReverb@4",FMUSIC_SetReverb,signed char,0,(signed char))
FMOD_DYN_FUNC("_FMUSIC_SetOrder@8",FMUSIC_SetOrder,signed char,0,(FMUSIC_MODULE*,int))
FMOD_DYN_FUNC("_FMUSIC_SetPaused@8",FMUSIC_SetPaused,signed char,0,(FMUSIC_MODULE*,signed char))
FMOD_DYN_FUNC("_FMUSIC_SetMasterVolume@8",FMUSIC_SetMasterVolume,signed char,0,(FMUSIC_MODULE*,int))
FMOD_DYN_FUNC("_FMUSIC_SetPanSeperation@8",FMUSIC_SetPanSeperation,signed char,0,(FMUSIC_MODULE*,float))
 
FMOD_DYN_FUNC("_FMUSIC_GetName@4",FMUSIC_GetName,char*,0,(FMUSIC_MODULE*))
FMOD_DYN_FUNC("_FMUSIC_GetType@4",FMUSIC_GetType,int,0,(FMUSIC_MODULE*))
FMOD_DYN_FUNC("_FMUSIC_GetNumOrders@4",FMUSIC_GetNumOrders,int,0,(FMUSIC_MODULE*))
FMOD_DYN_FUNC("_FMUSIC_GetNumPatterns@4",FMUSIC_GetNumPatterns,int,0,(FMUSIC_MODULE*))
FMOD_DYN_FUNC("_FMUSIC_GetNumInstruments@4",FMUSIC_GetNumInstrument,int,0,(FMUSIC_MODULE*))
FMOD_DYN_FUNC("_FMUSIC_GetNumSamples@4",FMUSIC_GetNumSamples,int,0,(FMUSIC_MODULE*))
FMOD_DYN_FUNC("_FMUSIC_GetNumChannels@4",FMUSIC_GetNumChannels,int,0,(FMUSIC_MODULE*))
FMOD_DYN_FUNC("_FMUSIC_GetSample@8",FMUSIC_GetSample,FSOUND_SAMPLE*,0,(FMUSIC_MODULE*,int))
FMOD_DYN_FUNC("_FMUSIC_GetPatternLength@8",FMUSIC_GetPatternLength,int,0,(FMUSIC_MODULE*,int))
 
FMOD_DYN_FUNC("_FMUSIC_IsFinished@4",FMUSIC_IsFinished,signed char,0,(FMUSIC_MODULE*))
FMOD_DYN_FUNC("_FMUSIC_IsPlaying@4",FMUSIC_IsPlaying,signed char,0,(FMUSIC_MODULE*))
FMOD_DYN_FUNC("_FMUSIC_GetMasterVolume@4",FMUSIC_GetMasterVolume,int,0,(FMUSIC_MODULE*))
FMOD_DYN_FUNC("_FMUSIC_GetGlobalVolume@4",FMUSIC_GetGlobalVolume,int,0,(FMUSIC_MODULE*))
FMOD_DYN_FUNC("_FMUSIC_GetOrder@4",FMUSIC_GetOrder,int,0,(FMUSIC_MODULE*))
FMOD_DYN_FUNC("_FMUSIC_GetPattern@4",FMUSIC_GetPattern,int,0,(FMUSIC_MODULE*))
FMOD_DYN_FUNC("_FMUSIC_GetSpeed@4",FMUSIC_GetSpeed,int,0,(FMUSIC_MODULE*))
FMOD_DYN_FUNC("_FMUSIC_GetBPM@4",FMUSIC_GetBPM,int,0,(FMUSIC_MODULE*))
FMOD_DYN_FUNC("_FMUSIC_GetRow@4",FMUSIC_GetRow,int,0,(FMUSIC_MODULE*))
FMOD_DYN_FUNC("_FMUSIC_GetPaused@4",FMUSIC_GetPaused,signed char,0,(FMUSIC_MODULE*))
FMOD_DYN_FUNC("_FMUSIC_GetTime@4",FMUSIC_GetTime,int,0,(FMUSIC_MODULE*))
