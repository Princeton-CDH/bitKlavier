/*
 ==============================================================================
 
 BKPianoSampler.cpp
 Created: 19 Oct 2016 10:00:04am
 Author:  Michael R Mulshine
 
 ==============================================================================
 */

#include "BKPianoSampler.h"
#include "AudioConstants.h"

#define FADE 10

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
        
        delay = reg->ampeg.delay;
        attack = reg->ampeg.attack;
        hold = reg->ampeg.hold;
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
                                     64,
                                     gain,
                                     direction,
                                     type,
                                     bktype,
                                     startingPosition,
                                     length,
                                     voiceRampOn,
                                     (int)(3*getSampleRate()),
                                     1.0,
                                     voiceRampOff,
                                     s);
}

#define REVENV 0

void BKPianoSamplerVoice::startNote (const float midiNoteNumber,
                                     const int pitchWheelValue,
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
        
        pitchbendMultiplier = powf(2.0f, (pitchWheelValue / 8192. - 1.)/12.);
        
        pitchRatio = powf(2.0f, (midiNoteNumber - (float)sound->midiRootNote + sound->transpose) / 12.0f)
        * sound->sourceSampleRate
        * generalSettings->getTuningRatio()
        / getSampleRate();
        
        bentRatio = pitchbendMultiplier * pitchRatio;
        
        bkType = bktype;
        playType = type;
        playDirection = direction;
        revRamped = false;
    
        offset = startingPosition - length;
        offset = (offset > 0.0) ? offset : 0.0;
        
        uint64 totalLength = length;
        
        if (bkType != MainNote)
        {
            
            //constrain total length minimum to no less than 50ms
            if(totalLength < 0.05 * getSampleRate()) totalLength = 0.05 * getSampleRate();
            
            //constrain adsr times
            uint64 envLen = adsrAttack + adsrDecay + adsrRelease;
            if(envLen > totalLength) {
                adsrAttack = adsrAttack * totalLength / envLen;
                adsrDecay = adsrDecay * totalLength / envLen;
                adsrRelease = adsrRelease * totalLength / envLen;
            }
            
            //set min adsrTimes, based on 50ms minimum note size.
            if(adsrAttack < 0.01f * getSampleRate()) adsrAttack = 0.01f * getSampleRate();
            if(adsrDecay < 0.003f * getSampleRate()) adsrDecay = 0.003f * getSampleRate();
            if(adsrRelease < 0.037f * getSampleRate()) adsrRelease = 0.037f * getSampleRate();
            
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
            samplePosition = sourceSamplePosition; //DT addition
            
            lgain *= 0.5;
            rgain *= 0.5;
            
            //cfSamples = 10.0;  //DT: 10 samples too small...
            cfSamples = getSampleRate() / 50.; //20ms; possibly an issue if the loop length is really small, but 20ms is a typical ramp length
            sampleEnv.setTime(cfSamples / getSampleRate());
            loopEnv.setTime(cfSamples / getSampleRate());
            
            DBG("loop mode: " + String(sound->loopMode));
            
            if (playDirection == Forward)
            {
                inLoop = false;
                //samplePosition = 0.0;
                loopPosition = sound->loopStart;
                sampleEnv.setValue(1.0f);
                loopEnv.setValue(0.0f);
                
                sfzadsr.setSampleRate(getSampleRate());
                
                float sustain = sound->sustain;
                if ((sound->attack == 0.0f) && (sound->decay == 0.0f) && (sound->release == 0.0f))
                {
                    sustain = 1.0f;
                }
                sfzadsr.setAttackTarget(1.0f);
                sfzadsr.setAllTimes((sound->attack > 0.0f) ? sound->attack : 0.001f,
                                    (sound->hold > 0.0f) ? sound->hold : 0.001f,
                                    (sound->decay > 0.0f) ? sound->decay : 0.001f,
                                    sustain,
                                    (sound->release > 0.0f) ? sound->release : 0.001f );
                
                playLengthSF2 = (totalLength - (sound->release * getSampleRate())) * pitchRatio;
                
                sfzEnvApplied = true;
                sfzadsr.keyOn();
            }
            else
            {
                inLoop = true;
                
                if (sound->loopMode <=  2)
                {
                    //samplePosition = playLength - 1;
                    loopEnv.setValue(0.0);
                    sampleEnv.setValue(1.0);
                }
                else // loop
                {
                    loopEnv.setValue(1.0f);
                    sampleEnv.setValue(0.0f);
                    
                    loopPosition = sound->loopStart;
                    
#if REVENV
                    // total length of sound minus the length of the AHDR should be how long sample was sustained (hold in reverse)
                    double totalLen = (totalLength * pitchRatio);
                    double envLen = ((sound->release + sound->decay + sound->hold + sound->attack) * getSampleRate());
                    double envLenNoRelease = ((sound->release + sound->decay + sound->hold + sound->attack) * getSampleRate());

                    if (totalLen > envLen)
                    {
                        double holdSamples = totalLen - envLen;
                        sfzadsr.setAttackTarget(sound->sustain);
                        sfzadsr.setAllTimes(sound->release,
                                            (holdSamples / getSampleRate()), // hold length in seconds
                                            sound->decay,
                                            1.0f,
                                            sound->attack);
                    }
                    else
#endif
                    {
                        sfzadsr.setAttackTarget(1.0f);
                        sfzadsr.setAllTimes(0.001f,
                                            0.001f,
                                            0.001f,
                                            1.0f,
                                            0.001f);
                    }
                    
                    sfzadsr.keyOn();
                    
                    sfzEnvApplied = true;
                    
                }
                
                playLengthSF2 = (totalLength - (sound->attack * getSampleRate())) * pitchRatio;
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
        //sfzadsr.keyOff();
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
    DBG("BKPianoSamplerVoice::pitchWheelMoved " + String(pitchbendMultiplier));
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
        
        if(loopPosition < 0) loopPosition = 0;
        if(loopPosition > playingSound->soundLength - 2) loopPosition = playingSound->soundLength - 2;
        
        int pos = (int) loopPosition;
        float alpha = (float) (loopPosition - pos);
        float invAlpha = 1.0f - alpha;
        int next = pos + 1;
        
        loopL = (inL [pos] * invAlpha + inL [next] * alpha);
        loopR = (inR != nullptr) ? (inR [pos] * invAlpha + inR [next] * alpha) : loopL;
        //===========================================
        
        //==============SAMPLE STUFF=================
        float sampleL, sampleR;
        if (playDirection == Forward)   samplePosition += bentRatio;
        else                            samplePosition -= bentRatio;
        
        if(samplePosition < 0) samplePosition = 0;
        if(samplePosition > playingSound->soundLength - 2) samplePosition = playingSound->soundLength - 2;
        
        pos = (int) samplePosition;
        alpha = (float) (samplePosition - pos);
        invAlpha = 1.0f - alpha;
        next = pos + 1;

        sampleL = (inL [pos] * invAlpha + inL [next] * alpha);
        sampleR = (inR != nullptr) ? (inR [pos] * invAlpha + inR [next] * alpha) : sampleL;

        //===========================================
        if (playDirection == Forward)
        {
            if (adsr.getState() == stk::ADSR::IDLE)
            {
                stopNote(0.0f, false);
                break;
            }
            
            // if we are about to enter loop, turn on loopEnv, turn off sampleEnv
            if (!inLoop && (samplePosition >= loopStart))
            {
                inLoop = true;
                
                loopPosition = samplePosition;
                
                loopEnv.keyOn();
                
                sampleEnv.keyOff();
            }
        }
        else if (playDirection == Reverse)
        {
            if(lengthTracker >= playLength + adsr.getReleaseTime() * getSampleRate())
            {
                clearCurrentNote(); break;
            }
            
            double reversePosition = playLength - lengthTracker + offset;
            
            // if we are about to leave loop, turn off loopEnv, turn on sampleEnv
            if (inLoop && (reversePosition <= (loopStart)))
            {
                inLoop = false;

                samplePosition = reversePosition;  //was original code
                
                loopEnv.keyOff(); //DT: should be keyOff/On or setTarget, set setValue
                //loopEnv.setValue(0.0f);
                
                sampleEnv.keyOn();
                //sampleEnv.setValue(1.0f);
            }
        }
        else
        {
            DBG("Invalid note direction.");
        }
        
        // Check for adsr keyOffs
        if ((playType != Normal) && ((adsr.getState() != stk::ADSR::RELEASE) && (adsr.getState() != stk::ADSR::IDLE)))
        {
            if (lengthTracker >= (playLength - adsr.getReleaseTime() * getSampleRate()))
            {
                adsr.keyOff();
            }
            
            if (lengthTracker >= (playLengthSF2 - sfzadsr.getReleaseTime() * getSampleRate()))
            {
                sfzadsr.keyOff();
            }
        }
        
        float l,r;
        
        l = lgain * adsr.tick()     * sfzadsr.tick()    * (loopL * loopEnv.tick()       + sampleL * sampleEnv.tick());
        r = rgain * adsr.lastOut()  * sfzadsr.lastOut() * (loopR * loopEnv.lastOut()    + sampleR * sampleEnv.lastOut());
        
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
    
    double bentRatio = pitchRatio * pitchbendMultiplier;
    
    while (--numSamples >= 0)
    {
        // always increment length
        lengthTracker += bentRatio;
        
        //==============SAMPLE STUFF=================
        float sampleL, sampleR;
        if (playDirection == Forward)   samplePosition += bentRatio;
        else                            samplePosition -= bentRatio;
        
        if(samplePosition < 0) samplePosition = 0;
        if(samplePosition > playingSound->soundLength - 2) samplePosition = playingSound->soundLength - 2;
        
        const int pos = (int) samplePosition;
        const float alpha = (float) (samplePosition - pos);
        const float invAlpha = 1.0f - alpha;
        int next = pos + 1;

        sampleL = (inL [pos] * invAlpha + inL [next] * alpha);
        sampleR = (inR != nullptr) ? (inR [pos] * invAlpha + inR [next] * alpha) : sampleL;

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
                if ((adsr.getState() != stk::ADSR::RELEASE) && (lengthTracker >= playLength))
                {
                    adsr.keyOff();
                }
            }
            
        }
        else if (playDirection == Reverse)
        {

            //DT: changed if checks here; lengthTracker already has adsrReleaseTime in it....
            //if(lengthTracker >= playLength)
            if(lengthTracker >= playLength + adsr.getReleaseTime() * getSampleRate())
            {
                clearCurrentNote(); break;
            }
            
            if (playType != Normal && (lengthTracker >= playLength))
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
            if (playingSound->loopMode <= 2)
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
        if(sourceSamplePosition > playingSound->soundLength - 2) sourceSamplePosition = playingSound->soundLength - 2;
        
        int pos = (int) sourceSamplePosition;
        const float alpha = (float) (sourceSamplePosition - pos);
        const float invAlpha = 1.0f - alpha;
        int next = pos + 1;

        float l = (inL [pos] * invAlpha + inL [next] * alpha);
        float r = (inR != nullptr) ? (inR [pos] * invAlpha + inR [next] * alpha) : l;
        
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

