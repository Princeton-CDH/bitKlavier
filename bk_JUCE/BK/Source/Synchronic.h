/*
  ==============================================================================

    Synchronic.h
    Created: 22 Nov 2016 3:46:35pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef SYNCHRONIC_H_INCLUDED
#define SYNCHRONIC_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

#include "BKSynthesiser.h"

#include "BKUtilities.h"

#include "AudioConstants.h"


class SynchronicPreparation : public ReferenceCountedObject
{
public:
    typedef ReferenceCountedObjectPtr<SynchronicPreparation> Ptr;
    
    SynchronicPreparation(float tempo,
                          int numPulses,
                          int clusterMin,
                          int clusterMax,
                          float clusterThresh,
                          SynchronicSyncMode mode,
                          int beatsToSkip,
                          Array<float> beatMultipliers,
                          Array<float> accentMultipliers,
                          Array<float> lengthMultipliers,
                          Array<float> tuningOffsets,
                          int basePitch):
    sTempo(tempo),
    sNumPulses(numPulses),
    sClusterMin(clusterMin),
    sClusterMax(clusterMax),
    sClusterThresh(clusterThresh),
    sMode(mode),
    sBeatsToSkip(beatsToSkip),
    sBeatMultipliers(beatMultipliers),
    sAccentMultipliers(accentMultipliers),
    sLengthMultipliers(lengthMultipliers),
    sTuningOffsets(tuningOffsets),
    sBasePitch(basePitch)
    {
        
    }
    
    SynchronicPreparation()
    {
        sTempo = 120.0;
        sNumPulses = 0;
        sClusterMin = 1;
        sClusterMax = 100;
        sClusterThresh = 1.0;
        sMode = FirstNoteSync;
        sBeatsToSkip = 0;
        sAccentMultipliers = Array<float>({1.0});
        sBeatMultipliers = Array<float>({1.0});
        sLengthMultipliers = Array<float>({1.0});
        sTuningOffsets = Array<float>(aEqualTuning);
        sBasePitch = 0;
        
    }
    
    void print(void)
    {
        DBG("sTempo: " + String(sTempo));
        DBG("sNumPulses: " + String(sNumPulses));
        DBG("sClusterMin: " + String(sClusterMin));
        DBG("sClusterMax: " + String(sClusterMax));
        DBG("sClusterThresh: " + String(sClusterThresh));
        DBG("sMode: " + String(sMode));
        DBG("sBeatsToSkip: " + String(sBeatsToSkip));
        DBG("sBeatMultipliers: " + floatArrayToString(sBeatMultipliers));
        DBG("sLengthMultipliers: " + floatArrayToString(sLengthMultipliers));
        DBG("sAccentMultipliers: " + floatArrayToString(sAccentMultipliers));
        DBG("sTuningOffsets: " + floatArrayToString(sTuningOffsets));
        DBG("sBasePitch: " + String(sBasePitch));
    }
    
    const float getTempo() const noexcept                       {return sTempo;                 }
    const int getNumPulses() const noexcept                     {return sNumPulses;             }
    const int getClusterMin() const noexcept                    {return sClusterMin;            }
    const int getClusterMax() const noexcept                    {return sClusterMax;            }
    const float getClusterThresh() const noexcept               {return sClusterThresh;         }
    const SynchronicSyncMode getMode() const noexcept           {return sMode;                  }
    const int getBeatsToSkip() const noexcept                   {return sBeatsToSkip;           }
    const Array<float> getBeatMultipliers() const noexcept      {return sBeatMultipliers;       }
    const Array<float> getAccentMultipliers() const noexcept    {return sAccentMultipliers;     }
    const Array<float> getLengthMultipliers() const noexcept    {return sLengthMultipliers;     }
    const Array<float> getTuningOffsets() const noexcept        {return sTuningOffsets;         }
    const int getBasePitch() const noexcept                     {return sBasePitch;             }
    
    void setTempo(float tempo)                                  {sTempo = tempo;                                    }
    void setNumPulses(int numPulses)                            {sNumPulses = numPulses;                            }
    void setClusterMin(int clusterMin)                          {sClusterMin = clusterMin;                          }
    void setClusterMax(int clusterMax)                          {sClusterMax = clusterMax;                          }
    void setClusterThresh(float clusterThresh)                  {sClusterThresh = clusterThresh;                    }
    void setMode(SynchronicSyncMode mode)                       {sMode = mode;                                      }
    void setBeatsToSkip(int beatsToSkip)                        {sBeatsToSkip = beatsToSkip;                        }
    void setBeatMultipliers(Array<float> beatMultipliers)       {sBeatMultipliers.swapWith(beatMultipliers);        }
    void setAccentMultipliers(Array<float> accentMultipliers)   {sAccentMultipliers.swapWith(accentMultipliers);    }
    void setLengthMultipliers(Array<float> lengthMultipliers)   {sLengthMultipliers.swapWith(lengthMultipliers);    }
    void setTuningOffsets(Array<float> tuningOffsets)           {sTuningOffsets = tuningOffsets;                    }
    void setBasePitch(int basePitch)                            {sBasePitch = basePitch;                            }
    
private:
    float sTempo;
    int sNumPulses,sClusterMin,sClusterMax;
    float sClusterThresh;
    SynchronicSyncMode sMode;
    int sBeatsToSkip; // float?
    Array<float> sBeatMultipliers;
    Array<float> sAccentMultipliers;
    Array<float> sLengthMultipliers;
    Array<float> sTuningOffsets;
    int sBasePitch; // float?
    
    JUCE_LEAK_DETECTOR(SynchronicPreparation);
};

class SynchronicProcessor
{
    
public:
    SynchronicProcessor(BKSynthesiser *s, SynchronicPreparation::Ptr prep);
    ~SynchronicProcessor();
    
    void renderNextBlock(int channel, int numSamples);
    
    void notePlayed(int noteNumber, int velocity);
    
private:
    
    BKSynthesiser *synth;
    SynchronicPreparation::Ptr preparation;
    
    float clusterThreshold; // beats
    uint64 clusterThresholdSamples;
    uint64 clusterThresholdTimer;
    
    float pulseThreshold; //beats
    uint64 pulseThresholdSamples;
    uint64 pulseThresholdTimer;
    
    int pulse;
    int beat;
    int accent;
    int length;
    int clusterSize;
    
    void playNote(int channel, int note);
    
    double tempoPeriod;
    uint64 tempoPeriodSamples;
    
    bool inPulses, inCluster, inPrePulses;
    
    double sampleRate;
    
    
    
    uint64 firstNoteTimer;
    uint64 tempoPeriodTimer;
    uint64 lastNoteTimer;
    
    Array<float> tuningOffsets;
    int tuningBasePitch;
    
    uint64 clusterTimer;
    uint64 pulseTimer;
    uint64 phasor;
    uint64 numSamplesBeat;
    
    Array<int> cluster;
    Array<int> toAdd;
    Array<int> on;
    
    JUCE_LEAK_DETECTOR(SynchronicProcessor);
};

#endif  // SYNCHRONIC_H_INCLUDED
