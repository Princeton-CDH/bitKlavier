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
    
    // Do we want to be saving global soundfont info to the gallery? We're not loading it for galleries...
    galleryVT.setProperty("sampleType", processor.globalSampleType, 0);

    File soundfont(processor.globalSoundfont);
    if (soundfont.exists()) galleryVT.setProperty("soundfontURL", soundfont.getFileName(), 0);
    else galleryVT.setProperty("soundfontURL", processor.globalSoundfont.fromLastOccurrenceOf("/", false, true), 0);

    galleryVT.setProperty("soundfontInst", processor.globalInstrument, 0);
    
    // We don't do anything with these on loading so don't see why should we save them?
//    ValueTree idCountVT( "idcounts");
//
//    for (int i = 0; i < BKPreparationTypeNil; i++)
//    {
//        idCountVT.setProperty( "i"+String(i), idcounts[i], 0);
//    }
//
//    galleryVT.addChild(idCountVT, -1, 0);
    
    galleryVT.addChild(general->getState(), -1, 0);
    
    galleryVT.addChild(processor.getBKEqualizer()->getState(), -1, 0);
    
    galleryVT.addChild(processor.getCompressor()->getState(), -1, 0);
    // Preparations and keymaps must be first.
    // Tempo and Tuning must be first of the preparations.
    for (int i = 0; i < tempo.size(); i++)          galleryVT.addChild( tempo[i]->getState(), -1, 0);
    
    for (int i = 0; i < tuning.size(); i++)         galleryVT.addChild( tuning[i]->getState(), -1, 0);
    
    for (int i = 0; i < direct.size(); i++)         galleryVT.addChild( direct[i]->getState(), -1, 0);
    
    for (int i = 0; i < synchronic.size(); i++)     galleryVT.addChild( synchronic[i]->getState(), -1, 0);
    
    for (int i = 0; i < nostalgic.size(); i++)      galleryVT.addChild( nostalgic[i]->getState(), -1, 0);
    
    for (int i = 0; i < blendronic.size(); i++)     galleryVT.addChild( blendronic[i]->getState(), -1, 0);
    
    for (int i = 0; i < resonance.size(); i++)      galleryVT.addChild( resonance[i]->getState(), -1, 0);
    
    for (int i = 0; i < modTempo.size(); i++)       galleryVT.addChild( modTempo[i]->getState(), -1, 0);
    
    for (int i = 0; i < modTuning.size(); i++)      galleryVT.addChild( modTuning[i]->getState(), -1, 0);
    
    for (int i = 0; i < modDirect.size(); i++)      galleryVT.addChild( modDirect[i]->getState(), -1, 0);
    
    for (int i = 0; i < modSynchronic.size(); i++)  galleryVT.addChild( modSynchronic[i]->getState(), -1, 0);
    
    for (int i = 0; i < modNostalgic.size(); i++)   galleryVT.addChild( modNostalgic[i]->getState(), -1, 0);
    
    for (int i = 0; i < modResonance.size(); i++)   galleryVT.addChild( modResonance[i]->getState(), -1, 0);
    
    for (int i = 0; i < modBlendronic.size(); i++)  galleryVT.addChild( modBlendronic[i]->getState(), -1, 0);
    
    for (int i = 0; i < bkKeymaps.size(); i++)      galleryVT.addChild( bkKeymaps[i]->getState(), -1, 0);
    
    // Pianos
    for (int piano = 0; piano < bkPianos.size(); piano++)
    {
        galleryVT.addChild( bkPianos[piano]->getState(), -1, 0);
    }
    
    
    galleryVT.setProperty("defaultPiano", getDefaultPiano(), 0);
    
    galleryVT.addChild(pianoIteratorOrder.getState(), -1, 0);
    galleryVT.addChild(iteratorUpKeymap->getState(), -1, 0);
    galleryVT.addChild(iteratorDownKeymap->getState(), -1,0);
    galleryVT.setProperty("iteratorIsEnabled",iteratorIsEnabled,0);
    return galleryVT;
}

void Gallery::setStateFromXML(XmlElement* xml)
{
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
        //// load global sound font
        if(xml->hasAttribute("sampleType"))
        {
            processor.globalSampleType = (BKSampleLoadType)xml->getStringAttribute("sampleType").getIntValue();
        } else
        {
            processor.globalSampleType = BKLoadHeavy;
        }
        if(xml->hasAttribute("soundfontURL") && processor.globalSampleType >= BKLoadSoundfont)
        {
            String soundfont = xml->getStringAttribute("soundfontURL");
            
            //File soundfont(processor.globalSoundfont);
            if (soundfont != "")
            {
                if(soundfont.contains("/"))
                {
                    soundfont = soundfont.fromLastOccurrenceOf("/", false, true);
                }
                Array<File> files = processor.getSoundfontsSearchPath().findChildFiles(File::findFiles, true,soundfont);
                if (files.isEmpty() && processor.globalSampleType > BKLoadSoundfont)
                {
                    files = processor.getCustomSamplesSearchPath().findChildFiles(File::findDirectories, true, soundfont);

                }
                
                if (files.isEmpty())
                {
                    AlertWindow::showMessageBox(juce::MessageBoxIconType::WarningIcon, "File Not Found!", "Could not find file '" + soundfont + "' please check that it is in the custom sample or soundfont search path");
                    processor.globalSampleType = BKLoadLite;
                    processor.globalSoundfont = "default.sf2";
                } else
                {
                    processor.globalSoundfont = files.getFirst().getFullPathName();
                }
            }
        } else
        {
            processor.globalSampleType = BKLoadLite;
            processor.globalSoundfont = "default.sf2";
        }
        

        processor.globalInstrument = (int) xml->getStringAttribute("soundfontInst").getIntValue();
        processor.loadSamples(processor.globalSampleType, processor.globalSoundfont, processor.globalInstrument);
        name = xml->getStringAttribute("name");
        //if (name != )
        setDefaultPiano(xml->getStringAttribute("defaultPiano").getIntValue());
        iteratorIsEnabled = xml->getStringAttribute("iteratorIsEnabled").getIntValue();
        // iterate through its sub-elements
        for (auto e : xml->getChildIterator())
        {
            if (e->hasTagName( vtagKeymap))
            {
                addKeymapWithId(0);

                bkKeymaps.getLast()->setState(e);

                int oldId = bkKeymaps.getLast()->getId();
                int newId = transformId(PreparationTypeKeymap, oldId);

                bkKeymaps.getLast()->setId(newId);
            }
            else if (e->hasTagName ( vtagGeneral))
            {
                general->setState(e);
            }
            else if (e->hasTagName(vtagEqualizer)) {
                processor.getBKEqualizer()->setState(e);
            }
            else if (e->hasTagName(vtagCompressor))
            {
                processor.getCompressor()->setState(e);
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
            else if (e->hasTagName( vtagResonance))
            {
                addResonanceWithId(0);
                
                resonance.getLast()->setState(e);
                
                int oldId = resonance.getLast()->getId();
                int newId = transformId(PreparationTypeResonance, oldId);
                
                resonance.getLast()->setId(newId);
            }
            else if (e->hasTagName( vtagModResonance))
            {
                addResonanceModWithId(0);
                
                modResonance.getLast()->setState(e);
                
                int oldId = modResonance.getLast()->getId();
                int newId = transformId(PreparationTypeResonanceMod, oldId);
                
                modResonance.getLast()->setId(newId);
                
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
        for (auto e : xml->getChildIterator()) 
        {
            if (e->hasTagName(vtagPiano))
            {
                Piano::Ptr thisPiano = bkPianos[which++];
                
                thisPiano->setState(e, &idmap, idcounts);
            }
        }
        for (auto e : xml->getChildIterator())
        {
            if(e->hasTagName(vtagPianoIterator))
            {
                pianoIteratorOrder.setState(e, bkPianos);
            }
        }
        for( auto e : xml->getChildIterator())
        {
            if(e->hasTagName("upkeymap"))
            {
                iteratorUpKeymap->setState(e);
            }
            if(e->hasTagName("downkeymap"))
            {
                iteratorDownKeymap->setState(e);
            }
            
        }
    }
}
