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
    
    Piano(BKSynthesiser *s,
          BKSynthesiser *res,
          BKSynthesiser *ham,
          int pianoNum);
    ~Piano();
    
    void prepareToPlay (double sampleRate);
    
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
    
    void addSynchronic(SynchronicPreparation::Ptr sp);
    void addNostalgic(NostalgicPreparation::Ptr np);
    void addDirect(DirectPreparation::Ptr dp);
    
    void removeSynchronic(SynchronicPreparation::Ptr sp);
    void removeNostalgic(NostalgicPreparation::Ptr np);
    void removeDirect(DirectPreparation::Ptr dp);

    void deactivateIfNecessary();
    
    void removeAllPreparations();
    
    int pianoNumber;
    bool isActive;
    
    void print(void)
    {
        DBG("pianoNum: " + String(pianoNumber));
    }
    
private:
    
    BKPreparationType prepType; //temporary, until we allow multiple types of prep per piano
    
    // Keymap for this Piano (one per piano)
    Keymap::Ptr                     pKeymap;
    
    // Preparations (flown in from BKAudioProcessor)
    Array<SynchronicPreparation::Ptr> sPreparations;
    Array<NostalgicPreparation::Ptr>  nPreparations;
    Array<DirectPreparation::Ptr>     dPreparations;
    
    // Processors
    SynchronicProcessor::CSArr      sProcessor;
    NostalgicProcessor::Arr         nProcessor;
    DirectProcessor::Arr            dProcessor;
    
    // Pointers to synths
    BKSynthesiser*              synth;
    BKSynthesiser*              resonanceSynth;
    BKSynthesiser*              hammerSynth;
    double                      sampleRate;
    
    
    JUCE_LEAK_DETECTOR(Piano)
};


#endif  // PIANO_H_INCLUDED
