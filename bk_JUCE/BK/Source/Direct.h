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
    dTransposition(p->getTransposition()),
    dGain(p->getGain()),
    dResonanceGain(p->getResonanceGain()),
    dHammerGain(p->getHammerGain()),
    tuning(p->getTuning())
    {
        
    }
    
    DirectPreparation(float transp,
                      float gain,
                      bool resAndHammer,
                      float resGain,
                      float hamGain,
                      Tuning::Ptr t):
    dTransposition(transp),
    dGain(gain),
    dResonanceGain(resGain),
    dHammerGain(hamGain),
    tuning(t)
    {
        
    }
    
    DirectPreparation(Tuning::Ptr t):
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
    
    inline void copy(DirectPreparation::Ptr d)
    {
        dTransposition = d->getTransposition();
        dGain = d->getGain();
        dResonanceGain = d->getResonanceGain();
        dHammerGain = d->getHammerGain();
        tuning = d->getTuning();
    }
    
    
    inline const float getTransposition() const noexcept                {return dTransposition; }
    inline const float getGain() const noexcept                         {return dGain;          }
    inline const float getResonanceGain() const noexcept                {return dResonanceGain; }
    inline const float getHammerGain() const noexcept                   {return dHammerGain;    }
    inline const Tuning::Ptr getTuning() const noexcept      {return tuning;         }
    
    inline void setTransposition(float val)                             {dTransposition = val;  }
    inline void setGain(float val)                                      {dGain = val;           }
    inline void setResonanceGain(float val)                             {dResonanceGain = val;  }
    inline void setHammerGain(float val)                                {dHammerGain = val;     }
    inline void setTuning(Tuning::Ptr t)                                {tuning = t;            }
    
    
    void print(void)
    {
        DBG("dTransposition: "  + String(dTransposition));
        DBG("dGain: "           + String(dGain));
        DBG("dResGain: "        + String(dResonanceGain));
        DBG("dHammerGain: "     + String(dHammerGain));
    }

private:
    
    float   dTransposition;       //transposition, in half steps
    float   dGain;                //gain multiplier
    float   dResonanceGain, dHammerGain;
    
    Tuning::Ptr tuning;
    
    JUCE_LEAK_DETECTOR(DirectPreparation);
};

class DirectModPreparation : public ReferenceCountedObject
{
public:
    
    typedef ReferenceCountedObjectPtr<DirectModPreparation>   Ptr;
    typedef Array<DirectModPreparation::Ptr>                  PtrArr;
    typedef Array<DirectModPreparation::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<DirectModPreparation>                  Arr;
    typedef OwnedArray<DirectModPreparation, CriticalSection> CSArr;
    
    /*
    DirectId = 0,
    DirectTuning,
    DirectTransposition,
    DirectGain,
    DirectResGain,
    DirectHammerGain,
    DirectParameterTypeNil,
     */
    
    DirectModPreparation(DirectPreparation::Ptr p)
    {
        param.ensureStorageAllocated(cDirectParameterTypes.size());
        
        param.set(DirectTuning, String(p->getTuning()->getId()));
        param.set(DirectTransposition, String(p->getTransposition()));
        param.set(DirectGain, String(p->getGain()));
        param.set(DirectResGain, String(p->getResonanceGain()));
        param.set(DirectHammerGain, String(p->getHammerGain()));
        
    }
    
    
    DirectModPreparation(void)
    {
        param.set(DirectTuning, "");
        param.set(DirectTransposition, "");
        param.set(DirectGain, "");
        param.set(DirectResGain, "");
        param.set(DirectHammerGain, "");
    }
           
    
    ~DirectModPreparation(void)
    {
        
    }
    
    inline void copy(DirectPreparation::Ptr d)
    {
        param.set(DirectTuning, String(d->getTuning()->getId()));
        param.set(DirectTransposition, String(d->getTransposition()));
        param.set(DirectGain, String(d->getGain()));
        param.set(DirectResGain, String(d->getResonanceGain()));
        param.set(DirectHammerGain, String(d->getHammerGain()));
    }
    
    
    inline const String getParam(DirectParameterType type)
    {
        if (type != DirectId)   return param[type];
        else                    return "";
    }
    
    inline void setParam(DirectParameterType type, String val) { param.set(type, val);}
    
    void print(void)
    {

    }
    
private:
    StringArray          param;
    
    JUCE_LEAK_DETECTOR(DirectModPreparation);
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
    
    DirectPreparation::Ptr  getPreparation(void) const                  { return preparation;      }
    
    void    keyPressed(int noteNumber, float velocity, int channel);
    void    keyReleased(int noteNumber, float velocity, int channel);
    
private:
    int Id;
    BKSynthesiser*              synth;
    BKSynthesiser*              resonanceSynth;
    BKSynthesiser*              hammerSynth;
    DirectPreparation::Ptr      preparation, active;
    TuningProcessor::Ptr        tuner;
    
    double sampleRate;
    
    JUCE_LEAK_DETECTOR(DirectProcessor);
};



#endif  // DIRECT_H_INCLUDED
