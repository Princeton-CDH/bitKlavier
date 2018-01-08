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

Piano::Piano(BKAudioProcessor& p,
             int Id):
currentPMap(PreparationMap::Ptr()),
activePMaps(PreparationMap::CSPtrArr()),
prepMaps(PreparationMap::CSPtrArr()),
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
    prepMaps.clear();
    activePMaps.clear();
    numPMaps = 0;
    
    for (auto proc : dprocessor) proc->reset();
    for (auto proc : mprocessor) proc->reset();
    for (auto proc : sprocessor) proc->reset();
    for (auto proc : nprocessor) proc->reset();
    for (auto proc : tprocessor) proc->reset();
    
    dprocessor.clear();
    mprocessor.clear();
    sprocessor.clear();
    nprocessor.clear();
    tprocessor.clear();
    
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
    
    for (auto item : items)
    {
        
        BKPreparationType thisType = item->getType();
        int thisId = item->getId();
        
        DBG("type: " + cPreparationTypes[thisType] + " Id: " + String(thisId));
        DBG("bounds: " + rectangleToString(item->getBounds()));
        
        if (thisId > processor.gallery->getIdCount(thisType)) processor.gallery->setIdCount(thisType, thisId);
        
        addProcessor(thisType, thisId);
    }
    
    for (auto item : items)
    {
        BKPreparationType type = item->getType();
        int Id = item->getId();
        
        
        // Connect keymaps to everything
        // Connect tunings, tempos, synchronics to preparations
        // Connect mods and resets to all their targets
        // Configure piano maps
        // ... should be all configured if done in that order ...
        if (type == PreparationTypeKeymap)
        {
            BKItem::PtrArr connex = item->getConnections();
            for (auto target : connex)
            {
                BKPreparationType targetType = target->getType();
                int targetId = target->getId();
                
                if (targetType >= PreparationTypeDirect && targetType <= PreparationTypeTempo)
                {
                    linkPreparationWithKeymap(targetType, targetId, Id);
                }
                else if (targetType >= PreparationTypeDirectMod && targetType <= PreparationTypeTempoMod)
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
            // Look for synchronic, direct, and nostalgic targets
            for (auto target : item->getConnections())
            {
                BKPreparationType targetType = target->getType();
                int targetId = target->getId();
                
                if (targetType >= PreparationTypeDirect && targetType <= PreparationTypeNostalgic)
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
                
                if (targetType == PreparationTypeSynchronic)
                {
                    linkSynchronicWithTempo(processor.gallery->getSynchronic(targetId), processor.gallery->getTempo(Id));
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
    }
    
    processor.updateState->pianoDidChangeForGraph = true;
}

SynchronicProcessor::Ptr Piano::addSynchronicProcessor(int thisId)
{
    SynchronicProcessor::Ptr sproc = new SynchronicProcessor(processor.gallery->getSynchronic(thisId),
                                        defaultT,
                                        defaultM,
                                        &processor.mainPianoSynth,
                                        processor.gallery->getGeneralSettings());
    sproc->prepareToPlay(sampleRate, &processor.mainPianoSynth);
    sprocessor.add(sproc);
    
    return sproc;
}

DirectProcessor::Ptr Piano::getDirectProcessor(int Id, bool add)
{
    for (auto proc : dprocessor)
    {
        if (proc->getId() == Id) return proc;
    }
    
    return add ? addDirectProcessor(Id) : nullptr;
}

NostalgicProcessor::Ptr Piano::getNostalgicProcessor(int Id, bool add)
{
    for (auto proc : nprocessor)
    {
        if (proc->getId() == Id) return proc;
    }
    
    return add ? addNostalgicProcessor(Id) : nullptr;
}

SynchronicProcessor::Ptr Piano::getSynchronicProcessor(int Id, bool add)
{
    for (auto proc : sprocessor)
    {
        if (proc->getId() == Id) return proc;
    }
    
    return add ? addSynchronicProcessor(Id) : nullptr;
}

TuningProcessor::Ptr Piano::getTuningProcessor(int Id, bool add)
{
    for (auto proc : tprocessor)
    {
        if (proc->getId() == Id) return proc;
    }
    
    return add ? addTuningProcessor(Id) : nullptr;
}

TempoProcessor::Ptr Piano::getTempoProcessor(int Id, bool add)
{
    for (auto proc : mprocessor)
    {
        if (proc->getId() == Id) return proc;
    }
    
    return add ? addTempoProcessor(Id) : nullptr;
}


NostalgicProcessor::Ptr Piano::addNostalgicProcessor(int thisId)
{
    NostalgicProcessor::Ptr nproc = new NostalgicProcessor(processor.gallery->getNostalgic(thisId),
                                       defaultT,
                                       defaultS,
                                       &processor.mainPianoSynth);
    nproc->prepareToPlay(sampleRate, &processor.mainPianoSynth);
    nprocessor.add(nproc);
    
    return nproc;
}

DirectProcessor::Ptr Piano::addDirectProcessor(int thisId)
{
    DirectProcessor::Ptr dproc = new DirectProcessor(processor.gallery->getDirect(thisId),
                                    defaultT,
                                    &processor.mainPianoSynth,
                                    &processor.resonanceReleaseSynth,
                                    &processor.hammerReleaseSynth);
    dproc->prepareToPlay(sampleRate, &processor.mainPianoSynth, &processor.resonanceReleaseSynth,&processor.hammerReleaseSynth);
    dprocessor.add(dproc);
    
    return dproc;
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

bool Piano::containsProcessor(BKPreparationType thisType, int thisId)
{
    if (thisType == PreparationTypeDirect)
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
}

bool Piano::contains(BKItem::Ptr thisItem)
{
    for (auto item : items) if (item == thisItem) return true;
    
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
        DBG("ITEMINREMOVE:" );
        items[i]->print();
        if (items[i]->getType() == item->getType() && items[i]->getId() == item->getId())
        {
            items.remove(i);
            removed = true;
        }
    }
    
    if (removed) configure();
}

void Piano::linkSynchronicWithTempo(Synchronic::Ptr synchronic, Tempo::Ptr thisTempo)
{
    SynchronicProcessor::Ptr proc = getSynchronicProcessor(synchronic->getId());
    
    proc->setTempo(getTempoProcessor(thisTempo->getId()));
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

void Piano::linkPreparationWithKeymap(BKPreparationType thisType, int thisId, int keymapId)
{
    PreparationMap::Ptr thisPreparationMap = getPreparationMapWithKeymap(keymapId);
    
    if (thisPreparationMap == nullptr)
    {
        addPreparationMap(processor.gallery->getKeymap(keymapId));
        
        thisPreparationMap = getPreparationMaps().getLast();
    }
    
    if (thisType == PreparationTypeDirect)
    {
        DirectProcessor::Ptr dproc = getDirectProcessor(thisId);
        
        thisPreparationMap->addDirectProcessor(dproc);
    }
    else if (thisType == PreparationTypeSynchronic)
    {
        SynchronicProcessor::Ptr sproc = getSynchronicProcessor(thisId);
        
        thisPreparationMap->addSynchronicProcessor(sproc);
    }
    else if (thisType == PreparationTypeNostalgic)
    {
        NostalgicProcessor::Ptr nproc = getNostalgicProcessor(thisId);
        
        thisPreparationMap->addNostalgicProcessor(nproc);
    }
    else if (thisType == PreparationTypeTempo)
    {
        TempoProcessor::Ptr mproc = getTempoProcessor(thisId);
        
        thisPreparationMap->addTempoProcessor(mproc);
    }
    else if (thisType == PreparationTypeTuning)
    {
        TuningProcessor::Ptr tproc = getTuningProcessor(thisId);
        
        thisPreparationMap->addTuningProcessor(tproc);
    }
    
}


void Piano::configureDirectModification(int key, DirectModPreparation::Ptr dmod, Array<int> whichPreps)
{
    int whichMod = dmod->getId();
    
    // Add Modifications
    for (int n = (int)cDirectParameterTypes.size(); --n >= 0; )
    {
        String param = dmod->getParam((DirectParameterType)n);
        
        if (param != "")
        {
            for (auto prep : whichPreps)
            {
                modificationMap[key]->addDirectModification(new DirectModification(key, prep, (DirectParameterType)n, param, whichMod));
            }
        }
    }
}

void Piano::configureDirectModification(DirectModPreparation::Ptr mod, Array<int> whichKeymaps, Array<int> whichPreps)
{
    Array<int> otherKeys;
    
    for (int i = 0; i < 128; i++) otherKeys.add(i);
    
    for (auto keymap : whichKeymaps)
    {
        for (auto key : processor.gallery->getKeymap(keymap)->keys())
        {
            configureDirectModification(key, mod, whichPreps);
            
            for (int i = otherKeys.size(); --i>=0;)
            {
                if (otherKeys[i] == key) otherKeys.remove(i);
            }
        }
    }
    
    deconfigureDirectModificationForKeys(mod, otherKeys);
}

void Piano::deconfigureDirectModificationForKeys(DirectModPreparation::Ptr mod, Array<int> keys)
{
    int whichMod = mod->getId();

    for (auto key : keys)
    {
        // Remove Modification from Key
        modificationMap[key]->removeDirectModification(whichMod);
        
    }
}

void Piano::configureNostalgicModification(int key, NostalgicModPreparation::Ptr dmod, Array<int> whichPreps)
{
    DBG("PIANO key: " + String(key) + " mod: " + String(dmod->getId()) + " preps: " + intArrayToString(whichPreps));
    
    int whichMod = dmod->getId();
    // Add Modifications
    for (int n = (int)cNostalgicParameterTypes.size(); --n >= 0; )
    {
        String param = dmod->getParam((NostalgicParameterType)n);
        
        if (param != "")
        {
            for (auto prep : whichPreps)
            {
                modificationMap[key]->addNostalgicModification(new NostalgicModification(key, prep, (NostalgicParameterType)n, param, whichMod));
            }
        }
    }
}

void Piano::configureReset(BKItem::Ptr item)
{
    Array<int> otherKeys;
    
    Array<int> whichKeymaps = item->getConnectionIdsOfType(PreparationTypeKeymap);
    
    Array<int> direct = item->getConnectionIdsOfType(PreparationTypeDirect);
    Array<int> nostalgic = item->getConnectionIdsOfType(PreparationTypeNostalgic);
    Array<int> synchronic = item->getConnectionIdsOfType(PreparationTypeSynchronic);
    Array<int> tempo = item->getConnectionIdsOfType(PreparationTypeTempo);
    Array<int> tuning = item->getConnectionIdsOfType(PreparationTypeTuning);
    
    for (int i = 0; i < 128; i++) otherKeys.add(i);
    
    for (auto keymap : whichKeymaps)
    {
        for (auto key : processor.gallery->getKeymap(keymap)->keys())
        {
            for (auto id : direct) modificationMap[key]->directReset.add(id);
            
            for (auto id : synchronic) modificationMap[key]->synchronicReset.add(id);
            
            for (auto id : nostalgic) modificationMap[key]->nostalgicReset.add(id);
        
            for (auto id : tuning) modificationMap[key]->tuningReset.add(id);
            
            for (auto id : tempo) modificationMap[key]->tempoReset.add(id);
            
            for (int i = otherKeys.size(); --i>=0;)
            {
                if (otherKeys[i] == key) otherKeys.remove(i);
            }
        }
    }
    
    
    deconfigureResetForKeys(item, otherKeys);
    
}

void Piano::deconfigureResetForKeys(BKItem::Ptr item, Array<int> otherKeys)
{
    Array<int> direct = item->getConnectionIdsOfType(PreparationTypeDirect);
    Array<int> nostalgic = item->getConnectionIdsOfType(PreparationTypeNostalgic);
    Array<int> synchronic = item->getConnectionIdsOfType(PreparationTypeSynchronic);
    Array<int> tempo = item->getConnectionIdsOfType(PreparationTypeTempo);
    Array<int> tuning = item->getConnectionIdsOfType(PreparationTypeTuning);
    
    for (auto key : otherKeys)
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
            
            DBG("PIANOMAP key: " + String(key) + " piano: " + String(pianoTarget));
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
        configureDirectModification(processor.gallery->getDirectModPreparation(Id), whichKeymaps, whichPreps);
    }
    else if (modType == PreparationTypeSynchronicMod)
    {
        configureSynchronicModification(processor.gallery->getSynchronicModPreparation(Id), whichKeymaps, whichPreps);
    }
    else if (modType == PreparationTypeNostalgicMod)
    {
        configureNostalgicModification(processor.gallery->getNostalgicModPreparation(Id), whichKeymaps, whichPreps);
    }
    else if (modType == PreparationTypeTuningMod)
    {
        configureTuningModification(processor.gallery->getTuningModPreparation(Id), whichKeymaps, whichPreps);
    }
    else if (modType == PreparationTypeTempoMod)
    {
        configureTempoModification(processor.gallery->getTempoModPreparation(Id), whichKeymaps, whichPreps);
    }

}

void Piano::configureNostalgicModification(NostalgicModPreparation::Ptr mod, Array<int> whichKeymaps, Array<int> whichPreps)
{
    Array<int> otherKeys;
    
    for (int i = 0; i < 128; i++) otherKeys.add(i);
    
    for (auto keymap : whichKeymaps)
    {
        for (auto key : processor.gallery->getKeymap(keymap)->keys())
        {
            configureNostalgicModification(key, mod, whichPreps);
            otherKeys.remove(key);
        }
    }
    
    deconfigureNostalgicModificationForKeys(mod, otherKeys);
}

void Piano::deconfigureNostalgicModificationForKeys(NostalgicModPreparation::Ptr mod, Array<int> keys)
{
    int whichMod = mod->getId();
    
    for (auto key : keys)
    {
        // Remove Modification from Key
        modificationMap[key]->removeNostalgicModification(whichMod);
    }
}

void Piano::configureSynchronicModification(int key, SynchronicModPreparation::Ptr dmod, Array<int> whichPreps)
{
    int whichMod = dmod->getId();
    
    // Add Modifications
    for (int n = (int)cSynchronicParameterTypes.size(); --n >= 0; )
    {
        String param = dmod->getParam((SynchronicParameterType)n);
        
        
        if (param != "")
        {
            for (auto prep : whichPreps)
            {
                modificationMap[key]->addSynchronicModification(new SynchronicModification(key, prep, (SynchronicParameterType)n, param, whichMod));
            }
        }
    }
}

void Piano::configureSynchronicModification(SynchronicModPreparation::Ptr mod, Array<int> whichKeymaps, Array<int> whichPreps)
{
    Array<int> otherKeys;
    
    for (int i = 0; i < 128; i++) otherKeys.add(i);
    
    for (auto keymap : whichKeymaps)
    {
        for (auto key : processor.gallery->getKeymap(keymap)->keys())
        {
            configureSynchronicModification(key, mod, whichPreps);
            otherKeys.remove(key);
        }
    }
    
    deconfigureSynchronicModificationForKeys(mod, otherKeys);
}

void Piano::deconfigureSynchronicModificationForKeys(SynchronicModPreparation::Ptr mod, Array<int> keys)
{
    int whichMod = mod->getId();

    for (auto key : keys)
    {
        // Remove Modification from Key
        modificationMap[key]->removeSynchronicModification(whichMod);
    }
}

void Piano::configureTempoModification(int key, TempoModPreparation::Ptr dmod, Array<int> whichPreps)
{
    DBG("TEMPOMOD key: " + String(key) + " mod: " + String(dmod->getId()) + " preps: " + intArrayToString(whichPreps));
    
    int whichMod = dmod->getId();
    
    // Add Modifications
    for (int n = (int)cTempoParameterTypes.size(); --n >= 0; )
    {
        String param = dmod->getParam((TempoParameterType)n);
        
        if (param != "")
        {
            for (auto prep : whichPreps)
            {
                modificationMap[key]->addTempoModification(new TempoModification(key, prep, (TempoParameterType)n, param, whichMod));
            }
        }
    }
}


void Piano::configureTempoModification(TempoModPreparation::Ptr mod, Array<int> whichKeymaps, Array<int> whichPreps)
{
    Array<int> otherKeys;
    
    for (int i = 0; i < 128; i++) otherKeys.add(i);
    
    for (auto keymap : whichKeymaps)
    {
        for (auto key : processor.gallery->getKeymap(keymap)->keys())
        {
            configureTempoModification(key, mod, whichPreps);
            otherKeys.remove(key);
        }
    }
    
    deconfigureTempoModificationForKeys(mod, otherKeys);
}

void Piano::deconfigureTempoModificationForKeys(TempoModPreparation::Ptr mod, Array<int> keys)
{
    int whichMod = mod->getId();
    
    for (auto key : keys)
    {
        // Remove Modification from Key
        modificationMap[key]->removeTempoModification(whichMod);
        
        //DBG("REMOVE whichmod: " + String(whichMod) + " FROM key: " +String(key));
    }
}

void Piano::configureTuningModification(int key, TuningModPreparation::Ptr dmod, Array<int> whichPreps)
{
    
    //DBG("TUNINGMOD key: " + String(key) + " mod: " + String(dmod->getId()) + " preps: " + intArrayToString(whichPreps));
    
    int whichMod = dmod->getId();

    // Add Modifications
    for (int n = (int)cTuningParameterTypes.size(); --n >= 0; )
    {
        String param = dmod->getParam((TuningParameterType)n);
        
        if (param != "")
        {
            for (auto prep : whichPreps)
            {
                modificationMap[key]->addTuningModification(new TuningModification(key, prep, (TuningParameterType)n, param, whichMod));
            }
        }
    }
}

void Piano::configureTuningModification(TuningModPreparation::Ptr mod, Array<int> whichKeymaps, Array<int> whichPreps)
{
    Array<int> otherKeys;
    
    for (int i = 0; i < 128; i++) otherKeys.add(i);
    
    for (auto keymap : whichKeymaps)
    {
        for (auto key : processor.gallery->getKeymap(keymap)->keys())
        {
            configureTuningModification(key, mod, whichPreps);
            otherKeys.remove(key);
        }
    }
    
    deconfigureTuningModificationForKeys(mod, otherKeys);
}

void Piano::deconfigureTuningModificationForKeys(TuningModPreparation::Ptr mod, Array<int> keys)
{
    int whichMod = mod->getId();
    
    for (auto key : keys)
    {
        // Remove Modification from Key
        modificationMap[key]->removeTuningModification(whichMod);
        
        //DBG("REMOVE whichmod: " + String(whichMod) + " FROM key: " +String(key));
    }
}

// Add preparation map, return its Id.
int Piano::addPreparationMap(void)
{
    PreparationMap::Ptr thisPreparationMap = new PreparationMap(processor.gallery->getKeymap(0), numPMaps);
    
    prepMaps.add(thisPreparationMap);
    
    thisPreparationMap->prepareToPlay(sampleRate);
    
    activePMaps.add(thisPreparationMap);
    
    return prepMaps.size()-1;
}

// Add preparation map, return its Id.
int Piano::addPreparationMap(Keymap::Ptr keymap)
{
    PreparationMap::Ptr thisPreparationMap = new PreparationMap(keymap, numPMaps);
    
    prepMaps.add(thisPreparationMap);
    
    thisPreparationMap->prepareToPlay(sampleRate);
    
    thisPreparationMap->setKeymap(keymap);
    
    activePMaps.add(thisPreparationMap);
    
    return prepMaps.size()-1;
}

PreparationMap::Ptr        Piano::getPreparationMapWithKeymap(int keymapId)
{
    PreparationMap::Ptr thisPMap = nullptr;
    for (auto pmap : prepMaps)
    {
        if (pmap->getKeymap()->getId() == keymapId)
        {
            thisPMap = pmap;
            break;
        }
    }
    return thisPMap;
}


// Add preparation map, return its Id.
int Piano::removePreparationMapWithKeymap(int Id)
{
    for (int i = activePMaps.size(); --i >= 0; )
    {
        if (activePMaps[i]->getKeymap()->getId() == Id)
        {
            activePMaps.remove(i);
            break;
        }
    }
    
    for (int i = prepMaps.size(); --i >= 0; )
    {
        if (prepMaps[i]->getKeymap()->getId() == Id)
        {
            prepMaps.remove(i);
            break;
        }
    }
    
    --numPMaps;
    
    return numPMaps;
}

// Add preparation map, return its Id.
int Piano::removeLastPreparationMap(void)
{
    for (int i = activePMaps.size(); --i >= 0;)
    {
        if (activePMaps[i]->getId() == (numPMaps-1))
        {
            activePMaps.remove(i);
        }
    }
    
    prepMaps.remove((numPMaps-1));
    
    --numPMaps;

    return numPMaps;
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
}

ValueTree Piano::getState(void)
{
    ValueTree pianoVT( vtagPiano);
    
    String name = getName();
    pianoVT.setProperty("name", ((name == String::empty) ? "Piano"+String(Id) : name) , 0);
    
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
            
            // Look for synchronic, direct, and nostalgic targets
            for (auto target : item->getConnections())
            {
                BKPreparationType targetType = target->getType();
                
                if (targetType >= PreparationTypeDirect && targetType <= PreparationTypeNostalgic)
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
                
                if (targetType == PreparationTypeSynchronic)
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
        else if ((type >= PreparationTypeDirectMod && type <= PreparationTypeTempoMod) || type == PreparationTypeReset)
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

void Piano::setState(XmlElement* e)
{
    int i = 0;
    
    Id = e->getStringAttribute("Id").getIntValue();
    
    String pianoName = e->getStringAttribute("name");
    
    if (pianoName != String::empty) setName(pianoName);
    
    setId(e->getStringAttribute("Id").getIntValue());
    
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
                    
                    i = item->getStringAttribute("X").getIntValue();
                    int x = i;
                    
                    i = item->getStringAttribute("Y").getIntValue();
                    int y = i;
                    
                    i = item->getStringAttribute("W").getIntValue();
                    int w = i;
                    
                    i = item->getStringAttribute("H").getIntValue();
                    int h = i;
                    
                    String s = item->getStringAttribute("text");
                    thisItem->setCommentText(s);
                    
                    thisItem->setCentrePosition(x, y);
                    thisItem->setSize(w, h);
                    
                    items.add(thisItem);
                }
                else
                {
                    i = item->getStringAttribute("Id").getIntValue();
                    int thisId = i;
                    
                    i = item->getStringAttribute("piano").getIntValue();
                    int piano = i;
                    
                    thisItem = itemWithTypeAndId(type, thisId);
                    
                    if (thisItem == nullptr)
                    {
                        thisItem = new BKItem(type, thisId, processor);
                        
                        thisItem->setPianoTarget(piano);
                        
                        thisItem->setItemName(item->getStringAttribute("name"));
                        
                        i = item->getStringAttribute("X").getIntValue();
                        int x = i;
                        
                        i = item->getStringAttribute("Y").getIntValue();
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
                    
                    i = connection->getStringAttribute("piano").getIntValue();
                    int cPiano = i;
                    
                    thisConnection = itemWithTypeAndId(cType, cId);
                    
                    if (thisConnection == nullptr)
                    {
                        thisConnection = new BKItem(cType, cId, processor);
                        
                        thisConnection->setItemName(connection->getStringAttribute("name"));
                        
                        thisConnection->setPianoTarget(cPiano);
                        
                        i = connection->getStringAttribute("X").getIntValue();
                        int x = i;
                        
                        i = connection->getStringAttribute("Y").getIntValue();
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



