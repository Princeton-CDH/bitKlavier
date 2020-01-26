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
	BKDelayL(float delayLength, float delayMax, float delayGain, double sr);
	~BKDelayL();

	//accessors
	inline const float getLength() const noexcept { return length; }
	inline const float getMax() const noexcept { return max; }
	inline const float getGain() const noexcept { return gain; }
	inline const float lastOutLeft() const noexcept { return lastFrameLeft; }
	inline const float lastOutRight() const noexcept { return lastFrameRight; }
    
    inline const AudioBuffer<float> getBuffer() const noexcept { return inputs; }

	//mutators
    void setLength(float delayLength);
	inline void setMax(float delayMax) { max = delayMax; }
	inline void setGain(float delayGain) { gain = delayGain; }
    inline void setFeedback(float fb) { feedback = fb; }
    inline unsigned long getInPoint() { return inPoint; }
    inline unsigned long getOutPoint() { return outPoint; }


	float nextOutLeft();
	float nextOutRight();
	void addSample(float input, unsigned long offset, int channel);
	void tick(float input, float* outputs, float outGain, bool stereo = true);
	void scalePrevious(float coefficient, unsigned long offset, int channel);
    void clear();
    void reset();
    
    inline void setSampleRate(double sr) { sampleRate = sr; }

private:
	AudioBuffer<float> inputs;
	int inPoint;
	int outPoint;
    float max;
	float length;
	float gain;
    float lastFrameLeft;
    float lastFrameRight;
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
	BKEnvelope(float bValue, float bDuration, double sr);
	~BKEnvelope();

	//accessors
	inline const float getValue() const noexcept { return value; }
	inline const float getTarget() const noexcept { return target; }
	inline const int getState() const noexcept { return state; }

	//mutators
	inline void setValue(float envelopeValue) { value = envelopeValue; }
    inline void setTarget(float envelopeTarget) { target = envelopeTarget; if ( target != value ) state = 1; }
	inline void setRate(float sr) { rate = sr; }
    inline void setTime(float time) { rate = 1.0 / ( time * sampleRate * 0.001 ); DBG("new rate = " + String(rate));} // time in ms for envelope to go from 0-1. need to update for sampleRate
    inline void setSampleRate(double sr) { sampleRate = sr; }
    
    //! Set target = 1.
    void keyOn( void ) { this->setTarget( 1.0 ); };

    //! Set target = 0.
    void keyOff( void ) { this->setTarget( 0.0 ); };

	float tick();
    float lastOut() { return lastvalue; }

private:
	float value;
    float lastvalue;
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

class BlendronicDelay : public ReferenceCountedObject
{
public:
	typedef ReferenceCountedObjectPtr<BlendronicDelay>   Ptr;
	typedef Array<BlendronicDelay::Ptr>                  PtrArr;
	typedef Array<BlendronicDelay::Ptr, CriticalSection> CSPtrArr;
	typedef OwnedArray<BlendronicDelay>                  Arr;
	typedef OwnedArray<BlendronicDelay, CriticalSection> CSArr;

	//constructors
	BlendronicDelay(BlendronicDelay::Ptr d);
	BlendronicDelay(float delayLength, float smoothValue, float smoothDuration, float delayMax, double sr, bool active = false);
	~BlendronicDelay();

	//accessors
    inline const BKDelayL::Ptr getDelay() const noexcept { return delayLinear; }
	inline const BKEnvelope::Ptr getDSmooth() const noexcept { return dSmooth; }
    inline const BKEnvelope::Ptr getEnvelope() const noexcept { return dEnv; }
	inline const float getDelayMax() const noexcept { return dDelayMax; }
	inline const float getDelayGain() const noexcept { return dDelayGain; }
	inline const float getDelayLength() const noexcept { return dDelayLength; }
	inline const float getSmoothValue() const noexcept { return dSmooth->getValue(); }
	inline const float getSmoothRate() const noexcept { return dSmoothRate; }
    inline const bool getInputState() const noexcept { return dInputOpen; }
    inline const bool getOutputState() const noexcept { return dOutputOpen; }
    inline const bool getShouldDuck() const noexcept { return shouldDuck; }
    inline const AudioBuffer<float> getDelayBuffer() const noexcept { return delayLinear->getBuffer(); }
    inline const unsigned long getCurrentSample() const noexcept { return delayLinear->getInPoint(); }
    inline const unsigned long getDelayedSample() const noexcept { return delayLinear->getOutPoint(); }

	//mutators
	void addSample(float sampleToAdd, unsigned long offset, int channel); //adds input sample into the delay line (first converted to float)
	inline void setDelayMax(float delayMax) { dDelayMax = delayMax; }
	inline void setDelayGain(float delayGain) { dDelayGain = delayGain; }
	inline void setDelayLength(float delayLength) { dDelayLength = delayLength; delayLinear->setLength(delayLength); }
    inline void setDelayTargetLength(float delayLength) { dSmooth->setTarget(delayLength); }
	inline void setSmoothValue(float smoothValue)
    {
        dSmoothValue = smoothValue;
        dSmooth->setValue(dSmoothValue);
    }
    inline void setEnvelopeTarget(float t) { dEnv->setTarget(t); }
    
    //we want to be able to do this two ways:
    //set a duration for the delay length changes that will be constant, so at the beginning of
    //  each beat we will need to calculate a new rate dependent on this duration and the beat length (rate ~ beatLength / duration)
    //have the rate be constant, regardless of beat length, so we'll use the length of smallest beat (1, as set by Tempo, so the pulseLength)
    //  so rate ~ pulseLength / duration
    inline void setSmoothRate(float smoothRate)
    {
        dSmoothRate = smoothRate;
        dSmooth->setRate(smoothRate);
    }
    
    inline void setFeedback(float fb) { delayLinear->setFeedback(fb); }
    inline const void setInputState(bool inputState) { dInputOpen = inputState; }
    inline const void toggleInput() { dInputOpen = !dInputOpen; }
    inline const void setOutputState(bool outputState) { dOutputOpen = outputState; }
    inline const void toggleOutput() { dOutputOpen = !dOutputOpen; }
    
    void tick(float* outputs, float outGain);
    
    void duckAndClear();
    
    inline void setSampleRate(double sr) { sampleRate = sr; delayLinear->setSampleRate(sr); dSmooth->setSampleRate(sr); }
    inline double getSampleRate() { return sampleRate; }
    inline void clear() { delayLinear->clear(); /*delayLinear->reset();*/ }

private:
    BKDelayL::Ptr delayLinear;
	BKEnvelope::Ptr dSmooth;
    BKEnvelope::Ptr dEnv;
	float dDelayMax;
	float dDelayGain;
    float dDelayLength;
	float dSmoothValue;
	float dSmoothRate;
    bool dInputOpen;
    bool dOutputOpen;
    bool shouldDuck;
    
    double sampleRate;
};

/***************************************************/
/*! \class ADSR
    \brief STK ADSR envelope class.

    This class implements a traditional ADSR (Attack, Decay, Sustain,
    Release) envelope.  It responds to simple keyOn and keyOff
    messages, keeping track of its state.  The \e state = ADSR::IDLE
    before being triggered and after the envelope value reaches 0.0 in
    the ADSR::RELEASE state.  All rate, target and level settings must
    be non-negative.  All time settings must be positive.

    by Perry R. Cook and Gary P. Scavone, 1995-2011.
*/
/***************************************************/

class BKADSR
{
 public:

  //! ADSR envelope states.
  enum {
    ATTACK,   /*!< Attack */
    DECAY,    /*!< Decay */
    SUSTAIN,  /*!< Sustain */
    RELEASE,  /*!< Release */
    IDLE      /*!< Before attack / after release */
  };

  //! Default constructor.
  BKADSR( void );

  //! Class destructor.
  ~BKADSR( void );

  //! Set target = 1, state = \e ADSR::ATTACK.
  void keyOn( void );

  //! Set target = 0, state = \e ADSR::RELEASE.
  void keyOff( void );

  //! Set the attack rate.
  void setAttackRate( float rate );

  //! Set the target value for the attack (default = 1.0).
  void setAttackTarget( float target );

  //! Set the decay rate.
  void setDecayRate( float rate );

  //! Set the sustain level.
  void setSustainLevel( float level );

  //! Set the release rate.
  void setReleaseRate( float rate );

  //! Set the attack rate based on a time duration.
  void setAttackTime( float time );

  //! Set the decay rate based on a time duration (seconds).
  void setDecayTime( float time );

  //! Set the release rate based on a time duration (seconds).
  void setReleaseTime( float time );
    float  getReleaseTime( void );

  //! Set sustain level and attack, decay, and release time durations.
  void setAllTimes( float aTime, float dTime, float sLevel, float rTime );

  //! Set a sustain target value and attack or decay from current value to target.
  void setTarget( float target );

  //! Return the current envelope \e state (ATTACK, DECAY, SUSTAIN, RELEASE, IDLE).
  int getState( void ) const { return state_; };

  //! Set to state = ADSR::SUSTAIN with current and target values of \e value.
  void setValue( float value );
    
    float getAttackRate() {return attackRate_; };
    float getDecayRate() {return decayRate_; };
    float getSustainLevel() {return sustainLevel_; };
    float getReleaseRate() {return releaseRate_; };

    //! Return the last computed output value.
    float lastOut( void ) const { return lastvalue_; };

    //! Compute and return one output sample.
    float tick( void );
    
    void setSampleRate(double sr)
    {
        sampleRateChanged(sr, sampleRate);
        sampleRate = sr;
    }

 protected:

  void sampleRateChanged( float newRate, float oldRate );

  int state_;
  float value_;
  float lastvalue_;
  float target_;
  float attackRate_;
  float decayRate_;
  float releaseRate_;
  float releaseTime_;
  float sustainLevel_;
    
  double sampleRate;
};


inline float BKADSR :: tick( void )
{
  switch ( state_ ) {

  case ATTACK:
    value_ += attackRate_;
    if ( value_ >= target_ ) {
      value_ = target_;
      target_ = sustainLevel_;
        state_ = DECAY;
    }
    lastvalue_ = value_;
    break;

  case DECAY:
    if ( value_ > sustainLevel_ ) {
      value_ -= decayRate_;
      if ( value_ <= sustainLevel_ ) {
        value_ = sustainLevel_;
        state_ = SUSTAIN;
      }
    }
    else {
      value_ += decayRate_; // attack target < sustain level
      if ( value_ >= sustainLevel_ ) {
        value_ = sustainLevel_;
        state_ = SUSTAIN;
      }
    }
    lastvalue_ = value_;
    break;

  case RELEASE:
    value_ -= releaseRate_;
    if ( value_ <= 0.0 ) {
      value_ = 0.0;
      state_ = IDLE;
    }
    lastvalue_ = value_;

  }

  return value_;
}


/***************************************************/
/*! \class ADHSR
    \brief STK ADHSR envelope class.
 */
/***************************************************/

class BKAHDSR
{
public:
    
    //! AHDSR envelope states.
    enum {
        ATTACK,   /*!< Attack */
        HOLD,     /*!< Hold */
        DECAY,    /*!< Decay */
        SUSTAIN,  /*!< Sustain */
        RELEASE,  /*!< Release */
        IDLE      /*!< Before attack / after release */
    };
    
    //! Default constructor.
    BKAHDSR( void );
    
    //! Class destructor.
    ~BKAHDSR( void );
    
    //! Set target = 1, state = \e AHDSR::ATTACK.
    void keyOn( void );
    
    //! Set target = 0, state = \e AHDSR::RELEASE.
    void keyOff( void );
    
    //! Set the attack rate.
    void setAttackRate( float rate );
    
    //! Set the target value for the attack (default = 1.0).
    void setAttackTarget( float target );
    
    //! Set the hold time.
    void setHoldTime( float time );
    
    //! Set the decay rate.
    void setDecayRate( float rate );
    
    //! Set the sustain level.
    void setSustainLevel( float level );
    
    //! Set the release rate.
    void setReleaseRate( float rate );
    
    //! Set the attack rate based on a time duration.
    void setAttackTime( float time );
    
    //! Set the decay rate based on a time duration (seconds).
    void setDecayTime( float time );
    
    //! Set the release rate based on a time duration (seconds).
    void setReleaseTime( float time );
    float  getReleaseTime( void );
    
    //! Set sustain level and attack, decay, and release time durations.
    void setAllTimes( float aTime, float hTime, float dTime, float sLevel, float rTime );
    
    //! Set a sustain target value and attack or decay from current value to target.
    void setTarget( float target );
    
    //! Return the current envelope \e state (ATTACK, HOLD, DECAY, SUSTAIN, RELEASE, IDLE).
    int getState( void ) const { return state_; };
    
    //! Set to state = ADSR::SUSTAIN with current and target values of \e value.
    void setValue( float value );
    
    float getAttackRate() {return attackRate_; };
    float getHoldSamples() {return holdSamples_; };
    float getDecayRate() {return decayRate_; };
    float getSustainLevel() {return sustainLevel_; };
    float getReleaseRate() {return releaseRate_; };
    
    //! Return the last computed output value.
    float lastOut( void ) const { return lastvalue_; };
    
    //! Compute and return one output sample.
    float tick( void );
    
    void setSampleRate(double sr)
    {
        sampleRateChanged(sr, sampleRate);
        sampleRate = sr;
    }

    
protected:
    
    void sampleRateChanged( float newRate, float oldRate );
    
    int state_;
    double holdSamples_;
    double held_;
    float value_;
    float lastvalue_;
    float target_;
    float attackRate_;
    float decayRate_;
    float releaseRate_;
    float releaseTime_;
    float sustainLevel_;
    
    double sampleRate;
};

inline float BKAHDSR :: tick( void )
{
    switch ( state_ ) {
            
        case ATTACK:
            value_ += attackRate_;
            if ( value_ >= target_ ) {
                value_ = target_;
                state_ = HOLD;
                held_ = 0;
            }
            lastvalue_ = value_;
            break;
            
        case HOLD:
            held_ ++;
            value_ = target_;
            if ( held_ >= holdSamples_) {
                    state_ = DECAY;
                    target_ = sustainLevel_;
            }
            lastvalue_ = value_;
            break;
            
        case DECAY:
            if ( value_ > sustainLevel_ ) {
                value_ -= decayRate_;
                if ( value_ <= sustainLevel_ ) {
                    value_ = sustainLevel_;
                    state_ = SUSTAIN;
                }
            }
            else {
                value_ += decayRate_; // attack target < sustain level
                if ( value_ >= sustainLevel_ ) {
                    value_ = sustainLevel_;
                    state_ = SUSTAIN;
                }
            }
            lastvalue_ = value_;
            break;
            
        case RELEASE:
            value_ -= releaseRate_;
            if ( value_ <= 0.0 ) {
                value_ = 0.0;
                state_ = IDLE;
            }
            lastvalue_ = value_;
            
    }
    
    return value_;
}

#endif
