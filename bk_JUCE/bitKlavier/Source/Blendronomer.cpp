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
	Array<float> feedbackCoefficients, float smoothValue, float smoothDuration,
	float delayMax, float delayLength, float feedbackGain) :
	name(newName),
	bBeats(beats),
	bSmoothDurations(smoothTimes),
	bFeedbackCoefficients(feedbackCoefficients),
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
	bDelayMax(4000 * 44.1),
	bFeedbackGain(0.97),
	bDelayLength(800 * 44.1),
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
	feedbackIndex(0)
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
	numSamplesBeat = (uint64)blendronomer->aPrep->getBeats()[beatIndex] * 400 * sampleRate * .001; // should be sampleRate
}

BlendronomerProcessor::~BlendronomerProcessor()
{
    synth->removeBKDelay(delay);
}

void BlendronomerProcessor::processBlock(int numSamples, int midiChannel)
{
	sampleTimer += numSamples;

	if (sampleTimer >= numSamplesBeat)
	{
		beatIndex++;
		if (beatIndex >= blendronomer->aPrep->getBeats().size()) beatIndex = 0;
		smoothIndex++;
		if (smoothIndex >= blendronomer->aPrep->getSmoothDurations().size()) smoothIndex = 0;
		feedbackIndex++;
		if (feedbackIndex >= blendronomer->aPrep->getFeedbackCoefficients().size()) feedbackIndex = 0;

		numSamplesBeat = (uint64) blendronomer->aPrep->getBeats()[beatIndex] * 200 * sampleRate * 0.001;
		sampleTimer = 0;

        blendronomer->aPrep->setDelayLength(numSamplesBeat);
        delay->setDelayTargetLength(numSamplesBeat);
        
        //need to stop through smooth and feedback vals as well
        //delay->setSmoothDuration(50.);
        //delay->setFeedback(....);
        
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
	sampleRate = sr;
    delay->setSampleRate(sr);
}

void BlendronomerProcessor::playNote(int channel, int note, float velocity)
{
	//may not need anything except communicating with other preparations?
}
