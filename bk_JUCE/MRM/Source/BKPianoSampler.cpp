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
                                                            ReferenceCountedBuffer::Ptr buffer,
                                                            uint64 soundLength,
                                                            double sourceSampleRate,
                                                            const BigInteger& notes,
                                                            const int rootMidiNote,
                                                            const BigInteger& velocities)
: name (soundName),
data(buffer),
soundLength(soundLength),
sourceSampleRate(sourceSampleRate),
midiNotes (notes),
midiVelocities(velocities),
midiRootNote (rootMidiNote)
{
    rampOnSamples = roundToInt (aRampOnTimeSec* sourceSampleRate);
    rampOffSamples = roundToInt (aRampOffTimeSec * sourceSampleRate);
    
}

BKPianoSamplerSound::~BKPianoSamplerSound()
{
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
BKPianoSamplerVoice::BKPianoSamplerVoice() : pitchRatio (0.0),
sourceSamplePosition (0.0),
lgain (0.0f), rgain (0.0f),
rampOnOffLevel (0),
rampOnDelta (0),
rampOffDelta (0),
isInRampOn (false), isInRampOff (false)
{
    numPulses = 0;
    maxPulses = 16;
}

BKPianoSamplerVoice::~BKPianoSamplerVoice()
{
}

bool BKPianoSamplerVoice::canPlaySound (BKSynthesiserSound* sound)
{
    return dynamic_cast<const BKPianoSamplerSound*> (sound) != nullptr;
}

void BKPianoSamplerVoice::startNote (const float midiNoteNumber,
                                              const float velocity,
                                              PianoSamplerNoteDirection direction,
                                              PianoSamplerNoteType type,
                                              BKNoteType bktype,
                                              const uint64 startingPosition,
                                              const uint64 length,
                                              BKSynthesiserSound* s)
{
    if (const BKPianoSamplerSound* const sound = dynamic_cast<const BKPianoSamplerSound*> (s))
    {
        
        numPulses = 0;
        maxPulses = 16;
        pitchRatio = powf(2.0f, (midiNoteNumber - (float)sound->midiRootNote) / 12.0f) * sound->sourceSampleRate / getSampleRate();
        
        bkType = bktype;
        playType = type;
        playDirection = direction;
        
        double playLength = ( length - sound->rampOffSamples ) * pitchRatio;
        double maxLength = sound->soundLength - sound->rampOffSamples;
        
        if (playDirection == Forward)
        {
            if (playType == Normal)
            {
                sourceSamplePosition = 0.0;
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
                playEndPosition = sound->rampOffSamples;
            }
            else if (playType == NormalFixedStart)
            {
                if (startingPosition < sound->rampOffSamples)
                {
                    sourceSamplePosition = sound->rampOffSamples;
                }
                else
                {
                    sourceSamplePosition = startingPosition;
                }
                playEndPosition = sound->rampOffSamples;
            }
            else if (playType == FixedLength)
            {
                sourceSamplePosition = sound->soundLength - 1;
                if (playLength >= sourceSamplePosition)
                {
                    playEndPosition = (double)sound->rampOffSamples;
                }
                else
                {
                    playEndPosition = (double)(sourceSamplePosition - playLength);
                }
                DBG(String(playEndPosition));
            }
            else if (playType == FixedLengthFixedStart)
            {
                sourceSamplePosition = startingPosition;
                if (playLength >= sourceSamplePosition)
                {
                    playEndPosition = (double)sound->rampOffSamples;
                }
                else
                {
                    playEndPosition = (double)(startingPosition - playLength);
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
        
        lgain = velocity;
        rgain = velocity;
        
        isInRampOn = (sound->rampOnSamples > 0);
        isInRampOff = false;
        
        noteStartingPosition = sourceSamplePosition;
        noteEndPosition = playEndPosition;
        

        if (isInRampOn)
        {
            rampOnOffLevel = 0.0f;
            rampOnDelta = (float) (pitchRatio / sound->rampOnSamples);
        }
        else
        {
            rampOnOffLevel = 1.0f;
            rampOnDelta = 0.0f;
        }
        
        if (sound->rampOffSamples > 0)
        {
            rampOffDelta = (float) (-pitchRatio / sound->rampOffSamples);
        }
        else
        {
            rampOffDelta = -1.0f;
        }
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
        isInRampOn = false;
        isInRampOff = true;
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


void BKPianoSamplerVoice::renderNextBlock (AudioSampleBuffer& outputBuffer, int startSample, int numSamples)
{
    if (const BKPianoSamplerSound* const playingSound = static_cast<BKPianoSamplerSound*> (getCurrentlyPlayingSound().get()))
    {
        
        const float* const inL = playingSound->data->getAudioSampleBuffer()->getReadPointer (0);
        const float* const inR = playingSound->data->getAudioSampleBuffer()->getNumChannels() > 1
        ? playingSound->data->getAudioSampleBuffer()->getReadPointer (1) : nullptr;
        
        
        float* outL = outputBuffer.getWritePointer (0, startSample);
        float* outR = outputBuffer.getNumChannels() > 1 ? outputBuffer.getWritePointer (1, startSample) : nullptr;
        
        while (--numSamples >= 0)
        {
            timer++;
            const int pos = (int) sourceSamplePosition;
            const float alpha = (float) (sourceSamplePosition - pos);
            const float invAlpha = 1.0f - alpha;
            
            float l = 0.0;
            float r = 0.0;
            
            // If pointer not past end of buffer, sample the buffer. 
            if (pos < playingSound->soundLength)
            {
                // just using a very simple linear interpolation here..
                l = (inL [pos] * invAlpha + inL [pos + 1] * alpha);
                r = (inR != nullptr) ? (inR [pos] * invAlpha + inR [pos + 1] * alpha) : l;
            }
            
            l *= lgain;
            r *= rgain;
            
            
            if (isInRampOn)
            {
                l *= rampOnOffLevel;
                r *= rampOnOffLevel;
                
                rampOnOffLevel += rampOnDelta;
                
                if (rampOnOffLevel >= 1.0f)
                {
                    rampOnOffLevel = 1.0f;
                    isInRampOff = false;
                }
            }
            else if (isInRampOff)
            {
                l *= rampOnOffLevel;
                r *= rampOnOffLevel;
                
                rampOnOffLevel += rampOffDelta;
                
                if (rampOnOffLevel <= 0.0f)
                {
                    // allow interruption of synchronic pulses here if receive synchronic note off.
                    if (playType != Normal && playType != NormalFixedStart && bkType == Synchronic && numPulses < maxPulses)
                    {
                        numPulses++;
                        
                        isInRampOff = 0;
                        isInRampOn = 1;
                        sourceSamplePosition = noteStartingPosition;

                    }
                    else
                    {
                        stopNote (0.0f, false);
                        break;
                    }
                }
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
                if (!isInRampOff)
                {
                    if (sourceSamplePosition >= playEndPosition)
                    {
                        stopNote (0.0f, true);
                    }
                }
            }
            else if (playDirection == Reverse)
            {
                sourceSamplePosition -= pitchRatio;

#if !CRAY_COOL_MUSIC_MAKER_2
                if (!isInRampOff)
                {
                    if (sourceSamplePosition <= playEndPosition)
                    {
                        stopNote (0.0f, true);
                    }
                }
                
#endif
            }
            else
            {
                DBG("Invalid note direction.");
            }
            

            
        }
    }
}


