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
	bSmoothDurations(p->getSmoothDurations()),
	bFeedbackCoefficients(p->getFeedbackCoefficients()),
	bDelayMax(p->getDelayMax()),
    bSmoothMode(p->getSmoothMode()),
    bSyncMode(p->getSyncMode()),
    bClearMode(p->getClearMode()),
    bOpenMode(p->getOpenMode()),
    bCloseMode(p->getCloseMode()),
	bInputThresh(p->getInputThreshMS()),
	bInputThreshSec(p->getInputThreshSEC()),
	holdMin(p->getHoldMin()),
	holdMax(p->getHoldMax()),
	velocityMin(p->getVelocityMin()),
	velocityMax(p->getVelocityMax())
{
}

//constructor with input
BlendronicPreparation::BlendronicPreparation(String newName, Array<float> beats, Array<float> delayLengths, Array<float> smoothTimes,
	Array<float> feedbackCoefficients, BlendronicSmoothMode smoothMode, BlendronicSyncMode syncMode, BlendronicClearMode clearMode, BlendronicOpenMode openMode, BlendronicCloseMode closeMode, float delayMax) :
	name(newName),
	bBeats(beats),
    bDelayLengths(delayLengths),
	bSmoothDurations(smoothTimes),
	bFeedbackCoefficients(feedbackCoefficients),
	bDelayMax(delayMax),
    bSmoothMode(smoothMode),
    bSyncMode(syncMode),
    bClearMode(clearMode),
    bOpenMode(openMode),
    bCloseMode(closeMode),
	bInputThresh(1),
	bInputThreshSec(0.001),
	holdMin(0),
	holdMax(12000),
	velocityMin(0),
	velocityMax(127)
{
}

//empty constructor
BlendronicPreparation::BlendronicPreparation(void) :
	name("blank blendronic"),
	bBeats(Array<float>({ 4., 3., 2., 3.})),
    bDelayLengths(Array<float>({ 4., 3., 2., 3.})),
	bSmoothDurations(Array<float>({ 0.1 })),
	bFeedbackCoefficients(Array<float>({ 0.95 })),
	bDelayMax(44100. * 5.),
    bSmoothMode(ConstantTimeSmooth),
    bSyncMode(BlendronicFirstNoteOnSync),
    bClearMode(BlendronicFirstNoteOnClear),
    bOpenMode(BlendronicOpenModeNil),
    bCloseMode(BlendronicCloseModeNil),
	bInputThresh(1),
	bInputThreshSec(0.001),
	holdMin(0),
	holdMax(12000),
	velocityMin(0),
	velocityMax(127)
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
    
    keysDepressed = Array<int>();
    
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
        if (smoothIndex >= prep->getSmoothDurations().size()) smoothIndex = 0;
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
}

void BlendronicProcessor::processBlock(int numSamples, int midiChannel)
{
    //DBG(String(delay->getSmoothValue()));
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
    bool doClear = targetStates[TargetTypeBlendronicClear] == TargetStateEnabled;
    bool doOpen = targetStates[TargetTypeBlendronicOpen] == TargetStateEnabled;
    bool doClose = targetStates[TargetTypeBlendronicClose] == TargetStateEnabled;
    
    if (!velocityCheck(noteNumber)) return;
    
    if (doClear)
    {
        if (prep->getClearMode() == BlendronicAnyNoteOnClear ||
           (prep->getClearMode() == BlendronicFirstNoteOnClear && keysDepressed.size() == 1))
        {
            // Just clear the delay line
            delay->clear();
            //clearDelayOnNextBeat = true;
        }
    }
    if (doSync)
    {
        if (prep->getSyncMode() == BlendronicAnyNoteOnSync ||
            (prep->getSyncMode() == BlendronicFirstNoteOnSync && keysDepressed.size() == 1))
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
    }
    // Get conditions for opening/closing the delay line
    bool noteOnClose = (prep->getCloseMode() == BlendronicAnyNoteOnClose) && doClose;
    bool firstNoteOnClose = (prep->getCloseMode() == BlendronicFirstNoteOnClose && keysDepressed.size() == 1) && doClose;
    bool noteOnOpen = (prep->getOpenMode() == BlendronicAnyNoteOnOpen) && doOpen;
    bool firstNoteOnOpen = (prep->getOpenMode() == BlendronicFirstNoteOnOpen && keysDepressed.size() == 1) && doOpen;
    
    // If the delay would be opened and closed by the same keypress, toggle it
    if ((noteOnClose || firstNoteOnClose) && (noteOnOpen || firstNoteOnOpen)) toggleActive();
    else if (noteOnClose || firstNoteOnClose) setActive(false);
    else if (noteOnOpen || firstNoteOnOpen) setActive(true);
}

void BlendronicProcessor::keyReleased(int noteNumber, float velocity, int midiChannel, Array<KeymapTargetState> targetStates, bool post)
{
    BlendronicPreparation::Ptr prep = blendronic->aPrep;
    TempoPreparation::Ptr tempoPrep = tempo->getTempo()->aPrep;
    
    //remove key from array of pressed keys
    keysDepressed.removeAllInstancesOf(noteNumber);
    
    // Get target flags
    bool doSync = targetStates[TargetTypeBlendronicSync] == TargetStateEnabled;
    bool doClear = targetStates[TargetTypeBlendronicClear] == TargetStateEnabled;
    bool doOpen = targetStates[TargetTypeBlendronicOpen] == TargetStateEnabled;
    bool doClose = targetStates[TargetTypeBlendronicClose] == TargetStateEnabled;
    
    if (!velocityCheck(noteNumber)) return;
    if (!holdCheck(noteNumber)) return;
    
    if (doClear)
    {
        if (prep->getClearMode() == BlendronicAnyNoteOffClear)
        {
            // Clear the delay line, but duck with an env first to prevent a click
            // (don't know if this is necessary, Blendronic tends to have clicks as part of its sound)
            delay->duckAndClear();
            if (keysDepressed.size() == 0)
            {
                // Set flag to clear on next beat to prevent any release sound from being left in buffer,
                // since the expected behavior of clearing on last note off is silence
                clearDelayOnNextBeat = true;
            }
        }
        else if (prep->getClearMode() == BlendronicLastNoteOffClear && keysDepressed.size() == 0)
        {
            // Clear the delay line, but duck with an env first to prevent a click
            delay->duckAndClear();
            // Set flag to clear on next beat to prevent any release sound from being left in buffer,
            // since the expected behavior of clearing on last note off is silence
            clearDelayOnNextBeat = true;
        }
    }
    if (doSync)
    {
        if (prep->getSyncMode() == BlendronicAnyNoteOffSync ||
            (prep->getSyncMode() == BlendronicLastNoteOffSync && keysDepressed.size() == 0))
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
    }
    
    bool noteOffClose = (prep->getCloseMode() == BlendronicAnyNoteOffClose) && doClose;
    bool firstNoteOffClose = (prep->getCloseMode() == BlendronicLastNoteOffClose && keysDepressed.size() == 0) && doClose;
    bool noteOffOpen = (prep->getOpenMode() == BlendronicAnyNoteOffOpen) && doOpen;
    bool firstNoteOffOpen = (prep->getOpenMode() == BlendronicLastNoteOffOpen && keysDepressed.size() == 0) && doOpen;
    
    if ((noteOffClose || firstNoteOffClose) && (noteOffOpen || firstNoteOffOpen)) toggleActive();
    else if (noteOffClose || firstNoteOffClose) setActive(false);
    else if (noteOffOpen || firstNoteOffOpen) setActive(true);
}

void BlendronicProcessor::postRelease(int noteNumber, int midiChannel)
{
	//may not need anything except communicating with other preparations?
}

void BlendronicProcessor::prepareToPlay(double sr)
{
    BlendronicPreparation::Ptr prep = blendronic->aPrep;
    TempoPreparation::Ptr tempoPrep = tempo->getTempo()->aPrep;
    
    delay = synth->createBlendronicDelay(prep->getDelayLengths()[0], prep->getSmoothDurations()[0],
                                         prep->getFeedbackCoefficients()[0], prep->getDelayMax(), true);
    
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
    if (prep->getSmoothMode()       == ConstantRateSmooth)
    {
        smoothRate = prep->getSmoothDurations()[smoothIndex] / pulseLength;
    }
    else if (prep->getSmoothMode()  == ConstantTimeSmooth)
    {
        if (delayDelta == 0)
            smoothRate = INFINITY;
        else
            smoothRate = delayDelta / (prep->getSmoothDurations()[smoothIndex] * pulseLength);
    }
    else if (prep->getSmoothMode()  == ProportionalRateSmooth)
    {
        smoothRate = prep->getSmoothDurations()[smoothIndex] / (pulseLength * prep->getBeats()[beatIndex]);
    }
    else if (prep->getSmoothMode()  == ProportionalTimeSmooth)
    {
        if (delayDelta == 0)
            smoothRate = INFINITY;//prep->getSmoothDurations()[smoothIndex] / (pulseLength * prep->getBeats()[beatIndex]);
        else
            smoothRate = delayDelta / (prep->getSmoothDurations()[smoothIndex] * pulseLength * prep->getBeats()[beatIndex]);
    }
    
    DBG(String(getId()) + " new envelope target = " + String(numSamplesDelay));
    DBG(String(smoothRate));
    delay->setDelayTargetLength(numSamplesDelay);
    delay->setSmoothDuration(smoothRate);
    delay->setFeedback(prep->getFeedbackCoefficients()[feedbackIndex]);
}
