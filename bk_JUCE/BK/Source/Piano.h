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
    typedef Array<Piano::Ptr>                   PtrArr;
    typedef Array<Piano::Ptr, CriticalSection>  CSPtrArr;
    typedef OwnedArray<Piano>                        Arr;
    typedef OwnedArray<Piano, CriticalSection>       CSArr;
    
    Piano(BKSynthesiser *s,
          BKSynthesiser *res,
          BKSynthesiser *ham,
          Keymap::Ptr keymap,
          int pianoNum);
    ~Piano();
    
    void prepareToPlay (double sampleRate);
    
    inline void setPianoId(int val)         { pianoId = val; print();   }
    inline int getPianoId(void)             { return pianoId;           }
    
    void processBlock(int numSamples, int midiChannel);
    
    void keyPressed(int noteNumber, float velocity, int channel);
    void keyReleased(int noteNumber, float velocity, int channel);
    
    void setKeymap(Keymap::Ptr km);
    inline Keymap::Ptr getKeymap()              { return pKeymap; }
    inline int getKeymapId()                    { if (pKeymap) return pKeymap->getId(); else return 0; }
    
    inline String getPreparationIds()
    {
        String prep = "";
        for (auto p : sPreparations)
        {
            prep.append("S",1);
            prep.append(String(p->getId()), 3);
            prep.append(" ",1);
        }
        
        for (auto p : nPreparations)
        {
            prep.append("N",1);
            prep.append(String(p->getId()), 3);
            prep.append(" ",1);
        }
        
        for (auto p : dPreparations)
        {
            prep.append("D",1);
            prep.append(String(p->getId()), 3);
            prep.append(" ",1);
        }
        return prep;
    }
    
    void setSynchronicPreparations(SynchronicPreparation::PtrArr sPrep);
    void setNostalgicPreparations(NostalgicPreparation::PtrArr nPrep);
    void setDirectPreparations(DirectPreparation::PtrArr dPrep);
    
    void addSynchronic(SynchronicPreparation::Ptr sp);
    void addNostalgic(NostalgicPreparation::Ptr np);
    void addDirect(DirectPreparation::Ptr dp);
    
    void removeSynchronic(SynchronicPreparation::Ptr sp);
    void removeNostalgic(NostalgicPreparation::Ptr np);
    void removeDirect(DirectPreparation::Ptr dp);

    void deactivateIfNecessary();
    
    void removeAllPreparations();
    
    
    bool isActive;
    
    void print(void)
    {
        DBG("pianoNum: " + String(pianoId));
    }
    
private:
    int pianoId;
    
    BKPreparationType prepType; //temporary, until we allow multiple types of prep per piano
    
    // Keymap for this Piano (one per piano)
    Keymap::Ptr                     pKeymap;
    
    // Preparations (flown in from BKAudioProcessor)
    SynchronicPreparation::PtrArr      sPreparations;
    NostalgicPreparation::PtrArr       nPreparations;
    DirectPreparation::PtrArr          dPreparations;
    
    // Processors
    SynchronicProcessor::CSPtrArr        sProcessor;
    NostalgicProcessor::CSPtrArr         nProcessor;
    DirectProcessor::CSPtrArr            dProcessor;
    
    // Pointers to synths
    BKSynthesiser*                  synth;
    BKSynthesiser*                  resonanceSynth;
    BKSynthesiser*                  hammerSynth;
    
    double                          sampleRate;
    
    
    JUCE_LEAK_DETECTOR(Piano)
};


#endif  // PIANO_H_INCLUDED
