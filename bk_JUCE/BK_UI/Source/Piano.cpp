/*
  ==============================================================================

    Piano.cpp
    Created: 7 Dec 2016 10:25:40am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "Piano.h"

Piano::Piano(Synchronic::PtrArr synchronic,
             Nostalgic::PtrArr nostalgic,
             Direct::PtrArr direct,
             Tuning::PtrArr tuning,
             Tempo::PtrArr tempo,
             SynchronicModPreparation::PtrArr mSynchronic,
             NostalgicModPreparation::PtrArr mNostalgic,
             DirectModPreparation::PtrArr mDirect,
             TuningModPreparation::PtrArr mTuning,
             TempoModPreparation::PtrArr mTempo,
             Keymap::PtrArr keymaps,
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
    
    modMap = OwnedArray<Modifications>();
    modMap.ensureStorageAllocated(128);
    
    modificationMaps = OwnedArray<ModificationMap>();
    modificationMaps.ensureStorageAllocated(128);
    
    for (int i = 0; i < 128; i++)
    {
        pianoMap.set(i, -1);
        modMap.add(new Modifications());
        modificationMaps.add(new ModificationMap());
    }
    
    prepMaps.ensureStorageAllocated(12);
    
    
    
}


Piano::~Piano()
{
    
}

void Piano::configureDirectModification(int key, DirectModPreparation::Ptr dmod, Array<int> whichPreps)
{
    DBG("CONFIGURING DIRECT MODIFICATION");
    
    Direct::PtrArr whichDirect = dmod->getTargets();
    
    DBG("key: " + String(key) + " mod: " + String(dmod->getId()) + " preps: " + intArrayToString(whichPreps));
    
    int whichMod = dmod->getId();
    
    // Find or make new ModPrepMap
    ModPrepMap::Ptr thisModPrepMap = modificationMaps[key]->getModPrepMap(PreparationTypeDirect, whichMod);
    
    if (thisModPrepMap == nullptr)
    {
        thisModPrepMap = new ModPrepMap(PreparationTypeDirect, whichMod);
        
        modificationMaps[key]->addModPrepMap(thisModPrepMap);
    }
    
    // Add Modifications
    for (int n = cDirectParameterTypes.size(); --n >= 0; )
    {
        String param = dmod->getParam((DirectParameterType)n);
        
        DBG("param: " + param);
        
        if (param != "")
        {
            for (auto prep : whichPreps)
            {
                modMap[key]->addDirectModification(new DirectModification(key, prep, (DirectParameterType)n, param, whichMod));
                
                thisModPrepMap->addPreparation(prep);
                
                DBG("ADD whichmod: " + String(whichMod) + " whichprep: " + String(prep) + " whichtype: " + cDirectParameterTypes[n] + " val: " +param + " TO key: " + String(key));
            }
        }
    }
}

void Piano::configureDirectModification(DirectModPreparation::Ptr mod)
{
    int whichMod = mod->getId();
    Array<int> whichPreps = getAllIds(mod->getTargets());
    Keymap::PtrArr whichKeymaps = mod->getKeymaps();
    
    Array<int> otherKeys;
    
    for (int i = 0; i < 128; i++) otherKeys.add(i);
    
    for (auto keymap : whichKeymaps)
    {
        for (auto key : keymap->keys())
        {
            configureDirectModification(key, mod, whichPreps);
            
            for (int i = otherKeys.size(); --i>=0;)
            {
                if (otherKeys[i] == key) otherKeys.remove(i);
            }
        }
    }
    
    for (auto key : otherKeys)
    {
        // Remove ModPrepMap from Key
        modificationMaps[key]->removeModPrepMap(PreparationTypeDirect, whichMod);
        
        // Remove Modification from Key
        modMap[key]->removeDirectModification(whichMod);
    }
}

void Piano::deconfigureDirectModificationForKeys(DirectModPreparation::Ptr mod, Array<int> keys)
{
    int whichMod = mod->getId();
    
    Keymap::PtrArr whichKeymaps = mod->getKeymaps();
    
    for (auto key : keys)
    {
        // Remove ModPrepMap from Key
        modificationMaps[key]->removeModPrepMap(PreparationTypeDirect, whichMod);
        
        // Remove Modification from Key
        modMap[key]->removeDirectModification(whichMod);
        
    }
}

void Piano::deconfigureDirectModification(DirectModPreparation::Ptr mod)
{
    int whichMod = mod->getId();
    
    Keymap::PtrArr whichKeymaps = mod->getKeymaps();
    
    for (auto keymap : whichKeymaps)
    {
        for (auto key : keymap->keys())
        {
            // Remove ModPrepMap from Key
            modificationMaps[key]->removeModPrepMap(PreparationTypeDirect, whichMod);
            
            // Remove Modification from Key
            modMap[key]->removeDirectModification(whichMod);
            
        }
    }
}

void Piano::configureNostalgicModification(int key, NostalgicModPreparation::Ptr dmod, Array<int> whichPreps)
{
    
    Nostalgic::PtrArr whichNostalgic = dmod->getTargets();
    
    DBG("key: " + String(key) + " mod: " + String(dmod->getId()) + " preps: " + intArrayToString(whichPreps));
    
    int whichMod = dmod->getId();
    
    // Find or make new ModPrepMap
    ModPrepMap::Ptr thisModPrepMap = modificationMaps[key]->getModPrepMap(PreparationTypeNostalgic, whichMod);
    
    if (thisModPrepMap == nullptr)
    {
        thisModPrepMap = new ModPrepMap(PreparationTypeNostalgic, whichMod);
        
        modificationMaps[key]->addModPrepMap(thisModPrepMap);
    }
    
    // Add Modifications
    for (int n = cNostalgicParameterTypes.size(); --n >= 0; )
    {
        String param = dmod->getParam((NostalgicParameterType)n);
        
        if (param != "")
        {
            for (auto prep : whichPreps)
            {
                modMap[key]->addNostalgicModification(new NostalgicModification(key, prep, (NostalgicParameterType)n, param, whichMod));
                
                thisModPrepMap->addPreparation(prep);
            }
        }
    }
}

void Piano::configureNostalgicModification(NostalgicModPreparation::Ptr mod)
{
    Array<int> whichPreps = getAllIds(mod->getTargets());
    Keymap::PtrArr whichKeymaps = mod->getKeymaps();
    
    Array<int> otherKeys;
    
    for (int i = 0; i < 128; i++) otherKeys.add(i);
    
    for (auto keymap : whichKeymaps)
    {
        for (auto key : keymap->keys())
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
    
    Keymap::PtrArr whichKeymaps = mod->getKeymaps();
    
    for (auto key : keys)
    {
        // Remove ModPrepMap from Key
        modificationMaps[key]->removeModPrepMap(PreparationTypeNostalgic, whichMod);
        
        // Remove Modification from Key
        modMap[key]->removeNostalgicModification(whichMod);
    }
}

void Piano::deconfigureNostalgicModification(NostalgicModPreparation::Ptr mod)
{
    int whichMod = mod->getId();
    
    Keymap::PtrArr whichKeymaps = mod->getKeymaps();
    
    for (auto keymap : whichKeymaps)
    {
        for (auto key : keymap->keys())
        {
            // Remove ModPrepMap from Key
            modificationMaps[key]->removeModPrepMap(PreparationTypeNostalgic, whichMod);
            
            // Remove Modification from Key
            modMap[key]->removeNostalgicModification(whichMod);
        }
    }
}


void Piano::configureSynchronicModification(int key, SynchronicModPreparation::Ptr dmod, Array<int> whichPreps)
{
    
    Synchronic::PtrArr whichSynchronic = dmod->getTargets();
    
    int whichMod = dmod->getId();
    
    // Find or make new ModPrepMap
    ModPrepMap::Ptr thisModPrepMap = modificationMaps[key]->getModPrepMap(PreparationTypeSynchronic, whichMod);
    
    if (thisModPrepMap == nullptr)
    {
        thisModPrepMap = new ModPrepMap(PreparationTypeSynchronic, whichMod);
        
        modificationMaps[key]->addModPrepMap(thisModPrepMap);
    }
    
    // Add Modifications
    for (int n = cSynchronicParameterTypes.size(); --n >= 0; )
    {
        String param = dmod->getParam((SynchronicParameterType)n);
        
        
        if (param != "")
        {
            for (auto prep : whichPreps)
            {
                modMap[key]->addSynchronicModification(new SynchronicModification(key, prep, (SynchronicParameterType)n, param, whichMod));
                
                thisModPrepMap->addPreparation(prep);
            }
        }
    }
}

void Piano::configureSynchronicModification(SynchronicModPreparation::Ptr mod)
{
    Array<int> whichPreps = getAllIds(mod->getTargets());
    Keymap::PtrArr whichKeymaps = mod->getKeymaps();
    
    Array<int> otherKeys;
    
    for (int i = 0; i < 128; i++) otherKeys.add(i);
    
    for (auto keymap : whichKeymaps)
    {
        for (auto key : keymap->keys())
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
    
    Keymap::PtrArr whichKeymaps = mod->getKeymaps();
    
    for (auto key : keys)
    {
        // Remove ModPrepMap from Key
        modificationMaps[key]->removeModPrepMap(PreparationTypeSynchronic, whichMod);
        
        // Remove Modification from Key
        modMap[key]->removeSynchronicModification(whichMod);
    }
}

void Piano::deconfigureSynchronicModification(SynchronicModPreparation::Ptr mod)
{
    int whichMod = mod->getId();
    
    Keymap::PtrArr whichKeymaps = mod->getKeymaps();
    
    for (auto keymap : whichKeymaps)
    {
        for (auto key : keymap->keys())
        {
            // Remove ModPrepMap from Key
            modificationMaps[key]->removeModPrepMap(PreparationTypeSynchronic, whichMod);
            
            // Remove Modification from Key
            modMap[key]->removeSynchronicModification(whichMod);
        }
    }
}

void Piano::configureTempoModification(int key, TempoModPreparation::Ptr dmod, Array<int> whichPreps)
{
    
    Tempo::PtrArr whichTempo = dmod->getTargets();
    
    DBG("key: " + String(key) + " mod: " + String(dmod->getId()) + " preps: " + intArrayToString(whichPreps));
    
    int whichMod = dmod->getId();
    
    // Find or make new ModPrepMap
    ModPrepMap::Ptr thisModPrepMap = modificationMaps[key]->getModPrepMap(PreparationTypeTempo, whichMod);
    
    if (thisModPrepMap == nullptr)
    {
        thisModPrepMap = new ModPrepMap(PreparationTypeTempo, whichMod);
        
        modificationMaps[key]->addModPrepMap(thisModPrepMap);
    }
    
    // Add Modifications
    for (int n = cTempoParameterTypes.size(); --n >= 0; )
    {
        String param = dmod->getParam((TempoParameterType)n);
        
        DBG("param: " + param);
        
        if (param != "")
        {
            for (auto prep : whichPreps)
            {
                modMap[key]->addTempoModification(new TempoModification(key, prep, (TempoParameterType)n, param, whichMod));
                
                thisModPrepMap->addPreparation(prep);
                
                DBG("ADD whichmod: " + String(whichMod) + " whichprep: " + String(prep) + " whichtype: " + cTempoParameterTypes[n] + " val: " +param + " TO key: " + String(key));
            }
        }
    }
}


void Piano::configureTempoModification(TempoModPreparation::Ptr mod)
{
    Array<int> whichPreps = getAllIds(mod->getTargets());
    Keymap::PtrArr whichKeymaps = mod->getKeymaps();
    
    Array<int> otherKeys;
    
    for (int i = 0; i < 128; i++) otherKeys.add(i);
    
    for (auto keymap : whichKeymaps)
    {
        for (auto key : keymap->keys())
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
    
    Keymap::PtrArr whichKeymaps = mod->getKeymaps();
    
    for (auto key : keys)
    {
        // Remove ModPrepMap from Key
        modificationMaps[key]->removeModPrepMap(PreparationTypeTempo, whichMod);
        
        // Remove Modification from Key
        modMap[key]->removeTempoModification(whichMod);
        
        DBG("REMOVE whichmod: " + String(whichMod) + " FROM key: " +String(key));
    }
}

void Piano::deconfigureTempoModification(TempoModPreparation::Ptr mod)
{
    int whichMod = mod->getId();
    
    Keymap::PtrArr whichKeymaps = mod->getKeymaps();
    
    for (auto keymap : whichKeymaps)
    {
        for (auto key : keymap->keys())
        {
            // Remove ModPrepMap from Key
            modificationMaps[key]->removeModPrepMap(PreparationTypeTempo, whichMod);
            
            // Remove Modification from Key
            modMap[key]->removeTempoModification(whichMod);
            
            DBG("REMOVE whichmod: " + String(whichMod) + " FROM key: " +String(key));
        }
    }
}

void Piano::configureTuningModification(int key, TuningModPreparation::Ptr dmod, Array<int> whichPreps)
{
    DBG("CONFIGURING Tuning MODIFICATION");
    
    Tuning::PtrArr whichTuning = dmod->getTargets();
    
    DBG("key: " + String(key) + " mod: " + String(dmod->getId()) + " preps: " + intArrayToString(whichPreps));
    
    int whichMod = dmod->getId();
    
    // Find or make new ModPrepMap
    ModPrepMap::Ptr thisModPrepMap = modificationMaps[key]->getModPrepMap(PreparationTypeTuning, whichMod);
    
    if (thisModPrepMap == nullptr)
    {
        thisModPrepMap = new ModPrepMap(PreparationTypeTuning, whichMod);
        
        modificationMaps[key]->addModPrepMap(thisModPrepMap);
    }
    
    // Add Modifications
    for (int n = cTuningParameterTypes.size(); --n >= 0; )
    {
        String param = dmod->getParam((TuningParameterType)n);
        
        DBG("param: " + param);
        
        if (param != "")
        {
            for (auto prep : whichPreps)
            {
                modMap[key]->addTuningModification(new TuningModification(key, prep, (TuningParameterType)n, param, whichMod));
                
                thisModPrepMap->addPreparation(prep);
                
                DBG("ADD whichmod: " + String(whichMod) + " whichprep: " + String(prep) + " whichtype: " + cTuningParameterTypes[n] + " val: " +param + " TO key: " + String(key));
            }
        }
    }
}

void Piano::configureTuningModification(TuningModPreparation::Ptr mod)
{
    Array<int> whichPreps = getAllIds(mod->getTargets());
    Keymap::PtrArr whichKeymaps = mod->getKeymaps();
    
    Array<int> otherKeys;
    
    for (int i = 0; i < 128; i++) otherKeys.add(i);
    
    for (auto keymap : whichKeymaps)
    {
        for (auto key : keymap->keys())
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
    
    Keymap::PtrArr whichKeymaps = mod->getKeymaps();
    
    for (auto key : keys)
    {
        // Remove ModPrepMap from Key
        modificationMaps[key]->removeModPrepMap(PreparationTypeTuning, whichMod);
        
        // Remove Modification from Key
        modMap[key]->removeTuningModification(whichMod);
        
        DBG("REMOVE whichmod: " + String(whichMod) + " FROM key: " +String(key));
    }
}

void Piano::deconfigureTuningModification(TuningModPreparation::Ptr mod)
{
    int whichMod = mod->getId();
    
    Keymap::PtrArr whichKeymaps = mod->getKeymaps();
    
    for (auto keymap : whichKeymaps)
    {
        for (auto key : keymap->keys())
        {
            // Remove ModPrepMap from Key
            modificationMaps[key]->removeModPrepMap(PreparationTypeTuning, whichMod);
            
            // Remove Modification from Key
            modMap[key]->removeTuningModification(whichMod);
            
            DBG("REMOVE whichmod: " + String(whichMod) + " FROM key: " +String(key));
        }
    }
}


// Add preparation map, return its Id.
int Piano::addPreparationMap(void)
{
    prepMaps.add(new PreparationMap(bkKeymaps[0],
                                    numPMaps));
    
    prepMaps[numPMaps]->prepareToPlay(sampleRate);
    
    activePMaps.addIfNotAlreadyThere(prepMaps[numPMaps]);
    
    ++numPMaps;
    
    
    return numPMaps-1;
}

// Add preparation map, return its Id.
int Piano::addPreparationMap(Keymap::Ptr keymap)
{
    prepMaps.add(new PreparationMap(bkKeymaps[0],
                                    numPMaps));
    
    prepMaps[numPMaps]->prepareToPlay(sampleRate);
    
    prepMaps[numPMaps]->setKeymap(keymap);
    
    activePMaps.addIfNotAlreadyThere(prepMaps[numPMaps]);
    
    ++numPMaps;
    
    
    return numPMaps-1;
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
    
    int pmapCount = 0;
    for (auto pmap : getPreparationMaps())
    {
        ValueTree pmapVT( vtagPrepMap+String(pmapCount++));
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
    
    pmapCount = 0;
    
    int resetCount = 0, modCount = 0;
    
    // Iterate through all keys and write data from PianoMap and ModMap to ValueTree
    for (int key = 0; key < 128; key++)
    {
        if (pianoMap[key] != 0)
        {
            ValueTree pmapVT( vtagPianoMap + String(pmapCount++));
            
            pmapVT.setProperty( ptagPianoMap_key, key, 0);
            pmapVT.setProperty( ptagPianoMap_piano, pianoMap[key], 0);
            
            pianoVT.addChild(pmapVT, -1, 0);
        }
        
        ModPrepMap::PtrArr theMods = modificationMaps[key]->getModPrepMaps();
        
        if (theMods.size())
        {
            ValueTree modVT ("mod"+String(modCount++));
            modVT.setProperty( ptagModX_key, key, 0);
            
            for (int thisOne = 0; thisOne < theMods.size(); thisOne++)
            {
                ModPrepMap::Ptr thisMod = theMods[thisOne];
                
                ValueTree thisModVT("m"+String(thisOne));
                thisModVT.setProperty("type", (int)thisMod->getType(), 0);
                thisModVT.setProperty("id", thisMod->getId(), 0);
                
                int pcount = 0;
                for (auto prep : thisMod->getPreparations())
                    thisModVT.setProperty("p"+String(pcount++), prep, 0);
                
                modVT.addChild(thisModVT, -1, 0);
            }
            
            pianoVT.addChild(modVT, -1, 0);
        }
        
        // RESET SAVING
        if (modMap[key]->directReset.size() ||
            modMap[key]->nostalgicReset.size() ||
            modMap[key]->synchronicReset.size() ||
            modMap[key]->tuningReset.size()||
            modMap[key]->tempoReset.size())
        {
            ValueTree resetVT( vtagReset + String(resetCount++));
            resetVT.setProperty( ptagModX_key, key, 0);
            
            int rcount = 0;
            for (auto reset : modMap[key]->directReset)
                resetVT.setProperty( "d" + String(rcount++), reset, 0);
            
            rcount = 0;
            for (auto reset : modMap[key]->synchronicReset)
                resetVT.setProperty( "s" + String(rcount++), reset, 0);
            
            rcount = 0;
            for (auto reset : modMap[key]->nostalgicReset)
                resetVT.setProperty( "n" + String(rcount++), reset, 0);
            
            rcount = 0;
            for (auto reset : modMap[key]->tuningReset)
                resetVT.setProperty( "t" + String(rcount++), reset, 0);
            
            rcount = 0;
            for (auto reset : modMap[key]->tempoReset)
                resetVT.setProperty( "m" + String(rcount++), reset, 0);
            
            pianoVT.addChild(resetVT, -1, 0);
        }
    }
    
    return pianoVT;
}

void Piano::setState(XmlElement* e)
{
    int i; float f;
    int pianoMapCount = 0, prepMapCount = 0, modCount = 0, resetCount = 0;
    
    String pianoName = e->getStringAttribute("bkPianoName");
    
    if (pianoName != String::empty)
        setName(e->getStringAttribute("bkPianoName"));
    
    forEachXmlChildElement (*e, pc)
    {
        
        if (pc->hasTagName( vtagPianoMap + String(pianoMapCount)))
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
            Keymap::Ptr keymap = bkKeymaps[i];
            
            addPreparationMap(); // should clean up this functionality . pretty bad
            
            prepMaps[prepMapCount]->setKeymap(keymap);
            
            Synchronic::PtrArr sync;
            for (int k = 0; k < 128; k++)
            {
                String attr = pc->getStringAttribute(ptagPrepMap_synchronicPrepId + String(k));
                
                if (attr == String::empty)  break;
                else                        sync.add(synchronic[attr.getIntValue()]);
                
            }
            prepMaps[prepMapCount]->setSynchronic(sync);
            
            Nostalgic::PtrArr nost;
            for (int k = 0; k < 128; k++)
            {
                String attr = pc->getStringAttribute(ptagPrepMap_nostalgicPrepId + String(k));
                
                if (attr == String::empty)  break;
                else                        nost.add(nostalgic[attr.getIntValue()]);
                
            }
            prepMaps[prepMapCount]->setNostalgic(nost);
            
            
            Direct::PtrArr drct;
            for (int k = 0; k < 128; k++)
            {
                String attr = pc->getStringAttribute(ptagPrepMap_directPrepId + String(k));
                
                if (attr == String::empty)  break;
                else                        drct.add(direct[attr.getIntValue()]);
                
            }
            prepMaps[prepMapCount]->setDirect(drct);
            
            Tuning::PtrArr tune;
            for (int k = 0; k < 128; k++)
            {
                String attr = pc->getStringAttribute(ptagPrepMap_tuningPrepId + String(k));
                
                if (attr == String::empty)  break;
                else                        tune.add(tuning[attr.getIntValue()]);
                
            }
            prepMaps[prepMapCount]->setTuning(tune);
            
            Tempo::PtrArr tmp;
            for (int k = 0; k < 128; k++)
            {
                String attr = pc->getStringAttribute(ptagPrepMap_tempoPrepId + String(k));
                
                if (attr == String::empty)  break;
                else                        tmp.add(tempo[attr.getIntValue()]);
                
            }
            prepMaps[prepMapCount]->setTempo(tmp);
            
            ++prepMapCount;
        }
        else if (pc->hasTagName( "mod" + String(modCount)))
        {
            int key = pc->getStringAttribute(ptagModX_key).getIntValue();
            
            int subModCount = 0;
            forEachXmlChildElement (*pc, mod)
            {
                if (mod->hasTagName("m"+String(subModCount)))
                {
                    BKPreparationType type = (BKPreparationType)mod->getStringAttribute("type").getIntValue();
                    int whichMod = mod->getStringAttribute("id").getIntValue();
                    
                    Array<int> whichPreps;
                    for (int p = 0; p < 200; p++) //arbitrary 200
                    {
                        String attr = mod->getStringAttribute("p" + String(p));
                        
                        if (attr == String::empty)  break;
                        else                        whichPreps.add(attr.getIntValue());
                    }
                    
                    if (whichPreps.size())
                        modificationMaps[key]->addModPrepMap(new ModPrepMap(type, whichMod, whichPreps));
                    
                    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ PUT IN OWN FUNCTION IN PIANOOOOO ~ ~ ~ ~ ~ ~ ~ ~ ~
                    if (type == PreparationTypeDirect)
                    {
                        DirectModPreparation::Ptr dmod = modDirect[whichMod];
                        
                        for (int n = cDirectParameterTypes.size(); --n >= 0; )
                        {
                            String param = dmod->getParam((DirectParameterType)n);
                            
                            if (param != "")
                            {
                                for (auto prep : whichPreps)
                                {
                                    modMap[key]->addDirectModification(new DirectModification(key, prep, (DirectParameterType)n, param, whichMod));
                                    
                                    DBG("[ADDINGMOD] whichmod: " + String(whichMod) +" whichprep: " + String(prep) + " whichtype: " + cDirectParameterTypes[n] + " val: " +param);
                                    
                                }
                                
                                
                            }
                        }
                    }
                    else if (type == PreparationTypeSynchronic)
                    {
                        SynchronicModPreparation::Ptr smod = modSynchronic[whichMod];
                        
                        for (int n = cSynchronicParameterTypes.size(); --n >= 0; )
                        {
                            String param = smod->getParam((SynchronicParameterType)n);
                            
                            if (param != "")
                            {
                                for (auto prep : whichPreps)
                                {
                                    modMap[key]->addSynchronicModification(new SynchronicModification(key, prep, (SynchronicParameterType)n, param, whichMod));
                                    
                                    DBG("[ADDINGMOD] whichmod: " + String(whichMod) +" whichprep: " + String(prep) + " whichtype: " + cSynchronicParameterTypes[n] + " val: " +param);
                                    
                                }
                                
                                
                            }
                        }
                    }
                    else if (type == PreparationTypeNostalgic)
                    {
                        NostalgicModPreparation::Ptr nmod = modNostalgic[whichMod];
                        
                        for (int n = cNostalgicParameterTypes.size(); --n >= 0; )
                        {
                            String param = nmod->getParam((NostalgicParameterType)n);
                            
                            if (param != "")
                            {
                                for (auto prep : whichPreps)
                                {
                                    modMap[key]->addNostalgicModification(new NostalgicModification(key, prep, (NostalgicParameterType)n, param, whichMod));
                                    
                                    DBG("[ADDINGMOD] whichmod: " + String(whichMod) +" whichprep: " + String(prep) + " whichtype: " + cNostalgicParameterTypes[n] + " val: " +param);
                                    
                                }
                                
                                
                            }
                        }
                    }
                    else if (type == PreparationTypeTuning)
                    {
                        TuningModPreparation::Ptr tmod = modTuning[whichMod];
                        
                        for (int n = cTuningParameterTypes.size(); --n >= 0; )
                        {
                            String param = tmod->getParam((TuningParameterType)n);
                            
                            if (param != "")
                            {
                                for (auto prep : whichPreps)
                                {
                                    modMap[key]->addTuningModification(new TuningModification(key, prep, (TuningParameterType)n, param, whichMod));
                                    
                                    DBG("[ADDINGMOD] whichmod: " + String(whichMod) +" whichprep: " + String(prep) + " whichtype: " + cTuningParameterTypes[n] + " val: " +param);
                                    
                                }
                                
                                
                            }
                        }
                    }
                    else if (type == PreparationTypeTempo)
                    {
                        TempoModPreparation::Ptr mmod = modTempo[whichMod];
                        
                        for (int n = cTempoParameterTypes.size(); --n >= 0; )
                        {
                            String param = mmod->getParam((TempoParameterType)n);
                            
                            if (param != "")
                            {
                                for (auto prep : whichPreps)
                                {
                                    modMap[key]->addTempoModification(new TempoModification(key, prep, (TempoParameterType)n, param, whichMod));
                                    
                                    DBG("[ADDINGMOD] whichmod: " + String(whichMod) +" whichprep: " + String(prep) + " whichtype: " + cTempoParameterTypes[n] + " val: " +param);
                                    
                                }
                                
                                
                            }
                        }
                    }
                    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
                    
                    ++subModCount;
                }
                
                
                
            }
            
            
            ++modCount;
        }
        else if (pc->hasTagName( vtagReset + String(resetCount))) // RESET LOADING
        {
            int k = pc->getStringAttribute(ptagModX_key).getIntValue();
            
            
            // Direct resets
            for (int n = 0; n < 128; n++)
            {
                String attr = pc->getStringAttribute("d" + String(n));
                
                if (attr == String::empty)  break;
                else                        modMap[k]->directReset.add(attr.getIntValue());
            }
            
            // Synchronic resets
            for (int n = 0; n < 128; n++)
            {
                String attr = pc->getStringAttribute("s" + String(n));
                
                if (attr == String::empty)  break;
                else                        modMap[k]->synchronicReset.add(attr.getIntValue());
            }
            
            // Nostalgic resets
            for (int n = 0; n < 128; n++)
            {
                String attr = pc->getStringAttribute("n" + String(n));
                
                if (attr == String::empty)  break;
                else                        modMap[k]->nostalgicReset.add(attr.getIntValue());
            }
            
            // Tuning resets
            for (int n = 0; n < 128; n++)
            {
                String attr = pc->getStringAttribute("t" + String(n));
                
                if (attr == String::empty)  break;
                else                        modMap[k]->tuningReset.add(attr.getIntValue());
            }
            
            // Tempo resets
            for (int n = 0; n < 128; n++)
            {
                String attr = pc->getStringAttribute("m" + String(n));
                
                if (attr == String::empty)  break;
                else                        modMap[k]->tempoReset.add(attr.getIntValue());
            }
            
            ++resetCount;
        }
        
    }
    
}



