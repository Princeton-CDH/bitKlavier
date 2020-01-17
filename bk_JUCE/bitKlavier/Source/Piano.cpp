/*
  ==============================================================================

    Piano.cpp
    Created: 7 Dec 2016 10:25:40am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "Piano.h"

#include "PluginProcessor.h"

#include "Gallery.h"

Piano::Piano(BKAudioProcessor& p, int Id):
processor(p),
Id(Id)
{
    numPMaps = 0;
    pianoMap.ensureStorageAllocated(128);
    
    modificationMap = OwnedArray<Modifications>();
    modificationMap.ensureStorageAllocated(128);
    
    for (int i = 0; i < 128; i++)
    {
        pianoMap.set(i, -1);
        
        pianoMaps.add(Array<int>());
        pianoMaps.add(Array<int>());
        
        modificationMap.add(new Modifications());
    }
}

Piano::~Piano()
{
    for (int i = 0; i < items.size(); i++)  items[i]->connections.clear();;
    items.clear();
}

void Piano::clear(void)
{
    items.clear();
}

void Piano::deconfigure(void)
{
    keymaps.clear();
    dprocessor.clear();
    mprocessor.clear();
    sprocessor.clear();
    nprocessor.clear();
    tprocessor.clear();
    for (auto b : bprocessor)
    {
        b->getSynth()->removeBlendronicProcessor(b->getId());
    }
    bprocessor.clear();
    
    for (int key = 0; key < 128; key++)
    {
        modificationMap[key]->clearModifications();
        modificationMap[key]->clearResets();
        pianoMap.set(key, -1);
    }
}

#define DEFAULT_ID -1
void Piano::configure(void)
{
    deconfigure();
    
    defaultT = getTuningProcessor(DEFAULT_ID);
    
    defaultM = getTempoProcessor(DEFAULT_ID);
    
    defaultS = getSynchronicProcessor(DEFAULT_ID);
    
    //defaultB = getBlendronicProcessor(DEFAULT_ID);
    
    // For each item in the piano that need a processor, add the processor
    for (auto item : items)
    {
        BKPreparationType thisType = item->getType();
        int thisId = item->getId();
        
        if (thisId > processor.gallery->getIdCount(thisType)) processor.gallery->setIdCount(thisType, thisId);
        
        addProcessor(thisType, thisId);
    }
    
    // Now handle connecting things
    for (auto item : items)
    {
        BKPreparationType type = item->getType();
        int Id = item->getId();
        
        if (type == PreparationTypeKeymap)
        {
            Keymap::Ptr keymap = processor.gallery->getKeymap(Id);
            
            BKItem::PtrArr connex = item->getConnections();
            for (auto target : connex)
            {
                BKPreparationType targetType = target->getType();
                int targetId = target->getId();
                
                if ((targetType >= PreparationTypeDirect && targetType <= PreparationTypeTempo) || targetType == PreparationTypeBlendronic)
                {
                    
                    DBG(String(targetType) + " linked with keymap");
                    
                    linkPreparationWithKeymap(targetType, targetId, Id);
                    
                }
                else if ((targetType >= PreparationTypeDirectMod && targetType <= PreparationTypeTempoMod) || targetType == PreparationTypeBlendronicMod)
                {
                    configureModification(target);
                }
                else if (targetType == PreparationTypePianoMap)
                {
                    configurePianoMap(target);
                }
                else if (targetType == PreparationTypeReset)
                {
                    configureReset(target);
                }
            }
            
            connex.clear();

        }
        else if (type == PreparationTypeTuning)
        {
            // Look for synchronic, direct, nostalgic, and blendronic targets
            for (auto target : item->getConnections())
            {
                BKPreparationType targetType = target->getType();
                int targetId = target->getId();
                
                if ((targetType >= PreparationTypeDirect && targetType <= PreparationTypeNostalgic))
                {
                    linkPreparationWithTuning(targetType, targetId, processor.gallery->getTuning(Id));
                }
            }
        }
        else if (type == PreparationTypeTempo)
        {
            // Look for synchronic targets
            for (auto target : item->getConnections())
            {
                BKPreparationType targetType = target->getType();
                int targetId = target->getId();
                
                if (targetType == PreparationTypeSynchronic || targetType == PreparationTypeBlendronic)
                {
                    linkPreparationWithTempo(targetType, targetId, processor.gallery->getTempo(Id));
                }
            }
        }
        else if (type == PreparationTypeSynchronic)
        {
            // Look for nostalgic targets
            for (auto target : item->getConnections())
            {
                BKPreparationType targetType = target->getType();
                int targetId = target->getId();
                
                if (targetType == PreparationTypeNostalgic)
                {
                    linkNostalgicWithSynchronic(processor.gallery->getNostalgic(targetId), processor.gallery->getSynchronic(Id));
                }
            }
        }

		else if (type == PreparationTypeBlendronic)
		{
			//look for direct, nostalgic, and synchronic targets
			for (auto target : item->getConnections())
			{
				BKPreparationType targetType = target->getType();
				int targetId = target->getId();

                if ((targetType >= PreparationTypeDirect && targetType <= PreparationTypeNostalgic) ||
                    targetType == PreparationTypeBlendronicMod)
				{
					linkPreparationWithBlendronic(targetType, targetId, processor.gallery->getBlendronic(Id));
				}
			}
		}
    }
    
    //processor.updateState->pianoDidChangeForGraph = true;
}

SynchronicProcessor::Ptr Piano::addSynchronicProcessor(int thisId)
{
    SynchronicProcessor::Ptr sproc = new SynchronicProcessor(processor.gallery->getSynchronic(thisId),
                                        defaultT,
                                        defaultM,
										defaultBA,
                                        &processor.mainPianoSynth,
                                        processor.gallery->getGeneralSettings());
    sproc->prepareToPlay(sampleRate, &processor.mainPianoSynth);
    sprocessor.add(sproc);
    
    return sproc;
}

Keymap::Ptr Piano::getKeymap(int Id, bool add)
{
    for (auto km : keymaps) if (km->getId() == Id) return km;
    return add ? addKeymap(processor.gallery->getKeymap(Id)) : nullptr;
}

DirectProcessor::Ptr Piano::getDirectProcessor(int Id, bool add)
{
    for (auto proc : dprocessor) if (proc->getId() == Id) return proc;
    return add ? addDirectProcessor(Id) : nullptr;
}

NostalgicProcessor::Ptr Piano::getNostalgicProcessor(int Id, bool add)
{
    for (auto proc : nprocessor) if (proc->getId() == Id) return proc;
    return add ? addNostalgicProcessor(Id) : nullptr;
}

SynchronicProcessor::Ptr Piano::getSynchronicProcessor(int Id, bool add)
{
    for (auto proc : sprocessor) if (proc->getId() == Id) return proc;
    return add ? addSynchronicProcessor(Id) : nullptr;
}

TuningProcessor::Ptr Piano::getTuningProcessor(int Id, bool add)
{
    for (auto proc : tprocessor) if (proc->getId() == Id) return proc;
    return add ? addTuningProcessor(Id) : nullptr;
}

TempoProcessor::Ptr Piano::getTempoProcessor(int Id, bool add)
{
    for (auto proc : mprocessor) if (proc->getId() == Id) return proc;
    return add ? addTempoProcessor(Id) : nullptr;
}

BlendronicProcessor::Ptr Piano::getBlendronicProcessor(int Id, bool add)
{
	for (auto proc : bprocessor) if (proc->getId() == Id) return proc;
	return add ? addBlendronicProcessor(Id) : nullptr;
}

Keymap::Ptr Piano::addKeymap(Keymap::Ptr km)
{
    keymaps.add(km);
    return km;
}

DirectProcessor::Ptr Piano::addDirectProcessor(int thisId)
{
    DirectProcessor::Ptr dproc = new DirectProcessor(processor.gallery->getDirect(thisId),
                                                     defaultT,
                                                     defaultBA,
                                                     &processor.mainPianoSynth,
                                                     &processor.resonanceReleaseSynth,
                                                     &processor.hammerReleaseSynth);
    dproc->prepareToPlay(sampleRate,
                         &processor.mainPianoSynth,
                         &processor.resonanceReleaseSynth,
                         &processor.hammerReleaseSynth);
    
    dprocessor.add(dproc);
    
    return dproc;
}

NostalgicProcessor::Ptr Piano::addNostalgicProcessor(int thisId)
{
    NostalgicProcessor::Ptr nproc = new NostalgicProcessor(processor.gallery->getNostalgic(thisId),
                                                           defaultT,
                                                           defaultS,
                                                           defaultBA,
                                                           &processor.mainPianoSynth);
    nproc->prepareToPlay(sampleRate, &processor.mainPianoSynth);
    nprocessor.add(nproc);
    
    return nproc;
}

TuningProcessor::Ptr Piano::addTuningProcessor(int thisId)
{
    TuningProcessor::Ptr tproc = new TuningProcessor(processor.gallery->getTuning(thisId));
    tproc->prepareToPlay(sampleRate);
    tprocessor.add(tproc);
    
    return tproc;
}

TempoProcessor::Ptr Piano::addTempoProcessor(int thisId)
{
    TempoProcessor::Ptr mproc = new TempoProcessor(processor.gallery->getTempo(thisId));
    mproc->prepareToPlay(sampleRate);
    mprocessor.add(mproc);

    return mproc;
}

BlendronicProcessor::Ptr Piano::addBlendronicProcessor(int thisId)
{
	BlendronicProcessor::Ptr bproc = new BlendronicProcessor(processor.gallery->getBlendronic(thisId),
                                                             defaultM,
                                                             processor.gallery->getGeneralSettings(),
                                                             &processor.mainPianoSynth);
	bproc->prepareToPlay(sampleRate);
	bprocessor.add(bproc);
    processor.mainPianoSynth.addBlendronicProcessor(bproc);

	return bproc;
}

void Piano::reset(void)
{
    configure();
}

bool Piano::containsProcessor(BKPreparationType thisType, int thisId)
{
    if (thisType == PreparationTypeKeymap)
    {
        return (getDirectProcessor(thisId) == nullptr) ? false : true;
    }
    else if (thisType == PreparationTypeDirect)
    {
        return (getDirectProcessor(thisId) == nullptr) ? false : true;
    }
    else if (thisType == PreparationTypeSynchronic)
    {
        return (getSynchronicProcessor(thisId) == nullptr) ? false : true;
    }
    else if (thisType == PreparationTypeNostalgic)
    {
        return (getNostalgicProcessor(thisId) == nullptr) ? false : true;
    }
    else if (thisType == PreparationTypeTuning)
    {
        return (getTuningProcessor(thisId) == nullptr) ? false : true;
    }
    else if (thisType == PreparationTypeTempo)
    {
        return (getTempoProcessor(thisId) == nullptr) ? false : true;
    }
    
    return false;
}

bool Piano::contains(Keymap::Ptr thisOne)
{
    for (auto p : keymaps) if (p->getId() == thisOne->getId()) return true;
    return false;
}

bool Piano::contains(DirectProcessor::Ptr thisOne)
{
    for (auto p : dprocessor) if (p->getId() == thisOne->getId()) return true;
    return false;
}

bool Piano::contains(SynchronicProcessor::Ptr thisOne)
{
    for (auto p : sprocessor) if (p->getId() == thisOne->getId()) return true;
    return false;
}

bool Piano::contains(NostalgicProcessor::Ptr thisOne)
{
    for (auto p : nprocessor) if (p->getId() == thisOne->getId()) return true;
    return false;
}

bool Piano::contains(BlendronicProcessor::Ptr thisOne)
{
    for (auto p : bprocessor) if (p->getId() == thisOne->getId()) return true;
    return false;
}

bool Piano::contains(TuningProcessor::Ptr thisOne)
{
    for (auto p : tprocessor) if (p->getId() == thisOne->getId()) return true;
    return false;
}

bool Piano::contains(TempoProcessor::Ptr thisOne)
{
    for (auto p : mprocessor) if (p->getId() == thisOne->getId()) return true;
    return false;
}

void Piano::addProcessor(BKPreparationType thisType, int thisId)
{
    if (thisType == PreparationTypeDirect)
    {
        addDirectProcessor(thisId);
    }
    else if (thisType == PreparationTypeSynchronic)
    {
        addSynchronicProcessor(thisId);
    }
    else if (thisType == PreparationTypeNostalgic)
    {
        addNostalgicProcessor(thisId);
    }
    else if (thisType == PreparationTypeTuning)
    {
        addTuningProcessor(thisId);
    }
    else if (thisType == PreparationTypeTempo)
    {
        addTempoProcessor(thisId);
    }
	else if (thisType == PreparationTypeBlendronic)
	{
		addBlendronicProcessor(thisId);
	}
}

bool Piano::contains(BKItem::Ptr thisItem)
{
    for (auto item : items) if (item == (BKItem *)thisItem) return true;
    
    return false;
}

void Piano::add(BKItem::Ptr item)
{
    bool added = items.addIfNotAlreadyThere(item);
    
    if (added)  configure();
}

void Piano::remove(BKItem::Ptr item)
{
    bool removed = false;
    for (int i = items.size(); --i >= 0; )
    {
        items[i]->print();

        if (items[i] == item)
        {
            items.remove(i);
            removed = true;
        }
    }
    
    if (removed) configure();
}

void Piano::linkPreparationWithTempo(BKPreparationType thisType, int thisId, Tempo::Ptr thisTempo)
{
    TempoProcessor::Ptr mproc = getTempoProcessor(thisTempo->getId());
    
    if (thisType == PreparationTypeSynchronic)
    {
        SynchronicProcessor::Ptr sproc = getSynchronicProcessor(thisId);
        
        sproc->setTempo(mproc);
    }
    else if (thisType == PreparationTypeBlendronic)
    {
        BlendronicProcessor::Ptr bproc = getBlendronicProcessor(thisId);
        
        bproc->setTempo(mproc);
    }
}

void Piano::linkNostalgicWithSynchronic(Nostalgic::Ptr nostalgic, Synchronic::Ptr synchronic)
{
    NostalgicProcessor::Ptr proc = getNostalgicProcessor(nostalgic->getId());
    
    proc->setSynchronic(getSynchronicProcessor(synchronic->getId()));
}

void Piano::linkPreparationWithTuning(BKPreparationType thisType, int thisId, Tuning::Ptr thisTuning)
{
    TuningProcessor::Ptr tproc = getTuningProcessor(thisTuning->getId());
    
    if (thisType == PreparationTypeDirect)
    {
        DirectProcessor::Ptr dproc = getDirectProcessor(thisId);
        
        dproc->setTuning(tproc);
    }
    else if (thisType == PreparationTypeSynchronic)
    {
        SynchronicProcessor::Ptr sproc = getSynchronicProcessor(thisId);
        
        sproc->setTuning(tproc);
    }
    else if (thisType == PreparationTypeNostalgic)
    {
        NostalgicProcessor::Ptr nproc = getNostalgicProcessor(thisId);
        
        nproc->setTuning(tproc);
    }
}

void Piano::linkPreparationWithBlendronic(BKPreparationType thisType, int thisId, Blendronic::Ptr thisBlend)
{
	BlendronicProcessor::Ptr bproc = getBlendronicProcessor(thisBlend->getId());

	if (thisType == PreparationTypeDirect)
	{
		DirectProcessor::Ptr dproc = getDirectProcessor(thisId);
		
		dproc->addBlendronic(bproc);
	}
	else if (thisType == PreparationTypeSynchronic)
	{
		SynchronicProcessor::Ptr sproc = getSynchronicProcessor(thisId);

		sproc->addBlendronic(bproc);
	}
	else if (thisType == PreparationTypeNostalgic)
	{
		NostalgicProcessor::Ptr nproc = getNostalgicProcessor(thisId);

		nproc->addBlendronic(bproc);
	}
}

void Piano::linkPreparationWithKeymap(BKPreparationType thisType, int thisId, int keymapId)
{
    Keymap::Ptr keymap = processor.gallery->getKeymap(keymapId);
    
    if (thisType == PreparationTypeDirect)
    {
        DirectProcessor::Ptr dproc = getDirectProcessor(thisId);
        
        keymap->addTarget(TargetTypeDirect);
    }
    else if (thisType == PreparationTypeSynchronic)
    {
        SynchronicProcessor::Ptr sproc = getSynchronicProcessor(thisId);
        
        keymap->addTarget(TargetTypeSynchronic);
        for (int i = TargetTypeSynchronic+1; i <= TargetTypeSynchronicRotate; i++)
        {
            keymap->addTarget((KeymapTargetType) i, TargetStateDisabled);
        }
    }
    else if (thisType == PreparationTypeNostalgic)
    {
        NostalgicProcessor::Ptr nproc = getNostalgicProcessor(thisId);
        
        keymap->addTarget(TargetTypeNostalgic);
    }
    else if (thisType == PreparationTypeBlendronic)
    {
        BlendronicProcessor::Ptr bproc = getBlendronicProcessor(thisId);
        
        for (int i = TargetTypeBlendronicPatternSync; i <= TargetTypeBlendronicOpenCloseOutput; i++)
        {
            keymap->addTarget((KeymapTargetType) i, TargetStateDisabled);
        }
    }
    else if (thisType == PreparationTypeTempo)
    {
        TempoProcessor::Ptr mproc = getTempoProcessor(thisId);
        
        keymap->addTarget(TargetTypeTempo);
    }
    else if (thisType == PreparationTypeTuning)
    {
        TuningProcessor::Ptr tproc = getTuningProcessor(thisId);
        
        keymap->addTarget(TargetTypeTuning);
    }
    linkKeymapToPreparation(keymapId, thisType, thisId);
}

void Piano::linkKeymapToPreparation(int keymapId, BKPreparationType thisType, int thisId)
{
    if (thisType == PreparationTypeDirect)
    {
        for (int i = 0; i < dprocessor.size(); ++i)
        {
            if (dprocessor[i]->getId() == thisId)
            {
                dprocessor[i]->addKeymap(getKeymap(keymapId));
            }
        }
    }
    else if (thisType == PreparationTypeSynchronic)
    {
        for (int i = 0; i < sprocessor.size(); ++i)
        {
            if (sprocessor[i]->getId() == thisId)
            {
                sprocessor[i]->addKeymap(getKeymap(keymapId));
            }
        }
    }
    else if (thisType == PreparationTypeNostalgic)
    {
        for (int i = 0; i < nprocessor.size(); ++i)
        {
            if (nprocessor[i]->getId() == thisId)
            {
                nprocessor[i]->addKeymap(getKeymap(keymapId));
            }
        }
    }
    else if (thisType == PreparationTypeBlendronic)
    {
        for (int i = 0; i < bprocessor.size(); ++i)
        {
            if (bprocessor[i]->getId() == thisId)
            {
                bprocessor[i]->addKeymap(getKeymap(keymapId));
            }
        }
    }
    else if (thisType == PreparationTypeTempo)
    {
        for (int i = 0; i < mprocessor.size(); ++i)
        {
            if (mprocessor[i]->getId() == thisId)
            {
                mprocessor[i]->addKeymap(getKeymap(keymapId));
            }
        }
    }
    else if (thisType == PreparationTypeTuning)
    {
        for (int i = 0; i < tprocessor.size(); ++i)
        {
            if (tprocessor[i]->getId() == thisId)
            {
                tprocessor[i]->addKeymap(getKeymap(keymapId));
            }
        }
    }
}

void Piano::configureDirectModification(DirectModification::Ptr mod, Array<int> whichKeymaps, Array<int> whichPreps)
{
    mod->setTargets(whichPreps);
    
    Array<bool> otherKeys;
    for (int i = 0; i < 128; i++) otherKeys.add(true);
    
    for (auto keymap : whichKeymaps)
    {
        for (auto key : processor.gallery->getKeymap(keymap)->keys())
        {
            modificationMap[key]->addDirectModification(mod);
        
            otherKeys.set(key, false);
        }
    }
    
    deconfigureDirectModificationForKeys(mod, otherKeys);
}

void Piano::deconfigureDirectModificationForKeys(DirectModification::Ptr mod, Array<bool> keys)
{
    int whichMod = mod->getId();

    for (int key = 0; key < 128; key++)
    {
        if (keys[key])
        {
            // Remove Modification from Key
            modificationMap[key]->removeDirectModification(whichMod);
        }
        
    }
}

void Piano::configureReset(BKItem::Ptr item)
{
    Array<bool> otherKeys;
    
    Array<int> whichKeymaps = item->getConnectionIdsOfType(PreparationTypeKeymap);
    
    Array<int> direct = item->getConnectionIdsOfType(PreparationTypeDirect);
    Array<int> nostalgic = item->getConnectionIdsOfType(PreparationTypeNostalgic);
    Array<int> synchronic = item->getConnectionIdsOfType(PreparationTypeSynchronic);
    Array<int> tempo = item->getConnectionIdsOfType(PreparationTypeTempo);
    Array<int> tuning = item->getConnectionIdsOfType(PreparationTypeTuning);
	Array<int> blendronic = item->getConnectionIdsOfType(PreparationTypeBlendronic);

    for (int i = 0; i < 128; i++) otherKeys.add(true);
    
    for (auto keymap : whichKeymaps)
    {
        for (auto key : processor.gallery->getKeymap(keymap)->keys())
        {
            for (auto id : direct) modificationMap[key]->directReset.add(id);
            
            for (auto id : synchronic) modificationMap[key]->synchronicReset.add(id);
            
            for (auto id : nostalgic) modificationMap[key]->nostalgicReset.add(id);
        
            for (auto id : tuning) modificationMap[key]->tuningReset.add(id);
            
            for (auto id : tempo) modificationMap[key]->tempoReset.add(id);

			for (auto id : blendronic) modificationMap[key]->blendronicReset.add(id);
            
            otherKeys.set(key, false);
        }
    }
    
    
    deconfigureResetForKeys(item, otherKeys);
    
}

void Piano::deconfigureResetForKeys(BKItem::Ptr item, Array<bool> otherKeys)
{
    Array<int> direct = item->getConnectionIdsOfType(PreparationTypeDirect);
    Array<int> nostalgic = item->getConnectionIdsOfType(PreparationTypeNostalgic);
    Array<int> synchronic = item->getConnectionIdsOfType(PreparationTypeSynchronic);
    Array<int> tempo = item->getConnectionIdsOfType(PreparationTypeTempo);
    Array<int> tuning = item->getConnectionIdsOfType(PreparationTypeTuning);
	Array<int> blendronic = item->getConnectionIdsOfType(PreparationTypeBlendronic);
    
    for (int key = 0; key < 128; key++)
    {
        if (otherKeys[key])
        {
            for (auto id : direct)
            {
                for (int i = modificationMap[key]->directReset.size(); --i>=0;)
                {
                    if (modificationMap[key]->directReset[i] == id) modificationMap[key]->directReset.remove(i);
                }
            }
            
            for (auto id : synchronic)
            {
                for (int i = modificationMap[key]->synchronicReset.size(); --i>=0;)
                {
                    if (modificationMap[key]->synchronicReset[i] == id) modificationMap[key]->synchronicReset.remove(i);
                }
            }
            
            for (auto id : nostalgic)
            {
                for (int i = modificationMap[key]->nostalgicReset.size(); --i>=0;)
                {
                    if (modificationMap[key]->nostalgicReset[i] == id) modificationMap[key]->nostalgicReset.remove(i);
                }
            }
            
            for (auto id : tuning)
            {
                for (int i = modificationMap[key]->tuningReset.size(); --i>=0;)
                {
                    if (modificationMap[key]->tuningReset[i] == id) modificationMap[key]->tuningReset.remove(i);
                }
            }
            
            for (auto id : tempo)
            {
                for (int i = modificationMap[key]->tempoReset.size(); --i>=0;)
                {
                    if (modificationMap[key]->tempoReset[i] == id) modificationMap[key]->tempoReset.remove(i);
                }
            }

			for (auto id : blendronic)
			{
				for (int i = modificationMap[key]->blendronicReset.size(); --i>0;)
				{
					if (modificationMap[key]->blendronicReset[i] == id) modificationMap[key]->blendronicReset.remove(i);
				}
			}
        }        
    }
}

void Piano::configurePianoMap(BKItem::Ptr map)
{
    int pianoTarget = map->getPianoTarget();
    
    if (pianoTarget == getId()) return;
    
    Array<int> keymaps = map->getConnectionIdsOfType(PreparationTypeKeymap);
    
    for (auto keymap : keymaps)
    {
        Keymap::Ptr thisKeymap = processor.gallery->getKeymap(keymap);
        for (auto key : thisKeymap->keys())
        {
            pianoMap.set(key, pianoTarget);
        }
    }
}

void Piano::configureModification(BKItem::Ptr map)
{
    map->print();
    
    BKPreparationType modType = map->getType();
    BKPreparationType targetType = modToPrepType(modType);
    int Id = map->getId();
    
    Array<int> whichPreps = map->getConnectionIdsOfType(targetType);
    
    Array<int> whichKeymaps = map->getConnectionIdsOfType(PreparationTypeKeymap);
    
    DBG("keymaps: " + intArrayToString(whichKeymaps) + " preps: " + intArrayToString(whichPreps));
    
    if (modType == BKPreparationTypeNil) return;
    else if (modType == PreparationTypeDirectMod)
    {
        configureDirectModification(processor.gallery->getDirectModification(Id), whichKeymaps, whichPreps);
    }
    else if (modType == PreparationTypeSynchronicMod)
    {
        configureSynchronicModification(processor.gallery->getSynchronicModification(Id), whichKeymaps, whichPreps);
    }
    else if (modType == PreparationTypeNostalgicMod)
    {
        configureNostalgicModification(processor.gallery->getNostalgicModification(Id), whichKeymaps, whichPreps);
    }
    else if (modType == PreparationTypeTuningMod)
    {
        configureTuningModification(processor.gallery->getTuningModification(Id), whichKeymaps, whichPreps);
    }
    else if (modType == PreparationTypeTempoMod)
    {
        configureTempoModification(processor.gallery->getTempoModification(Id), whichKeymaps, whichPreps);
    }
	else if (modType == PreparationTypeBlendronicMod)
	{
		configureBlendronicModification(processor.gallery->getBlendronicModification(Id), whichKeymaps, whichPreps);
	}

}

void Piano::configureNostalgicModification(NostalgicModification::Ptr mod, Array<int> whichKeymaps, Array<int> whichPreps)
{
    mod->setTargets(whichPreps);
    
    Array<bool> otherKeys;
    for (int i = 0; i < 128; i++) otherKeys.add(true);
    
    for (auto keymap : whichKeymaps)
    {
        for (auto key : processor.gallery->getKeymap(keymap)->keys())
        {
            modificationMap[key]->addNostalgicModification(mod);
            
            otherKeys.set(key, false);
        }
    }
    
    deconfigureNostalgicModificationForKeys(mod, otherKeys);
}

void Piano::deconfigureNostalgicModificationForKeys(NostalgicModification::Ptr mod, Array<bool> keys)
{
    int whichMod = mod->getId();
    
    for (int key = 0; key < 128; key++)
    {
        if (keys[key])
        {
            // Remove Modification from Key
            modificationMap[key]->removeNostalgicModification(whichMod);
        }
    }
}

void Piano::configureSynchronicModification(SynchronicModification::Ptr mod, Array<int> whichKeymaps, Array<int> whichPreps)
{
    mod->setTargets(whichPreps);
    
    Array<bool> otherKeys;
    for (int i = 0; i < 128; i++) otherKeys.add(true);
    
    for (auto keymap : whichKeymaps)
    {
        for (auto key : processor.gallery->getKeymap(keymap)->keys())
        {
            modificationMap[key]->addSynchronicModification(mod);
            
            otherKeys.set(key,false);
        }
    }
    
    deconfigureSynchronicModificationForKeys(mod, otherKeys);
}

void Piano::deconfigureSynchronicModificationForKeys(SynchronicModification::Ptr mod, Array<bool> keys)
{
    int whichMod = mod->getId();

    for (int key = 0; key < 128; key++)
    {
        if (keys[key])
        {
            // Remove Modification from Key
            modificationMap[key]->removeSynchronicModification(whichMod);
        }
    }
}

void Piano::configureTempoModification(TempoModification::Ptr mod, Array<int> whichKeymaps, Array<int> whichPreps)
{
    mod->setTargets(whichPreps);
    
    Array<bool> otherKeys;
    for (int i = 0; i < 128; i++) otherKeys.add(true);
    
    for (auto keymap : whichKeymaps)
    {
        for (auto key : processor.gallery->getKeymap(keymap)->keys())
        {
            modificationMap[key]->addTempoModification(mod);
            
            otherKeys.set(key, false);
        }
    }
    
    deconfigureTempoModificationForKeys(mod, otherKeys);
}

void Piano::deconfigureTempoModificationForKeys(TempoModification::Ptr mod, Array<bool> keys)
{
    int whichMod = mod->getId();
    
    for (int key = 0; key < 128; key++)
    {
        if (keys[key])
        {
            // Remove Modification from Key
            modificationMap[key]->removeTempoModification(whichMod);
        
            //DBG("REMOVE whichmod: " + String(whichMod) + " FROM key: " +String(key));
        }
    }
}

void Piano::configureBlendronicModification(BlendronicModification::Ptr mod, Array<int> whichKeymaps, Array<int> whichPreps)
{
	mod->setTargets(whichPreps);

	Array<bool> otherKeys;
	for (int i = 0; i < 128; i++) otherKeys.add(true);

	for (auto keymap : whichKeymaps)
	{
		for (auto key : processor.gallery->getKeymap(keymap)->keys())
		{
			modificationMap[key]->addBlendronicModification(mod);

			otherKeys.set(key, false);
		}
	}

	deconfigureBlendronicModificationForKeys(mod, otherKeys);
}

void Piano::deconfigureBlendronicModificationForKeys(BlendronicModification::Ptr mod, Array<bool> keys)
{
	int whichMod = mod->getId();

	for (int key = 0; key < 128; key++)
	{
		if (keys[key])
		{
			// Remove Modification from Key
			modificationMap[key]->removeBlendronicModification(whichMod);

			//DBG("REMOVE whichmod: " + String(whichMod) + " FROM key: " +String(key));
		}
	}
}

void Piano::configureTuningModification(TuningModification::Ptr mod, Array<int> whichKeymaps, Array<int> whichPreps)
{
    mod->setTargets(whichPreps);
    
    Array<bool> otherKeys;
    for (int i = 0; i < 128; i++) otherKeys.add(true);
    
    for (auto keymap : whichKeymaps)
    {
        for (auto key : processor.gallery->getKeymap(keymap)->keys())
        {
            modificationMap[key]->addTuningModification(mod);
            
            otherKeys.set(key, false);
        }
    }

    deconfigureTuningModificationForKeys(mod, otherKeys);
}

void Piano::deconfigureTuningModificationForKeys(TuningModification::Ptr mod, Array<bool> keys)
{
    int whichMod = mod->getId();
    
    for (int key = 0; key < 128; key++)
    {
        if (keys[key])
        {
            // Remove Modification from Key
            modificationMap[key]->removeTuningModification(whichMod);
            
            DBG("REMOVE whichmod: " + String(whichMod) + " FROM key: " + String(key));
        }
        
    }
}

//not sure why some of these have Channel and some don't; should rectify?
void Piano::keyPressed(int noteNumber, float velocity, int channel, bool soundfont, String source)
{
    // These 2 arrays will represent the targets of the pressed note. They will be set
    // by checking each keymap that contains the note and enabling each of those keymaps'
    // targets in these arrays. Check the bprocessor loop below for further explanation.
    // We need both pressTargetStates and releaseTargetStates because keymap inversion
    // and the ability to connect multiple keymaps to a preparation means one keypress
    // can result in both a press and a release being sent to a preparation.
    Array<KeymapTargetState> pressTargetStates;
    Array<KeymapTargetState> releaseTargetStates;
    
    // This will just act as a reference to either pressTargetStates of releaseTargetStates
    // depending on whether a keymap is inverted
    Array<KeymapTargetState>* targetStates;
    
    // Initialize the target states as being disabled
    pressTargetStates.ensureStorageAllocated(TargetTypeNil);
    releaseTargetStates.ensureStorageAllocated(TargetTypeNil);
    for (int i = 0; i < TargetTypeNil; i++)
    {
        pressTargetStates.add(TargetStateNil);
        releaseTargetStates.add(TargetStateNil);
    }
    
    bool foundReattack = false;
    bool foundSustain = false;
    for (auto km : keymaps)
    {
        if (km->getAllMidiInputSources().contains(source))
        {
            if (km->getMidiEdit())
            {
                km->toggleNote(noteNumber);
            }
            if (km->containsNote(noteNumber))
            {
                if (km->isInverted()) foundSustain = true;
                else foundReattack = true;
            }
        }
    }
    if (foundSustain) sustain(noteNumber, velocity, channel, soundfont);
    if (foundReattack) reattack(noteNumber);
    
    for (auto proc : bprocessor)
    {
        // For each keymap
        for (auto km : proc->getKeymaps())
        {
            // First check that the the keymap contain the pressed note and uses the midi source of the note
            if (km->containsNote(noteNumber) && km->getAllMidiInputSources().contains(source))
            {
                // targetStates will refer to press or release depending on inversion
                targetStates = &pressTargetStates;
                if (km->isInverted()) targetStates = &releaseTargetStates;
                
                // If the keymap has a target enabled, enable that target in targetStates
                for (int i = TargetTypeBlendronicPatternSync; i <= TargetTypeBlendronicOpenCloseOutput; i++)
                {
                    if (km->getTargetStates()[i] == TargetStateEnabled)
                        targetStates->set(i, TargetStateEnabled);
                }
            }
        }
        // If there are any targets enabled, do the press and/or release
        if (pressTargetStates.contains(TargetStateEnabled)) {
            proc->keyPressed(noteNumber, velocity, channel, pressTargetStates);
            pressTargetStates.fill(TargetStateNil); /* reset for the next processor */}
        if (releaseTargetStates.contains(TargetStateEnabled)) {
            proc->keyReleased(noteNumber, velocity, channel, releaseTargetStates);
            releaseTargetStates.fill(TargetStateNil); /* reset for the next processor */}
    }
    
    for (auto proc : tprocessor)
    {
        for (auto km : proc->getKeymaps())
        {
            if (km->containsNote(noteNumber) && km->getAllMidiInputSources().contains(source))
            {
                targetStates = &pressTargetStates;
                if (km->isInverted()) targetStates = &releaseTargetStates;
                
                if (km->getTargetStates()[TargetTypeTuning] == TargetStateEnabled)
                    targetStates->set(TargetTypeTuning, TargetStateEnabled);
            }
        }
        if (pressTargetStates.contains(TargetStateEnabled)) {
            proc->keyPressed(noteNumber);
            pressTargetStates.fill(TargetStateNil); }
        if (releaseTargetStates.contains(TargetStateEnabled)) {
            proc->keyReleased(noteNumber);
            releaseTargetStates.fill(TargetStateNil); }
    }
    
    for (auto proc : dprocessor)
    {
        for (auto km : proc->getKeymaps())
        {
            if (km->containsNote(noteNumber) && km->getAllMidiInputSources().contains(source))
            {
                targetStates = &pressTargetStates;
                if (km->isInverted()) targetStates = &releaseTargetStates;
                
                if (km->getTargetStates()[TargetTypeDirect] == TargetStateEnabled)
                    targetStates->set(TargetTypeDirect, TargetStateEnabled);
            }
        }
        if (releaseTargetStates.contains(TargetStateEnabled)) {
            proc->playReleaseSample(noteNumber, velocity, channel, soundfont);
            proc->keyReleased(noteNumber, velocity, channel, soundfont);
            releaseTargetStates.fill(TargetStateNil); }
        if (pressTargetStates.contains(TargetStateEnabled)) {
            proc->keyPressed(noteNumber, velocity, channel);
            pressTargetStates.fill(TargetStateNil); }
    }
    
    for (auto proc : sprocessor)
    {
        for (auto km : proc->getKeymaps())
        {
            if (km->containsNote(noteNumber) && km->getAllMidiInputSources().contains(source))
            {
                targetStates = &pressTargetStates;
                if (km->isInverted()) targetStates = &releaseTargetStates;
                
                for (int i = TargetTypeSynchronic; i <= TargetTypeSynchronicRotate; i++)
                {
                    if (km->getTargetStates()[i] == TargetStateEnabled)
                        targetStates->set(i, TargetStateEnabled);
                }
            }
        }
        if (pressTargetStates.contains(TargetStateEnabled)) {
            proc->keyPressed(noteNumber, velocity, pressTargetStates);
            pressTargetStates.fill(TargetStateNil); }
        if (releaseTargetStates.contains(TargetStateEnabled)) {
            proc->keyReleased(noteNumber, velocity, channel, releaseTargetStates);
            releaseTargetStates.fill(TargetStateNil); }
    }
    
    for (auto proc : nprocessor)
    {
        for (auto km : proc->getKeymaps())
        {
            if (km->containsNote(noteNumber) && km->getAllMidiInputSources().contains(source))
            {
                targetStates = &pressTargetStates;
                if (km->isInverted()) targetStates = &releaseTargetStates;
                
                if (km->getTargetStates()[TargetTypeNostalgic] == TargetStateEnabled)
                    targetStates->set(TargetTypeNostalgic, TargetStateEnabled);
            }
        }
        if (pressTargetStates.contains(TargetStateEnabled)) {
            proc->keyPressed(noteNumber, velocity, channel);
            pressTargetStates.fill(TargetStateNil); }
        if (releaseTargetStates.contains(TargetStateEnabled)) {
            proc->keyReleased(noteNumber, velocity, channel);
            releaseTargetStates.fill(TargetStateNil); }
    }
    
    for (auto proc : mprocessor)
    {
        for (auto km : proc->getKeymaps())
        {
            if (km->containsNote(noteNumber) && km->getAllMidiInputSources().contains(source))
            {
                targetStates = &pressTargetStates;
                if (km->isInverted()) targetStates = &releaseTargetStates;
                
                if (km->getTargetStates()[TargetTypeTempo] == TargetStateEnabled)
                    targetStates->set(TargetTypeTempo, TargetStateEnabled);
            }
        }
        if (pressTargetStates.contains(TargetStateEnabled)) {
            proc->keyPressed(noteNumber, velocity);
            pressTargetStates.fill(TargetStateNil); }
        if (releaseTargetStates.contains(TargetStateEnabled)) {
            proc->keyReleased(noteNumber, channel);
            releaseTargetStates.fill(TargetStateNil); }
    }
    // PERFORM MODIFICATION STUFF
}


void Piano::keyReleased(int noteNumber, float velocity, int channel, bool soundfont, String source)
{
    
    //DBG("Piano::keyReleased : " + String(noteNumber));
    
    Array<KeymapTargetState> pressTargetStates;
    Array<KeymapTargetState> releaseTargetStates;
    Array<KeymapTargetState>* targetStates;
    pressTargetStates.ensureStorageAllocated(TargetTypeNil);
    releaseTargetStates.ensureStorageAllocated(TargetTypeNil);
    for (int i = 0; i < TargetTypeNil; i++)
    {
        pressTargetStates.add(TargetStateNil);
        releaseTargetStates.add(TargetStateNil);
    }
    
    bool foundReattack = false;
    bool foundSustain = false;
    for (auto km : keymaps)
    {
        if (km->containsNote(noteNumber) && km->getAllMidiInputSources().contains(source))
        {
            if (km->isInverted()) foundReattack = true;
            else foundSustain = true;
        }
    }
    if (foundSustain) sustain(noteNumber, velocity, channel, soundfont);
    if (foundReattack) reattack(noteNumber);
    
    for (auto proc : dprocessor)
    {
        for (auto km : proc->getKeymaps())
        {
            if (km->containsNote(noteNumber) && km->getAllMidiInputSources().contains(source))
            {
                targetStates = &releaseTargetStates;
                if (km->isInverted()) targetStates = &pressTargetStates;
                
                if (km->getTargetStates()[TargetTypeDirect] == TargetStateEnabled)
                    targetStates->set(TargetTypeDirect, TargetStateEnabled);
            }
        }
        if (pressTargetStates.contains(TargetStateEnabled)) {
            proc->keyPressed(noteNumber, velocity, channel);
            pressTargetStates.fill(TargetStateNil); }
        if (releaseTargetStates.contains(TargetStateEnabled)) {
            proc->playReleaseSample(noteNumber, velocity, channel, soundfont);
            proc->keyReleased(noteNumber, velocity, channel, soundfont);
            releaseTargetStates.fill(TargetStateNil); }
    }
    
    for (auto proc : tprocessor)
    {
        for (auto km : proc->getKeymaps())
        {
            if (km->containsNote(noteNumber) && km->getAllMidiInputSources().contains(source))
            {
                targetStates = &releaseTargetStates;
                if (km->isInverted()) targetStates = &pressTargetStates;
                
                if (km->getTargetStates()[TargetTypeTuning] == TargetStateEnabled)
                    targetStates->set(TargetTypeTuning, TargetStateEnabled);
            }
        }
        if (pressTargetStates.contains(TargetStateEnabled)) {
            proc->keyPressed(noteNumber);
            pressTargetStates.fill(TargetStateNil); }
        if (releaseTargetStates.contains(TargetStateEnabled)) {
            proc->keyReleased(noteNumber);
            releaseTargetStates.fill(TargetStateNil); }
    }
    
    for (auto proc : sprocessor)
    {
        for (auto km : proc->getKeymaps())
        {
            if (km->containsNote(noteNumber) && km->getAllMidiInputSources().contains(source))
            {
                targetStates = &releaseTargetStates;
                if (km->isInverted()) targetStates = &pressTargetStates;
                
                for (int i = TargetTypeSynchronic; i <= TargetTypeSynchronicRotate; i++)
                {
                    if (km->getTargetStates()[i] == TargetStateEnabled)
                        targetStates->set(i, TargetStateEnabled);
                }
            }
        }
        if (pressTargetStates.contains(TargetStateEnabled)) {
            proc->keyPressed(noteNumber, velocity, pressTargetStates);
            pressTargetStates.fill(TargetStateNil); }
        if (releaseTargetStates.contains(TargetStateEnabled)) {
            proc->keyReleased(noteNumber, velocity, channel, releaseTargetStates);
            releaseTargetStates.fill(TargetStateNil); }
    }
    
    for (auto proc : nprocessor)
    {
        for (auto km : proc->getKeymaps())
        {
            if (km->containsNote(noteNumber) && km->getAllMidiInputSources().contains(source))
            {
                targetStates = &releaseTargetStates;
                if (km->isInverted()) targetStates = &pressTargetStates;
                
                if (km->getTargetStates()[TargetTypeNostalgic] == TargetStateEnabled)
                    targetStates->set(TargetTypeNostalgic, TargetStateEnabled);
            }
        }
        if (pressTargetStates.contains(TargetStateEnabled)) {
            proc->keyPressed(noteNumber, velocity, channel);
            pressTargetStates.fill(TargetStateNil); }
        if (releaseTargetStates.contains(TargetStateEnabled)) {
            proc->keyReleased(noteNumber, velocity, channel);
            releaseTargetStates.fill(TargetStateNil); }
    }
    
    for (auto proc : bprocessor)
    {
        for (auto km : proc->getKeymaps())
        {
            if (km->containsNote(noteNumber) && km->getAllMidiInputSources().contains(source))
            {
                targetStates = &releaseTargetStates;
                if (km->isInverted()) targetStates = &pressTargetStates;
                
                for (int i = TargetTypeBlendronicPatternSync; i <= TargetTypeBlendronicOpenCloseOutput; i++)
                {
                    if (km->getTargetStates()[i] == TargetStateEnabled)
                        targetStates->set(i, TargetStateEnabled);
                }
            }
        }
        if (pressTargetStates.contains(TargetStateEnabled)) {
            proc->keyPressed(noteNumber, velocity, channel, pressTargetStates);
            pressTargetStates.fill(TargetStateNil); }
        if (releaseTargetStates.contains(TargetStateEnabled)) {
            proc->keyReleased(noteNumber, velocity, channel, releaseTargetStates);
            releaseTargetStates.fill(TargetStateNil); }
    }
    
    for (auto proc : mprocessor)
    {
        for (auto km : proc->getKeymaps())
        {
            if (km->containsNote(noteNumber) && km->getAllMidiInputSources().contains(source))
            {
                targetStates = &releaseTargetStates;
                if (km->isInverted()) targetStates = &pressTargetStates;
                
                if (km->getTargetStates()[TargetTypeTempo] == TargetStateEnabled)
                    targetStates->set(TargetTypeTempo, TargetStateEnabled);
            }
        }
        if (pressTargetStates.contains(TargetStateEnabled)) {
            proc->keyPressed(noteNumber, velocity);
            pressTargetStates.fill(TargetStateNil); }
        if (releaseTargetStates.contains(TargetStateEnabled)) {
            proc->keyReleased(noteNumber, channel);
            releaseTargetStates.fill(TargetStateNil); }
    }
}



void Piano::postRelease(int noteNumber, float velocity, int channel, String source)
{
    DBG("Piano::postRelease " + String(noteNumber));
    
    Array<KeymapTargetState> targetStates;
    targetStates.ensureStorageAllocated(TargetStateNil);
    for (int i = 0; i < TargetTypeNil; i++)
    {
        targetStates.add(TargetStateNil);
    }
    
    for (auto km : keymaps)
    {
        if (km->containsNote(noteNumber))
        {
            for (auto state : km->getTargetStates()) if (state == TargetStateEnabled) targetStates = state;
        }
    }
    
    if(sustainPedalIsDepressed && targetStates.contains(TargetStateEnabled))
    {
        SustainedNote newNote;
        newNote.noteNumber = noteNumber;
        newNote.velocity = velocity;
        newNote.channel = channel;
        DBG("storing sustained note " + String(noteNumber));
        
        sustainedNotes.add(newNote);
    }
    
    if (targetStates.contains(TargetStateEnabled))
    {
        for (auto proc : dprocessor)
        {
            if (!sustainPedalIsDepressed) proc->keyReleased(noteNumber, velocity, channel);
            //proc->keyReleased(noteNumber, velocity, channel);
        }
        
        for (auto proc : tprocessor)
        {
            if (!sustainPedalIsDepressed) proc->keyReleased(noteNumber);
        }
        
        for (auto proc : nprocessor)
        {
            if (!sustainPedalIsDepressed) proc->keyReleased(noteNumber, velocity, true);
        }
        
        for (auto proc : mprocessor)
        {
            proc->keyReleased(noteNumber, velocity);
        }
    }
}

void Piano::reattack(int noteNumber)
{
    if(sustainPedalIsDepressed)
    {
        //DBG("removing sustained note " + String(noteNumber));
        
        for(int i=0; i<sustainedNotes.size(); i++)
        {
            if(sustainedNotes.getUnchecked(i).noteNumber == noteNumber)
                sustainedNotes.remove(i);
        }
    }
}

void Piano::sustain(int noteNumber, float velocity, int channel, bool soundfont)
{
    if(sustainPedalIsDepressed)
    {
        SustainedNote newNote;
        newNote.noteNumber = noteNumber;
        newNote.velocity = velocity;
        newNote.channel = channel;
        //DBG("storing sustained note " + String(noteNumber));
        
        sustainedNotes.add(newNote);
        
        if (!soundfont)
        {
            //play hammers and resonance when keys are released, even with pedal down
            for (auto proc : dprocessor)
            {
                proc->playReleaseSample(noteNumber, velocity, channel);
            }
        }
    }
}


void Piano::clearKey(int noteNumber)
{
    if(sustainPedalIsDepressed)
    {
        for(int i=0; i<sustainedNotes.size(); i++)
        {
            if(sustainedNotes.getUnchecked(i).noteNumber == noteNumber)
                sustainedNotes.remove(i);
        }
    }
}

void Piano::sustainPedalReleased(Array<bool> keysThatAreDepressed, bool post)
{
    sustainPedalIsDepressed = false;
    
    Array<KeymapTargetState> targetStates;
    targetStates.ensureStorageAllocated(TargetStateNil);
    for (int i = 0; i < TargetTypeNil; i++)
    {
        targetStates.add(TargetStateNil);
    }
    
    //do all keyReleased calls now
    for(int n=0; n<sustainedNotes.size(); n++)
    {
        SustainedNote releaseNote = sustainedNotes.getUnchecked(n);
        
        DBG(releaseNote.noteNumber);
        
        targetStates.fill(TargetStateNil);
        
        for (auto km : keymaps)
        {
            if (km->containsNote(releaseNote.noteNumber))
            {
                for (auto state : km->getTargetStates()) if (state == TargetStateEnabled) targetStates = state;
            }
        }
        
        for (auto proc : dprocessor)
        {
            if(!keysThatAreDepressed.getUnchecked(releaseNote.noteNumber)) //don't turn off note if key is down!
                proc->keyReleased(releaseNote.noteNumber, releaseNote.velocity, releaseNote.channel);
        }
        
        for (auto proc : tprocessor)
        {
            proc->keyReleased(releaseNote.noteNumber);
        }
        
        for (auto proc : sprocessor)
        {
            proc->keyReleased(releaseNote.noteNumber, releaseNote.velocity, releaseNote.channel, targetStates);
        }
        
        for (auto proc : nprocessor)
        {
            //DBG("nostalgic sustainPedalReleased " + String((int)post));
            proc->keyReleased(releaseNote.noteNumber, releaseNote.channel, post);
        }
        
        for (auto proc : bprocessor)
        {
            proc->keyReleased(releaseNote.noteNumber, releaseNote.velocity, releaseNote.channel, targetStates);
        }
    }
    
    sustainedNotes.clearQuick();
}

void Piano::sustainPedalReleased(bool post)
{
    sustainPedalIsDepressed = false;
    
    Array<KeymapTargetState> targetStates;
    targetStates.ensureStorageAllocated(TargetStateNil);
    for (int i = 0; i < TargetTypeNil; i++)
    {
        targetStates.add(TargetStateNil);
    }
    
    //do all keyReleased calls now
    for(int n=0; n<sustainedNotes.size(); n++)
    {
        SustainedNote releaseNote = sustainedNotes.getUnchecked(n);
        
        targetStates.fill(TargetStateNil);
        
        for (auto km : keymaps)
        {
            if (km->containsNote(releaseNote.noteNumber))
            {
                for (auto state : km->getTargetStates()) if (state == TargetStateEnabled) targetStates = state;
            }
        }
        
        for (auto proc : dprocessor)
        {
            proc->keyReleased(releaseNote.noteNumber, releaseNote.velocity, releaseNote.channel);
        }
        
        for (auto proc : tprocessor)
        {
            proc->keyReleased(releaseNote.noteNumber);
        }
        
        for (auto proc : sprocessor)
        {
            proc->keyReleased(releaseNote.noteNumber, releaseNote.velocity, releaseNote.channel, targetStates);
        }
        
        for (auto proc : nprocessor)
        {
            //DBG("nostalgic sustainPedalReleased " + String((int)post));
            proc->keyReleased(releaseNote.noteNumber, releaseNote.channel, post);
        }
        
        for (auto proc : bprocessor)
        {
            proc->keyReleased(releaseNote.noteNumber, releaseNote.velocity, releaseNote.channel, targetStates);
        }
    }
    
    sustainedNotes.clearQuick();
}

void Piano::processBlock(AudioSampleBuffer& buffer, int numSamples, int midiChannel, BKSampleLoadType type, bool onlyNostalgic)
{
    sampleType = type;
    if(onlyNostalgic) {
        for (auto nproc : nprocessor)
            nproc->processBlock(numSamples, midiChannel, sampleType);
    }
    
    else
    {
        for (auto dproc : dprocessor)
        {
            dproc->processBlock(numSamples, midiChannel, sampleType);
        }
        
        for (auto sproc : sprocessor)
        {
            sproc->processBlock(numSamples, midiChannel, sampleType);
        }
        
        for (auto nproc : nprocessor)
        {
            nproc->processBlock(numSamples, midiChannel, sampleType);
        }
        
        for (auto tproc : tprocessor)
            tproc->processBlock(numSamples);
        
        for (auto mproc : mprocessor)
            mproc->processBlock(numSamples, midiChannel);
        
        for (auto bproc : bprocessor)
            bproc->processBlock(numSamples, midiChannel);
    }
}


void Piano::prepareToPlay(double sr)
{
    sampleRate = sr;

    for (auto dproc : dprocessor)
        dproc->prepareToPlay(sampleRate, &processor.mainPianoSynth, &processor.resonanceReleaseSynth, &processor.hammerReleaseSynth);
    
    for (auto mproc : mprocessor)
        mproc->prepareToPlay(sampleRate);
    
    for (auto nproc : nprocessor)
        nproc->prepareToPlay(sampleRate, &processor.mainPianoSynth);
    
    for (auto sproc : sprocessor)
        sproc->prepareToPlay(sampleRate, &processor.mainPianoSynth);
    
    for (auto tproc : tprocessor)
        tproc->prepareToPlay(sampleRate);

	for (auto bproc : bprocessor)
		bproc->prepareToPlay(sampleRate);
}

ValueTree Piano::getState(void)
{
    ValueTree pianoVT( vtagPiano);
    
    String name = getName();
    pianoVT.setProperty("name", ((name == String()) ? "Piano"+String(Id) : name) , 0);
    
    pianoVT.setProperty("Id", Id, 0);
    
    for (auto item : items)
    {
        BKPreparationType type = item->getType();
        
        ValueTree itemVT("item");
        ValueTree connectionsVT("connections");
        
        if (type == PreparationTypeKeymap)
        {
            itemVT.addChild(item->getState(), -1, 0);
            
            for (auto target : item->getConnections())
            {
                connectionsVT.addChild(target->getState(), -1, 0);
            }
            
            itemVT.addChild(connectionsVT, -1, 0);
            pianoVT.addChild(itemVT, -1, 0);
        }
        else if (type == PreparationTypeTuning)
        {
            itemVT.addChild(item->getState(), -1, 0);
            
            // Look for synchronic, direct, blendronic, and nostalgic targets
            for (auto target : item->getConnections())
            {
                BKPreparationType targetType = target->getType();
                
                if ((targetType >= PreparationTypeDirect && targetType <= PreparationTypeNostalgic))
                {
                    connectionsVT.addChild(target->getState(), -1, 0);
                }
            }
            
            itemVT.addChild(connectionsVT, -1, 0);
            pianoVT.addChild(itemVT, -1, 0);
        }
        else if (type == PreparationTypeTempo)
        {
            itemVT.addChild(item->getState(), -1, 0);
            
            // Look for synchronic targets
            for (auto target : item->getConnections())
            {
                BKPreparationType targetType = target->getType();
                
                if (targetType == PreparationTypeSynchronic || targetType == PreparationTypeBlendronic  )
                {
                    connectionsVT.addChild(target->getState(), -1, 0);
                }
            }
            
            itemVT.addChild(connectionsVT, -1, 0);
            pianoVT.addChild(itemVT, -1, 0);
        }
        else if (type == PreparationTypeSynchronic)
        {
            itemVT.addChild(item->getState(), -1, 0);
            
            // Look for nostalgic targets
            for (auto target : item->getConnections())
            {
                BKPreparationType targetType = target->getType();
                
                if (targetType == PreparationTypeNostalgic)
                {
                    connectionsVT.addChild(target->getState(), -1, 0);
                }
            }
            
            itemVT.addChild(connectionsVT, -1, 0);
            pianoVT.addChild(itemVT, -1, 0);
        }
		else if (type == PreparationTypeBlendronic)
		{
			itemVT.addChild(item->getState(), -1, 0);
			//look for direct, nostalgic, and synchronic targets
			for (auto target : item->getConnections())
			{
				BKPreparationType targetType = target->getType();

				if ((targetType >= PreparationTypeDirect && targetType <= PreparationTypeNostalgic) ||
                    targetType == PreparationTypeBlendronicMod)
				{
					connectionsVT.addChild(target->getState(), -1, 0);
				}
			}
			itemVT.addChild(connectionsVT, -1, 0);
			pianoVT.addChild(itemVT, -1, 0);
		}
        else if ((type >= PreparationTypeDirectMod && type <= PreparationTypeTempoMod) ||
                 type == PreparationTypeReset ||
                 type == PreparationTypeBlendronicMod)
        {
            itemVT.addChild(item->getState(), -1, 0);
            
            // Look for non-Keymap connections
            for (auto target : item->getConnections())
            {
                BKPreparationType targetType = target->getType();
                
                if (targetType != PreparationTypeKeymap)
                {
                    connectionsVT.addChild(target->getState(), -1, 0);
                }
            }
            
            itemVT.addChild(connectionsVT, -1, 0);
            pianoVT.addChild(itemVT, -1, 0);
        }
        else if (type == PreparationTypeComment)
        {
            itemVT.addChild(item->getState(), -1, 0);
            
            pianoVT.addChild(itemVT, -1, 0);
        }
    }
    
    return pianoVT;
}
#define LOAD_VERSION 0

void Piano::setState(XmlElement* e, OwnedArray<HashMap<int,int>>* idmap)
{
    int i = 0;
    
    //Id = e->getStringAttribute("Id").getIntValue();
    //setId(e->getStringAttribute("Id").getIntValue());
    
    String pianoName = e->getStringAttribute("name");
    
    if (pianoName != String()) setName(pianoName);
    
    BKItem::Ptr thisItem;
    BKItem::Ptr thisConnection;

    forEachXmlChildElement (*e, group)
    {
        forEachXmlChildElement(*group, item)
        {
            if (item->getTagName() == "item")
            {
                i = item->getStringAttribute("type").getIntValue();
                BKPreparationType type = (BKPreparationType) i;
                
                if (type == PreparationTypeComment)
                {
                    thisItem = new BKItem(PreparationTypeComment, -1, processor);
                    
                    thisItem->setItemName("Comment");
                    
                    i =  (int) atof(item->getStringAttribute("X").getCharPointer());
                    int x = i;
                    
                    i =  (int) atof(item->getStringAttribute("Y").getCharPointer());
                    int y = i;
                    
                    i =  (int) atof(item->getStringAttribute("W").getCharPointer());
                    int w = i;
                    
                    i =  (int) atof(item->getStringAttribute("H").getCharPointer());
                    int h = i;
                    
                    String s = item->getStringAttribute("text");
                    thisItem->setCommentText(s);
                    
                    thisItem->setSize(w, h);
                    thisItem->setCentrePosition(x, y);
                    
                    items.add(thisItem);
                }
                else
                {
                    int oldId = item->getStringAttribute("Id").getIntValue();
                    
                    int thisId = oldId;
                    
                    if (idmap->getUnchecked(type)->contains(oldId))
                    {
                        thisId = idmap->getUnchecked(type)->getReference(oldId);
                    }
                
                    i = item->getStringAttribute("piano").getIntValue();
                    int piano = i;
                    
                    if (idmap->getUnchecked(PreparationTypePiano)->contains(piano))
                    {
                        piano = idmap->getUnchecked(PreparationTypePiano)->getReference(piano);
                    }
                    
                    thisItem = itemWithTypeAndId(type, thisId);
                    
                    if (thisItem == nullptr)
                    {
                        thisItem = new BKItem(type, thisId, processor);
                        
                        thisItem->setPianoTarget(piano);
                        thisItem->setItemName(item->getStringAttribute("name"));
                        
                        i =  (int) atof(item->getStringAttribute("X").getCharPointer());
                        int x = i;
                        
                        i =  (int) atof(item->getStringAttribute("Y").getCharPointer());
                        int y = i;
                        
                        thisItem->setCentrePosition(x, y);
                        
                        i = item->getStringAttribute("active").getIntValue();
                        bool active = (bool)i;
                        
                        thisItem->setActive(active);
                        
                        items.add(thisItem);
                    }
                }
                
                
            }
            
            XmlElement* connections = group->getChildByName("connections");
            
            if (connections != nullptr)
            {
                forEachXmlChildElement (*connections, connection)
                {
                    i = connection->getStringAttribute("type").getIntValue();
                    BKPreparationType cType = (BKPreparationType) i;
                    
                    i = connection->getStringAttribute("Id").getIntValue();
                    int cId = i;
                    
                    if (idmap->getUnchecked(cType)->contains(cId))
                    {
                        cId = idmap->getUnchecked(cType)->getReference(cId);
                    }
                    
                    i = connection->getStringAttribute("piano").getIntValue();
                    int cPiano = i;
                    DBG("conn piano target old: " + String(cPiano));
                    
                    if (idmap->getUnchecked(PreparationTypePiano)->contains(cPiano))
                    {
                        cPiano = idmap->getUnchecked(PreparationTypePiano)->getReference(cPiano);
                    }
                    
                    DBG("conn piano target new: " + String(cPiano));
                    
                    thisConnection = itemWithTypeAndId(cType, cId);
                    
                    if (thisConnection == nullptr)
                    {
                        thisConnection = new BKItem(cType, cId, processor);
                        
                        thisConnection->setItemName(connection->getStringAttribute("name"));
                        
                        thisConnection->setPianoTarget(cPiano);
                        
                        i =  (int) atof(connection->getStringAttribute("X").getCharPointer());
                        int x = i;
                        
                        i =  (int) atof(connection->getStringAttribute("Y").getCharPointer());
                        int y = i;
                        
                        thisConnection->setCentrePosition(x, y);
                        
                        i = connection->getStringAttribute("active").getIntValue();
                        bool active = (bool)i;
                        
                        thisConnection->setActive(active);
                        
                        items.add(thisConnection);
                    }
                    
                    thisItem->addConnection(thisConnection);
                    thisConnection->addConnection(thisItem);
                }
            }
        }
    }
    
#if JUCE_IOS
    for (auto item : items)
    {
        DBG("centre x: " + String(item->getX() + item->getWidth() / 2));
        item->setCentrePosition((item->getX() + item->getWidth() / 2) * processor.uiScaleFactor, (item->getY() + item->getHeight() / 2) * processor.uiScaleFactor);
    }
#endif

}
