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

#include "Reset.h"

#include "PianoConfig.h"

class Piano : public ReferenceCountedObject
{
public:
    typedef ReferenceCountedObjectPtr<Piano>   Ptr;
    typedef Array<Piano::Ptr>                  PtrArr;
    typedef Array<Piano::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<Piano>                  Arr;
    typedef OwnedArray<Piano, CriticalSection> CSArr;
    
    Piano(Synchronic::PtrArr* synchronic,
          Nostalgic::PtrArr* nostalgic,
          Direct::PtrArr* direct,
          Tuning::PtrArr* tuning,
          Tempo::PtrArr* tempo,
          SynchronicModPreparation::PtrArr* mSynchronic,
          NostalgicModPreparation::PtrArr* mNostalgic,
          DirectModPreparation::PtrArr* mDirect,
          TuningModPreparation::PtrArr* mTuning,
          TempoModPreparation::PtrArr* mTempo,
          Keymap::PtrArr* keymap,
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
    
    int numResetMappers;
    
    OwnedArray<Modifications> modificationMap;

    
    ModificationMapper::PtrArr mappers;
    ModificationMapper::PtrArr resetMappers;
    
    inline ModificationMapper::PtrArr getMappers(void) const noexcept { return mappers; }
    
    inline ModificationMapper::Ptr getMapper(int which) { return mappers[which]; }
    
    inline void clearMapper(void) { mappers.clear(); }
    
    inline ModificationMapper::Ptr getMapper(BKPreparationType type, int Id)
    {
        ModificationMapper::Ptr thisMapper = new ModificationMapper(type, Id);
        
        bool add = true;
        for (auto map : mappers)
        {
            if (map->getType() == type && map->getId() == Id)
            {
                thisMapper = map;
                add = false;
                break;
            }
        }
        
        if (add) mappers.add(thisMapper);
        
        return thisMapper;
    }
    
    inline void addMapper(ModificationMapper::Ptr thisMapper)
    {
        mappers.addIfNotAlreadyThere(thisMapper);
    }
    
    inline void removeMapper(ModificationMapper::Ptr thisMapper)
    {
        for (int i = mappers.size(); --i >= 0; )
        {
            if (mappers[i] == thisMapper) mappers.remove(i);
        }
    }
    
    ValueTree getState(void);
    
    void setState(XmlElement* e);
    
    inline void setUIState(ValueTree vt)
    {
        
    }
    
    inline ValueTree getUIState(void)
    {
        
    }
    
    inline void configurePianoMap(Array<int> keymaps, int pianoId)
    {
        for (auto keymapId : keymaps)
        {
            Keymap::Ptr thisKeymap = getKeymap(keymapId);
            for (auto key : thisKeymap->keys())
            {
                pianoMap.set(key, pianoId);
                
                DBG("key: " + String(key) + " piano: " + String(pianoId));
            }
        }
    }
    
    inline void deconfigurePianoMap(Array<int> keymaps, int pianoId)
    {
        for (auto keymapId : keymaps)
        {
            Keymap::Ptr thisKeymap = getKeymap(keymapId);
        
            for (auto key : thisKeymap->keys())
            {
                pianoMap.set(key, -1);
            }
            
        }
    }
    
    String modificationMapsToString(void)
    {
        String out = "";
        for (int i = 0; i < 128; i++)
        {
            String ptype = "";
            for (auto map : mappers)
            {
                if (map->getType() == PreparationTypeDirect) ptype = "d";
                else if (map->getType() == PreparationTypeNostalgic) ptype = "n";
                else if (map->getType() == PreparationTypeSynchronic) ptype = "s";
                else if (map->getType() == PreparationTypeTuning) ptype = "t";
                else if (map->getType() == PreparationTypeTempo) ptype = "m";
                
                out += String(i) + ":" + ptype + String(map->getId()) + ":" + "{" + intArrayToString(map->getTargets()) +"} ";
                
            }
        }
        
        return out;
    }

    void clearmodificationMap(void)
    {
        for (int i = 0; i<modificationMap.size(); i++)
        {
            modificationMap[i]->clearModifications();
        }
    }

    void clearResetMap(void)
    {
        for (int i = 0; i<modificationMap.size(); i++)
        {
            modificationMap[i]->clearResets();
        }
    }


    int                         numModSMaps, numModNMaps, numModDMaps;

    void                        prepareToPlay(double sampleRate);
    
    void deconfigureResets(Array<Array<int>> resets, Array<int> whichKeymaps);
    void configureResets(Array<Array<int>> resets, Array<int> whichKeymaps, Array<int> whichPreps);
    void deconfigureResetsForKeys(Array<Array<int>> resets, Array<int> otherKeys);
    
    void configureModifications(ModificationMapper::PtrArr maps);
    void configureModification(ModificationMapper::Ptr map);
    void deconfigureModification(ModificationMapper::Ptr map);
    
    int                         addPreparationMap(void);
    int                         addPreparationMap(Keymap::Ptr keymap);
    PreparationMap::Ptr         getPreparationMapWithKeymap(int keymapId);
    int                         removeLastPreparationMap(void);
    int                         removePreparationMapWithKeymap(int keymapId);

    PianoConfiguration::Ptr     configuration;
    
    Array<Array<int>> pianoMaps;
private:
    int Id;
    String pianoName;

    double sampleRate;
    
    

    // Pointers to synths (flown in from BKAudioProcessor)
    BKSynthesiser*                      synth;
    BKSynthesiser*                      resonanceSynth;
    BKSynthesiser*                      hammerSynth;
    
    Synchronic::PtrArr*  synchronic;
    Nostalgic::PtrArr*   nostalgic;
    Direct::PtrArr*      direct;
    Tuning::PtrArr*      tuning;
    Tempo::PtrArr*       tempo;
    
    SynchronicModPreparation::PtrArr*    modSynchronic;
    NostalgicModPreparation::PtrArr*     modNostalgic;
    DirectModPreparation::PtrArr*        modDirect;
    TempoModPreparation::PtrArr*         modTempo;
    
    Keymap::PtrArr*                      bkKeymaps;
    TuningModPreparation::PtrArr*        modTuning;
    
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
    
    inline Array<int> getAllIds(Synchronic::PtrArr direct)
    {
        Array<int> which;
        for (auto p : direct)
        {
            which.add(p->getId());
        }
        
        return which;
    }
    
    void configureDirectModification(DirectModPreparation::Ptr, Array<int> whichKeymaps, Array<int> whichPreps);
    void configureDirectModification(int key, DirectModPreparation::Ptr, Array<int>);
    void deconfigureDirectModification(DirectModPreparation::Ptr, Array<int> whichKeymaps);
    void deconfigureDirectModificationForKeys(DirectModPreparation::Ptr, Array<int>);
    
    void configureSynchronicModification(SynchronicModPreparation::Ptr, Array<int> whichKeymaps, Array<int> whichPreps);
    void configureSynchronicModification(int key, SynchronicModPreparation::Ptr, Array<int>);
    void deconfigureSynchronicModification(SynchronicModPreparation::Ptr, Array<int> whichKeymaps);
    void deconfigureSynchronicModificationForKeys(SynchronicModPreparation::Ptr, Array<int>);
    
    void configureNostalgicModification(NostalgicModPreparation::Ptr mod, Array<int> whichKeymaps, Array<int> whichPreps);
    void configureNostalgicModification(int key, NostalgicModPreparation::Ptr, Array<int>);
    void deconfigureNostalgicModification(NostalgicModPreparation::Ptr, Array<int> whichKeymaps);
    void deconfigureNostalgicModificationForKeys(NostalgicModPreparation::Ptr, Array<int>);
    
    void configureTuningModification(TuningModPreparation::Ptr, Array<int> whichKeymaps, Array<int> whichPreps);
    void configureTuningModification(int key, TuningModPreparation::Ptr, Array<int>);
    void deconfigureTuningModification(TuningModPreparation::Ptr, Array<int> whichKeymaps);
    void deconfigureTuningModificationForKeys(TuningModPreparation::Ptr, Array<int>);
    
    void configureTempoModification(TempoModPreparation::Ptr, Array<int> whichKeymaps, Array<int> whichPreps);
    void configureTempoModification(int key, TempoModPreparation::Ptr, Array<int>);
    void deconfigureTempoModification(TempoModPreparation::Ptr, Array<int> whichKeymaps);
    void deconfigureTempoModificationForKeys(TempoModPreparation::Ptr, Array<int>);
    
    inline const Synchronic::Ptr getSynchronic(int Id) const noexcept
    {
        for (int i = 0; i < synchronic->size(); i++)
        {
            Synchronic::Ptr thisOne = synchronic->getUnchecked(i);
            
            if (thisOne->getId() == Id)   return thisOne;
        }
        return nullptr;
    }
    
    inline const Nostalgic::Ptr getNostalgic(int Id) const noexcept
    {
        for (int i = 0; i < nostalgic->size(); i++)
        {
            Nostalgic::Ptr thisOne = nostalgic->getUnchecked(i);
            
            if (thisOne->getId() == Id)   return thisOne;
        }
        return nullptr;
    }
    
    inline const Direct::Ptr getDirect(int Id) const noexcept
    {
        for (int i = 0; i < direct->size(); i++)
        {
            Direct::Ptr thisOne = direct->getUnchecked(i);
            
            if (thisOne->getId() == Id)   return thisOne;
        }
        return nullptr;
    }
    
    inline const Tuning::Ptr getTuning(int Id) const noexcept
    {
        for (int i = 0; i < tuning->size(); i++)
        {
            Tuning::Ptr thisOne = tuning->getUnchecked(i);
            
            if (thisOne->getId() == Id)   return thisOne;
        }
        return nullptr;
    }
    
    inline const Tempo::Ptr getTempo(int Id) const noexcept
    {
        for (int i = 0; i < tempo->size(); i++)
        {
            Tempo::Ptr thisOne = tempo->getUnchecked(i);
            
            if (thisOne->getId() == Id)   return thisOne;
        }
        return nullptr;
    }
    
    inline const SynchronicModPreparation::Ptr getSynchronicModPreparation(int Id) const noexcept
    {
        for (int i = 0; i < modSynchronic->size(); i++)
        {
            SynchronicModPreparation::Ptr thisOne = modSynchronic->getUnchecked(i);
            
            if (thisOne->getId() == Id)   return thisOne;
        }
        return nullptr;
    }
    
    inline const NostalgicModPreparation::Ptr getNostalgicModPreparation(int Id) const noexcept
    {
        for (int i = 0; i < modNostalgic->size(); i++)
        {
            NostalgicModPreparation::Ptr thisOne = modNostalgic->getUnchecked(i);
            
            if (thisOne->getId() == Id)   return thisOne;
        }
        return nullptr;
    }
    
    inline const DirectModPreparation::Ptr getDirectModPreparation(int Id) const noexcept
    {
        for (int i = 0; i < modDirect->size(); i++)
        {
            DirectModPreparation::Ptr thisOne = modDirect->getUnchecked(i);
            
            if (thisOne->getId() == Id)   return thisOne;
        }
        return nullptr;
    }
    
    inline const TuningModPreparation::Ptr getTuningModPreparation(int Id) const noexcept
    {
        for (int i = 0; i < modTuning->size(); i++)
        {
            TuningModPreparation::Ptr thisOne = modTuning->getUnchecked(i);
            
            if (thisOne->getId() == Id)   return thisOne;
        }
        return nullptr;
    }
    
    inline const TempoModPreparation::Ptr getTempoModPreparation(int Id) const noexcept
    {
        for (int i = 0; i < modTempo->size(); i++)
        {
            TempoModPreparation::Ptr thisOne = modTempo->getUnchecked(i);
            
            if (thisOne->getId() == Id)   return thisOne;
        }
        return nullptr;
    }
    
    inline const Keymap::Ptr getKeymap(int Id) const noexcept
    {
        for (int i = 0; i < bkKeymaps->size(); i++)
        {
            Keymap::Ptr thisOne = bkKeymaps->getUnchecked(i);
            
            if (thisOne->getId() == Id)   return thisOne;
        }
        return nullptr;
    }
    
    JUCE_LEAK_DETECTOR(Piano)
};


#endif  // Piano_H_INCLUDED
