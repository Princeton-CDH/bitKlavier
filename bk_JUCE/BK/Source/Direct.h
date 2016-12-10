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
                      bool overlay):
    Id(Id),
    dTransposition(transp),
    dGain(gain),
    dOverlay(overlay)
    {
        
    }
    
    DirectPreparation(int Id):
    Id(Id),
    dTransposition(0.0),
    dGain(1.0),
    dOverlay(false)
    {
        
    }
    
    ~DirectPreparation()
    {
        
    }
    
    
    inline const float getTransposition() const noexcept                   {return dTransposition; }
    inline const float getGain() const noexcept                            {return dGain;          }
    inline const bool getOverlay() const noexcept                          {return dOverlay;       }
    
    inline void setTransposition(float val)                                {dTransposition = val;  }
    inline void setGain(float val)                                         {dGain = val;           }
    inline void setOverlay(bool val)                                       {dOverlay = val;        }
    
    inline int getId(void) {   return Id; }
    
    void print(void)
    {
        DBG("dTransposition: "  + String(dTransposition));
        DBG("dGain: "           + String(dGain));
        DBG("dOverlay: "        + String((int)dOverlay));
    }
private:
    int Id;
    float   dTransposition;       //transposition, in half steps
    float   dGain;                //gain multiplier
    bool    dOverlay;
    
    JUCE_LEAK_DETECTOR(DirectPreparation);
};


class DirectProcessor : public ReferenceCountedObject
{
    
public:
    typedef ReferenceCountedObjectPtr<DirectProcessor>   Ptr;
    typedef Array<DirectProcessor::Ptr>                  Arr;
    typedef Array<DirectProcessor::Ptr, CriticalSection> CSArr;
    
    DirectProcessor(
                        BKSynthesiser *s,
                        Keymap::Ptr km,
                        DirectPreparation::Ptr prep,
                        int layer);
    
    ~DirectProcessor();

    void keyPressed(int noteNumber, float velocity);
    
    void keyReleased(int noteNumber, int channel);
    
    int layer;
    
    inline void setKeymap(Keymap::Ptr km)
    {
        keymap = km;
    }
    
    inline void setPreparation(DirectPreparation::Ptr prep)
    {
        preparation = prep;
    }
    
    /*
    inline void setTuning(TuningProcessor::Ptr t)
    {
        tuner = t;
    }
    */
    Keymap::Ptr getKeymap(void)
    {
        return keymap;
    }
    
    DirectPreparation::Ptr getPreparation(void)
    {
        return preparation;
    }
    
    int getKeymapId(void)
    {
        return keymap->getId();
    }
    
    int getPreparationId(void)
    {
        return preparation->getId();
    }
    
    float getTimeToBeatMS(float beatsToSkip);
    
private:
    
    BKSynthesiser*              synth;
    Keymap::Ptr                 keymap;
    DirectPreparation::Ptr      preparation;
    
    double sampleRate;
    
    JUCE_LEAK_DETECTOR(DirectProcessor);
};



#endif  // DIRECT_H_INCLUDED
