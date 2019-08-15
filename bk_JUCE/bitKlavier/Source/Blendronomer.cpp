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
	bDelayGain(p->getDelayGain()),
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
	Array<float> feedbackCoefficients, Array<float> clickGains, float smoothValue, float smoothDuration) :
	name(newName),
	bBeats(beats),
	bSmoothTimes(smoothTimes),
	bFeedbackCoefficients(feedbackCoefficients),
	bClickGains(clickGains),
	bDelayMax(24000.),
	bDelayGain(0.97),
	bDelayLength(180.),
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
	bDelayGain(0.97),
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
	return false;
}

//assigns random values to a preparation, mainly used for unit testing
inline void BlendronomerPreparation::randomize()
{
}

//prints each variable
void BlendronomerPreparation::print(void)
{
}

//will do loading and storing later
/*ValueTree BlendronomerPreparation::getState(void)
{
	return ValueTree();
}

void BlendronomerPreparation::setState(XmlElement * e)
{
}*/

////////////////////////////////////////////////////////////////////////////////////
/////////////////////////BLENDRONOMER PROCESSOR/////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

BlendronomerProcessor::BlendronomerProcessor(Blendronomer::Ptr blendronomer, TuningProcessor::Ptr tuning, TempoProcessor::Ptr tempo, BKSynthesiser* main, GeneralSettings::Ptr general)
{
}

BlendronomerProcessor::~BlendronomerProcessor()
{
}

void BlendronomerProcessor::processBlock(int numSamples, int midiChannel, BKSampleLoadType type)
{
}

float BlendronomerProcessor::getTimeToBeatMS(float beatsToSkip)
{
	return 0.0f;
}

void BlendronomerProcessor::keyPressed(int midiNoteNumber, float midiNoteVelocity, int midiChannel)
{
}

void BlendronomerProcessor::keyReleased(int midiNoteNumber, float midiVelocity, int midiChannel, bool post)
{
}

void BlendronomerProcessor::postRelease(int midiNoteNumber, int midiChannel)
{
}
