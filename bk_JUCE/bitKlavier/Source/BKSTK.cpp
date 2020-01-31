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

    lastvalue = value;
	return value;
}

BKDelayL::BKDelayL() :
    inPoint(0),
    outPoint(0),
    bufferSize(44100.),
    length(0.0),
	gain(1.0),
	lastFrameLeft(0),
	lastFrameRight(0),
    feedback(0.9),
	doNextOutLeft(false),
	doNextOutRight(false),
    loading(false),
	sampleRate(44100.)
{
	inputs = AudioBuffer<float>(2, bufferSize);
	inputs.clear();
	setLength(0.0);
}

BKDelayL::BKDelayL(float delayLength, int bufferSize, float delayGain, double sr) :
    inPoint(0),
    outPoint(0),
    bufferSize(bufferSize),
    length(delayLength),
	gain(delayGain),
	lastFrameLeft(0),
	lastFrameRight(0),
	doNextOutLeft(false),
	doNextOutRight(false),
    loading(false),
	sampleRate(sr)
{
	inputs = AudioBuffer<float>(2, bufferSize);
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

void BKDelayL::setBufferSize(int size)
{
    const ScopedLock sl (lock);
    loading = true;
    inputs.setSize(2, bufferSize);
    reset();
    bufferSize = size;
    inputs.clear();
    loading = false;
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
    if (inPoint >= inputs.getNumSamples()) inPoint = 0;
    inputs.addSample(channel, (inPoint + offset) % inputs.getNumSamples(), input * gain);
}

//redo so it has channel argument, like addSample, also bool which indicates whether to increment inPoint
//or, have it take float* input
void BKDelayL::tick(float input, float* outputs, float outGain, bool stereo)
{
    if (loading) return;
    if (inPoint >= inputs.getNumSamples()) inPoint = 0;
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
		outputs[0] = lastFrameLeft * outGain;
		outputs[1] = lastFrameRight * outGain;
	}
	else
	{
		outputs[0] = lastFrameLeft * outGain;
    }
}

void BKDelayL::scalePrevious(float coefficient, unsigned long offset, int channel)
{
    if (loading) return;
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
	dBufferSize(d->getBufferSize()),
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

BlendronicDelay::BlendronicDelay(float delayLength, float smoothValue, float smoothRate, int delayBufferSize, double sr, bool active) :
	dBufferSize(delayBufferSize),
	dDelayGain(1.0f),
	dDelayLength(delayLength),
	dSmoothValue(smoothValue),
	dSmoothRate(smoothRate),
    dInputOpen(true),
    dOutputOpen(true),
    sampleRate(sr)
{
	delayLinear =  new BKDelayL(dDelayLength, dBufferSize, dDelayGain, sampleRate);
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

void BlendronicDelay::tick(float* outputs, float outGain)
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
        delayLinear->tick(0, outputs, outGain, true);
        outputs[0] *= env;
        outputs[1] *= env;
    }
    else delayLinear->tick(0, dummyOut, outGain, true);
}

void BlendronicDelay::duckAndClear()
{
    shouldDuck = true;
    setEnvelopeTarget(0.0f);
}

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

BKADSR :: BKADSR( void )
{
  target_ = 0.0;
  value_ = 0.0;
  attackRate_ = 0.001;
  decayRate_ = 0.001;
  releaseRate_ = 0.005;
  releaseTime_ = -1.0;
  sustainLevel_ = 0.5;
  state_ = IDLE;
  
  sampleRate = 44100.;
}

BKADSR :: ~BKADSR( void )
{
}

void BKADSR :: sampleRateChanged( float newRate, float oldRate )
{
    attackRate_ = oldRate * attackRate_ / newRate;
    decayRate_ = oldRate * decayRate_ / newRate;
    releaseRate_ = oldRate * releaseRate_ / newRate;
}

void BKADSR :: keyOn()
{
  if ( target_ <= 0.0 ) target_ = 1.0;
  state_ = ATTACK;
}

void BKADSR :: keyOff()
{
  target_ = 0.0;
  state_ = RELEASE;

  // FIXED October 2010 - Nick Donaldson
  // Need to make release rate relative to current value!!
  // Only update if we have set a TIME rather than a RATE,
  // in which case releaseTime_ will be -1
  if ( releaseTime_ > 0.0 )
      releaseRate_ = value_ / ( releaseTime_ * sampleRate );
}

void BKADSR :: setAttackRate( float rate )
{
  if ( rate < 0.0 ) {
    //oStream_ << "ADSR::setAttackRate: argument must be >= 0.0!";
    //handleError( StkError::WARNING ); return;
      return;
  }

  attackRate_ = rate;
}

void BKADSR :: setAttackTarget( float target )
{
  if ( target < 0.0 ) {
    //oStream_ << "ADSR::setAttackRate: argument must be >= 0.0!";
    //handleError( StkError::WARNING ); return;
      return;
  }

  target_ = target;
}

void BKADSR :: setDecayRate( float rate )
{
  if ( rate < 0.0 ) {
    //oStream_ << "ADSR::setAttackRate: argument must be >= 0.0!";
    //handleError( StkError::WARNING ); return;
      return;
  }

  decayRate_ = rate;
}

void BKADSR :: setSustainLevel( float level )
{
  if ( level < 0.0 ) {
    //oStream_ << "ADSR::setAttackRate: argument must be >= 0.0!";
    //handleError( StkError::WARNING ); return;
      return;
  }

  sustainLevel_ = level;
}

void BKADSR :: setReleaseRate( float rate )
{
  if ( rate < 0.0 ) {
    //oStream_ << "ADSR::setAttackRate: argument must be >= 0.0!";
    //handleError( StkError::WARNING ); return;
      return;
  }

  releaseRate_ = rate;

  // Set to negative value so we don't update the release rate on keyOff()
  releaseTime_ = -1.0;
}

void BKADSR :: setAttackTime( float time )
{
  if ( time <= 0.0 ) {
    //oStream_ << "ADSR::setAttackRate: argument must be >= 0.0!";
    //handleError( StkError::WARNING ); return;
      return;
  }

  attackRate_ = 1.0 / ( time * sampleRate );
}

void BKADSR :: setDecayTime( float time )
{
  if ( time <= 0.0 ) {
    //oStream_ << "ADSR::setAttackRate: argument must be >= 0.0!";
    //handleError( StkError::WARNING ); return;
      return;
  }

  decayRate_ = (1.0 - sustainLevel_) / ( time * sampleRate );
}

void BKADSR :: setReleaseTime( float time )
{
  if ( time <= 0.0 ) {
    //oStream_ << "ADSR::setAttackRate: argument must be >= 0.0!";
    //handleError( StkError::WARNING ); return;
      return;
  }

  releaseRate_ = sustainLevel_ / ( time * sampleRate );
  releaseTime_ = time;
}
    
float BKADSR :: getReleaseTime( void )
{
    return releaseTime_;
}

void BKADSR :: setAllTimes( float aTime, float dTime, float sLevel, float rTime )
{
  this->setSustainLevel( sLevel );
  this->setAttackTime( aTime );
  this->setDecayTime( dTime );
  //this->setSustainLevel( sLevel );
  this->setReleaseTime( rTime );
}

void BKADSR :: setTarget( float target )
{
  if ( target < 0.0 ) {
    //oStream_ << "ADSR::setAttackRate: argument must be >= 0.0!";
    //handleError( StkError::WARNING ); return;
      return;
  }

  target_ = target;

  this->setSustainLevel( target_ );
  if ( value_ < target_ ) state_ = ATTACK;
  if ( value_ > target_ ) state_ = DECAY;
}

void BKADSR :: setValue( float value )
{
  state_ = SUSTAIN;
  target_ = value;
  value_ = value;
  this->setSustainLevel( value );
  lastvalue_ = value;
}


/***************************************************/
/*! \class ADHSR
    \brief STK ADHSR envelope class.
 */
/***************************************************/


BKAHDSR :: BKAHDSR( void )
{
    target_ = 0.0;
    value_ = 0.0;
    held_ = 0.0;
    holdSamples_ = 0.0;
    attackRate_ = 0.001;
    decayRate_ = 0.001;
    releaseRate_ = 0.005;
    releaseTime_ = -1.0;
    sustainLevel_ = 0.5;
    state_ = IDLE;

    sampleRate = 44100.;
}

BKAHDSR :: ~BKAHDSR( void )
{
}

void BKAHDSR :: sampleRateChanged( float newRate, float oldRate )
{
    attackRate_ = oldRate * attackRate_ / newRate;
    holdSamples_ = oldRate * holdSamples_ / newRate;
    decayRate_ = oldRate * decayRate_ / newRate;
    releaseRate_ = oldRate * releaseRate_ / newRate;
}

void BKAHDSR :: keyOn()
{
    if ( target_ <= 0.0 ) target_ = 1.0;
    state_ = ATTACK;
}

void BKAHDSR :: keyOff()
{
    target_ = 0.0;
    state_ = RELEASE;
    
    // FIXED October 2010 - Nick Donaldson
    // Need to make release rate relative to current value!!
    // Only update if we have set a TIME rather than a RATE,
    // in which case releaseTime_ will be -1
    if ( releaseTime_ > 0.0 )
        releaseRate_ = value_ / ( releaseTime_ * sampleRate );
}

void BKAHDSR :: setAttackRate( float rate )
{
    if ( rate < 0.0 ) {
        //oStream_ << "ADSR::setTarget: negative target not allowed!";
        //handleError( StkError::WARNING );
        return;
    }
    
    attackRate_ = rate;
}

void BKAHDSR :: setAttackTarget( float target )
{
    if ( target < 0.0 ) {
        //oStream_ << "ADSR::setTarget: negative target not allowed!";
        //handleError( StkError::WARNING );
        return;
    }
    
    target_ = target;
}
    
void BKAHDSR :: setHoldTime( float time )
{
    if ( time < 0.0 ){
        //oStream_ << "ADSR::setTarget: negative target not allowed!";
        //handleError( StkError::WARNING );
        return;
    }
    
    holdSamples_ = time * sampleRate;
}

void BKAHDSR :: setDecayRate( float rate )
{
    if ( rate < 0.0 ) {
        //oStream_ << "ADSR::setTarget: negative target not allowed!";
        //handleError( StkError::WARNING );
        return;
    }
    
    decayRate_ = rate;
}

void BKAHDSR :: setSustainLevel( float level )
{
    if ( level < 0.0 ) {
        //oStream_ << "ADSR::setTarget: negative target not allowed!";
        //handleError( StkError::WARNING );
        return;
    }
    
    sustainLevel_ = level;
}

void BKAHDSR :: setReleaseRate( float rate )
{
    if ( rate < 0.0 ) {
        //oStream_ << "ADSR::setTarget: negative target not allowed!";
        //handleError( StkError::WARNING );
        return;
    }
    
    releaseRate_ = rate;
    
    // Set to negative value so we don't update the release rate on keyOff()
    releaseTime_ = -1.0;
}

void BKAHDSR :: setAttackTime( float time )
{
    if ( time <= 0.0 ) {
        //oStream_ << "ADSR::setTarget: negative target not allowed!";
        //handleError( StkError::WARNING );
        return;
    }
    
    attackRate_ = 1.0 / ( time * sampleRate);
}

void BKAHDSR :: setDecayTime( float time )
{
    if ( time <= 0.0 ) {
        //oStream_ << "ADSR::setTarget: negative target not allowed!";
        //handleError( StkError::WARNING );
        return;
    }
    
    decayRate_ = (1.0 - sustainLevel_) / ( time * sampleRate );
}

void BKAHDSR :: setReleaseTime( float time )
{
    if ( time <= 0.0 ) {
        //oStream_ << "ADSR::setTarget: negative target not allowed!";
        //handleError( StkError::WARNING );
        return;
    }
    
    releaseRate_ = sustainLevel_ / ( time * sampleRate );
    releaseTime_ = time;
}

float BKAHDSR :: getReleaseTime( void )
{
    return releaseTime_;
}

void BKAHDSR :: setAllTimes( float aTime, float hTime, float dTime, float sLevel, float rTime )
{
    this->setSustainLevel( sLevel );

    this->setAttackTime( aTime );
    this->setHoldTime( hTime );
    this->setDecayTime( dTime );
    this->setReleaseTime( rTime );
}

void BKAHDSR :: setTarget( float target )
{
    if ( target < 0.0 ) {
        //oStream_ << "ADSR::setTarget: negative target not allowed!";
        //handleError( StkError::WARNING );
        return;
    }
    
    target_ = target;
    
    this->setSustainLevel( target_ );
    if ( value_ < target_ ) state_ = ATTACK;
    if ( value_ > target_ ) state_ = DECAY;
}

void BKAHDSR :: setValue( float value )
{
    state_ = SUSTAIN;
    target_ = value;
    value_ = value;
    this->setSustainLevel( value );
    lastvalue_ = value;
}
