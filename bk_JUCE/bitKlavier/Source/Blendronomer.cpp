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
	bFeedbackCoefficient(p->getFeedbackCoefficient()),
	bDelayLength(p->getDelayLength()),
	bSmoothValue(p->getSmoothValue()),
	bSmoothDuration(p->getSmoothDuration()),
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
	Array<float> feedbackCoefficients, float smoothValue, float smoothDuration,
	float delayMax, float delayLength, float feedbackCoefficient) :
	name(newName),
	bBeats(beats),
	bSmoothDurations(smoothTimes),
	bFeedbackCoefficients(feedbackCoefficients),
	bDelayMax(delayMax),
	bFeedbackCoefficient(feedbackCoefficient),
	bDelayLength(delayLength),
	bSmoothValue(smoothValue),
	bSmoothDuration(smoothDuration),
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
	bSmoothDurations(Array<float>({ 50. })),
	bFeedbackCoefficients(Array<float>({ 0.95 })),
	bDelayMax(44100. * 5.),
	bFeedbackCoefficient(0.97),
	bDelayLength(44100. * 2.),
	bSmoothValue(180. * 44.1),
	bSmoothDuration(0),
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
	if (delayL == nullptr)
	{
		delay = synth->addBKDelay(blendronomer->aPrep->getDelayMax(),
			blendronomer->aPrep->getFeedbackCoefficient(),
			blendronomer->aPrep->getDelayLength(),
			blendronomer->aPrep->getSmoothValue(),
			blendronomer->aPrep->getSmoothDuration(),
			true);
	}
	numSamplesBeat = blendronomer->aPrep->getBeats()[beatIndex] * sampleRate * ((60.0 / tempo->getTempo()->aPrep->getSubdivisions()) / tempo->getTempo()->aPrep->getTempo());
    blendronomer->aPrep->setDelayLength(numSamplesBeat);
    delay->setDelayTargetLength(numSamplesBeat);
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
        
        numSamplesBeat = prep->getBeats()[beatIndex] * sampleRate * ((60.0 / tempoPrep->getSubdivisions()) / tempoPrep->getTempo());
		sampleTimer = 0;

        prep->setDelayLength(numSamplesBeat);
        prep->setSmoothDuration(prep->getSmoothDurations()[smoothIndex]);
        prep->setFeedback(prep->getFeedbackCoefficients()[feedbackIndex]);
        
        delay->setDelayTargetLength(numSamplesBeat);
        delay->setSmoothDuration(prep->getSmoothDurations()[smoothIndex]);
        delay->setFeedback(prep->getFeedbackCoefficients()[feedbackIndex]);
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
