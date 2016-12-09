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
    typedef ReferenceCountedObjectPtr<SynchronicPreparation>   Ptr;
    typedef Array<SynchronicPreparation::Ptr>                  Arr;
    typedef Array<SynchronicPreparation::Ptr, CriticalSection> CSArr;
    
    SynchronicPreparation(int Id,
                          float tempo,
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
                          PitchClass basePitch):
    Id(Id),
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

    
    SynchronicPreparation(int Id):
    Id(Id),
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
    sBasePitch(C),
    sPulseThreshSec(60.0/sTempo),
    sClusterThreshSec(sPulseThreshSec * sClusterThresh)
    {
    }
    
    inline const float getTempo() const noexcept                       {return sTempo;                 }
    inline const int getNumPulses() const noexcept                     {return sNumPulses;             }
    inline const int getClusterMin() const noexcept                    {return sClusterMin;            }
    inline const int getClusterMax() const noexcept                    {return sClusterMax;            }
    inline const float getClusterThresh() const noexcept               {return sClusterThreshSec;      }
    inline const float getPulseThresh() const noexcept              {return sPulseThreshSec;        }
    inline const float getClusterThreshold() const noexcept            {return sClusterThresh;         }
    inline const SynchronicSyncMode getMode() const noexcept           {return sMode;                  }
    inline const int getBeatsToSkip() const noexcept                   {return sBeatsToSkip;           }
    inline const Array<float> getBeatMultipliers() const noexcept      {return sBeatMultipliers;       }
    inline const Array<float> getAccentMultipliers() const noexcept    {return sAccentMultipliers;     }
    inline const Array<float> getLengthMultipliers() const noexcept    {return sLengthMultipliers;     }
    inline const Array<float> getTranspOffsets() const noexcept        {return sTranspOffsets;         }
    inline const TuningSystem getTuning() const noexcept               {return sTuning;             }
    inline const PitchClass getBasePitch() const noexcept              {return sBasePitch;             }
    
    inline int getId(void) {   return Id; }
    
    inline void setTempo(float tempo)
    {
        sTempo = tempo;
        
        float tempoPeriodS = (60.0/sTempo);
        
        sClusterThreshSec = tempoPeriodS * sClusterThresh;
        
        sPulseThreshSec = tempoPeriodS;
    }
    
    inline void setClusterThresh(float clusterThresh)
    {
        sClusterThresh = clusterThresh;
        sClusterThreshSec = (60.0/sTempo) * clusterThresh;
    }
    
    inline void setNumPulses(int numPulses)                            {sNumPulses = numPulses;                            }
    inline void setClusterMin(int clusterMin)                          {sClusterMin = clusterMin;                          }
    inline void setClusterMax(int clusterMax)                          {sClusterMax = clusterMax;                          }
    inline void setMode(SynchronicSyncMode mode)                       {sMode = mode;                                      }
    inline void setBeatsToSkip(int beatsToSkip)                        {sBeatsToSkip = beatsToSkip;                        }
    inline void setBeatMultipliers(Array<float> beatMultipliers)       {sBeatMultipliers.swapWith(beatMultipliers);        }
    inline void setAccentMultipliers(Array<float> accentMultipliers)   {sAccentMultipliers.swapWith(accentMultipliers);    }
    inline void setTranspOffsets(Array<float> transpOffsets)           {sTranspOffsets.swapWith(transpOffsets);            }
    inline void setLengthMultipliers(Array<float> lengthMultipliers)   {sLengthMultipliers.swapWith(lengthMultipliers);    }
    inline void setTuning(TuningSystem tuning)                         {sTuning = tuning;                                  }
    inline void setBasePitch(PitchClass basePitch)                     {sBasePitch = basePitch;                            }
    
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
    int Id;
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
    PitchClass sBasePitch;
    
    float sPulseThreshSec;
    float sClusterThreshSec;
    
    JUCE_LEAK_DETECTOR(SynchronicPreparation);
};

class SynchronicProcessor  : public ReferenceCountedObject
{
    
public:
    typedef ReferenceCountedObjectPtr<SynchronicProcessor>   Ptr;
    typedef Array<SynchronicProcessor::Ptr>                  Arr;
    typedef Array<SynchronicProcessor::Ptr, CriticalSection> CSArr;
    
    SynchronicProcessor(BKSynthesiser *synth, Keymap::Ptr km, SynchronicPreparation::Ptr prep, TuningProcessor::Ptr tuning, int Id);
    
    ~SynchronicProcessor();
    
    inline void setCurrentPlaybackSampleRate(double sr) { sampleRate = sr;}
    inline const uint64 getCurrentNumSamplesBeat(void)     const noexcept { return numSamplesBeat;  }
    inline const uint64 getCurrentPhasor(void)             const noexcept { return phasor;          }
    
    void processBlock(int numSamples, int channel);
    
    void keyPressed(int noteNumber, float velocity);
    
    void keyReleased(int noteNumber, int channel);
    
    inline void setKeymap(Keymap::Ptr km)
    {
        keymap = km;
    }
    
    inline void setPreparation(SynchronicPreparation::Ptr prep)
    {
        preparation = prep;
    }
    
    inline void setTuning(TuningProcessor::Ptr t)
    {
        tuner = t;
    }
    
    inline Keymap::Ptr getKeymap(void)
    {
        return keymap;
    }
    
    SynchronicPreparation::Ptr getPreparation(void)
    {
        return preparation;
    }
    
    float getTimeToBeatMS(float beatsToSkip);
    
private:
    int Id;
    BKSynthesiser*              synth;
    Keymap::Ptr                 keymap;
    SynchronicPreparation::Ptr  preparation;
    TuningProcessor::Ptr            tuner;
    
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
    PitchClass tuningBasePitch;
    
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
