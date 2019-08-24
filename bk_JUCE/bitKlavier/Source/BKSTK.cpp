/*
  ==============================================================================

    BKSTK.cpp
    Created: 22 Aug 2019 1:24:15pm
    Author:  theot

  ==============================================================================
*/

#include "BKSTK.h"

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
	delayLinear = stk::DelayL(dDelayLength, dDelayMax);
	dSmooth = stk::Envelope();
	delayLinear.setGain(dDelayGain);
	dSmooth.setValue(dSmoothValue);
	dSmooth.setTime(dSmoothDuration);
}

void BKDelay::updateValues()
{
	delayLinear.setDelay(dDelayLength);
	delayLinear.setMaximumDelay(dDelayMax);
	delayLinear.setGain(dDelayGain);
	dSmooth.setValue(dSmoothValue);
	dSmooth.setTime(dSmoothDuration);
}

void BKDelay::updateDelayFromSmooth()
{
	dDelayLength = dSmoothValue;
	delayLinear.setDelay(dSmoothValue);
}
