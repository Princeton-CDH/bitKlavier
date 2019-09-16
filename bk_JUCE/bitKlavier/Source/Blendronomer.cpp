/*
  ==============================================================================

	Blendronomer.cpp
	Created: 11 Jun 2019 2:00:53pm
	Author:  Theodore R Trevisan

  ==============================================================================
*/

#include "Blendronomer.h"

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
	Array<float> feedbackCoefficients, float smoothValue, BlendronomerSmoothMode smoothMode, float smoothDuration,
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
	bSmoothDurations(Array<float>({ 500. })),
	bFeedbackCoefficients(Array<float>({ 0.95 })),
	bDelayMax(44100. * 5.),
	bDelayLength(44100. * 2.),
	bSmoothValue(180. * 44.1),
	bSmoothDuration(0),
    bSmoothMode(ConstantTimeSmooth),
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
	TempoProcessor::Ptr bTempo, BKDelay::Ptr delayL, GeneralSettings::Ptr bGeneral, BKSynthesiser* bMain):
	blendronomer(bBlendronomer),
	tempo(bTempo),
	delay(delayL),
	synth(bMain),
	general(bGeneral),
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
}

BlendronomerProcessor::~BlendronomerProcessor()
{
    synth->removeBKDelay(delay);
}

void BlendronomerProcessor::processBlock(int numSamples, int midiChannel)
{
    BlendronomerPreparation::Ptr prep = blendronomer->aPrep;
    TempoPreparation::Ptr tempoPrep = tempo->getTempo()->aPrep;
    
	sampleTimer += numSamples;

	if (sampleTimer >= numSamplesBeat)
	{
		beatIndex++;
		if (beatIndex >= prep->getBeats().size()) beatIndex = 0;
		smoothIndex++;
		if (smoothIndex >= prep->getSmoothDurations().size()) smoothIndex = 0;
		feedbackIndex++;
		if (feedbackIndex >= prep->getFeedbackCoefficients().size()) feedbackIndex = 0;
        
        float pulseLength = sampleRate * ((60.0 / tempoPrep->getSubdivisions()) / tempoPrep->getTempo());
        numSamplesBeat = prep->getBeats()[beatIndex] * pulseLength;
        
        float smoothDuration = 0.0f;
        float currBeat = prep->getBeats()[beatIndex];
        float prevBeat = beatIndex > 0 ? prep->getBeats()[beatIndex - 1] : prep->getBeats()[prep->getBeats().size() - 1];
        float beatDelta = fabsf(prevBeat - currBeat);
        if (prep->getSmoothMode()       == ConstantRateSmooth)
        {
            smoothDuration = pulseLength / (prep->getSmoothDurations()[smoothIndex] * 0.001 * sampleRate);
        }
        else if (prep->getSmoothMode()  == ConstantTimeSmooth)
        {
            smoothDuration = (pulseLength / (prep->getSmoothDurations()[smoothIndex] * 0.001 * sampleRate)) * beatDelta;
        }
        else if (prep->getSmoothMode()  == ProportionalRateSmooth)
        {
            smoothDuration = (pulseLength / (prep->getSmoothDurations()[smoothIndex] * 0.001 * sampleRate)) / prep->getBeats()[beatIndex];
        }
        else if (prep->getSmoothMode()  == ProportionalTimeSmooth)
        {
            smoothDuration = ((pulseLength / (prep->getSmoothDurations()[smoothIndex] * 0.001 * sampleRate)) * beatDelta) / prep->getBeats()[beatIndex];
        }
        
		sampleTimer = 0;

        DBG(String(smoothDuration));
        delay->setDelayTargetLength(numSamplesBeat);
        delay->setSmoothDuration(smoothDuration);
        delay->setFeedback(prep->getFeedbackCoefficients()[feedbackIndex]);
	}

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

void BlendronomerProcessor::keyPressed(int noteNumber, float velocity, int midiChannel)
{
    BlendronomerPreparation::Ptr prep = blendronomer->aPrep;
    
    //add note to array of depressed notes
    keysDepressed.addIfNotAlreadyThere(noteNumber);
    velocities.set(noteNumber, velocity);
    holdTimers.set(noteNumber, 0);
    
    if (!velocityCheck(noteNumber)) return;
    
    setSampleTimer(0);
    setBeatIndex(0);
    setSmoothIndex(0);
    setFeedbackIndex(0);
}

void BlendronomerProcessor::keyReleased(int noteNumber, float velocity, int midiChannel, bool post)
{
    BlendronomerPreparation::Ptr prep = blendronomer->aPrep;
    
    //remove key from array of pressed keys
    keysDepressed.removeAllInstancesOf(noteNumber);
    
    if (!velocityCheck(noteNumber)) return;
    if (!holdCheck(noteNumber)) return;
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
