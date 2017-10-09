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
    updateState->tuningPreparationDidChange = true;
    updateState->tempoPreparationDidChange = true;
    updateState->generalSettingsDidChange = true;
    
    
    updateState->directDidChange = true;
    updateState->keymapDidChange = true;
}

void BKAudioProcessor::loadPianoSamples(BKSampleLoadType type)
{
    // TO IMPLEMENT: Should turn off all notes in the processors/synths before loading new samples.
    if(type >= 0)
    {
        didLoadMainPianoSamples = false;
        
        BKSampleLoader::loadMainPianoSamples(&mainPianoSynth, type);
        
        didLoadMainPianoSamples = true;
        
        if (!didLoadHammersAndRes)
        {
            didLoadHammersAndRes = true;
            BKSampleLoader::loadHammerReleaseSamples(&hammerReleaseSynth);
            BKSampleLoader::loadResonanceReleaseSamples(&resonanceReleaseSynth);
        }   
    }
}

void BKAudioProcessor::collectGalleriesFromFolder(File folder)
{
    //DirectoryIterator xmlIter (File ("~/bkGalleries"), true, "*.xml");
    
    DBG("folder: " + folder.getFileName());
    
    DirectoryIterator xmlIter (File (folder), true, "*.xml");
    while (xmlIter.next())
    {
        File galleryFile (xmlIter.getFile());
        
        DBG("- - - " + galleryFile.getFileName());
        
        galleryNames.add(galleryFile.getFullPathName());
    }
    
    
    DirectoryIterator jsonIter (File (folder), true, "*.json");
    while (jsonIter.next())
    {
        File galleryFile (jsonIter.getFile());
        
        DBG("- - - " + galleryFile.getFileName());
        
        galleryNames.add(galleryFile.getFullPathName());
    }
}

void BKAudioProcessor::collectGalleries(void)
{
    galleryNames.clear();
    
    File bkGalleries;
    
#if JUCE_IOS
    bkGalleries = bkGalleries.getSpecialLocation(File::invokedExecutableFile).getParentDirectory().getChildFile("bitKlavier resources").getChildFile("galleries");
    
    collectGalleriesFromFolder(bkGalleries);
    
    File moreGalleries = File::getSpecialLocation (File::userDocumentsDirectory);
    
    
    collectGalleriesFromFolder(moreGalleries);
    
#endif
    
#if JUCE_MAC || JUCE_WINDOWS
    bkGalleries = bkGalleries.getSpecialLocation(File::userDocumentsDirectory).getChildFile("bitKlavier resources").getChildFile("galleries");
    
    collectGalleriesFromFolder(bkGalleries);
#endif
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
    
    clipboard.clear();
    
    updateState->galleryDidChange = true;
}

