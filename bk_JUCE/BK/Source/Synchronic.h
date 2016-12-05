/*
  ==============================================================================

    Synchronic.h
    Created: 22 Nov 2016 3:46:35pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef SYNCHRONIC_H_INCLUDED
#define SYNCHRONIC_H_INCLUDED

#include "BKUtilities.h"

#include "BKSynthesiser.h"

#include "Keymap.h"

#include "Tuning.h"

class SynchronicPreparation : public ReferenceCountedObject
{
public:
    typedef ReferenceCountedObjectPtr<SynchronicPreparation>    Ptr;
    typedef Array<SynchronicPreparation::Ptr, CriticalSection>  CSArr;
    typedef Array<SynchronicPreparation::Ptr>                   Arr;
    
    
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
                          Array<float> transpOffsets,
                          TuningSystem tuning,
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
    sTranspOffsets(transpOffsets),
    sTuning(tuning),
    sBasePitch(basePitch),
    sPulseThreshSec(60.0/sTempo),
    sClusterThreshSec(sPulseThreshSec * sClusterThresh)
    {
    }

    
    SynchronicPreparation():
    sTempo(120),
    sNumPulses(0),
    sClusterMin(1),
    sClusterMax(100),
    sClusterThresh(1.0),
    sMode(FirstNoteSync),
    sBeatsToSkip(0),
    sBeatMultipliers(Array<float>({1.0})),
    sAccentMultipliers(Array<float>({1.0})),
    sLengthMultipliers(Array<float>({1.0})),
    sTranspOffsets(Array<float>({0.0})),
    sTuning(PartialTuning),
    sBasePitch(0),
    sPulseThreshSec(60.0/sTempo),
    sClusterThreshSec(sPulseThreshSec * sClusterThresh)
    {
    }
    
    const float getTempo() const noexcept                       {return sTempo;                 }
    const int getNumPulses() const noexcept                     {return sNumPulses;             }
    const int getClusterMin() const noexcept                    {return sClusterMin;            }
    const int getClusterMax() const noexcept                    {return sClusterMax;            }
    const float getClusterThresh() const noexcept               {return sClusterThreshSec;      }
    const float getPulseThresh() const noexcept              {return sPulseThreshSec;        }
    const float getClusterThreshold() const noexcept            {return sClusterThresh;         }
    const SynchronicSyncMode getMode() const noexcept           {return sMode;                  }
    const int getBeatsToSkip() const noexcept                   {return sBeatsToSkip;           }
    const Array<float> getBeatMultipliers() const noexcept      {return sBeatMultipliers;       }
    const Array<float> getAccentMultipliers() const noexcept    {return sAccentMultipliers;     }
    const Array<float> getLengthMultipliers() const noexcept    {return sLengthMultipliers;     }
    const Array<float> getTranspOffsets() const noexcept        {return sTranspOffsets;         }
    
    const TuningSystem getTuning() const noexcept               {return sTuning;             }
    const int getBasePitch() const noexcept                     {return sBasePitch;             }
    
    
    void setTempo(float tempo)
    {
        sTempo = tempo;
        
        float tempoPeriodS = (60.0/sTempo);
        
        sClusterThreshSec = tempoPeriodS * sClusterThresh;
        
        sPulseThreshSec = tempoPeriodS;
    }
    
    void setClusterThresh(float clusterThresh)
    {
        sClusterThresh = clusterThresh;
        sClusterThreshSec = (60.0/sTempo) * clusterThresh;
    }
    
    void setNumPulses(int numPulses)                            {sNumPulses = numPulses;                            }
    void setClusterMin(int clusterMin)                          {sClusterMin = clusterMin;                          }
    void setClusterMax(int clusterMax)                          {sClusterMax = clusterMax;                          }
    void setMode(SynchronicSyncMode mode)                       {sMode = mode;                                      }
    void setBeatsToSkip(int beatsToSkip)                        {sBeatsToSkip = beatsToSkip;                        }
    void setBeatMultipliers(Array<float> beatMultipliers)       {sBeatMultipliers.swapWith(beatMultipliers);        }
    void setAccentMultipliers(Array<float> accentMultipliers)   {sAccentMultipliers.swapWith(accentMultipliers);    }
    void setTranspOffsets(Array<float> transpOffsets)           {sTranspOffsets.swapWith(transpOffsets);            }
    void setLengthMultipliers(Array<float> lengthMultipliers)   {sLengthMultipliers.swapWith(lengthMultipliers);    }
    
    void setTuning(TuningSystem tuning)                         {sTuning = tuning;                                  }
    void setBasePitch(int basePitch)                            {sBasePitch = basePitch;                            }
    
    void print(void)
    {
        DBG("| - - - Synchronic Preparation - - - |");
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
        DBG("sTranspOffsets: " + floatArrayToString(sTranspOffsets));
        DBG("sTuning: " + String(sTuning));
        DBG("sBasePitch: " + String(sBasePitch));
        DBG("sPulseThreshSec: " + String(sPulseThreshSec));
        DBG("sClusterThreshSec: " + String(sClusterThreshSec));
        DBG("| - - - - - - - - -- - - - - - - - - |");
    }
    
private:
    float sTempo;
    int sNumPulses,sClusterMin,sClusterMax;
    float sClusterThresh; // beats
    SynchronicSyncMode sMode;
    int sBeatsToSkip; // float?
    Array<float> sBeatMultipliers;
    Array<float> sAccentMultipliers;
    Array<float> sLengthMultipliers;
    Array<float> sTranspOffsets;
    
    TuningSystem sTuning;
    int sBasePitch;
    
    float sPulseThreshSec;
    float sClusterThreshSec;
    
    
    
    
    
    JUCE_LEAK_DETECTOR(SynchronicPreparation);
};

class SynchronicProcessor : public ReferenceCountedObject
{
    
public:
    typedef ReferenceCountedObjectPtr<SynchronicProcessor>      Ptr;
    typedef Array<SynchronicProcessor::Ptr, CriticalSection>    CSArr;
    typedef Array<SynchronicProcessor::Ptr>                     Arr;
    
    SynchronicProcessor(BKSynthesiser *s, Keymap::Ptr km, SynchronicPreparation::Ptr prep, int layer, TuningProcessor *t);
    ~SynchronicProcessor();
    
    void processBlock(int numSamples, int channel);
    
    void keyPressed(int noteNumber, float velocity);
    
    void keyReleased(int noteNumber, int channel);
    
    const uint64 getCurrentNumSamplesBeat()     const noexcept { return numSamplesBeat;  }
    const uint64 getCurrentPhasor()             const noexcept { return phasor;          }
    
    int layer;
    
    Keymap::Ptr getKeymap(void)
    {
        return keymap;
    }
    
    SynchronicPreparation::Ptr getPreparation(void)
    {
        return preparation;
    }
    
    float getTimeToBeatMS(float beatsToSkip);
    
private:
    
    BKSynthesiser*              synth;
    Keymap::Ptr                 keymap;
    SynchronicPreparation::Ptr  preparation;
    TuningProcessor*            tuner;
    
    int pulse;
    
    int beat;
    int accent;
    int length;
    int transp;
    
    void playNote(int channel, int note);
    
    uint64 tempoPeriodSamples;
    
    bool inPulses, inCluster, inPrePulses;
    
    double sampleRate;
    
    uint64 clusterThresholdSamples;
    uint64 clusterThresholdTimer;
    
    uint64 pulseThresholdSamples;
    uint64 pulseThresholdTimer;
    
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
