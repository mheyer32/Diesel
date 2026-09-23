/*
This file is part of Diesel
(c) 2002-2026 by Mathias Heyer
*/
#if !defined(AFX_SPEAKEREMITTER_H_INCLUDED_)
#define AFX_SPEAKEREMITTER_H_INCLUDED_

#include <adt/RefCount.h>
#include <adt/SmartPointer.h>
#include <math/Vector3.h>
#include <sound/SoundDevice.h>

#include <list>

class SpeakerEmitter : public CRefCount
{
public:
    typedef SmartPointer<SpeakerEmitter> SMARTPTR;
    typedef std::list<SMARTPTR>          SMARTPTRLIST;
    typedef SMARTPTRLIST::iterator       SMARTPTRITERATOR;

    enum Flags
    {
        LOOPING   = 1,
        GLOBAL    = 2,
        ACTIVATOR = 4
    };

    enum State
    {
        STOP,
        PLAYING
    };

    SpeakerEmitter();

    void setPosition(const VECTOR3& pos) { m_position = pos; }
    const VECTOR3& getPosition() const { return m_position; }

    void setBuffer(const SampleBufferPtr& buffer) { m_buffer = buffer; }
    const SampleBufferPtr& getBuffer() const { return m_buffer; }

    void     setFlags(unsigned flags) { m_flags = flags; }
    unsigned getFlags() const { return m_flags; }

    void  setMaxDistance(float q3Units) { m_maxDistance = q3Units; }
    float getMaxDistance() const { return m_maxDistance; }

    void  setRefDistance(float q3Units) { m_refDistance = q3Units; }
    float getRefDistance() const { return m_refDistance; }

    void  play();
    void  stop();
    State getState() const { return m_state; }

    bool isAudible(const VECTOR3& listenerPos);

    void attachSource(const SoundSourcePtr& source);
    void detachSource();
    void applyGain(float masterVolume);
    const SoundSourcePtr& getSource() const { return m_source; }

    void  setAreaLinks(void* links) { m_areaLinks = links; }
    void* getAreaLinks() const { return m_areaLinks; }

    void setCullFrame(int frame) { m_cullFrame = frame; }
    int  getCullFrame() const { return m_cullFrame; }

protected:
    virtual ~SpeakerEmitter();

private:
    SpeakerEmitter(const SpeakerEmitter&);
    SpeakerEmitter& operator=(const SpeakerEmitter&);

    float channelGain() const;

    VECTOR3         m_position;
    SampleBufferPtr m_buffer;
    SoundSourcePtr  m_source;
    unsigned        m_flags;
    float           m_maxDistance;
    float           m_refDistance;
    State           m_state;
    void*           m_areaLinks;
    int             m_cullFrame;
};

typedef SpeakerEmitter::SMARTPTR SpeakerEmitterPtr;

#endif
