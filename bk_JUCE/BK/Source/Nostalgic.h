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

#include "Keymap.h"

#include "Tuning.h"


class NostalgicPreparation : public ReferenceCountedObject
{
public:
    typedef ReferenceCountedObjectPtr<NostalgicPreparation>   Ptr;
    typedef Array<NostalgicPreparation::Ptr>                  Arr;
    typedef Array<NostalgicPreparation::Ptr, CriticalSection> CSArr;
    
    NostalgicPreparation(int Id,
                         int waveDistance,
                         int undertow,
                         float transposition,
                         float gain,
                         float lengthMultiplier,
                         float beatsToSkip,
                         NostalgicSyncMode mode,
                         int syncTarget,
                         TuningSystem tuning,
                         PitchClass basePitch):
    Id(Id),
    nWaveDistance(waveDistance),
    nUndertow(undertow),
    nTransposition(transposition),
    nGain(gain),
    nLengthMultiplier(lengthMultiplier),
    nBeatsToSkip(beatsToSkip),
    nMode(mode),
    nSyncTarget(syncTarget),
    nTuning(tuning),
    nBasePitch(basePitch)
    {
        
    }
    
    NostalgicPreparation(int Id):
    Id(Id),
    nWaveDistance(0),
    nUndertow(0),
    nTransposition(0.0),
    nGain(1.0),
    nLengthMultiplier(1.0),
    nBeatsToSkip(0.0),
    nMode(NoteLengthSync),
    nSyncTarget(0),
    nTuning(PartialTuning),
    nBasePitch(C)
    {

    }
    
    ~NostalgicPreparation()
    {
        
    }
    
    
    inline const int getWavedistance() const noexcept                      {return nWaveDistance;      }
    inline const int getUndertow() const noexcept                          {return nUndertow;          }
    inline const float getTransposition() const noexcept                   {return nTransposition;     }
    inline const float getGain() const noexcept                            {return nGain;              }
    inline const float getLengthMultiplier() const noexcept                {return nLengthMultiplier;  }
    inline const float getBeatsToSkip() const noexcept                     {return nBeatsToSkip;       }
    inline const NostalgicSyncMode getMode() const noexcept                {return nMode;              }
    inline const int getSyncTarget() const noexcept                        {return nSyncTarget;        }
    inline const TuningSystem getTuning() const noexcept                   {return nTuning;            }
    inline const PitchClass getBasePitch() const noexcept                  {return nBasePitch;         }
    
    inline int getId(void) {   return Id; }
    
    inline void setWaveDistance(int waveDistance)                          {nWaveDistance = waveDistance;          }
    inline void setUndertow(int undertow)                                  {nUndertow = undertow;                  }
    inline void setTransposition(float transposition)                      {nTransposition = transposition;        }
    inline void setGain(float gain)                                        {nGain = gain;                          }
    inline void setLengthMultiplier(float lengthMultiplier)                {nLengthMultiplier = lengthMultiplier;  }
    inline void setBeatsToSkip(float beatsToSkip)                          {nBeatsToSkip = beatsToSkip;            }
    inline void setMode(NostalgicSyncMode mode)                            {nMode = mode;                          }
    inline void setSyncTarget(int syncTarget)                              {nSyncTarget = syncTarget;              }
    inline void setTuning(TuningSystem tuning)                             {nTuning = tuning;                      }
    inline void setBasePitch(PitchClass basePitch)                         {nBasePitch = basePitch;                }
    
    

    void print(void)
    {
        DBG("nWaveDistance: " + String(nWaveDistance));
        DBG("nUndertow: " + String(nUndertow));
        DBG("nTransposition: " + String(nTransposition));
        DBG("nGain: " + String(nGain));
        DBG("nLengthMultiplier: " + String(nLengthMultiplier));
        DBG("nBeatsToSkip: " + String(nBeatsToSkip));
        DBG("nMode: " + String(nMode));
        DBG("nSyncTarget: " + String(nSyncTarget));
        DBG("nBasePitch: " + String(nBasePitch));
    }
private:
    int Id;
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
    
    float nTransposition;       //transposition, in half steps
    float nGain;                //gain multiplier
    float nLengthMultiplier;    //note-length mode: toscale reverse playback time
    float nBeatsToSkip;         //synchronic mode: beats to skip before reverse peak
    NostalgicSyncMode nMode;    //which sync mode to use
    int nSyncTarget;            //which synchronic layer to sync to, when nMode = NostalgicSyncSynchronic
    
    TuningSystem nTuning;
    PitchClass nBasePitch;
    
    JUCE_LEAK_DETECTOR(NostalgicPreparation);
};

class NostalgicProcessor : public ReferenceCountedObject
{
    
public:
    typedef ReferenceCountedObjectPtr<NostalgicProcessor>   Ptr;
    typedef Array<NostalgicProcessor::Ptr>                  Arr;
    typedef Array<NostalgicProcessor::Ptr, CriticalSection> CSArr;
    
    NostalgicProcessor(
                       BKSynthesiser *s,
                       Keymap::Ptr km,
                       NostalgicPreparation::Ptr prep,
                       TuningPreparation::Ptr tPrep,
                       SynchronicProcessor::CSArr& proc,
                       int Id);
    
    virtual ~NostalgicProcessor();
    
    inline void setCurrentPlaybackSampleRate(double sr) { sampleRate = sr;}
    
    //called with every audio vector
    void processBlock(int numSamples, int midiChannel);
    
    //begin timing played note length, called with noteOn
    void keyPressed(int midiNoteNumber, float midiNoteVelocity);
    
    //begin playing reverse note, called with noteOff
    void keyReleased(int midiNoteNumber, int midiChannel);
    
    inline void setKeymap(Keymap::Ptr km)   { keymap = km;  }
    inline void setPreparation(NostalgicPreparation::Ptr prep)  { preparation = prep;   }
    //inline void setTuning(TuningProcessor::Ptr t)   { tuner = t;    }
    
    int                         getId(void)             { return Id;            }    
    Keymap::Ptr                 getKeymap(void)         { return keymap;        }
    NostalgicPreparation::Ptr   getPreparation(void)    { return preparation;   }
    
    int getKeymapId(void)       { return keymap->getId();       }
    int getPreparationId(void)  { return preparation->getId();  }
    
    inline void                             setTuning(TuningPreparation::Ptr t) { tuning = t;  tuner.setPreparation(tuning);         }
    inline TuningPreparation::Ptr           getTuning(void)                     { return tuning;           }
    inline int                              getTuningId(void)                   { return tuning->getId();  }
    
private:
    int Id;
    BKSynthesiser*              synth;
    Keymap::Ptr                 keymap;
    NostalgicPreparation::Ptr   preparation;
    TuningPreparation::Ptr      tuning;
    TuningProcessor             tuner;
    
    //target Synchronic layer
    SynchronicProcessor::CSArr& syncProcessor;
    
    //store values so that undertow note retains preparation from reverse note
    NostalgicPreparation::Arr preparationAtKeyOn;
    Array<float> tuningsAtKeyOn;
    Array<float> velocitiesAtKeyOn;
    
    Array<uint64> noteLengthTimers;     //store current length of played notes here
    Array<int> activeNotes;             //table of notes currently being played by player
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
