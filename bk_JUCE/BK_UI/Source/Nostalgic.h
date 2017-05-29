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
#include "Synchronic.h"
#include "BKSynthesiser.h"
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
    nSyncTarget(p->getSyncTarget()),
    tuning(p->getTuning())
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
    nSyncTarget(syncTarget),
    tuning(t)
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
    nSyncTarget(0),
    tuning(t)
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
        tuning = n->getTuning();
        //resetMap->copy(n->resetMap);
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
                nSyncTarget == n->getSyncTarget()&&
                tuning == n->getTuning());
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
    //inline const Keymap::Ptr getResetMap() const noexcept                  {return resetMap;           }
    
    inline void setWaveDistance(int waveDistance)                          {nWaveDistance = waveDistance;          }
    inline void setUndertow(int undertow)                                  {nUndertow = undertow;                  }
    inline void setTransposition(Array<float> transposition)                      {nTransposition = transposition;        }
    inline void setGain(float gain)                                        {nGain = gain;                          }
    inline void setLengthMultiplier(float lengthMultiplier)                {nLengthMultiplier = lengthMultiplier;  }
    inline void setBeatsToSkip(float beatsToSkip)                          {nBeatsToSkip = beatsToSkip;            }
    inline void setMode(NostalgicSyncMode mode)                            {nMode = mode;                          }
    inline void setSyncTarget(int syncTarget)                              {nSyncTarget = syncTarget;              }
    inline void setSyncTargetProcessor(SynchronicProcessor::Ptr syncTargetProcessor)
                                                                           {nSyncProcessor = syncTargetProcessor; }
    //inline void setResetMap(Keymap::Ptr k)                                 {resetMap = k;          }
    
    inline const Tuning::Ptr getTuning() const noexcept                    {return tuning; }
    inline void setTuning(Tuning::Ptr t)                                   {tuning = t; }
    
    
    

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
        //DBG("resetKeymap: " + intArrayToString(getResetMap()->keys()));
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
    
    Tuning::Ptr tuning;
    
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


class NostalgicProcessor : public ReferenceCountedObject
{
    
public:
    typedef ReferenceCountedObjectPtr<NostalgicProcessor>   Ptr;
    typedef Array<NostalgicProcessor::Ptr>                  PtrArr;
    typedef Array<NostalgicProcessor::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<NostalgicProcessor>                  Arr;
    typedef OwnedArray<NostalgicProcessor, CriticalSection> CSArr;
    
    NostalgicProcessor(BKSynthesiser* main,
                       NostalgicPreparation::Ptr active,
                       int Id);
    
    virtual ~NostalgicProcessor();
    
    void setCurrentPlaybackSampleRate(double sr);
    
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
    
    inline void setTuner(TuningProcessor::Ptr p)
    {
        tuner = p;
    }
    
    inline TuningProcessor::Ptr getTuner(void)
    {
        return tuner;
    }
    
    Array<int> getPlayPositions();
    Array<int> getUndertowPositions();
    
private:
    int Id;
    BKSynthesiser*              synth;
    NostalgicPreparation::Ptr   active;

    TuningProcessor::Ptr             tuner;
    
    //target Synchronic layer
    SynchronicProcessor::Ptr syncProcessor;
    
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
