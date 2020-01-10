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
    sampleRate = 44100.;
}

BKEnvelope::BKEnvelope(float bValue, float bTarget) :
	value(bValue),
	target(bTarget),
	rate(1.0f)
{
	state = 0;
    sampleRate = 44100.;
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
    max(4096),
    length(0.0),
	gain(1.0),
	lastFrameLeft(0),
	lastFrameRight(0),
    feedback(0.9),
	doNextOutLeft(false),
	doNextOutRight(false)
{
	inputs = AudioBuffer<float>(2, max);
	inputs.clear();
	setLength(0.0);
    sampleRate = 44100.;
}

BKDelayL::BKDelayL(float delayLength, float delayMax, float delayGain) :
    inPoint(0),
    outPoint(0),
    max(delayMax),
    length(delayLength),
	gain(delayGain),
	lastFrameLeft(0),
	lastFrameRight(0),
	doNextOutLeft(false),
	doNextOutRight(false)
{
	inputs = AudioBuffer<float>(2, max);
	inputs.clear();
	setLength(delayLength);
    sampleRate = 44100.;
    feedback = 0.9;
}

BKDelayL::~BKDelayL()
{
}

void BKDelayL::setLength(float delayLength)
{
    length = delayLength;
    float outPointer = inPoint - length;
	while (outPointer < 0) outPointer += inputs.getNumSamples();

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
	dSmoothDuration(d->getSmoothDuration()),
	dBlendronicActive(d->getActive()),
    dInputOpen(d->getInputState()),
    dOutputOpen(d->getOutputState()),
    shouldDuck(d->getShouldDuck()),
    sampleRate(d->getSampleRate())
{
    delayLengthRecord.ensureStorageAllocated(dDelayMax);
    for (int i = 0; i < dDelayMax; i++)
    {
        delayLengthRecord.add(0.0f);
    }
    delayLengthRecordInPoint = 0;
    DBG("Create bdelay");
}

BlendronicDelay::BlendronicDelay(float delayLength, float smoothValue, float smoothDuration, float delayGain, float delayMax, bool active) :
	dDelayMax(delayMax),
	dDelayGain(delayGain),
	dDelayLength(delayLength),
	dSmoothValue(smoothValue),
	dSmoothDuration(smoothDuration),
	dBlendronicActive(active),
    dInputOpen(true),
    dOutputOpen(true),
    sampleRate(44100)
{
	delayLinear =  new BKDelayL(dDelayLength, dDelayMax, dDelayGain);
    delayLengthRecord.ensureStorageAllocated(dDelayMax);
    for (int i = 0; i < dDelayMax; i++)
    {
        delayLengthRecord.add(0.0f);
    }
    delayLengthRecordInPoint = 0;
	dSmooth = new BKEnvelope(dSmoothValue, delayLength);
    dSmooth->setRate(dSmoothDuration);
    dEnv = new BKEnvelope(1.0f, 1.0f);
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
    if (dBlendronicActive)
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
    delayLengthRecord.set(delayLengthRecordInPoint++, dDelayLength/sampleRate);
    if (delayLengthRecordInPoint >= delayLengthRecord.size()) delayLengthRecordInPoint = 0;
}

void BlendronicDelay::duckAndClear()
{
    shouldDuck = true;
    setEnvelopeTarget(0.0f);
}
