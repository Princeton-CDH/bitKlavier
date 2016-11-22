/*
  ==============================================================================

    Preparations.h
    Created: 28 Oct 2016 10:42:48am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef PREPARATIONS_H_INCLUDED
#define PREPARATIONS_H_INCLUDED

#include "AudioConstants.h"

#include "BKUtilities.h"

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


#endif  // PREPARATIONS_H_INCLUDED
