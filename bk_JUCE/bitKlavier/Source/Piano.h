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
    
    inline Piano::Ptr duplicate(bool withSameId = false)
    {
        Piano::Ptr copyPiano = new Piano(processor, withSameId ? Id : -1);
        
        BKItem::PtrArr newItems;
        
        for (auto item : items)
        {
            BKItem* newItem = new BKItem(item->getType(), item->getId(), processor);
            
            newItem->setCommentText(item->getCommentText());
            
            newItem->setTopLeftPosition(item->getPosition());
            newItem->setName(item->getName());
            
            copyPiano->add(newItem);
            newItems.add(newItem);
            
        }
        
        int idx = 0;
        for (auto item : items)
        {
            BKItem* newItem = newItems.getUnchecked(idx++);
            
            BKItem::PtrArr oldConnections = item->getConnections();
            
            for (auto connection : item->getConnections())
            {
                for (auto newConnection : newItems)
                {
                    if ((newConnection->getType() == connection->getType()) &&
                        (newConnection->getId() == connection->getId()))
                    {
                        newItem->addConnection(newConnection);
                    }
                }
            }
        }
        
        copyPiano->setName(pianoName );
        
        copyPiano->prepareToPlay(sampleRate);
        
        copyPiano->configure();
        
        return copyPiano;
    }

    void clear(void);
    
    void storeCurrentPiano();
    
    void recallCurrentPiano();


    ValueTree*  getPianoValueTree(void);
    
    inline void setId(int newId) { Id = newId; }
    
    inline int getId(void) { return Id; }

    inline const String getName() const noexcept {return pianoName;}
    inline void setName(String n){pianoName = n;}
    
    Keymap::PtrArr                       keymaps;
    
    DirectProcessor::PtrArr              dprocessor;
    SynchronicProcessor::PtrArr          sprocessor;
    NostalgicProcessor::PtrArr           nprocessor;
    TempoProcessor::PtrArr               mprocessor;
    TuningProcessor::PtrArr              tprocessor;
	BlendronicProcessor::PtrArr		     bprocessor;
    
    void addProcessor(BKPreparationType thisType, int thisId);
    bool containsProcessor(BKPreparationType thisType, int thisId);
    bool contains(Keymap::Ptr);
    bool contains(DirectProcessor::Ptr);
    bool contains(NostalgicProcessor::Ptr);
    bool contains(SynchronicProcessor::Ptr);
    bool contains(TuningProcessor::Ptr);
    bool contains(TempoProcessor::Ptr);
    bool contains(BlendronicProcessor::Ptr);
    
    Keymap::Ptr                         addKeymap               (Keymap::Ptr km);
    Keymap::Ptr                         getKeymap               (int Id, bool add = true);
    inline Keymap::PtrArr               getKeymaps              (void) const noexcept { return keymaps; }
    
    DirectProcessor::Ptr                addDirectProcessor      (int Id);
    DirectProcessor::Ptr                getDirectProcessor      (int Id, bool add = true);
    inline DirectProcessor::PtrArr      getDirectProcessors     (void) const noexcept { return dprocessor; }
    
    NostalgicProcessor::Ptr             addNostalgicProcessor   (int Id);
    NostalgicProcessor::Ptr             getNostalgicProcessor   (int Id, bool add = true);
    inline NostalgicProcessor::PtrArr   getNostalgicProcessors  (void) const noexcept { return nprocessor; }
    
    SynchronicProcessor::Ptr            addSynchronicProcessor  (int Id);
    SynchronicProcessor::Ptr            getSynchronicProcessor  (int Id, bool add = true);
    inline SynchronicProcessor::PtrArr  getSynchronicProcessors (void) const noexcept { return sprocessor; }
    
    TuningProcessor::Ptr                addTuningProcessor      (int Id);
    TuningProcessor::Ptr                getTuningProcessor      (int Id, bool add = true);
    inline TuningProcessor::PtrArr      getTuningProcessors     (void) const noexcept { return tprocessor; }
    
    TempoProcessor::Ptr                 addTempoProcessor       (int Id);
    TempoProcessor::Ptr                 getTempoProcessor       (int Id, bool add = true);
    inline TempoProcessor::PtrArr       getTempoProcessors      (void) const noexcept { return mprocessor; }
    
    BlendronicProcessor::Ptr            addBlendronicProcessor  (int Id);
    BlendronicProcessor::Ptr            getBlendronicProcessor  (int Id, bool add = true);
    inline BlendronicProcessor::PtrArr  getBlendronicProcessors (void) const noexcept { return bprocessor; }
    
    void clearOldNotes(Piano::Ptr prevPiano)
    {
        SynchronicProcessor::PtrArr sprocessors = getSynchronicProcessors();
        SynchronicProcessor::PtrArr prevSprocessors = prevPiano->getSynchronicProcessors();
        
        for(int i=0; i<sprocessors.size(); i++)
        {
            bool inPrevSproc = false;
            for(int j=0; j<prevSprocessors.size(); j++)
            {
                if(sprocessors.getUnchecked(i) == prevSprocessors.getUnchecked(j))
                    inPrevSproc = true;
            }
            if(!inPrevSproc) sprocessors.getUnchecked(i)->clearOldNotes(); //want to keep oldNotes if sProc is in previous piano
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
    

    Array<int>                  pianoMap;
    int                         numPMaps;
    
    int numResetMappers;
    
    OwnedArray<Modifications> modificationMap;
    
    inline BKItem::PtrArr getItems(void) const noexcept { return items; }
    
    inline BKItem* itemWithTypeAndId(BKPreparationType type, int thisId)
    {
        for (auto item : items)
        {
            if ((item->getType() == type) && (item->getId() == thisId)) return item;
        }
        return nullptr;
    }
    
    inline bool containsItem(BKPreparationType type, int thisId)
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
    
    void add(BKItem::Ptr item);
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
    
    void linkKeymapToPreparation(int keymapId, BKPreparationType thisType, int thisId);
    
    ValueTree getState(void);
    
    void setState(XmlElement* e, OwnedArray<HashMap<int,int>> *idmap);
    
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
    void deconfigurePianoMap(BKItem::Ptr map);
    
    void configureReset(BKItem::Ptr item);
    void deconfigureResetForKeys(BKItem::Ptr item, Array<bool> otherKeys);

    void configureModification(BKItem::Ptr map);
    void deconfigureModification(BKItem::Ptr map);
    
    void keyPressed(int noteNumber, float velocity, int channel, bool soundfont = false, String source = String("Default"));
    void keyReleased(int noteNumber, float velocity, int channel, bool soundfont = false, String source = String("Default"));
    void postRelease(int noteNumber, float velocity, int channel, String source);
    
    void reattack(int noteNumber);
    void sustain(int noteNumber, float velocity, int channel, bool soundfont);
    
    void clearKey(int noteNumber);
    void sustainPedalPressed()  { sustainPedalIsDepressed = true;  }
    void sustainPedalReleased(bool post);
    void sustainPedalReleased(Array<bool> keysThatArePressed, bool post);
    void sustainPedalReleased() {sustainPedalReleased(false);};
    
    inline bool keymapsContainNote(int noteNumber) {
        bool contains = false;
        for (auto km : keymaps) if (km->containsNote(noteNumber)) contains = true;
        return contains;
    }
    
     void processBlock(AudioSampleBuffer& buffer, int numSamples, int midiChannel, BKSampleLoadType type, bool onlyNostalgic = false);
    
    Array<Array<int>> pianoMaps;
    
    void reset(void);
private:
    BKAudioProcessor& processor;
    
    int Id;
    String pianoName;
    
    BKSampleLoadType sampleType;

    double sampleRate;
    
    TuningProcessor::Ptr defaultT;
    TempoProcessor::Ptr defaultM;
    SynchronicProcessor::Ptr defaultS;
	BlendronicProcessor::Ptr defaultB;
    BlendronicProcessor::PtrArr defaultBA;
	BlendronicDelay::Ptr defaultD;
    
    bool sustainPedalIsDepressed;
    
    struct SustainedNote
    {
        int noteNumber;
        float velocity;
        int channel;
    };
    Array<SustainedNote> sustainedNotes;
    
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
    
    void configureDirectModification(DirectModification::Ptr, Array<int> whichKeymaps, Array<int> whichPreps);
    void deconfigureDirectModification(DirectModification::Ptr, Array<int> whichKeymaps);
    void deconfigureDirectModificationForKeys(DirectModification::Ptr, Array<bool>);
    
    void configureSynchronicModification(SynchronicModification::Ptr, Array<int> whichKeymaps, Array<int> whichPreps);
    void deconfigureSynchronicModification(SynchronicModification::Ptr, Array<int> whichKeymaps);
    void deconfigureSynchronicModificationForKeys(SynchronicModification::Ptr, Array<bool>);
    
    void configureNostalgicModification(NostalgicModification::Ptr mod, Array<int> whichKeymaps, Array<int> whichPreps);
    void deconfigureNostalgicModification(NostalgicModification::Ptr, Array<int> whichKeymaps);
    void deconfigureNostalgicModificationForKeys(NostalgicModification::Ptr, Array<bool>);
    
    void configureTuningModification(TuningModification::Ptr, Array<int> whichKeymaps, Array<int> whichPreps);
    void deconfigureTuningModification(TuningModification::Ptr, Array<int> whichKeymaps);
    void deconfigureTuningModificationForKeys(TuningModification::Ptr, Array<bool>);
    
    void configureTempoModification(TempoModification::Ptr, Array<int> whichKeymaps, Array<int> whichPreps);
    void deconfigureTempoModification(TempoModification::Ptr, Array<int> whichKeymaps);
    void deconfigureTempoModificationForKeys(TempoModification::Ptr, Array<bool>);
    
	void configureBlendronicModification(BlendronicModification::Ptr mod, Array<int> whichKeymaps, Array<int> whichPreps);
	void deconfigureBlendronicModification(BlendronicModification::Ptr, Array<int> whichKeymaps);
	void deconfigureBlendronicModificationForKeys(BlendronicModification::Ptr, Array<bool>);

    JUCE_LEAK_DETECTOR(Piano)
};


#endif  // Piano_H_INCLUDED
