/*
  ==============================================================================

    Nostalgic.h
    Created: 22 Nov 2016 3:46:45pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef NOSTALGIC_H_INCLUDED
#define NOSTALGIC_H_INCLUDED

#include "BKUtilities.h"

#include "BKSynthesiser.h"

#include "Synchronic.h"
#include "Tuning.h"
#include "Keymap.h"

class NostalgicPreparation : public ReferenceCountedObject
{
public:
    typedef ReferenceCountedObjectPtr<NostalgicPreparation>   Ptr;
    typedef Array<NostalgicPreparation::Ptr>                  PtrArr;
    typedef Array<NostalgicPreparation::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<NostalgicPreparation>                  Arr;
    typedef OwnedArray<NostalgicPreparation, CriticalSection> CSArr;
    
    
    NostalgicPreparation(NostalgicPreparation::Ptr p):
    nWaveDistance(p->getWavedistance()),
    nUndertow(p->getUndertow()),
    nTransposition(p->getTransposition()),
    nGain(p->getGain()),
    nLengthMultiplier(p->getLengthMultiplier()),
    nBeatsToSkip(p->getBeatsToSkip()),
    nMode(p->getMode()),
    nSyncTarget(p->getSyncTarget())
    {
        
    }
    
    NostalgicPreparation(int waveDistance,
                         int undertow,
                         Array<float> transposition,
                         float gain,
                         float lengthMultiplier,
                         float beatsToSkip,
                         NostalgicSyncMode mode,
                         int syncTarget,
                         TuningSystem tuning,
                         PitchClass basePitch,
                         Tuning::Ptr t):
    nWaveDistance(waveDistance),
    nUndertow(undertow),
    nTransposition(transposition),
    nGain(gain),
    nLengthMultiplier(lengthMultiplier),
    nBeatsToSkip(beatsToSkip),
    nMode(mode),
    nSyncTarget(syncTarget)
    {
        
    }
    
    NostalgicPreparation(Tuning::Ptr t):
    nWaveDistance(0),
    nUndertow(0),
    nTransposition(Array<float>({0.0})),
    nGain(1.0),
    nLengthMultiplier(1.0),
    nBeatsToSkip(0.0),
    nMode(NoteLengthSync),
    nSyncTarget(0)
    {

    }
    
    ~NostalgicPreparation()
    {
        
    }
    
    inline void copy(NostalgicPreparation::Ptr n)
    {
        nWaveDistance = n->getWavedistance();
        nUndertow = n->getUndertow();
        nTransposition = n->getTransposition();
        nGain = n->getGain();
        nLengthMultiplier = n->getLengthMultiplier();
        nBeatsToSkip = n->getBeatsToSkip();
        nMode = n->getMode();
        nSyncTarget = n->getSyncTarget();
    }
    
    inline bool compare (NostalgicPreparation::Ptr n)
    {
        return (nWaveDistance == n->getWavedistance() &&
                nUndertow == n->getUndertow() &&
                nTransposition == n->getTransposition() &&
                nGain == n->getGain() &&
                nLengthMultiplier == n->getLengthMultiplier() &&
                nBeatsToSkip == n->getBeatsToSkip() &&
                nMode == n->getMode() &&
                nSyncTarget == n->getSyncTarget());
    }
    
    inline const String getName() const noexcept {return name;}
    inline void setName(String n){name = n;}
    
    inline const int getWavedistance() const noexcept                      {return nWaveDistance;      }
    inline const int getUndertow() const noexcept                          {return nUndertow;          }
    inline const Array<float> getTransposition() const noexcept            {return nTransposition;     }
    inline const float getGain() const noexcept                            {return nGain;              }
    inline const float getLengthMultiplier() const noexcept                {return nLengthMultiplier;  }
    inline const float getBeatsToSkip() const noexcept                     {return nBeatsToSkip;       }
    inline const NostalgicSyncMode getMode() const noexcept                {return nMode;              }
    inline const int getSyncTarget() const noexcept                        {return nSyncTarget;        }
    inline const SynchronicProcessor::Ptr getSyncTargetProcessor() const noexcept
                                                                           {return nSyncProcessor;     }
    
    inline void setWaveDistance(int waveDistance)                          {nWaveDistance = waveDistance;          }
    inline void setUndertow(int undertow)                                  {nUndertow = undertow;                  }
    inline void setTransposition(Array<float> transposition)               {nTransposition = transposition;        }
    inline void setGain(float gain)                                        {nGain = gain;                          }
    inline void setLengthMultiplier(float lengthMultiplier)                {nLengthMultiplier = lengthMultiplier;  }
    inline void setBeatsToSkip(float beatsToSkip)                          {nBeatsToSkip = beatsToSkip;            }
    inline void setMode(NostalgicSyncMode mode)                            {nMode = mode;                          }
    inline void setSyncTarget(int syncTarget)                              {nSyncTarget = syncTarget;}
    inline void setSyncTargetProcessor(SynchronicProcessor::Ptr syncTargetProcessor)
                                                                           {nSyncProcessor = syncTargetProcessor;}

    void print(void)
    {
        DBG("nWaveDistance: " + String(nWaveDistance));
        DBG("nUndertow: " + String(nUndertow));
        DBG("nTransposition: " + floatArrayToString(nTransposition));
        DBG("nGain: " + String(nGain));
        DBG("nLengthMultiplier: " + String(nLengthMultiplier));
        DBG("nBeatsToSkip: " + String(nBeatsToSkip));
        DBG("nMode: " + String(nMode));
        DBG("nSyncTarget: " + String(nSyncTarget));
    }
    
    
private:
    String name;
    int nWaveDistance;  //ms; distance from beginning of sample to stop reverse playback and begin undertow
    int nUndertow;      //ms; length of time to play forward after directional change
    /*
     one thing i discovered is that the original bK actually plays the forward undertow
     sample for TWICE this value; the first half at steady gain, and then the second
     half with a ramp down. i'm not sure why, and i'm not sure i want to keep that
     behavior, but if we don't, then the instrument will sound different when we import
     old presets
     --dt
     */
    
    Array<float> nTransposition;       //transposition, in half steps
    float nGain;                //gain multiplier
    float nLengthMultiplier;    //note-length mode: toscale reverse playback time
    float nBeatsToSkip;         //synchronic mode: beats to skip before reverse peak
    NostalgicSyncMode nMode;    //which sync mode to use
    int nSyncTarget;            //which synchronic layer to sync to, when nMode = NostalgicSyncSynchronic
    SynchronicProcessor::Ptr nSyncProcessor;
    
    //internal keymap for resetting internal values to static
    //Keymap::Ptr resetMap = new Keymap(0);
    
    JUCE_LEAK_DETECTOR(NostalgicPreparation);
};


class NostalgicNoteStuff : public ReferenceCountedObject
{
public:
    
    typedef ReferenceCountedObjectPtr<NostalgicNoteStuff>   Ptr;
    typedef Array<NostalgicNoteStuff::Ptr>                  PtrArr;
    typedef Array<NostalgicNoteStuff::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<NostalgicNoteStuff>                  Arr;
    typedef OwnedArray<NostalgicNoteStuff, CriticalSection> CSArr;
    
    NostalgicNoteStuff(int noteNumber) : notenumber(noteNumber)
    {
        resetReverseTimer();
        resetUndertowTimer();
    }
    
    ~NostalgicNoteStuff() {}
    
    void setNoteNumber(int newnote)                         { notenumber = newnote; }
    inline const int getNoteNumber() const noexcept         { return notenumber; }
    
    void setPrepAtKeyOn(NostalgicPreparation::Ptr nprep)    { prepAtKeyOn = nprep; }
    NostalgicPreparation::Ptr getPrepAtKeyOn()              { return prepAtKeyOn; }
    
    void setTuningAtKeyOn(float t)                          { tuningAtKeyOn = t; }
    inline const float getTuningAtKeyOn() const noexcept    { return tuningAtKeyOn; }
    
    void setVelocityAtKeyOn(float v)                        { velocityAtKeyOn = v; }
    inline const float getVelocityAtKeyOn() const noexcept  { return velocityAtKeyOn; }
    
    void incrementReverseTimer(uint64 numsamples)           { reverseTimer += numsamples; }
    void incrementUndertowTimer(uint64 numsamples)          { undertowTimer += numsamples; }
    
    void resetReverseTimer()                                { reverseTimer = 0; }
    void resetUndertowTimer()                               { undertowTimer = 0; }
    
    void setReverseStartPosition(uint64 rsp)                        { reverseStartPosition = rsp; }
    inline const uint64 getReverseStartPosition() const noexcept    { return reverseStartPosition; }
    
    void setUndertowStartPosition(uint64 usp)                        { undertowStartPosition = usp; }
    inline const uint64 getUndertowStartPosition() const noexcept    { return undertowStartPosition; }
    
    void setReverseTargetLength(uint64 rtl)                         { reverseTargetLength = rtl; }
    void setUndertowTargetLength(uint64 utl)                        { undertowTargetLength = utl; }
    inline const uint64 getUndertowTargetLength() const noexcept    { return undertowTargetLength; }
    
    bool reverseTimerExceedsTarget()    { if(reverseTimer > reverseTargetLength) return true; else return false; }
    bool undertowTimerExceedsTarget()   { if(undertowTimer > undertowTargetLength) return true; else return false; }
    
    inline const uint64 getReversePlayPosition()     { return (reverseStartPosition - reverseTimer); }
    inline const uint64 getUndertowPlayPosition()    { return (undertowStartPosition + undertowTimer); }
    
    bool isActive() { if(reverseStartPosition < reverseTimer) return false; else return true; }
    
private:
    
    int notenumber;
    NostalgicPreparation::Ptr prepAtKeyOn;
    float tuningAtKeyOn;
    float velocityAtKeyOn;
    
    uint64 reverseTimer;
    uint64 undertowTimer;
    
    uint64 reverseStartPosition;
    uint64 reversePosition;
    
    uint64 undertowStartPosition;
    uint64 undertowPosition;
    
    uint64 reverseTargetLength;
    uint64 undertowTargetLength;
    
    JUCE_LEAK_DETECTOR(NostalgicNoteStuff);
};


/* ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ NOSTALGIC ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ */

class Nostalgic : public ReferenceCountedObject
{
    
public:
    typedef ReferenceCountedObjectPtr<Nostalgic>   Ptr;
    typedef Array<Nostalgic::Ptr>                  PtrArr;
    typedef Array<Nostalgic::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<Nostalgic>                  Arr;
    typedef OwnedArray<Nostalgic, CriticalSection> CSArr;
    
    
    Nostalgic(BKSynthesiser *s,
              NostalgicPreparation::Ptr prep,
              Tuning::Ptr tuning,
              Synchronic::Ptr sync,
              int Id):
    sPrep(new NostalgicPreparation(prep)),
    aPrep(new NostalgicPreparation(sPrep)),
    Id(Id),
    name(String(Id))
    {
    }
    
    
    Nostalgic(BKSynthesiser *s,
              Tuning::Ptr tuning,
              Synchronic::Ptr sync,
              BKUpdateState::Ptr us,
              int Id):
    Id(Id),
    name(String(Id)),
    updateState(us)
    {
        sPrep       = new NostalgicPreparation(tuning);
        aPrep       = new NostalgicPreparation(sPrep);
    };
    
    ~Nostalgic() {};
    
    inline void copy(Nostalgic::Ptr from)
    {
        sPrep->copy(from->sPrep);
        aPrep->copy(sPrep);
    }
    
    inline ValueTree getState(void)
    {
        ValueTree prep( vtagNostalgic );
        
        prep.setProperty( "Id",Id, 0);
        prep.setProperty( "name", name, 0);
        
        prep.setProperty( ptagNostalgic_waveDistance,       sPrep->getWavedistance(), 0);
        prep.setProperty( ptagNostalgic_undertow,           sPrep->getUndertow(), 0);
        
        ValueTree transp( vtagNostalgic_transposition);
        int count = 0;
        for (auto f : sPrep->getTransposition())
        {
            transp.      setProperty( ptagFloat + String(count++), f, 0);
        }
        prep.addChild(transp, -1, 0);
        
        prep.setProperty( ptagNostalgic_gain,               sPrep->getGain(), 0);
        prep.setProperty( ptagNostalgic_lengthMultiplier,   sPrep->getLengthMultiplier(), 0);
        prep.setProperty( ptagNostalgic_beatsToSkip,        sPrep->getBeatsToSkip(), 0);
        prep.setProperty( ptagNostalgic_mode,               sPrep->getMode(), 0);
        prep.setProperty( ptagNostalgic_syncTarget,         sPrep->getSyncTarget(), 0);
        
        return prep;
    }
    
    inline void setState(XmlElement* e, Tuning::PtrArr tuning, Synchronic::PtrArr synchronic)
    {
        int i; float f;
        
        Id = e->getStringAttribute("Id").getIntValue();
        
        String n = e->getStringAttribute("name");
        
        if (n != String::empty)     name = n;
        else                        name = String(Id);

        
        i = e->getStringAttribute(ptagNostalgic_waveDistance).getIntValue();
        sPrep->setWaveDistance(i);
        
        i = e->getStringAttribute(ptagNostalgic_undertow).getIntValue();
        sPrep->setUndertow(i);
        
        forEachXmlChildElement (*e, sub)
        {
            if (sub->hasTagName(vtagNostalgic_transposition))
            {
                Array<float> transp;
                for (int k = 0; k < 128; k++)
                {
                    String attr = sub->getStringAttribute(ptagFloat + String(k));
                    
                    if (attr == String::empty) break;
                    else
                    {
                        f = attr.getFloatValue();
                        transp.add(f);
                    }
                }
                
                sPrep->setTransposition(transp);
                
            }
        }
        
        
        f = e->getStringAttribute(ptagNostalgic_lengthMultiplier).getFloatValue();
        sPrep->setLengthMultiplier(f);
        
        f = e->getStringAttribute(ptagNostalgic_beatsToSkip).getFloatValue();
        sPrep->setBeatsToSkip(f);
        
        f = e->getStringAttribute(ptagNostalgic_gain).getFloatValue();
        sPrep->setGain(f);
        
        i = e->getStringAttribute(ptagNostalgic_mode).getIntValue();
        sPrep->setMode((NostalgicSyncMode)i);
        
        i = e->getStringAttribute(ptagNostalgic_syncTarget).getIntValue();
        sPrep->setSyncTarget(i);
        
        aPrep->copy(sPrep);
        
    }
    
    inline int getId() {return Id;}
    
    NostalgicPreparation::Ptr      sPrep;
    NostalgicPreparation::Ptr      aPrep;
    
    void reset()
    {
        aPrep->copy(sPrep);
        updateState->nostalgicPreparationDidChange = true;
        DBG("nostalgic reset");
    }
    
    //void didChange(bool which) { updateState->nostalgicPreparationDidChange = which; }
    
    inline String getName(void) const noexcept {return name;}inline void setName(String newName)
    {
        name = newName;
        updateState->nostalgicPreparationDidChange = true;
    }
    
private:
    int Id;
    String name;
    BKUpdateState::Ptr updateState;
    
    JUCE_LEAK_DETECTOR(Nostalgic)
};


class NostalgicModPreparation : public ReferenceCountedObject
{
public:
    
    typedef ReferenceCountedObjectPtr<NostalgicModPreparation>   Ptr;
    typedef Array<NostalgicModPreparation::Ptr>                  PtrArr;
    typedef Array<NostalgicModPreparation::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<NostalgicModPreparation>                  Arr;
    typedef OwnedArray<NostalgicModPreparation, CriticalSection> CSArr;
    
    /*
     NostalgicId = 0,
     NostalgicTuning,
     NostalgicWaveDistance,
     NostalgicUndertow,
     NostalgicTransposition,
     NostalgicGain,
     NostalgicLengthMultiplier,
     NostalgicBeatsToSkip,
     NostalgicMode,
     NostalgicSyncTarget,
     NostalgicParameterTypeNil
     
     */
    
    NostalgicModPreparation(NostalgicPreparation::Ptr p, int Id):
    Id(Id)
    {
        param.ensureStorageAllocated((int)cNostalgicParameterTypes.size());
        
        param.set(NostalgicWaveDistance, String(p->getWavedistance()));
        param.set(NostalgicUndertow, String(p->getUndertow()));
        param.set(NostalgicTransposition, floatArrayToString(p->getTransposition()));
        param.set(NostalgicGain, String(p->getGain()));
        param.set(NostalgicLengthMultiplier, String(p->getLengthMultiplier()));
        param.set(NostalgicBeatsToSkip, String(p->getBeatsToSkip()));
        param.set(NostalgicMode, String(p->getMode()));
        param.set(NostalgicSyncTarget, String(p->getSyncTarget()));
        
    }
    
    
    NostalgicModPreparation(int Id):
    Id(Id)
    {
        param.set(NostalgicWaveDistance, "");
        param.set(NostalgicUndertow, "");
        param.set(NostalgicTransposition, "");
        param.set(NostalgicGain, "");
        param.set(NostalgicLengthMultiplier, "");
        param.set(NostalgicBeatsToSkip, "");
        param.set(NostalgicMode, "");
        param.set(NostalgicSyncTarget, "");
    }
    
    inline void setId(int newId) { Id = newId; }
    inline int getId(void) const noexcept { return Id; }
    
    inline ValueTree getState(void)
    {
        ValueTree prep( vtagModNostalgic );
        
        prep.setProperty( "Id",Id, 0);
        
        String p = "";
        
        p = getParam(NostalgicWaveDistance);
        if (p != String::empty) prep.setProperty( ptagNostalgic_waveDistance,       p.getIntValue(), 0);
        
        p = getParam(NostalgicUndertow);
        if (p != String::empty) prep.setProperty( ptagNostalgic_undertow,           p.getIntValue(), 0);
        
        ValueTree transp( vtagNostalgic_transposition);
        int count = 0;
        p = getParam(NostalgicTransposition);
        if (p != String::empty)
        {
            Array<float> m = stringToFloatArray(p);
            for (auto f : m)
            {
                transp.      setProperty( ptagFloat + String(count++), f, 0);
            }
        }
        prep.addChild(transp, -1, 0);
        
        p = getParam(NostalgicGain);
        if (p != String::empty) prep.setProperty( ptagNostalgic_gain,               p.getFloatValue(), 0);
        
        p = getParam(NostalgicLengthMultiplier);
        if (p != String::empty) prep.setProperty( ptagNostalgic_lengthMultiplier,   p.getFloatValue(), 0);
        
        p = getParam(NostalgicBeatsToSkip);
        if (p != String::empty) prep.setProperty( ptagNostalgic_beatsToSkip,        p.getFloatValue(), 0);
        
        p = getParam(NostalgicMode);
        if (p != String::empty) prep.setProperty( ptagNostalgic_mode,               p.getIntValue(), 0);
        
        p = getParam(NostalgicSyncTarget);
        if (p != String::empty) prep.setProperty( ptagNostalgic_syncTarget,         p.getIntValue(), 0);
        
        
        return prep;
    }
    
    inline void setState(XmlElement* e)
    {
        float f;
        
        Id = e->getStringAttribute("Id").getIntValue();
        
        String p = e->getStringAttribute(ptagNostalgic_waveDistance);
        setParam(NostalgicWaveDistance, p);
        
        p = e->getStringAttribute(ptagNostalgic_undertow);
        setParam(NostalgicUndertow, p);
        
        forEachXmlChildElement (*e, sub)
        {
            if (sub->hasTagName(vtagNostalgic_transposition))
            {
                Array<float> transp;
                for (int k = 0; k < 128; k++)
                {
                    String attr = sub->getStringAttribute(ptagFloat + String(k));
                    
                    if (attr == String::empty) break;
                    else
                    {
                        f = attr.getFloatValue();
                        transp.add(f);
                    }
                }
                
                setParam(NostalgicTransposition, floatArrayToString(transp));
                
            }
        }
        
        p = e->getStringAttribute(ptagNostalgic_lengthMultiplier);
        setParam(NostalgicLengthMultiplier, p);
        
        p = e->getStringAttribute(ptagNostalgic_beatsToSkip);
        setParam(NostalgicBeatsToSkip, p);
        
        p = e->getStringAttribute(ptagNostalgic_gain);
        setParam(NostalgicGain, p);
        
        p = e->getStringAttribute(ptagNostalgic_mode);
        setParam(NostalgicMode, p);
        
        p = e->getStringAttribute(ptagNostalgic_syncTarget);
        setParam(NostalgicSyncTarget, p);
    }
    
    
    ~NostalgicModPreparation(void)
    {
        
    }
    
    inline void copy(NostalgicPreparation::Ptr p)
    {
        param.set(NostalgicWaveDistance, String(p->getWavedistance()));
        param.set(NostalgicUndertow, String(p->getUndertow()));
        param.set(NostalgicTransposition, floatArrayToString(p->getTransposition()));
        param.set(NostalgicGain, String(p->getGain()));
        param.set(NostalgicLengthMultiplier, String(p->getLengthMultiplier()));
        param.set(NostalgicBeatsToSkip, String(p->getBeatsToSkip()));
        param.set(NostalgicMode, String(p->getMode()));
        param.set(NostalgicSyncTarget, String(p->getSyncTarget()));
    }
    
    inline void copy(NostalgicModPreparation::Ptr p)
    {
        for (int i = NostalgicId+1; i < NostalgicParameterTypeNil; i++)
        {
            param.set(i, p->getParam((NostalgicParameterType)i));
        }
    }
    
    void clearAll()
    {
        for (int i = NostalgicId+1; i < NostalgicParameterTypeNil; i++)
        {
            param.set(i, "");
        }
    }
    
    inline const StringArray getStringArray(void) { return param; }
    
    inline const String getParam(NostalgicParameterType type)
    {
        if (type != NostalgicId)
            return param[type];
        else
            return "";
    }
    
    inline void setParam(NostalgicParameterType type, String val) { param.set(type, val);}
    
    void print(void)
    {
        
    }
    
    inline String getName(void) const noexcept {return name;}
    inline void setName(String newName) {name = newName;}
    
private:
    int Id;
    String name;
    StringArray          param;
    
    JUCE_LEAK_DETECTOR(NostalgicModPreparation);
};

class NostalgicProcessor : public ReferenceCountedObject
{
    
public:
    typedef ReferenceCountedObjectPtr<NostalgicProcessor>   Ptr;
    typedef Array<NostalgicProcessor::Ptr>                  PtrArr;
    typedef Array<NostalgicProcessor::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<NostalgicProcessor>                  Arr;
    typedef OwnedArray<NostalgicProcessor, CriticalSection> CSArr;
    
    NostalgicProcessor(Nostalgic::Ptr nostalgic,
                       TuningProcessor::Ptr tuning,
                       SynchronicProcessor::Ptr synchronic,
                       BKSynthesiser *s);
    
    virtual ~NostalgicProcessor();
    
    //called with every audio vector
    void processBlock(int numSamples, int midiChannel);
    
    //begin timing played note length, called with noteOn
    void keyPressed(int midiNoteNumber, float midiNoteVelocity, int midiChannel);
    
    //begin playing reverse note, called with noteOff
    void keyReleased(int midiNoteNumber, int midiChannel);
    
    void postRelease(int midiNoteNumber, int midiChannel);
    
    inline void attachToSynthesiser(BKSynthesiser* main)
    {
        synth = main;
    }
    
    inline void setNostalgic(Nostalgic::Ptr nost)
    {
        nostalgic = nost;
    }
    
    inline Nostalgic::Ptr getNostalgic(void) const noexcept
    {
        return nostalgic;
    }
    
    inline void setSynchronic(SynchronicProcessor::Ptr sync)
    {
        synchronic = sync;
    }
    
    inline SynchronicProcessor::Ptr getSynchronic(void) const noexcept
    {
        return synchronic;
    }
    
    inline int getSynchronicId(void)
    {
        return synchronic->getId();
    }
    
    inline void setTuning(TuningProcessor::Ptr p)
    {
        tuner = p;
    }
    
    inline TuningProcessor::Ptr getTuning(void)
    {
        return tuner;
    }
    
    void prepareToPlay(double sr, BKSynthesiser* main)
    {
        sampleRate = sr;
        synth = main;
    }
    
    inline int getId(void) const noexcept { return nostalgic->getId(); }
    
    Array<int> getPlayPositions();
    Array<int> getUndertowPositions();
    
private:
    BKSynthesiser*              synth;
    
    Nostalgic::Ptr                  nostalgic;
    TuningProcessor::Ptr            tuner;
    SynchronicProcessor::Ptr        synchronic;
    
    Array<uint64> noteLengthTimers;     //store current length of played notes here
    Array<int> activeNotes;             //table of notes currently being played by player
    Array<bool> noteOn;                 // table of booleans representing state of each note
    Array<float> velocities;            //table of velocities played
    
    OwnedArray<NostalgicNoteStuff> reverseNotes;
    OwnedArray<NostalgicNoteStuff> undertowNotes;
    
    double sampleRate;
    
    //move timers forward by blocksize
    void incrementTimers(int numSamples);
    
    JUCE_LEAK_DETECTOR (NostalgicProcessor) //is this the right one to use here?
};

#endif  // NOSTALGIC_H_INCLUDED
