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
pitchbendMultiplier(1.),
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
                                     uint64 voiceRampOn,
                                     uint64 voiceRampOff,
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

/*
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
        
        double playLength = 0.0;
        double maxLength = sound->soundLength - adsrRelease;
        
        
        if (bkType != MainNote)
        {

            //should redo this to scale ADSR rather than just set this way...
            //if(adsrAttack  > (0.5 * length))   adsrAttack     = 0.5 * length;
            //if(adsrRelease > (0.5 * length))   adsrRelease    = 0.5 * length;
 
            //playLength = (length - adsrRelease) * pitchRatio;

 
            // ******
 
            playLength = length * pitchRatio;
            if(playLength < .05 * getSampleRate()) playLength = .05 * getSampleRate();
 
            int envLen = adsrAttack + adsrDecay + adsrRelease;
            if(envLen > playLength) {
                //adsrAttack = adsrAttack * playLength / envLen;
                //adsrDecay = adsrDecay * playLength / envLen;
                //adsrRelease = adsrRelease * playLength / envLen;
                adsrAttack     = 0.5 * playLength;
                adsrDecay      = 0.;
                adsrSustain    = 1.;
                adsrRelease    = 0.5 * playLength;
            }
 
            if(adsrAttack < .003 * getSampleRate()) adsrAttack = .003 * getSampleRate();
            if(adsrDecay < .003 * getSampleRate()) adsrDecay = .003 * getSampleRate();
            if(adsrRelease < .003 * getSampleRate()) adsrRelease = .003 * getSampleRate();
 
            playLength = playLength - adsrRelease * pitchRatio;
        }
         
        
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
                playEndPosition = adsrRelease;
            }
            else if (playType == NormalFixedStart)
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

                playEndPosition = adsrRelease;
            }
            else if (playType == FixedLength)
            {
                sourceSamplePosition = sound->soundLength - 1;
                if (playLength >= sourceSamplePosition)
                {
                    playEndPosition = (double)adsrRelease;
                }
                else
                {
                    playEndPosition = (double)(sourceSamplePosition - playLength);
                }
            }
            else if (playType == FixedLengthFixedStart)
            {
                //sourceSamplePosition = startingPosition * pitchRatio;
                sourceSamplePosition = (startingPosition + adsrRelease) * pitchRatio;
                if (playLength >= sourceSamplePosition)
                {
                    //playEndPosition = (double)adsrRelease;
                    playEndPosition = (double)adsrRelease * pitchRatio;
                }
                else
                {
                    playEndPosition = (double)(sourceSamplePosition - playLength);
                }
                //if(playEndPosition < adsrRelease * pitchRatio) playEndPosition = adsrRelease * pitchRatio;
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
        adsr.setAllTimes(adsrAttack / (getSampleRate() * pitchRatio),
                         adsrDecay / (getSampleRate() * pitchRatio),
                         adsrSustain,
                         adsrRelease / (getSampleRate() * pitchRatio));

        adsr.keyOn();
        //DBG("ADSR vals = " + String(adsr.getAttackRate()) + " " + String(adsr.getDecayRate()) + " " + String(adsr.getSustainLevel()) + " " + String(adsr.getReleaseRate()) + " " );
        
        noteStartingPosition = sourceSamplePosition;
        noteEndPosition = playEndPosition;
    }
    else
    {
        jassertfalse; // this object can only play BKSamplerSounds!
    }
}
*/

void BKPianoSamplerVoice::startNote (const float midiNoteNumber,
                                     const float gain,
                                     PianoSamplerNoteDirection direction,
                                     PianoSamplerNoteType type,
                                     BKNoteType bktype,
                                     const uint64 startingPosition,
                                     const uint64 length,  //total desired playlength, including ADSR times
                                     uint64 adsrAttack,
                                     uint64 adsrDecay,
                                     float adsrSustain,
                                     uint64 adsrRelease,
                                     BKSynthesiserSound* s)
{
    if (const BKPianoSamplerSound* const sound = dynamic_cast<const BKPianoSamplerSound*> (s))
    {
        
        pitchRatio  = powf(2.0f, (midiNoteNumber - (float)sound->midiRootNote) / 12.0f)
                    * sound->sourceSampleRate
                    * generalSettings->getTuningRatio()
                    / getSampleRate();
        
        DBG("PITCHRATIO = " + String(pitchRatio));
        
        bkType = bktype;
        playType = type;
        playDirection = direction;
        
        double playLength = 0.0;
        uint64 totalLength = length;

        if (bkType != MainNote)
        {

            //constrain total length minimum to no less than 50ms
            if(totalLength < .05 * getSampleRate()) totalLength = .05 * getSampleRate();
            
            //constrain adsr times
            uint64 envLen = adsrAttack + adsrDecay + adsrRelease;
            if(envLen > totalLength) {
                adsrAttack = adsrAttack * totalLength / envLen;
                adsrDecay = adsrDecay * totalLength / envLen;
                adsrRelease = adsrRelease * totalLength / envLen;
            }

            //set min adsrTimes, based on 50ms minimum note size.
            if(adsrAttack < .01 * getSampleRate()) adsrAttack = .01 * getSampleRate();
            if(adsrDecay < .003 * getSampleRate()) adsrDecay = .003 * getSampleRate();
            if(adsrRelease < .037 * getSampleRate()) adsrRelease = .037 * getSampleRate();

            //playLength => how long to play before keyOff/adsrRelease, accounting for playbackSpeed (pitchRatio)
            playLength = (totalLength - adsrRelease) * pitchRatio;

        }
        
        //playLength should now be the actual duration we want to hear, minus the release time, all scaled for playbackSpeed.
        
        //actual maxLength, based on soundfile size, leaving enough samples for release
        double maxLength = sound->soundLength - adsrRelease * pitchRatio;
        
        
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
                playEndPosition = adsrRelease * pitchRatio;
            }
            else if (playType == NormalFixedStart)
            {
                if (startingPosition < adsrRelease)
                {
                    sourceSamplePosition = adsrRelease * pitchRatio;
                }
                else if (startingPosition >= sound->soundLength)
                {
                    sourceSamplePosition = (sound->soundLength - 1);
                }
                else
                {
                    sourceSamplePosition = startingPosition;
                }
                
                playEndPosition = adsrRelease;
            }
            else if (playType == FixedLength)
            {
                sourceSamplePosition = sound->soundLength - 1;
                if (playLength >= sourceSamplePosition)
                {
                    playEndPosition = (double)adsrRelease * pitchRatio;
                }
                else
                {
                    playEndPosition = (double)(sourceSamplePosition - playLength);
                }
            }
            else if (playType == FixedLengthFixedStart)
            {
                sourceSamplePosition = startingPosition * pitchRatio;
                if (playLength >= sourceSamplePosition)
                {
                    playEndPosition = (double)adsrRelease * pitchRatio;
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
        adsr.setAllTimes(adsrAttack / getSampleRate(),
                         adsrDecay / getSampleRate(),
                         adsrSustain,
                         adsrRelease / getSampleRate());
        
        adsr.keyOn();
        //DBG("ADSR vals = " + String(adsr.getAttackRate()) + " " + String(adsr.getDecayRate()) + " " + String(adsr.getSustainLevel()) + " " + String(adsr.getReleaseRate()) + " " );
        
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

void BKPianoSamplerVoice::pitchWheelMoved (const int newValue)
{
    pitchbendMultiplier = powf(2.0f, (newValue / 8192. - 1.)/12.);
    //DBG("BKPianoSamplerVoice::pitchWheelMoved " + String(pitchbendMultiplier));
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
                               ? playingSound->data->getAudioSampleBuffer()->getReadPointer (1)
                               : nullptr;
        
        float* outL = outputBuffer.getWritePointer (0, startSample);
        float* outR = outputBuffer.getNumChannels() > 1 ? outputBuffer.getWritePointer (1, startSample) : nullptr;
        
        double bentRatio = pitchRatio * pitchbendMultiplier;
        
        while (--numSamples >= 0)
        {
            //if (playDirection == Reverse && sourceSamplePosition > playingSound->soundLength)
            if (playDirection == Reverse && sourceSamplePosition >= playingSound->soundLength - 1)
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
                sourceSamplePosition -= bentRatio;
                continue;
            }
            
            if(sourceSamplePosition < 0) sourceSamplePosition = 0;
            
            const int pos = (int) sourceSamplePosition;
            const float alpha = (float) (sourceSamplePosition - pos);
            const float invAlpha = 1.0f - alpha;
            
            // just using a very simple linear interpolation here..
            float l = (inL [pos] * invAlpha + inL [pos + 1] * alpha);
            float r = (inR != nullptr) ? (inR [pos] * invAlpha + inR [pos + 1] * alpha) : l;
            
            l *= lgain;
            r *= rgain;

            l *= adsr.tick();
            r *= adsr.lastOut();
            
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
                sourceSamplePosition += bentRatio;

                if (sourceSamplePosition >= playEndPosition)
                {
                    if (adsr.getState() != stk::ADSR::RELEASE)
                    {
                        adsr.keyOff();
                    }
                }
                if(sourceSamplePosition >= playingSound->soundLength)
                {
                    clearCurrentNote();
                }
            }
            else if (playDirection == Reverse)
            {
                sourceSamplePosition -= bentRatio;
         
                if (sourceSamplePosition <= playEndPosition)
                {
                    if (adsr.getState() != stk::ADSR::RELEASE)
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
}


