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
    
    DirectPreparation(DirectPreparation::Ptr p)
    {
        copy(p);
    }
    
    DirectPreparation(Array<float> transp,
                      float gain,
                      bool resAndHammer,
                      float resGain,
                      float hamGain):
    dTransposition(transp),
    dGain(gain),
    dResonanceGain(resGain),
    dHammerGain(hamGain)
    {
        
    }
    
    DirectPreparation(void):
    dTransposition(Array<float>({0.0})),
    dGain(1.0),
    dResonanceGain(1.0),
    dHammerGain(1.0)
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
    }
    
    inline bool compare(DirectPreparation::Ptr d)
    {
        return (dTransposition == d->getTransposition() &&
                dGain == d->getGain() &&
                dResonanceGain == d->getResonanceGain() &&
                dHammerGain == d->getHammerGain());
    }
    
    
    
    inline const String getName() const noexcept {return name;}
    inline void setName(String n){name = n;}
    
    inline Array<float> getTransposition() const noexcept         {return dTransposition; }
    inline const float getGain() const noexcept                         {return dGain;          }
    inline const float getResonanceGain() const noexcept                {return dResonanceGain; }
    inline const float getHammerGain() const noexcept                   {return dHammerGain;    }
    
    inline void setTransposition(Array<float> val)                      {dTransposition = val;  }
    inline void setGain(float val)                                      {dGain = val;           }
    inline void setResonanceGain(float val)                             {dResonanceGain = val;  }
    inline void setHammerGain(float val)                                {dHammerGain = val;     }
    
    
    void print(void)
    {
        DBG("dTransposition: "  + floatArrayToString(dTransposition));
        DBG("dGain: "           + String(dGain));
        DBG("dResGain: "        + String(dResonanceGain));
        DBG("dHammerGain: "     + String(dHammerGain));
    }
    
    

private:
    String  name;
    Array<float>   dTransposition;       //transposition, in half steps
    float   dGain;                //gain multiplier
    float   dResonanceGain, dHammerGain;
    
    //internal keymap for resetting internal values to static
    //Keymap::Ptr resetMap = new Keymap(0);
    
    JUCE_LEAK_DETECTOR(DirectPreparation);
};


/* ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ DIRECT ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ */

class Direct : public ReferenceCountedObject
{
    
public:
    typedef ReferenceCountedObjectPtr<Direct>   Ptr;
    typedef Array<Direct::Ptr>                  PtrArr;
    typedef Array<Direct::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<Direct>                  Arr;
    typedef OwnedArray<Direct, CriticalSection> CSArr;
    
    
    Direct(DirectPreparation::Ptr d,
           int Id):
    sPrep(new DirectPreparation(d)),
    aPrep(new DirectPreparation(sPrep)),
    Id(Id),
    name(String(Id))
    {
        
    }
    
    Direct(int Id):
    Id(Id),
    name(String(Id))
    {
        sPrep       = new DirectPreparation();
        aPrep       = new DirectPreparation(sPrep);
    };
    
    inline Direct::Ptr duplicate()
    {
        DirectPreparation::Ptr copyPrep = new DirectPreparation(sPrep);
        
        Direct::Ptr copy = new Direct(copyPrep, -1);
        
        copy->setName(name);
        
        return copy;
    }
    
    inline ValueTree getState(void)
    {
        ValueTree prep( vtagDirect);
        
        prep.setProperty( "Id",Id, 0);
        prep.setProperty( "name", name, 0);
        
        ValueTree transp( vtagDirect_transposition);
        Array<float> m = sPrep->getTransposition();
        int count = 0;
        for (auto f : m)    transp.setProperty( ptagFloat + String(count++), f, 0);
        prep.addChild(transp, -1, 0);
        
        prep.setProperty( ptagDirect_gain,              sPrep->getGain(), 0);
        prep.setProperty( ptagDirect_resGain,           sPrep->getResonanceGain(), 0);
        prep.setProperty( ptagDirect_hammerGain,        sPrep->getHammerGain(), 0);
        
        return prep;
    }
    
    inline void setState(XmlElement* e, Tuning::PtrArr tuning)
    {
        float f;
        
        Id = e->getStringAttribute("Id").getIntValue();
        
        String n = e->getStringAttribute("name");
        
        if (n != String::empty)     name = n;
        else                        name = String(Id);
        
        f = e->getStringAttribute(ptagDirect_gain).getFloatValue();
        sPrep->setGain(f);
        
        f = e->getStringAttribute(ptagDirect_hammerGain).getFloatValue();
        sPrep->setHammerGain(f);
        
        f = e->getStringAttribute(ptagDirect_resGain).getFloatValue();
        sPrep->setResonanceGain(f);
        
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
                
                sPrep->setTransposition(transp);
                
            }
        }
        // copy static to active
        aPrep->copy(sPrep);
    }
    
    ~Direct() {};
    
    inline int getId() {return Id;}
    inline void setId(int newId) { Id = newId;}
    
    DirectPreparation::Ptr      sPrep;
    DirectPreparation::Ptr      aPrep;
    
    inline void copy(Direct::Ptr from)
    {
        sPrep->copy(from->sPrep);
        aPrep->copy(sPrep);
    }
    
    inline String getName(void) const noexcept {return name;}
    inline void setName(String newName)
    {
        name = newName;
    }
    
private:
    int Id;
    String name;
    
    JUCE_LEAK_DETECTOR(Direct)
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
    
    DirectModPreparation(DirectPreparation::Ptr p, int Id):
    Id(Id)
    {
        param.ensureStorageAllocated((int)cDirectParameterTypes.size());
        
        param.set(DirectTransposition, floatArrayToString(p->getTransposition()));
        param.set(DirectGain, String(p->getGain()));
        param.set(DirectResGain, String(p->getResonanceGain()));
        param.set(DirectHammerGain, String(p->getHammerGain()));
    }
    
    
    DirectModPreparation(int Id):
    Id(Id)
    {
        param.add("");
        param.add("");
        param.add("");
        param.add("");
        //param.add("");
    }
    
    inline DirectModPreparation::Ptr duplicate(void)
    {
        DirectModPreparation::Ptr copyPrep = new DirectModPreparation(-1);
        
        copyPrep->copy(this);
        
        copyPrep->setName(getName());
        
        return copyPrep;
    }
    
    inline ValueTree getState(void)
    {
        ValueTree prep( vtagModDirect);
        
        prep.setProperty( "Id",Id, 0);
        
        String p = "";
        
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
        
        Id = e->getStringAttribute("Id").getIntValue();
        
        String p = e->getStringAttribute(ptagDirect_gain);
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
        param.set(DirectTransposition, floatArrayToString(d->getTransposition()));
        param.set(DirectGain, String(d->getGain()));
        param.set(DirectResGain, String(d->getResonanceGain()));
        param.set(DirectHammerGain, String(d->getHammerGain()));
    }
    
    inline void copy(DirectModPreparation::Ptr p)
    {
        for (int i = DirectId+1; i < DirectParameterTypeNil; i++)
        {
            param.set(i, p->getParam((DirectParameterType)i));
        }
    }
    
    void clearAll()
    {
        for (int i = DirectId+1; i < DirectParameterTypeNil; i++)
        {
            param.set(i, "");
        }
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
    
    inline void setId(int newId) { Id = newId; }
    inline int getId(void)const noexcept {return Id;}
    
    inline String getName(void) const noexcept {return name;}
    inline void setName(String newName) {name = newName;}
    
private:
    int Id;
    String name;
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
    
    DirectProcessor(Direct::Ptr direct,
                    TuningProcessor::Ptr tuning,
                    BKSynthesiser *s, BKSynthesiser *res, BKSynthesiser *ham);
    
    ~DirectProcessor();
    
    void processBlock(int numSamples, int midiChannel);
    
    
    void    keyPressed(int noteNumber, float velocity, int channel);
    void    keyReleased(int noteNumber, float velocity, int channel);
    
    inline void attachToSynthesiser(BKSynthesiser* main,BKSynthesiser* res, BKSynthesiser* ham)
    {
        
    }
    
    inline void prepareToPlay(double sr, BKSynthesiser* main, BKSynthesiser* res, BKSynthesiser* hammer)
    {
        sampleRate = sr;
        
        synth = main;
        resonanceSynth = res;
        hammerSynth = hammer;
    }
    
    inline void reset(void)
    {
        direct->aPrep->copy(direct->sPrep);
    }
    
    inline int getId(void) const noexcept { return direct->getId(); }
    
    inline void setTuning(TuningProcessor::Ptr tuning)
    {
        tuner = tuning;
    }
    
    inline TuningProcessor::Ptr getTuning(void)
    {
        return tuner;
    }
    
private:
    BKSynthesiser*      synth;
    BKSynthesiser*      resonanceSynth;
    BKSynthesiser*      hammerSynth;
    
    Direct::Ptr             direct;
    TuningProcessor::Ptr    tuner;
    
    //need to keep track of the actual notes played and their offsets when a particular key is pressed
    //so that they can all be turned off properly, even in the event of a preparation change
    //while the key is held
    Array<int>      keyPlayed[128];         //keep track of pitches played associated with particular key on keyboard
    Array<float>    keyPlayedOffset[128];   //and also the offsets
    
    double sampleRate;
    
    JUCE_LEAK_DETECTOR(DirectProcessor);
};


#endif  // DIRECT_H_INCLUDED
