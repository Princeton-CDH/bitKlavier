#ifndef STK_ADSR_H
#define STK_ADSR_H

#include "Generator.h"

namespace stk {

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

class ADSR : public Generator
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
  ADSR( void );

  //! Class destructor.
  ~ADSR( void );

  //! Set target = 1, state = \e ADSR::ATTACK.
  void keyOn( void );

  //! Set target = 0, state = \e ADSR::RELEASE.
  void keyOff( void );

  //! Set the attack rate.
  void setAttackRate( StkFloat rate );

  //! Set the target value for the attack (default = 1.0).
  void setAttackTarget( StkFloat target );

  //! Set the decay rate.
  void setDecayRate( StkFloat rate );

  //! Set the sustain level.
  void setSustainLevel( StkFloat level );

  //! Set the release rate.
  void setReleaseRate( StkFloat rate );

  //! Set the attack rate based on a time duration.
  void setAttackTime( StkFloat time );

  //! Set the decay rate based on a time duration (seconds).
  void setDecayTime( StkFloat time );

  //! Set the release rate based on a time duration (seconds).
  void setReleaseTime( StkFloat time );
    StkFloat  getReleaseTime( void );

  //! Set sustain level and attack, decay, and release time durations.
  void setAllTimes( StkFloat aTime, StkFloat dTime, StkFloat sLevel, StkFloat rTime );

  //! Set a sustain target value and attack or decay from current value to target.
  void setTarget( StkFloat target );

  //! Return the current envelope \e state (ATTACK, DECAY, SUSTAIN, RELEASE, IDLE).
  int getState( void ) const { return state_; };

  //! Set to state = ADSR::SUSTAIN with current and target values of \e value.
  void setValue( StkFloat value );
    
    StkFloat getAttackRate() {return attackRate_; };
    StkFloat getDecayRate() {return decayRate_; };
    StkFloat getSustainLevel() {return sustainLevel_; };
    StkFloat getReleaseRate() {return releaseRate_; };

  //! Return the last computed output value.
  StkFloat lastOut( void ) const { return lastFrame_[0]; };

  //! Compute and return one output sample.
  StkFloat tick( void );

  //! Fill a channel of the StkFrames object with computed outputs.
  /*!
    The \c channel argument must be less than the number of
    channels in the StkFrames argument (the first channel is specified
    by 0).  However, range checking is only performed if _STK_DEBUG_
    is defined during compilation, in which case an out-of-range value
    will trigger an StkError exception.
  */
  StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );
    
    void print(void)
    {
        printf("%f %f %f %f %f\n", attackRate_, decayRate_, releaseRate_, releaseTime_, sustainLevel_);
    }

 protected:  

  void sampleRateChanged( StkFloat newRate, StkFloat oldRate );

  int state_;
  StkFloat value_;
  StkFloat target_;
  StkFloat attackRate_;
  StkFloat decayRate_;
  StkFloat releaseRate_;
  StkFloat releaseTime_;
  StkFloat sustainLevel_;
};

inline StkFloat ADSR :: tick( void )
{
  switch ( state_ ) {

  case ATTACK:
    value_ += attackRate_;
    if ( value_ >= target_ ) {
      value_ = target_;
      target_ = sustainLevel_;
	    state_ = DECAY;
    }
    lastFrame_[0] = value_;
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
    lastFrame_[0] = value_;
    break;

  case RELEASE:
    value_ -= releaseRate_;
    if ( value_ <= 0.0 ) {
      value_ = 0.0;
      state_ = IDLE;
    }
    lastFrame_[0] = value_;

  }

  return value_;
}

inline StkFrames& ADSR :: tick( StkFrames& frames, unsigned int channel )
{
#if defined(_STK_DEBUG_)
  if ( channel >= frames.channels() ) {
    oStream_ << "ADSR::tick(): channel and StkFrames arguments are incompatible!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  StkFloat *samples = &frames[channel];
  unsigned int hop = frames.channels();
  for ( unsigned int i=0; i<frames.frames(); i++, samples += hop )
    *samples = ADSR::tick();

  return frames;
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
    
    class AHDSR : public Generator
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
        AHDSR( void );
        
        //! Class destructor.
        ~AHDSR( void );
        
        //! Set target = 1, state = \e AHDSR::ATTACK.
        void keyOn( void );
        
        //! Set target = 0, state = \e AHDSR::RELEASE.
        void keyOff( void );
        
        //! Set the attack rate.
        void setAttackRate( StkFloat rate );
        
        //! Set the target value for the attack (default = 1.0).
        void setAttackTarget( StkFloat target );
        
        //! Set the hold time.
        void setHoldTime( StkFloat time );
        
        //! Set the decay rate.
        void setDecayRate( StkFloat rate );
        
        //! Set the sustain level.
        void setSustainLevel( StkFloat level );
        
        //! Set the release rate.
        void setReleaseRate( StkFloat rate );
        
        //! Set the attack rate based on a time duration.
        void setAttackTime( StkFloat time );
        
        //! Set the decay rate based on a time duration (seconds).
        void setDecayTime( StkFloat time );
        
        //! Set the release rate based on a time duration (seconds).
        void setReleaseTime( StkFloat time );
        StkFloat  getReleaseTime( void );
        
        //! Set sustain level and attack, decay, and release time durations.
        void setAllTimes( StkFloat aTime, StkFloat hTime, StkFloat dTime, StkFloat sLevel, StkFloat rTime );
        
        //! Set a sustain target value and attack or decay from current value to target.
        void setTarget( StkFloat target );
        
        //! Return the current envelope \e state (ATTACK, HOLD, DECAY, SUSTAIN, RELEASE, IDLE).
        int getState( void ) const { return state_; };
        
        //! Set to state = ADSR::SUSTAIN with current and target values of \e value.
        void setValue( StkFloat value );
        
        StkFloat getAttackRate() {return attackRate_; };
        StkFloat getHoldSamples() {return holdSamples_; };
        StkFloat getDecayRate() {return decayRate_; };
        StkFloat getSustainLevel() {return sustainLevel_; };
        StkFloat getReleaseRate() {return releaseRate_; };
        
        //! Return the last computed output value.
        StkFloat lastOut( void ) const { return lastFrame_[0]; };
        
        //! Compute and return one output sample.
        StkFloat tick( void );

        
        //! Fill a channel of the StkFrames object with computed outputs.
        /*!
         The \c channel argument must be less than the number of
         channels in the StkFrames argument (the first channel is specified
         by 0).  However, range checking is only performed if _STK_DEBUG_
         is defined during compilation, in which case an out-of-range value
         will trigger an StkError exception.
         */
        StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );
        
        void print(void)
        {
            printf("%f %f %f %f %f\n", attackRate_, decayRate_, releaseRate_, releaseTime_, sustainLevel_);
        }
        
    protected:
        
        void sampleRateChanged( StkFloat newRate, StkFloat oldRate );
        
        int state_;
        double holdSamples_;
        double held_;
        
        StkFloat value_;
        StkFloat target_;
        StkFloat attackRate_;
        StkFloat decayRate_;
        StkFloat releaseRate_;
        StkFloat releaseTime_;
        StkFloat sustainLevel_;
    };
    
    inline StkFloat AHDSR :: tick( void )
    {
        switch ( state_ ) {
                
            case ATTACK:
                value_ += attackRate_;
                if ( value_ >= target_ ) {
                    value_ = target_;
                    state_ = HOLD;
                    held_ = 0;
                }
                lastFrame_[0] = value_;
                break;
                
            case HOLD:
                held_ ++;
                value_ = target_;
                if ( held_ >= holdSamples_) {
                        state_ = DECAY;
                        target_ = sustainLevel_;
                }
                lastFrame_[0] = value_;
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
                lastFrame_[0] = value_;
                break;
                
            case RELEASE:
                value_ -= releaseRate_;
                if ( value_ <= 0.0 ) {
                    value_ = 0.0;
                    state_ = IDLE;
                }
                lastFrame_[0] = value_;
                
        }
        
        return value_;
    }
    
    inline StkFrames& AHDSR :: tick( StkFrames& frames, unsigned int channel )
    {
#if defined(_STK_DEBUG_)
        if ( channel >= frames.channels() ) {
            oStream_ << "AHDSR::tick(): channel and StkFrames arguments are incompatible!";
            handleError( StkError::FUNCTION_ARGUMENT );
        }
#endif
        
        StkFloat *samples = &frames[channel];
        unsigned int hop = frames.channels();
        for ( unsigned int i=0; i<frames.frames(); i++, samples += hop )
            *samples = AHDSR::tick();
        
        return frames;
    }


} // stk namespace

#endif
