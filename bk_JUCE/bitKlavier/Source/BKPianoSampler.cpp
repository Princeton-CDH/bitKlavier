/*
 ==============================================================================
 
 BKPianoSampler.cpp
 Created: 19 Oct 2016 10:00:04am
 Author:  Michael R Mulshine
 
 ==============================================================================
 */

#include "BKPianoSampler.h"
#include "AudioConstants.h"

#define FADE 4

BKPianoSamplerSound::BKPianoSamplerSound (const String& soundName,
                                          BKReferenceCountedBuffer::Ptr buffer,
                                          uint64 soundLength,
                                          double sourceSampleRate,
                                          const BigInteger& notes,
                                          const int rootMidiNote,
                                          const int transp,
                                          const BigInteger& velocities,
                                          sfzero::Region* reg)
:
name (soundName),
data(buffer),
sourceSampleRate(sourceSampleRate),
midiNotes (notes),
midiVelocities(velocities),
soundLength(soundLength),
midiRootNote (rootMidiNote),
transpose(transp)
{
    rampOnSamples = roundToInt (aRampOnTimeSec* sourceSampleRate);
    rampOffSamples = roundToInt (aRampOffTimeSec * sourceSampleRate);

    
    if (reg != nullptr)
    {
        isSoundfont = true;
        
        loopStart = reg->loop_start; // loop start and end take in account minimum fade amt
        loopEnd = reg->loop_end;
        start = reg->offset;
        end = reg->end;
        
        attack = reg->ampeg.attack;
        decay = reg->ampeg.decay;
        sustain = reg->ampeg.sustain / 100.0f;
        release = reg->ampeg.release;
        
        loopMode = reg->loop_mode;
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
        pitchRatio = powf(2.0f, (midiNoteNumber - (float)sound->midiRootNote + sound->transpose) / 12.0f)
        * sound->sourceSampleRate
        * generalSettings->getTuningRatio()
        / getSampleRate();
        
        bentRatio = pitchbendMultiplier * pitchRatio;
        
        bkType = bktype;
        playType = type;
        playDirection = direction;
        
        revRamped = false;
        
        playLength = length * pitchRatio;
        
        uint64 totalLength = length;
        
        if (sound->isSoundfont)
        {
            if (bkType != MainNote)
            {
                //should redo this to scale ADSR rather than just set this way...
                if(adsrAttack  > (0.5 * length))   adsrAttack     = 0.5 * length;
                if(adsrRelease > (0.5 * length))   adsrRelease    = 0.5 * length;
                
                playLength = (length - adsrRelease) * pitchRatio;
            }
        }
        else
        {
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
        }
        
        lgain = gain;
        rgain = gain;
        
        lengthTracker = 0.0;
        
        inLoop = false;
        
        adsr.setSampleRate(getSampleRate());
        
        adsr.setAllTimes(adsrAttack / getSampleRate(),
                         adsrDecay / getSampleRate(),
                         adsrSustain,
                         adsrRelease / getSampleRate());
        
        adsr.keyOn();
        
        if (sound->isSoundfont)
        {
            cfSamples = 50.0;
            sampleEnv.setTime(cfSamples / getSampleRate());
            loopEnv.setTime(cfSamples / getSampleRate());
            
            DBG("loop mode: " + String(sound->loopMode));
            
            if (playDirection == Forward)
            {
                inLoop = false;
                samplePosition = 0.0;
                loopPosition = sound->loopStart * pitchRatio;
                sampleEnv.setValue(1.0f);
                loopEnv.setValue(0.0f);
                
                sfzadsr.setSampleRate(getSampleRate());
                
                sfzadsr.setAllTimes((sound->attack > 0.0f) ? sound->attack : 0.001f,
                                    (sound->decay > 0.0f) ? sound->decay : 0.001f,
                                    sound->sustain,
                                    (sound->release > 0.0f) ? sound->release : 0.001f );
                
                sfzEnvApplied = true;
                sfzadsr.keyOn();
            }
            else
            {
                inLoop = true;
                if (sound->loopMode == 1 || sound->loopMode == 2)
                {
                    samplePosition = playLength - 1;
                    loopEnv.setValue(0.0);
                    sampleEnv.setValue(1.0);
                }
                else // loop
                {
                    samplePosition = 0.0;
                    loopEnv.setValue(1.0f);
                    sampleEnv.setValue(0.0f);
                    
                    loopPosition = sound->loopStart * pitchRatio;
                    
                    lengthEnv =  (sound->attack + sound->decay + sound->release) * getSampleRate();
                    
                    lengthEnv = (playLength < lengthEnv) ? playLength : lengthEnv;
                    float lengthEnvSec = lengthEnv / getSampleRate();
                    
                    sfzadsr.setValue(0.0f);
                    if (lengthEnvSec <= 0.005f)
                    {
                        sfzadsr.setAllTimes(0.005f, 0.001f, 1.0f, 0.005f);
                        sfzadsr.keyOn();
                        sfzEnvApplied = true;
                    }
                    else
                    {
                        sfzadsr.setAllTimes(lengthEnvSec - 0.01f, 0.01f, 0.0f, 0.001f);
                        sfzEnvApplied = false;
                    }
                    
                    
                }
            }
        }
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
        sfzadsr.keyOff();
    }
    else
    {
        clearCurrentNote();
    }
}

void BKPianoSamplerVoice::pitchWheelMoved (const int newValue)
{
    pitchbendMultiplier = powf(2.0f, (newValue / 8192. - 1.)/12.);
    bentRatio = pitchRatio * pitchbendMultiplier;
    //DBG("BKPianoSamplerVoice::pitchWheelMoved " + String(pitchbendMultiplier));
}

void BKPianoSamplerVoice::controllerMoved (const int /*controllerNumber*/,
                                           const int /*newValue*/)
{
}

//==============================================================================
void BKPianoSamplerVoice::processSoundfontLoop(AudioSampleBuffer& outputBuffer,
                                               int startSample, int numSamples,
                                               const BKPianoSamplerSound* playingSound)
{
    
    const float* const inL = playingSound->data->getAudioSampleBuffer()->getReadPointer (0);
    const float* const inR = playingSound->data->getAudioSampleBuffer()->getNumChannels() > 1
    ? playingSound->data->getAudioSampleBuffer()->getReadPointer (1)
    : nullptr;
    
    float* outL = outputBuffer.getWritePointer (0, startSample);
    float* outR = outputBuffer.getNumChannels() > 1 ? outputBuffer.getWritePointer (1, startSample) : nullptr;
    
    int64 loopStart, loopEnd, start, end;
    
    double bentRatio = pitchRatio * pitchbendMultiplier;
    
    start = playingSound->start;
    end = playingSound->end;
    loopStart = playingSound->loopStart;
    loopEnd = playingSound->loopEnd;
    
    while (--numSamples >= 0)
    {
        // always increment length
        lengthTracker += bentRatio;
        
        // We always tick loop area sample and begin/end area samples.
        // We envelope on and off each area when needed, and reset sample positions for begin/end.
        //================LOOP STUFF=================
        loopPosition += bentRatio;
        if (loopPosition >= loopEnd)
        {
            loopPosition = loopStart;
        }
        
        float loopL, loopR;
        
        fadeTracker = loopPosition - loopEnd + FADE;
        
        int pos = (int) loopPosition;
        float alpha = (float) (loopPosition - pos);
        float invAlpha = 1.0f - alpha;
        
        if (fadeTracker > 0.0)
        {
            float fade = (float)(fadeTracker / FADE);
            
            int fadePos = (int) fadeTracker;
            const float fadeAlpha = (float) (fadeTracker - fadePos);
            const float fadeInvAlpha = 1.0f - fadeAlpha;
            
            fadePos += loopStart-FADE;
            
            loopL = (1.0f - fade) * (inL [pos] * invAlpha + inL [pos + 1] * alpha);
            loopR = (1.0f - fade) * ((inR != nullptr) ? (inR [pos] * invAlpha + inR [pos + 1] * alpha) : loopL);
            
            loopL += fade * (inL [fadePos] * fadeInvAlpha + inL [fadePos+1] * fadeAlpha);
            loopR += fade * ((inR != nullptr) ? (inR [fadePos] * fadeInvAlpha + inR [fadePos+1] * fadeAlpha) : loopL);
            
        }
        else
        {
            loopL = (inL [pos] * invAlpha + inL [pos + 1] * alpha);
            loopR = (inR != nullptr) ? (inR [pos] * invAlpha + inR [pos + 1] * alpha) : loopL;
        }
        //===========================================
        
        //==============SAMPLE STUFF=================
        float sampleL, sampleR;
        if (playDirection == Forward)   samplePosition += bentRatio;
        else                            samplePosition -= bentRatio;
        
        pos = (int) samplePosition;
        alpha = (float) (samplePosition - pos);
        invAlpha = 1.0f - alpha;
        
        if (pos >= 0 && pos < (playingSound->soundLength - 1))
        {
            sampleL = (inL [pos] * invAlpha + inL [pos + 1] * alpha);
            sampleR = (inR != nullptr) ? (inR [pos] * invAlpha + inR [pos + 1] * alpha) : sampleL;
        }
        //===========================================
        if (playDirection == Forward)
        {
            if (adsr.getState() == stk::ADSR::IDLE)
            {
                stopNote(0.0f, false);
                break;
            }
            
            // if we are about to enter loop, turn on loopEnv, turn off sampleEnv
            if (!inLoop && (samplePosition >= (loopStart - cfSamples)))
            {
                inLoop = true;
                
                loopPosition = samplePosition;
                
                loopEnv.keyOn();
                
                sampleEnv.keyOff();
            }
            
            if (playType != Normal)
            {
                if ((adsr.getState() != stk::ADSR::RELEASE) && (lengthTracker >= (playLength - (0.005f * getSampleRate()))))
                {
                    adsr.keyOff();
                    sfzadsr.keyOff();
                }
            }
            
            
        }
        else if (playDirection == Reverse)
        {
            if(lengthTracker >= playLength)
            {
                clearCurrentNote(); break;
            }
            
            int64 reversePosition = playLength - lengthTracker;
            
            if (!sfzEnvApplied && (reversePosition <= lengthEnv))
            {
                sfzEnvApplied = true;
                sfzadsr.keyOn();
            }
            
            // if we are about to leave loop, turn off loopEnv, turn on sampleEnv
            if (inLoop && (reversePosition <= (loopStart + cfSamples)))
            {
                inLoop = false;
                
                samplePosition = reversePosition;
                
                loopEnv.keyOff();
                
                sampleEnv.keyOn();
            }
            
            if ((playType != Normal) && (lengthTracker >= (playLength - FADE)))
            {
                if ((adsr.getState() != stk::ADSR::RELEASE) && (adsr.getState() != stk::ADSR::IDLE))
                {
                    adsr.keyOff();
                    sfzadsr.keyOff();
                }
            }
        }
        else
        {
            DBG("Invalid note direction.");
        }
        
        float l = lgain * adsr.tick() * sfzadsr.tick() * (loopL * loopEnv.tick() + sampleL * sampleEnv.tick());
        float r = rgain * adsr.lastOut() * sfzadsr.lastOut() * (loopR * loopEnv.lastOut() + sampleR * sampleEnv.lastOut());
        
        if (outR != nullptr)
        {
            *outL++ += (l);
            *outR++ += (r);
        }
        else
        {
            *outL++ += ((l + r) * 0.5f);
        }
    }
}

void BKPianoSamplerVoice::processSoundfontNoLoop(AudioSampleBuffer& outputBuffer,
                                                 int startSample, int numSamples,
                                                 const BKPianoSamplerSound* playingSound)
{
    const float* const inL = playingSound->data->getAudioSampleBuffer()->getReadPointer (0);
    const float* const inR = playingSound->data->getAudioSampleBuffer()->getNumChannels() > 1
    ? playingSound->data->getAudioSampleBuffer()->getReadPointer (1)
    : nullptr;
    
    float* outL = outputBuffer.getWritePointer (0, startSample);
    float* outR = outputBuffer.getNumChannels() > 1 ? outputBuffer.getWritePointer (1, startSample) : nullptr;
    
    int64 loopStart, loopEnd, start, end;
    
    double bentRatio = pitchRatio * pitchbendMultiplier;
    
    start = playingSound->start;
    end = playingSound->end;
    loopStart = playingSound->loopStart;
    loopEnd = playingSound->loopEnd;
    
    while (--numSamples >= 0)
    {
        // always increment length
        lengthTracker += bentRatio;
        
        //==============SAMPLE STUFF=================
        float sampleL, sampleR;
        if (playDirection == Forward)   samplePosition += bentRatio;
        else                            samplePosition -= bentRatio;
        
        const int pos = (int) samplePosition;
        const float alpha = (float) (samplePosition - pos);
        const float invAlpha = 1.0f - alpha;
        
        if (pos >= 0 && pos < (playingSound->soundLength - 1))
        {
            sampleL = (inL [pos] * invAlpha + inL [pos + 1] * alpha);
            sampleR = (inR != nullptr) ? (inR [pos] * invAlpha + inR [pos + 1] * alpha) : sampleL;
        }
        //===========================================
        if (playDirection == Forward)
        {
            if (adsr.getState() == stk::ADSR::IDLE)
            {
                stopNote(0.0f, false);
                break;
            }
            
            if (playType != Normal)
            {
                if ((adsr.getState() != stk::ADSR::RELEASE) && (lengthTracker >= (playLength-FADE)))
                {
                    adsr.keyOff();
                }
            }
            
        }
        else if (playDirection == Reverse)
        {
            if(lengthTracker >= playLength)
            {
                clearCurrentNote(); break;
            }
            
            if ((playType != Normal) && (lengthTracker >= (playLength - FADE)))
            {
                if ((adsr.getState() != stk::ADSR::RELEASE) && (adsr.getState() != stk::ADSR::IDLE))
                {
                    adsr.keyOff();
                }
            }
        }
        else
        {
            DBG("Invalid note direction.");
        }
        
        float l = lgain * adsr.tick() * (sampleL * sampleEnv.tick());
        float r = rgain * adsr.lastOut() * (sampleR * sampleEnv.lastOut());
        
        if (outR != nullptr)
        {
            *outL++ += (l);
            *outR++ += (r);
        }
        else
        {
            *outL++ += ((l + r) * 0.5f);
        }
    }
}

void BKPianoSamplerVoice::renderNextBlock (AudioSampleBuffer& outputBuffer, int startSample, int numSamples)
{
    if (const BKPianoSamplerSound* const playingSound = static_cast<BKPianoSamplerSound*> (getCurrentlyPlayingSound().get()))
    {
        if (playingSound->isSoundfont)
        {
            if (playingSound->loopMode == 1 || playingSound->loopMode == 2)
            {
                processSoundfontNoLoop(outputBuffer, startSample, numSamples, playingSound);
            }
            else
            {
                processSoundfontLoop(outputBuffer, startSample, numSamples, playingSound);
            }
        }
        else
        {
            processPiano(outputBuffer, startSample, numSamples, playingSound);
        }
        
    }
}

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
    
    double bentRatio = pitchRatio * pitchbendMultiplier;
    
    while (--numSamples >= 0)
    {
        if ((adsr.getState() != stk::ADSR::IDLE) &&
            (playDirection == Reverse) &&
            (sourceSamplePosition > playingSound->soundLength - 1))
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
                //clearCurrentNote();
                stopNote(0.0f, true);
            }
        }
        else if (playDirection == Reverse)
        {
            sourceSamplePosition -= bentRatio;
            
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

