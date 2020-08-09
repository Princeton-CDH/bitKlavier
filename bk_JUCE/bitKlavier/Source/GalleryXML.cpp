/*
  ==============================================================================

    XMLGallery.cpp
    Created: 22 Mar 2017 12:31:17pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "PluginProcessor.h"

void Gallery::addFromXML(XmlElement* xml)
{
    
}

int Gallery::transformId(BKPreparationType type, int oldId)
{
    int newId = 0;
    
    if (oldId == -1) return -1;
    
    if (idmap[type]->contains(oldId))
    {
        newId = idmap[type]->getReference(oldId);
    }
    else
    {
        newId = idcounts[type]++;
        idmap[type]->set(oldId, newId);
        
        // DBG("TRANS " + String(cPreparationTypes[type]) + " old: " + String(oldId) + " new: " + String(newId));
    }
    
    return newId;
}

ValueTree  Gallery::getState(void)
{
    ValueTree galleryVT( vtagGallery);
    
    galleryVT.setProperty("name", name, 0);
    
    galleryVT.setProperty("sampleType", processor.globalSampleType, 0);
    galleryVT.setProperty("soundfontURL", processor.globalSoundfont, 0);
    galleryVT.setProperty("soundfontInst", processor.globalInstrument, 0);
    
    // We don't do anything with these on loading so don't see why we should save them
//    ValueTree idCountVT( "idcounts");
//
//    for (int i = 0; i < BKPreparationTypeNil; i++)
//    {
//        idCountVT.setProperty( "i"+String(i), idcounts[i], 0);
//    }
//
//    galleryVT.addChild(idCountVT, -1, 0);
    
    galleryVT.addChild(general->getState(), -1, 0);
    
    // Preparations and keymaps must be first.
    // Tempo and Tuning must be first of the preparations.
    for (int i = 0; i < tempo.size(); i++)                  galleryVT.addChild( tempo[i]->getState(), -1, 0);
    
    for (int i = 0; i < tuning.size(); i++)                 galleryVT.addChild( tuning[i]->getState(), -1, 0);
    
    for (int i = 0; i < direct.size(); i++)                 galleryVT.addChild( direct[i]->getState(), -1, 0);
    
    for (int i = 0; i < synchronic.size(); i++)             galleryVT.addChild( synchronic[i]->getState(), -1, 0);
    
    for (int i = 0; i < nostalgic.size(); i++)              galleryVT.addChild( nostalgic[i]->getState(), -1, 0);
    
    for (int i = 0; i < blendronic.size(); i++)           galleryVT.addChild( blendronic[i]->getState(), -1, 0);
    
    for (int i = 0; i < modTempo.size(); i++)               galleryVT.addChild( modTempo[i]->getState(), -1, 0);
    
    for (int i = 0; i < modTuning.size(); i++)              galleryVT.addChild( modTuning[i]->getState(), -1, 0);
    
    for (int i = 0; i < modDirect.size(); i++)              galleryVT.addChild( modDirect[i]->getState(), -1, 0);
    
    for (int i = 0; i < modSynchronic.size(); i++)          galleryVT.addChild( modSynchronic[i]->getState(), -1, 0);
    
    for (int i = 0; i < modNostalgic.size(); i++)           galleryVT.addChild( modNostalgic[i]->getState(), -1, 0);
    
    for (int i = 0; i < modBlendronic.size(); i++)        galleryVT.addChild( modBlendronic[i]->getState(), -1, 0);
    
    for (int i = 0; i < bkKeymaps.size(); i++)              galleryVT.addChild( bkKeymaps[i]->getState(), -1, 0);
    
    // Pianos
    for (int piano = 0; piano < bkPianos.size(); piano++)   galleryVT.addChild( bkPianos[piano]->getState(), -1, 0);
    
    galleryVT.setProperty("defaultPiano", getDefaultPiano(), 0);
    
    return galleryVT;
}

void Gallery::setStateFromXML(XmlElement* xml)
{
    int i;
    Array<float> fa;
    Array<int> fi;
    
    bkPianos.clear();
    idmap.clear();
    
    for (int i = 0; i < BKPreparationTypeNil; i++)
    {
        idmap.add(new HashMap<int, int>());
        idcounts[i] = 1;
    }
    
    if (xml != nullptr)
    {
        name = xml->getStringAttribute("name");
        
        setDefaultPiano(xml->getStringAttribute("defaultPiano").getIntValue());
        
        // iterate through its sub-elements
        forEachXmlChildElement (*xml, e)
        {
            if (e->hasTagName( vtagKeymap))
            {
                // TODO: why not use keymap setState()?
                
                addKeymapWithId(0);

                bkKeymaps.getLast()->setState(e);

                int oldId = bkKeymaps.getLast()->getId();
                int newId = transformId(PreparationTypeKeymap, oldId);

                bkKeymaps.getLast()->setId(newId);

                /*addKeymapWithId(0);
                
                String n = e->getStringAttribute("name");
                
                Keymap::Ptr newKeymap = bkKeymaps.getLast();
                
                int oldId = e->getStringAttribute("Id").getIntValue();
                int newId = transformId(PreparationTypeKeymap, oldId);
                
                newKeymap->setId(newId);
                
                if (n != String())     newKeymap->setName(n);
                
                Array<int> keys;
                for (int k = 0; k < 128; k++)
                {
                    String attr = e->getStringAttribute(ptagKeymap_key + String(k));
                    
                    if (attr == String()) break;
                    else
                    {
                        i = attr.getIntValue();
                        keys.add(i);
                    }
                }
                
                newKeymap->setKeymap(keys);
                
                Array<KeymapTargetState> targetStates;
                targetStates.ensureStorageAllocated(TargetTypeNil);
                for (int i = 0; i < TargetTypeNil; ++i)
                {
                    targetStates.add(TargetStateNil);
                    String attr = e->getStringAttribute(ptagKeymap_targetStates + String(i));
                    
                    if (attr != String())
                    {
                        targetStates.setUnchecked(i, (KeymapTargetState) attr.getIntValue());
                    }
                }
                
                newKeymap->setTargetStates(targetStates);
                
                String i = e->getStringAttribute(ptagKeymap_inverted);
                if (i != String()) newKeymap->setInverted(i.getIntValue());
                
                forEachXmlChildElement (*e, sub)
                {
                    if (sub->hasTagName(vtagKeymap_midiInputs))
                    {
                        Array<String> inputs;
                        for (int k = 0; k < sub->getNumAttributes(); k++)
                        {
                            String attr = sub->getStringAttribute(ptagKeymap_midiInput + String(k));
                            if (attr == String()) continue;
                            inputs.add(attr);
                        }
                        
                        newKeymap->setMidiInputSources(inputs);
                    }
                }
                
                String d = e->getStringAttribute(ptagKeymap_defaultSelected);
                if (d != String()) newKeymap->setDefaultSelected(d.getIntValue());
                */
            }
            else if (e->hasTagName ( vtagGeneral))
            {
                general->setState(e);
            }
            else if (e->hasTagName( vtagTuning))
            {
                addTuningWithId(0);
                
                tuning.getLast()->setState(e);
                
                int oldId = tuning.getLast()->getId();
                int newId = transformId(PreparationTypeTuning, oldId);
                
                tuning.getLast()->setId(newId);
                
            }
            else if (e->hasTagName( vtagModTuning))
            {
                addTuningModWithId(0);
                
                modTuning.getLast()->setState(e);
                
                int oldId = modTuning.getLast()->getId();
                int newId = transformId(PreparationTypeTuningMod, oldId);
                
                modTuning.getLast()->setId(newId);
            }
            else if (e->hasTagName( vtagDirect))
            {
                addDirectWithId(0);
                
                direct.getLast()->setState(e);
                
                int oldId = direct.getLast()->getId();
                int newId = transformId(PreparationTypeDirect, oldId);
                
                direct.getLast()->setId(newId);
            }
            else if (e->hasTagName( vtagModDirect))
            {
                addDirectModWithId(0);
                
                modDirect.getLast()->setState(e);
                
                int oldId = modDirect.getLast()->getId();
                int newId = transformId(PreparationTypeDirectMod, oldId);
                
                modDirect.getLast()->setId(newId);
            }
            else if (e->hasTagName( vtagSynchronic))
            {
                addSynchronicWithId(0);
                
                synchronic.getLast()->setState(e);
                
                int oldId = synchronic.getLast()->getId();
                int newId = transformId(PreparationTypeSynchronic, oldId);
                
                synchronic.getLast()->setId(newId);
            }
            else if (e->hasTagName( vtagModSynchronic))
            {
                addSynchronicModWithId(0);
            
                modSynchronic.getLast()->setState(e);
                
                int oldId = modSynchronic.getLast()->getId();
                int newId = transformId(PreparationTypeSynchronicMod, oldId);
                
                modSynchronic.getLast()->setId(newId);
                
            }
            else if (e->hasTagName( vtagTempo))
            {
                addTempoWithId(0);

                tempo.getLast()->setState(e);
                
                int oldId = tempo.getLast()->getId();
                int newId = transformId(PreparationTypeTempo, oldId);
                
                tempo.getLast()->setId(newId);
                
            }
            else if (e->hasTagName( vtagModTempo))
            {
                addTempoModWithId(0);
                
                modTempo.getLast()->setState(e);
                
                int oldId = modTempo.getLast()->getId();
                int newId = transformId(PreparationTypeTempoMod, oldId);
                
                modTempo.getLast()->setId(newId);
                
            }
            else if (e->hasTagName( vtagNostalgic))
            {
                addNostalgicWithId(0);
                
                nostalgic.getLast()->setState(e);
                
                int oldId = nostalgic.getLast()->getId();
                int newId = transformId(PreparationTypeNostalgic, oldId);
                
                nostalgic.getLast()->setId(newId);
            }
            else if (e->hasTagName( vtagModNostalgic))
            {
                addNostalgicModWithId(0);
                
                modNostalgic.getLast()->setState(e);
                
                int oldId = modNostalgic.getLast()->getId();
                int newId = transformId(PreparationTypeNostalgicMod, oldId);
                
                modNostalgic.getLast()->setId(newId);
            }
            else if (e->hasTagName( vtagBlendronic))
            {
                addBlendronicWithId(0);
                
                // TODO: write setState for Blendronic
                blendronic.getLast()->setState(e);
                
                int oldId = blendronic.getLast()->getId();
                int newId = transformId(PreparationTypeBlendronic, oldId);
                
                blendronic.getLast()->setId(newId);
            }
            else if (e->hasTagName( vtagModBlendronic))
            {
                addBlendronicModWithId(0);
                
                modBlendronic.getLast()->setState(e);
                
                int oldId = modBlendronic.getLast()->getId();
                int newId = transformId(PreparationTypeBlendronicMod, oldId);
                
                modBlendronic.getLast()->setId(newId);
            }
            else if (e->hasTagName(vtagPiano))
            {
                addPianoWithId(0);
                
                Piano::Ptr thisPiano = bkPianos.getLast();
 
                int oldId = e->getStringAttribute("Id").getIntValue();
                int newId = transformId(PreparationTypePiano, oldId);
                
                thisPiano->setId(newId);
            }
        }
        
        int which = 0;
        forEachXmlChildElement (*xml, e)
        {
            if (e->hasTagName(vtagPiano))
            {
                Piano::Ptr thisPiano = bkPianos[which++];
                
                thisPiano->setState(e, &idmap, idcounts);
            }
        }
    }
}


