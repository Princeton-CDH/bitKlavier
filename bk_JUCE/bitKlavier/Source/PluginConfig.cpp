/*
  ==============================================================================

    PluginConfig.cpp
    Created: 22 Mar 2017 12:40:02pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

void BKAudioProcessor::releaseResources() {
    
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    //fileBuffer.setSize (0, 0);
    
}


#ifndef JucePlugin_PreferredChannelConfigurations
bool BKAudioProcessor::setPreferredBusArrangement (bool isInput, int bus, const AudioChannelSet& preferredSet) {
    
    // Reject any bus arrangements that are not compatible with your plugin
    
    const int numChannels = preferredSet.size();
    
#if JucePlugin_IsMidiEffect
    if (numChannels != 0)
        return false;
#elif JucePlugin_IsSynth
    if (isInput || (numChannels != 1 && numChannels != 2))
        return false;
#else
    if (numChannels != 1 && numChannels != 2)
        return false;
    
    if (! AudioProcessor::setPreferredBusArrangement (! isInput, bus, preferredSet))
        return false;
#endif
    
    return AudioProcessor::setPreferredBusArrangement (isInput, bus, preferredSet);
}
#endif


//==============================================================================
void BKAudioProcessor::changeListenerCallback(ChangeBroadcaster *source)
{
    
}

//==============================================================================
bool BKAudioProcessor::hasEditor() const
{
    
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* BKAudioProcessor::createEditor()
{
    editor = new BKAudioProcessorEditor(*this);
    return editor;
}

AudioProcessorEditor* BKAudioProcessor::getEditor()
{
    return editor;
}

//==============================================================================
void BKAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    ValueTree settingsVT("userSettings");
    
    settingsVT.setProperty("tooltipsEnabled", (int)areTooltipsEnabled(), 0);
    settingsVT.setProperty("hotkeysEnabled", (int)areHotkeysEnabled(), 0);
    settingsVT.setProperty("memoryMappingEnabled", (int)isMemoryMappingEnabled(), 0);
    
    settingsVT.setProperty("defaultSamplesSearchPath", defaultSamplesPath.getFullPathName(), 0);
    settingsVT.setProperty("soundfontsSearchPath", soundfontsPaths.toString(), 0);
    settingsVT.setProperty("sampleSearchPath", customSamplesPaths.toString(), 0);
    settingsVT.setProperty("gallerySearchPath", galleryPaths.toString(), 0);
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    ValueTree galleryVT(vtagGalleryPath);
    
    galleryVT.setProperty("defaultLoaded", (int)defaultLoaded, 0);
    galleryVT.setProperty("defaultName", defaultName, 0);
    
    galleryVT.setProperty("sampleType", (int)globalSampleType, 0);
    
    galleryVT.setProperty("soundfontURL", globalSoundfont, 0);
    
    galleryVT.setProperty("soundfontInst", globalInstrument, 0);
    
    galleryVT.setProperty("galleryPath", gallery->getURL(), 0);
    
    galleryVT.setProperty("defaultPiano", currentPiano->getId(), 0);
    
    galleryVT.setProperty("invertSustain", getSustainInversion(), 0);
    
    DBG("sustain inversion saved: " + String((int)getSustainInversion()));
    
    galleryVT.addChild(settingsVT, 0, 0);
    
    std::unique_ptr<XmlElement> galleryXML = galleryVT.createXml();
    copyXmlToBinary (*galleryXML, destData);
}

void BKAudioProcessor::loadSamplesStartup(void)
{
    // LOAD SAMPLES
    // Load Litest, Lite, Medium, or Heavy
    if (globalSampleType < BKLoadSoundfont)
    {
        loadSamples(globalSampleType);
    }
    // Load Soundfont or Custom
    else if (globalSampleType <= BKLoadCustom)
    {
        File file (globalSoundfont);
        if (file.exists())
        {
            loadSamples(globalSampleType, globalSoundfont, globalInstrument);
        }
        else
        {
            globalSampleType = BKLoadLite;
            loadSamples(BKLoadLite);
        }
    }
    else
    {
#if JUCE_IOS
        loadSamples(BKLoadLite);
#else
        loadSamples(BKLoadHeavy);
#endif
    }
}

void BKAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    DBG("BKAudioProcessor::setStateInformation");
    
    std::unique_ptr<XmlElement> galleryXML = getXmlFromBinary (data, sizeInBytes);
    
    DBG("galleryXML: " + galleryXML->toString(XmlElement::TextFormat()));
    if (galleryXML != nullptr)
    {
        XmlElement* userSettings = galleryXML->getChildElement(0);
        if (userSettings != nullptr)
        {
            setTooltipsEnabled((bool)userSettings->getIntAttribute("tooltipsEnabled", 1));
            setHotkeysEnabled((bool)userSettings->getIntAttribute("hotkeysEnabled", 1));
            setMemoryMappingEnabled((bool)userSettings->getIntAttribute("memoryMappingEnabled", 0));
            
            // Make sure this path exists, otherwise leave it as its initial value set in the processor construtor
            String defaultSamplesPathName = userSettings->getStringAttribute("defaultSamplesSearchPath", "");
            if (defaultSamplesPathName.isNotEmpty())
            {
                File path (defaultSamplesPathName);
                if (path.exists()) defaultSamplesPath = path;
            }
            
            soundfontsPaths = userSettings->getStringAttribute("soundfontsSearchPath", soundfontsPaths.toString());
            
            customSamplesPaths = userSettings->getStringAttribute("sampleSearchPath", "");
            galleryPaths = userSettings->getStringAttribute("gallerySearchPath", galleryPaths.toString());
        }
        else
        {
            setTooltipsEnabled(true);
            setHotkeysEnabled(true);
            setMemoryMappingEnabled(false);
            customSamplesPaths = "";
        }
        
        collectCustomSamples();
        
        defaultLoaded = (bool) galleryXML->getStringAttribute("defaultLoaded").getIntValue();
        
        if (defaultLoaded)
        {
            defaultName = galleryXML->getStringAttribute("defaultName");
            
            int size;
            
            String xmlData = BinaryData::getNamedResource(defaultName.toUTF8(), size);
            
            loadGalleryFromXml(XmlDocument::parse(xmlData).get());
        }
        else
        {
            currentGalleryPath = galleryXML->getStringAttribute("galleryPath");
            currentGallery = galleryXML->getStringAttribute("name");
            
            DBG("loading gallery and piano from plugin state: setStateInformation() "
                + currentGalleryPath + " "
                + String(galleryXML->getStringAttribute("defaultPiano").getIntValue()));
            
            loadGalleryFromPath(currentGalleryPath);
        }
        
        bool invertSustain = (bool)galleryXML->getStringAttribute("invertSustain").getIntValue();
        
        setSustainInversion(invertSustain);

        
        //override gallery-saved defaultPiano with pluginHost-saved defaultPiano
        setCurrentPiano(galleryXML->getStringAttribute("defaultPiano").getIntValue());
        
        if (currentPiano == nullptr)
        {
            String xmlData = CharPointer_UTF8 (BinaryData::Basic_Piano_xml);
            
            defaultLoaded = true;
            defaultName = "Basic_Piano_xml";
            
            loadGalleryFromXml(XmlDocument::parse(xmlData).get());
        }
        else initializeGallery();
        
#if JUCE_IOS
        BKSampleLoadType sampleType = BKLoadLite;
#else
        BKSampleLoadType sampleType = BKLoadHeavy;
#endif
        
        String sampleString = galleryXML->getStringAttribute("sampleType");
        
        if (sampleString != "")
        {
            sampleType = (BKSampleLoadType) sampleString.getIntValue();
        }
        
        globalSampleType = sampleType;

        globalSoundfont = galleryXML->getStringAttribute("soundfontURL");
        
        // Check that the file exists in it's absolute path and if not reduce to just the file name
        if (File::isAbsolutePath(globalSoundfont))
        {
            File soundfont(globalSoundfont);
            if (!soundfont.exists()) globalSoundfont = soundfont.getFileName();
        }
        
        // If the file didn't exist in above block or if it wasn't saved as an absolute path
        // look for it in the soundfont folder and the sample search paths
        if (!File::isAbsolutePath(globalSoundfont))
        {
            Array<File> files;
            // Check the soundfonts folder
            for (auto path : getSoundfontsPaths())
            {
                files.addArray(path.findChildFiles(File::findFiles, true, globalSoundfont.fromLastOccurrenceOf(File::getSeparatorString(), false, false)));
            }
        
            // Set the global soundfont if we've found a matching file
            if (!files.isEmpty()) globalSoundfont = files.getUnchecked(0).getFullPathName();
            // Or if not in the soundfonts folder, check the custom samples search paths
            else
            {
                for (auto path : getCustomSamplesPaths())
                {
                    if (path.getFileName() == globalSoundfont)
                    {
                        globalSoundfont = path.getFullPathName();
                        files.clear();
                    }
                }
            }
        }

        globalInstrument = galleryXML->getStringAttribute("soundfontInst").getIntValue();
        
        loadSamplesStartup();
    }
    
}

//==============================================================================
const String BKAudioProcessor::getName() const {
    
    return JucePlugin_Name;
}

bool BKAudioProcessor::acceptsMidi() const {
    
    return true;
}

bool BKAudioProcessor::producesMidi() const {
    
    return false;
}

double BKAudioProcessor::getTailLengthSeconds() const {
    
    return 0.0;
}

int BKAudioProcessor::getNumPrograms() {
    
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int BKAudioProcessor::getCurrentProgram() {
    
    return 0;
}

void BKAudioProcessor::setCurrentProgram (int index)
{
    
}

const String BKAudioProcessor::getProgramName (int index) {
    
    return String("bitKlavier");
}

void BKAudioProcessor::changeProgramName (int index, const String& newName) {
    
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    AudioProcessor* processor = new BKAudioProcessor();

    
    return  processor;
}

File BKAudioProcessor::getDefaultSamplesPath()
{
#if JUCE_IOS
    return File::getSpecialLocation(File::invokedExecutableFile)
    .getParentDirectory().getChildFile("samples");
#else
    return defaultSamplesPath;
#endif
}

Array<File> BKAudioProcessor::getSoundfontsPaths()
{
    Array<File> directories = soundfontsPaths
    .findChildFiles(File::TypesOfFileToFind::findDirectories, true);
    for (int i = 0; i < soundfontsPaths.getNumPaths(); ++i)
    {
        directories.add(soundfontsPaths[i]);
    }
    
#if JUCE_IOS
    directories.add(File::getSpecialLocation(File::invokedExecutableFile)
                    .getParentDirectory().getChildFile("soundfonts"));
    directories.add(File::getSpecialLocation(File::userDocumentsDirectory));
#endif
#if JUCE_MAC
    directories.add(File::getSpecialLocation(File::globalApplicationsDirectory)
                    .getChildFile("bitKlavier").getChildFile("soundfonts"));
#endif
#if JUCE_WINDOWS || JUCE_LINUX
    directories.add(File::getSpecialLocation(File::userDocumentsDirectory)
                    .getChildFile("bitKlavier").getChildFile("soundfonts"));
#endif
    
    return directories;
}

Array<File> BKAudioProcessor::getGalleryPaths()
{
    Array<File> directories = galleryPaths
    .findChildFiles(File::TypesOfFileToFind::findDirectories, true);
    for (int i = 0; i < galleryPaths.getNumPaths(); ++i)
    {
        directories.add(galleryPaths[i]);
    }
#if JUCE_IOS
    directories.add(File::getSpecialLocation(File::userDocumentsDirectory));
#endif
#if JUCE_MAC
    directories.add(File::getSpecialLocation(File::globalApplicationsDirectory)
                    .getChildFile("bitKlavier").getChildFile("galleries"));
#endif
#if JUCE_WINDOWS || JUCE_LINUX
    directories.add(File::getSpecialLocation(File::userDocumentsDirectory)
                    .getChildFile("bitKlavier").getChildFile("galleries"));
#endif
    
    return directories;
}

Array<File> BKAudioProcessor::getCustomSamplesPaths()
{
    Array<File> directories = customSamplesPaths
    .findChildFiles(File::TypesOfFileToFind::findDirectories, true);
    for (int i = 0; i < customSamplesPaths.getNumPaths(); ++i)
    {
        directories.add(customSamplesPaths[i]);
    }

#if JUCE_IOS
    Array<File> subDirs = File::getSpecialLocation(File::userDocumentsDirectory)
    .findChildFiles(File::TypesOfFileToFind::findDirectories, true);
    directories.addArray(subDirs);
#endif
    
    return directories;
}
