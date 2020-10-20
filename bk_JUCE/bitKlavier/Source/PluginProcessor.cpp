
#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "BKPianoSampler.h"

#include "BKStandaloneWindow.h"

#if JUCE_IOS
int fontHeight;

int gComponentComboBoxHeight;
int gComponentLabelHeight;
int gComponentTextFieldHeight;

int gComponentRangeSliderHeight;
int gComponentSingleSliderHeight;
int gComponentStackedSliderHeight;

int gComponentToggleBoxHeight;
#endif

class BKUnitTestRunner : public UnitTestRunner
{
    void logMessage(const String& message) override
    {
        Logger::writeToLog(message);
    }
};

#if BK_UNIT_TESTS

class GalleryTests : public UnitTest
{
public:
    GalleryTests(BKAudioProcessor& p) : UnitTest("Galleries", "Gallery"), processor(p) {}
    
    BKAudioProcessor& processor;
    
    void runTest() override
    {
        beginTest("GalleryXML");
        
        for (int i = 0; i < 5; i++)
        {
			Random::getSystemRandom().setSeedRandomly();
            // create gallery and randomize it
            // call getState() to convert to ValueTree
            // call setState() to convert from ValueTree to preparation
            // compare begin and end states
            String name = "random gallery " + String(i);
            DBG("test consistency: " + name);
            
            // GALLERY 1
            processor.gallery = new Gallery(processor);
            processor.gallery->randomize();
            processor.gallery->setName(name);
            
            ValueTree vt1 = processor.gallery->getState();
            
            std::unique_ptr<XmlElement> xml = vt1.createXml();
            
            // GALLERY 2
            processor.gallery = new Gallery(*xml, processor);
            processor.gallery->setName(name);
            
            ValueTree vt2 =  processor.gallery->getState();

#if JUCE_WINDOWS
			File file1("C:\\Users\\User\\Desktop\\Programming\\output1.txt");
			File file2("C:\\Users\\User\\Desktop\\Programming\\output2.txt");
			FileLogger pressFtoPayRespects(file1, "blah", 128*1024*8);
			FileLogger pressFtoPayRespects2ElectricBoogaloo(file2, "idk whatever", 128 * 1024 * 8);
			pressFtoPayRespects.logMessage(vt1.toXmlString() +
				"\n=======================\n");
			pressFtoPayRespects2ElectricBoogaloo.logMessage(vt2.toXmlString() +
				"\n=======================\n");

#else
            File file1("~/Desktop/bk_unittest/gal1");
            File file2("~/Desktop/bk_unittest/gal2");
            FileLogger pressFtoPayRespects(file1, "gallery1", 128 * 1024 *16);
            FileLogger pressFtoPayRespects2ElectricBoogaloo(file2, "gallery2", 128 * 1024 * 16);
            pressFtoPayRespects.logMessage(vt1.toXmlString() +
                                           "\n=======================\n");
            pressFtoPayRespects2ElectricBoogaloo.logMessage(vt2.toXmlString() +
                                                            "\n=======================\n");
            
            expect(vt1.isEquivalentTo(vt2), "GALLERIES DO NOT MATCH");

#endif

            //expect(tp2->compare(tp1), tp1->getName() + " and " + tp2->getName() + " did not match.");
        }
    }
};

#endif

//==============================================================================
BKAudioProcessor::BKAudioProcessor(void):
updateState(new BKUpdateState()),
loader(),
mainPianoSynth(*this),
hammerReleaseSynth(*this),
resonanceReleaseSynth(*this),
pedalSynth(*this),
firstTime(true),
progress(0),
progressInc(0),
currentSampleRate(44100.),
doneWithSetStateInfo(false),
midiReady(false),
tooltipsEnabled(true),
hotkeysEnabled(true)
{
#if BK_UNIT_TESTS
    
    static GalleryTests galleryTest(*this);
    
    UnitTest::getAllTests().add(&galleryTest);
    
    BKUnitTestRunner tests;
    
    tests.setAssertOnFailure(false);
    
    tests.runAllTests();
    
#endif
    didLoadMainPianoSamples = false;
    didLoadHammersAndRes = false;
    sustainIsDown                   = false;
    noteOnCount                     = 0;
    
    Process::setPriority(juce::Process::RealtimePriority);
    
    Rectangle<int> r = Desktop::getInstance().getDisplays().getMainDisplay().userArea;
    screenWidth = r.getWidth();
    screenHeight = r.getHeight();
        
    float w_factor = ((float) screenWidth / (float) DEFAULT_WIDTH);
    float h_factor = ((float) screenHeight / (float) DEFAULT_HEIGHT);
    
    
    
#if JUCE_IOS
    int heightUnit = ((screenHeight * 0.1f) > 48) ? 48 : (screenHeight * 0.1f);
    
    fontHeight = screenHeight * 0.025f;
    fontHeight = (fontHeight < 13) ? 13 : fontHeight;
    
    //gComponentComboBoxHeight = heightUnit;
    gComponentComboBoxHeight = heightUnit * 0.85;
    gComponentToggleBoxHeight = heightUnit * 0.65;;
    
    DBG("JUCE_IOS: gComponentToggleBoxHeight = " + String(gComponentToggleBoxHeight));
    
    gComponentLabelHeight = heightUnit * 0.75f;
    //gComponentTextFieldHeight = heightUnit * 0.75f;
    gComponentTextFieldHeight = heightUnit * 0.6f;
    
    gComponentRangeSliderHeight = heightUnit * 1.25f;
    gComponentSingleSliderHeight = heightUnit * 1.25f;
    gComponentSingleSliderHeight = heightUnit* 1.1;
    gComponentStackedSliderHeight = heightUnit * 1.25f;

#endif
    
    uiScaleFactor = (w_factor + h_factor) * 0.5f;
    
    uiScaleFactor *= 1.15f; // making up for smallness on little ios devices
    
    uiScaleFactor = (uiScaleFactor > 1.0f) ? 1.0f : uiScaleFactor;
    
    loadGalleries();
    
    startTimer(1);
}

void BKAudioProcessor::loadGalleries()
{
    collectGalleries();
    collectPianos();
    collectPreparations();
    collectSoundfonts();
    
    updateUI();
    
    String xmlData = CharPointer_UTF8 (BinaryData::Basic_Piano_xml);
    
    defaultLoaded = true;
    defaultName = "Basic_Piano_xml";
    
    loadGalleryFromXml(XmlDocument::parse(xmlData).get());
    

#if JUCE_IOS
        platform = BKIOS;
        lastGalleryPath = lastGalleryPath.getSpecialLocation(File::userDocumentsDirectory);
#endif
#if JUCE_MAC
        platform = BKOSX;
        lastGalleryPath = lastGalleryPath.getSpecialLocation(File::globalApplicationsDirectory).getChildFile("bitKlavier").getChildFile("galleries");
#endif
#if JUCE_WINDOWS
        platform = BKWindows;
        lastGalleryPath = lastGalleryPath.getSpecialLocation(File::userDocumentsDirectory).getChildFile("bitKlavier").getChildFile("galleries");
#endif
#if JUCE_LINUX
        platform = BKLinux;
        lastGalleryPath = lastGalleryPath.getSpecialLocation(File::globalApplicationsDirectory).getChildFile("bitKlavier").getChildFile("galleries");
#endif

    
    noteOn.ensureStorageAllocated(128);
    noteVelocity.ensureStorageAllocated(128);
    for(int i = 0; i < 128; i++)
    {
        noteOn.set(i, new HashMap<String, int> ());
        noteVelocity.set(i, new HashMap<String, float> ());
    }
    
    bk_examples = StringArray({
        "1. Synchronic 1",
        "2. Synchronic 2",
        "3. Synchronic 3",
        "4. Synchronic 4",
        "5. Synchronic 5",
        "6. Synchronic 6",
        "7. Synchronic 7",
        "8. Synchronic 8",
        "9. Synchronic 9",
        "10. Nostalgic 1",
        "11. Nostalgic 2",
        "12. Nostalgic 3",
        "13. Nostalgic 4",
        "14. Nostalgic 5",
        "15. Nostalgic 6",
        "16. Tuning 1",
        "17. Tuning 2",
        "18. Tuning 3",
        "19. Tuning 4",
        "20. Tuning 5",
        "21. Direct 1",
        "22. Direct 2",
        "23. Adaptive Tempo 1",
        "24. Adaptive Tempo 2",
        "25. Adaptive Tempo 3",
        "26. Adaptive Tempo 4",
        "27. PianoMapGallery",
        "28. Spring Tuning 1",
        "29. Spring Tuning 2",
        "30. Spring Tuning 3",
        "31. Spring Tuning 4",
        "32. Spring Tuning 5",
        "33. Blendronic 1",
        "34. Blendronic 2",
        "35. Blendronic 3",
        "36. Blendronic 4",
        "37. Blendronic 5",
        "38. Blendronic 6",
        "39. Blendronic 7",
        
    });
    
    mikroetudes = StringArray({
        "And So",
        "Around 60",
        "Circleville",
        "Crests",
        "Cygnet",
        "Daily Decrease",
        "Didymus",
        "for Bill D",
        "Gigue Interrupted",
        "Houseboat",
        "Hurra",
        "Juxtaposed Weather",
        "Keep It Steady (OrNot)",
        "Keep It Steady",
        "Listen",
        "Mama's Musette",
        "Petite Gymnopedie",
        "Pyramids",
        "Quickie",
        "Scales within Sliding Scales",
        "Slow To Come Back",
        "Southwing",
        "Who do you think you are Mars",
        "Worm"
    });
    
    ns_etudes = StringArray({
        "NS_1_Prelude",
        "NS_2_Undertow",
        "NS_3_Song",
        "NS_4_Marbles",
        "NS_5_Wallumrod",
        "NS_6_PointsAmongLines",
        "NS_7_Systerslaat",
        "NS_8_ItIsEnough"
    });
    
    machines_for_listening = StringArray({
        "Machines for Listening (49key)",
        "Machines for Listening"
    });
}

void BKAudioProcessor::openSoundfont(void)
{
#if JUCE_IOS
#else
    FileChooser myChooser ("Load soundfont file...",
                           //File::getSpecialLocation (File::userHomeDirectory),
                           lastGalleryPath,
                           "*.sf2;*.sfz;");
    
    if (myChooser.browseForFileToOpen())
    {
        File sfzFile (myChooser.getResult());
        
        loadingSoundfont = sfzFile.getFullPathName();
        
        loadSamples(BKLoadSoundfont, sfzFile.getFullPathName());
    }
#endif
    
}

void BKAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
#if JUCE_IOS
    //stk::Stk::setSampleRate(sampleRate);
#endif
    //stk::Stk::setSampleRate(sampleRate); //crashes Logic Audio Unit Validation Tool
    
    mainPianoSynth.playbackSampleRateChanged();
    hammerReleaseSynth.playbackSampleRateChanged();
    resonanceReleaseSynth.playbackSampleRateChanged();
    pedalSynth.playbackSampleRateChanged();
    
    //mainPianoSynth.setGeneralSettings(gallery->getGeneralSettings());
    resonanceReleaseSynth.setGeneralSettings(gallery->getGeneralSettings());
    hammerReleaseSynth.setGeneralSettings(gallery->getGeneralSettings());
    pedalSynth.setGeneralSettings(gallery->getGeneralSettings());

    mainPianoSynth.clearVoices();
    resonanceReleaseSynth.clearVoices();
    hammerReleaseSynth.clearVoices();
    pedalSynth.clearVoices();

    // 88 or more seems to work well
    for (int i = 0; i < 300; i++)
    {
        mainPianoSynth.addVoice(new BKPianoSamplerVoice(gallery->getGeneralSettings()));
    }
    for (int i = 0; i < 128; i++)
    {
        resonanceReleaseSynth.addVoice(new BKPianoSamplerVoice(gallery->getGeneralSettings()));
        hammerReleaseSynth.addVoice(new BKPianoSamplerVoice(gallery->getGeneralSettings()));
        pedalSynth.addVoice(new BKPianoSamplerVoice(gallery->getGeneralSettings()));
    }
    
    levelBuf.setSize(2, 25);
    
    gallery->prepareToPlay(currentSampleRate);
    
    sustainIsDown = false;
    
    if (!didLoadMainPianoSamples)
    {
        if (loader.getNumJobs() == 0)
        {
    #if JUCE_IOS
            loadSamples(BKLoadLite);
    #else
            if (wrapperType == wrapperType_AudioUnit || wrapperType == wrapperType_VST || wrapperType == wrapperType_VST3)
            {
                loadSamples(BKLoadLite);
            }
            else
            {
                loadSamples(BKLoadHeavy);
            }
    #endif
        }
    }
}

BKAudioProcessor::~BKAudioProcessor()
{
    for (auto item : clipboard)
        item->clearConnections();
    clipboard.clear();
}

void BKAudioProcessor::deleteGallery(void)
{
    File file(gallery->getURL());
    file.deleteFile();
    
    String first = firstGallery();

    loadGalleryFromPath(first);
}

// Duplicates current gallery and gives it name
void BKAudioProcessor::writeCurrentGalleryToURL(String newURL)
{
    File myFile(newURL);
    
    ValueTree galleryVT = gallery->getState();
    
    galleryVT.setProperty("name", myFile.getFileName().upToFirstOccurrenceOf(".xml", false, false), 0);
    
    std::unique_ptr<XmlElement> myXML = galleryVT.createXml();
    
    myXML->writeTo(myFile, XmlElement::TextFormat());
    
    loadGalleryFromXml(myXML.get(), false);
    
    gallery->setURL(newURL);
    
    lastGalleryPath = myFile;
    
    defaultLoaded = false;
}

void BKAudioProcessor::clearBitKlavier(void)
{
    for (int i = 0; i < 15; i++)
    {
        hammerReleaseSynth.allNotesOff(i, true);
        resonanceReleaseSynth.allNotesOff(i, true);
        mainPianoSynth.allNotesOff(i, true);
        pedalSynth.allNotesOff(i, true);
    }

    /*
    THIS DID NOT SOLVE ALL OFF ISSUES - PROBLEM IS PROBABLY SOMEWHERE IN NOSTALGIC?
    for (auto piano : gallery->getPianos())
    {
        for (auto processor : piano->getNostalgicProcessors())
        {
            for (int i = 0; i < 15; i++)
            {
                processor->clearAll(i);
            }
        }
    }
    */
    
    //handleAllNotesOff();

    for (auto map : noteOn)
    {
        for (HashMap<String, int>::Iterator i (*map); i.next();)
        {
            String key = i.getKey();
            currentPiano->prepMap->keyReleased(i.getValue(), 0, 0,
                                               key.getTrailingIntValue(),
                                               false,
                                               (loadingSampleType == BKLoadSoundfont),
                                               key.upToLastOccurrenceOf("n", false, false));
        }
        map->clear();
    }
    
    for (auto map : noteVelocity)
        map->clear();
    
    sustainDeactivate();
    
    for (auto piano : gallery->getPianos())
    {
        piano->reset();
    }
}

void BKAudioProcessor::deleteGalleryAtURL(String path)
{
    File galleryPath(path);
    
    galleryPath.deleteFile();
}


void BKAudioProcessor::createNewGallery(String name, std::shared_ptr<XmlElement> xml)
{
    updateState->loadedJson = false;
    
    File bkGalleries;

#if JUCE_IOS
    bkGalleries = bkGalleries.getSpecialLocation(File::userDocumentsDirectory);
#endif
#if JUCE_MAC
    bkGalleries = bkGalleries.getSpecialLocation(File::globalApplicationsDirectory).getChildFile("bitKlavier").getChildFile("galleries");
#endif
#if JUCE_WINDOWS || JUCE_LINUX
    bkGalleries = bkGalleries.getSpecialLocation(File::userDocumentsDirectory).getChildFile("bitKlavier").getChildFile("galleries");
#endif
    
    
    File myFile(bkGalleries);
    String galleryName = name.upToFirstOccurrenceOf(".xml",false,false);
    DBG("new file name: " + galleryName);
    myFile = myFile.getNonexistentChildFile(name.upToFirstOccurrenceOf(".xml",false,false), ".xml", true);
    if (xml == nullptr)
    {
        myFile.appendData(BinaryData::Basic_Piano_xml, BinaryData::Basic_Piano_xmlSize);
        xml = XmlDocument::parse(myFile);
        xml->setAttribute("name", galleryName);
        xml->writeTo(myFile, XmlElement::TextFormat());
    }
    else
    {
        xml->setAttribute("name", galleryName);
        xml->writeTo(myFile, XmlElement::TextFormat());
    }
    
    xml->writeTo(myFile, XmlElement::TextFormat());
    
    galleryNames.add(myFile.getFullPathName());
	galleryNames.sortNatural();
    
    if (xml != nullptr)
    {
        currentGallery = myFile.getFileName();
        
        DBG("new gallery: " + currentGallery);

        gallery = new Gallery(xml.get(), *this);
        
        gallery->setURL(myFile.getFullPathName());
        gallery->setName(currentGallery);
        
        gallery->print();
        
        initializeGallery();
        
        gallery->setGalleryDirty(false);
        
        defaultLoaded = false;
    }
    
    
}

void BKAudioProcessor::duplicateGallery(String newName)
{
    newName = newName.upToFirstOccurrenceOf(".xml",false,false);
    
    File oldFile (gallery->getURL());
    String baseURL = oldFile.getParentDirectory().getFullPathName();
    
    String newURL = baseURL + "/" + newName + ".xml";
    
    DBG("to create: " + newURL);
    
    writeCurrentGalleryToURL(newURL);
}

void BKAudioProcessor::renameGallery(String newName)
{
    String oldName = gallery->getName().upToFirstOccurrenceOf(".xml",false,false);
    newName = newName.upToFirstOccurrenceOf(".xml",false,false);

    File oldFile (gallery->getURL());
    String baseURL = oldFile.getParentDirectory().getFullPathName();
    
    String newURL = baseURL + "/" + newName + ".xml";
    String oldURL = baseURL + "/" + oldName + ".xml";
    
    DBG("to create: " + newURL);
    DBG("to delete: " + oldURL);
    
    writeCurrentGalleryToURL(newURL);
    deleteGalleryAtURL(oldURL);
}

void BKAudioProcessor::handleNoteOn(int noteNumber, float velocity, int channel, int mappedFrom, String source, bool harmonizer)
{
    PreparationMap::Ptr pmap = currentPiano->getPreparationMap();
    
    bool activeSource = false;

    if (pmap != nullptr)
    {
        for (auto km : pmap->getKeymaps())
        {
            if (km->getAllMidiInputIdentifiers().contains(source))
            {
                activeSource = true;
                if (!harmonizer)
				{
                    if (km->getMidiEdit())
                    {
                        km->toggleNote(noteNumber);
                        //return;
                    }
                    else if (km->getHarMidiEdit())
                    {
                        km->setHarKey(noteNumber);
                        //return;
                    }
                    else if (km->getHarArrayMidiEdit())
                    {
                        km->toggleHarmonizerList(noteNumber);
                        //return;
                    }
                    else if (km->containsNote(noteNumber))
                    {
                        Array<int> harmonizer = km->getHarmonizationForKey(noteNumber, true, true);
                        for (int i = 0; i < harmonizer.size(); i++)
                        {
                            handleNoteOn(harmonizer[i], velocity, channel, noteNumber, source, true);
                        }
                        if (!km->isInverted())
                        {
                            if (km->getAllNotesOff())
                                clearBitKlavier();
                            
                            if (km->getSustainPedalKeys() && !km->getTriggeredKeys().contains(true))
                                sustainActivate();
                            
                            km->setTriggered(noteNumber, true);
                        }
                        else
                        {
                            km->setTriggered(noteNumber, false);
                            if (km->getSustainPedalKeys() && !km->getTriggeredKeys().contains(true))
                                sustainDeactivate();
                        }
                    }
                }
            }   
        }
    }
    if (!harmonizer) return;
    
    String key = source + "n" + String(mappedFrom);
    bool noteDown = noteOn.getUnchecked(noteNumber)->size() > 0;
    
    if (activeSource || getDefaultMidiInputIdentifiers().contains(source))
    {
        ++noteOnCount;
        noteOn.getUnchecked(noteNumber)->set(key, noteNumber);
        noteVelocity.getUnchecked(noteNumber)->set(key, velocity);
    }

    if (!activeSource) return;
    
    // Check PianoMap for whether piano should change due to key strike.
    for (auto pmap : currentPiano->modificationMap.getUnchecked(noteNumber)->pianoMaps)
    {
        for (auto keymap : pmap.keymaps)
        {
            if (keymap->keys().contains(noteNumber) && keymap->getAllMidiInputIdentifiers().contains(source))
            {
                int whichPiano = pmap.pianoTarget;
                if (whichPiano > 0 && whichPiano != currentPiano->getId())
                {
                    DBG("change piano to " + String(whichPiano));
                    setCurrentPiano(whichPiano);
                }
                break;
            }
        }
    }

    // modifications
    performResets(noteNumber, source);
    performModifications(noteNumber, source);

    // clears key from array of depressed notes in prevPiano so they don't get cutoff by sustain pedal release
    for (auto piano : prevPianos)
    {
        if (piano != currentPiano)
            piano->prepMap->clearKey(noteNumber);
    }

    // Send key on to each pmap in current piano
    //DBG("noteon: " +String(noteNumber) + " pmap: " + String(p));

    // TODO : for multi sample set support, remove soundfont argument from this chain of functions
    // UPDATE: actually seems like that argument isn't really used so it doesn't matter. still should clean this up
    currentPiano->prepMap->keyPressed(noteNumber, velocity, channel, mappedFrom, noteDown,
                                      (loadingSampleType == BKLoadSoundfont), source);

    //add note to springTuning, if only for Graph display
    //this could be a bad idea, in that a user may want to have only some keys (via a keymap) go to Spring tuning
    /*
    for ( auto t : currentPiano->getTuningProcessors())
    {
        t->getTuning()->getCurrentSpringTuning()->addNote(noteNumber);
        //t->getTuning()->getSpringTuning()->addNote(noteNumber);
    }
    */
    //keeping out for now; I just think it's better to be consistent, and even though it can be easy to forget
    //that you need to connect a Keymap to Tuning to get adaptive/spring tunings, it also allows you
    //control over which keys are going to spring tuning, which i can imagine being useful sometimes.
}

void BKAudioProcessor::handleAllNotesOff()
{
    
}

void BKAudioProcessor::handleNoteOff(int noteNumber, float velocity, int channel, int mappedFrom, String source, bool harmonizer)
{
    PreparationMap::Ptr pmap = currentPiano->getPreparationMap();
     
    bool activeSource = false;
    
    if (pmap != nullptr)
    {
        for (auto km : pmap->getKeymaps())
        {
            if (km->getAllMidiInputIdentifiers().contains(source))
            {
                activeSource = true;
                if (!harmonizer && km->containsNote(noteNumber))
                {
                    Array<int> harmonizer = km->getHarmonizationForKey(noteNumber, true, true);
                    for (int i = 0; i < harmonizer.size(); i++)
                    {
                        handleNoteOff(harmonizer[i], velocity, channel, noteNumber, source, true);
                    }
                    if (km->isInverted())
                    {
                        if (km->getAllNotesOff())
                            clearBitKlavier();
                        
                        if (km->getSustainPedalKeys() && !km->getTriggeredKeys().contains(true))
                            sustainActivate();
                        
                        km->setTriggered(noteNumber, true);
                    }
                    else
                    {
                        km->setTriggered(noteNumber, false);
                        if (km->getSustainPedalKeys() && !km->getTriggeredKeys().contains(true))
                            sustainDeactivate();
                    }
                }
            }
        }
    }

    if (harmonizer == false) return;
    
    String key = source + "n" + String(mappedFrom);
    
    if (activeSource || getDefaultMidiInputIdentifiers().contains(source))
    {
        noteOn.getUnchecked(noteNumber)->remove(key);
        noteVelocity.getUnchecked(noteNumber)->remove(key);
        --noteOnCount;
        if(noteOnCount < 0) noteOnCount = 0;
    }
    
    if (activeSource)
    {
        //DBG("noteoff velocity = " + String(velocity));
        
        noteOnSetsNoteOffVelocity = gallery->getGeneralSettings()->getNoteOnSetsNoteOffVelocity();
        if(noteOnSetsNoteOffVelocity) velocity = (*noteVelocity.getUnchecked(noteNumber))[key];
        else if(velocity <= 0) velocity = 0.7; //for keyboards that don't do proper noteOff messages
        
        bool noteDown = noteOn.getUnchecked(noteNumber)->size() > 0;
        currentPiano->prepMap->keyReleased(noteNumber, velocity, channel, mappedFrom, noteDown,
                                           (loadingSampleType == BKLoadSoundfont), source);
    }
    
    // This is to make sure note offs are sent to Direct and Nostalgic processors from previous pianos with holdover notes.
    for (auto piano : prevPianos)
    {
        pmap  = piano->getPreparationMap();
        
        activeSource = false;
        for (auto km : pmap->getKeymaps())
        {
            if (km->getAllMidiInputIdentifiers().contains(source))
            {
                activeSource = true;
            }
        }
        
        if (activeSource)
            if (piano != currentPiano)
                piano->prepMap->postRelease(noteNumber, velocity, mappedFrom, channel, source);
    }
}

void BKAudioProcessor::sustainActivate(void)
{
    DBG("BKAudioProcessor::sustainActivate");
    if(!sustainIsDown)
    {
        sustainIsDown = true;
        DBG("SUSTAIN ON");
        
        currentPiano->prepMap->sustainPedalPressed();
    
        prevPiano->prepMap->sustainPedalPressed();
    
        //play pedalDown resonance
        pedalSynth.keyOn(channel,
                           //synthNoteNumber,
                           21,
                           21,
                           0,
                           0.02, //gain
                           1.,
                           Forward,
                           Normal, //FixedLength,
                           PedalNote,
                           0, 
                           0,
                           0,
                           20000,
                           3,
                           3 );
    }
    
}

/*
 BUG:

 play a note, hold it down
 change pianos while note is still held
 press sustain pedal
 release the note (pedal still down)
 press the note again
 while holding the note down still, release the pedal (note will be cut off, even though it is still being held down)
 
 FIXED: through addition of PreparationMap::clearKey(int noteNumber)
 
 BUT, there still seems to be a problem when sometimes a note will be shut off by pedal release even when it is depressed
 this one doesn't seem to require a piano change, but i haven't been able to find a consistent way to reproduce it yet
 
 I think neither of these "fixes" are actually right.
 
 another one that is easy to reproduce:
 
 play a note
 press sustain pedal
 release sustain pedal (still holding note)
 change pianos by playing a different note
 press sustain pedal
 release both notes (first note cuts off, when it shouldn't because sustain pedal is down)
 next time you play/release notes, they should work ok, so it's only this first time, and it's only if the switch of pianos happens while another note is being held
 
 ALL FIXED (i think!)
 
 */

void BKAudioProcessor::sustainDeactivate(void)
{
    if(sustainIsDown)
    {
        sustainIsDown = false;
        DBG("SUSTAIN OFF");
        
        currentPiano->prepMap->sustainPedalReleased(noteOn, false);
        
        if(prevPiano != currentPiano) prevPiano->prepMap->sustainPedalReleased(noteOn, true);
        
        //turn off pedal down resonance
        pedalSynth.keyOff(channel,
                          PedalNote,
                          0,
                          0,
                          21,
                          21,
                          1.,
                          1.,
                          true);
        
        //play pedalUp sample
        pedalSynth.keyOn(channel,
                         //synthNoteNumber,
                         22,
                         22,
                         0,
                         0.03, //gain
                         1.,
                         Forward,
                         Normal, //FixedLength,
                         PedalNote,
                         0,
                         0,
                         0,
                         2000,
                         3,
                         3 );
    }
}

#define LITTLE_NOTE_MACHINE 0
#define NOTE 500

void BKAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    buffer.clear();
    
    if (!didLoadMainPianoSamples) return;
    
    if(wrapperType == wrapperType_AudioUnit ||
       wrapperType == wrapperType_VST ||
       wrapperType == wrapperType_VST3) //check this on setup; if(isPlugIn) {...
    {
        playHead = this->getPlayHead();
        playHead->getCurrentPosition (currentPositionInfo);
        hostTempo = currentPositionInfo.bpm;

        // hostTempo
        Tempo::PtrArr allTempoPreps = gallery->getAllTempo();
        for (auto p : allTempoPreps)
        {
            p->aPrep->setHostTempo(hostTempo);
        }
        // DBG("DAW bpm = " + String(currentPositionInfo.bpm));
    }
    
    int numSamples = buffer.getNumSamples();
    if (numSamples != levelBuf.getNumSamples()) levelBuf.setSize(buffer.getNumChannels(), numSamples);
    
    // Process all active prep maps in current piano
    
    if (currentPiano == nullptr) return;
    
    if (currentPiano->prepMap != nullptr)
    {
        currentPiano->prepMap->processBlock(buffer, numSamples, channel, loadingSampleType, false);
        
        // Process all active nostalgic preps in previous piano
        if(prevPiano != currentPiano)
            prevPiano->prepMap->processBlock(buffer, numSamples, channel, loadingSampleType, true); // true for onlyNostalgic
    }
    
	
	for (int i = 0; i < notesOnUI.size(); i++)
	{
		//if (keystrokesEnabled.getValue())
        int note = notesOnUI.getUnchecked(i);
        handleNoteOn(note, 0.6, channel, note, cMidiInputUI);
		notesOnUI.remove(i);
	}
    
    for(int i=0; i<notesOffUI.size(); i++)
    {
        int note = notesOffUI.getUnchecked(i);
        handleNoteOff(note, 0.6, channel, note, cMidiInputUI);
        notesOffUI.remove(i);
    }
    
    // MIDI message handling for plugin wrappers
    if(wrapperType == wrapperType_AudioUnit ||
       wrapperType == wrapperType_VST ||
       wrapperType == wrapperType_VST3) //check this on setup; if(isPlugIn) {...
    {
        for (const MidiMessageMetadata m : midiMessages)
        {
            // kludgy, but just trying to see if this works...
            handleIncomingMidiMessage(nullptr, m.getMessage());
        }
    }
  
	//if(didNoteOffs && !sustainIsDown) prevPianos.clearQuick(); //fixes phantom piano, but breaks Nostalgic keyUps over Piano changes. grr...
    
    // Sets some flags to determine whether to send noteoffs to previous pianos.
    if (!noteOnCount && !sustainIsDown) {
        
        /*
        // Process all active prep maps in previous piano
        for (auto pmap : prevPiano->prepMaps)
            pmap->processBlock(numSamples, m.getChannel());
        */
        
        prevPianos.clearQuick();
        if (!allNotesOff) allNotesOff = true;
    }

    mainPianoSynth.renderNextBlock(buffer,midiMessages,0, numSamples);
    hammerReleaseSynth.renderNextBlock(buffer,midiMessages,0, numSamples);
    resonanceReleaseSynth.renderNextBlock(buffer,midiMessages,0, numSamples);
    pedalSynth.renderNextBlock(buffer,midiMessages,0, numSamples);
    
#if JUCE_IOS
    buffer.applyGain(0, numSamples, 0.3 * gallery->getGeneralSettings()->getGlobalGain());
#else
    buffer.applyGain(0, numSamples, gallery->getGeneralSettings()->getGlobalGain());
#endif
    
    // store buffer for level calculation when needed
    levelBuf.copyFrom(0, 0, buffer, 0, 0, numSamples);
    if(levelBuf.getNumChannels() == 2) levelBuf.copyFrom(1, 0, buffer, 1, 0, numSamples);
    
}

double BKAudioProcessor::getLevelL()
{
    if(didLoadMainPianoSamples && levelBuf.getNumSamples() > 0) return levelBuf.getRMSLevel(0, 0, levelBuf.getNumSamples());
    else return 0.;
}

double BKAudioProcessor::getLevelR()
{
    if(levelBuf.getNumChannels() == 2) {
        if(didLoadMainPianoSamples) return levelBuf.getRMSLevel(1, 0, levelBuf.getNumSamples());
        else return 0.;
    }
    else return getLevelL();
}

// Piano
void  BKAudioProcessor::setCurrentPiano(int which)
{
    
    updateState->setCurrentDisplay(DisplayNil);

    if (noteOnCount)  prevPianos.addIfNotAlreadyThere(currentPiano);
    
    prevPiano = currentPiano;

    currentPiano = gallery->getPiano(which);

    if(currentPiano != nullptr)
    {
        for (auto bprocessor : prevPiano->getBlendronicProcessors())
            bprocessor->setActive(false);
        
        currentPiano->clearOldNotes(prevPiano); // to clearOldNotes so it doesn't playback shit from before
        currentPiano->configure();
        
        for (auto bprocessor : currentPiano->getBlendronicProcessors())
            bprocessor->setActive(true);
        
        currentPiano->copySynchronicState(prevPiano);
        currentPiano->copyAdaptiveTuningState(prevPiano);
        currentPiano->copyAdaptiveTempoState(prevPiano);
        
        updateState->pianoDidChangeForGraph = true;
        updateState->synchronicPreparationDidChange = true;
        updateState->nostalgicPreparationDidChange = true;
        updateState->directPreparationDidChange = true;
        updateState->blendronicPreparationDidChange = true;
        updateState->tempoPreparationDidChange = true;
        updateState->tuningPreparationDidChange = true;
        
        gallery->setDefaultPiano(which);
        gallery->setGalleryDirty(false);
        
        DBG("setting current piano to: " + String(which));
        
        if (sustainIsDown)
            currentPiano->prepMap->sustainPedalPressed();
    }
}

// Reset
void BKAudioProcessor::performResets(int noteNumber, String source)
{
    for (auto reset : currentPiano->modificationMap.getUnchecked(noteNumber)->directResets)
    {
        for (auto Id : reset.keymapIds)
        {
            Keymap::Ptr keymap = gallery->getKeymap(Id);
            
            if (keymap->keys().contains(noteNumber) && keymap->getAllMidiInputIdentifiers().contains(source))
            {
                currentPiano->getDirectProcessor(reset.prepId)->reset();
                updateState->directPreparationDidChange = true;
                break;
            }
        }
    }
    for (auto reset : currentPiano->modificationMap.getUnchecked(noteNumber)->synchronicResets)
    {
        for (auto Id : reset.keymapIds)
        {
            Keymap::Ptr keymap = gallery->getKeymap(Id);
            
            if (keymap->keys().contains(noteNumber) && keymap->getAllMidiInputIdentifiers().contains(source))
            {
                currentPiano->getSynchronicProcessor(reset.prepId)->reset();
                updateState->synchronicPreparationDidChange = true;
                break;
            }
        }
    }
    for (auto reset : currentPiano->modificationMap.getUnchecked(noteNumber)->nostalgicResets)
    {
        for (auto Id : reset.keymapIds)
        {
            Keymap::Ptr keymap = gallery->getKeymap(Id);
            
            if (keymap->keys().contains(noteNumber) && keymap->getAllMidiInputIdentifiers().contains(source))
            {
                currentPiano->getNostalgicProcessor(reset.prepId)->reset();
                updateState->nostalgicPreparationDidChange = true;
                break;
            }
        }
    }
    for (auto reset : currentPiano->modificationMap.getUnchecked(noteNumber)->blendronicResets)
    {
        for (auto Id : reset.keymapIds)
        {
            Keymap::Ptr keymap = gallery->getKeymap(Id);
            
            if (keymap->keys().contains(noteNumber) && keymap->getAllMidiInputIdentifiers().contains(source))
            {
                currentPiano->getBlendronicProcessor(reset.prepId)->reset();
                updateState->blendronicPreparationDidChange = true;
                break;
            }
        }
    }
    for (auto reset : currentPiano->modificationMap.getUnchecked(noteNumber)->tuningResets)
    {
        for (auto Id : reset.keymapIds)
        {
            Keymap::Ptr keymap = gallery->getKeymap(Id);
            
            if (keymap->keys().contains(noteNumber) && keymap->getAllMidiInputIdentifiers().contains(source))
            {
                currentPiano->getTuningProcessor(reset.prepId)->reset();
                updateState->tuningPreparationDidChange = true;
                break;
            }
        }
    }
    for (auto reset : currentPiano->modificationMap.getUnchecked(noteNumber)->tempoResets)
    {
        for (auto Id : reset.keymapIds)
        {
            Keymap::Ptr keymap = gallery->getKeymap(Id);
            
            if (keymap->keys().contains(noteNumber) && keymap->getAllMidiInputIdentifiers().contains(source))
            {
                currentPiano->getTempoProcessor(reset.prepId)->reset();
                updateState->tempoPreparationDidChange = true;
                break;
            }
        }
    }
}

// Modification
void BKAudioProcessor::performModifications(int noteNumber, String source)
{
    TuningModification::PtrArr tMod = currentPiano->modificationMap[noteNumber]->getTuningModifications();
    for (int i = tMod.size(); --i >= 0;)
    {
        TuningModification::Ptr mod = tMod[i];
        
        for (auto keymap : mod->getKeymaps())
        {
            if (keymap->keys().contains(noteNumber) && keymap->getAllMidiInputIdentifiers().contains(source))
            {
                Array<int> targets = mod->getTargets();
                for (auto target : targets)
                {
                    TuningPreparation::Ptr prep = gallery->getTuning(target)->aPrep;
                    prep->performModification(mod, mod->getDirty());
                }
                updateState->tuningPreparationDidChange = true;
                break;
            }
        }
    }
    
    TempoModification::PtrArr mMod = currentPiano->modificationMap[noteNumber]->getTempoModifications();
    for (int i = mMod.size(); --i >= 0;)
    {
        TempoModification::Ptr mod = mMod[i];
        
        for (auto keymap : mod->getKeymaps())
        {
            if (keymap->keys().contains(noteNumber) && keymap->getAllMidiInputIdentifiers().contains(source))
            {
                Array<int> targets = mod->getTargets();
                for (auto target : targets)
                {
                    TempoPreparation::Ptr prep = gallery->getTempo(target)->aPrep;
                    prep->performModification(mod, mod->getDirty());
                }
                updateState->tempoPreparationDidChange = true;
                break;
            }
        }
    }
    
    DirectModification::PtrArr dMod = currentPiano->modificationMap[noteNumber]->getDirectModifications();
    for (int i = dMod.size(); --i >= 0;)
    {
        DirectModification::Ptr mod = dMod[i];
        
        for (auto keymap : mod->getKeymaps())
        {
            if (keymap->keys().contains(noteNumber) && keymap->getAllMidiInputIdentifiers().contains(source))
            {
                Array<int> targets = mod->getTargets();
                for (auto target : targets)
                {
                    DirectPreparation::Ptr prep = gallery->getDirect(target)->prep;
                    prep->performModification(mod, mod->getDirty());
                }
                updateState->directPreparationDidChange = true;
                break;
            }
        }
    }
    
    NostalgicModification::PtrArr nMod = currentPiano->modificationMap[noteNumber]->getNostalgicModifications();
    for (int i = nMod.size(); --i >= 0;)
    {
        NostalgicModification::Ptr mod = nMod[i];
        
        for (auto keymap : mod->getKeymaps())
        {
            if (keymap->keys().contains(noteNumber) && keymap->getAllMidiInputIdentifiers().contains(source))
            {
                Array<int> targets = mod->getTargets();
                for (auto target : targets)
                {
                    NostalgicPreparation::Ptr prep = gallery->getNostalgic(target)->aPrep;
                    prep->performModification(mod, mod->getDirty());
                }
                updateState->nostalgicPreparationDidChange = true;
                break;
            }
        }
    }
    
    SynchronicModification::PtrArr sMod = currentPiano->modificationMap[noteNumber]->getSynchronicModifications();
    for (int i = sMod.size(); --i >= 0;)
    {
        SynchronicModification::Ptr mod = sMod[i];
        
        for (auto keymap : mod->getKeymaps())
        {
            if (keymap->keys().contains(noteNumber) && keymap->getAllMidiInputIdentifiers().contains(source))
            {
                Array<int> targets = mod->getTargets();
                for (auto target : targets)
                {
                    SynchronicPreparation::Ptr prep = gallery->getSynchronic(target)->aPrep;
                    prep->performModification(mod, mod->getDirty());
                }
                updateState->synchronicPreparationDidChange = true;
                break;
            }
        }
    }
    
    BlendronicModification::PtrArr bMod = currentPiano->modificationMap[noteNumber]->getBlendronicModifications();
    for (int i = bMod.size(); --i >= 0;)
    {
        BlendronicModification::Ptr mod = bMod[i];
        
        for (auto keymap : mod->getKeymaps())
        {
            if (keymap->keys().contains(noteNumber) && keymap->getAllMidiInputIdentifiers().contains(source))
            {
                Array<int> targets = mod->getTargets();
                for (auto target : targets)
                {
                    BlendronicPreparation::Ptr prep = gallery->getBlendronic(target)->aPrep;
                    prep->performModification(mod, mod->getDirty());
                }
                updateState->blendronicPreparationDidChange = true;
                break;
            }
        }
    }
}

void BKAudioProcessor::importSoundfont(void)
{
    fc = new FileChooser ("Import your gallery",
                          File::getCurrentWorkingDirectory(),
                          "*",
                          true);
    
    fc->launchAsync (FileBrowserComponent::openMode | FileBrowserComponent::canSelectFiles,
                     [this] (const FileChooser& chooser)
                     {
                         auto results = chooser.getURLResults();
                         if (results.size() > 0)
                         {
                             auto url = results.getReference (0);
                             
                             std::unique_ptr<InputStream> wi (url.createInputStream (false));
                             
                             if (wi != nullptr)
                             {
                                 MemoryBlock block(wi->getTotalLength());
                                 wi->readIntoMemoryBlock(block);
        
                                 File file = File::getSpecialLocation (File::userDocumentsDirectory).getChildFile(url.getFileName());
                                 file.create();
                                 
                                 FileOutputStream fos (file);
                    
                                 fos.write (&block, block.getSize());
                                 
                                 fos.flush();
                                 
                                 //if( fos.openedOk())
                                 {
                                     loadSamples(BKLoadSoundfont, file.getFullPathName(), 0);
                                 }
                             }
                         }
                     });
}

void BKAudioProcessor::importCurrentGallery(void)
{
    fc = new FileChooser ("Import your gallery",
                          File::getCurrentWorkingDirectory(),
                          "*.xml",
                          true);
    
    fc->launchAsync (FileBrowserComponent::openMode | FileBrowserComponent::canSelectFiles,
     [this] (const FileChooser& chooser)
     {
         auto results = chooser.getURLResults();
         if (results.size() > 0)
         {
             auto url = results.getReference (0);
            
             if (url.createInputStream (false) != nullptr)
             {
                 std::shared_ptr<XmlElement> xml = url.readEntireXmlStream();
                
                 createNewGallery(url.getFileName().replace("%20", " "), xml);
             }
         }
     });

}

void BKAudioProcessor::exportCurrentGallery(void)
{
    if (defaultLoaded)
    {
        AlertWindow::showMessageBoxAsync (AlertWindow::InfoIcon,
                                          "Export not available",
                                          "You cannot export a default gallery.");
        return;
    }
    saveCurrentGallery();

    File fileToSave (gallery->getURL());
    
    fc = new FileChooser ("Export your gallery.",
                          fileToSave,
                          "*",
                          true);
    
    fc->launchAsync (FileBrowserComponent::saveMode | FileBrowserComponent::canSelectFiles | FileBrowserComponent::canSelectDirectories,
                     [fileToSave] (const FileChooser& chooser)
                     {
                         auto result = chooser.getURLResult();
                         auto name = result.isEmpty() ? String()
                         : (result.isLocalFile() ? result.getLocalFile().getFullPathName()
                            : result.toString (true));
                         
                         // Android and iOS file choosers will create placeholder files for chosen
                         // paths, so we may as well write into those files.
                         if (! result.isEmpty())
                         {
                             std::unique_ptr<InputStream> wi (fileToSave.createInputStream());
                             std::unique_ptr<OutputStream> wo (result.createOutputStream());
                             
                             if (wi != nullptr && wo != nullptr)
                             {
                                 //auto numWritten = wo->writeFromInputStream (*wi, -1);
                                 wo->flush();
                             }
                         }
                     });

}

void BKAudioProcessor::saveCurrentGalleryAs(void)
{
    FileChooser myChooser ("Save gallery to file...",
                           lastGalleryPath,
                           "*.xml");
    
    if (myChooser.browseForFileToSave(true))
    {
        writeCurrentGalleryToURL(myChooser.getResult().getFullPathName());
    }
    
    updateGalleries();
    if (wrapperType == wrapperType_Standalone) getPluginHolder()->savePluginState();
}

void BKAudioProcessor::saveCurrentGallery(void)
{
    if (defaultLoaded) return;
    if (gallery->getURL() == "")
    {
#if JUCE_IOS
        writeCurrentGalleryToURL( File::getSpecialLocation(File::userDocumentsDirectory).getFullPathName() + "/" + gallery->getName());
#endif
#if JUCE_MAC
        writeCurrentGalleryToURL( File::getSpecialLocation(File::globalApplicationsDirectory).getFullPathName() + "/bitKlavier/galleries/" + gallery->getName());
#endif
#if JUCE_WINDOWS || JUCE_LINUX
        writeCurrentGalleryToURL( File::getSpecialLocation(File::userDocumentsDirectory).getChildFile("bitKlavier").getFullPathName() + "\\bitKlavier\\galleries\\" + gallery->getName());
#endif
    }
    else
    {
        writeCurrentGalleryToURL(gallery->getURL());
    }
    if (wrapperType == wrapperType_Standalone) getPluginHolder()->savePluginState();
}


// Gallery/Preset Management
void BKAudioProcessor::loadGalleryDialog(void)
{
    int galleryIsDirtyAlertResult = 0;
    if(gallery->isGalleryDirty())
    {
        DBG("GALLERY IS DIRTY, CHECK FOR SAVE HERE");
        
        galleryIsDirtyAlertResult = AlertWindow::showYesNoCancelBox (AlertWindow::QuestionIcon,
                                                                     "The current gallery has been modified.",
                                                                     "Do you want to save before loading a new gallery?",
                                                                     String(),
                                                                     String(),
                                                                     String(),
                                                                     0,
                                                                     //ModalCallbackFunction::forComponent (alertBoxResultChosen, this)
                                                                     nullptr);
        
        
        if(galleryIsDirtyAlertResult == 0)
        {
            return;
        }
        else if(galleryIsDirtyAlertResult == 1)
        {
            DBG("saving gallery first");

            saveCurrentGallery();
        }
    }
    
    updateState->loadedJson = false;
    
    FileChooser myChooser ("Load gallery from xml file...",
                           //File::getSpecialLocation (File::userHomeDirectory),
                           lastGalleryPath,
                           "*.xml");
    
    if (myChooser.browseForFileToOpen())
    {
        File myFile (myChooser.getResult());

        File user   (File::getSpecialLocation(File::globalApplicationsDirectory));
        user = user.getChildFile("bitKlavier/galleries/");
        
        user = user.getChildFile(myFile.getFileName());
        
        if (myFile.getFullPathName() != user.getFullPathName())
        {
            if (myFile.moveFileTo(user))    DBG("MOVED");
            else                            DBG("NOT MOVED");
        }
        
        std::unique_ptr<XmlElement> xml = XmlDocument::parse (user);
        
        if (xml != nullptr /*&& xml->hasTagName ("foobar")*/)
        {
            currentGallery = user.getFileName();
            
            gallery = new Gallery(xml.get(), *this);
            
            gallery->setURL(user.getFullPathName());
            
            initializeGallery();
            
            lastGalleryPath = user;
        }
    }
    
}

void BKAudioProcessor::loadGalleryFromXml(XmlElement* xml, bool resetHistory)
{
    if (xml != nullptr /*&& xml->hasTagName ("foobar")*/)
    {
        // if (currentPiano != nullptr) currentPiano->deconfigure();
        
        if (gallery != nullptr)
        {
            for (auto piano : gallery->getPianos())
            {
                piano->deconfigure();
            }
        }
        
        gallery = new Gallery(xml, *this);

        currentGallery = gallery->getName() + ".xml";
        
        initializeGallery();
        
        gallery->setGalleryDirty(false);
    }
    
    currentPiano->configure();

    for (auto bprocessor : currentPiano->getBlendronicProcessors())
        bprocessor->setActive(true);
    
    if (resetHistory) resetGalleryHistory();
}

void BKAudioProcessor::loadGalleryFromPath(String path)
{
    updateState->loadedJson = false;
    
    if (path == "")
    {
        String xmlData = CharPointer_UTF8 (BinaryData::Basic_Piano_xml);
        
        defaultLoaded = true;
        defaultName = "Basic_Piano_xml";
        
        loadGalleryFromXml(XmlDocument::parse(xmlData).get());
    }
    else
    {
        File myFile (path);

        std::unique_ptr<XmlElement> xml = XmlDocument::parse(myFile);

        loadGalleryFromXml(xml.get());

        gallery->setURL(path);
    }
}

void BKAudioProcessor::loadJsonGalleryDialog(void)
{
    
    int galleryIsDirtyAlertResult = 0;
    if(gallery->isGalleryDirty())
    {
        DBG("GALLERY IS DIRTY, CHECK FOR SAVE HERE");
        
        galleryIsDirtyAlertResult = AlertWindow::showYesNoCancelBox (AlertWindow::QuestionIcon,
                                                                     "The current gallery has changed!",
                                                                     "do you want to save it before loading a new gallery?",
                                                                     String(),
                                                                     String(),
                                                                     String(),
                                                                     0,
                                                                     //ModalCallbackFunction::forComponent (alertBoxResultChosen, this)
                                                                     nullptr);
        
        
        if(galleryIsDirtyAlertResult == 0)
        {
            return;
        }
        else if(galleryIsDirtyAlertResult == 1)
        {
            DBG("saving gallery first");
            saveCurrentGallery();
        }
    }
    
    FileChooser myChooser ("Load gallery from json file...",
                           File::getSpecialLocation (File::userHomeDirectory),
                           "*.json");
    
    
    if (myChooser.browseForFileToOpen())
    {
        updateState->loadedJson = true;
        
        File myFile (myChooser.getResult());
        
        File user   (File::getSpecialLocation(File::globalApplicationsDirectory));
        user = user.getChildFile("bitKlavier/galleries/");
        
        user = user.getChildFile(myFile.getFileName());
        
        if (myFile.getFullPathName() != user.getFullPathName())
        {
            if (myFile.moveFileTo(user))    DBG("MOVED");
            else                            DBG("NOT MOVED");
        }
        
        currentGallery = user.getFileName();
        
        var myJson = JSON::parse(user);
        
        gallery = new Gallery(myJson, *this);
        
        gallery->setURL(user.getFullPathName());
        
        initializeGallery();
        
        gallery->setGalleryDirty(false);
    }
}

void BKAudioProcessor::loadJsonGalleryFromPath(String path)
{
    updateState->loadedJson = true;
    
    File myFile (path);
    
    currentGallery = myFile.getFileName();
    
    var myJson = JSON::parse(myFile);
    
    gallery = new Gallery(myJson, *this);
    
    initializeGallery();
    
}


void BKAudioProcessor::initializeGallery()
{
    prevPiano = gallery->getPianos().getFirst();
    
    int defPiano = gallery->getDefaultPiano();

    //if (defPiano >= gallery->getNumPianos() || defPiano < 1)
    if (defPiano < 1)
    {
        defPiano = gallery->getPianos().getFirst()->getId();
    }

    currentPiano = gallery->getPiano(defPiano);
    if(currentPiano == nullptr)
    {
        defPiano = gallery->getPianos().getFirst()->getId();
        currentPiano = gallery->getPiano(defPiano);
    }
    
    for (auto piano : gallery->getPianos())
    {
        piano->configure();
        if (piano->getId() > gallery->getIdCount(PreparationTypePiano)) gallery->setIdCount(PreparationTypePiano, piano->getId());
        if (piano != currentPiano)
            for (auto bprocessor : piano->getBlendronicProcessors())
                bprocessor->setActive(false);
    }

    gallery->prepareToPlay(getSampleRate()); 
    
    updateUI();
    
    updateGalleries();
}

Array<MidiDeviceInfo> BKAudioProcessor::getMidiOutputDevices()
{
    return MidiOutput::getAvailableDevices();
}

Array<MidiDeviceInfo> BKAudioProcessor::getMidiInputDevices()
{
    return MidiInput::getAvailableDevices();
}

std::unique_ptr<MidiInput> BKAudioProcessor::openMidiInputDevice(const String &deviceIdentifier, MidiInputCallback* callback)
{
    return MidiInput::openDevice(deviceIdentifier, callback);
}

void BKAudioProcessor::handleIncomingMidiMessage(MidiInput* source, const MidiMessage& m)
{
    int noteNumber = m.getNoteNumber();
    float velocity = m.getFloatVelocity();
    
    String sourceIdentifier;
    if(source != nullptr) sourceIdentifier = source->getIdentifier();
    else sourceIdentifier = cMidiInputDAW;
    
    channel = m.getChannel();
    
    if (m.isNoteOn()) //&& keystrokesEnabled.getValue())
    {
        handleNoteOn(noteNumber, velocity, channel, noteNumber, sourceIdentifier);
    }
    else if (m.isNoteOff())
    {
        handleNoteOff(noteNumber, velocity, channel, noteNumber, sourceIdentifier);
        //didNoteOffs = true;
    }
    
    // NEED WAY TO TRIGGER RELEASE PEDAL SAMPLE FOR SFZ
    else if (m.isSustainPedalOn())
    {
        //DBG("m.isSustainPedalOn()");
        sustainInverted = gallery->getGeneralSettings()->getInvertSustain();
        if (sustainInverted)    sustainDeactivate();
        else                    sustainActivate();
        
    }
    else if (m.isSustainPedalOff())
    {
        //DBG("m.isSustainPedalOff()");
        sustainInverted = gallery->getGeneralSettings()->getInvertSustain();
        if (sustainInverted)    sustainActivate();
        else                    sustainDeactivate();
    }
    else
    {
        mainPianoSynth.handleMidiEvent(m);
        hammerReleaseSynth.handleMidiEvent(m);
        resonanceReleaseSynth.handleMidiEvent(m);
        pedalSynth.handleMidiEvent(m);
    }
}

void BKAudioProcessor::hiResTimerCallback()
{
    for (auto d : gallery->getAllDirect())
    {
        d->prep->stepModdables();
    }
}

void BKAudioProcessor::reset(BKPreparationType type, int Id)
{
    if (type == PreparationTypeDirect)
    {
        DirectProcessor::Ptr proc = currentPiano->getDirectProcessor(Id, false);
        
        if (proc != nullptr) proc->reset();
    }
    else if (type == PreparationTypeNostalgic)
    {
        NostalgicProcessor::Ptr proc = currentPiano->getNostalgicProcessor(Id, false);
        
        if (proc != nullptr) proc->reset();
    }
    else if (type == PreparationTypeSynchronic)
    {
        SynchronicProcessor::Ptr proc = currentPiano->getSynchronicProcessor(Id, false);
        
        if (proc != nullptr) proc->reset();
    }
    else if (type == PreparationTypeBlendronic)
    {
        BlendronicProcessor::Ptr proc = currentPiano->getBlendronicProcessor(Id, false);
        
        if (proc != nullptr) proc->reset();
    }
    else if (type == PreparationTypeTuning)
    {
        TuningProcessor::Ptr proc = currentPiano->getTuningProcessor(Id, false);
        
        if (proc != nullptr) proc->reset();
    }
    else if (type == PreparationTypeTempo)
    {
        TempoProcessor::Ptr proc = currentPiano->getTempoProcessor(Id, false);
        
        if (proc != nullptr) proc->reset();
    }
    else if (type == PreparationTypeKeymap)
    {
        gallery->getKeymap(Id)->clear();
    }
    if (type == PreparationTypeDirectMod)
    {
        gallery->getDirectModification(Id)->reset();
    }
    else if (type == PreparationTypeNostalgicMod)
    {
        gallery->getNostalgicModification(Id)->reset();
    }
    else if (type == PreparationTypeSynchronicMod)
    {
        gallery->getSynchronicModification(Id)->reset();
    }
    else if (type == PreparationTypeBlendronicMod)
    {
        gallery->getBlendronicModification(Id)->reset();
    }
    else if (type == PreparationTypeTuningMod)
    {
        gallery->getTuningModification(Id)->reset();
    }
    else if (type == PreparationTypeTempoMod)
    {
        gallery->getTempoModification(Id)->reset();
    }
    
}

void BKAudioProcessor::clear(BKPreparationType type, int Id)
{
    if (type == PreparationTypeDirect)
    {
        gallery->getDirect(Id)->clear();
        
        DirectProcessor::Ptr proc = currentPiano->getDirectProcessor(Id, false);
        
        if (proc != nullptr) proc->reset();
    }
    else if (type == PreparationTypeNostalgic)
    {
        gallery->getNostalgic(Id)->clear();
        
        NostalgicProcessor::Ptr proc = currentPiano->getNostalgicProcessor(Id, false);
        
        if (proc != nullptr) proc->reset();
    }
    else if (type == PreparationTypeSynchronic)
    {
        gallery->getSynchronic(Id)->clear();
        
        SynchronicProcessor::Ptr proc = currentPiano->getSynchronicProcessor(Id, false);
        
        if (proc != nullptr) proc->reset();
    }
    else if (type == PreparationTypeBlendronic)
    {
        gallery->getBlendronic(Id)->clear();
        
        BlendronicProcessor::Ptr proc = currentPiano->getBlendronicProcessor(Id, false);
        
        if (proc != nullptr) proc->reset();
    }
    else if (type == PreparationTypeTuning)
    {
        gallery->getTuning(Id)->clear();
        
        TuningProcessor::Ptr proc = currentPiano->getTuningProcessor(Id, false);
        
        if (proc != nullptr) proc->reset();
    }
    else if (type == PreparationTypeTempo)
    {
        gallery->getTempo(Id)->clear();
        
        TempoProcessor::Ptr proc = currentPiano->getTempoProcessor(Id, false);
        
        if (proc != nullptr) proc->reset();
    }
    else if (type == PreparationTypeKeymap)
    {
        gallery->getKeymap(Id)->clear();
    }
    if (type == PreparationTypeDirectMod)
    {
        gallery->getDirectModification(Id)->reset();
    }
    else if (type == PreparationTypeNostalgicMod)
    {
        gallery->getNostalgicModification(Id)->reset();
    }
    else if (type == PreparationTypeSynchronicMod)
    {
        gallery->getSynchronicModification(Id)->reset();
    }
    else if (type == PreparationTypeBlendronicMod)
    {
        gallery->getBlendronicModification(Id)->reset();
    }
    else if (type == PreparationTypeTuningMod)
    {
        gallery->getTuningModification(Id)->reset();
    }
    else if (type == PreparationTypeTempoMod)
    {
        gallery->getTempoModification(Id)->reset();
    }
    
}

StandalonePluginHolder* BKAudioProcessor::getPluginHolder()
{
    return StandalonePluginHolder::getInstance();
}

AudioDeviceManager* BKAudioProcessor::getAudioDeviceManager()
{
    return &getPluginHolder()->deviceManager;
}

AudioProcessorPlayer* BKAudioProcessor::getAudioProcessorPlayer()
{
    return &getPluginHolder()->player;
}

double BKAudioProcessor::getCurrentSampleRate()
{
    return currentSampleRate;
}


