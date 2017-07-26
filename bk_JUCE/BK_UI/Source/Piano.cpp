/*
  ==============================================================================

    Piano.cpp
    Created: 7 Dec 2016 10:25:40am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "Piano.h"

Piano::Piano(Synchronic::PtrArr* synchronic,
             Nostalgic::PtrArr* nostalgic,
             Direct::PtrArr* direct,
             Tuning::PtrArr* tuning,
             Tempo::PtrArr* tempo,
             SynchronicModPreparation::PtrArr* mSynchronic,
             NostalgicModPreparation::PtrArr* mNostalgic,
             DirectModPreparation::PtrArr* mDirect,
             TuningModPreparation::PtrArr* mTuning,
             TempoModPreparation::PtrArr* mTempo,
             Keymap::PtrArr* keymaps,
             int Id):
currentPMap(PreparationMap::Ptr()),
activePMaps(PreparationMap::CSPtrArr()),
prepMaps(PreparationMap::CSPtrArr()),
synchronic(synchronic),
nostalgic(nostalgic),
direct(direct),
tuning(tuning),
tempo(tempo),
modSynchronic(mSynchronic),
modNostalgic(mNostalgic),
modDirect(mDirect),
modTuning(mTuning),
modTempo(mTempo),
bkKeymaps(keymaps),
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
    
}

void Piano::deconfigure(void)
{
    
}

void Piano::configure(void)
{
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
            for (auto target : item->getConnections())
            {
                BKPreparationType targetType = target->getType();
                int targetId = target->getId();
                
                if (targetType >= PreparationTypeDirect && targetType <= PreparationTypeTempo)
                {
                    addPreparationToKeymap(targetType, targetId, Id);
                }
                else if (type >= PreparationTypeDirectMod && type <= PreparationTypeTempoMod)
                {
                    configureModification(item);
                }
                else if (targetType == PreparationTypePianoMap)
                {
                    configurePianoMap(item);
                }
                else if (targetType == PreparationTypeReset)
                {
                    configureReset(item);
                }
            }
            
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
                    
                }
            }
        }
        else if (type >= PreparationTypeDirectMod && type <= PreparationTypeTempoMod)
        {
            // Look for direct, nostalgic, synchronic, tuning, and tempo targets
            for (auto target : item->getConnections())
            {
                BKPreparationType targetType = target->getType();
                int targetId = target->getId();
                
                if (targetType >= PreparationTypeDirect && targetType <= PreparationTypeTempo)
                {
                    
                }
            }
        }
        else if (type == PreparationTypeReset)
        {
            // Look for direct, nostalgic, synchronic, tuning, and tempo targets
            for (auto target : item->getConnections())
            {
                BKPreparationType targetType = target->getType();
                int targetId = target->getId();
                
                if (targetType >= PreparationTypeDirect && targetType <= PreparationTypeTempo)
                {
                    
                }
            }
        }
        else if (type == PreparationTypePianoMap)
        {
            // Configure piano map based on saved piano Id
        }
        
        
        
    }
   
    
}

void Piano::add(ItemMapper::Ptr item)
{
    bool added = items.addIfNotAlreadyThere(item);
    
    if (added) configure();
}


void Piano::remove(ItemMapper::Ptr item)
{
    bool removed = false;
    for (int i = items.size(); --i >= 0; )
    {
        if (items[i] == item)
        {
            items.remove(i);
            removed = true;
        }
    }
    
    if (removed) configure();
}


void Piano::addPreparationToKeymap(BKPreparationType thisType, int thisId, int keymapId)
{
    PreparationMap::Ptr thisPreparationMap = getPreparationMapWithKeymap(keymapId);
    
    if (thisPreparationMap == nullptr)
    {
        addPreparationMap(getKeymap(keymapId));
        
        thisPreparationMap = getPreparationMaps().getLast();
    }
    
    if (thisType == PreparationTypeDirect)
    {
        Direct::Ptr thisDirect = getDirect(thisId);
        
        thisPreparationMap->addDirect(thisDirect);
    }
    else if (thisType == PreparationTypeSynchronic)
    {
        Synchronic::Ptr thisSynchronic = getSynchronic(thisId);
        
        thisPreparationMap->addSynchronic(thisSynchronic);
    }
    else if (thisType == PreparationTypeNostalgic)
    {
        Nostalgic::Ptr thisNostalgic = getNostalgic(thisId);
        
        thisPreparationMap->addNostalgic(thisNostalgic);
    }
    else if (thisType == PreparationTypeTempo)
    {
        Tempo::Ptr thisTempo = getTempo(thisId);
        
        thisPreparationMap->addTempo(thisTempo);
    }
    else if (thisType == PreparationTypeTuning)
    {
        Tuning::Ptr thisTuning = getTuning(thisId);
        
        thisPreparationMap->addTuning(thisTuning);
    }
    
}

void Piano::removePreparationFromKeymap(BKPreparationType thisType, int thisId, int keymapId)
{
    PreparationMap::Ptr thisPreparationMap = getPreparationMapWithKeymap(keymapId);
    
    if (thisPreparationMap == nullptr) return;
    
    if (thisType == PreparationTypeDirect)
    {
        Direct::Ptr thisDirect = getDirect(thisId);
        
        thisPreparationMap->removeDirect(thisDirect);
    }
    else if (thisType == PreparationTypeSynchronic)
    {
        Synchronic::Ptr thisSynchronic = getSynchronic(thisId);
        
        thisPreparationMap->removeSynchronic(thisSynchronic);
    }
    else if (thisType == PreparationTypeNostalgic)
    {
        Nostalgic::Ptr thisNostalgic = getNostalgic(thisId);
        
        thisPreparationMap->removeNostalgic(thisNostalgic);
    }
    else if (thisType == PreparationTypeTempo)
    {
        Tempo::Ptr thisTempo = getTempo(thisId);
        
        thisPreparationMap->removeTempo(thisTempo);
    }
    else if (thisType == PreparationTypeTuning)
    {
        Tuning::Ptr thisTuning = getTuning(thisId);
        
        thisPreparationMap->removeTuning(thisTuning);
    }
    
    if (!thisPreparationMap->isActive) removePreparationMapWithKeymap(keymapId);
    
}

void Piano::configureDirectModification(int key, DirectModPreparation::Ptr dmod, Array<int> whichPreps)
{
    DBG("key: " + String(key) + " mod: " + String(dmod->getId()) + " preps: " + intArrayToString(whichPreps));
    
    int whichMod = dmod->getId();
    
    // Add Modifications
    for (int n = cDirectParameterTypes.size(); --n >= 0; )
    {
        String param = dmod->getParam((DirectParameterType)n);
        
        if (param != "")
        {
            for (auto prep : whichPreps)
            {
                modificationMap[key]->addDirectModification(new DirectModification(key, prep, (DirectParameterType)n, param, whichMod));
                
                DBG("ADD whichmod: " + String(whichMod) + " whichprep: " + String(prep) + " whichtype: " + cDirectParameterTypes[n] + " val: " +param + " TO key: " + String(key));
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
        for (auto key : bkKeymaps->getUnchecked(keymap)->keys())
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

void Piano::deconfigureDirectModification(DirectModPreparation::Ptr mod, Array<int> whichKeymaps)
{
    int whichMod = mod->getId();
    
    for (auto keymap : whichKeymaps)
    {
        for (auto key : bkKeymaps->getUnchecked(keymap)->keys())
        {
            // Remove Modification from Key
            modificationMap.getUnchecked(key)->removeDirectModification(whichMod);
        }
    }
}

void Piano::configureNostalgicModification(int key, NostalgicModPreparation::Ptr dmod, Array<int> whichPreps)
{
    DBG("key: " + String(key) + " mod: " + String(dmod->getId()) + " preps: " + intArrayToString(whichPreps));
    
    int whichMod = dmod->getId();
    // Add Modifications
    for (int n = cNostalgicParameterTypes.size(); --n >= 0; )
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

void Piano::configureModifications(ItemMapper::PtrArr mods)
{
    for (auto mod : mods)
    {
        configureModification(mod);
    }
}

void Piano::deconfigureModification(ItemMapper::Ptr map)
{
    BKPreparationType type = map->getType();
    
    Array<int> whichPreps = map->getConnectionIdsOfType(type);
    
    Array<int> whichKeymaps = map->getConnectionIdsOfType(PreparationTypeKeymap);
    
    int Id = map->getId();
    
    if (type == BKPreparationTypeNil) return;
    else if (type == PreparationTypeDirect)
    {
        deconfigureDirectModification(modDirect->getUnchecked(Id), whichKeymaps);
    }
    else if (type == PreparationTypeSynchronic)
    {
        deconfigureSynchronicModification(modSynchronic->getUnchecked(Id), whichKeymaps);
    }
    else if (type == PreparationTypeNostalgic)
    {
        deconfigureNostalgicModification(modNostalgic->getUnchecked(Id), whichKeymaps);
    }
    else if (type == PreparationTypeTuning)
    {
        deconfigureTuningModification(modTuning->getUnchecked(Id), whichKeymaps);
    }
    else if (type == PreparationTypeTempo)
    {
        deconfigureTempoModification(modTempo->getUnchecked(Id), whichKeymaps);
    }
}

void Piano::configureReset(ItemMapper::Ptr item)
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
        for (auto key : bkKeymaps->getUnchecked(keymap)->keys())
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

void Piano::deconfigureReset(ItemMapper::Ptr item)
{
    Array<int> otherKeys;
    
    Array<int> whichKeymaps = item->getConnectionIdsOfType(PreparationTypeKeymap);
    
    for (int i = 0; i < 128; i++) otherKeys.add(i);
    
    for (auto keymap : whichKeymaps)
    {
        deconfigureResetForKeys(item, bkKeymaps->getUnchecked(keymap)->keys());
    }
    
}

void Piano::deconfigureResetForKeys(ItemMapper::Ptr item, Array<int> otherKeys)
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

void Piano::configurePianoMap(ItemMapper::Ptr map)
{
    int pianoTarget = map->getPianoTarget();
    
    Array<int> keymaps = map->getConnectionIdsOfType(PreparationTypeKeymap);
    
    for (auto keymap : keymaps)
    {
        Keymap::Ptr thisKeymap = getKeymap(keymap);
        for (auto key : thisKeymap->keys())
        {
            pianoMap.set(key, pianoTarget);
            
            DBG("key: " + String(key) + " piano: " + String(pianoTarget));
        }
    }
}

// MAYBE INSTEAD OF INDIVIDUAL DECONFIGURES HAVE ONE BIG DECONFIGURE???
void Piano::deconfigurePianoMap(ItemMapper::Ptr map)
{
    int pianoTarget = map->getPianoTarget();
    
    Array<int> keymaps = map->getConnectionIdsOfType(PreparationTypeKeymap);
    
    for (auto keymap : keymaps)
    {
        Keymap::Ptr thisKeymap = getKeymap(keymap);
        
        for (auto key : thisKeymap->keys())
        {
            pianoMap.set(key, -1);
        }
    }
}

void Piano::configureModification(ItemMapper::Ptr map)
{
    map->print();
    
    BKPreparationType modType = map->getType();
    BKPreparationType targetType = modToPrepType(modType);
    int Id = map->getId();
    
    Array<int> whichPreps = map->getConnectionIdsOfType(targetType);
    
    Array<int> whichKeymaps = map->getConnectionIdsOfType(PreparationTypeKeymap);
    
    if (modType < BKPreparationTypeNil) return;
    else if (modType == PreparationTypeDirectMod)
    {
        configureDirectModification(modDirect->getUnchecked(Id), whichKeymaps, whichPreps);
    }
    else if (modType == PreparationTypeSynchronicMod)
    {
        configureSynchronicModification(modSynchronic->getUnchecked(Id), whichKeymaps, whichPreps);
    }
    else if (modType == PreparationTypeNostalgicMod)
    {
        configureNostalgicModification(modNostalgic->getUnchecked(Id), whichKeymaps, whichPreps);
    }
    else if (modType == PreparationTypeTuningMod)
    {
        configureTuningModification(modTuning->getUnchecked(Id), whichKeymaps, whichPreps);
    }
    else if (modType == PreparationTypeTempoMod)
    {
        configureTempoModification(modTempo->getUnchecked(Id), whichKeymaps, whichPreps);
    }

}

void Piano::configureNostalgicModification(NostalgicModPreparation::Ptr mod, Array<int> whichKeymaps, Array<int> whichPreps)
{
    Array<int> otherKeys;
    
    for (int i = 0; i < 128; i++) otherKeys.add(i);
    
    for (auto keymap : whichKeymaps)
    {
        for (auto key : bkKeymaps->getUnchecked(keymap)->keys())
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

void Piano::deconfigureNostalgicModification(NostalgicModPreparation::Ptr mod, Array<int> whichKeymaps)
{
    int whichMod = mod->getId();
    
    for (auto keymap : whichKeymaps)
    {
        for (auto key : bkKeymaps->getUnchecked(keymap)->keys())
        {
            // Remove Modification from Key
            modificationMap[key]->removeNostalgicModification(whichMod);
        }
    }
}


void Piano::configureSynchronicModification(int key, SynchronicModPreparation::Ptr dmod, Array<int> whichPreps)
{
    int whichMod = dmod->getId();
    
    // Add Modifications
    for (int n = cSynchronicParameterTypes.size(); --n >= 0; )
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
        for (auto key : bkKeymaps->getUnchecked(keymap)->keys())
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

void Piano::deconfigureSynchronicModification(SynchronicModPreparation::Ptr mod, Array<int> whichKeymaps
)
{
    int whichMod = mod->getId();
    
    for (auto keymap : whichKeymaps)
    {
        for (auto key : bkKeymaps->getUnchecked(keymap)->keys())
        {
            // Remove Modification from Key
            modificationMap[key]->removeSynchronicModification(whichMod);
        }
    }
}

void Piano::configureTempoModification(int key, TempoModPreparation::Ptr dmod, Array<int> whichPreps)
{
    DBG("key: " + String(key) + " mod: " + String(dmod->getId()) + " preps: " + intArrayToString(whichPreps));
    
    int whichMod = dmod->getId();
    
    // Add Modifications
    for (int n = cTempoParameterTypes.size(); --n >= 0; )
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
        for (auto key : bkKeymaps->getUnchecked(keymap)->keys())
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
        
        DBG("REMOVE whichmod: " + String(whichMod) + " FROM key: " +String(key));
    }
}

void Piano::deconfigureTempoModification(TempoModPreparation::Ptr mod, Array<int> whichKeymaps)
{
    int whichMod = mod->getId();
    
    for (auto keymap : whichKeymaps)
    {
        for (auto key : bkKeymaps->getUnchecked(keymap)->keys())
        {
            // Remove Modification from Key
            modificationMap[key]->removeTempoModification(whichMod);
            
            DBG("REMOVE whichmod: " + String(whichMod) + " FROM key: " +String(key));
        }
    }
}

void Piano::configureTuningModification(int key, TuningModPreparation::Ptr dmod, Array<int> whichPreps)
{
    
    DBG("key: " + String(key) + " mod: " + String(dmod->getId()) + " preps: " + intArrayToString(whichPreps));
    
    int whichMod = dmod->getId();

    // Add Modifications
    for (int n = cTuningParameterTypes.size(); --n >= 0; )
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
        for (auto key : bkKeymaps->getUnchecked(keymap)->keys())
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
        
        DBG("REMOVE whichmod: " + String(whichMod) + " FROM key: " +String(key));
    }
}

void Piano::deconfigureTuningModification(TuningModPreparation::Ptr mod, Array<int> whichKeymaps)
{
    int whichMod = mod->getId();
    
    for (auto keymap : whichKeymaps)
    {
        for (auto key : bkKeymaps->getUnchecked(keymap)->keys())
        {
            // Remove Modification from Key
            modificationMap[key]->removeTuningModification(whichMod);
            
            DBG("REMOVE whichmod: " + String(whichMod) + " FROM key: " +String(key));
        }
    }
}


// Add preparation map, return its Id.
int Piano::addPreparationMap(void)
{
    PreparationMap::Ptr thisPreparationMap = new PreparationMap(bkKeymaps->getUnchecked(0), numPMaps);
    
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
        if (activePMaps[i]->getKeymapId() == Id)
        {
            activePMaps.remove(i);
            break;
        }
    }
    
    for (int i = prepMaps.size(); --i >= 0; )
    {
        if (prepMaps[i]->getKeymapId() == Id)
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

}

ValueTree Piano::getState(void)
{
    ValueTree pianoVT( vtagPiano + String(getId()));
    
    pianoVT.setProperty("bkPianoName", getName(), 0);
    
    int prepMapCount = 0;
    for (auto pmap : getPreparationMaps())
    {
        ValueTree pmapVT( vtagPrepMap+String(prepMapCount++));
        pmapVT.setProperty( ptagPrepMap_keymapId, pmap->getKeymapId(), 0);
        
        int pcount = 0;
        for (auto prep : pmap->getDirect())
            pmapVT.setProperty(ptagPrepMap_directPrepId+String(pcount++), prep->getId(), 0);
        
        pcount = 0;
        for (auto prep : pmap->getNostalgic())
            pmapVT.setProperty(ptagPrepMap_nostalgicPrepId+String(pcount++), prep->getId(), 0);
        
        pcount = 0;
        for (auto prep : pmap->getSynchronic())
            pmapVT.setProperty(ptagPrepMap_synchronicPrepId+String(pcount++), prep->getId(), 0);
        
        pcount = 0;
        for (auto prep : pmap->getTuning())
            pmapVT.setProperty(ptagPrepMap_tuningPrepId +String(pcount++), prep->getId(), 0);
        
        pcount = 0;
        for (auto prep : pmap->getTempo())
            pmapVT.setProperty(ptagPrepMap_tempoPrepId +String(pcount++), prep->getId(), 0);
        
        
        pianoVT.addChild(pmapVT, -1, 0);
    }
    
    /*// FIX THIS
    // Mapper Saving
    int mapCount = 0;
    for (auto map : items)
    {
        ValueTree mapVT ("item"+String(mapCount++));
        
        mapVT.setProperty("type", map->getType(), 0);
        
        mapVT.setProperty("Id", map->getId(), 0);
        
        for (int i = 0; i < BKPreparationTypeNil; i++)
        {
            
            ValueTree connexVT( "type" + String(i));
            
            Array<int> connex = map->getConnections((BKPreparationType)i);
            
            int count = 0;
            for (auto c : connex)
            {
                connexVT.setProperty("id"+String(count++), c, 0);
            }
            
            mapVT.addChild(connexVT, -1, 0);
            
        }
        
        map->print();
        
        pianoVT.addChild(mapVT, -1, 0);

    }
     */
    
    
    
    return pianoVT;
}
#define LOAD_VERSION 0

void Piano::setState(XmlElement* e)
{
    
#if LOAD_VERSION
    int i; float f;
    int pianoMapCount = 0, prepMapCount = 0, resetCount = 0, mapperCount = 0;
    
    String pianoName = e->getStringAttribute("bkPianoName");
    
    if (pianoName != String::empty)
        setName(e->getStringAttribute("bkPianoName"));
    
    configuration = new PianoConfiguration();
    
    forEachXmlChildElement (*e, pc)
    {
        String map =  "mapper" + String(mapperCount);
        
        if (pc->hasTagName("configuration"))
        {
            configuration->setState(pc);
        }
        else if (pc->hasTagName(map))
        {
            i = pc->getStringAttribute("type").getIntValue();
            BKPreparationType type = (BKPreparationType) i;
            
            i = pc->getStringAttribute("Id").getIntValue();
            int thisId = i;
            
            i = pc->getStringAttribute("piano").getIntValue();
            int piano = i;
            
            Array<int> keymaps;
            for (int k = 0; k < 500; k++) // arbitrary
            {
                String attr = pc->getStringAttribute("k" + String(k));
                
                if (attr == String::empty)  break;
                else                        keymaps.add(attr.getIntValue());
            }
            
            Array<int> targets;
            for (int k = 0; k < 500; k++) // arbitrary
            {
                String attr = pc->getStringAttribute("t" + String(k));
                
                if (attr == String::empty)  break;
                else                        targets.add(attr.getIntValue());
            }
            
            ItemMapper::Ptr mapper = new ItemMapper(type, thisId, keymaps, targets);
            mapper->piano = piano;
            
            int resetCount = 0;
            forEachXmlChildElement (*pc, sub)
            {
                if (sub->hasTagName(vtagReset+String(resetCount)))
                {
                    Array<int> theseResets = mapper->resets.getUnchecked(resetCount);
                    
                    for (int k = 0; k < 100; k++)
                    {
                        String attr = sub->getStringAttribute("r" + String(k));
                        
                        if (attr == String::empty) break;
                        else
                        {
                            theseResets.add(attr.getIntValue());
                        }
                    }
                    
                    mapper->resets.set(resetCount, theseResets);
                    
                    resetCount++;
                }
            }
            
            DBG("SETTINGUP");
            mapper->print();
            
            configureModification(mapper);
            
            mappers.add(mapper);
            
            ++mapperCount;
        }
        else if (pc->hasTagName( vtagPianoMap + String(pianoMapCount)))
        {
            // PianoMap
            i = pc->getStringAttribute(ptagPianoMap_key).getIntValue();
            int key = i;
            
            i = pc->getStringAttribute(ptagPianoMap_piano).getIntValue();
            int piano = i;
            
            pianoMap.set(key, piano);
            
            ++pianoMapCount;
        }
        else if (pc->hasTagName( vtagPrepMap + String(prepMapCount)))
        {
            // PrepMap
            i = pc->getStringAttribute(ptagPrepMap_keymapId).getIntValue();
            Keymap::Ptr keymap = bkKeymaps->getUnchecked(i);
            
            addPreparationMap(); // should clean up this functionality . pretty bad
            
            prepMaps.getLast()->setKeymap(keymap);
            
            Synchronic::PtrArr sync;
            for (int k = 0; k < 128; k++)
            {
                String attr = pc->getStringAttribute(ptagPrepMap_synchronicPrepId + String(k));
                
                if (attr == String::empty)  break;
                else                        sync.add(synchronic->getUnchecked(attr.getIntValue()));
                
            }
            prepMaps[prepMapCount]->setSynchronic(sync);
            
            Nostalgic::PtrArr nost;
            for (int k = 0; k < 128; k++)
            {
                String attr = pc->getStringAttribute(ptagPrepMap_nostalgicPrepId + String(k));
                
                if (attr == String::empty)  break;
                else                        nost.add(nostalgic->getUnchecked(attr.getIntValue()));
                
            }
            prepMaps[prepMapCount]->setNostalgic(nost);
            
            
            Direct::PtrArr drct;
            for (int k = 0; k < 128; k++)
            {
                String attr = pc->getStringAttribute(ptagPrepMap_directPrepId + String(k));
                
                if (attr == String::empty)  break;
                else                        drct.add(direct->getUnchecked(attr.getIntValue()));
                
            }
            DBG(pianoName + " direct: " + intArrayToString(getAllIds(drct)));
            prepMaps[prepMapCount]->setDirect(drct);
            
            Tuning::PtrArr tune;
            for (int k = 0; k < 128; k++)
            {
                String attr = pc->getStringAttribute(ptagPrepMap_tuningPrepId + String(k));
                
                if (attr == String::empty)  break;
                else                        tune.add(tuning->getUnchecked(attr.getIntValue()));
                
            }
            prepMaps[prepMapCount]->setTuning(tune);
            
            Tempo::PtrArr tmp;
            for (int k = 0; k < 128; k++)
            {
                String attr = pc->getStringAttribute(ptagPrepMap_tempoPrepId + String(k));
                
                if (attr == String::empty)  break;
                else                        tmp.add(tempo->getUnchecked(attr.getIntValue()));
                
            }
            prepMaps[prepMapCount]->setTempo(tmp);
            
            ++prepMapCount;
        }
        
    }
#endif 
    
}



