/*
  ==============================================================================

    PreparationsMap.h
    Created: 8 Dec 2016 12:59:46am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef PreparationsMap_H_INCLUDED
#define PreparationsMap_H_INCLUDED

#include "BKUtilities.h"

#include "Keymap.h"

#include "Tuning.h"

#include "Synchronic.h"

#include "Nostalgic.h"

#include "Direct.h"

class PreparationsMap : public ReferenceCountedObject
{
public:
    typedef ReferenceCountedObjectPtr<PreparationsMap>    Ptr;
    typedef Array<PreparationsMap::Ptr>                   PtrArr;
    typedef Array<PreparationsMap::Ptr, CriticalSection>  CSPtrArr;
    typedef OwnedArray<PreparationsMap>                   Arr;
    typedef OwnedArray<PreparationsMap, CriticalSection>  CSArr;
    
    PreparationsMap(BKSynthesiser *s,
          BKSynthesiser *res,
          BKSynthesiser *ham,
          Keymap::Ptr keymap,
          int PreparationsMapNum);
    ~PreparationsMap();
    
    void prepareToPlay (double sampleRate);
    
    inline void setPreparationsMapId(int val)         { PreparationsMapId = val; print();   }
    inline int getPreparationsMapId(void)             { return PreparationsMapId;           }
    
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
        DBG("PreparationsMapNum: " + String(PreparationsMapId));
    }
    
private:
    int PreparationsMapId;
    
    // Keymap for this PreparationsMap (one per PreparationsMap)
    Keymap::Ptr                     pKeymap;
    
    // Preparations (flown in from BKAudioProcessor)
    SynchronicPreparation::PtrArr   sPreparations;
    NostalgicPreparation::PtrArr    nPreparations;
    DirectPreparation::PtrArr       dPreparations;
    
    // Processors (internal)
    SynchronicProcessor::CSPtrArr   sProcessor;
    NostalgicProcessor::CSPtrArr    nProcessor;
    DirectProcessor::CSPtrArr       dProcessor;
    
    // Pointers to synths (flown in from BKAudioProcessor)
    BKSynthesiser*                  synth;
    BKSynthesiser*                  resonanceSynth;
    BKSynthesiser*                  hammerSynth;
    
    double                          sampleRate;
    
    
    JUCE_LEAK_DETECTOR(PreparationsMap)
};


#endif  // PreparationsMap_H_INCLUDED
