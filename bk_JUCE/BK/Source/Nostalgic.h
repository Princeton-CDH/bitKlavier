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
    inline void setTuning(Tuning::Ptr t)                                   {tuning = t;  }
    
    
    

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
    
    NostalgicModPreparation(NostalgicPreparation::Ptr p)
    {
        param.ensureStorageAllocated(cNostalgicParameterTypes.size());
        
        param.set(NostalgicTuning, String(p->getTuning()->getId()));
        param.set(NostalgicWaveDistance, String(p->getWavedistance()));
        param.set(NostalgicUndertow, String(p->getUndertow()));
        param.set(NostalgicTransposition, floatArrayToString(p->getTransposition()));
        param.set(NostalgicGain, String(p->getGain()));
        param.set(NostalgicLengthMultiplier, String(p->getLengthMultiplier()));
        param.set(NostalgicBeatsToSkip, String(p->getBeatsToSkip()));
        param.set(NostalgicMode, String(p->getMode()));
        param.set(NostalgicSyncTarget, String(p->getSyncTarget()));
        
    }
    
    
    NostalgicModPreparation(void)
    {
        param.set(NostalgicTuning, "");
        param.set(NostalgicWaveDistance, "");
        param.set(NostalgicUndertow, "");
        param.set(NostalgicTransposition, "");
        param.set(NostalgicGain, "");
        param.set(NostalgicLengthMultiplier, "");
        param.set(NostalgicBeatsToSkip, "");
        param.set(NostalgicMode, "");
        param.set(NostalgicSyncTarget, "");
    }
    
    inline ValueTree getState(int Id)
    {
        ValueTree prep( vtagModNostalgic + String(Id));
        
        String p = "";
        
        p = getParam(NostalgicTuning);
        if (p != String::empty) prep.setProperty( ptagNostalgic_tuning,             p.getIntValue(), 0);
        
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
    
    
    ~NostalgicModPreparation(void)
    {
        
    }
    
    inline void copy(NostalgicPreparation::Ptr p)
    {
        param.set(NostalgicTuning, String(p->getTuning()->getId()));
        param.set(NostalgicWaveDistance, String(p->getWavedistance()));
        param.set(NostalgicUndertow, String(p->getUndertow()));
        param.set(NostalgicTransposition, floatArrayToString(p->getTransposition()));
        param.set(NostalgicGain, String(p->getGain()));
        param.set(NostalgicLengthMultiplier, String(p->getLengthMultiplier()));
        param.set(NostalgicBeatsToSkip, String(p->getBeatsToSkip()));
        param.set(NostalgicMode, String(p->getMode()));
        param.set(NostalgicSyncTarget, String(p->getSyncTarget()));
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
    
private:
    
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
    
    NostalgicProcessor(BKSynthesiser *s,
                       NostalgicPreparation::Ptr active,
                       int Id);
    
    virtual ~NostalgicProcessor();
    
    void setCurrentPlaybackSampleRate(double sr);
    
    //called with every audio vector
    void processBlock(int numSamples, int midiChannel);
    
    //begin timing played note length, called with noteOn
    void keyPressed(int midiNoteNumber, float midiNoteVelocity);
    
    //begin playing reverse note, called with noteOff
    void keyReleased(int midiNoteNumber, int midiChannel);
    
    void postRelease(int midiNoteNumber, int midiChannel);
    
private:
    int Id;
    BKSynthesiser*              synth;
    NostalgicPreparation::Ptr   active;

    TuningProcessor::Ptr             tuner;
    
    //target Synchronic layer
    SynchronicProcessor::Ptr syncProcessor;
    
    //store values so that undertow note retains preparation from reverse note
    NostalgicPreparation::PtrArr preparationAtKeyOn;
    Array<float> tuningsAtKeyOn;
    Array<float> velocitiesAtKeyOn;
    
    Array<uint64> noteLengthTimers;     //store current length of played notes here
    Array<int> activeNotes;             //table of notes currently being played by player
    Array<bool> noteOn;                 // table of booleans representing state of each note
    Array<float> velocities;            //table of velocities played
    
    Array<uint64> reverseLengthTimers;  //keep track of how long reverse notes have been playing
    Array<int> activeReverseNotes;      //table of active reverse notes
    Array<int> reverseTargetLength;     //target reverse length (in samples)
    
    double sampleRate;
    
    //functions
    void playNote(int channel, int note);
    
    //finish timing played note length, called with noteOff
    void noteLengthTimerOff(int midiNoteNumber);
    
    //begin timing reverse note play time
    void reverseNoteLengthTimerOn(int midiNoteNumber, float noteLength);
    
    //move timers forward by blocksize
    void incrementTimers(int numSamples);
    
    JUCE_LEAK_DETECTOR (NostalgicProcessor) //is this the right one to use here?
};



#endif  // NOSTALGIC_H_INCLUDED
