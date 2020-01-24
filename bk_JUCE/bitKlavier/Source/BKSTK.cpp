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
	rate(1.0f),
	sampleRate(44100.)
{
	state = 0;
}

BKEnvelope::BKEnvelope(float bValue, float bTarget, double sr) :
	value(bValue),
	target(bTarget),
	rate(1.0f),
	sampleRate(sr)
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
    inPoint(0),
    outPoint(0),
    max(1.),
    length(0.0),
	gain(1.0),
	lastFrameLeft(0),
	lastFrameRight(0),
    feedback(0.9),
	doNextOutLeft(false),
	doNextOutRight(false),
	sampleRate(44100.)
{
	inputs = AudioBuffer<float>(2, max*sampleRate);
	inputs.clear();
	setLength(0.0);
}

BKDelayL::BKDelayL(float delayLength, float delayMax, float delayGain, double sr) :
    inPoint(0),
    outPoint(0),
    max(delayMax),
    length(delayLength),
	gain(delayGain),
	lastFrameLeft(0),
	lastFrameRight(0),
	doNextOutLeft(false),
	doNextOutRight(false),
	sampleRate(sr)
{
	inputs = AudioBuffer<float>(2, max*sampleRate);
	inputs.clear();
	setLength(delayLength);
    feedback = 0.9;
}

BKDelayL::~BKDelayL()
{
}

void BKDelayL::setLength(float delayLength)
{
    length = delayLength;
    float outPointer = inPoint - length;
    if (inputs.getNumSamples() > 0)
        while (outPointer < 0) outPointer += inputs.getNumSamples();
    else outPointer = 0;

	outPoint = outPointer; //integer part
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
    inputs.addSample(channel, (inPoint + offset) % inputs.getNumSamples(), input * gain);
}

//redo so it has channel argument, like addSample, also bool which indicates whether to increment inPoint
//or, have it take float* input
void BKDelayL::tick(float input, float* outputs, bool stereo)
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

	if (++outPoint >= inputs.getNumSamples()) outPoint = 0;

	//feedback
	inputs.addSample(0, inPoint, lastFrameLeft * feedback);
	if (stereo) inputs.addSample(1, inPoint, lastFrameRight * feedback);

	inPoint++;
	if (inPoint == inputs.getNumSamples()) inPoint = 0;

	if (stereo)
	{
		outputs[0] = lastFrameLeft;
		outputs[1] = lastFrameRight;
	}
	else
	{
		outputs[0] = lastFrameLeft;
    }
}

void BKDelayL::scalePrevious(float coefficient, unsigned long offset, int channel)
{
	inputs.setSample(channel, (inPoint + offset) % inputs.getNumSamples(), inputs.getSample(channel, (inPoint + offset) % inputs.getNumSamples()) * coefficient);
}

void BKDelayL::clear()
{
    inputs.clear();
}

void BKDelayL::reset()
{
    inPoint = 0;
    outPoint = 0;
    float outPointer = inPoint - length;
    while (outPointer < 0) outPointer += inputs.getNumSamples();
    
    outPoint = outPointer; //integer part
    alpha = outPointer - outPoint; //fractional part
    omAlpha = (float)1.0 - alpha;
    if (outPoint == inputs.getNumSamples()) outPoint = 0;
    doNextOutLeft = true;
    doNextOutRight = true;
}


BlendronicDelay::BlendronicDelay(BlendronicDelay::Ptr d):
	delayLinear(d->getDelay()),
	dSmooth(d->getDSmooth()),
    dEnv(d->getEnvelope()),
	dDelayMax(d->getDelayMax()),
	dDelayGain(d->getDelayGain()),
	dDelayLength(d->getDelayLength()),
	dSmoothValue(d->getSmoothValue()),
	dSmoothRate(d->getSmoothRate()),
    dInputOpen(d->getInputState()),
    dOutputOpen(d->getOutputState()),
    shouldDuck(d->getShouldDuck()),
    sampleRate(d->getSampleRate())
{
    DBG("Create bdelay");
}

BlendronicDelay::BlendronicDelay(float delayLength, float smoothValue, float smoothRate, float delayMax, double sr, bool active) :
	dDelayMax(delayMax),
	dDelayGain(1.0f),
	dDelayLength(delayLength),
	dSmoothValue(smoothValue),
	dSmoothRate(smoothRate),
    dInputOpen(true),
    dOutputOpen(true),
    sampleRate(sr)
{
	delayLinear =  new BKDelayL(dDelayLength, dDelayMax, dDelayGain, sampleRate);
	dSmooth = new BKEnvelope(dSmoothValue, dDelayLength, sampleRate);
    dSmooth->setRate(dSmoothRate);
    dEnv = new BKEnvelope(1.0f, 1.0f, sampleRate);
    dEnv->setTime(5.0f);
    shouldDuck = false;
    DBG("Create bdelay");
}

BlendronicDelay::~BlendronicDelay()
{
    DBG("Destroy bdelay");
}


void BlendronicDelay::addSample(float sampleToAdd, unsigned long offset, int channel)
{
    if (dInputOpen) delayLinear->addSample(sampleToAdd, offset, channel);
}

void BlendronicDelay::tick(float* outputs)
{
    float dummyOut[2];
    setDelayLength(dSmooth->tick());
    float env = dEnv->tick();
    if (shouldDuck && env == 0.0f)
    {
        clear();
        setEnvelopeTarget(1.0f);
        shouldDuck = false;
    }
    if (dOutputOpen)
    {
        delayLinear->tick(0, outputs, true);
        outputs[0] *= env;
        outputs[1] *= env;
    }
    else delayLinear->tick(0, dummyOut, true);
}

void BlendronicDelay::duckAndClear()
{
    shouldDuck = true;
    setEnvelopeTarget(0.0f);
}
