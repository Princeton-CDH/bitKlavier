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
	bSmoothDurations(p->getSmoothDurations()),
	bFeedbackCoefficients(p->getFeedbackCoefficients()),
	bDelayMax(p->getDelayMax()),
	bDelayLength(p->getDelayLength()),
	bSmoothValue(p->getSmoothValue()),
	bSmoothDuration(p->getSmoothDuration()),
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
BlendronicPreparation::BlendronicPreparation(String newName, Array<float> beats, Array<float> smoothTimes,
	Array<float> feedbackCoefficients, float smoothValue, float smoothDuration, BlendronicSmoothMode smoothMode,
    BlendronicSyncMode syncMode, BlendronicClearMode clearMode, BlendronicOpenMode openMode, BlendronicCloseMode closeMode,
	float delayMax, float delayLength, float feedbackCoefficient) :
	name(newName),
	bBeats(beats),
	bSmoothDurations(smoothTimes),
	bFeedbackCoefficients(feedbackCoefficients),
	bDelayMax(delayMax),
	bDelayLength(delayLength),
	bSmoothValue(smoothValue),
	bSmoothDuration(smoothDuration),
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
	bBeats(Array<float>({ 4. })),
	bSmoothDurations(Array<float>({ 0.1 })),
	bFeedbackCoefficients(Array<float>({ 0.95 })),
	bDelayMax(44100. * 5.),
	bDelayLength(44100. * 2.),
	bSmoothValue(180. * 44.1),
	bSmoothDuration(0),
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
/////////////////////////BLENDRONIC PROCESSOR/////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

BlendronicProcessor::BlendronicProcessor(Blendronic::Ptr bBlendronic,
	TempoProcessor::Ptr bTempo, GeneralSettings::Ptr bGeneral, BKSynthesiser* bMain):
	blendronic(bBlendronic),
	tempo(bTempo),
	synth(bMain),
	general(bGeneral),
    keymaps(Keymap::PtrArr()),
	sampleTimer(0),
	beatIndex(0),
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
    
    BlendronicPreparation::Ptr prep = blendronic->aPrep;
    
    delay = synth->createBlendronicDelay(prep->getDelayMax(), prep->getFeedbackCoefficients()[0], prep->getDelayLength(), prep->getSmoothValue(), prep->getSmoothDuration(), true);
    
    DBG("Create bproc");
}

BlendronicProcessor::~BlendronicProcessor()
{
    DBG("Destroy bproc");
}

void BlendronicProcessor::tick(float* outputs)
{
    BlendronicPreparation::Ptr prep = blendronic->aPrep;
    
    sampleTimer++;

    if (sampleTimer >= numSamplesBeat)
    {
        beatIndex++;
        if (beatIndex >= prep->getBeats().size()) beatIndex = 0;
        smoothIndex++;
        if (smoothIndex >= prep->getSmoothDurations().size()) smoothIndex = 0;
        feedbackIndex++;
        if (feedbackIndex >= prep->getFeedbackCoefficients().size()) feedbackIndex = 0;

        updateDelayParameters();
        if (clearDelayOnNextBeat)
        {
            delay->clear();
            clearDelayOnNextBeat = false;
        }
    }
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
    
    //add note to array of depressed notes
    keysDepressed.addIfNotAlreadyThere(noteNumber);
    velocities.set(noteNumber, velocity);
    holdTimers.set(noteNumber, 0);
    
    bool doSync = targetStates[TargetTypeBlendronicSync] == TargetStateEnabled;
    bool doClear = targetStates[TargetTypeBlendronicClear] == TargetStateEnabled;
    bool doOpen = targetStates[TargetTypeBlendronicOpen] == TargetStateEnabled;
    bool doClose = targetStates[TargetTypeBlendronicClose] == TargetStateEnabled;
    
    if (!velocityCheck(noteNumber)) return;
    
    if (doSync)
    {
        if (prep->getSyncMode() == BlendronicAnyNoteOnSync ||
           (prep->getSyncMode() == BlendronicFirstNoteOnSync && keysDepressed.size() == 1))
        {
            setSampleTimer(0);
            setBeatIndex(0);
            setSmoothIndex(0);
            setFeedbackIndex(0);
            updateDelayParameters();
        }
    }
    if (doClear)
    {
        if (prep->getClearMode() == BlendronicAnyNoteOnClear ||
           (prep->getClearMode() == BlendronicFirstNoteOnClear && keysDepressed.size() == 1))
        {
            delay->clear();
            //clearDelayOnNextBeat = true;
        }
    }
    bool noteOnClose = prep->getCloseMode() == BlendronicAnyNoteOnClose;
    bool firstNoteOnClose = prep->getCloseMode() == BlendronicFirstNoteOnClose && keysDepressed.size() == 1;
    bool noteOnOpen = prep->getOpenMode() == BlendronicAnyNoteOnOpen;
    bool firstNoteOnOpen = prep->getOpenMode() == BlendronicFirstNoteOnOpen && keysDepressed.size() == 1;
    if (doClose && doOpen)
    {
        if ((noteOnClose && noteOnOpen) || (firstNoteOnClose && firstNoteOnOpen)) delay->setActive(!delay->getActive());
    }
    else if (doClose)
    {
        if (noteOnClose || firstNoteOnClose) delay->setActive(false);
    }
    else if (doOpen)
    {
        if (noteOnOpen || firstNoteOnOpen) delay->setActive(true);
    }
}

void BlendronicProcessor::keyReleased(int noteNumber, float velocity, int midiChannel, Array<KeymapTargetState> targetStates, bool post)
{
    BlendronicPreparation::Ptr prep = blendronic->aPrep;
    
    //remove key from array of pressed keys
    keysDepressed.removeAllInstancesOf(noteNumber);
    
    bool doSync = targetStates[TargetTypeBlendronicSync] == TargetStateEnabled;
    bool doClear = targetStates[TargetTypeBlendronicClear] == TargetStateEnabled;
    bool doOpen = targetStates[TargetTypeBlendronicOpen] == TargetStateEnabled;
    bool doClose = targetStates[TargetTypeBlendronicClose] == TargetStateEnabled;
    
    if (!velocityCheck(noteNumber)) return;
    if (!holdCheck(noteNumber)) return;
    
    if (doSync)
    {
        if (prep->getSyncMode() == BlendronicAnyNoteOffSync ||
           (prep->getSyncMode() == BlendronicLastNoteOffSync && keysDepressed.size() == 0))
        {
            setSampleTimer(0);
            setBeatIndex(0);
            setSmoothIndex(0);
            setFeedbackIndex(0);
            updateDelayParameters();
        }
    }
    if (doClear)
    {
        if (prep->getClearMode() == BlendronicAnyNoteOffClear)
        {
            delay->clear();
        }
        else if (prep->getClearMode() == BlendronicLastNoteOffClear && keysDepressed.size() == 0)
        {
            delay->duckAndClear();
            clearDelayOnNextBeat = true;
        }
    }
    
    bool noteOffClose = prep->getCloseMode() == BlendronicAnyNoteOffClose;
    bool firstNoteOffClose = prep->getCloseMode() == BlendronicLastNoteOffClose && keysDepressed.size() == 0;
    bool noteOffOpen = prep->getOpenMode() == BlendronicAnyNoteOffOpen;
    bool firstNoteOffOpen = prep->getOpenMode() == BlendronicLastNoteOffOpen && keysDepressed.size() == 0;
    if (doClose && doOpen)
    {
        if ((noteOffClose && noteOffOpen) || (firstNoteOffClose && firstNoteOffOpen)) delay->setActive(!delay->getActive());
    }
    else if (doClose)
    {
        if (noteOffClose || firstNoteOffClose) delay->setActive(false);
    }
    else if (doOpen)
    {
        if (noteOffOpen || firstNoteOffOpen) delay->setActive(true);
    }
}

void BlendronicProcessor::postRelease(int noteNumber, int midiChannel)
{
	//may not need anything except communicating with other preparations?
}

void BlendronicProcessor::prepareToPlay(double sr)
{
    BlendronicPreparation::Ptr prep = blendronic->aPrep;
    prevBeat = prep->getBeats()[0];
    
	sampleRate = sr;
    delay->setSampleRate(sr);
    numSamplesBeat = prep->getBeats()[beatIndex] * sampleRate * ((60.0 / tempo->getTempo()->aPrep->getSubdivisions()) / tempo->getTempo()->aPrep->getTempo());
    prep->setDelayLength(numSamplesBeat);
    delay->setDelayTargetLength(numSamplesBeat);
}

void BlendronicProcessor::playNote(int channel, int note, float velocity)
{
	//may not need anything except communicating with other preparations?
}

void BlendronicProcessor::updateDelayParameters()
{
    BlendronicPreparation::Ptr prep = blendronic->aPrep;
    TempoPreparation::Ptr tempoPrep = tempo->getTempo()->aPrep;
    
    float pulseLength = (60.0 / (tempoPrep->getSubdivisions() * tempoPrep->getTempo()));
    numSamplesBeat = prep->getBeats()[beatIndex] * pulseLength * sampleRate;
    
    float smoothRate = 0.0f; // samplesOfDelayLength per tick
    float beatDelta = fabsf(prevBeat - prep->getBeats()[beatIndex]);
    prevBeat = prep->getBeats()[beatIndex];
    if (prep->getSmoothMode()       == ConstantRateSmooth)
    {
        smoothRate = prep->getSmoothDurations()[smoothIndex] / pulseLength;
    }
    else if (prep->getSmoothMode()  == ConstantTimeSmooth)
    {
        if (beatDelta == 0)
            smoothRate = INFINITY;//prep->getSmoothDurations()[smoothIndex] / pulseLength;
        else
            smoothRate = beatDelta / (prep->getSmoothDurations()[smoothIndex] * pulseLength);
    }
    else if (prep->getSmoothMode()  == ProportionalRateSmooth)
    {
        smoothRate = prep->getSmoothDurations()[smoothIndex] / (pulseLength * prep->getBeats()[beatIndex]);
    }
    else if (prep->getSmoothMode()  == ProportionalTimeSmooth)
    {
        if (beatDelta == 0)
            smoothRate = INFINITY;//prep->getSmoothDurations()[smoothIndex] / (pulseLength * prep->getBeats()[beatIndex]);
        else
            smoothRate = beatDelta / (prep->getSmoothDurations()[smoothIndex] * pulseLength * prep->getBeats()[beatIndex]);
    }
    
    sampleTimer = 0;
    
    DBG(String(getId()) + " new envelope target = " + String(numSamplesBeat));
    DBG(String(smoothRate));
    delay->setDelayTargetLength(numSamplesBeat);
    delay->setSmoothDuration(smoothRate);
    delay->setFeedback(prep->getFeedbackCoefficients()[feedbackIndex]);
}
