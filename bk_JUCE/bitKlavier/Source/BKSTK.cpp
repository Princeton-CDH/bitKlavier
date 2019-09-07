/*
  ==============================================================================

    BKSTK.cpp
    Created: 22 Aug 2019 1:24:15pm
    Author:  theot

  ==============================================================================
*/

#include "BKSTK.h"

BKEnvelope::BKEnvelope() :
	value(0.0f),
	target(0.0f),
	rate(1.0f)
{
	state = 0;
}

BKEnvelope::BKEnvelope(float bValue, float bTarget) :
	value(bValue),
	target(bTarget),
	rate(1.0f)
{
	state = 0;
}

BKEnvelope::~BKEnvelope()
{
}

float BKEnvelope::tick()
{
	if (state == 1) {
		if (target > value) {
			value += rate;
			if (value >= target) {
				value = target;
				state = 0;
			}
		}
		else {
			value -= rate;
			if (value <= target) {
				value = target;
				state = 0;
			}
		}
	}

	return value;
}

BKDelayL::BKDelayL() :
	max(4095),
	gain(1.0),
	length(0.0),
	inPoint(0),
	outPoint(0),
	lastFrameLeft(0),
	lastFrameRight(0),
	doNextOutLeft(false),
	doNextOutRight(false)
{
	inputs = AudioBuffer<float>(2, max);
	inputs.clear();
	setLength(0.0);
}

BKDelayL::BKDelayL(float delayLength, float delayMax, float delayGain) :
	max(delayMax),
	gain(delayGain),
	length(delayLength),
	inPoint(0),
	outPoint(0),
	lastFrameLeft(0),
	lastFrameRight(0),
	doNextOutLeft(false),
	doNextOutRight(false)
{
	inputs = AudioBuffer<float>(2, max);
	inputs.clear();
	setLength(delayLength);
}

BKDelayL::~BKDelayL()
{
}

inline void BKDelayL::setLength(float delayLength)
{
	float outPointer = inPoint - length;
	length = delayLength;
	while (outPointer < 0) outPointer += inputs.getNumSamples();

	outPoint = (long)outPointer; //integer part
	alpha = outPointer - outPoint; //fractional part
	omAlpha = (float)1.0 - alpha;
	if (outPoint == inputs.getNumSamples()) outPoint = 0;
	doNextOutLeft = true;
	doNextOutRight = true;
}

float BKDelayL::nextOutLeft()
{
	if (doNextOutLeft)
	{
		nextOutput = inputs.getSample(0, outPoint) * omAlpha;
		if (outPoint + 1 < inputs.getNumSamples())
			nextOutput += inputs.getSample(0, outPoint + 1) * alpha;
		else
			nextOutput += inputs.getSample(0, 0) * alpha;
		doNextOutLeft = false;
	}

	return nextOutput;
}

float BKDelayL::nextOutRight()
{
	if (doNextOutRight)
	{
		nextOutput = inputs.getSample(1, outPoint) * omAlpha;
		if (outPoint + 1 < inputs.getNumSamples())
			nextOutput += inputs.getSample(1, outPoint + 1) * alpha;
		else
			nextOutput += inputs.getSample(1, 0) * alpha;
		doNextOutRight = false;
	}

	return nextOutput;
}

//allows addition of samples without incrementing delay position value
void BKDelayL::addSample(float input, unsigned long offset, int channel)
{
    //simplify? why not: inputs.addSample(channel, (inPoint + offset) % inputs.getNumSamples(), input * gain); ?
    unsigned long dec = 0;
    while (inPoint + (offset - dec) >= inputs.getNumSamples()) dec += inputs.getNumSamples();
    inputs.addSample(channel, inPoint + (offset - dec), input * gain);
	
}

//redo so it has channel argument, like addSample, also bool which indicates whether to increment inPoint
//or, have it take float* input
float* BKDelayL::tick(float input, bool stereo)
{
	inputs.addSample(0, inPoint, input * gain);
	if (stereo) inputs.addSample(1, inPoint, input * gain);

	lastFrameLeft = nextOutLeft();
	doNextOutLeft = true;
	if (stereo)
	{
		lastFrameRight = nextOutRight();
		doNextOutRight = true;
	}

	if (++outPoint == inputs.getNumSamples()) outPoint = 0;

	//feedback
	inputs.addSample(0, inPoint, lastFrameLeft * 0.9);
	if (stereo) inputs.addSample(1, inPoint, lastFrameRight * 0.9); 

	inPoint++;
	if (inPoint == inputs.getNumSamples()) inPoint = 0;

	if (stereo)
	{
		float outs[2];
		outs[0] = lastFrameLeft;
		outs[1] = lastFrameRight;
		return outs;
	}
	else
	{
		float outs[1];
		outs[0] = lastFrameLeft;
		return outs;
	}
}

void BKDelayL::scalePrevious(float coefficient, unsigned long offset, int channel)
{
	inputs.setSample(channel, (inPoint + offset) % inputs.getNumSamples(), inputs.getSample(channel, (inPoint + offset) % inputs.getNumSamples()) * coefficient);
}


BKDelay::BKDelay(BKDelay::Ptr d):
	delayLinear(d->getDelay()),
	dSmooth(d->getDSmooth()),
	dDelayMax(d->getDelayMax()),
	dDelayGain(d->getDelayGain()),
	dDelayLength(d->getDelayLength()),
	dSmoothValue(d->getSmoothValue()),
	dSmoothDuration(d->getSmoothDuration()),
    dId(d->getId()),
	dBlendronicActive(d->getActive())
{
}

BKDelay::BKDelay(float delayMax, float delayGain, float delayLength, float smoothValue, float smoothDuration, int Id, bool active) :
	dDelayMax(delayMax),
	dDelayGain(delayGain),
	dDelayLength(delayLength),
	dSmoothValue(smoothValue),
	dSmoothDuration(smoothDuration),
    dId(Id),
	dBlendronicActive(active)
{
	delayLinear =  new BKDelayL(dDelayLength, dDelayMax, dDelayGain);
	dSmooth = new BKEnvelope(dSmoothValue, dSmoothDuration);
}

BKDelay::~BKDelay()
{
}

void BKDelay::updateValues()
{
	delayLinear->setLength(dDelayLength);
	//delayLinear->setMax(dDelayMax);
	delayLinear->setGain(dDelayGain);
	//dSmooth->setValue(dSmoothValue);
	//dSmooth->setTarget(dSmoothDuration);
}

void BKDelay::updateDelayFromSmooth()
{
	dDelayLength = dSmoothValue;
	delayLinear->setLength(dSmoothValue);
}

void BKDelay::addSample(float sampleToAdd, unsigned long offset, int channel)
{
	delayLinear->addSample(sampleToAdd, offset, channel);
}
