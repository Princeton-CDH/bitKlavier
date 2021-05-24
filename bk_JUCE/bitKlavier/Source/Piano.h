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
    
    DirectProcessor::PtrArr              dprocessor;
    SynchronicProcessor::PtrArr          sprocessor;
    NostalgicProcessor::PtrArr           nprocessor;
    TempoProcessor::PtrArr               mprocessor;
    TuningProcessor::PtrArr              tprocessor;
	BlendronicProcessor::PtrArr		     bprocessor;
    ResonanceProcessor::PtrArr           rprocessor;
    
    void addProcessor(BKPreparationType thisType, int thisId);
    bool containsProcessor(BKPreparationType thisType, int thisId);
    
    DirectProcessor::Ptr        getDirectProcessor(int Id, bool add = true);
    NostalgicProcessor::Ptr     getNostalgicProcessor(int Id, bool add = true);
    SynchronicProcessor::Ptr    getSynchronicProcessor(int Id, bool add = true);
    TuningProcessor::Ptr        getTuningProcessor(int Id, bool add = true);
    TempoProcessor::Ptr         getTempoProcessor(int Id, bool add = true);
	BlendronicProcessor::Ptr	getBlendronicProcessor(int Id, bool add = true);
    ResonanceProcessor::Ptr     getResonanceProcessor(int Id, bool add = true);
    
    inline DirectProcessor::PtrArr        getDirectProcessors(void) const noexcept { return dprocessor; }
    inline NostalgicProcessor::PtrArr     getNostalgicProcessors(void) const noexcept { return nprocessor; }
    inline SynchronicProcessor::PtrArr    getSynchronicProcessors(void) const noexcept { return sprocessor; }
    inline TuningProcessor::PtrArr        getTuningProcessors(void) const noexcept { return tprocessor; }
    inline TempoProcessor::PtrArr         getTempoProcessors(void) const noexcept { return mprocessor; }
	inline BlendronicProcessor::PtrArr  getBlendronicProcessors(void) const noexcept { return bprocessor; }
    inline ResonanceProcessor::PtrArr   getResonanceProcessors(void) const noexcept { return rprocessor; }
    inline PreparationMap::Ptr       getPreparationMap(void) const noexcept { return prepMap; }
    
    NostalgicProcessor::Ptr     addNostalgicProcessor(int thisId);
    SynchronicProcessor::Ptr    addSynchronicProcessor(int thisId);
    DirectProcessor::Ptr        addDirectProcessor(int thisId);
    TuningProcessor::Ptr        addTuningProcessor(int thisId);
    TempoProcessor::Ptr         addTempoProcessor(int thisId);
	BlendronicProcessor::Ptr    addBlendronicProcessor(int thisId);
    ResonanceProcessor::Ptr     addResonanceProcessor(int thisId);
    
    void clearOldNotes(Piano::Ptr prevPiano)
    {
        SynchronicProcessor::PtrArr sprocessors = getSynchronicProcessors();
        SynchronicProcessor::PtrArr prevSprocessors = prevPiano->getSynchronicProcessors();
        
        for(int i = 0; i < sprocessors.size(); i++)
        {
            bool inPrevSproc = false;
            for(int j=0; j<prevSprocessors.size(); j++)
            {
                if(sprocessors.getUnchecked(i) == prevSprocessors.getUnchecked(j))
                    inPrevSproc = true;
            }
            
            if(!inPrevSproc) {
                DBG("clearing old Synchronic notes");
                sprocessors.getUnchecked(i)->clearOldNotes(); //want to keep oldNotes if sProc is in previous piano
            }
        }
    }
    
    void copyAdaptiveTuningState (Piano::Ptr prevPiano)
    {
        TuningProcessor::PtrArr prevTuningProcessors = prevPiano->getTuningProcessors();
        for(int i=0; i<prevTuningProcessors.size(); i++)
        {
            for(int j=0; j<tprocessor.size(); j++)
            {
                if(tprocessor.getUnchecked(j)->getId() == prevTuningProcessors.getUnchecked(i)->getId())
                {
                    tprocessor.getUnchecked(j)->setAdaptiveHistoryCounter(prevTuningProcessors.getUnchecked(i)->getAdaptiveHistoryCounter());
                    tprocessor.getUnchecked(j)->setAdaptiveFundamentalFreq(prevTuningProcessors.getUnchecked(i)->getAdaptiveFundamentalFreq());
                    tprocessor.getUnchecked(j)->setAdaptiveFundamentalNote(prevTuningProcessors.getUnchecked(i)->getAdaptiveFundamentalNote());
                }
            }
        }
    }
    
    void copyAdaptiveTempoState (Piano::Ptr prevPiano)
    {
        TempoProcessor::PtrArr prevTempoProcessors = prevPiano->getTempoProcessors();
        for(int i=0; i<prevTempoProcessors.size(); i++)
        {
            for(int j=0; j<mprocessor.size(); j++)
            {
                if(mprocessor.getUnchecked(j)->getId() == prevTempoProcessors.getUnchecked(i)->getId())
                {
                    mprocessor.getUnchecked(j)->setAtTimer(prevTempoProcessors.getUnchecked(i)->getAtTimer());
                    mprocessor.getUnchecked(j)->setAtLastTime(prevTempoProcessors.getUnchecked(i)->getAtLastTime());
                    mprocessor.getUnchecked(j)->setAtDeltaHistory(prevTempoProcessors.getUnchecked(i)->getAtDeltaHistory());
                    mprocessor.getUnchecked(j)->setAdaptiveTempoPeriodMultiplier(prevTempoProcessors.getUnchecked(i)->getAdaptiveTempoPeriodMultiplier());
                }
            }
        }
    }
    
    void copySynchronicState (Piano::Ptr prevPiano)
    {
        SynchronicProcessor::PtrArr prevSynchronicProcessors = prevPiano->getSynchronicProcessors();
        for(int i = 0; i < prevSynchronicProcessors.size(); i++)
        {
            for(int j = 0; j < sprocessor.size(); j++)
            {
                if(sprocessor.getUnchecked(j)->getId() == prevSynchronicProcessors.getUnchecked(i)->getId())
                {
                    sprocessor.getUnchecked(j)->setClusters(prevSynchronicProcessors.getUnchecked(i)->getClusters()); 
                }
            }
        }
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

    void linkPreparationwithResonance(BKPreparationType thisType, int thisId, Resonance::Ptr thisRes);
    
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


    int                         numModSMaps, numModNMaps, numModDMaps;

    void                        prepareToPlay(double sampleRate);
    
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
	BlendronicProcessor::Ptr defaultB;
    BlendronicProcessor::PtrArr defaultBA;
	BlendronicDelay::Ptr defaultD;
    ResonanceProcessor::Ptr defaultR;
    
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

	inline Array<int> getAllIds(Blendronic::PtrArr direct)
	{
		Array<int> which;
		for (auto p : direct)
		{
			which.add(p->getId());
		}

		return which;
	}

    inline Array<int> getAllIds(Resonance::PtrArr resonance)
    {
        Array<int> which;
        for (auto p : resonance)
        {
            which.add(p->getId());
        }
        return which;
    }
    
    void configureDirectModification(DirectModification::Ptr, Array<int> whichKeymaps, Array<int> whichPreps);
    
    void configureSynchronicModification(SynchronicModification::Ptr, Array<int> whichKeymaps, Array<int> whichPreps);
    
    void configureNostalgicModification(NostalgicModification::Ptr mod, Array<int> whichKeymaps, Array<int> whichPreps);
    
    void configureTuningModification(TuningModification::Ptr, Array<int> whichKeymaps, Array<int> whichPreps);
    
    void configureTempoModification(TempoModification::Ptr, Array<int> whichKeymaps, Array<int> whichPreps);
    
	void configureBlendronicModification(BlendronicModification::Ptr mod, Array<int> whichKeymaps, Array<int> whichPreps);

    JUCE_LEAK_DETECTOR(Piano)
};

/*class PianoComparator : public ReferenceCountedObject
{
public:
	//currently compares by name as to enable "reodering" by changing name
	int compareElements(Piano::Ptr first, Piano::Ptr second)
	{
		int compareResult = first->getName().compare(second->getName());
		if (compareResult > 1) return 1;
		else if (compareResult < 1) return -1;
		else return 0;
	}
};*/


#endif  // Piano_H_INCLUDED
