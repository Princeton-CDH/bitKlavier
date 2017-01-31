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
    typedef Array<DirectPreparation::Ptr>                  PtrArr;
    typedef Array<DirectPreparation::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<DirectPreparation>                  Arr;
    typedef OwnedArray<DirectPreparation, CriticalSection> CSArr;
    
    DirectPreparation(DirectPreparation::Ptr p):
    Id(p->getId()),
    dTransposition(p->getTransposition()),
    dGain(p->getGain()),
    dResonanceGain(p->getResonanceGain()),
    dHammerGain(p->getHammerGain()),
    tuning(p->getTuning())
    {
        
    }
    
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
    typedef Array<DirectProcessor::Ptr>                  PtrArr;
    typedef Array<DirectProcessor::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<DirectProcessor>                  Arr;
    typedef OwnedArray<DirectProcessor, CriticalSection> CSArr;
    
    DirectProcessor(BKSynthesiser *s,
                    BKSynthesiser *res,
                    BKSynthesiser *ham,
                    DirectPreparation::Ptr prep,
                    DirectPreparation::Ptr active,
                    int Id);
    
    ~DirectProcessor();
    
    void processBlock(int numSamples, int midiChannel);
    
    void setCurrentPlaybackSampleRate(double sr);
    
    inline void             setPreparation(DirectPreparation::Ptr prep) { preparation = prep;               }
    
    int                     getPreparationId(void) const                { return preparation->getId();      }
    DirectPreparation::Ptr  getPreparation(void) const                  { return preparation;      }
    
    void    keyPressed(int noteNumber, float velocity, int channel);
    void    keyReleased(int noteNumber, float velocity, int channel);
    
    inline int getId(void){return Id;}
    
private:
    int Id;
    BKSynthesiser*              synth;
    BKSynthesiser*              resonanceSynth;
    BKSynthesiser*              hammerSynth;
    DirectPreparation::Ptr      preparation, active;
    TuningProcessor             tuner;
    
    double sampleRate;
    
    JUCE_LEAK_DETECTOR(DirectProcessor);
};



#endif  // DIRECT_H_INCLUDED
