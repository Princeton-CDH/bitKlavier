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
                                          const BigInteger& velocities)
: name (soundName),
data(buffer),
sourceSampleRate(sourceSampleRate),
midiNotes (notes),
midiVelocities(velocities),
soundLength(soundLength),
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
    if (const BKPianoSamplerSound* const sound = dynamic_cast<const BKPianoSamplerSound*> (s))
    {
        
        //DBG(sound->getName());
        //DBG("startNote: getting tuning ratio " + String(generalSettings->getTuningRatio()));
        
        pitchRatio = powf(2.0f, (midiNoteNumber - (float)sound->midiRootNote) / 12.0f)
                        * sound->sourceSampleRate
                        * generalSettings->getTuningRatio()
                        / getSampleRate();
        
        DBG("pitch ratio: " + String(pitchRatio) + String(" ssr: ") + String(sound->sourceSampleRate) + " tuning ratio: " + String(generalSettings->getTuningRatio()));
        
        bkType = bktype;
        playType = type;
        playDirection = direction;
        
        double playLength = 0.0;
        double maxLength = sound->soundLength - voiceRampOff;
        
        if (bkType != MainNote)
        {
            if(voiceRampOn  > (0.5 * length))   voiceRampOn     = 0.5 * length;
            if(voiceRampOff > (0.5 * length))   voiceRampOff    = 0.5 * length;
            
            //playLength = (length - voiceRampOff) * pitchRatio;
            playLength = (length - (voiceRampOff + voiceRampOn)) * pitchRatio; 
        }
        
        if (playDirection == Forward)
        {
            if (playType == Normal)
            {
                sourceSamplePosition = 220; //was 0.0
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
                /*
                 DBG("starting forward note, starting position = "
                 + std::to_string(startingPosition * 1000./getSampleRate())
                 + " ending position = "
                 + std::to_string(playEndPosition * 1000./getSampleRate())
                 );
                 */
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
                playEndPosition = voiceRampOff;
            }
            else if (playType == NormalFixedStart)
            {
                //if (startingPosition < sound->rampOffSamples)
                if (startingPosition < voiceRampOff)
                {
                    //sourceSamplePosition = sound->rampOffSamples;
                    sourceSamplePosition = voiceRampOff;
                }
                else if (startingPosition >= sound->soundLength)
                {
                    sourceSamplePosition = (sound->soundLength - 1);
                }
                else
                {
                    sourceSamplePosition = startingPosition;
                }
                //playEndPosition = sound->rampOffSamples;
                playEndPosition = voiceRampOff;
            }
            else if (playType == FixedLength)
            {
                sourceSamplePosition = sound->soundLength - 1;
                if (playLength >= sourceSamplePosition)
                {
                    //playEndPosition = (double)sound->rampOffSamples;
                    playEndPosition = (double)voiceRampOff;
                }
                else
                {
                    playEndPosition = (double)(sourceSamplePosition - playLength);
                }
            }
            else if (playType == FixedLengthFixedStart)
            {
                //sourceSamplePosition = startingPosition; //* pitchRatio! do this here, instead of outside as it is now?
                sourceSamplePosition = startingPosition * pitchRatio;
                if (playLength >= sourceSamplePosition)
                {
                    //playEndPosition = (double)sound->rampOffSamples;
                    playEndPosition = (double)voiceRampOff;
                }
                else
                {
                    //playEndPosition = (double)(startingPosition - playLength);
                    playEndPosition = (double)(startingPosition * pitchRatio - playLength);
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
        
        isInRampOn = (voiceRampOn > 0);
        isInRampOff = false;
        
        noteStartingPosition = sourceSamplePosition;
        noteEndPosition = playEndPosition;
        
        
        if (isInRampOn)
        {
            rampOnOffLevel = 0.0f;
            //rampOnDelta = (float) (pitchRatio / sound->rampOnSamples);
            rampOnDelta = (float) (pitchRatio / voiceRampOn);
        }
        else
        {
            rampOnOffLevel = 1.0f;
            rampOnDelta = 0.0f;
        }
        
        if (voiceRampOff > 0)
        {
            rampOffDelta = (float) (-pitchRatio / voiceRampOff);
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
            
            if (playDirection == Reverse && sourceSamplePosition > playingSound->soundLength)
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
                    isInRampOn = false; //wasn't here before!
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
                        //DBG("stopping forward note, playEndPosition = " + std::to_string(playEndPosition * 1000./getSampleRate()));
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


