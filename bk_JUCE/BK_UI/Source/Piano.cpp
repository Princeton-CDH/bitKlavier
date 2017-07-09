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
    
    configuration = new PianoConfiguration();
    
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

void Piano::configureModifications(ModificationMapper::PtrArr mods)
{
    for (auto mod : mods)
    {
        configureModification(mod);
    }
}

void Piano::deconfigureModification(ModificationMapper::Ptr map)
{
    Array<int> whichPreps = map->getTargets();
    Array<int> whichKeymaps = map->getKeymaps();
    
    BKPreparationType type = map->getType();
    int Id = map->getId();
    
    if (type == BKPreparationTypeNil) return;
    else if (type == PreparationTypePianoMap)
    {
        deconfigurePianoMap(whichKeymaps, map->piano);
    }
    else if (type == PreparationTypeReset)
    {
        deconfigureResets(map->resets, whichKeymaps);
    }
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

void Piano::configureResets(Array<Array<int>> resets, Array<int> whichKeymaps, Array<int> whichPreps)
{
    Array<int> otherKeys;
    
    for (int i = 0; i < 128; i++) otherKeys.add(i);
    
    for (auto keymap : whichKeymaps)
    {
        for (auto key : bkKeymaps->getUnchecked(keymap)->keys())
        {
            
            Array<int> these = resets[PreparationTypeDirect];
            for (auto id : these) modificationMap[key]->directReset.add(id);
            
            these = resets[PreparationTypeSynchronic];
            for (auto id : these) modificationMap[key]->synchronicReset.add(id);
            
            these = resets[PreparationTypeNostalgic];
            for (auto id : these) modificationMap[key]->nostalgicReset.add(id);
            
            these = resets[PreparationTypeTuning];
            for (auto id : these) modificationMap[key]->tuningReset.add(id);
            
            these = resets[PreparationTypeTempo];
            for (auto id : these) modificationMap[key]->tempoReset.add(id);
            
            
            for (int i = otherKeys.size(); --i>=0;)
            {
                if (otherKeys[i] == key) otherKeys.remove(i);
            }
        }
    }
    
    
    deconfigureResetsForKeys(resets, otherKeys);
    
}

void Piano::deconfigureResets(Array<Array<int>> resets, Array<int> whichKeymaps)
{
    Array<int> otherKeys;
    
    for (int i = 0; i < 128; i++) otherKeys.add(i);
    
    for (auto keymap : whichKeymaps)
    {
        deconfigureResetsForKeys(resets, bkKeymaps->getUnchecked(keymap)->keys());
    }
    
}

void Piano::deconfigureResetsForKeys(Array<Array<int>> resets, Array<int> otherKeys)
{
    for (auto key : otherKeys)
    {
        
        Array<int> these = resets[PreparationTypeDirect];
        for (auto id : these)
        {
            for (int i = modificationMap[key]->directReset.size(); --i>=0;)
            {
                if (modificationMap[key]->directReset[i] == id) modificationMap[key]->directReset.remove(i);
            }
        }
        
        these = resets[PreparationTypeSynchronic];
        for (auto id : these)
        {
            for (int i = modificationMap[key]->synchronicReset.size(); --i>=0;)
            {
                if (modificationMap[key]->synchronicReset[i] == id) modificationMap[key]->synchronicReset.remove(i);
            }
        }
        
        these = resets[PreparationTypeNostalgic];
        for (auto id : these)
        {
            for (int i = modificationMap[key]->nostalgicReset.size(); --i>=0;)
            {
                if (modificationMap[key]->nostalgicReset[i] == id) modificationMap[key]->nostalgicReset.remove(i);
            }
        }
        
        these = resets[PreparationTypeTuning];
        for (auto id : these)
        {
            for (int i = modificationMap[key]->tuningReset.size(); --i>=0;)
            {
                if (modificationMap[key]->tuningReset[i] == id) modificationMap[key]->tuningReset.remove(i);
            }
        }
        
        these = resets[PreparationTypeTempo];
        for (auto id : these)
        {
            for (int i = modificationMap[key]->tempoReset.size(); --i>=0;)
            {
                if (modificationMap[key]->tempoReset[i] == id) modificationMap[key]->tempoReset.remove(i);
            }
        }
        
        
    }
}

void Piano::configureModification(ModificationMapper::Ptr map)
{
    map->print();
    DBG("numKeymaps: " + String(bkKeymaps->size()));
    
    Array<int> whichPreps = map->getTargets();
    Array<int> whichKeymaps = map->getKeymaps();

    BKPreparationType type = map->getType();
    int Id = map->getId();
    
    if (type == BKPreparationTypeNil) return;
    else if (type == PreparationTypePianoMap)
    {
        configurePianoMap(whichKeymaps, map->piano);
    }
    else if (type == PreparationTypeReset)
    {
        configureResets(map->resets, whichKeymaps, whichPreps);
    }
    else if (type == PreparationTypeDirect)
    {
        configureDirectModification(modDirect->getUnchecked(Id), whichKeymaps, whichPreps);
    }
    else if (type == PreparationTypeSynchronic)
    {
        configureSynchronicModification(modSynchronic->getUnchecked(Id), whichKeymaps, whichPreps);
    }
    else if (type == PreparationTypeNostalgic)
    {
        configureNostalgicModification(modNostalgic->getUnchecked(Id), whichKeymaps, whichPreps);
    }
    else if (type == PreparationTypeTuning)
    {
        configureTuningModification(modTuning->getUnchecked(Id), whichKeymaps, whichPreps);
    }
    else if (type == PreparationTypeTempo)
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

PreparationMap::Ptr        Piano::getPreparationMapWithKeymap(Keymap::Ptr thisKeymap)
{
    PreparationMap::Ptr thisPMap = nullptr;
    for (auto pmap : prepMaps)
    {
        if (pmap->getKeymap()->compare(thisKeymap))
        {
            thisPMap = pmap;
            break;
        }
    }
    return thisPMap;
}

// Add preparation map, return its Id.
int Piano::removePreparationMapWithKeymap(Keymap::Ptr thisKeymap)
{
    for (int i = activePMaps.size(); --i >= 0; )
    {
        if (activePMaps[i]->getKeymapId() == thisKeymap->getId())
        {
            activePMaps.remove(i);
            break;
        }
    }
    
    for (int i = prepMaps.size(); --i >= 0; )
    {
        if (prepMaps[i]->getKeymapId() == thisKeymap->getId())
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
    
    // Mapper Saving
    int mapCount = 0;
    for (auto map : mappers)
    {
        if ((map->getType() != PreparationTypeReset) && !map->getKeymaps().size()) continue;
        
        if ((map->getType() != PreparationTypeReset) && (!map->getKeymaps().size() || !map->getTargets().size())) continue;
        
        DBG("SAVING THIS MAPPER: ");
        map->print();
        
        ValueTree mapVT ("mapper"+String(mapCount++));
        
        mapVT.setProperty("type", map->getType(), 0);
        
        mapVT.setProperty("Id", map->getId(), 0);
        
        mapVT.setProperty("piano", map->piano,0);
        
        int pcount = 0;
        for (auto keymap : map->getKeymaps())
        {
            mapVT.setProperty("k"+String(pcount++), keymap, 0);
        }
        
        pcount = 0;
        for (auto target : map->getTargets())
        {
            mapVT.setProperty("t"+String(pcount++), target, 0);
        }
        
        for (int i = 0; i < 5; i++)
        {
            ValueTree resetVT( vtagReset + String(i));
            
            int rcount = 0;
            for (auto reset : map->resets[i])
            {
                resetVT.setProperty( "r" + String(rcount++), reset, 0);
            }
            
            mapVT.addChild(resetVT, -1, 0);
        }
        
        pianoVT.addChild(mapVT, -1, 0);

    }
    
    ValueTree configurationVT = configuration->getState();
    pianoVT.addChild(configurationVT, -1, 0);
    
    return pianoVT;
}

void Piano::setState(XmlElement* e)
{
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
            
            ModificationMapper::Ptr mapper = new ModificationMapper(type, thisId, keymaps, targets);
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
            
            prepMaps[prepMapCount]->setKeymap(keymap);
            
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
    
}



