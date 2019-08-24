/*
  ==============================================================================

    BKSTK.h
    Created: 22 Aug 2019 1:24:15pm
    Author:  Theodore R Trevisan

  ==============================================================================
*/

#ifndef BKSTK_INCLUDED
#define BKSTK_INCLUDED

#include "BKUtilities.h"
#pragma once

/*
////////////////////////////////////////////////////////////////////////////////
   this will eventually incorporate the replacement for the STK classes but for
   now it's going to be a wrapper of the delay line-related variables with an ID
   that will match the blendronomer's ID
////////////////////////////////////////////////////////////////////////////////
*/

class BKDelay : public ReferenceCountedObject
{
public:
	typedef ReferenceCountedObjectPtr<BKDelay>   Ptr;
	typedef Array<BKDelay::Ptr>                  PtrArr;
	typedef Array<BKDelay::Ptr, CriticalSection> CSPtrArr;
	typedef OwnedArray<BKDelay>                  Arr;
	typedef OwnedArray<BKDelay, CriticalSection> CSArr;

	//constructors
	BKDelay(BKDelay::Ptr d);
	BKDelay(float delayMax, float delayGain, float delayLength, float smoothValue, float smoothDuration, bool active = false);

	//accessors
	inline stk::DelayL getDelay() const noexcept { return delayLinear; }
	inline stk::Envelope getDSmooth() const noexcept { return dSmooth; }
	inline const float getDelayMax() const noexcept { return dDelayMax; }
	inline const float getDelayGain() const noexcept { return dDelayGain; }
	inline const float getDelayLength() const noexcept { return dDelayLength; }
	inline const float getSmoothValue() const noexcept { return dSmoothValue; }
	inline const float getSmoothDuration() const noexcept { return dSmoothDuration; }
	inline const bool getActive() const noexcept { return dBlendronicActive; }

	//mutators
	inline void updateValues();
    inline void updateDelayFromSmooth() { delayLinear = dSmoothValue; }
	inline void setDelayMax(float delayMax) { dDelayMax = delayMax; }
	inline void setDelayGain(float delayGain) { dDelayGain = delayGain; }
	inline void setDelayLength(float delayLength) { dDelayLength = delayLength; }
	inline void setSmoothValue(float smoothValue) { dSmoothValue = smoothValue; }
	inline void setSmoothDuration(float smoothDuration) { dSmoothDuration = smoothDuration; }
	inline const void setActive(bool newActive) { dBlendronicActive = newActive; }
	inline const void toggleActive() { dBlendronicActive = !dBlendronicActive; }

private:
	stk::DelayL delayLinear;
	stk::Envelope dSmooth;
	float dDelayLength;
	float dDelayMax;
	float dDelayGain;
	float dSmoothValue;
	float dSmoothDuration;
	bool dBlendronicActive;
};

#endif
