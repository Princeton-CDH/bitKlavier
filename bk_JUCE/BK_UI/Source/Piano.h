/*
  ==============================================================================

    Piano.h
    Created: 7 Dec 2016 10:25:40am
    Author:  Michael R Mulshine
 
    A "Piano" is an array of PreparationsMaps
    (equivalent to a "Preset" in the original bitKlavier)

  ==============================================================================
*/

#ifndef Piano_H_INCLUDED
#define Piano_H_INCLUDED

#include "BKUtilities.h"

#include "PreparationMap.h"
#include "Modifications.h"

#include "Keymap.h"

#include "Preparation.h"


class Piano : public ReferenceCountedObject
{
public:
    typedef ReferenceCountedObjectPtr<Piano>   Ptr;
    typedef Array<Piano::Ptr>                  PtrArr;
    typedef Array<Piano::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<Piano>                  Arr;
    typedef OwnedArray<Piano, CriticalSection> CSArr;
    
    Piano(Synchronic::PtrArr synchronic,
          Nostalgic::PtrArr nostalgic,
          Direct::PtrArr direct,
          Tuning::PtrArr tuning,
          Tempo::PtrArr tempo,
          SynchronicModPreparation::PtrArr mSynchronic,
          NostalgicModPreparation::PtrArr mNostalgic,
          DirectModPreparation::PtrArr mDirect,
          TuningModPreparation::PtrArr mTuning,
          TempoModPreparation::PtrArr mTempo,
          Keymap::PtrArr keymap,
          int Id);
    ~Piano();
    
    void storeCurrentPiano();
    
    void recallCurrentPiano();


    ValueTree*  getPianoValueTree(void);
    
    inline void setId(int Id) { Id = Id; }
    
    inline int getId(void) { return Id; }
    
    inline PreparationMap::CSPtrArr getPreparationMaps(void) { return prepMaps; }
    
    inline const String getName() const noexcept {return pianoName;}
    inline void setName(String n){pianoName = n;}
    
    PreparationMap::Ptr         currentPMap;
    PreparationMap::CSPtrArr    activePMaps;
    PreparationMap::CSPtrArr    prepMaps;
    
    
    Array<int>                  pianoMap;
    int                         numPMaps;
    
    OwnedArray<Modifications> modMap;
    OwnedArray<ModificationMap> modificationMaps;
    
    ValueTree getState(void);
    
    void setState(XmlElement* e);

    
    inline Array<int> getAllIds(Synchronic::PtrArr direct)
    {
        Array<int> which;
        for (auto p : direct)
        {
            which.add(p->getId());
        }
        
        return which;
    }
    
    String modificationMapsToString(void)
    {
        String out = "";
        for (int i = 0; i < 128; i++)
        {
            String ptype = "";
            for (auto mpm : modificationMaps[i]->getModPrepMaps())
            {
                if (mpm->getType() == PreparationTypeDirect) ptype = "d";
                else if (mpm->getType() == PreparationTypeNostalgic) ptype = "n";
                else if (mpm->getType() == PreparationTypeSynchronic) ptype = "s";
                else if (mpm->getType() == PreparationTypeTuning) ptype = "t";
                else if (mpm->getType() == PreparationTypeTempo) ptype = "m";
                
                out += String(i) + ":" + ptype + String(mpm->getId()) + ":" + "{" + intArrayToString(mpm->getPreparations()) +"} ";
                
            }
        }
        
        return out;
    }

    void clearModMap(void)
    {
        for (int i = 0; i<modMap.size(); i++)
        {
            modMap[i]->clearModifications();
        }
    }

    void clearResetMap(void)
    {
        for (int i = 0; i<modMap.size(); i++)
        {
            modMap[i]->clearResets();
        }
    }

    int                         numModSMaps, numModNMaps, numModDMaps;

    void                        prepareToPlay(double sampleRate);
    
    
    void deconfigureDirectModification(DirectModPreparation::Ptr);
    void configureDirectModification(int key, DirectModPreparation::Ptr, Array<int>);
    void configureDirectModification(DirectModPreparation::Ptr);
    
    void deconfigureSynchronicModification(SynchronicModPreparation::Ptr);
    void configureSynchronicModification(int key, SynchronicModPreparation::Ptr, Array<int>);
    void configureSynchronicModification(SynchronicModPreparation::Ptr);
    
    void deconfigureNostalgicModification(NostalgicModPreparation::Ptr);
    void configureNostalgicModification(int key, NostalgicModPreparation::Ptr, Array<int>);
    void configureNostalgicModification(NostalgicModPreparation::Ptr dmod);
    
    void deconfigureTempoModification(TempoModPreparation::Ptr);
    void configureTempoModification(int key, TempoModPreparation::Ptr, Array<int>);
    void configureTempoModification(TempoModPreparation::Ptr);
    
    void deconfigureTuningModification(TuningModPreparation::Ptr);
    void configureTuningModification(int key, TuningModPreparation::Ptr, Array<int>);
    void configureTuningModification(TuningModPreparation::Ptr dmod);
    
    int                         addPreparationMap(void);
    int                         addPreparationMap(Keymap::Ptr keymap);
    PreparationMap::Ptr         getPreparationMapWithKeymap(Keymap::Ptr);
    int                         removeLastPreparationMap(void);

private:
    int Id;
    String pianoName;

    double sampleRate;

    // Pointers to synths (flown in from BKAudioProcessor)
    BKSynthesiser*                      synth;
    BKSynthesiser*                      resonanceSynth;
    BKSynthesiser*                      hammerSynth;
    
    Synchronic::PtrArr  synchronic;
    Nostalgic::PtrArr   nostalgic;
    Direct::PtrArr      direct;
    Tuning::PtrArr      tuning;
    Tempo::PtrArr       tempo;
    
    SynchronicModPreparation::PtrArr    modSynchronic;
    NostalgicModPreparation::PtrArr     modNostalgic;
    DirectModPreparation::PtrArr        modDirect;
    TuningModPreparation::PtrArr        modTuning;
    TempoModPreparation::PtrArr         modTempo;
    
    
    Keymap::PtrArr                      bkKeymaps;
    
    inline Array<int> getAllIds(Direct::PtrArr direct)
    {
        Array<int> which;
        for (auto p : direct)
        {
            which.add(p->getId());
        }
        
        return which;
    }
    
    inline Array<int> getAllIds(Tempo::PtrArr direct)
    {
        Array<int> which;
        for (auto p : direct)
        {
            which.add(p->getId());
        }
        
        return which;
    }
    
    inline Array<int> getAllIds(Tuning::PtrArr direct)
    {
        Array<int> which;
        for (auto p : direct)
        {
            which.add(p->getId());
        }
        
        return which;
    }
    
    inline Array<int> getAllIds(Nostalgic::PtrArr direct)
    {
        Array<int> which;
        for (auto p : direct)
        {
            which.add(p->getId());
        }
        
        return which;
    }
    
    
    JUCE_LEAK_DETECTOR(Piano)
};


#endif  // Piano_H_INCLUDED
