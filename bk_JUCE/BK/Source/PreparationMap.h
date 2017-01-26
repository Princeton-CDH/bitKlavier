/*
  ==============================================================================

    PreparationMap.h
    Created: 8 Dec 2016 12:59:46am
    Author:  Michael R Mulshine
 
    A "PreparationsMap" is a Keymap connected to one or more Preparations of any type

  ==============================================================================
*/

#ifndef PreparationMap_H_INCLUDED
#define PreparationMap_H_INCLUDED

#include "BKUtilities.h"

#include "Keymap.h"

#include "Tuning.h"

#include "Synchronic.h"

#include "Nostalgic.h"

#include "Direct.h"



class PreparationMap : public ReferenceCountedObject
{
public:
    typedef ReferenceCountedObjectPtr<PreparationMap>    Ptr;
    typedef Array<PreparationMap::Ptr>                   PtrArr;
    typedef Array<PreparationMap::Ptr, CriticalSection>  CSPtrArr;
    typedef OwnedArray<PreparationMap>                   Arr;
    typedef OwnedArray<PreparationMap, CriticalSection>  CSArr;
    
    PreparationMap(BKSynthesiser *s,
                   BKSynthesiser *res,
                   BKSynthesiser *ham,
                   Keymap::Ptr keymap,
                   int Id);
    ~PreparationMap();
    
    void prepareToPlay (double sampleRate);
    
    inline void setId(int val)         { Id = val; print();   }
    inline int getId(void)             { return Id;           }
    
    void processBlock(int numSamples, int midiChannel);
    
    void keyPressed(int noteNumber, float velocity, int channel);
    void keyReleased(int noteNumber, float velocity, int channel);
    void postRelease(int noteNumber, float velocity, int channel);
    
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
        
        if (prep == "")
            return " ";
        
        return prep;
        
    }
    
    void setSynchronicPreparations(SynchronicPreparation::PtrArr sPrep);
    void setNostalgicPreparations(NostalgicPreparation::PtrArr nPrep);
    void setDirectPreparations(DirectPreparation::PtrArr dPrep);
    
    SynchronicPreparation::PtrArr getSynchronicPreparations(void);
    NostalgicPreparation::PtrArr getNostalgicPreparations(void);
    DirectPreparation::PtrArr getDirectPreparations(void);
    

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
        DBG("PrepMapId: " + String(Id));
    }
    
private:
    int Id;
    
    // Keymap for this PreparationMap (one per PreparationMap)
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
    
    
    JUCE_LEAK_DETECTOR(PreparationMap)
};


#endif  // PreparationMap_H_INCLUDED
