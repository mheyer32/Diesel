/*
This file is part of Diesel
(c) 2002 by Mathias Heyer
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
#define FMOD_DYN_IMPL
#define FMOD_DYN_NOASSERT

#include <fmoddyn.h>

#include <defs.h>
#include <file/FileManager.h>
#include <iostream>

#include <adt/ConVar.h>
#include <adt/ConCmd.h>

#include <MemoryTracker.h>

#define FMOD_DYN_GUARD if (instance) {
#define FMOD_DYN_UNGUARD                                                                                               \
    }                                                                                                                  \
    ;

void cb_s_musicvolume(ConVar& cvar);
void music(ConCmd& command, const std::string& args);

FMOD_INSTANCE* instance = NULL;
FSOUND_SAMPLE* bgmusic  = NULL;
int            bgmusic_channel;
ConVar         s_musicvolume("s_musicvolume", "0.5", 0, cb_s_musicvolume);
ConCmd         cmd_music("music", music);

using namespace std;

void freeBGMusic()
{
    FMOD_DYN_GUARD
    if (bgmusic) {
        if (bgmusic_channel != -1) {
            instance->FSOUND_StopSound(bgmusic_channel);
        }
        instance->FSOUND_Sample_Free(bgmusic);
        bgmusic = NULL;
    }
    FMOD_DYN_UNGUARD
}

bool initSoundSystem()
{
    cout << endl << "======== initSoundSystem() ========" << endl << endl;
    // it once was using serac´s code, but then dynamic loading became a feature of fmod
    // cout<<"FMOD runtime dynamic loading by:\nAaron 'Serac' Hill <serac@hillvisions.com> "<<endl;
    ;
    if (instance = FMOD_CreateInstance("fmod.dll")) {
        cout << "fmod.dll successfully loaded, sound enabled" << endl;
        return (bool)instance->FSOUND_Init(44100, 16, 0);
    } else {
        cout << "WARNING: fmod.dll not found, you can get it at 'www.fmod.org'\nmaybe you just have an outdated version"
             << endl;
        return true;
    }
    cout << "====================================" << endl << endl;
    ;
}

void shutdownSoundSystem()
{
    FMOD_DYN_GUARD

    instance->FSOUND_StopSound(FSOUND_ALL);
    freeBGMusic();
    instance->FSOUND_Close();

    FMOD_FreeInstance(instance);
    FMOD_DYN_UNGUARD
}

void setMusicVolume(float volume)
{
    FMOD_DYN_GUARD
    instance->FSOUND_SetVolume(bgmusic_channel, (int)(255.0f * volume));
    FMOD_DYN_UNGUARD
}

void setBGMusic(const std::string& filename)
{
    FMOD_DYN_GUARD
    freeBGMusic();

    CFileManager* fman = CFileManager::Instance();
    CFile*        file;
    if (!(file = fman->open(filename))) {
        cout << "setBackgroundMusic() could not open file " << filename << endl;
        return;
    }

    int   filesize = file->getSize();
    char* buffer   = new char[filesize];

    file->readVOID(buffer, filesize);
    file->close();
    delete file;

    bgmusic = instance->FSOUND_Sample_Load(0, buffer, FSOUND_LOADMEMORY | FSOUND_LOOP_NORMAL | FSOUND_2D, 0, filesize);

    if (bgmusic) {
        bgmusic_channel = instance->FSOUND_PlaySound(FSOUND_FREE, bgmusic);
        if (bgmusic_channel == -1) {
            std::cout << "setBGMusic()->FSOUND_PlaySound()  failed" << std::endl;
            freeBGMusic();
            return;
        }
    } else {
        cout << "FSOUND_Sample_Load() returned NULL" << endl;
    }

    setMusicVolume((float)s_musicvolume);

    KILLARRAY(buffer);
    FMOD_DYN_UNGUARD
}

void cb_s_musicvolume(ConVar& cvar)
{
    setMusicVolume((float)cvar);
}

void music(ConCmd& command, const std::string& args)
{
    if (args.empty()) {
        freeBGMusic();
        return;
    }
    setBGMusic(args);
}