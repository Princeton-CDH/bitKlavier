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
    
    inline ModificationMapper::Ptr getResetMapper(BKPreparationType resetType, int which)
    {
        ModificationMapper::Ptr thisMapper = new ModificationMapper(resetType, Id);
        
        bool add = true;
        for (auto map : mappers)
        {
            if (map->getType() == PreparationTypeReset && map->getId() == Id)
            {
                thisMapper = map;
                add = false;
                break;
            }
        }
        
        if (add) mappers.add(thisMapper);
        
        return thisMapper;
    }
    
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
        mappers.add(thisMapper);
    }
    
    ValueTree getState(void);
    
    void setState(XmlElement* e);
    
    inline void setUIState(ValueTree vt)
    {
        
    }
    
    inline ValueTree getUIState(void)
    {
        
    }
    
    inline void configurePianoMap(Keymap::Ptr thisKeymap, int pianoId)
    {
        for (auto key : thisKeymap->keys())
        {
            pianoMap.set(key, pianoId);
            
            DBG("key: " + String(key) + " piano: " + String(pianoId));
        }
    
    }
    
    inline void deconfigurePianoMap(Keymap::Ptr thisKeymap)
    {
        for (auto key: thisKeymap->keys())
            pianoMap.set(key, -1);
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
    
    int                         addPreparationMap(void);
    int                         addPreparationMap(Keymap::Ptr keymap);
    PreparationMap::Ptr         getPreparationMapWithKeymap(Keymap::Ptr);
    int                         removeLastPreparationMap(void);
    int                         removePreparationMapWithKeymap(Keymap::Ptr thisKeymap);

    PianoConfiguration::Ptr     configuration;
    
    
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
    
    
    
    JUCE_LEAK_DETECTOR(Piano)
};


#endif  // Piano_H_INCLUDED
