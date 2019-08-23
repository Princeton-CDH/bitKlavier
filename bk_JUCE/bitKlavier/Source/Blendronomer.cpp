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
	bSmoothTimes(p->getSmoothTimes()),
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
	bSmoothTimes(smoothTimes),
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
	bBeats(Array<int>({ 4, 3, 2 })),
	bSmoothTimes(Array<float>({ 50., 0. })),
	bFeedbackCoefficients(Array<float>({ 0.97, 0.93 })),
	bClickGains(Array<float>({ 1. })),
	bDelayMax(24000.),
	bFeedbackGain(0.97),
	bDelayLength(180.),
	bSmoothValue(180),
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

//assigns random values to a preparation, mainly used for unit testing
inline void BlendronomerPreparation::randomize()
{
	return;
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
}

BlendronomerProcessor::~BlendronomerProcessor()
{
}

void BlendronomerProcessor::processBlock(int numSamples, int midiChannel)
{
	sampleTimer += numSamples;
	if (sampleTimer >= numSamplesBeat)
	{
		beatIndex++;
		if (beatIndex > blendronomer->aPrep->getBeats().size()) beatIndex = 0;
		smoothIndex++;
		if (smoothIndex > blendronomer->aPrep->getSmoothTimes().size()) smoothIndex = 0;
		gainIndex++;
		if (gainIndex > blendronomer->aPrep->getFeedbackCoefficients().size()) gainIndex = 0;
		clickIndex++;
		if (clickIndex > blendronomer->aPrep->getClickGains().size()) clickIndex = 0;
		numSamplesBeat = blendronomer->aPrep->getBeats()[beatIndex];
		sampleTimer = 0;
	}
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
	//TBD?
}

void BlendronomerProcessor::playNote(int channel, int note, float velocity)
{
	//may not need anything except communicating with other preparations?
}

void BlendronomerProcessor::updateDelay()
{
	delay->setDelayLength(blendronomer->aPrep->getDelayLength());
	delay->setDelayMax(blendronomer->aPrep->getDelayMax());
	delay->setDelayGain(blendronomer->aPrep->getFeedbackGain());
	delay->setSmoothDuration(blendronomer->aPrep->getSmoothDuration());
	delay->setSmoothValue(blendronomer->aPrep->getSmoothValue());
	delay->setActive(blendronomer->aPrep->getActive());
}
