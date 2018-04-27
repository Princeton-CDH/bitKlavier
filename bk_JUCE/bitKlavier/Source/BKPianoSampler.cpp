/*
 ==============================================================================
 
 BKPianoSampler.cpp
 Created: 19 Oct 2016 10:00:04am
 Author:  Michael R Mulshine
 
 ==============================================================================
 */

#include "BKPianoSampler.h"
#include "AudioConstants.h"


BKPianoSamplerSound::BKPianoSamplerSound (const String& soundName,
                                          BKReferenceCountedBuffer::Ptr buffer,
                                          uint64 soundLength,
                                          double sourceSampleRate,
                                          const BigInteger& notes,
                                          const int rootMidiNote,
                                          const BigInteger& velocities,
                                          sfzero::Region* reg)
:
name (soundName),
data(buffer),
sourceSampleRate(sourceSampleRate),
midiNotes (notes),
midiVelocities(velocities),
soundLength(soundLength),
midiRootNote (rootMidiNote)
{
    rampOnSamples = roundToInt (aRampOnTimeSec* sourceSampleRate);
    rampOffSamples = roundToInt (aRampOffTimeSec * sourceSampleRate);

    
    if (reg != nullptr)
    {
        isSoundfont = true;
        
        loopStart = reg->loop_start;
        loopEnd = reg->loop_end;
        float attack = reg->ampeg.attack * 0.001f;
        float decay = reg->ampeg.decay * 0.001f;
        float sustain = reg->ampeg.sustain / 100.0f;
        float release = reg->ampeg.release * 0.001f;
        
        adsr.setAllTimes((attack > 1.0f) ? attack : 1.0f, (decay > 1.0f) ? decay : 1.0f, sustain, (release > 1.0f) ? release : 1.0f);
    }
    else
    {
        isSoundfont = false;
    }
}

BKPianoSamplerSound::~BKPianoSamplerSound()
{
}

bool BKPianoSamplerSound::isSoundfontSound(void)
{
    return isSoundfont;
}

bool BKPianoSamplerSound::appliesToNote (int midiNoteNumber)
{
    return midiNotes [midiNoteNumber];
}

bool BKPianoSamplerSound::appliesToVelocity(int midiNoteVelocity)
{
    return midiVelocities [midiNoteVelocity];
}

bool BKPianoSamplerSound::appliesToChannel (int /*midiChannel*/)
{
    return true;
}


//==============================================================================
BKPianoSamplerVoice::BKPianoSamplerVoice(GeneralSettings::Ptr gen) :
//generalSettings(gen),
pitchRatio (0.0),
sourceSamplePosition (0.0),
lgain (0.0f), rgain (0.0f),
rampOnOffLevel (0),
rampOnDelta (0),
rampOffDelta (0),
isInRampOn (false), isInRampOff (false)
{
    generalSettings = gen;
}

BKPianoSamplerVoice::~BKPianoSamplerVoice()
{
}

bool BKPianoSamplerVoice::canPlaySound (BKSynthesiserSound* sound)
{
    return dynamic_cast<const BKPianoSamplerSound*> (sound) != nullptr;
}


void BKPianoSamplerVoice::startNote (const float midiNoteNumber,
                                     const float gain,
                                     PianoSamplerNoteDirection direction,
                                     PianoSamplerNoteType type,
                                     BKNoteType bktype,
                                     const uint64 startingPosition,
                                     const uint64 length,
                                     int voiceRampOn,
                                     int voiceRampOff,
                                     BKSynthesiserSound* s)
{
    
    
                        startNote   (midiNoteNumber,
                                     gain,
                                     direction,
                                     type,
                                     bktype,
                                     startingPosition,
                                     length,
                                     voiceRampOn,
                                     (int)(3*getSampleRate()),
                                     1.,
                                     voiceRampOff,
                                     s);
}

void BKPianoSamplerVoice::startNote (const float midiNoteNumber,
                                     const float gain,
                                     PianoSamplerNoteDirection direction,
                                     PianoSamplerNoteType type,
                                     BKNoteType bktype,
                                     const uint64 startingPosition,
                                     const uint64 length,
                                     int adsrAttack,
                                     int adsrDecay,
                                     float adsrSustain,
                                     int adsrRelease,
                                     BKSynthesiserSound* s)
{
    if (const BKPianoSamplerSound* const sound = dynamic_cast<const BKPianoSamplerSound*> (s))
    {
        pitchRatio = powf(2.0f, (midiNoteNumber - (float)sound->midiRootNote) / 12.0f)
                        * sound->sourceSampleRate
                        * generalSettings->getTuningRatio()
                        / getSampleRate();
    
        bkType = bktype;
        playType = type;
        playDirection = direction;
        
        revRamped = false;
        
        playLength = 0.0;
        double maxLength;
        if (sound->isSoundfont)
        {
            maxLength = sound->soundLength+2;
        }
        else
        {
            maxLength = sound->soundLength - adsrRelease;
        }
        
        
        if (bkType != MainNote)
        {
            //should redo this to scale ADSR rather than just set this way...
            if(adsrAttack  > (0.5 * length))   adsrAttack     = 0.5 * length;
            if(adsrRelease > (0.5 * length))   adsrRelease    = 0.5 * length;

            playLength = (length - adsrRelease) * pitchRatio;
        }
         
        lengthTracker = 0.0;
        
        DBG("playlength: " + String(playLength));
        
        if (playDirection == Forward)
        {
            if (playType == Normal)
            {
                sourceSamplePosition = 0;
                playEndPosition = maxLength - 1;
            }
            else if (playType == NormalFixedStart)
            {
                sourceSamplePosition = startingPosition;
                playEndPosition = maxLength - 1;
            }
            else if (playType == FixedLength)
            {
                sourceSamplePosition = 0.0;
                playEndPosition = jmin(playLength, maxLength) - 1;
            }
            else if (playType == FixedLengthFixedStart)
            {
                sourceSamplePosition = startingPosition;
                playEndPosition = jmin( (startingPosition + playLength), maxLength) - 1;
            }
            else
            {
                DBG("Invalid note type.");
            }
        }
        else if (playDirection == Reverse)
        {
            if (playType == Normal)
            {
                sourceSamplePosition = sound->soundLength - 1;
                
                playEndPosition = (sound->isSoundfont ? sound->loopStart : adsrRelease);
            }
            else if (playType == NormalFixedStart)
            {
                if (sound->isSoundfont)
                {
                    sourceSamplePosition = (startingPosition % (sound->loopEnd - sound->loopStart));
                }
                else
                {
                    if (startingPosition < adsrRelease)
                    {
                        sourceSamplePosition = adsrRelease;
                    }
                    else if (startingPosition >= sound->soundLength)
                    {
                        sourceSamplePosition = (sound->soundLength - 1);
                    }
                    else
                    {
                        sourceSamplePosition = startingPosition;
                    }
                }
                

                playEndPosition = (sound->isSoundfont ? sound->loopStart : adsrRelease);
            }
            else if (playType == FixedLength)
            {
                sourceSamplePosition = sound->soundLength - 1;
                if (playLength >= sourceSamplePosition)
                {
                    playEndPosition = (sound->isSoundfont ? sound->loopStart : adsrRelease);
                }
                else
                {
                    playEndPosition = (double)(sourceSamplePosition - playLength);
                }
            }
            else if (playType == FixedLengthFixedStart)
            {
                uint64 pos = startingPosition;
                if (sound->isSoundfont) pos = (startingPosition % (sound->loopEnd - sound->loopStart));
                
                sourceSamplePosition = pos * pitchRatio;
                
                if (playLength >= sourceSamplePosition)
                {
                    playEndPosition = (sound->isSoundfont ? sound->loopStart : adsrRelease);
                }
                else
                {
                    playEndPosition = (double)(sourceSamplePosition - playLength);
                }
            }
            else
            {
                DBG("Invalid note type.");
            }
        }
        else
        {
            DBG("Invalid note direction.");
        }
        
        lgain = gain;
        rgain = gain;

        adsr.setSampleRate(getSampleRate());
        
        //DBG("BKPianoSamplerVoice::startNote ADSR vals: " + String(adsrAttack/(0.001*getSampleRate())) + " " + String(adsrDecay/(0.001*getSampleRate())) + " " + String(adsrSustain) + " " + String(adsrRelease/(0.001*getSampleRate())));
        adsr.setAllTimes(adsrAttack / getSampleRate(), adsrDecay / getSampleRate(), adsrSustain, adsrRelease / getSampleRate());

        adsr.keyOn();
        //DBG("ADSR vals = " + String(adsr.getAttackRate()) + " " + String(adsr.getDecayRate()) + " " + String(adsr.getSustainLevel()) + " " + String(adsr.getReleaseRate()));
        
        noteStartingPosition = sourceSamplePosition;
        noteEndPosition = playEndPosition;
    }
    else
    {
        jassertfalse; // this object can only play BKSamplerSounds!
    }
}



void BKPianoSamplerVoice::stopNote (float /*velocity*/, bool allowTailOff)
{
    if (allowTailOff)
    {
        adsr.keyOff();
    }
    else
    {
        clearCurrentNote();
    }
}

void BKPianoSamplerVoice::pitchWheelMoved (const int /*newValue*/)
{
}

void BKPianoSamplerVoice::controllerMoved (const int /*controllerNumber*/,
                                           const int /*newValue*/)
{
}

//==============================================================================

void BKPianoSamplerVoice::processPiano(AudioSampleBuffer& outputBuffer,
                                       int startSample, int numSamples,
                                       const BKPianoSamplerSound* playingSound)
{
    const float* const inL = playingSound->data->getAudioSampleBuffer()->getReadPointer (0);
    const float* const inR = playingSound->data->getAudioSampleBuffer()->getNumChannels() > 1
    ? playingSound->data->getAudioSampleBuffer()->getReadPointer (1)
    : nullptr;
    
    float* outL = outputBuffer.getWritePointer (0, startSample);
    float* outR = outputBuffer.getNumChannels() > 1 ? outputBuffer.getWritePointer (1, startSample) : nullptr;
    
    while (--numSamples >= 0)
    {
        if ((adsr.getState() != stk::ADSR::IDLE) &&
            (playDirection == Reverse) &&
            (sourceSamplePosition > playingSound->soundLength))
        {
            if (outR != nullptr)
            {
                *outL++ += 0;
                *outR++ += 0;
            }
            else
            {
                *outL++ += 0;
            }
            sourceSamplePosition -= pitchRatio;
            continue;
        }
        
        const int pos = (int) sourceSamplePosition;
        const float alpha = (float) (sourceSamplePosition - pos);
        const float invAlpha = 1.0f - alpha;
        
        // just using a very simple linear interpolation here..
        float l = (inL [pos] * invAlpha + inL [pos + 1] * alpha);
        float r = (inR != nullptr) ? (inR [pos] * invAlpha + inR [pos + 1] * alpha) : l;
        
        l *= (lgain * adsr.tick());
        r *= (rgain * adsr.lastOut());
        
        if (adsr.getState() == stk::ADSR::IDLE)
        {
            stopNote (0.0f, false);
            break;
        }
        
        if (outR != nullptr)
        {
            *outL++ += (l * 1.0f);
            *outR++ += (r * 1.0f);
        }
        else
        {
            *outL++ += ((l + r) * 0.5f) * 1.0f;
        }
        
        if (playDirection == Forward)
        {
            sourceSamplePosition += pitchRatio;
            
            if (sourceSamplePosition >= playEndPosition)
            {
                if (adsr.getState() != stk::ADSR::RELEASE)
                {
                    adsr.keyOff();
                }
            }
            
            if(sourceSamplePosition >= playingSound->soundLength)
            {
                //clearCurrentNote();
                stopNote(0.0f, true);
            }
        }
        else if (playDirection == Reverse)
        {
            sourceSamplePosition -= pitchRatio;

            if (sourceSamplePosition <= playEndPosition)
            {
                if ((adsr.getState() != stk::ADSR::RELEASE) && (adsr.getState() != stk::ADSR::IDLE))
                {
                    adsr.keyOff();
                }
            }
            
            if(sourceSamplePosition <= 0)
            {
                clearCurrentNote();
            }
        }
        else
        {
            DBG("Invalid note direction.");
        }
    }
}


#define INTERP 0

void BKPianoSamplerVoice::processSoundfont(AudioSampleBuffer& outputBuffer,
                                           int startSample, int numSamples,
                                           const BKPianoSamplerSound* playingSound)
{
    
    const float* const inL = playingSound->data->getAudioSampleBuffer()->getReadPointer (0);
    const float* const inR = playingSound->data->getAudioSampleBuffer()->getNumChannels() > 1
    ? playingSound->data->getAudioSampleBuffer()->getReadPointer (1)
    : nullptr;

    float* outL = outputBuffer.getWritePointer (0, startSample);
    float* outR = outputBuffer.getNumChannels() > 1 ? outputBuffer.getWritePointer (1, startSample) : nullptr;
    
    int64 loopStart, loopEnd;
    
    loopStart = playingSound->loopStart;
    loopEnd = playingSound->loopEnd;
    
    while (--numSamples >= 0)
    {
        if (playDirection == Reverse)
        {
            if (sourceSamplePosition > playingSound->soundLength)
            {
                if (outR != nullptr)
                {
                    *outL++ += 0;
                    *outR++ += 0;
                }
                else
                {
                    *outL++ += 0;
                }
                sourceSamplePosition -= pitchRatio;
                continue;
            }
            else if (!revRamped && (sourceSamplePosition < (playingSound->soundLength + pitchRatio)))
            {
                revRamped = true;
                adsr.keyOn();
            }
        }
        
        const int pos = (int) sourceSamplePosition;
        const float alpha = (float) (sourceSamplePosition - pos);
        const float invAlpha = 1.0f - alpha;
        
        float l,r;
        if (fadeTracker > 0.0)
        {
            const int fadePos = (int) fadeTracker;
            const float fadeAlpha = (float) (fadeTracker - fadePos);
            const float fadeInvAlpha = 1.0f - fadeAlpha;
            
            float fade = (fadeTracker > 0.0) ? (float)(fadeTracker / PAD) : 1.0f;
            
            if (playDirection == Forward)
            {
                l = (1.0f - fade) * (inL [pos] * invAlpha + inL [pos + 1] * alpha);
                r = (1.0f - fade) * ((inR != nullptr) ? (inR [pos] * invAlpha + inR [pos + 1] * alpha) : l);
                
                l += fade * (inL [fadePos] * fadeInvAlpha + inL [fadePos + 1] * fadeAlpha);
                r += fade * ((inR != nullptr) ? (inR [fadePos] * fadeInvAlpha + inR [fadePos + 1] * fadeAlpha) : l);
            }
            else
            {
                l = fade * (inL [pos] * invAlpha + inL [pos + 1] * alpha);
                r = fade * ((inR != nullptr) ? (inR [pos] * invAlpha + inR [pos + 1] * alpha) : l);
                
                l += (1.0f - fade) * (inL [fadePos] * fadeInvAlpha + inL [fadePos + 1] * fadeAlpha);
                r += (1.0f - fade) * ((inR != nullptr) ? (inR [fadePos] * fadeInvAlpha + inR [fadePos + 1] * fadeAlpha) : l);
            }
            
        }
        else
        {
            l = (inL [pos] * invAlpha + inL [pos + 1] * alpha);
            r = (inR != nullptr) ? (inR [pos] * invAlpha + inR [pos + 1] * alpha) : l;
        }
        
        
        
        l *= (lgain * adsr.tick());
        r *= (rgain * adsr.lastOut());
        
        if (outR != nullptr)
        {
            *outL++ += (l * 1.0f);
            *outR++ += (r * 1.0f);
        }
        else
        {
            *outL++ += ((l + r) * 0.5f) * 1.0f;
        }
        
        if (adsr.getState() == stk::ADSR::IDLE)
        {
            stopNote (0.0f, false);
            break;
        }
        
        
        if (playDirection == Forward)
        {
            sourceSamplePosition += pitchRatio;
            lengthTracker += pitchRatio;
            
            if (sourceSamplePosition >= loopEnd)
            {
                sourceSamplePosition = loopStart;
            }
                          
            fadeTracker = (sourceSamplePosition -  (loopEnd-PAD));
                          
            
            if ((playType != Normal) && (lengthTracker >= playLength))
            {
                if (adsr.getState() != stk::ADSR::RELEASE)
                {
                    adsr.keyOff();
                }
            }
            
            if(sourceSamplePosition >= (playingSound->soundLength+PAD) )
            {
                //stopNote(0.0f, true);
                clearCurrentNote();
            }
        }
        else if (playDirection == Reverse)
        {
            sourceSamplePosition -= pitchRatio;
            lengthTracker += pitchRatio;
            
            fadeTracker = PAD - (sourceSamplePosition - loopStart);
            
            if (sourceSamplePosition <= loopStart)
            {
                sourceSamplePosition = loopEnd;
            }
            
            if ((playType != Normal) && (lengthTracker >= playLength))
            {
                if ((adsr.getState() != stk::ADSR::RELEASE) && (adsr.getState() != stk::ADSR::IDLE))
                {
                    adsr.keyOff();
                }
            }
            
            if (sourceSamplePosition <= 0.0)
            {
                //stopNote(0.0f, true);
                clearCurrentNote();
            }
        }
        else
        {
            DBG("Invalid note direction.");
        }
    }
}

void BKPianoSamplerVoice::renderNextBlock (AudioSampleBuffer& outputBuffer, int startSample, int numSamples)
{
    if (const BKPianoSamplerSound* const playingSound = static_cast<BKPianoSamplerSound*> (getCurrentlyPlayingSound().get()))
    {
        if (playingSound->isSoundfont)  processSoundfont(outputBuffer, startSample, numSamples, playingSound);
        else                            processPiano(outputBuffer, startSample, numSamples, playingSound);
        
    }
}


