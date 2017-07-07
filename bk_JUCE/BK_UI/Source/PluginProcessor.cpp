
#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "BKPianoSampler.h"

#include "Reset.h"

#define NOST_KEY_OFF 1

//==============================================================================
BKAudioProcessor::BKAudioProcessor():
updateState(new BKUpdateState()),
mainPianoSynth(),
hammerReleaseSynth(),
resonanceReleaseSynth()
{
    didLoadHammersAndRes            = false;
    didLoadMainPianoSamples         = false;
    
    collectGalleries();
    
    updateUI();
    
    loadGalleryFromPath(galleryNames[0]);
    
    noteOn.ensureStorageAllocated(128);
    for(int i=0; i< 128; i++)
    {
        noteOn.set(i, false);
    }
    
    
    
}

void BKAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    bkSampleRate = sampleRate;
    
    mainPianoSynth.setCurrentPlaybackSampleRate(sampleRate);
    hammerReleaseSynth.setCurrentPlaybackSampleRate(sampleRate);
    resonanceReleaseSynth.setCurrentPlaybackSampleRate(sampleRate);
    
    mainPianoSynth.setGeneralSettings(gallery->getGeneralSettings());
    resonanceReleaseSynth.setGeneralSettings(gallery->getGeneralSettings());
    hammerReleaseSynth.setGeneralSettings(gallery->getGeneralSettings());
    
    levelBuf.setSize(2, 25);
    
    gallery->prepareToPlay(sampleRate);
    
    loadPianoSamples(BKLoadLite);
    
    
}

BKAudioProcessor::~BKAudioProcessor()
{
    
}

void BKAudioProcessor::handleNoteOn(int noteNumber, float velocity, int channel)  
{
    int p;
    
    ++noteOnCount;
    noteOn.set(noteNumber, true);
    
    if (allNotesOff)   allNotesOff = false;
    
    // Check PianoMap for whether piano should change due to key strike.
    int whichPiano = currentPiano->pianoMap[noteNumber] - 1;
    if (whichPiano >= 0 && whichPiano != currentPiano->getId())
    {
        DBG("change piano to " + String(whichPiano));
        setCurrentPiano(whichPiano);
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
    DBG("noteoff velocity = " + String(velocity));
    
    // Send key off to each pmap in current piano
    for (p = currentPiano->activePMaps.size(); --p >= 0;)
        currentPiano->activePMaps[p]->keyReleased(noteNumber, velocity, channel);
    
    // This is to make sure note offs are sent to Direct and Nostalgic processors from previous pianos with holdover notes.
    for (p = prevPianos.size(); --p >= 0;) {
        for (pm = prevPianos[p]->activePMaps.size(); --pm >= 0;) {
            prevPianos[p]->activePMaps[pm]->postRelease(noteNumber, velocity, channel);
        }
    }
    
    --noteOnCount;
    
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
    for (int p = currentPiano->activePMaps.size(); --p >= 0;)
        currentPiano->activePMaps[p]->processBlock(numSamples, m.getChannel());
    
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
        DBG("note: " + String(noteNumber) + " " + String(m.getVelocity()));
        float velocity = m.getFloatVelocity();
         
        channel = m.getChannel();
        
        if (m.isNoteOn())
        {
            /*
            ++noteOnCount;
            noteOn.set(noteNumber, true);
            
            if (allNotesOff)   allNotesOff = false;
            
            // Check PianoMap for whether piano should change due to key strike.
            int whichPiano = currentPiano->pianoMap[noteNumber] - 1;
            if (whichPiano >= 0 && whichPiano != currentPiano->getId())
            {
                DBG("change piano to " + String(whichPiano));
                setCurrentPiano(whichPiano);
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
             */
            handleNoteOn(noteNumber, velocity, channel);
        }
        else if (m.isNoteOff())
        {
            /*
            noteOn.set(noteNumber, false);
            
            // Send key off to each pmap in current piano
            for (p = currentPiano->activePMaps.size(); --p >= 0;)
                currentPiano->activePMaps[p]->keyReleased(noteNumber, velocity, channel);
            
            // This is to make sure note offs are sent to Direct and Nostalgic processors from previous pianos with holdover notes.
            for (p = prevPianos.size(); --p >= 0;) {
                for (pm = prevPianos[p]->activePMaps.size(); --pm >= 0;) {
                    prevPianos[p]->activePMaps[pm]->postRelease(noteNumber, velocity, channel);
                }
            }
            
            --noteOnCount;
             */
            handleNoteOff(noteNumber, velocity, channel);
            
        }
        else if (m.isController())
        {
            int controller = m.getControllerNumber();
            int piano = controller-51;
            
            if ((m.getControllerValue() != 0) && piano >= 0 && piano < 5)   setCurrentPiano(piano);
        }
        
        if (m.isSustainPedalOn())
        {
            DBG("sustain pedal depressed");
            for (int p = currentPiano->activePMaps.size(); --p >= 0;)
                currentPiano->activePMaps[p]->sustainPedalPressed();
            
        }
        else if (m.isSustainPedalOff())
        {
            DBG("sustain pedal released");
            for (int p = currentPiano->activePMaps.size(); --p >= 0;)
                currentPiano->activePMaps[p]->sustainPedalReleased();
        }
    }
    
    // Sets some flags to determine whether to send noteoffs to previous pianos.
    if (!allNotesOff && !noteOnCount) {
        prevPianos.clearQuick();
        allNotesOff = true;
    }

    mainPianoSynth.renderNextBlock(buffer,midiMessages,0, numSamples);
    hammerReleaseSynth.renderNextBlock(buffer,midiMessages,0, numSamples);
    resonanceReleaseSynth.renderNextBlock(buffer,midiMessages,0, numSamples);
    
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
    gallery->resetPreparations();
    
    if (noteOnCount)  prevPianos.addIfNotAlreadyThere(currentPiano);
    
    prevPiano = currentPiano;
    
    currentPiano = gallery->getPiano(which);
    
    updateState->pianoDidChange = true;
    updateState->pianoDidChangeForGraph = true;
    updateState->synchronicPreparationDidChange = true;
    updateState->nostalgicPreparationDidChange = true;
    updateState->directPreparationDidChange = true;
    updateState->tempoPreparationDidChange = true;
    updateState->tuningPreparationDidChange = true;
}


// Reset
void BKAudioProcessor::performResets(int noteNumber)
{
    for (auto prep : currentPiano->modificationMap.getUnchecked(noteNumber)->directReset)      gallery->getDirect(prep)->reset();
    for (auto prep : currentPiano->modificationMap.getUnchecked(noteNumber)->synchronicReset)  gallery->getSynchronic(prep)->reset();
    for (auto prep : currentPiano->modificationMap.getUnchecked(noteNumber)->nostalgicReset)   gallery->getNostalgic(prep)->reset();
    for (auto prep : currentPiano->modificationMap.getUnchecked(noteNumber)->tuningReset)      gallery->getTuning(prep)->reset();
    for (auto prep : currentPiano->modificationMap.getUnchecked(noteNumber)->tempoReset)       gallery->getTempo(prep)->reset();
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
        else if (type == TuningCustomScale)         active->setCustomScale(modfa);
        else if (type == TuningAbsoluteOffsets)
        {
            for(int i = 0; i< modfa.size(); i+=2) {
                //DBG("modfa AbsoluteOffsets val = " + String(modfa[i]) + " " + String(modfa[i+1]));
                active->setAbsoluteOffset(modfa[i], modfa[i+1] * .01);
            }
        }
        
        
        updateState->tuningPreparationDidChange = true;
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
        else if (type == DirectTuning)      active->setTuning(gallery->getTuning(modi));
        
        
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
        else if (type == NostalgicSyncTarget)       active->setSyncTarget(modi);
        else if (type == NostalgicBeatsToSkip)      active->setBeatsToSkip(modf);
        else if (type == NostalgicWaveDistance)     active->setWaveDistance(modi);
        else if (type == NostalgicLengthMultiplier) active->setLengthMultiplier(modf);
        else if (type == NostalgicTuning)           active->setTuning(gallery->getTuning(modi));
        
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
        
        if (type == SynchronicTranspOffsets)            active->setTransposition(modafa);
        else if (type == SynchronicTempo)               active->setTempoControl(gallery->getTempo(modi));
        else if (type == SynchronicMode)                active->setMode((SynchronicSyncMode)modi);
        else if (type == SynchronicClusterMin)          active->setClusterMin(modi);
        else if (type == SynchronicClusterMax)          active->setClusterMax(modi);
        else if (type == SynchronicClusterThresh)       active->setClusterThresh(modi);
        else if (type == SynchronicNumPulses )          active->setNumBeats(modi);
        else if (type == SynchronicBeatsToSkip)         active->setBeatsToSkip(modi);
        else if (type == SynchronicBeatMultipliers)     active->setBeatMultipliers(modfa);
        else if (type == SynchronicLengthMultipliers)   active->setLengthMultipliers(modfa);
        else if (type == SynchronicAccentMultipliers)   active->setAccentMultipliers(modfa);
        
        updateState->synchronicPreparationDidChange = true;
    }
}

void BKAudioProcessor::saveGalleryAs(void)
{
    FileChooser myChooser ("Save gallery to file...",
                           File::getSpecialLocation (File::userHomeDirectory),
                           "*.xml");
    
    if (myChooser.browseForFileToSave(true))
    {
        File myFile (myChooser.getResult());
        currentGallery = myFile.getFileName();
        
        String currentURL = gallery->getURL();
        String newURL = myFile.getFullPathName();
        
        DBG("newURL: " + newURL);
        
        if (currentURL != newURL)   gallery->setURL(newURL);
        
        ValueTree galleryVT = gallery->getState();
        
        ScopedPointer<XmlElement> myXML = galleryVT.createXml();
        
        myXML->writeToFile(myFile, String::empty);
    }
    
    
    updateGalleries();
    
    galleryDidLoad = true;
    
}

void BKAudioProcessor::saveGallery(void)
{
    String currentURL = gallery->getURL();

    if (currentURL == String::empty)
    {
        saveGalleryAs();
        return;
    }
    else
    {
        File myFile (currentURL);
        
        ValueTree galleryVT = gallery->getState();
        
        ScopedPointer<XmlElement> myXML = galleryVT.createXml();
        
        myXML->writeToFile(myFile, String::empty);
    }
}


// Gallery/Preset Management
void BKAudioProcessor::loadGalleryDialog(void)
{
    FileChooser myChooser ("Load gallery from xml file...",
                           File::getSpecialLocation (File::userHomeDirectory),
                           "*.xml");
    
    if (myChooser.browseForFileToOpen())
    {
        File myFile (myChooser.getResult());
        
        ScopedPointer<XmlElement> xml (XmlDocument::parse (myFile));
        
        if (xml != nullptr /*&& xml->hasTagName ("foobar")*/)
        {
            currentGallery = myFile.getFileName();
            
            gallery = new Gallery(xml, &mainPianoSynth, &resonanceReleaseSynth, &hammerReleaseSynth, updateState);
            
            initializeGallery();
        }
    }
    
}

void BKAudioProcessor::loadGalleryFromPath(String path)
{
    File myFile (path);
    
    ScopedPointer<XmlElement> xml (XmlDocument::parse (myFile));
    
    if (xml != nullptr /*&& xml->hasTagName ("foobar")*/)
    {
        currentGallery = myFile.getFileName();
        
        gallery = new Gallery(xml, &mainPianoSynth, &resonanceReleaseSynth, &hammerReleaseSynth, updateState);
        
        initializeGallery();
    }
}

void BKAudioProcessor::loadJsonGalleryDialog(void)
{
    FileChooser myChooser ("Load gallery from json file...",
                           File::getSpecialLocation (File::userHomeDirectory),
                           "*.json");
    
    
    if (myChooser.browseForFileToOpen())
    {
        File myFile (myChooser.getResult());
        
        currentGallery = myFile.getFileName();
        
        var myJson = JSON::parse(myFile);
        
        gallery = new Gallery(myJson, &mainPianoSynth, &resonanceReleaseSynth, &hammerReleaseSynth, updateState);
        
        initializeGallery();
    }
    
}

void BKAudioProcessor::initializeGallery(void)
{
    gallery->prepareToPlay(bkSampleRate);
    
    prevPiano = gallery->getPiano(0);
    currentPiano = gallery->getPiano(0);
    
    updateUI();
    
    updateGalleries();
}

void BKAudioProcessor::loadJsonGalleryFromPath(String path)
{
    File myFile (path);
    
    currentGallery = myFile.getFileName();
    
    var myJson = JSON::parse(myFile);
    
    gallery = new Gallery(myJson, &mainPianoSynth, &resonanceReleaseSynth, &hammerReleaseSynth, updateState);
    
    initializeGallery();
    
}



