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
#include "Sound.h"

#include "Q3BSPMesh.h"
#include "SpeakerEmitter.h"

#include <adt/ConCmd.h>
#include <adt/ConVar.h>
#include <defs.h>
#include <sound/SampleBuffer.h>
#include <sound/SoundDevice.h>
#include <sound/SoundSource.h>
#include <sound/WavLoader.h>

#include <iostream>
#include <list>
#include <map>
#include <vector>

void cb_s_musicvolume(ConVar& cvar);
void cb_s_volume(ConVar& cvar);
void music(ConCmd& command, const std::string& args);

static SoundDevice*                              g_soundDevice = 0;
static SampleBufferPtr                           g_bgmusicBuffer;
static SoundSourcePtr                            g_bgmusicSource;
static std::map<std::string, SampleBufferPtr>    g_sampleCache;
static SpeakerEmitter::SMARTPTRLIST              g_speakers;
static std::list<SoundSourcePtr>                 g_sourcePool;
static int                                       g_soundCullFrame = 0;
static Q3BSPMesh*                                g_soundWorld     = 0;

static const int MAX_SOUND_CHANNELS = 32;

ConVar s_musicvolume("s_musicvolume", "0.5", 0, cb_s_musicvolume);
ConVar s_volume("s_volume", "0.8", 0, cb_s_volume);
ConCmd cmd_music("music", music);

using namespace std;

static float getMasterVolume()
{
    return (float)s_volume;
}

static void refreshSpeakerGains()
{
    float master = getMasterVolume();
    SpeakerEmitter::SMARTPTRITERATOR sIt = g_speakers.begin();
    for (; sIt != g_speakers.end(); ++sIt)
        (*sIt)->applyGain(master);
}

SoundDevice* getSoundDevice()
{
    return g_soundDevice;
}

void freeBGMusic()
{
    if (g_bgmusicSource) {
        g_bgmusicSource->stop();
        g_bgmusicSource->setBuffer(SampleBufferPtr());
        g_bgmusicSource = 0;
    }
    g_bgmusicBuffer = 0;
}

bool initSoundSystem()
{
    cout << endl << "======== initSoundSystem() ========" << endl << endl;

    try {
        g_soundDevice = new SoundDevice();
        cout << "OpenAL sound enabled" << endl;
    } catch (CException& e) {
        cout << "WARNING: OpenAL init failed, sound disabled: " << e.getCompleteText() << endl;
        KILLOBJECT(g_soundDevice);
    }

    cout << "====================================" << endl << endl;
    return true;
}

void shutdownSoundSystem()
{
    clearMapSounds();
    freeBGMusic();
    g_sampleCache.clear();
    KILLOBJECT(g_soundDevice);
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

void cb_s_volume(ConVar& cvar)
{
    refreshSpeakerGains();
}

void music(ConCmd& command, const std::string& args)
{
    if (args.empty()) {
        freeBGMusic();
        return;
    }
    setBGMusic(args);
}

void clearMapSounds()
{
    if (g_soundWorld) {
        g_soundWorld->unlinkAllSounds();
        g_soundWorld = 0;
    }

    SpeakerEmitter::SMARTPTRITERATOR sIt = g_speakers.begin();
    for (; sIt != g_speakers.end(); ++sIt)
        (*sIt)->detachSource();

    g_speakers.clear();
    g_sourcePool.clear();
}

SampleBufferPtr findOrLoadSample(const std::string& path)
{
    if (!g_soundDevice || path.empty())
        return SampleBufferPtr();

    map<string, SampleBufferPtr>::iterator it = g_sampleCache.find(path);
    if (it != g_sampleCache.end())
        return it->second;

    SampleBufferPtr buffer = loadWAV(*g_soundDevice, path);
    if (!buffer)
        cout << "findOrLoadSample() could not load " << path << endl;
    g_sampleCache[path] = buffer;
    return buffer;
}

SpeakerEmitterPtr addSpeakerEmitter(const SpeakerEmitterPtr& emitter)
{
    if (!emitter)
        return SpeakerEmitterPtr();
    g_speakers.push_back(emitter);
    return emitter;
}

static void ensureSourcePool()
{
    if (!g_soundDevice || !g_sourcePool.empty())
        return;

    for (int i = 0; i < MAX_SOUND_CHANNELS; ++i)
        g_sourcePool.push_back(g_soundDevice->createSource());
}

void linkMapSpeakers(Q3BSPMesh* worldmodel)
{
    g_soundWorld = worldmodel;
    if (!worldmodel)
        return;

    worldmodel->unlinkAllSounds();
    SpeakerEmitter::SMARTPTRITERATOR sIt = g_speakers.begin();
    for (; sIt != g_speakers.end(); ++sIt)
        worldmodel->linkSound((*sIt).getPointer());
}

void updateMapSounds(const VECTOR3& listenerPos, Q3BSPMesh* worldmodel)
{
    if (!worldmodel)
        worldmodel = g_soundWorld;
    if (!g_soundDevice || g_speakers.empty())
        return;

    ensureSourcePool();
    ++g_soundCullFrame;

    vector<SpeakerEmitter*> audible;
    if (worldmodel && worldmodel->getNumAreas() > 0)
        worldmodel->cullSounds(listenerPos, g_soundCullFrame, audible);
    else {
        SpeakerEmitter::SMARTPTRITERATOR sIt = g_speakers.begin();
        for (; sIt != g_speakers.end(); ++sIt) {
            SpeakerEmitter* sp = (*sIt).getPointer();
            if (!sp->isAudible(listenerPos))
                continue;
            sp->setCullFrame(g_soundCullFrame);
            audible.push_back(sp);
        }
    }

    SpeakerEmitter::SMARTPTRITERATOR sIt = g_speakers.begin();
    for (; sIt != g_speakers.end(); ++sIt) {
        SpeakerEmitter* sp = (*sIt).getPointer();
        if (sp->getSource() && sp->getCullFrame() != g_soundCullFrame)
            sp->detachSource();
    }

    for (size_t a = 0; a < audible.size(); ++a) {
        SpeakerEmitter* sp = audible[a];
        if (sp->getSource())
            continue;

        list<SoundSourcePtr>::iterator srcIt = g_sourcePool.begin();
        for (; srcIt != g_sourcePool.end(); ++srcIt) {
            bool used = false;
            SpeakerEmitter::SMARTPTRITERATOR eIt = g_speakers.begin();
            for (; eIt != g_speakers.end(); ++eIt) {
                if ((*eIt)->getSource().getPointer() == (*srcIt).getPointer()) {
                    used = true;
                    break;
                }
            }
            if (!used) {
                sp->attachSource(*srcIt);
                sp->applyGain(getMasterVolume());
                break;
            }
        }
    }
}
