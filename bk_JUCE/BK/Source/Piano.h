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
          Keymap::Ptr keymap,
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
    
    Synchronic::PtrArr  synchronic;
    Nostalgic::PtrArr   nostalgic;
    Direct::PtrArr      direct;
    Tuning::PtrArr      tuning;
    Tempo::PtrArr       tempo;
    
    Array<int>                  pianoMap;
    int                         numPMaps;
    
    OwnedArray<Modifications> modMap;
    OwnedArray<ModificationMap> modificationMaps;
    
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
    
    int                         addPreparationMap(void);
    int                         removeLastPreparationMap(void);
    
private:
    int Id;
    String pianoName;
    
    double sampleRate;

    // Pointers to synths (flown in from BKAudioProcessor)
    BKSynthesiser*                  synth;
    BKSynthesiser*                  resonanceSynth;
    BKSynthesiser*                  hammerSynth;
    Keymap::Ptr                     initialKeymap;
    
    ValueTree vt;

    
    JUCE_LEAK_DETECTOR(Piano)
};


#endif  // Piano_H_INCLUDED
