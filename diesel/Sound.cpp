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
#include "Sound.h"

#include <adt/ConCmd.h>
#include <adt/ConVar.h>
#include <sound/SampleBuffer.h>
#include <sound/SoundDevice.h>
#include <sound/SoundSource.h>
#include <sound/WavLoader.h>

#include <iostream>
#include <memory>

void cb_s_musicvolume(ConVar& cvar);
void music(ConCmd& command, const std::string& args);

static std::unique_ptr<SoundDevice> g_soundDevice;
static SampleBufferPtr              g_bgmusicBuffer;
static SoundSourcePtr               g_bgmusicSource;

ConVar s_musicvolume("s_musicvolume", "0.5", 0, cb_s_musicvolume);
ConCmd cmd_music("music", music);

using namespace std;

SoundDevice* getSoundDevice()
{
    return g_soundDevice.get();
}

void freeBGMusic()
{
    if (g_bgmusicSource) {
        g_bgmusicSource->stop();
        g_bgmusicSource->setBuffer(SampleBufferPtr());
        g_bgmusicSource.reset();
    }
    g_bgmusicBuffer.reset();
}

bool initSoundSystem()
{
    cout << endl << "======== initSoundSystem() ========" << endl << endl;

    try {
        g_soundDevice.reset(new SoundDevice());
        cout << "OpenAL sound enabled" << endl;
    } catch (CException& e) {
        cout << "WARNING: OpenAL init failed, sound disabled: " << e.getCompleteText() << endl;
        g_soundDevice.reset();
    }

    cout << "====================================" << endl << endl;
    return true;
}

void shutdownSoundSystem()
{
    freeBGMusic();
    g_soundDevice.reset();
}

void setMusicVolume(float volume)
{
    if (g_bgmusicSource)
        g_bgmusicSource->set(AL_GAIN, volume);
}

void setBGMusic(const std::string& filename)
{
    if (!g_soundDevice)
        return;

    freeBGMusic();

    g_bgmusicBuffer = loadWAV(*g_soundDevice, filename);
    if (!g_bgmusicBuffer) {
        cout << "setBGMusic() could not load " << filename << endl;
        return;
    }

    g_bgmusicSource = g_soundDevice->createSource();
    g_bgmusicSource->setBuffer(g_bgmusicBuffer);
    g_bgmusicSource->setLooping(true);
    g_bgmusicSource->set(AL_SOURCE_RELATIVE, AL_TRUE);
    g_bgmusicSource->set(AL_POSITION, VECTOR3(0.0f, 0.0f, 0.0f));
    setMusicVolume((float)s_musicvolume);
    g_bgmusicSource->play();
}

void updateSoundListener(const MATRIX4& cameraMatrix)
{
    if (g_soundDevice)
        g_soundDevice->setListenerPosition(cameraMatrix);
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
