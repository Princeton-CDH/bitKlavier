/*
  ==============================================================================

    BKFixedNotePianoSampler.cpp
    Created: 18 Oct 2016 11:54:49am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "BKFixedNotePianoSampler.h"

#include "AudioConstants.h"

BKFixedNotePianoSamplerSound::BKFixedNotePianoSamplerSound (const String& soundName,
                                                  ReferenceCountedBuffer::Ptr buffer,
                                                  double sourceSampleRate,
                                                  const BigInteger& notes,
                                                  const int rootMidiNote,
                                                  const BigInteger& velocities)
: name (soundName),
data(buffer),
sourceSampleRate(sourceSampleRate),
midiNotes (notes),
midiVelocities(velocities),
midiRootNote (rootMidiNote)
{
    rampOnSamples = roundToInt (aRampOnTimeSec* sourceSampleRate);
    rampOffSamples = roundToInt (aRampOffTimeSec * sourceSampleRate);
}

BKFixedNotePianoSamplerSound::~BKFixedNotePianoSamplerSound()
{
}

bool BKFixedNotePianoSamplerSound::appliesToNote (int midiNoteNumber)
{
    return midiNotes [midiNoteNumber];
}

bool BKFixedNotePianoSamplerSound::appliesToVelocity(int midiNoteVelocity)
{
    return midiVelocities [midiNoteVelocity];
}

bool BKFixedNotePianoSamplerSound::appliesToChannel (int /*midiChannel*/)
{
    return true;
}

//==============================================================================
BKFixedNotePianoSamplerVoice::BKFixedNotePianoSamplerVoice() : pitchRatio (0.0),
sourceSamplePosition (0.0),
lgain (0.0f), rgain (0.0f),
rampOnOffLevel (0),
rampOnDelta (0),
rampOffDelta (0),
isInRampOn (false), isInRampOff (false)
{
}

BKFixedNotePianoSamplerVoice::~BKFixedNotePianoSamplerVoice()
{
}

bool BKFixedNotePianoSamplerVoice::canPlaySound (BKFixedNoteSynthesiserSound* sound)
{
    return dynamic_cast<const BKFixedNotePianoSamplerSound*> (sound) != nullptr;
}

void BKFixedNotePianoSamplerVoice::startNote (const int midiNoteNumber,
                                         const float velocity,
                                         PianoSamplerNoteType type,
                                         const uint32 length,
                                         BKFixedNoteSynthesiserSound* s
                                         /*, const int */)
{
    if (const BKFixedNotePianoSamplerSound* const sound = dynamic_cast<const BKFixedNotePianoSamplerSound*> (s))
    {
        pitchRatio = pow (2.0, (midiNoteNumber - sound->midiRootNote) / 12.0)
        * sound->sourceSampleRate / getSampleRate();
        
        sourceSamplePosition = 0.0;
        lgain = velocity;
        rgain = velocity;
        
        isInRampOn = (sound->rampOnSamples > 0);
        isInRampOff = false;
        
        playType = type;
        if (playType == ForwardFixed) {
            playLength = length;
        } else {
            playLength = 30.0;
        }
        
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



void BKFixedNotePianoSamplerVoice::stopNote (float /*velocity*/, bool allowTailOff)
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

void BKFixedNotePianoSamplerVoice::pitchWheelMoved (const int /*newValue*/)
{
}

void BKFixedNotePianoSamplerVoice::controllerMoved (const int /*controllerNumber*/,
                                               const int /*newValue*/)
{
}

//==============================================================================
void BKFixedNotePianoSamplerVoice::renderNextBlock (AudioSampleBuffer& outputBuffer, int startSample, int numSamples)
{
    if (const BKFixedNotePianoSamplerSound* const playingSound = static_cast<BKFixedNotePianoSamplerSound*> (getCurrentlyPlayingSound().get()))
    {
        
        const float* const inL = playingSound->data->getAudioSampleBuffer()->getReadPointer (0);
        const float* const inR = playingSound->data->getAudioSampleBuffer()->getNumChannels() > 1
        ? playingSound->data->getAudioSampleBuffer()->getReadPointer (1) : nullptr;
        
        
        float* outL = outputBuffer.getWritePointer (0, startSample);
        float* outR = outputBuffer.getNumChannels() > 1 ? outputBuffer.getWritePointer (1, startSample) : nullptr;
        
        while (--numSamples >= 0)
        {
            const int pos = (int) sourceSamplePosition;
            const float alpha = (float) (sourceSamplePosition - pos);
            const float invAlpha = 1.0f - alpha;
            
            // just using a very simple linear interpolation here..
            float l = (inL [pos] * invAlpha + inL [pos + 1] * alpha);
            float r = (inR != nullptr) ? (inR [pos] * invAlpha + inR [pos + 1] * alpha) : l;
            
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
                    stopNote (0.0f, false);
                    break;
                }
            }
            
            if (outR != nullptr)
            {
                *outL++ += l;
                *outR++ += r;
            }
            else
            {
                *outL++ += (l + r) * 0.5f;
            }
            
            sourceSamplePosition += pitchRatio;
            
            if (playType == ForwardFixed)
            {
                if (!isInRampOff && sourceSamplePosition > playLength)
                {
                    stopNote (0.0f, true);
                }
            }
        }
    }
}


