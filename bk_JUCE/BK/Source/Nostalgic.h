/*
  ==============================================================================

    Nostalgic.h
    Created: 22 Nov 2016 3:46:45pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef NOSTALGIC_H_INCLUDED
#define NOSTALGIC_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

#include "AudioConstants.h"

#include "BKSynthesiser.h"

#include "BKUtilities.h"


class NostalgicPreparation : public ReferenceCountedObject
{
public:
    typedef ReferenceCountedObjectPtr<NostalgicPreparation> Ptr;
    
    NostalgicPreparation(int waveDistance,
                         int undertow,
                         float transposition,
                         float gain,
                         float lengthMultiplier,
                         float beatsToSkip,
                         NostalgicSyncMode mode,
                         int syncTarget,
                         Array<float> tuningOffsets,
                         int basePitch):
    nWaveDistance(waveDistance),
    nUndertow(undertow),
    nTransposition(transposition),
    nGain(gain),
    nLengthMultiplier(lengthMultiplier),
    nBeatsToSkip(beatsToSkip),
    nMode(mode),
    nSyncTarget(syncTarget),
    nTuningOffsets(tuningOffsets),
    nBasePitch(basePitch)
    {
        
    }
    
    NostalgicPreparation()
    {
        nWaveDistance = 0;
        nUndertow = 0;
        nTransposition = 0.0;
        nGain = 1.0;
        nLengthMultiplier = 1.0;
        nBeatsToSkip = 0.0;
        nMode = NoteLengthSync,
        nSyncTarget = 0;
        nTuningOffsets = Array<float>(aEqualTuning, 12);
        nBasePitch = 0;
    }
    
    ~NostalgicPreparation()
    {
        
    }
    
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
        DBG("nTuningOffsets: " + floatArrayToString(nTuningOffsets));
        DBG("nBasePitch: " + String(nBasePitch));
    }
    
    const int getWavedistance() const noexcept                      {return nWaveDistance;      }
    const int getUndertow() const noexcept                          {return nUndertow;          }
    const float getTransposition() const noexcept                   {return nTransposition;     }
    const float getGain() const noexcept                            {return nGain;              }
    const float getLengthMultiplier() const noexcept                {return nLengthMultiplier;  }
    const float getBeatsToSkip() const noexcept                     {return nBeatsToSkip;       }
    const NostalgicSyncMode getMode() const noexcept                {return nMode;              }
    const int getSyncTarget() const noexcept                        {return nSyncTarget;        }
    const Array<float> getTuningOffsets() const noexcept            {return nTuningOffsets;     }
    const int getBasePitch() const noexcept                         {return nBasePitch;         }
    
    void setWaveDistance(int waveDistance)                          {nWaveDistance = waveDistance;          }
    void setUndertow(int undertow)                                  {nUndertow = undertow;                  }
    void setTransposition(float transposition)                      {nTransposition = transposition;        }
    void setGain(float gain)                                        {nGain = gain;                          }
    void setLengthMultiplier(float lengthMultiplier)                {nLengthMultiplier = lengthMultiplier;  }
    void setBeatsToSkip(float beatsToSkip)                          {nBeatsToSkip = beatsToSkip;            }
    void setMode(NostalgicSyncMode mode)                            {nMode = mode;                          }
    void setSyncTarget(int syncTarget)                              {nSyncTarget = syncTarget;              }
    void setTuningOffsets(Array<float> tuningOffsets)               {nTuningOffsets = tuningOffsets;        }
    void setBasePitch(int basePitch)                                {nBasePitch = basePitch;                }

    
private:
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
    
    Array<float> nTuningOffsets;
    int nBasePitch;
    
    JUCE_LEAK_DETECTOR(NostalgicPreparation);
};

class NostalgicProcessor
{
public:

    NostalgicProcessor(BKSynthesiser *s, NostalgicPreparation::Ptr prep);
    virtual ~NostalgicProcessor();
    
    //called with every audio vector
    void processBlock(int numSamples, int midiChannel);
    
    //begin timing played note length, called with noteOn
    void keyOn(int midiNoteNumber, float midiNoteVelocity);
    
    //begin playing reverse note, called with noteOff
    void keyOff(int midiNoteNumber, int midiChannel, int timeToNext, int beatLength);
    
private:
    
    void playNote(int channel, int note);
    
    //finish timing played note length, called with noteOff
    void noteLengthTimerOff(int midiNoteNumber);
    
    //begin timing reverse note play time
    void reverseNoteLengthTimerOn(int midiNoteNumber, float noteLength);
    
    //move timers forward by blocksize
    void incrementTimers(int numSamples);
    
    //data and pointers
    BKSynthesiser *synth;
    NostalgicPreparation::Ptr preparation;
    double sampleRate;

    Array<uint64> noteLengthTimers;        //store current length of played notes here
    Array<int> activeNotes;             //table of notes currently being played by player
    Array<float> velocities;            //table of velocities played
    
    Array<uint64> reverseLengthTimers;     //keep track of how long reverse notes have been playing
    Array<int> activeReverseNotes;      //table of active reverse notes
    Array<int> reverseTargetLength;     //target reverse length (in samples)
    
    //store values so that undertow note retains preparation from reverse note
    Array<float> undertowVelocities;
    Array<NostalgicPreparation::Ptr> undertowPreparations;
    
    JUCE_LEAK_DETECTOR (NostalgicProcessor) //is this the right one to use here?
};



#endif  // NOSTALGIC_H_INCLUDED