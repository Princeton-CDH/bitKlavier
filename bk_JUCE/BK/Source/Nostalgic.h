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
                         int transposition,
                         int gain,
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
        nTransposition = 0.;
        nGain = 1.;
        nLengthMultiplier = 1.;
        nBeatsToSkip = 0.;
        nMode = NostalgicSyncNoteLength;
        nSyncTarget = 0;
        nTuningOffsets = Array<float>(aEqualTuning);
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
    int nWaveDistance;          //ms; distance from beginning of sample to stop reverse playback and begin undertow
    int nUndertow;              //ms; length of time to play forward after directional change
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
    
    //begin timing played note length, called with noteOn
    void noteLengthTimerOn(int midiNoteNumber, float midiNoteVelocity);
    
    //finish timing played note length, called with noteOff
    void noteLengthTimerOff(int midiNoteNumber);
    
    //begin playing reverse note
    void playNote(int midiNoteNumber, int midiChannel);
    
    //begin timing reverse note play time
    void reverseNoteLengthTimerOn(int midiNoteNumber, float noteLength);
    
    //called with every audio vector
    void processBlock(int numSamples, int midiChannel);
    void incrementTimers(int numSamples);
    
    //data retrieval callbacks
    int getNoteLengthTimer(int midiNoteNumber) const noexcept;
    float getVelocity(int midiNoteNumber) const noexcept;
    int getReverseNoteLengthTimer(int midiNoteNumber) const noexcept;
      
private:

    BKSynthesiser *synth;
    NostalgicPreparation::Ptr preparation;
    double sampleRate;

    Array<int> noteLengthTimers; //store current length of played notes here
    Array<int> activeNotes; //table of notes currently being played by player
    Array<float> velocities; //table of velocities played
    
    Array<int> reverseLengthTimers; //keep track of how long reverse notes have been playing
    Array<int> activeReverseNotes; //table of active reverse notes
    Array<int> reverseTargetLength; //target reverse length (in samples)
    Array<float> undertowVelocities; //velocities stored for undertow
    
    /** primary user parameters
    int waveDistance; //ms; distance from beginning of sample to stop reverse playback and begin undertow
    int undertow;     //ms; length of time to play forward after directional change
    float transposition; //transposition, in half steps
    float gain; //gain multiplier
    float lengthMultiplier; //note-length mode: toscale reverse playback time
    float beatsToSkip; //synchronic mode: beats to skip before reverse peak
    
    NostalgicSyncMode syncMode = NostalgicSyncNoteLength;
     */
    
    //need callbacks to set/get these....
    Array<float> tuningOffsets = Array<float>(aPartialTuning,aNumScaleDegrees);
    int tuningBasePitch = 0;
    
    JUCE_LEAK_DETECTOR (NostalgicProcessor) //is this the right one to use here?
};



#endif  // NOSTALGIC_H_INCLUDED
