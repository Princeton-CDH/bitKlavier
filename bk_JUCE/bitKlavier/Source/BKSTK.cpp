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
	duration(0.0f),
	rate(0.001f)
{
	state = 0;
}

BKEnvelope::BKEnvelope(float bValue, float bDuration) :
	value(bValue),
	duration(bDuration),
	rate(0.001f)
{
	state = 0;
}

BKEnvelope::~BKEnvelope()
{
}

float BKEnvelope::tick()
{
	if (state == 1) {
		if (duration > value) {
			value += rate;
			if (value >= duration) {
				value = duration;
				state = 0;
			}
		}
		else {
			value -= rate;
			if (value <= duration) {
				value = duration;
				state = 0;
			}
		}
	}

	return value;
}

BKDelayL::BKDelayL() :
	max(4095),
	gain(1.0),
	inPoint(0),
	outPoint(0),
	doNextOut(false)
{
	setLength(0.0);
}

BKDelayL::BKDelayL(float delayLength, float delayMax, float delayGain) :
	max(delayMax),
	gain(delayGain),
	inPoint(0),
	outPoint(0),
	doNextOut(false)
{
	setLength(delayLength);
}

BKDelayL::~BKDelayL()
{
}

inline void BKDelayL::setLength(float delayLength)
{
	float outPointer = inPoint - length;
	length = delayLength;
	while (outPointer < 0) outPointer += inputs.size();

	outPoint = (long)outPointer; //integer part
	alpha = outPointer - outPoint; //fractional part
	omAlpha = (float)1.0 - alpha;
	if (outPoint == inputs.size()) outPoint = 0;
	doNextOut = true;
}

float BKDelayL::nextOut()
{
	if (doNextOut)
	{
		nextOutput = inputs[outPoint] * omAlpha;
		if (outPoint + 1 < inputs.size())
			nextOutput += inputs[outPoint + 1] * alpha;
		else
			nextOutput += inputs[0] * alpha;
		doNextOut = false;
	}

	return nextOutput;
}

//allows addition of samples without incrementing delay position value
void BKDelayL::addSample(float input, unsigned long offset)
{
	inputs.set(inPoint + offset, input * gain);
}

float BKDelayL::tick(float input)
{
	inputs.set(inPoint++, input * gain);
	if (inPoint = inputs.size()) inPoint = 0;

	lastFrame.set(0, nextOut());
	doNextOut = true;

	if (++outPoint == inputs.size()) outPoint = 0;

	return lastFrame[0];
}

void BKDelayL::scalePrevious(float coefficient, unsigned long offset)
{
	if (inputs.size() > 0 && inPoint + offset < inputs.size()) inputs.set((inPoint + offset) % inputs.size(), inputs[inPoint + offset % inputs.size()] * coefficient);
}


BKDelay::BKDelay(BKDelay::Ptr d):
	delayLinear(d->getDelay()),
	dSmooth(d->getDSmooth()),
	dDelayMax(d->getDelayMax()),
	dDelayGain(d->getDelayGain()),
	dDelayLength(d->getDelayLength()),
	dSmoothValue(d->getSmoothValue()),
	dSmoothDuration(d->getSmoothDuration()),
	dBlendronicActive(d->getActive())
{
}

BKDelay::BKDelay(float delayMax, float delayGain, float delayLength, float smoothValue, float smoothDuration, bool active) :
	dDelayMax(delayMax),
	dDelayGain(delayGain),
	dDelayLength(delayLength),
	dSmoothValue(smoothValue),
	dSmoothDuration(smoothDuration),
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
	delayLinear->setMax(dDelayMax);
	delayLinear->setGain(dDelayGain);
	dSmooth->setValue(dSmoothValue);
	dSmooth->setDuration(dSmoothDuration);
}

void BKDelay::updateDelayFromSmooth()
{
	dDelayLength = dSmoothValue;
	delayLinear->setLength(dSmoothValue);
}

void BKDelay::addSample(float sampleToAdd, unsigned long offset)
{
	delayLinear->addSample((stk::StkFloat) sampleToAdd, offset);
}
