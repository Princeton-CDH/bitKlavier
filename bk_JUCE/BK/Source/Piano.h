/*
  ==============================================================================

    Piano.h
    Created: 8 Dec 2016 12:59:46am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef PIANO_H_INCLUDED
#define PIANO_H_INCLUDED

#include "BKUtilities.h"

#include "Keymap.h"

#include "Tuning.h"

#include "Synchronic.h"

#include "Nostalgic.h"

#include "Direct.h"

class Piano : public ReferenceCountedObject
{
public:
    typedef ReferenceCountedObjectPtr<Piano>    Ptr;
    typedef Array<Piano::Ptr>                   Arr;
    typedef Array<Piano::Ptr, CriticalSection>  CSArr;
    
    Piano(int val);
    ~Piano();
    
    inline void setPianoNumber(int val)         { pianoNumber = val; print();   }
    inline int getPianoNumber(void)             { return pianoNumber;           }
    
    void processBlock(int numSamples, int midiChannel);
    void keyPressed(int noteNumber, float velocity, int channel);
    void keyReleased(int noteNumber, float velocity, int channel);
    
    void setKeymap(Keymap::Ptr km);
    inline Keymap::Ptr getKeymap()              { return pKeymap; }
    inline int getKeymapId()                    { return pKeymap->getId(); }
    
    inline void setType(BKPreparationType p)    { prepType = p; }
    inline BKPreparationType getType()          { return prepType; }
    
    void addSynchronic(SynchronicProcessor::Ptr sp)         { sProcessors.addIfNotAlreadyThere(sp); isActive = true;}
    void addNostalgic(NostalgicProcessor::Ptr np)           { nProcessors.addIfNotAlreadyThere(np); isActive = true;}
    void addDirect(DirectProcessor::Ptr dp)                 { dProcessors.addIfNotAlreadyThere(dp); isActive = true;}
    
    void removeSynchronic(SynchronicProcessor::Ptr sp)      { sProcessors.removeFirstMatchingValue(sp); }
    void removeNostalgic(NostalgicProcessor::Ptr np)        { nProcessors.removeFirstMatchingValue(np); }
    void removeDirect(DirectProcessor::Ptr dp)              { dProcessors.removeFirstMatchingValue(dp); }

    void removeAllPreparations();
    
    bool isActive;
    
    void print(void)
    {
        DBG("pianoNum: " + String(pianoNumber));
    }
    
private:
    
    int pianoNumber;
    BKPreparationType prepType; //temporary, until we allow multiple types of prep per piano
    
    // Keymap for this Piano (one per piano)
    Keymap::Ptr                     pKeymap;
    
    // Processors (with Preparations previously attached)
    Array<SynchronicProcessor::Ptr> sProcessors;
    Array<NostalgicProcessor::Ptr>  nProcessors;
    Array<DirectProcessor::Ptr>     dProcessors;

    
    JUCE_LEAK_DETECTOR(Piano)
};


#endif  // PIANO_H_INCLUDED
