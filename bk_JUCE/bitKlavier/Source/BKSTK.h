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
	inline const float lastOutLeft() const noexcept { return lastFrameLeft; }
	inline const float lastOutRight() const noexcept { return lastFrameRight; }

	//mutators
	inline void setLength(float delayLength);
	inline void setMax(float delayMax) { max = delayMax; }
	inline void setGain(float delayGain) { gain = delayGain; }
    inline void setFeedback(float fb) { feedback = fb; }

	float nextOutLeft();
	float nextOutRight();
	void addSample(float input, unsigned long offset, int channel);
	float* tick(float input, bool stereo = true);
	void scalePrevious(float coefficient, unsigned long offset, int channel);
    
    inline void setSampleRate(double sr) { sampleRate = sr; }

private:
	AudioBuffer<float> inputs;
	float lastFrameLeft;
	float lastFrameRight;
	unsigned long inPoint;
	unsigned long outPoint;
	float length;
	float max;
	float gain;
	float alpha;
	float omAlpha;
    float feedback;
	float nextOutput;
	bool doNextOutLeft;
	bool doNextOutRight;
    
    double sampleRate;
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
	inline const float getTarget() const noexcept { return target; }
	inline const int getState() const noexcept { return state; }

	//mutators
	inline void setValue(float envelopeValue) { value = envelopeValue; }
    inline void setTarget(float envelopeTarget) { target = envelopeTarget; if ( target != value ) state = 1; DBG("new envelope target = " + String(target));}
	inline void setRate(float sr) { rate = sr; }
    inline void setTime(float time) { rate = 1.0 / ( time * sampleRate * 0.001 ); } // time in ms for envelope to go from 0-1. need to update for sampleRate
    inline void setSampleRate(double sr) { sampleRate = sr; }

	float tick();

private:
	float value;
	float target;
	float rate;
	int state;
    
    double sampleRate;
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
	BKDelay(float delayMax, float delayGain, float delayLength, float smoothValue, float smoothDuration, int Id, bool active = false);
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
    inline const int getId() const noexcept { return dId; }

	//mutators
    void updateValues();
    void updateDelayFromSmooth();
	void addSample(float sampleToAdd, unsigned long offset, int channel); //adds input sample into the delay line (first converted to stkFloat)
	inline void setDelayMax(float delayMax) { dDelayMax = delayMax; }
	inline void setDelayGain(float delayGain) { dDelayGain = delayGain; }
	inline void setDelayLength(float delayLength) { dDelayLength = delayLength; delayLinear->setLength(delayLength); }
    inline void setDelayTargetLength(float delayLength) { dSmooth->setTarget(delayLength); }
	inline void setSmoothValue(float smoothValue) { dSmoothValue = smoothValue; }
    inline void setSmoothDuration(float smoothDuration) { dSmoothDuration = smoothDuration; }
    inline void setFeedback(float fb) { delayLinear->setFeedback(fb); }
	inline const void setActive(bool newActive) { dBlendronicActive = newActive; }
	inline const void toggleActive() { dBlendronicActive = !dBlendronicActive; }
    inline const void setId(int Id) { dId = Id; }
    
    float* tick();
    
    inline void setSampleRate(double sr) { delayLinear->setSampleRate(sr); dSmooth->setSampleRate(sr); }
    
    

private:
    BKDelayL::Ptr delayLinear;
	BKEnvelope::Ptr dSmooth;
	float dDelayMax;
	float dDelayGain;
    float dDelayLength;
	float dSmoothValue;
	float dSmoothDuration;
    int dId;
	bool dBlendronicActive;
};

#endif
