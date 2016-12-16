/*
  ==============================================================================

    Direct.h
    Created: 6 Dec 2016 12:46:37pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef DIRECT_H_INCLUDED
#define DIRECT_H_INCLUDED

#include "BKUtilities.h"

#include "BKSynthesiser.h"

#include "Keymap.h"

#include "Tuning.h"

class DirectPreparation : public ReferenceCountedObject
{
public:
    
    typedef ReferenceCountedObjectPtr<DirectPreparation>   Ptr;
    typedef Array<DirectPreparation::Ptr>                  Arr;
    typedef Array<DirectPreparation::Ptr, CriticalSection> CSArr;
    
    DirectPreparation(int Id,
                      float transp,
                      float gain,
                      bool resAndHammer,
                      float resGain,
                      float hamGain,
                      TuningPreparation::Ptr t):
    Id(Id),
    dTransposition(transp),
    dGain(gain),
    dResonanceGain(resGain),
    dHammerGain(hamGain),
    tuning(t)
    {
        
    }
    
    DirectPreparation(int Id, TuningPreparation::Ptr t):
    Id(Id),
    dTransposition(0.0),
    dGain(1.0),
    dResonanceGain(1.0),
    dHammerGain(1.0),
    tuning(t)
    {
        
    }
    
    ~DirectPreparation()
    {
        
    }
    
    
    inline const float getTransposition() const noexcept                {return dTransposition; }
    inline const float getGain() const noexcept                         {return dGain;          }
    inline const float getResonanceGain() const noexcept                {return dResonanceGain; }
    inline const float getHammerGain() const noexcept                   {return dHammerGain;    }
    inline const TuningPreparation::Ptr getTuning() const noexcept      {return tuning;         }
    
    inline void setTransposition(float val)                             {dTransposition = val;  }
    inline void setGain(float val)                                      {dGain = val;           }
    inline void setResonanceGain(float val)                             {dResonanceGain = val;  }
    inline void setHammerGain(float val)                                {dHammerGain = val;     }
    inline void setTuning(TuningPreparation::Ptr t)                     {tuning = t;            }
    
    inline int getId(void) {   return Id; }
    
    void print(void)
    {
        DBG("dTransposition: "  + String(dTransposition));
        DBG("dGain: "           + String(dGain));
        DBG("dResGain: "        + String(dResonanceGain));
        DBG("dHammerGain: "     + String(dHammerGain));
    }

private:
    
    int Id;
    
    float   dTransposition;       //transposition, in half steps
    float   dGain;                //gain multiplier
    float   dResonanceGain, dHammerGain;
    
    TuningPreparation::Ptr tuning;
    
    JUCE_LEAK_DETECTOR(DirectPreparation);
};


class DirectProcessor : public ReferenceCountedObject
{
    
public:
    typedef ReferenceCountedObjectPtr<DirectProcessor>   Ptr;
    typedef Array<DirectProcessor::Ptr>                  Arr;
    typedef Array<DirectProcessor::Ptr, CriticalSection> CSArr;
    
    DirectProcessor(BKSynthesiser *s,
                    BKSynthesiser *res,
                    BKSynthesiser *ham,
                    Keymap::Ptr km,
                    DirectPreparation::Ptr prep,
                    int Id);
    
    ~DirectProcessor();
    
    void processBlock(int numSamples, int midiChannel);
    
    void setCurrentPlaybackSampleRate(double sr);
    
    inline void             setKeymap(Keymap::Ptr km)                   { keymap = km;                      }
    inline void             setPreparation(DirectPreparation::Ptr prep) { preparation = prep;               }
    
    int                     getId(void)                                 { return Id;                        } 
    Keymap::Ptr             getKeymap(void) const                       { return keymap;                    }
    DirectPreparation::Ptr  getPreparation(void) const                  { return preparation;               }
    
    int                     getKeymapId(void) const                     { return keymap->getId();           }
    int                     getPreparationId(void) const                { return preparation->getId();      }
    
    void    keyPressed(int noteNumber, float velocity, int channel);
    void    keyReleased(int noteNumber, float velocity, int channel);
    
private:
    
    int                         Id;
    BKSynthesiser*              synth;
    BKSynthesiser*              resonanceSynth;
    BKSynthesiser*              hammerSynth;
    Keymap::Ptr                 keymap;
    DirectPreparation::Ptr      preparation;
    TuningProcessor             tuner;
    
    double sampleRate;
    
    JUCE_LEAK_DETECTOR(DirectProcessor);
};



#endif  // DIRECT_H_INCLUDED
