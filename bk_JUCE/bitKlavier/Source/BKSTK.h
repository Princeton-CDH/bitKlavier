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
   bitKlavier replacement for STK DelayL - limited implementation
////////////////////////////////////////////////////////////////////////////////
*/

class BKDelayL : public ReferenceCountedObject
{
public:
	typedef ReferenceCountedObjectPtr<BKDelayL>   Ptr;
	typedef Array<BKDelayL::Ptr>                  PtrArr;
	typedef Array<BKDelayL::Ptr, CriticalSection> CSPtrArr;
	typedef OwnedArray<BKDelayL>                  Arr;
	typedef OwnedArray<BKDelayL, CriticalSection> CSArr;

	//constructors
	BKDelayL();
	BKDelayL(float delayLength, float delayMax, float delayGain);
	~BKDelayL();

	//accessors
	inline const float getLength() const noexcept { return length; }
	inline const float getMax() const noexcept { return max; }
	inline const float getGain() const noexcept { return gain; }
	inline const float lastOut() const noexcept { return lastFrame[0]; }

	//mutators
	inline void setLength(float delayLength);
	inline void setMax(float delayMax) { max = delayMax; }
	inline void setGain(float delayGain) { gain = delayGain; }

	float nextOut();
	void addSample(float input, unsigned long offset);
	float tick(float input);
	void scalePrevious(float coefficient, unsigned long offset);

private:
	Array<float> inputs;
	Array<float> lastFrame;
	unsigned long inPoint;
	unsigned long outPoint;
	float length;
	float max;
	float gain;
	float alpha;
	float omAlpha;
	float nextOutput;
	bool doNextOut;
};

/*
////////////////////////////////////////////////////////////////////////////////
   bitKlavier replacement for STK envelope - limited implementation
////////////////////////////////////////////////////////////////////////////////
*/

class BKEnvelope : public ReferenceCountedObject
{
public:
	typedef ReferenceCountedObjectPtr<BKEnvelope>   Ptr;
	typedef Array<BKEnvelope::Ptr>                  PtrArr;
	typedef Array<BKEnvelope::Ptr, CriticalSection> CSPtrArr;
	typedef OwnedArray<BKEnvelope>                  Arr;
	typedef OwnedArray<BKEnvelope, CriticalSection> CSArr;

	//constructors
	BKEnvelope();
	BKEnvelope(float bValue, float bDuration);
	~BKEnvelope();

	//accessors
	inline const float getValue() const noexcept { return value; }
	inline const float getDuration() const noexcept { return duration; }
	inline const int getState() const noexcept { return state; }

	//mutators
	inline void setValue(float envelopeValue) { value = envelopeValue; }
	inline void setDuration(float envelopeDuration) { duration = envelopeDuration; }
	inline void setRate(float sr) { rate = sr; }

	float tick();

private:
	float value;
	float duration;
	float rate;
	int state;
};

/*
////////////////////////////////////////////////////////////////////////////////
   this will eventually incorporate the replacement for the STK classes but for
   now it's going to be a wrapper of the delay line and related variables
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
	~BKDelay();

	//accessors
    inline const BKDelayL::Ptr getDelay() const noexcept { return delayLinear; }
	inline const BKEnvelope::Ptr getDSmooth() const noexcept { return dSmooth; }
	inline const float getDelayMax() const noexcept { return dDelayMax; }
	inline const float getDelayGain() const noexcept { return dDelayGain; }
	inline const float getDelayLength() const noexcept { return dDelayLength; }
	inline const float getSmoothValue() const noexcept { return dSmoothValue; }
	inline const float getSmoothDuration() const noexcept { return dSmoothDuration; }
	inline const bool getActive() const noexcept { return dBlendronicActive; }

	//mutators
    void updateValues();
    void updateDelayFromSmooth();
	void addSample(float sampleToAdd, unsigned long offset); //adds input sample into the delay line (first converted to stkFloat)
	inline void setDelayMax(float delayMax) { dDelayMax = delayMax; }
	inline void setDelayGain(float delayGain) { dDelayGain = delayGain; }
	inline void setDelayLength(float delayLength) { dDelayLength = delayLength; }
	inline void setSmoothValue(float smoothValue) { dSmoothValue = smoothValue; }
	inline void setSmoothDuration(float smoothDuration) { dSmoothDuration = smoothDuration; }
	inline const void setActive(bool newActive) { dBlendronicActive = newActive; }
	inline const void toggleActive() { dBlendronicActive = !dBlendronicActive; }

private:
    BKDelayL::Ptr delayLinear;
	BKEnvelope::Ptr dSmooth;
	float dDelayLength;
	float dDelayMax;
	float dDelayGain;
	float dSmoothValue;
	float dSmoothDuration;
	bool dBlendronicActive;
};

#endif
