/*
  ==============================================================================

	Blendronic.cpp
	Created: 11 Jun 2019 2:00:53pm
	Author:  Theodore R Trevisan

  ==============================================================================
*/

#include "Blendronic.h"

#include "BKSynthesiser.h"

//copy constructor
BlendronicPreparation::BlendronicPreparation(BlendronicPreparation::Ptr p) :
	name(p->getName()),
	bBeats(p->getBeats()),
    bDelayLengths(p->getDelayLengths()),
	bSmoothValues(p->getSmoothValues()),
	bFeedbackCoefficients(p->getFeedbackCoefficients()),
	bDelayMax(p->getDelayMax()),
//    bSmoothMode(p->getSmoothMode()),
//    bSyncMode(p->getSyncMode()),
//    bClearMode(p->getClearMode()),
//    bOpenMode(p->getOpenMode()),
//    bCloseMode(p->getCloseMode()),
    bSmoothBase(BlendronicSmoothPulse),
    bSmoothScale(BlendronicSmoothFull),
    targetTypeBlendronicSync(p->getTargetTypeBlendronicSync()),
    targetTypeBlendronicPatternSync(p->getTargetTypeBlendronicPatternSync()),
    targetTypeBlendronicClear(p->getTargetTypeBlendronicClear()),
    targetTypeBlendronicPausePlay(p->getTargetTypeBlendronicPausePlay()),
    targetTypeBlendronicOpenCloseInput(p->getTargetTypeBlendronicOpenCloseInput()),
    targetTypeBlendronicOpenCloseOutput(p->getTargetTypeBlendronicOpenCloseOutput()),
	bInputThresh(p->getInputThreshMS()),
	bInputThreshSec(p->getInputThreshSEC()),
	holdMin(p->getHoldMin()),
	holdMax(p->getHoldMax()),
	velocityMin(p->getVelocityMin()),
	velocityMax(p->getVelocityMax()),
    bClusterThresh(p->getClusterThreshMS()),
    bClusterThreshSec(p->getClusterThreshSEC())
{
}

//constructor with input
BlendronicPreparation::BlendronicPreparation(String newName,
                                             Array<float> beats,
                                             Array<float> delayLengths,
                                             Array<float> smoothValues,
                                             Array<float> feedbackCoefficients,
                                             float clusterThresh,
//                                             BlendronicSmoothMode smoothMode,
//                                             BlendronicSyncMode syncMode,
//                                             BlendronicClearMode clearMode,
//                                             BlendronicOpenMode openMode,
//                                             BlendronicCloseMode closeMode,
                                             float delayMax) :
	name(newName),
	bBeats(beats),
    bDelayLengths(delayLengths),
	bSmoothValues(smoothValues),
	bFeedbackCoefficients(feedbackCoefficients),
	bDelayMax(delayMax),
//    bSmoothMode(smoothMode),
//    bSyncMode(syncMode),
//    bClearMode(clearMode),
//    bOpenMode(openMode),
//    bCloseMode(closeMode),
    bSmoothBase(BlendronicSmoothPulse),
    bSmoothScale(BlendronicSmoothFull),
    targetTypeBlendronicSync(NoteOn),
    targetTypeBlendronicPatternSync(NoteOn),
    targetTypeBlendronicClear(NoteOn),
    targetTypeBlendronicPausePlay(NoteOn),
    targetTypeBlendronicOpenCloseInput(NoteOn),
    targetTypeBlendronicOpenCloseOutput(NoteOn),
	bInputThresh(1),
	bInputThreshSec(0.001),
	holdMin(0),
	holdMax(12000),
	velocityMin(0),
	velocityMax(127),
    bClusterThresh(clusterThresh),
    bClusterThreshSec(.001 * bClusterThresh)
{
}

//empty constructor
BlendronicPreparation::BlendronicPreparation(void) :
	name("blank blendronic"),
	bBeats(Array<float>({ 4., 3., 2., 3.})),
    bDelayLengths(Array<float>({ 4., 3., 2., 3.})),
	bSmoothValues(Array<float>({ 0.1 })),
	bFeedbackCoefficients(Array<float>({ 0.95 })),
	bDelayMax(44100. * 5.),
//    bSmoothMode(ConstantTimeSmooth),
//    bSyncMode(BlendronicFirstNoteOnSync),
//    bClearMode(BlendronicFirstNoteOnClear),
//    bOpenMode(BlendronicOpenModeNil),
//    bCloseMode(BlendronicCloseModeNil),
    bSmoothBase(BlendronicSmoothPulse),
    bSmoothScale(BlendronicSmoothFull),
    targetTypeBlendronicSync(NoteOn),
    targetTypeBlendronicPatternSync(NoteOn),
    targetTypeBlendronicClear(NoteOn),
    targetTypeBlendronicPausePlay(NoteOn),
    targetTypeBlendronicOpenCloseInput(NoteOn),
    targetTypeBlendronicOpenCloseOutput(NoteOn),
	bInputThresh(1),
	bInputThreshSec(0.001),
	holdMin(0),
	holdMax(12000),
	velocityMin(0),
	velocityMax(127),
    bClusterThresh(500),
    bClusterThreshSec(.001 * bClusterThresh)
{
}

//copy
void BlendronicPreparation::copy(BlendronicPreparation::Ptr b)
{
}

//maps modification to values
void BlendronicPreparation::performModification(BlendronicPreparation::Ptr b, Array<bool> dirty)
{
}

//compares two blendronics
bool BlendronicPreparation::compare(BlendronicPreparation::Ptr b)
{
	//will do later
	return false;
}

////////////////////////////////////////////////////////////////////////////////////
///////////////////////// BLENDRONIC PROCESSOR /////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

BlendronicProcessor::BlendronicProcessor(Blendronic::Ptr bBlendronic,
	TempoProcessor::Ptr bTempo, GeneralSettings::Ptr bGeneral, BKSynthesiser* bMain):
	blendronic(bBlendronic),
    synth(bMain),
	tempo(bTempo),
	general(bGeneral),
    keymaps(Keymap::PtrArr()),
    blendronicActive(true),
	sampleTimer(0),
	beatIndex(0),
    delayIndex(0),
	smoothIndex(0),
	feedbackIndex(0)
{
    velocities.ensureStorageAllocated(128);
    holdTimers.ensureStorageAllocated(128);
    for (int i = 0; i < 128; i++)
    {
        velocities.insert(i, 0.);
        holdTimers.insert(i, 0);
    }
    
    inSyncCluster = false;
    inClearCluster = false;
    inOpenCluster = false;
    inCloseCluster = false;
    
    keysDepressed = Array<int>();
//    syncKeysDepressed = Array<int>();
//    clearKeysDepressed = Array<int>();
//    openKeysDepressed = Array<int>();
//    closeKeysDepressed = Array<int>();
    
    delayLengthRecord.ensureStorageAllocated(blendronic->aPrep->getDelayMax());
    for (int i = 0; i < blendronic->aPrep->getDelayMax(); i++)
    {
        delayLengthRecord.add(0.0f);
    }
    delayLengthRecordInPoint = 0;
    
    DBG("Create bproc");
}

BlendronicProcessor::~BlendronicProcessor()
{
    DBG("Destroy bproc");
}

void BlendronicProcessor::tick(float* outputs)
{
    BlendronicPreparation::Ptr prep = blendronic->aPrep;
    TempoPreparation::Ptr tempoPrep = tempo->getTempo()->aPrep;
    
    if (!blendronicActive) return;
    
    // Check for beat change
    if (sampleTimer >= numSamplesBeat)
    {
        // Clear if we need to
        if (clearDelayOnNextBeat)
        {
            delay->clear();
            clearDelayOnNextBeat = false;
        }
        
        // Update the pulse length in case tempo or subdiv changed
        pulseLength = (60.0 / (tempoPrep->getSubdivisions() * tempoPrep->getTempo()));
        
        // Calculate the length of the entire beat pattern
        float beatPatternLength = 0.0;
        for (auto b : prep->getBeats()) beatPatternLength += b * pulseLength * sampleRate;
        
        ///
        if (numBeatPositions != (int) ((delay->getDelayBuffer().getNumSamples() / beatPatternLength) * prep->getBeats().size()) - 1)
        {
            beatPositionsInBuffer.clear();
            beatPositionsIndex = -1;
            pulseOffset = delay->getCurrentSample();
            numBeatPositions = ((delay->getDelayBuffer().getNumSamples() / beatPatternLength) * prep->getBeats().size()) - 1;
        }
        
        // Set the next beat position, cycle if we've reached the max number of positions for the buffer
        beatPositionsInBuffer.set(++beatPositionsIndex, delay->getCurrentSample());
        if (beatPositionsIndex >= numBeatPositions) beatPositionsIndex = -1;
   
        // Step sequenced params
        beatIndex++;
        if (beatIndex >= prep->getBeats().size()) beatIndex = 0;
        delayIndex++;
        if (delayIndex >= prep->getDelayLengths().size()) delayIndex = 0;
        smoothIndex++;
        if (smoothIndex >= prep->getSmoothValues().size()) smoothIndex = 0;
        feedbackIndex++;
        if (feedbackIndex >= prep->getFeedbackCoefficients().size()) feedbackIndex = 0;
        
        // Set parameters of the delay object
        updateDelayParameters();
        
        // Update numSamplesBeat for the new beat and reset sampleTimer
        numSamplesBeat = prep->getBeats()[beatIndex] * pulseLength * sampleRate;
        sampleTimer = 0;
    }
    sampleTimer++;
    
    // Tick the delay
    delay->tick(outputs);
    
    delayLengthRecord.set(delayLengthRecordInPoint++, delay->getDelayLength()/(pulseLength*sampleRate));
    if (delayLengthRecordInPoint >= delayLengthRecord.size()) delayLengthRecordInPoint = 0;
}

void BlendronicProcessor::processBlock(int numSamples, int midiChannel)
{
    BlendronicPreparation::Ptr prep = blendronic->aPrep;
    thresholdSamples = (prep->getClusterThreshSEC() * sampleRate);
    
    if (inSyncCluster)
    {
        if (syncThresholdTimer >= thresholdSamples)
        {
            inSyncCluster = false;
        }
        else
        {
            syncThresholdTimer += numSamples;
        }
    }
    
    if (inClearCluster)
    {
        if (clearThresholdTimer >= thresholdSamples)
        {
            inClearCluster = false;
        }
        else
        {
            clearThresholdTimer += numSamples;
        }
    }
    
    if (inOpenCluster)
    {
        if (openThresholdTimer >= thresholdSamples)
        {
            inOpenCluster = false;
        }
        else
        {
            openThresholdTimer += numSamples;
        }
    }
    
    if (inCloseCluster)
    {
        if (closeThresholdTimer >= thresholdSamples)
        {
            inCloseCluster = false;
        }
        else
        {
            closeThresholdTimer += numSamples;
        }
    }
}

float BlendronicProcessor::getTimeToBeatMS(float beatsToSkip)
{
	uint64 timeToReturn = numSamplesBeat - sampleTimer;
	return timeToReturn * 1000. / sampleRate; //will make more precise later
}

bool BlendronicProcessor::velocityCheck(int noteNumber)
{
    BlendronicPreparation::Ptr prep = blendronic->aPrep;
    
    int velocity = (int)(velocities.getUnchecked(noteNumber) * 127.0);
    
    if (velocity > 127) velocity = 127;
    if (velocity < 0)   velocity = 0;
    
    if(prep->getVelocityMin() <= prep->getVelocityMax())
    {
        if (velocity >= prep->getVelocityMin() && velocity <= prep->getVelocityMax())
        {
            return true;
        }
    }
    else
    {
        if (velocity >= prep->getVelocityMin() || velocity <= prep->getVelocityMax())
        {
            return true;
        }
    }
    
    DBG("failed velocity check");
    return false;
}

bool BlendronicProcessor::holdCheck(int noteNumber)
{
    BlendronicPreparation::Ptr prep = blendronic->aPrep;
    
    uint64 hold = holdTimers.getUnchecked(noteNumber) * (1000.0 / sampleRate);
    
    if(prep->getHoldMin() <= prep->getHoldMax())
    {
        if (hold >= prep->getHoldMin() && hold <= prep->getHoldMax())
        {
            return true;
        }
    }
    else
    {
        if (hold >= prep->getHoldMin() || hold <= prep->getHoldMax())
        {
            return true;
        }
    }
    
    DBG("failed hold check");
    return false;
}

void BlendronicProcessor::keyPressed(int noteNumber, float velocity, int midiChannel, Array<KeymapTargetState> targetStates)
{
    BlendronicPreparation::Ptr prep = blendronic->aPrep;
    TempoPreparation::Ptr tempoPrep = tempo->getTempo()->aPrep;
    
    //add note to array of depressed notes
    keysDepressed.addIfNotAlreadyThere(noteNumber);
    velocities.set(noteNumber, velocity);
    holdTimers.set(noteNumber, 0);
    
    // Get target flags
    bool doSync = targetStates[TargetTypeBlendronicSync] == TargetStateEnabled;
    bool doPatternSync = targetStates[TargetTypeBlendronicPatternSync] == TargetStateEnabled;
    bool doClear = targetStates[TargetTypeBlendronicClear] == TargetStateEnabled;
    bool doPausePlay = targetStates[TargetTypeBlendronicPausePlay] == TargetStateEnabled;
    bool doOpenCloseInput = targetStates[TargetTypeBlendronicOpenCloseInput] == TargetStateEnabled;
    bool doOpenCloseOutput = targetStates[TargetTypeBlendronicOpenCloseOutput] == TargetStateEnabled;
//
//    if (doSync) syncKeysDepressed.addIfNotAlreadyThere(noteNumber);
//    if (doPatternSync) clearKeysDepressed.addIfNotAlreadyThere(noteNumber);
//    if (doClear) clearKeysDepressed.addIfNotAlreadyThere(noteNumber);
//    if (doPausePlay) clearKeysDepressed.addIfNotAlreadyThere(noteNumber);
//    if (doOpenCloseInput) openKeysDepressed.addIfNotAlreadyThere(noteNumber);
//    if (doOpenCloseInput) closeKeysDepressed.addIfNotAlreadyThere(noteNumber);
    
    
    if (!velocityCheck(noteNumber)) return;
    
    if (doSync && (prep->getTargetTypeBlendronicSync() == NoteOn || prep->getTargetTypeBlendronicSync() == Both))
    {
        // Sync to the next beat
        setSampleTimer(numSamplesBeat);
        beatPositionsInBuffer.clear();
        beatPositionsInBuffer.add(delay->getCurrentSample());
        pulseOffset = delay->getCurrentSample();
        beatPositionsIndex = 0;
    }
    
    if (doPatternSync &&
        (prep->getTargetTypeBlendronicPatternSync() == NoteOn || prep->getTargetTypeBlendronicPatternSync() == Both))
    {
        // Reset the phase of all params and update values
        setSampleTimer(0);
        setBeatIndex(0);
        setDelayIndex(0);
        setSmoothIndex(0);
        setFeedbackIndex(0);
        beatPositionsInBuffer.clear();
        beatPositionsInBuffer.add(delay->getCurrentSample());
        pulseOffset = delay->getCurrentSample();
        beatPositionsIndex = 0;
        pulseLength = (60.0 / (tempoPrep->getSubdivisions() * tempoPrep->getTempo()));
        numSamplesBeat = prep->getBeats()[beatIndex] * pulseLength * sampleRate;
        updateDelayParameters();
    }
    
    if (doClear &&
        (prep->getTargetTypeBlendronicClear() == NoteOn || prep->getTargetTypeBlendronicClear() == Both))
    {
        delay->clear();
    }
    
    if (doPausePlay &&
        (prep->getTargetTypeBlendronicPausePlay() == NoteOn || prep->getTargetTypeBlendronicPausePlay() == Both))
    {
        toggleActive();
    }
    
    if (doOpenCloseInput &&
        (prep->getTargetTypeBlendronicOpenCloseInput() == NoteOn || prep->getTargetTypeBlendronicOpenCloseInput() == Both))
    {
        toggleInput();
    }
    
    if (doOpenCloseOutput &&
        (prep->getTargetTypeBlendronicOpenCloseOutput() == NoteOn || prep->getTargetTypeBlendronicOpenCloseOutput() == Both))
    {
        toggleOutput();
    }
}

void BlendronicProcessor::keyReleased(int noteNumber, float velocity, int midiChannel, Array<KeymapTargetState> targetStates, bool post)
{
    BlendronicPreparation::Ptr prep = blendronic->aPrep;
    TempoPreparation::Ptr tempoPrep = tempo->getTempo()->aPrep;
    
    //remove key from array of pressed keys
    keysDepressed.removeAllInstancesOf(noteNumber);
    
    // Get target flags
    bool doSync = targetStates[TargetTypeBlendronicSync] == TargetStateEnabled;
    bool doPatternSync = targetStates[TargetTypeBlendronicPatternSync] == TargetStateEnabled;
    bool doClear = targetStates[TargetTypeBlendronicClear] == TargetStateEnabled;
    bool doPausePlay = targetStates[TargetTypeBlendronicPausePlay] == TargetStateEnabled;
    bool doOpenCloseInput = targetStates[TargetTypeBlendronicOpenCloseInput] == TargetStateEnabled;
    bool doOpenCloseOutput = targetStates[TargetTypeBlendronicOpenCloseOutput] == TargetStateEnabled;
//
//    if (doSync) syncKeysDepressed.removeAllInstancesOf(noteNumber);
//    if (doClear) clearKeysDepressed.removeAllInstancesOf(noteNumber);
//    if (doOpen) openKeysDepressed.removeAllInstancesOf(noteNumber);
//    if (doClose) closeKeysDepressed.removeAllInstancesOf(noteNumber);
    
    if (!velocityCheck(noteNumber)) return;
    if (!holdCheck(noteNumber)) return;
    
    if (doSync && (prep->getTargetTypeBlendronicSync() == NoteOff || prep->getTargetTypeBlendronicSync() == Both))
    {
        // Sync to the next beat
        setSampleTimer(numSamplesBeat);
        beatPositionsInBuffer.clear();
        beatPositionsInBuffer.add(delay->getCurrentSample());
        pulseOffset = delay->getCurrentSample();
        beatPositionsIndex = 0;
    }
    
    if (doPatternSync &&
        (prep->getTargetTypeBlendronicPatternSync() == NoteOff || prep->getTargetTypeBlendronicPatternSync() == Both))
    {
        // Reset the phase of all params and update values
        setSampleTimer(0);
        setBeatIndex(0);
        setDelayIndex(0);
        setSmoothIndex(0);
        setFeedbackIndex(0);
        beatPositionsInBuffer.clear();
        beatPositionsInBuffer.add(delay->getCurrentSample());
        pulseOffset = delay->getCurrentSample();
        beatPositionsIndex = 0;
        pulseLength = (60.0 / (tempoPrep->getSubdivisions() * tempoPrep->getTempo()));
        numSamplesBeat = prep->getBeats()[beatIndex] * pulseLength * sampleRate;
        updateDelayParameters();
    }
    
    if (doClear &&
        (prep->getTargetTypeBlendronicClear() == NoteOff || prep->getTargetTypeBlendronicClear() == Both))
    {
        delay->clear();
    }
    
    if (doPausePlay &&
        (prep->getTargetTypeBlendronicPausePlay() == NoteOff || prep->getTargetTypeBlendronicPausePlay() == Both))
    {
        toggleActive();
    }
    
    if (doOpenCloseInput &&
        (prep->getTargetTypeBlendronicOpenCloseInput() == NoteOff || prep->getTargetTypeBlendronicOpenCloseInput() == Both))
    {
        toggleInput();
    }
    
    if (doOpenCloseOutput &&
        (prep->getTargetTypeBlendronicOpenCloseOutput() == NoteOff || prep->getTargetTypeBlendronicOpenCloseOutput() == Both))
    {
        toggleOutput();
    }
}

void BlendronicProcessor::postRelease(int noteNumber, int midiChannel)
{
	//may not need anything except communicating with other preparations?
}

void BlendronicProcessor::prepareToPlay(double sr)
{
    BlendronicPreparation::Ptr prep = blendronic->aPrep;
    TempoPreparation::Ptr tempoPrep = tempo->getTempo()->aPrep;
    
    delay = synth->createBlendronicDelay(prep->getDelayLengths()[0], prep->getSmoothValues()[0],
                                         prep->getFeedbackCoefficients()[0], prep->getDelayMax(), true);
    delay->setSampleRate(sr);
    
    beatPositionsInBuffer.ensureStorageAllocated(20);
    beatPositionsInBuffer.clear();
    beatPositionsInBuffer.add(0);
    pulseOffset = 0;
    beatPositionsIndex = 0;
    
    prevBeat = prep->getBeats()[0];
    prevDelay = prep->getDelayLengths()[0];
    
	sampleRate = sr;
    delay->setSampleRate(sr);
    
    beatIndex = 0;
    delayIndex = 0;
    smoothIndex = 0;
    feedbackIndex = 0;
    
    pulseLength = (60.0 / (tempoPrep->getSubdivisions() * tempoPrep->getTempo()));
    numSamplesBeat = prep->getBeats()[beatIndex] * sampleRate * ((60.0 / tempoPrep->getSubdivisions()) / tempoPrep->getTempo());
    numSamplesDelay = prep->getDelayLengths()[delayIndex] * sampleRate * ((60.0 / tempoPrep->getSubdivisions()) / tempoPrep->getTempo());
    
    delay->setDelayLength(numSamplesDelay);
    delay->setDelayTargetLength(numSamplesDelay);
    
    updateDelayParameters();
}

void BlendronicProcessor::playNote(int channel, int note, float velocity)
{
	//may not need anything except communicating with other preparations?
}

void BlendronicProcessor::updateDelayParameters()
{
    BlendronicPreparation::Ptr prep = blendronic->aPrep;
    
    numSamplesDelay = prep->getDelayLengths()[delayIndex] * pulseLength * sampleRate;
    
    float delayDelta = fabsf(prevDelay - prep->getDelayLengths()[delayIndex]);
    prevDelay = prep->getDelayLengths()[delayIndex];
    
    float smoothRate = 0.0f;
    float smoothBase = 0.0f;
    if (prep->getSmoothBase()       == BlendronicSmoothPulse)
    {
        smoothBase = pulseLength;
    }
    else if (prep->getSmoothBase()  == BlendronicSmoothBeat)
    {
        smoothBase = pulseLength * prep->getBeats()[beatIndex];
    }
    
    if (prep->getSmoothScale()  == BlendronicSmoothConstant)
    {
        smoothRate = smoothBase / prep->getSmoothValues()[smoothIndex];
    }
    else if (prep->getSmoothScale() == BlendronicSmoothFull)
    {
        smoothRate = delayDelta * (smoothBase / prep->getSmoothValues()[smoothIndex]);
        if (delayDelta == 0) smoothRate = INFINITY;
    }
    
    // DBG(String(getId()) + " new envelope target = " + String(numSamplesDelay));
    // DBG(String(1000. / smoothRate) + "ms"); // smooth rate is change / second
    delay->setDelayTargetLength(numSamplesDelay);
    delay->setSmoothDuration(smoothRate); // this is really a rate, not a duration
    delay->setFeedback(prep->getFeedbackCoefficients()[feedbackIndex]);
}
