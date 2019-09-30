/*
  ==============================================================================

	Blendronomer.cpp
	Created: 11 Jun 2019 2:00:53pm
	Author:  Theodore R Trevisan

  ==============================================================================
*/

#include "Blendronomer.h"

#include "BKSynthesiser.h"

//copy constructor
BlendronomerPreparation::BlendronomerPreparation(BlendronomerPreparation::Ptr p) :
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
BlendronomerPreparation::BlendronomerPreparation(String newName, Array<float> beats, Array<float> smoothTimes,
	Array<float> feedbackCoefficients, float smoothValue, float smoothDuration, BlendronomerSmoothMode smoothMode,
    BlendronomerSyncMode syncMode, BlendronomerClearMode clearMode, BlendronomerOpenMode openMode, BlendronomerCloseMode closeMode,
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
BlendronomerPreparation::BlendronomerPreparation(void) :
	name("blank blendronomer"),
	bBeats(Array<float>({ 4. })),
	bSmoothDurations(Array<float>({ 0.1 })),
	bFeedbackCoefficients(Array<float>({ 0.95 })),
	bDelayMax(44100. * 5.),
	bDelayLength(44100. * 2.),
	bSmoothValue(180. * 44.1),
	bSmoothDuration(0),
    bSmoothMode(ConstantTimeSmooth),
    bSyncMode(BlendronomerFirstNoteOnSync),
    bClearMode(BlendronomerFirstNoteOnClear),
    bOpenMode(BlendronomerFirstNoteOnOpen),
    bCloseMode(BlendronomerFirstNoteOnClose),
	bInputThresh(1),
	bInputThreshSec(0.001),
	holdMin(0),
	holdMax(12000),
	velocityMin(0),
	velocityMax(127)
{
}

//copy
void BlendronomerPreparation::copy(BlendronomerPreparation::Ptr b)
{
}

//maps modification to values
void BlendronomerPreparation::performModification(BlendronomerPreparation::Ptr b, Array<bool> dirty)
{
}

//compares two blendronomers
bool BlendronomerPreparation::compare(BlendronomerPreparation::Ptr b)
{
	//will do later
	return false;
}

//prints each variable
void BlendronomerPreparation::print(void)
{
}

//will do these later
ValueTree BlendronomerPreparation::getState(void)
{
	return ValueTree();
}

void BlendronomerPreparation::setState(XmlElement* e)
{
}


////////////////////////////////////////////////////////////////////////////////////
/////////////////////////BLENDRONOMER PROCESSOR/////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

BlendronomerProcessor::BlendronomerProcessor(Blendronomer::Ptr bBlendronomer,
	TempoProcessor::Ptr bTempo, BlendronicDelay::Ptr delayL, GeneralSettings::Ptr bGeneral, BKSynthesiser* bMain):
	blendronomer(bBlendronomer),
	tempo(bTempo),
	delay(delayL),
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
    prevBeat = blendronomer->aPrep->getBeats()[0];
    pulseRate = tempo->getTempo()->aPrep->getSubdivisions() * tempo->getTempo()->aPrep->getTempo();
}

BlendronomerProcessor::~BlendronomerProcessor()
{
    synth->removeBlendronicDelay(delay);
}

void BlendronomerProcessor::tick(float* outputs)
{
    BlendronomerPreparation::Ptr prep = blendronomer->aPrep;
    
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

void BlendronomerProcessor::processBlock(int numSamples, int midiChannel)
{
    //DBG(String(delay->getSmoothValue()));
}

float BlendronomerProcessor::getTimeToBeatMS(float beatsToSkip)
{
	uint64 timeToReturn = numSamplesBeat - sampleTimer;
	return timeToReturn * 1000. / sampleRate; //will make more precise later
}

bool BlendronomerProcessor::velocityCheck(int noteNumber)
{
    BlendronomerPreparation::Ptr prep = blendronomer->aPrep;
    
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

bool BlendronomerProcessor::holdCheck(int noteNumber)
{
    BlendronomerPreparation::Ptr prep = blendronomer->aPrep;
    
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

void BlendronomerProcessor::keyPressed(int noteNumber, float velocity, int midiChannel, Array<KeymapTargetState> targetStates)
{
    BlendronomerPreparation::Ptr prep = blendronomer->aPrep;
    
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
        if (prep->getSyncMode() == BlendronomerAnyNoteOnSync ||
           (prep->getSyncMode() == BlendronomerFirstNoteOnSync && keysDepressed.size() == 1))
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
        if (prep->getClearMode() == BlendronomerAnyNoteOnClear ||
           (prep->getClearMode() == BlendronomerFirstNoteOnClear && keysDepressed.size() == 1))
        {
            delay->clear();
            //clearDelayOnNextBeat = true;
        }
    }
    if (doClose)
    {
        if (prep->getCloseMode() == BlendronomerAnyNoteOnClose ||
           (prep->getCloseMode() == BlendronomerFirstNoteOnClose && keysDepressed.size() == 1))
            delay->setActive(false);
    }
    if (doOpen)
    {
        if (prep->getOpenMode() == BlendronomerAnyNoteOnOpen ||
           (prep->getOpenMode() == BlendronomerFirstNoteOnOpen && keysDepressed.size() == 1))
            delay->setActive(true);
    }
}

void BlendronomerProcessor::keyReleased(int noteNumber, float velocity, int midiChannel, Array<KeymapTargetState> targetStates, bool post)
{
    BlendronomerPreparation::Ptr prep = blendronomer->aPrep;
    
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
        if (prep->getSyncMode() == BlendronomerAnyNoteOffSync ||
           (prep->getSyncMode() == BlendronomerLastNoteOffSync && keysDepressed.size() == 0))
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
        if (prep->getClearMode() == BlendronomerAnyNoteOffClear)
        {
            delay->clear();
        }
        else if (prep->getClearMode() == BlendronomerLastNoteOffClear && keysDepressed.size() == 0)
        {
            delay->duckAndClear();
            clearDelayOnNextBeat = true;
        }
    }
    if (doClose)
    {
        if (prep->getCloseMode() == BlendronomerAnyNoteOffClose ||
           (prep->getCloseMode() == BlendronomerLastNoteOffClose && keysDepressed.size() == 0))
        delay->setActive(false);
    }
    if (doOpen)
    {
        if (prep->getOpenMode() == BlendronomerAnyNoteOffOpen ||
           (prep->getOpenMode() == BlendronomerLastNoteOffOpen && keysDepressed.size() == 0))
        delay->setActive(true);
    }
}

void BlendronomerProcessor::postRelease(int noteNumber, int midiChannel)
{
	//may not need anything except communicating with other preparations?
}

void BlendronomerProcessor::prepareToPlay(double sr)
{
	sampleRate = sr;
    delay->setSampleRate(sr);
    numSamplesBeat = blendronomer->aPrep->getBeats()[beatIndex] * sampleRate * ((60.0 / tempo->getTempo()->aPrep->getSubdivisions()) / tempo->getTempo()->aPrep->getTempo());
    blendronomer->aPrep->setDelayLength(numSamplesBeat);
    delay->setDelayTargetLength(numSamplesBeat);
}

void BlendronomerProcessor::playNote(int channel, int note, float velocity)
{
	//may not need anything except communicating with other preparations?
}

void BlendronomerProcessor::updateDelayParameters()
{
    BlendronomerPreparation::Ptr prep = blendronomer->aPrep;
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
        if (pulseRate != tempoPrep->getSubdivisions() * tempoPrep->getTempo() && beatDelta == 0)
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
        if (pulseRate != tempoPrep->getSubdivisions() * tempoPrep->getTempo() && beatDelta == 0)
            smoothRate = INFINITY;//prep->getSmoothDurations()[smoothIndex] / (pulseLength * prep->getBeats()[beatIndex]);
        else
            smoothRate = beatDelta / (prep->getSmoothDurations()[smoothIndex] * pulseLength * prep->getBeats()[beatIndex]);
    }
    pulseRate = tempoPrep->getSubdivisions() * tempoPrep->getTempo();
    
    sampleTimer = 0;
    
    DBG(String(smoothRate));
    delay->setDelayTargetLength(numSamplesBeat);
    delay->setSmoothDuration(smoothRate);
    delay->setFeedback(prep->getFeedbackCoefficients()[feedbackIndex]);
}
