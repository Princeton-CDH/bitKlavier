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
	bTempo(p->getTempo()),
	bBeats(p->getBeats()),
	bSmoothDurations(p->getSmoothDurations()),
	bFeedbackCoefficients(p->getFeedbackCoefficients()),
	bClickGains(p->getClickGains()),
	bDelayMax(p->getDelayMax()),
	bFeedbackGain(p->getFeedbackGain()),
	bDelayLength(p->getDelayLength()),
	bSmoothValue(p->getSmoothValue()),
	bSmoothDuration(p->getSmoothDuration()),
	bInputThresh(p->getInputThreshMS()),
	bInputThreshSec(p->getInputThreshSEC()),
	holdMin(p->getHoldMin()),
	holdMax(p->getHoldMax()),
	velocityMin(p->getVelocityMin()),
	velocityMax(p->getVelocityMax()),
	bNumVoices(p->getNumVoices())
{
}

//constructor with input
BlendronomerPreparation::BlendronomerPreparation(String newName, Array<int> beats, Array<float> smoothTimes,
	Array<float> feedbackCoefficients, Array<float> clickGains, float smoothValue, float smoothDuration, 
	float delayMax, float delayLength, float feedbackGain) :
	name(newName),
	bBeats(beats),
	bSmoothDurations(smoothTimes),
	bFeedbackCoefficients(feedbackCoefficients),
	bClickGains(clickGains),
	bDelayMax(delayMax),
	bFeedbackGain(feedbackGain),
	bDelayLength(delayLength),
	bSmoothValue(smoothValue),
	bSmoothDuration(smoothDuration),
	bInputThresh(1),
	bInputThreshSec(0.001),
	holdMin(0),
	holdMax(12000),
	velocityMin(0),
	velocityMax(127),
	bNumVoices(1)
{
}

//empty constructor
BlendronomerPreparation::BlendronomerPreparation(void) :
	name("blank blendronomer"),
	bBeats(Array<int>({ 4, 3, 2, 3 })),
	bSmoothDurations(Array<float>({ 50., 0. })),
	bFeedbackCoefficients(Array<float>({ 0.97, 0.93 })),
	bClickGains(Array<float>({ 1. })),
	bDelayMax(4000 * 44.1),
	bFeedbackGain(0.97),
	bDelayLength(280 * 44.1),
	bSmoothValue(180. * 44.1),
	bSmoothDuration(0),
	bInputThresh(1),
	bInputThreshSec(0.001),
	holdMin(0),
	holdMax(12000),
	velocityMin(0),
	velocityMax(127),
	bNumVoices(1)
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

BlendronomerProcessor::BlendronomerProcessor(Blendronomer::Ptr bBlendronomer, TuningProcessor::Ptr bTuning, 
	TempoProcessor::Ptr bTempo, BKDelay::Ptr delayL, GeneralSettings::Ptr bGeneral, BKSynthesiser* bMain):
	blendronomer(bBlendronomer),
	tuner(bTuning),
	tempo(bTempo),
	delay(delayL),
	synth(bMain),
	general(bGeneral),
	sampleTimer(0),
	beatIndex(0),
	smoothIndex(0),
	gainIndex(0),
	clickIndex(0)
{
	if (delayL == nullptr)
	{
		delay = synth->addBKDelay(blendronomer->aPrep->getDelayMax(),
			blendronomer->aPrep->getFeedbackGain(),
			blendronomer->aPrep->getDelayLength(),
			blendronomer->aPrep->getSmoothValue(),
			blendronomer->aPrep->getSmoothDuration(),
			true); //currently for testing
	}
	numSamplesBeat = (uint64)blendronomer->aPrep->getBeats()[beatIndex] * 400 * 44.1; // should be sampleRate
}

BlendronomerProcessor::~BlendronomerProcessor()
{
    synth->removeBKDelay(delay);
}

void BlendronomerProcessor::processBlock(int numSamples, int midiChannel)
{
	sampleTimer += numSamples;
	//DBG("sample rate is " + String(sampleRate));
	//DBG("processing blendronomer block, timer is at " + String(sampleTimer) + "out of " + String(numSamplesBeat));
	if (sampleTimer >= numSamplesBeat)
	{
        DBG("blendronomer: beat index = " + String(beatIndex + 1));
		beatIndex++;
		if (beatIndex >= blendronomer->aPrep->getBeats().size()) beatIndex = 0;
		smoothIndex++;
		if (smoothIndex >= blendronomer->aPrep->getSmoothDurations().size()) smoothIndex = 0;
		gainIndex++;
		if (gainIndex >= blendronomer->aPrep->getFeedbackCoefficients().size()) gainIndex = 0;
		clickIndex++;
		if (clickIndex >= blendronomer->aPrep->getClickGains().size()) clickIndex = 0;

		//numSamplesBeat = blendronomer->aPrep->getBeats()[beatIndex] * tempo->getTempo()->aPrep->getBeatThresh() * sampleRate;
		//numSamplesBeat = blendronomer->aPrep->getBeats()[beatIndex] * 0.25 * 44100; //should be samplerate
		numSamplesBeat = (uint64) blendronomer->aPrep->getBeats()[beatIndex] * 200 * 44.1;
		sampleTimer = 0;
        
        DBG("beat length = " + String(numSamplesBeat / 44.1));
        
        updateDelay();
	}
    //updateDelay();
}

float BlendronomerProcessor::getTimeToBeatMS(float beatsToSkip)
{
	uint64 timeToReturn = numSamplesBeat - sampleTimer;
	return timeToReturn * 1000. / sampleRate; //will make more precise later
}

void BlendronomerProcessor::keyPressed(int midiNoteNumber, float midiNoteVelocity, int midiChannel)
{
	//may not need anything except communicating with other preparations?
}

void BlendronomerProcessor::keyReleased(int midiNoteNumber, float midiVelocity, int midiChannel, bool post)
{
	//may not need anything except communicating with other preparations?
}

void BlendronomerProcessor::postRelease(int midiNoteNumber, int midiChannel)
{
	//may not need anything except communicating with other preparations?
}

void BlendronomerProcessor::prepareToPlay(double sr)
{
	sampleRate = sr;
}

void BlendronomerProcessor::playNote(int channel, int note, float velocity)
{
	//may not need anything except communicating with other preparations?
}

void BlendronomerProcessor::updateDelay()
{
	//delay->setDelayLength(blendronomer->aPrep->getDelayLength());
	//delay->setDelayMax(blendronomer->aPrep->getDelayMax());
    
	//delay->setDelayLength(blendronomer->aPrep->getBeats()[beatIndex] * 200 * 44.1);
	//blendronomer->aPrep->setDelayLength(blendronomer->aPrep->getBeats()[beatIndex] * 200 * 44.1);
	
    //blendronomer->aPrep->getBeats()[beatIndex] * 200 * 44.1
    delay->setDelayLength(numSamplesBeat);
    blendronomer->aPrep->setDelayLength(numSamplesBeat);
    
    //delay->setDelayGain(blendronomer->aPrep->getFeedbackCoefficients()[gainIndex]);
	//delay->setSmoothDuration(blendronomer->aPrep->getSmoothDurations()[smoothIndex]);
	//delay->setSmoothValue(numSamplesBeat);
	//delay->setActive(blendronomer->aPrep->getActive());
}
