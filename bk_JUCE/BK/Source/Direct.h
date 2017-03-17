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
    
    DirectPreparation(Array<float> transp,
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
    dTransposition(Array<float>({0.0})),
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
    
    inline bool compare(DirectPreparation::Ptr d)
    {
        return (dTransposition == d->getTransposition() &&
                dGain == d->getGain() &&
                dResonanceGain == d->getResonanceGain() &&
                dHammerGain == d->getHammerGain() &&
                tuning == d->getTuning());
    }
    
    
    
    inline const String getName() const noexcept {return name;}
    inline void setName(String n){name = n;}
    
    inline Array<float> getTransposition() const noexcept         {return dTransposition; }
    inline const float getGain() const noexcept                         {return dGain;          }
    inline const float getResonanceGain() const noexcept                {return dResonanceGain; }
    inline const float getHammerGain() const noexcept                   {return dHammerGain;    }
    inline const Tuning::Ptr getTuning() const noexcept                 {return tuning;         }
    //inline const Keymap::Ptr getResetMap() const noexcept               {return resetMap;       }
    
    inline void setTransposition(Array<float> val)                      {dTransposition = val;  }
    inline void setGain(float val)                                      {dGain = val;           }
    inline void setResonanceGain(float val)                             {dResonanceGain = val;  }
    inline void setHammerGain(float val)                                {dHammerGain = val;     }
    inline void setTuning(Tuning::Ptr t)                                {tuning = t;            }
    //inline void setResetMap(Keymap::Ptr k)                              {resetMap = k;          }
    
    
    void print(void)
    {
        DBG("dTransposition: "  + floatArrayToString(dTransposition));
        DBG("dGain: "           + String(dGain));
        DBG("dResGain: "        + String(dResonanceGain));
        DBG("dHammerGain: "     + String(dHammerGain));
        //DBG("resetKeymap: "     + intArrayToString(getResetMap()->keys()));
    }
    
    

private:
    String  name;
    Array<float>   dTransposition;       //transposition, in half steps
    float   dGain;                //gain multiplier
    float   dResonanceGain, dHammerGain;
    
    Tuning::Ptr tuning;
    
    //internal keymap for resetting internal values to static
    //Keymap::Ptr resetMap = new Keymap(0);
    
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
        param.set(DirectTransposition, floatArrayToString(p->getTransposition()));
        param.set(DirectGain, String(p->getGain()));
        param.set(DirectResGain, String(p->getResonanceGain()));
        param.set(DirectHammerGain, String(p->getHammerGain()));
    }
    
    
    DirectModPreparation(void)
    {
        param.add("");
        param.add("");
        param.add("");
        param.add("");
        param.add("");
        //param.add("");
    }
    
    inline ValueTree getState(int Id)
    {
        ValueTree prep( vtagModDirect+String(Id));
        
        String p = "";
        
        p = getParam(DirectTuning);
        if (p != String::empty) prep.setProperty( ptagDirect_tuning,            p.getIntValue(), 0);
        
        ValueTree transp( vtagDirect_transposition);
        int count = 0;
        p = getParam(DirectTransposition);
        if (p != String::empty)
        {
            Array<float> m = stringToFloatArray(p);
            for (auto f : m)
            {
                transp.      setProperty( ptagFloat + String(count++), f, 0);
            }
        }
        prep.addChild(transp, -1, 0);
        
        p = getParam(DirectGain);
        if (p != String::empty) prep.setProperty( ptagDirect_gain,              p.getFloatValue(), 0);
        
        p = getParam(DirectResGain);
        if (p != String::empty) prep.setProperty( ptagDirect_resGain,           p.getFloatValue(), 0);
        
        p = getParam(DirectHammerGain);
        if (p != String::empty) prep.setProperty( ptagDirect_hammerGain,        p.getFloatValue(), 0);
    
        return prep;
    }
    
    inline void setState(XmlElement* e)
    {
        float f;
        
        String p = e->getStringAttribute(ptagDirect_tuning);
        setParam(DirectTuning, p);
        
        p = e->getStringAttribute(ptagDirect_gain);
        setParam(DirectGain, p);
        
        p = e->getStringAttribute(ptagDirect_hammerGain);
        setParam(DirectHammerGain, p);
        
        p = e->getStringAttribute(ptagDirect_resGain);
        setParam(DirectResGain, p);
        
        forEachXmlChildElement (*e, sub)
        {
            if (sub->hasTagName(vtagDirect_transposition))
            {
                Array<float> transp;
                for (int k = 0; k < 128; k++)
                {
                    String attr = sub->getStringAttribute(ptagFloat + String(k));
                    
                    if (attr == String::empty) break;
                    else
                    {
                        f = attr.getFloatValue();
                        transp.add(f);
                    }
                }
                
                setParam(DirectTransposition, floatArrayToString(transp));
                
            }
        }
    }
    
    ~DirectModPreparation(void)
    {
        
    }
    
    inline void copy(DirectPreparation::Ptr d)
    {
        param.set(DirectTuning, String(d->getTuning()->getId()));
        param.set(DirectTransposition, floatArrayToString(d->getTransposition()));
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
    
    inline const StringArray getStringArray(void) { return param; }
    
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
                    DirectPreparation::Ptr active,
                    int Id);
    
    ~DirectProcessor();
    
    void processBlock(int numSamples, int midiChannel);
    
    void setCurrentPlaybackSampleRate(double sr);
    
    void    keyPressed(int noteNumber, float velocity, int channel);
    void    keyReleased(int noteNumber, float velocity, int channel);
    
private:
    int Id;
    BKSynthesiser*              synth;
    BKSynthesiser*              resonanceSynth;
    BKSynthesiser*              hammerSynth;
    DirectPreparation::Ptr      active;
    TuningProcessor::Ptr        tuner;
    
    //need to keep track of the actual notes played and their offsets when a particular key is pressed
    //so that they can all be turned off properly, even in the event of a preparation change
    //while the key is held
    Array<int>      keyPlayed[128];         //keep track of pitches played associated with particular key on keyboard
    Array<float>    keyPlayedOffset[128];   //and also the offsets
    
    double sampleRate;
    
    JUCE_LEAK_DETECTOR(DirectProcessor);
};



#endif  // DIRECT_H_INCLUDED
