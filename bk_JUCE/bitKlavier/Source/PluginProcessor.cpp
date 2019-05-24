
#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "BKPianoSampler.h"

#if JUCE_IOS
int fontHeight;

int gComponentComboBoxHeight;
int gComponentLabelHeight;
int gComponentTextFieldHeight;

int gComponentRangeSliderHeight;
int gComponentSingleSliderHeight;
int gComponentStackedSliderHeight;
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
            
            ScopedPointer<XmlElement> xml = vt1.createXml();
            
            // GALLERY 2
            processor.gallery = new Gallery(xml, processor);
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
firstTime(true),
updateState(new BKUpdateState()),
mainPianoSynth(),
hammerReleaseSynth(),
resonanceReleaseSynth(),
pedalSynth(),
doneWithSetStateInfo(false),
loader(*this)
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
    
    gComponentComboBoxHeight = heightUnit;
    gComponentLabelHeight = heightUnit * 0.75f;
    gComponentTextFieldHeight = heightUnit * 0.75f;
    
    gComponentRangeSliderHeight = heightUnit * 1.25f;
    gComponentSingleSliderHeight = heightUnit * 1.25f;
    gComponentStackedSliderHeight = heightUnit * 1.25f;

#endif
    
    uiScaleFactor = (w_factor + h_factor) * 0.5f;
    
    uiScaleFactor *= 1.15f; // making up for smallness on little ios devices
    
    uiScaleFactor = (uiScaleFactor > 1.0f) ? 1.0f : uiScaleFactor;
    
    collectGalleries();
    collectPianos();
    collectPreparations();
    collectSoundfonts();
    
    updateUI();
    
    String xmlData = CharPointer_UTF8 (BinaryData::Basic_Piano_xml);
    
    defaultLoaded = true;
    defaultName = "Basic_Piano_xml";
    
    loadGalleryFromXml(XmlDocument::parse(xmlData));

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
    for(int i=0; i< 128; i++)
    {
        noteOn.set(i, false);
        noteVelocity.set(i, 0.);
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
        
    });
    
    mikroetudes = StringArray({
        "And So...",
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
        "Listen!",
        "Mama's Musette",
        "Petite Gymnopedie",
        "Pyramids",
        "Quickie",
        "Scales within Sliding Scales",
        "Slow To Come Back",
        "Southwing",
        "Who do you think you are, Mars?",
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
        
        currentSoundfont = sfzFile.getFullPathName();
        
        loadSamples(BKLoadSoundfont, sfzFile.getFullPathName());
    }
#endif
    
}

void BKAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
#if JUCE_IOS
    stk::Stk::setSampleRate(sampleRate);
#endif
    stk::Stk::setSampleRate(sampleRate); //crashes Logic Audio Unit Validation Tool
    
    mainPianoSynth.setCurrentPlaybackSampleRate(sampleRate);
    hammerReleaseSynth.setCurrentPlaybackSampleRate(sampleRate);
    resonanceReleaseSynth.setCurrentPlaybackSampleRate(sampleRate);
    pedalSynth.setCurrentPlaybackSampleRate(sampleRate);
    
    //mainPianoSynth.setGeneralSettings(gallery->getGeneralSettings());
    resonanceReleaseSynth.setGeneralSettings(gallery->getGeneralSettings());
    hammerReleaseSynth.setGeneralSettings(gallery->getGeneralSettings());
    pedalSynth.setGeneralSettings(gallery->getGeneralSettings());
    
    levelBuf.setSize(2, 25);
    
    gallery->prepareToPlay(sampleRate);
    
    sustainIsDown = false;
    
    if (!didLoadMainPianoSamples)
    {
        if (!loader.isThreadRunning())
        {
    #if JUCE_IOS
            loadSamples(BKLoadLite);
    #else
            if (wrapperType == wrapperType_AudioUnit)
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
    
    ScopedPointer<XmlElement> myXML = galleryVT.createXml();
    
    myXML->writeToFile(myFile, String::empty);
    
    loadGalleryFromXml(myXML);
    
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


void BKAudioProcessor::createNewGallery(String name, ScopedPointer<XmlElement> xml)
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
        xml->writeToFile(myFile, "");
    }
    else
    {
        xml->setAttribute("name", galleryName);
        xml->writeToFile(myFile, "");
    }
    
    xml->writeToFile(myFile, "");
    
    galleryNames.add(myFile.getFullPathName());
    
    if (xml != nullptr)
    {
        currentGallery = myFile.getFileName();
        
        DBG("new gallery: " + currentGallery);

        gallery = new Gallery(xml, *this);
        
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

void BKAudioProcessor::handleNoteOn(int noteNumber, float velocity, int channel)  
{
    int p;
    
    ++noteOnCount;
    noteOn.set(noteNumber, true);
    noteVelocity.set(noteNumber, velocity);
    
    if (allNotesOff)   allNotesOff = false;
    
    // Check PianoMap for whether piano should change due to key strike.
    int whichPiano = currentPiano->pianoMap[noteNumber];
    if (whichPiano > 0 && whichPiano != currentPiano->getId())
    {
        DBG("change piano to " + String(whichPiano));
        setCurrentPiano(whichPiano);
    }
    
    // modifications
    performResets(noteNumber);
    performModifications(noteNumber);
    
    //tempo
    for (p = prevPiano->activePMaps.size(); --p >= 0;) {
        if (prevPianos[p] != currentPiano)
            prevPiano->activePMaps[p]->clearKey(noteNumber); //clears key from array of depressed notes in prevPiano so they don't get cutoff by sustain pedal release
    }
    
    // Send key on to each pmap in current piano
    for (p = currentPiano->activePMaps.size(); --p >= 0;) {
        //DBG("noteon: " +String(noteNumber) + " pmap: " + String(p));
        currentPiano->activePMaps[p]->keyPressed(noteNumber, velocity, channel, (currentSampleType == BKLoadSoundfont));
    }
    
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

void BKAudioProcessor::handleNoteOff(int noteNumber, float velocity, int channel)
{
    int p, pm;
    
    noteOn.set(noteNumber, false);
    //DBG("noteoff velocity = " + String(velocity));
    
    noteOnSetsNoteOffVelocity = gallery->getGeneralSettings()->getNoteOnSetsNoteOffVelocity();
    if(noteOnSetsNoteOffVelocity) velocity = noteVelocity.getUnchecked(noteNumber);
    else if(velocity <= 0) velocity = 0.7; //for keyboards that don't do proper noteOff messages
    
    // Send key off to each pmap in current piano
    for (p = currentPiano->activePMaps.size(); --p >= 0;)
        currentPiano->activePMaps[p]->keyReleased(noteNumber, velocity, channel, (currentSampleType == BKLoadSoundfont));
    

    // This is to make sure note offs are sent to Direct and Nostalgic processors from previous pianos with holdover notes.

    for (p = prevPianos.size(); --p >= 0;) {
        if (prevPianos[p] != currentPiano) {
            for (pm = prevPianos[p]->activePMaps.size(); --pm >= 0;) {
                prevPianos[p]->activePMaps[pm]->postRelease(noteNumber, velocity, channel);
            }
        }
    }

    /*
     //do this in the PreparationMap, not here; springs should be turned on/off by Keymap
    for ( auto t : currentPiano->getTuningProcessors())
    {
        t->getTuning()->getCurrentSpringTuning()->removeNote(noteNumber);
    }
    */
    
    --noteOnCount;
    if(noteOnCount < 0) noteOnCount = 0;
    
}

void BKAudioProcessor::sustainActivate(void)
{
    DBG("BKAudioProcessor::sustainActivate");
    if(!sustainIsDown)
    {
        sustainIsDown = true;
        DBG("SUSTAIN ON");
        
        for (int p = currentPiano->activePMaps.size(); --p >= 0;)
            currentPiano->activePMaps[p]->sustainPedalPressed();
        
        for (int p = prevPiano->activePMaps.size(); --p >= 0;)
            prevPiano->activePMaps[p]->sustainPedalPressed();
        
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
        
        for (int p = currentPiano->activePMaps.size(); --p >= 0;)
            currentPiano->activePMaps[p]->sustainPedalReleased(noteOn, false);
        
        if(prevPiano != currentPiano)
        {
            for (int p = prevPiano->activePMaps.size(); --p >= 0;)
                //prevPiano->activePMaps[p]->sustainPedalReleased(true);
                prevPiano->activePMaps[p]->sustainPedalReleased(noteOn, true);
        }
        
        //turn off pedal down resonance
        pedalSynth.keyOff(channel,
                      PedalNote,
                      0,
                      21,
                      21,
                      1.,
                      true);
        
        //play pedalUp sample
        pedalSynth.keyOn(channel,
                         //synthNoteNumber,
                         22,
                         22,
                         0,
                         0.1, //gain
                         1.,
                         Forward,
                         Normal, //FixedLength,
                         PedalNote,
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

        //hostTempo = 113; //confirmed that the following is working...
        Tempo::PtrArr allTempoPreps = gallery->getAllTempo();
        for (auto p : allTempoPreps)
        {
            p->aPrep->setHostTempo(hostTempo);
        }
        DBG("DAW bpm = " + String(currentPositionInfo.bpm));
    }
 
    int time;
    bool didNoteOffs = false;
    
    int numSamples = buffer.getNumSamples();
    if(numSamples != levelBuf.getNumSamples()) levelBuf.setSize(buffer.getNumChannels(), numSamples);
    
    // Process all active prep maps in current piano
    
    if (currentPiano == nullptr) return;
    
    for (auto pmap : currentPiano->activePMaps)
        pmap->processBlock(numSamples, channel, currentSampleType, false);
    
    // OLAGON: Process all active nostalgic preps in previous piano
    if(prevPiano != currentPiano)
    {
        for (auto pmap : prevPiano->activePMaps)
            pmap->processBlock(numSamples, channel, currentSampleType, true); // true for onlyNostalgic
    }

    
    for(int i=0; i<notesOnUI.size(); i++)
    {
        handleNoteOn(notesOnUI.getUnchecked(i), 0.6, channel);
        notesOnUI.remove(i);
    }
    
    for(int i=0; i<notesOffUI.size(); i++)
    {
        handleNoteOff(notesOffUI.getUnchecked(i), 0.6, channel);
        notesOffUI.remove(i);
    }
    
    
    MidiMessage m;
    for (MidiBuffer::Iterator i (midiMessages); i.getNextEvent (m, time);)
    {
        int noteNumber = m.getNoteNumber();
        float velocity = m.getFloatVelocity();
         
        channel = m.getChannel();
        
        if (m.isNoteOn())
        {
            handleNoteOn(noteNumber, velocity, channel);
        }
        else if (m.isNoteOff())
        {
            handleNoteOff(noteNumber, velocity, channel);
            didNoteOffs = true;
        }
        
        // NEED WAY TO TRIGGER RELEASE PEDAL SAMPLE FOR SFZ
        if (m.isSustainPedalOn())
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
    }
    
    //if(didNoteOffs && !sustainIsDown) prevPianos.clearQuick(); //fixes phantom piano, but breaks Nostalgic keyUps over Piano changes. grr...
    
    // Sets some flags to determine whether to send noteoffs to previous pianos.
    if (!allNotesOff && !noteOnCount) {
        
        /*
        // Process all active prep maps in previous piano
        for (auto pmap : prevPiano->activePMaps)
            pmap->processBlock(numSamples, m.getChannel());
        */
        
        prevPianos.clearQuick();
        allNotesOff = true;
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
    if(didLoadMainPianoSamples) return levelBuf.getRMSLevel(0, 0, levelBuf.getNumSamples());
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
    
    //gallery->resetPreparations(); //modded preps should remain modded across piano changes; user can Reset if desired

    if (noteOnCount)  prevPianos.addIfNotAlreadyThere(currentPiano);

    prevPiano = currentPiano;

    currentPiano = gallery->getPiano(which);
    currentPiano->clearOldNotes(prevPiano); // to clearOldNotes so it doesn't playback shit from before
    currentPiano->copyAdaptiveTuningState(prevPiano);
    currentPiano->copyAdaptiveTempoState(prevPiano);
    
    updateState->pianoDidChangeForGraph = true;
    updateState->synchronicPreparationDidChange = true;
    updateState->nostalgicPreparationDidChange = true;
    updateState->directPreparationDidChange = true;
    updateState->tempoPreparationDidChange = true;
    updateState->tuningPreparationDidChange = true;
    
    gallery->setDefaultPiano(which);
    gallery->setGalleryDirty(false);
    
    DBG("setting current piano to: " + String(which));
    
    if (sustainIsDown)
    {
        for (int p = currentPiano->activePMaps.size(); --p >= 0;)
            currentPiano->activePMaps[p]->sustainPedalPressed();
    }
}

// Reset
void BKAudioProcessor::performResets(int noteNumber)
{
    for (auto prep : currentPiano->modificationMap.getUnchecked(noteNumber)->directReset)
    {
        currentPiano->getDirectProcessor(prep)->reset();
        updateState->directPreparationDidChange = true;
    }
    for (auto prep : currentPiano->modificationMap.getUnchecked(noteNumber)->synchronicReset)
    {
        currentPiano->getSynchronicProcessor(prep)->reset();
        updateState->synchronicPreparationDidChange = true;
    }
    for (auto prep : currentPiano->modificationMap.getUnchecked(noteNumber)->nostalgicReset)
    {
        currentPiano->getNostalgicProcessor(prep)->reset();
        updateState->nostalgicPreparationDidChange = true;
    }
    for (auto prep : currentPiano->modificationMap.getUnchecked(noteNumber)->tuningReset)
    {
        currentPiano->getTuningProcessor(prep)->reset();
        updateState->tuningPreparationDidChange = true;
    }
    for (auto prep : currentPiano->modificationMap.getUnchecked(noteNumber)->tempoReset)
    {
        currentPiano->getTempoProcessor(prep)->reset();
        updateState->tempoPreparationDidChange = true;
    }
}

// Modification
void BKAudioProcessor::performModifications(int noteNumber)
{
    TuningModification::PtrArr tMod = currentPiano->modificationMap[noteNumber]->getTuningModifications();
    for (int i = tMod.size(); --i >= 0;)
    {
        TuningModification::Ptr mod = tMod[i];
        Array<int> targets = mod->getTargets();
        
        for (auto target : targets)
        {
            TuningPreparation::Ptr prep = gallery->getTuning(target)->aPrep;
            prep->copy(mod);
        }
 
        updateState->tuningPreparationDidChange = true;
    }
    
    TempoModification::PtrArr mMod = currentPiano->modificationMap[noteNumber]->getTempoModifications();
    for (int i = mMod.size(); --i >= 0;)
    {
        TempoModification::Ptr mod = mMod[i];
        Array<int> targets = mod->getTargets();
        
        for (auto target : targets)
        {
            TempoPreparation::Ptr prep = gallery->getTempo(target)->aPrep;
            prep->copy(mod);
        }
        
        updateState->tempoPreparationDidChange = true;
    }
    
    DirectModification::PtrArr dMod = currentPiano->modificationMap[noteNumber]->getDirectModifications();
    for (int i = dMod.size(); --i >= 0;)
    {
        DirectModification::Ptr mod = dMod[i];
        Array<int> targets = mod->getTargets();
        
        for (auto target : targets)
        {
            DirectPreparation::Ptr prep = gallery->getDirect(target)->aPrep;
            prep->copy(mod);
        }
        
        updateState->directPreparationDidChange = true;
    }
    
    NostalgicModification::PtrArr nMod = currentPiano->modificationMap[noteNumber]->getNostalgicModifications();
    for (int i = nMod.size(); --i >= 0;)
    {
        NostalgicModification::Ptr mod = nMod[i];
        Array<int> targets = mod->getTargets();
        
        for (auto target : targets)
        {
            NostalgicPreparation::Ptr prep = gallery->getNostalgic(target)->aPrep;
            prep->copy(mod);
        }
        
        updateState->nostalgicPreparationDidChange = true;
    }
    
    SynchronicModification::PtrArr sMod = currentPiano->modificationMap[noteNumber]->getSynchronicModifications();
    for (int i = sMod.size(); --i >= 0;)
    {
        SynchronicModification::Ptr mod = sMod[i];
        Array<int> targets = mod->getTargets();
        
        for (auto target : targets)
        {
            SynchronicPreparation::Ptr prep = gallery->getSynchronic(target)->aPrep;
            prep->copy(mod);
        }
        
        updateState->synchronicPreparationDidChange = true;
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
                             
                             ScopedPointer<InputStream> wi (url.createInputStream (false));
                             
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
                 ScopedPointer<XmlElement> xml = url.readEntireXmlStream();
                
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
                             ScopedPointer<InputStream> wi (fileToSave.createInputStream());
                             ScopedPointer<OutputStream> wo (result.createOutputStream());
                             
                             if (wi != nullptr && wo != nullptr)
                             {
                                 auto numWritten = wo->writeFromInputStream (*wi, -1);
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
        
        ScopedPointer<XmlElement> xml (XmlDocument::parse (user));
        
        if (xml != nullptr /*&& xml->hasTagName ("foobar")*/)
        {
            currentGallery = user.getFileName();
            
            gallery = new Gallery(xml, *this);
            
            gallery->setURL(user.getFullPathName());
            
            initializeGallery();
            
            lastGalleryPath = user;
        }
    }
    
}

void BKAudioProcessor::loadGalleryFromXml(ScopedPointer<XmlElement> xml)
{
    if (xml != nullptr /*&& xml->hasTagName ("foobar")*/)
    {
        gallery = new Gallery(xml, *this);
        
        currentGallery = gallery->getName() + ".xml";
        
        initializeGallery();
        
        gallery->setGalleryDirty(false);
    }
}

void BKAudioProcessor::loadGalleryFromPath(String path)
{
    updateState->loadedJson = false;
    
    if (path == "")
    {
        String xmlData = CharPointer_UTF8 (BinaryData::Basic_Piano_xml);
        
        defaultLoaded = true;
        defaultName = "Basic_Piano_xml";
        
        loadGalleryFromXml(XmlDocument::parse(xmlData));
    }
    else
    {
        File myFile (path);

        ScopedPointer<XmlElement> xml (XmlDocument::parse (myFile));

        loadGalleryFromXml(xml);

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


void BKAudioProcessor::initializeGallery(void)
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
    }
    
    gallery->prepareToPlay(getSampleRate());
    
    updateUI();
    
    updateGalleries();

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
    else if (type == PreparationTypeTuningMod)
    {
        gallery->getTuningModification(Id)->reset();
    }
    else if (type == PreparationTypeTempoMod)
    {
        gallery->getTempoModification(Id)->reset();
    }
    
}



