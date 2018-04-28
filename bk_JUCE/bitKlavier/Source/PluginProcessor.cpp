
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


//==============================================================================
BKAudioProcessor::BKAudioProcessor(void):
firstTime(true),
updateState(new BKUpdateState()),
mainPianoSynth(),
hammerReleaseSynth(),
resonanceReleaseSynth(),
currentSampleType(BKLoadNil),
loader(*this)
#if TRY_UNDO
,epoch(0),
#endif
{
    didLoadHammersAndRes            = false;
    didLoadMainPianoSamples         = false;
    
#if TRY_UNDO
    history.ensureStorageAllocated(10);
#endif
    
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
    
    updateUI();
    
    String xmlData = CharPointer_UTF8 (BinaryData::Basic_Piano_xml);
    
    defaultLoaded = true;
    defaultName = "Basic_Piano_xml";
    
    loadGalleryFromXml(XmlDocument::parse(xmlData));

#if JUCE_IOS
    platform = BKIOS;
    lastGalleryPath = lastGalleryPath.getSpecialLocation(File::userDocumentsDirectory);
#endif
 

#if JUCE_MAC || JUCE_WINDOWS
    platform = BKOSX;
    lastGalleryPath = lastGalleryPath.getSpecialLocation(File::userDocumentsDirectory).getChildFile("bitKlavier resources").getChildFile("galleries");


#endif
    
    noteOn.ensureStorageAllocated(128);
    for(int i=0; i< 128; i++)
    {
        noteOn.set(i, false);
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
        "27. PianoMapGallery"
        
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

void BKAudioProcessor::loadSoundfontFromFile(File sfzFile)
{
    AudioFormatManager formatManager;
    formatManager.registerBasicFormats();
    
    String ext = sfzFile.getFileExtension();
    
    mainPianoSynth.clearVoices();
    mainPianoSynth.clearSounds();
    
    for (int i = 0; i < 300; ++i)
    {
        mainPianoSynth.addVoice(new BKPianoSamplerVoice(gallery->getGeneralSettings()));
    }
    
    ScopedPointer<sfzero::SF2Sound>     sf2sound;
    ScopedPointer<sfzero::SF2Reader>    sf2reader;
    ScopedPointer<sfzero::Sound>     sfzsound;
    ScopedPointer<sfzero::Reader>    sfzreader;
    
    if      (ext == ".sf2")
    {
        sf2sound   = new sfzero::SF2Sound(sfzFile);
        //sf2sound->useSubsound(0);
        
        sf2reader  = new sfzero::SF2Reader(sf2sound, sfzFile);
        
        sf2sound->loadRegions();
        sf2sound->loadSamples(&formatManager);
        
        regions.clear();
        regions = sf2sound->getRegions();
        DBG("regions.size: " + String(regions.size()));
    }
    else if (ext == ".sfz")
    {
        sfzsound   = new sfzero::Sound(sfzFile);
        
        sfzreader  = new sfzero::Reader(sfzsound);
        
        sfzsound->loadRegions();
        sfzsound->loadSamples(&formatManager);
        
        regions.clear();
        regions = sfzsound->getRegions();
        DBG("regions.size: " + String(regions.size()));
    }
    else    return;
    
    for (auto region : regions)
    {
        int64 sampleStart = region->loop_start;
        
        int64 sampleLength = region->loop_end - sampleStart;
        double sourceSampleRate = region->sample->getSampleRate();
        
        DBG("start: " + String(region->loop_start) + " end: " + String(region->loop_end) + " len: " + String(region->loop_end - region->loop_start));
        
        // check out fluidsynth as alternative
        AudioSampleBuffer* sourceBuffer = region->sample->getBuffer();
        
        BKReferenceCountedBuffer::Ptr buffer = new BKReferenceCountedBuffer(region->sample->getShortName(), 1, (int)sampleLength + 2*PAD);
        
        AudioSampleBuffer* destBuffer = buffer->getAudioSampleBuffer();
        
        destBuffer->copyFrom(0, 0, sourceBuffer->getReadPointer(0, sampleStart - PAD), (int)sampleLength+2*PAD);
        
        int nbits = region->hikey - region->lokey; nbits = (nbits > 0) ? nbits : 1;
        int vbits = region->hivel - region->lovel; vbits = (vbits > 0) ? vbits : 1;
        BigInteger nrange; nrange.setRange(region->lokey, nbits, true);
        BigInteger vrange; vrange.setRange(region->lovel, vbits, true);
        
        region->loop_end = (region->loop_end - region->loop_start) + PAD;
        region->loop_start = PAD;
        
        DBG("LOAD loop start: " + String(region->loop_start));
        DBG("LOAD loop   end: " + String(region->loop_end));

        DBG("len: " + String(sampleLength));
        mainPianoSynth.addSound(new BKPianoSamplerSound(region->sample->getShortName(),
                                                        buffer,
                                                        sampleLength,
                                                        sourceSampleRate,
                                                        nrange,
                                                        region->pitch_keycenter,
                                                        vrange,
                                                        region));
    }
    
    didLoadMainPianoSamples = true;
}

void BKAudioProcessor::openSoundfont(void)
{
    FileChooser myChooser ("Load soundfont file...",
                           //File::getSpecialLocation (File::userHomeDirectory),
                           lastGalleryPath,
                           "*.sf2;*.sfz");
    
    if (myChooser.browseForFileToOpen())
    {
        File sfzFile (myChooser.getResult());
        
        loadSoundfontFromFile(sfzFile);
    }
        
    
}

void BKAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    bkSampleRate = sampleRate;
    
    
    stk::Stk::setSampleRate(bkSampleRate);
    
    mainPianoSynth.setCurrentPlaybackSampleRate(sampleRate);
    hammerReleaseSynth.setCurrentPlaybackSampleRate(sampleRate);
    resonanceReleaseSynth.setCurrentPlaybackSampleRate(sampleRate);
    
    //mainPianoSynth.setGeneralSettings(gallery->getGeneralSettings());
    resonanceReleaseSynth.setGeneralSettings(gallery->getGeneralSettings());
    hammerReleaseSynth.setGeneralSettings(gallery->getGeneralSettings());
    
    levelBuf.setSize(2, 25);
    
    gallery->prepareToPlay(sampleRate);
    
#if JUCE_DEBUG
    //File file("~/soundfonts/elecanl/Elecanl.sf2");
    //loadSoundfontFromFile(file);
    
    loadPianoSamples(BKLoadLite);
#else
    
#if JUCE_IOS
    String osname = SystemStats::getOperatingSystemName();
    float iosVersion = osname.fromLastOccurrenceOf("iOS ", false, true).getFloatValue();
    
    String device = SystemStats::getDeviceDescription();
    
    if (device.contains("iPhone"))  updateState->needsExtraKeys = true;
    else                            updateState->needsExtraKeys = false;
    
    if (iosVersion <= 9.3)  loadPianoSamples(BKLoadLitest);
    else                    loadPianoSamples(BKLoadLite); // CHANGE BACK TO MEDIUM
#else
    loadPianoSamples(BKLoadHeavy); // CHANGE THIS BACK TO HEAVY
#endif
    
#endif

    
#if TRY_UNDO
    for (int i = 0; i < NUM_EPOCHS; i++)
    {
        history.set(i, currentPiano);
    }
#endif

}

BKAudioProcessor::~BKAudioProcessor()
{
    clipboard.clear();
}

void BKAudioProcessor::deleteGallery(void)
{
    File file(gallery->getURL());
    file.deleteFile();
    
    loadGalleryFromPath(firstGallery());
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
    }
    
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
    
#if JUCE_MAC || JUCE_WINDOWS
    bkGalleries = bkGalleries.getSpecialLocation(File::userDocumentsDirectory).getChildFile("bitKlavier resources").getChildFile("galleries");
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
        
        galleryDidLoad = true;
        
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
    
    if (allNotesOff)   allNotesOff = false;
    
    // Check PianoMap for whether piano should change due to key strike.
    int whichPiano = currentPiano->pianoMap[noteNumber];
    if (whichPiano > 0 && whichPiano != currentPiano->getId())
    {
        DBG("change piano to " + String(whichPiano));
        setCurrentPiano(whichPiano);
        
        if (sustainIsDown)
        {
            for (int p = currentPiano->activePMaps.size(); --p >= 0;)
                currentPiano->activePMaps[p]->sustainPedalPressed();
        }
        
    }
    
    // modifications
    performResets(noteNumber);
    performModifications(noteNumber);
    
    //tempo
    
    // Send key on to each pmap in current piano
    for (p = currentPiano->activePMaps.size(); --p >= 0;) {
        //DBG("noteon: " +String(noteNumber) + " pmap: " + String(p));
        currentPiano->activePMaps[p]->keyPressed(noteNumber, velocity, channel);
    }
}

void BKAudioProcessor::handleNoteOff(int noteNumber, float velocity, int channel)
{
    int p, pm;
    
    noteOn.set(noteNumber, false);
    //DBG("noteoff velocity = " + String(velocity));
    if(velocity <= 0) velocity = 0.7; //for keyboards that don't do proper noteOff messages
    
    // Send key off to each pmap in current piano
    for (p = currentPiano->activePMaps.size(); --p >= 0;)
        currentPiano->activePMaps[p]->keyReleased(noteNumber, velocity, channel);
    
    // This is to make sure note offs are sent to Direct and Nostalgic processors from previous pianos with holdover notes.
    if (prevPiano != currentPiano)
    {
        for (p = prevPianos.size(); --p >= 0;) {
            for (pm = prevPianos[p]->activePMaps.size(); --pm >= 0;) {
                prevPianos[p]->activePMaps[pm]->postRelease(noteNumber, velocity, channel);
            }
        }
    }
    
    --noteOnCount;
    
}

void BKAudioProcessor::sustainActivate(void)
{
    if(!sustainIsDown)
    {
        sustainIsDown = true;
        DBG("SUSTAIN ON");
        
        for (int p = currentPiano->activePMaps.size(); --p >= 0;)
            currentPiano->activePMaps[p]->sustainPedalPressed();
    }
    
}

void BKAudioProcessor::sustainDeactivate(void)
{
    
    if(sustainIsDown)
    {
        sustainIsDown = false;
        DBG("SUSTAIN OFF");
        
        for (int p = currentPiano->activePMaps.size(); --p >= 0;)
            currentPiano->activePMaps[p]->sustainPedalReleased();
        
        if(prevPiano != currentPiano)
        {
            for (int p = prevPiano->activePMaps.size(); --p >= 0;)
                prevPiano->activePMaps[p]->sustainPedalReleased(true);
        }
    }
}


void BKAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    buffer.clear();
    
    if (!didLoadMainPianoSamples) return;
    
    int time;
    MidiMessage m;
    
    int numSamples = buffer.getNumSamples();
    if(numSamples != levelBuf.getNumSamples()) levelBuf.setSize(buffer.getNumChannels(), numSamples);
    
    // Process all active prep maps in current piano
    for (auto pmap : currentPiano->activePMaps)
        pmap->processBlock(numSamples, m.getChannel(), false);
    
    // OLAGON: Process all active nostalgic preps in previous piano
    if(prevPiano != currentPiano)
    {
        for (auto pmap : prevPiano->activePMaps)
            pmap->processBlock(numSamples, m.getChannel(), true); // true for onlyNostalgic
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
    
    for (MidiBuffer::Iterator i (midiMessages); i.getNextEvent (m, time);)
    {
        int noteNumber = m.getNoteNumber();
        //DBG("note: " + String(noteNumber) + " " + String(m.getVelocity()));
        float velocity = m.getFloatVelocity();
         
        channel = m.getChannel();
        
        if (m.isNoteOn())
        {
            handleNoteOn(noteNumber, velocity, channel);
        }
        else if (m.isNoteOff())
        {
            handleNoteOff(noteNumber, velocity, channel);
        }
        
        if (m.isSustainPedalOn())
        {
            if (sustainInverted)    sustainDeactivate();
            else                    sustainActivate();
               
        }
        else if (m.isSustainPedalOff())
        {
            if (sustainInverted)    sustainActivate();
            else                    sustainDeactivate();
        }
    }
    
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
    
    gallery->resetPreparations();
    
    if (noteOnCount)  prevPianos.addIfNotAlreadyThere(currentPiano);
    
    prevPiano = currentPiano;
    
    currentPiano = gallery->getPiano(which);
    
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
    // NEEDS OPTIMIZATION
    Array<float> modfa;
    Array<Array<float>> modafa;
    Array<int> modia;
    float modf;
    int   modi;
    bool  modb;
    
    TuningModification::PtrArr tMod = currentPiano->modificationMap[noteNumber]->getTuningModifications();
    for (int i = tMod.size(); --i >= 0;)
    {
        TuningPreparation::Ptr active = gallery->getTuning(tMod[i]->getPrepId())->aPrep;
        TuningParameterType type = tMod[i]->getParameterType();
        modf = tMod[i]->getModFloat();
        modi = tMod[i]->getModInt();
        modb = tMod[i]->getModBool();
        modfa = tMod[i]->getModFloatArr();
        modia = tMod[i]->getModIntArr();
        
        if (type == TuningScale)                    active->setTuning((TuningSystem)modi);
        else if (type == TuningFundamental)         active->setFundamental((PitchClass)modi);
        else if (type == TuningOffset)              active->setFundamentalOffset(modf);
        else if (type == TuningA1IntervalScale)     active->setAdaptiveIntervalScale((TuningSystem)modi);
        else if (type == TuningA1Inversional)       active->setAdaptiveInversional(modb);
        else if (type == TuningA1AnchorScale)       active->setAdaptiveAnchorScale((TuningSystem)modi);
        else if (type == TuningA1ClusterThresh)     active->setAdaptiveClusterThresh(modi);
        else if (type == TuningA1AnchorFundamental) active->setAdaptiveAnchorFundamental((PitchClass) modi);
        else if (type == TuningA1History)           active->setAdaptiveHistory(modi);
        else if (type == TuningNToneRootCB)         active->setNToneRoot(modi);
        else if (type == TuningNToneRootOctaveCB)   active->setNToneRootOctave(modi);
        else if (type == TuningNToneSemitoneWidth)  active->setNToneSemitoneWidth(modi);
        else if (type == TuningCustomScale)
        {
            active->setTuning(CustomTuning);
            active->setCustomScaleCents(modfa);
        }
        else if (type == TuningAbsoluteOffsets)
        {
            for(int i = 0; i< modfa.size(); i+=2) {
                active->setAbsoluteOffset(modfa[i], modfa[i+1] * .01);
            }
        }
        
        
        updateState->tuningPreparationDidChange = true;
    }
    
    TempoModification::PtrArr mMod = currentPiano->modificationMap[noteNumber]->getTempoModifications();
    for (int i = mMod.size(); --i >= 0;)
    {
        TempoPreparation::Ptr active = gallery->getTempo(mMod[i]->getPrepId())->aPrep;
        TempoParameterType type = mMod[i]->getParameterType();
        modfa = mMod[i]->getModFloatArr();
        modf = mMod[i]->getModFloat();
        modi = mMod[i]->getModInt();
        modia = mMod[i]->getModIntArr();
        
        if (type == TempoBPM)               active->setTempo(modf);
        else if (type == TempoSystem)       active->setTempoSystem((TempoType)modi);
        else if (type == AT1History)        active->setAdaptiveTempo1History(modi);
        else if (type == AT1Subdivisions)   active->setAdaptiveTempo1Subdivisions(modf);
        else if (type == AT1Min)            active->setAdaptiveTempo1Min(modf);
        else if (type == AT1Max)            active->setAdaptiveTempo1Max(modf);
        else if (type == AT1Mode)           active->setAdaptiveTempo1Mode((AdaptiveTempo1Mode)modi);

        updateState->tempoPreparationDidChange = true;
    }
    
    DirectModification::PtrArr dMod = currentPiano->modificationMap[noteNumber]->getDirectModifications();
    for (int i = dMod.size(); --i >= 0;)
    {
        DirectPreparation::Ptr active = gallery->getDirect(dMod[i]->getPrepId())->aPrep;
        DirectParameterType type = dMod[i]->getParameterType();
        modfa = dMod[i]->getModFloatArr();
        modf = dMod[i]->getModFloat();
        modi = dMod[i]->getModInt();
        modia = dMod[i]->getModIntArr();
        
        if (type == DirectTransposition)    active->setTransposition(modfa);
        else if (type == DirectGain)        active->setGain(modf);
        else if (type == DirectHammerGain)  active->setHammerGain(modf);
        else if (type == DirectResGain)     active->setResonanceGain(modf);
        else if (type == DirectADSR)        active->setADSRvals(modfa);
        
        updateState->directPreparationDidChange = true;
    }
    
    NostalgicModification::PtrArr nMod = currentPiano->modificationMap[noteNumber]->getNostalgicModifications();
    for (int i = nMod.size(); --i >= 0;)
    {
        NostalgicPreparation::Ptr active = gallery->getNostalgic(nMod[i]->getPrepId())->aPrep;
        NostalgicParameterType type = nMod[i]->getParameterType();
        modfa = nMod[i]->getModFloatArr();
        modf = nMod[i]->getModFloat();
        modi = nMod[i]->getModInt();
        modia = nMod[i]->getModIntArr();
        
        if (type == NostalgicTransposition)         active->setTransposition(modfa);
        else if (type == NostalgicGain)             active->setGain(modf);
        else if (type == NostalgicMode)             active->setMode((NostalgicSyncMode)modi);
        else if (type == NostalgicUndertow)         active->setUndertow(modi);
        else if (type == NostalgicBeatsToSkip)      active->setBeatsToSkip(modi);
        else if (type == NostalgicWaveDistance)     active->setWaveDistance(modi);
        else if (type == NostalgicLengthMultiplier) active->setLengthMultiplier(modf);
        else if (type == NostalgicReverseADSR)      active->setReverseADSRvals(modfa);
        else if (type == NostalgicUndertowADSR)     active->setUndertowADSRvals(modfa);
        
        updateState->nostalgicPreparationDidChange = true;
    }
    
    SynchronicModification::PtrArr sMod = currentPiano->modificationMap[noteNumber]->getSynchronicModifications();
    for (int i = sMod.size(); --i >= 0;)
    {
        SynchronicPreparation::Ptr active = gallery->getSynchronic(sMod[i]->getPrepId())->aPrep;
        SynchronicParameterType type = sMod[i]->getParameterType();
        modf = sMod[i]->getModFloat();
        modi = sMod[i]->getModInt();
        modfa = sMod[i]->getModFloatArr();
        modafa = sMod[i]->getModArrFloatArr();
        modia = sMod[i]->getModIntArr();
        
        DBG("transps: " + arrayFloatArrayToString(modafa));
        
        if (type == SynchronicTranspOffsets)            active->setTransposition(modafa);
        else if (type == SynchronicMode)                active->setMode((SynchronicSyncMode)modi);
        else if (type == SynchronicClusterMin)          active->setClusterMin(modi);
        else if (type == SynchronicClusterMax)          active->setClusterMax(modi);
        else if (type == SynchronicClusterThresh)       active->setClusterThresh(modi);
        else if (type == SynchronicNumPulses )          active->setNumBeats(modi);
        else if (type == SynchronicBeatsToSkip)         active->setBeatsToSkip(modi);
        else if (type == SynchronicBeatMultipliers)     active->setBeatMultipliers(modfa);
        else if (type == SynchronicLengthMultipliers)   active->setLengthMultipliers(modfa);
        else if (type == SynchronicAccentMultipliers)   active->setAccentMultipliers(modfa);
        else if (type == SynchronicADSRs)               active->setADSRs(modafa);
        
        updateState->synchronicPreparationDidChange = true;
    }
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
                             
                             ScopedPointer<InputStream> wi (url.createInputStream (false));
                             
                             if (wi != nullptr)
                             {
                                 MemoryBlock block(wi->getTotalLength());
                                 wi->readIntoMemoryBlock(block);
                                 
                                 XmlDocument doc(block.toString());
                                 ScopedPointer<XmlElement> xml = doc.getDocumentElement();
                                 
                                 DBG("url file name: " + url.getFileName().replace("%20", " "));
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
    
    galleryDidLoad = true;
    
}



void BKAudioProcessor::saveCurrentGallery(void)
{
    if (gallery->getURL() == "")
    {
#if JUCE_IOS
        writeCurrentGalleryToURL( File::getSpecialLocation(File::userDocumentsDirectory).getFullPathName() + "/" + gallery->getName());
#else
        writeCurrentGalleryToURL( File::getSpecialLocation(File::userDocumentsDirectory).getFullPathName() + "/bitKlavier resources/galleries/" + gallery->getName());
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

        File user   (File::getSpecialLocation(File::userDocumentsDirectory));
        user = user.getChildFile("bitKlavier resources/galleries/");

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
            
            galleryDidLoad = true;
            
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
        
        galleryDidLoad = true;
        
        gallery->setGalleryDirty(false);
    }
}

void BKAudioProcessor::loadGalleryFromPath(String path)
{
    updateState->loadedJson = false;
    
    File myFile (path);
    
    ScopedPointer<XmlElement> xml (XmlDocument::parse (myFile));
    
    loadGalleryFromXml(xml);
    
    gallery->setURL(path);
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
        
        File user   (File::getSpecialLocation(File::userDocumentsDirectory));
        user = user.getChildFile("bitKlavier resources/galleries/");
        
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
        
        galleryDidLoad = true;
        
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
    
    galleryDidLoad = true;
    
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
    
    gallery->prepareToPlay(bkSampleRate);
    
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
        gallery->getDirectModPreparation(Id)->clearAll();
    }
    else if (type == PreparationTypeNostalgicMod)
    {
        gallery->getNostalgicModPreparation(Id)->clearAll();
    }
    else if (type == PreparationTypeSynchronicMod)
    {
        gallery->getSynchronicModPreparation(Id)->clearAll();
    }
    else if (type == PreparationTypeTuningMod)
    {
        gallery->getTuningModPreparation(Id)->clearAll();
    }
    else if (type == PreparationTypeTempoMod)
    {
        gallery->getTempoModPreparation(Id)->clearAll();
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
        gallery->getDirectModPreparation(Id)->clearAll();
    }
    else if (type == PreparationTypeNostalgicMod)
    {
        gallery->getNostalgicModPreparation(Id)->clearAll();
    }
    else if (type == PreparationTypeSynchronicMod)
    {
        gallery->getSynchronicModPreparation(Id)->clearAll();
    }
    else if (type == PreparationTypeTuningMod)
    {
        gallery->getTuningModPreparation(Id)->clearAll();
    }
    else if (type == PreparationTypeTempoMod)
    {
        gallery->getTempoModPreparation(Id)->clearAll();
    }
    
}

#if TRY_UNDO
void BKAudioProcessor::updateHistory(void)
{
    for (int i = 0; i < NUM_EPOCHS; i++)
    {
        history.set(i+1, history.getUnchecked(i));
    }
    
    history.set(0, currentPiano->duplicate(true));
}

void BKAudioProcessor::timeTravel(bool forward)
{
    if (forward)    epoch--;
    else            epoch++;
    
    if (epoch >= NUM_EPOCHS) epoch = NUM_EPOCHS-1;
    if (epoch <  0 ) epoch = 0;
    
    Piano::Ptr newPiano = history.getUnchecked(epoch);
    
    if (newPiano != nullptr) currentPiano = newPiano;
    
    updateState->pianoDidChangeForGraph = true;
}
#endif



