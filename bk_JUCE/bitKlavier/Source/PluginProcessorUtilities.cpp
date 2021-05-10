/*
  ==============================================================================

    PluginProcessorUtilities.cpp
    Created: 22 Mar 2017 12:28:41pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "PluginProcessor.h"

void BKAudioProcessor::updateUI(void)
{
    updateState->pianoDidChangeForGraph = true;
    updateState->directPreparationDidChange = true;
    updateState->nostalgicPreparationDidChange = true;
    updateState->synchronicPreparationDidChange = true;
    updateState->blendronicPreparationDidChange = true;
    updateState->tuningPreparationDidChange = true;
    updateState->tempoPreparationDidChange = true;
    updateState->generalSettingsDidChange = true;
    
    
    updateState->directDidChange = true;
    updateState->keymapDidChange = true;

    updateState->editsMade = false;
}

OwnedArray<HashMap<String, int>>& BKAudioProcessor::getSourcedNotesOn()
{
    return sourcedNotesOn;
}

int BKAudioProcessor::loadSamples(BKSampleLoadType type, String path, int subsound, bool updateGlobalSet)
{
    didLoadMainPianoSamples = false;
    
    // Check if path isn't valid and load BKLoadLite if it is not
    if (type == BKLoadSoundfont)
    {
        if (!path.startsWith("default.sf"))
        {
            File file(path);
            if (!file.exists())
            {
                type = BKLoadLite;
            }
        }
    }
    
    if (type < BKLoadSoundfont)
    {
        loadingSampleType = type;
        
//        progress = 0.0;
        
        loadingSoundSet = cBKSampleLoadTypes[type];
        
        if (!loadedSoundSets.contains(loadingSoundSet))
        {
            loadingSoundSetId = loadedSoundSets.size();
            loader.addJob(new BKSampleLoader(*this, loadingSampleType, loadingSoundfont, loadingInstrument, loadingSoundSetId, true), isMemoryMappingEnabled());
        }
        else
        {
            didLoadMainPianoSamples = true;
        }
    }
    else if (type == BKLoadSoundfont)
    {
        loadingSampleType = type;
        loadingSoundfont = path;
        loadingInstrument = subsound;
        
        loadingSoundSet = path + ".subsound" + String(subsound);
        
        if (!loadedSoundSets.contains(loadingSoundSet))
        {
            loadingSoundSetId = loadedSoundSets.size();
            loader.addJob(new BKSampleLoader(*this, loadingSampleType, loadingSoundfont, loadingInstrument, loadingSoundSetId, false), false);
        }
        else
        {
            didLoadMainPianoSamples = true;
        }
    }
    else if (type == BKLoadCustom)
    {
        loadingSampleType = type;
        loadingSoundfont = path;
        loadingSoundSet = path;
        
        if (!loadedSoundSets.contains(loadingSoundSet))
        {
            loadingSoundSetId = loadedSoundSets.size();
            loader.addJob(new BKSampleLoader(*this, loadingSampleType, loadingSoundfont, loadingInstrument, loadingSoundSetId, true), isMemoryMappingEnabled());
        }
        else
        {
            didLoadMainPianoSamples = true;
        }
    }
    
    loadedSoundSets.addIfNotAlreadyThere(loadingSoundSet);
    if (updateGlobalSet)
    {
        globalSoundSetId = loadedSoundSets.indexOf(loadingSoundSet);
        globalSampleType = loadingSampleType;
        globalSoundfont = loadingSoundfont;
        globalInstrument = loadingInstrument;
    }
    
    return loadedSoundSets.indexOf(loadingSoundSet);
}


void BKAudioProcessor::collectSoundfontsFromFolder(File folder)
{
    for (auto iter : RangedDirectoryIterator (File (folder), true, "*.sf2;*.sfz"))
    {
        File soundfontFile = iter.getFile();
        
        soundfontNames.add(soundfontFile.getFullPathName());
    }
}

void BKAudioProcessor::collectGalleriesFromFolder(File folder)
{
    for (auto xmlIter : RangedDirectoryIterator (File (folder), true, "*.xml"))
    {
        File galleryFile (xmlIter.getFile());

        galleryNames.add(galleryFile.getFullPathName());
    }
    
    
    for (auto jsonIter : RangedDirectoryIterator (File (folder), true, "*.json"))
    {
        File galleryFile (jsonIter.getFile());
        
        galleryNames.add(galleryFile.getFullPathName());
    }

	galleryNames.sortNatural();
}

void BKAudioProcessor::collectGalleries(void)
{
    galleryNames.clear();
    
    File bkGalleries;
    
#if JUCE_IOS
    bkGalleries = File::getSpecialLocation (File::userDocumentsDirectory);
#endif
#if JUCE_MAC
    bkGalleries = File::getSpecialLocation(File::globalApplicationsDirectory).getChildFile("bitKlavier").getChildFile("galleries");
#endif
#if JUCE_WINDOWS || JUCE_LINUX
    //bkGalleries = File::getSpecialLocation(File::currentExecutableFile).getParentDirectory().getChildFile("galleries");
    bkGalleries = File::getSpecialLocation(File::userDocumentsDirectory).getChildFile("bitKlavier").getChildFile("galleries");
#endif
    
    collectGalleriesFromFolder(bkGalleries);
}

void BKAudioProcessor::collectSoundfonts(void)
{
    soundfontNames.clear();
    
    File bkSoundfonts;
    
#if JUCE_IOS
    bkSoundfonts = File::getSpecialLocation(File::invokedExecutableFile).getParentDirectory().getChildFile("soundfonts");
    collectSoundfontsFromFolder(bkSoundfonts);
    bkSoundfonts = File::getSpecialLocation (File::userDocumentsDirectory);
#endif
#if JUCE_MAC
    bkSoundfonts = File::getSpecialLocation(File::globalApplicationsDirectory).getChildFile("bitKlavier").getChildFile("soundfonts");
#endif
#if JUCE_WINDOWS || JUCE_LINUX
    bkSoundfonts = File::getSpecialLocation(File::userDocumentsDirectory).getChildFile("bitKlavier").getChildFile("soundfonts");
#endif
    
    collectSoundfontsFromFolder(bkSoundfonts);
    soundfontNames.sort(true);
}

void BKAudioProcessor::collectCustomSamples()
{
    customSampleSetNames.clear();
    
    // Get every folder in the search path
    Array<File> directories = sampleSearchPath.findChildFiles(File::TypesOfFileToFind::findDirectories, true);
    for (int i = 0; i < sampleSearchPath.getNumPaths(); ++i)
    {
        directories.add(sampleSearchPath[i]);
    }
    
    for (auto dir : directories)
    {
        // Check if the folder contains any properly formatted wav files
        for (auto iter : RangedDirectoryIterator (File (dir), false, "*.wav"))
        {
            String fileName = iter.getFile().getFileNameWithoutExtension();
            
            std::regex reg("[ABCDEFG]#*b*\\dv\\d+");
            if (std::regex_search(fileName.toStdString(), reg))
            {
                customSampleSetNames.add(dir.getFullPathName());
                break;
            }
        }
    }

}

String BKAudioProcessor::firstGallery(void)
{
    collectGalleries();
    
    return galleryNames[0];
}



void BKAudioProcessor::updateGalleries()
{
    collectGalleries();
    
    updateState->setAllCurrentIdsTo(-1);
    
    gallery->getGeneralSettings();
    mainPianoSynth.updateGeneralSettings(gallery->getGeneralSettings());
    hammerReleaseSynth.updateGeneralSettings(gallery->getGeneralSettings());
    resonanceReleaseSynth.updateGeneralSettings(gallery->getGeneralSettings());
    pedalSynth.updateGeneralSettings(gallery->getGeneralSettings());
    
    //clipboard.clear();
    
    updateState->galleriesUpdated = true;
}

ValueTree BKAudioProcessor::getPreparationState(BKPreparationType type, int Id)
{
    if (type == PreparationTypeDirect)
    {
        return gallery->getDirect(Id)->getState();
    }
    else if (type == PreparationTypeDirectMod)
    {
        return gallery->getDirectModification(Id)->getState();
    }
    else if (type == PreparationTypeNostalgic)
    {
        return gallery->getNostalgic(Id)->getState(true);
    }
    else if (type == PreparationTypeNostalgicMod)
    {
        return gallery->getNostalgicModification(Id)->getState();
    }
    else if (type == PreparationTypeSynchronic)
    {
        return gallery->getSynchronic(Id)->getState(true);
    }
    else if (type == PreparationTypeSynchronicMod)
    {
        return gallery->getSynchronicModification(Id)->getState();
    }
    else if (type == PreparationTypeBlendronic)
    {
        return gallery->getBlendronic(Id)->getState(true);
    }
    else if (type == PreparationTypeBlendronicMod)
    {
        return gallery->getBlendronicModification(Id)->getState();
    }
    else if (type == PreparationTypeTuning)
    {
        return gallery->getTuning(Id)->getState(true);
    }
    else if (type == PreparationTypeTuningMod)
    {
        return gallery->getTuningModification(Id)->getState();
    }
    else if (type == PreparationTypeTempo)
    {
        return gallery->getTempo(Id)->getState(true);
    }
    else if (type == PreparationTypeTempoMod)
    {
        return gallery->getTempoModification(Id)->getState();
    }
    else if (type == PreparationTypeKeymap)
    {
        return gallery->getKeymap(Id)->getState();
    }
    
    return ValueTree();
}

void BKAudioProcessor::setPreparationState(BKPreparationType type, int Id, XmlElement* xml)
{
    if (type == PreparationTypeDirect)
    {
        Direct::Ptr prep = gallery->getDirect(Id);
        String name = prep->getName();
        prep->setState(xml);
        prep->setName(name);
        prep->setId(Id);
    }
    else if (type == PreparationTypeDirectMod)
    {
        DirectModification::Ptr prep = gallery->getDirectModification(Id);
        String name = prep->getName();
        prep->setState(xml);
        prep->setName(name);
        prep->setId(Id);
    }
    else if (type == PreparationTypeNostalgic)
    {
        Nostalgic::Ptr prep = gallery->getNostalgic(Id);
        String name = prep->getName();
        prep->setState(xml);
        prep->setName(name);
        prep->setId(Id);
    }
    else if (type == PreparationTypeNostalgicMod)
    {
        NostalgicModification::Ptr prep = gallery->getNostalgicModification(Id);
        String name = prep->getName();
        prep->setState(xml);
        prep->setName(name);
        prep->setId(Id);
    }
    else if (type == PreparationTypeSynchronic)
    {
        Synchronic::Ptr prep = gallery->getSynchronic(Id);
        String name = prep->getName();
        prep->setState(xml);
        prep->setName(name);
        prep->setId(Id);
    }
    else if (type == PreparationTypeSynchronicMod)
    {
        SynchronicModification::Ptr prep = gallery->getSynchronicModification(Id);
        String name = prep->getName();
        prep->setState(xml);
        prep->setName(name);
        prep->setId(Id);
    }
    else if (type == PreparationTypeBlendronic)
    {
        Blendronic::Ptr prep = gallery->getBlendronic(Id);
        String name = prep->getName();
        prep->setState(xml);
        prep->setName(name);
        prep->setId(Id);
    }
    else if (type == PreparationTypeBlendronicMod)
    {
        BlendronicModification::Ptr prep = gallery->getBlendronicModification(Id);
        String name = prep->getName();
        prep->setState(xml);
        prep->setName(name);
        prep->setId(Id);
    }
    else if (type == PreparationTypeTuning)
    {
        Tuning::Ptr prep = gallery->getTuning(Id);
        String name = prep->getName();
        prep->setState(xml);
        prep->setName(name);
        prep->setId(Id);
    }
    else if (type == PreparationTypeTuningMod)
    {
        TuningModification::Ptr prep = gallery->getTuningModification(Id);
        String name = prep->getName();
        prep->setState(xml);
        prep->setName(name);
        prep->setId(Id);
    }
    else if (type == PreparationTypeTempo)
    {
        Tempo::Ptr prep = gallery->getTempo(Id);
        String name = prep->getName();
        prep->setState(xml);
        prep->setName(name);
        prep->setId(Id);
    }
    else if (type == PreparationTypeTempoMod)
    {
        TempoModification::Ptr prep = gallery->getTempoModification(Id);
        String name = prep->getName();
        prep->setState(xml);
        prep->setName(name);
        prep->setId(Id);
    }
    else if (type == PreparationTypeKeymap)
    {
        Keymap::Ptr keymap = gallery->getKeymap(Id);
        String name = keymap->getName();
        keymap->setState(xml);
        keymap->setName(name);
        keymap->setId(Id);
    }
}

void BKAudioProcessor::collectPreparations(void)
{
    File file;
    
    exportedPreparations.clear();
    
    for (int i = 0; i < BKPreparationTypeNil; i++)
    {
        exportedPreparations.add(new StringArray());
    }
    
#if JUCE_IOS
    file  = file.getSpecialLocation(File::userDocumentsDirectory);
#endif
#if JUCE_MAC
    file = file.getSpecialLocation(File::globalApplicationsDirectory).getChildFile("bitKlavier");
#endif
#if JUCE_WINDOWS || JUCE_LINUX
    file = file.getSpecialLocation(File::userDocumentsDirectory).getChildFile("bitKlavier");
#endif
    
    file = file.getChildFile("preparations");

    for (int i = 0; i < BKPreparationTypeNil; i++ )
    {
        File preps = file.getChildFile(cPreparationTypes[i]);
        
        for (auto xmlIter : RangedDirectoryIterator (File(preps), true, "*.xml"))
        {
            exportedPreparations[i]->add(xmlIter.getFile().getFileName());
        }
    }
}

void BKAudioProcessor::importPreparation(BKPreparationType type, int Id, int importId)
{
    File file;
    
#if JUCE_IOS
    file  = file.getSpecialLocation(File::userDocumentsDirectory);
#endif
#if JUCE_MAC
    file = file.getSpecialLocation(File::globalApplicationsDirectory).getChildFile("bitKlavier");
#endif
#if JUCE_WINDOWS || JUCE_LINUX
    file = file.getSpecialLocation(File::userDocumentsDirectory).getChildFile("bitKlavier");
#endif
    
    file = file.getChildFile("preparations");
    file = file.getChildFile(cPreparationTypes[type]);
    file = file.getChildFile(exportedPreparations[type]->getReference(importId));
    
    std::unique_ptr<XmlElement> xml = XmlDocument::parse (file);
    
    setPreparationState(type, Id, xml.get());
}


void BKAudioProcessor::exportPreparation(BKPreparationType type, int Id, String name)
{
    File file;
    
#if JUCE_IOS
    file  = file.getSpecialLocation(File::userDocumentsDirectory);
#endif
#if JUCE_MAC
    file = file.getSpecialLocation(File::globalApplicationsDirectory).getChildFile("bitKlavier");
#endif
#if JUCE_WINDOWS || JUCE_LINUX
    file = file.getSpecialLocation(File::userDocumentsDirectory).getChildFile("bitKlavier");
#endif
    
    file = file.getChildFile("preparations");
    
    if (!file.isDirectory()) file.createDirectory();

    file = file.getChildFile(cPreparationTypes[type]);
    
    if (!file.isDirectory()) file.createDirectory();
    
    file = file.getChildFile(name+".xml");
    
    DBG("URL: " + file.getFullPathName());
    
    std::unique_ptr<XmlElement> xml = getPreparationState(type, Id).createXml();
    xml->writeTo(file, XmlElement::TextFormat());
}

void BKAudioProcessor::collectPianos(void)
{
    File file;
    
    exportedPianos.clear();
    
#if JUCE_IOS
    file  = file.getSpecialLocation(File::userDocumentsDirectory);
#endif
#if JUCE_MAC
    file = file.getSpecialLocation(File::globalApplicationsDirectory).getChildFile("bitKlavier");
#endif
#if JUCE_WINDOWS || JUCE_LINUX
    file = file.getSpecialLocation(File::userDocumentsDirectory).getChildFile("bitKlavier");
#endif
    
    file = file.getChildFile("pianos");
    
    for (auto iter : RangedDirectoryIterator (File(file), true, "*.xml"))
    {
        File piano (iter.getFile());
        
        exportedPianos.add(piano.getFileName());
    }
}

void BKAudioProcessor::importPiano(int Id, int importId)
{
    File file;
    
#if JUCE_IOS
    file  = file.getSpecialLocation(File::userDocumentsDirectory);
#endif
#if JUCE_MAC
    file = file.getSpecialLocation(File::globalApplicationsDirectory).getChildFile("bitKlavier");
#endif
#if JUCE_WINDOWS || JUCE_LINUX
    file = file.getSpecialLocation(File::userDocumentsDirectory).getChildFile("bitKlavier");
#endif
    
    file = file.getChildFile("pianos");
    file = file.getChildFile(exportedPianos[importId]);
    
    std::unique_ptr<XmlElement> xml = XmlDocument::parse (file);
    
    OwnedArray<HashMap<int,int>> importmap;
    
    for (int i = 0; i < BKPreparationTypeNil; i++) importmap.add(new HashMap<int, int>());
    
    for (auto sub : xml->getChildIterator()) 
    {
        String tag = sub->getTagName();
        
        int newId = -1;
        BKPreparationType type = BKPreparationTypeNil;
        
        if (tag == vtagDirect)
        {
            newId = gallery->addCopy(PreparationTypeDirect, sub);
            type = PreparationTypeDirect;
        }
        else if (tag == vtagNostalgic)
        {
            newId = gallery->addCopy(PreparationTypeNostalgic, sub);
            type = PreparationTypeNostalgic;
        }
        else if (tag == vtagSynchronic)
        {
            newId = gallery->addCopy(PreparationTypeSynchronic, sub);
            type = PreparationTypeSynchronic;
        }
        else if (tag == vtagTempo)
        {
            newId = gallery->addCopy(PreparationTypeTempo, sub);
            type = PreparationTypeTempo;
        }
        else if (tag == vtagTuning)
        {
            newId = gallery->addCopy(PreparationTypeTuning, sub);
            type = PreparationTypeTuning;
        }
        else if (tag == vtagModDirect)
        {
            newId = gallery->addCopy(PreparationTypeDirectMod, sub);
            type = PreparationTypeDirectMod;
        }
        else if (tag == vtagModNostalgic)
        {
            newId = gallery->addCopy(PreparationTypeNostalgicMod, sub);
            type = PreparationTypeNostalgicMod;
        }
        else if (tag == vtagModSynchronic)
        {
            newId = gallery->addCopy(PreparationTypeSynchronicMod, sub);
            type = PreparationTypeSynchronicMod;
        }
        else if (tag == vtagModTempo)
        {
            newId = gallery->addCopy(PreparationTypeTempoMod, sub);
            type = PreparationTypeTempoMod;
        }
        else if (tag == vtagModTuning)
        {
            newId = gallery->addCopy(PreparationTypeTuningMod, sub);
            type = PreparationTypeTuningMod;
        }
        else if (tag == vtagKeymap)
        {
            newId = gallery->addCopy(PreparationTypeKeymap, sub);
            type = PreparationTypeKeymap;
        }
        else if (tag == vtagBlendronic)
        {
            newId = gallery->addCopy(PreparationTypeBlendronic, sub);
            type = PreparationTypeBlendronic;
        }
        else if (tag == vtagModBlendronic)
        {
            newId = gallery->addCopy(PreparationTypeBlendronicMod, sub);
            type = PreparationTypeBlendronicMod;
        }
        else continue;
        
        int oldId = sub->getStringAttribute("Id").getIntValue();
        
        importmap[type]->set(oldId, newId);
    }
    
    XmlElement* pianoxml = xml->getChildByName("piano");
    gallery->addPiano(pianoxml, &importmap);
    
    setCurrentPiano(gallery->getPianos().getLast()->getId());
    
    saveGalleryToHistory("Import Piano");
}

void BKAudioProcessor::exportPiano(int Id, String name)
{
    File file;
    
#if JUCE_IOS
    file  = file.getSpecialLocation(File::userDocumentsDirectory);
#endif
#if JUCE_MAC
    file = file.getSpecialLocation(File::globalApplicationsDirectory).getChildFile("bitKlavier");
#endif
#if JUCE_WINDOWS || JUCE_LINUX
    file = file.getSpecialLocation(File::userDocumentsDirectory).getChildFile("bitKlavier");
#endif
    
    file = file.getChildFile("pianos");
    
    if (!file.isDirectory()) file.createDirectory();
    
    file = file.getChildFile(name+".xml");
    
    
    Piano::Ptr piano = gallery->getPiano(Id);
    
    ValueTree toExport("exportedPiano");
    
    for (auto item : piano->getItems())
    {
        BKPreparationType type = item->getType();
        int Id = item->getId();
        
        if (type <= PreparationTypeTempoMod)
        {
            toExport.addChild( getPreparationState(type, Id), -1, 0 );
        }
    }
   
    toExport.addChild(piano->getState(), -1, 0);
    
    toExport.createXml()->writeTo(file, XmlElement::TextFormat());
}

