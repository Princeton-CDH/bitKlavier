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
    if(type >= 0 && currentSampleType != type)
    {
        currentSampleType = type;
        
        didLoadMainPianoSamples = false;
        
        DBG("SAMPLE_SET: " + cBKSampleLoadTypes[type]);
        progress = 0.0;
        progressInc = 1.0f / (88.0 + 88.0 + ((type == BKLoadHeavy)  ? (8 * 4 * 8) :
                                             (type == BKLoadMedium) ? (8 * 4 * 4) :
                                             (type == BKLoadLite)   ? (8 * 4 * 2) :
                                             (type == BKLoadLitest) ? (8 * 4 * 1) :
                                             1.0));
        
        BKSampleLoader::loadMainPianoSamples(&mainPianoSynth, type, progress, progressInc);
        
        didLoadMainPianoSamples = true;
    
        if (!didLoadHammersAndRes && type != BKLoadLitest)
        {
            didLoadHammersAndRes = true;
            BKSampleLoader::loadHammerReleaseSamples(&hammerReleaseSynth, progress, progressInc);
            BKSampleLoader::loadResonanceReleaseSamples(&resonanceReleaseSynth, progress, progressInc);
        }   
    }
    
}

void BKAudioProcessor::collectGalleriesFromFolder(File folder)
{
    DirectoryIterator xmlIter (File (folder), true, "*.xml");
    while (xmlIter.next())
    {
        File galleryFile (xmlIter.getFile());
        
        galleryNames.add(galleryFile.getFullPathName());
    }
    
    
    DirectoryIterator jsonIter (File (folder), true, "*.json");
    while (jsonIter.next())
    {
        File galleryFile (jsonIter.getFile());
        
        galleryNames.add(galleryFile.getFullPathName());
    }
}

void BKAudioProcessor::collectGalleries(void)
{
    galleryNames.clear();
    
    File bkGalleries;
    
#if JUCE_IOS
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

