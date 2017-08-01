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

#include "PianoConfig.h"

#include "ItemMapper.h"

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

    
    ItemMapper::PtrArr     items;
    
    inline ItemMapper::PtrArr getItems(void) const noexcept { return items; }
    
    inline bool isActive(BKPreparationType type, int Id)
    {
        for (auto item : items)
        {
            if (item->getType() == type && item->getId() == Id) return item->isActive();
        }
        return false;
    }
    
    inline void setActive(BKPreparationType type, int Id, bool active)
    {
        for (auto item : items)
        {
            if (item->getType() == type && item->getId() == Id) item->setActive(active);
        }
    }
    
    inline void clearItems(void) { items.clear(); }
    
    inline ItemMapper::Ptr getItem(BKPreparationType type, int Id)
    {
        ItemMapper::Ptr thisMapper = new ItemMapper(type, Id);
        
        bool add = true;
        for (auto map : items)
        {
            if (map->getType() == type && map->getId() == Id)
            {
                thisMapper = map;
                add = false;
                break;
            }
        }
        
        if (add) items.add(thisMapper);
        
        return thisMapper;
    }
    
    void add(ItemMapper::Ptr item);
    void remove(ItemMapper::Ptr item);
    void configure(void);
    void deconfigure(void);

    
    void removePreparationFromKeymap(BKPreparationType thisType, int thisId, int keymapId);
    
    void addPreparationToKeymap(BKPreparationType thisType, int thisId, int keymapId);
    
    void linkSynchronicWithTempo(Synchronic::Ptr synchronic, Tempo::Ptr thisTempo);
    
    void linkNostalgicWithSynchronic(Nostalgic::Ptr nostalgic, Synchronic::Ptr synchronic);
    
    void linkPreparationWithTuning(BKPreparationType thisType, int thisId, Tuning::Ptr thisTuning);
    
    ValueTree getState(void);
    
    void setState(XmlElement* e);
    
    inline void setUIState(ValueTree vt)
    {
        
    }
    
    inline ValueTree getUIState(void)
    {
        
    }
    
    
    String modificationMapsToString(void)
    {
        String out = "";
        for (int i = 0; i < 128; i++)
        {
            String ptype = "";
            for (auto item : items)
            {
                BKPreparationType type = item->getType();
                
                if (type == PreparationTypeDirect) ptype = "d";
                else if (type == PreparationTypeNostalgic) ptype = "n";
                else if (type == PreparationTypeSynchronic) ptype = "s";
                else if (type == PreparationTypeTuning) ptype = "t";
                else if (type == PreparationTypeTempo) ptype = "m";
                
                out += String(i) + ":" + ptype + String(item->getId()) + ":" + "{" + item->connectionsToString() +"} ";
                
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
    
    void configurePianoMap(ItemMapper::Ptr map);
    void deconfigurePianoMap(ItemMapper::Ptr map);
    
    void configureReset(ItemMapper::Ptr item);
    void deconfigureReset(ItemMapper::Ptr item);
    void deconfigureResetForKeys(ItemMapper::Ptr item, Array<int> otherKeys);
    
    void deconfigureResets(Array<Array<int>> resets, Array<int> whichKeymaps);
    void configureResets(Array<Array<int>> resets, Array<int> whichKeymaps, Array<int> whichPreps);
    void deconfigureResetsForKeys(Array<Array<int>> resets, Array<int> otherKeys);
    
    void configureModifications(ItemMapper::PtrArr maps);
    void configureModification(ItemMapper::Ptr map);
    void deconfigureModification(ItemMapper::Ptr map);
    
    int                         addPreparationMap(void);
    int                         addPreparationMap(Keymap::Ptr keymap);
    PreparationMap::Ptr         getPreparationMapWithKeymap(int keymapId);
    int                         removeLastPreparationMap(void);
    int                         removePreparationMapWithKeymap(int keymapId);
    
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
