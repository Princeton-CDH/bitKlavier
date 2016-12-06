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
    typedef ReferenceCountedObjectPtr<DirectPreparation>     Ptr;
    typedef Array<DirectPreparation::Ptr, CriticalSection>   CSArr;
    typedef Array<DirectPreparation::Ptr>                    Arr;
    
    DirectPreparation(float transp,
                         float gain,
                         bool overlay):
    dTransposition(transp),
    dGain(gain),
    dOverlay(overlay)
    {
        
    }
    
    DirectPreparation():
    dTransposition(0.0),
    dGain(1.0),
    dOverlay(false)
    {
        
    }
    
    ~DirectPreparation()
    {
        
    }
    
    
    const float getTransposition() const noexcept                   {return dTransposition; }
    const float getGain() const noexcept                            {return dGain;          }
    const bool getOverlay() const noexcept                          {return dOverlay;       }
    
    void setTransposition(float val)                                {dTransposition = val;  }
    void setGain(float val)                                         {dGain = val;           }
    void setOverlay(bool val)                                       {dOverlay = val;        }
    
    void print(void)
    {
        DBG("dTransposition: "  + String(dTransposition));
        DBG("dGain: "           + String(dGain));
        DBG("dOverlay: "        + String((int)dOverlay));
    }
private:
    
    float   dTransposition;       //transposition, in half steps
    float   dGain;                //gain multiplier
    bool    dOverlay;

    
    JUCE_LEAK_DETECTOR(DirectPreparation);
};


class DirectProcessor : public ReferenceCountedObject
{
    
public:
    typedef ReferenceCountedObjectPtr<DirectProcessor>      Ptr;
    typedef Array<DirectProcessor::Ptr, CriticalSection>    CSArr;
    typedef Array<DirectProcessor::Ptr>                     Arr;
    
    DirectProcessor(
                        BKSynthesiser *s,
                        Keymap::Ptr km,
                        DirectPreparation::Ptr prep,
                        int layer);
    
    ~DirectProcessor();

    void keyPressed(int noteNumber, float velocity);
    
    void keyReleased(int noteNumber, int channel);
    
    int layer;
    
    Keymap::Ptr getKeymap(void)
    {
        return keymap;
    }
    
    DirectPreparation::Ptr getPreparation(void)
    {
        return preparation;
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
