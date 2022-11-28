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
#include "BKSTK.h"

#include "PianoConfig.h"

#include "Direct.h"
class BKAudioProcessor;

#include "BKGraph.h"

class Piano : public ReferenceCountedObject
{
public:
    typedef ReferenceCountedObjectPtr<Piano>   Ptr;
    typedef Array<Piano::Ptr>                  PtrArr;
    typedef Array<Piano::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<Piano>                  Arr;
    typedef OwnedArray<Piano, CriticalSection> CSArr;
    
    Piano(BKAudioProcessor& p,
          int Id);
    ~Piano();
    
    Piano::Ptr duplicate(bool withSameId = false);
    
    void clear(void);
    
    void storeCurrentPiano();
    
    void recallCurrentPiano();
    
    
    ValueTree*  getPianoValueTree(void);
    
    inline void setId(int newId) { Id = newId; }
    
    inline int getId(void) { return Id; }
    
    inline const String getName() const noexcept {return pianoName;}
    inline void setName(String n){pianoName = n;}
    
    PreparationMap::Ptr         prepMap;
 
    
    GenericProcessor::Ptr addProcessor(BKPreparationType thisType, int thisId);
    bool containsProcessor(BKPreparationType thisType, int thisId);
    
	inline EffectProcessor::PtrArr  getBlendronicProcessors(void) const noexcept
    {
        EffectProcessor::PtrArr blends;
        return blends;
    }
    
    GenericProcessor::Ptr getProcessorOfType(int Id, BKPreparationType type, bool add = true);
    inline PreparationMap::Ptr       getPreparationMap(void) const noexcept { return prepMap; }
    
    
    GenericProcessor::Ptr     addNostalgicProcessor(int thisId);
    GenericProcessor::Ptr    addSynchronicProcessor(int thisId);
    GenericProcessor::Ptr        addDirectProcessor(int thisId);
    GenericProcessor::Ptr        addTuningProcessor(int thisId);
    GenericProcessor::Ptr         addTempoProcessor(int thisId);
	EffectProcessor::Ptr        addBlendronicProcessor(int thisId);
    GenericProcessor::Ptr     addResonanceProcessor(int thisId);

    void pianoSwitched(Piano::Ptr prevPiano)
    {
       //Maintain order (clearOldNotes happens first)
        clearOldNotes(prevPiano);
        copyProcessorStates(prevPiano);
    }
    void clearOldNotes(Piano::Ptr prevPiano)
    {
        

        for(auto currproc : *prepMap->getProcessorsOfType(PreparationTypeSynchronic))
        {
            bool inPrevSproc = false;
            for(auto proc : *prevPiano->getPreparationMap()->getProcessorsOfType(PreparationTypeSynchronic))
            {
                if(currproc->getId() == proc->getId())
                    inPrevSproc = true;
            }

            if(!inPrevSproc) {
                DBG("clearing old Synchronic notes");
                dynamic_cast<SynchronicProcessor*>(currproc)->clearOldNotes(); //want to keep oldNotes if sProc is in previous piano
            }
        }
    }
    
    // For when a piano switch occurs
    // Pianos use separate processors even for the same preparation, so we need to copy over
    // some parts (things like saved velocities or Synchronic clusters, but not reference to
    // connected preparations) of the processor from the previous piano to keep continuity
    void copyProcessorStates (Piano::Ptr prevPiano)
    {
        prepMap->copyProcessorStates(prevPiano->getPreparationMap());
    }
    
    int numPMaps;
    int numResetMappers;
    
    OwnedArray<Modifications> modificationMap;
    
    inline BKItem::PtrArr getItems(void) const noexcept { return items; }
    
    inline BKItem::Ptr itemWithTypeAndId(BKPreparationType type, int thisId)
    {
        for (auto item : items)
        {
            if ((item->getType() == type) && (item->getId() == thisId)) return item;
        }
        return nullptr;
    }
    
    inline bool contains(BKPreparationType type, int thisId)
    {
        for (auto item : items)
        {
            if ((item->getType() == type) && (item->getId() == thisId)) return true;
        }
        return false;
    }
    
    inline bool isActive(BKPreparationType type, int thisId)
    {
        for (auto item : items)
        {
            if (item->getType() == type && item->getId() == thisId) return item->isActive();
        }
        return false;
    }
    
    inline void setActive(BKPreparationType type, int thisId, bool active)
    {
        for (auto item : items)
        {
            if (item->getType() == type && item->getId() == thisId) item->setActive(active);
        }
    }
    
    inline void clearItems(void) { items.clear(); }
    
    void add(BKItem::Ptr item, bool configureIfAdded=true);
    bool contains(BKItem::Ptr item);
    void remove(BKItem::Ptr item);
    void configure(void);
    void deconfigure(void);
    
    BKItem::PtrArr    items;
    
    void removePreparationFromKeymap(BKPreparationType thisType, int thisId, int keymapId);
    
    void linkPreparationWithKeymap(BKPreparationType thisType, int thisId, int keymapId);
    
    void linkPreparationWithTempo(BKPreparationType thisType, int thisId, Tempo::Ptr thisTempo);
    
    void linkNostalgicWithSynchronic(Nostalgic::Ptr nostalgic, Synchronic::Ptr synchronic);
    
    void linkPreparationWithTuning(BKPreparationType thisType, int thisId, Tuning::Ptr thisTuning);
    
    void linkPreparationWithBlendronic(BKPreparationType thisType, int thisId, Blendronic::Ptr thisBlend);
    
    ValueTree getState(void);
    
    void setState(XmlElement* e, OwnedArray<HashMap<int,int>> *idmap, int* idcounts);
    
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
                else if (type == PreparationTypeResonance) ptype = "r";
				else if (type == PreparationTypeSynchronic) ptype = "s";
				else if (type == PreparationTypeTuning) ptype = "t";
				else if (type == PreparationTypeTempo) ptype = "m";
				else if (type == PreparationTypeBlendronic) ptype = "b";

                
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
    
    int numModSMaps, numModNMaps, numModDMaps;

    void prepareToPlay();
    
    void configurePianoMap(BKItem::Ptr map);
    
    void configureReset(BKItem::Ptr item);
    
    void configureModification(BKItem::Ptr map);
    
    
    void reset(void);
private:
    BKAudioProcessor& processor;
    
    int Id;
    String pianoName;
    
    TuningProcessor::Ptr defaultT;
    TempoProcessor::Ptr defaultM;
    SynchronicProcessor::Ptr defaultS;
    ResonanceProcessor::Ptr defaultR;
    
    
    void configureDirectModification(DirectModification::Ptr, Array<int> whichKeymaps, Array<int> whichPreps);
    
    void configureSynchronicModification(SynchronicModification::Ptr, Array<int> whichKeymaps, Array<int> whichPreps);
    
    void configureNostalgicModification(NostalgicModification::Ptr mod, Array<int> whichKeymaps, Array<int> whichPreps);
    
    void configureTuningModification(TuningModification::Ptr, Array<int> whichKeymaps, Array<int> whichPreps);
    
    void configureTempoModification(TempoModification::Ptr, Array<int> whichKeymaps, Array<int> whichPreps);
    
	void configureBlendronicModification(BlendronicModification::Ptr mod, Array<int> whichKeymaps, Array<int> whichPreps);
    
    void configureResonanceModification(ResonanceModification::Ptr mod, Array<int> whichKeymaps, Array<int> whichPreps);

    JUCE_LEAK_DETECTOR(Piano)
};


#endif  // Piano_H_INCLUDED

