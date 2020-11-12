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
                                          sfzero::Region::Ptr reg, bool isSF2)
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
        region_ = new sfzero::Region();
        *region_ = std::move(*reg);
        
        isSoundfont = true;
        
        loopStart = region_->loop_start; // loop start and end take in account minimum fade amt
        loopEnd = region_->loop_end;
        start = region_->offset;
        end = region_->end;
        
        delay = region_->ampeg.delay;
        attack = region_->ampeg.attack;
        hold = region_->ampeg.hold;
        decay = region_->ampeg.decay;
        sustain = region_->ampeg.sustain / 100.0f;
        release = region_->ampeg.release;
        
        loopMode = region_->loop_mode;
        
        trigger = region_->trigger;
        
        pedal = region_->pedal;
        
        sampleName = soundName;
        sampleLength = region_->sample->getSampleLength();
        
        pan = region_->pan;
    }
    else
    {
        region_ = nullptr;
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
    adsr.setSampleRate(currentSampleRate);
    sfzadsr.setSampleRate(currentSampleRate);
}

BKPianoSamplerVoice::~BKPianoSamplerVoice()
{
}

void BKPianoSamplerVoice::setCurrentPlaybackSampleRate(const double newRate)
{
    currentSampleRate = newRate;
    adsr.setSampleRate(currentSampleRate);
    sfzadsr.setSampleRate(currentSampleRate);
}

bool BKPianoSamplerVoice::canPlaySound (BKSynthesiserSound* sound)
{
    return dynamic_cast<const BKPianoSamplerSound*> (sound) != nullptr;
}


void BKPianoSamplerVoice::startNote (const int midiNoteNumber,
                                     const float offset,
                                     const float gain,
                                     const float velocity,
                                     PianoSamplerNoteDirection direction,
                                     PianoSamplerNoteType type,
                                     BKNoteType bktype,
                                     const uint64 startingPosition,
                                     const uint64 length,
                                     uint64 voiceRampOn,
                                     uint64 voiceRampOff,
                                     BKSynthesiserSound* s,
                                     float* dynamicGain,
									 float blendronicGain,
									 BlendronicProcessor::PtrArr blendronic)
{
    
    
                        startNote   (midiNoteNumber,
                                     offset,
                                     64,
                                     gain,
                                     velocity,
                                     direction,
                                     type,
                                     bktype,
                                     startingPosition,
                                     length,
                                     voiceRampOn,
                                     (int)(3*getSampleRate()),
                                     1.0,
                                     voiceRampOff,
                                     s,
                                     dynamicGain,
									 blendronicGain,
									 blendronic);
}

#define REVENV 0

#define PRINT 500

void BKPianoSamplerVoice::updatePitch(const BKPianoSamplerSound* const sound)
{
    pitchbendMultiplier = powf(2.0f, (pitchWheel/ 8192.0f - 1.0f)/6.0f); //whole-step range for pitchbend
    
    if (tuning != nullptr && tuning->getTuning()->aPrep->getSpringsActive())
    {
        Particle::PtrArr particles = tuning->getTuning()->aPrep->getParticles();
 
        /*
        double x = particles[currentMidiNoteNumber]->getX();
        int octave = particles[currentMidiNoteNumber]->getOctave();
        double midi = Utilities::clip(0, ftom(Utilities::centsToFreq(x - 1200.0 * octave),
                                              tuning->getGlobalTuningReference()), 128) - 60.0 + (octave * 12.0);
        
        midi += (tuning->getTuning()->aPrep->getAbsoluteOffsets().getUnchecked(currentMidiNoteNumber) +
                 tuning->getTuning()->aPrep->getFundamentalOffset());
         */
        
        
        //need to get tuning values for active particles, which are only those associated with depressed keys
        double x = particles[getCurrentlyPlayingKey()]->getX();
        int octave = particles[getCurrentlyPlayingKey()]->getOctave();
        //double transpOffset = (currentMidiNoteNumber - getCurrentlyPlayingKey()) * 100.;
        double transpOffset = (cookedNote - getCurrentlyPlayingKey()) * 100.;
        //DBG("BKPianoSamplerVoice::updatePitch cookedNote = " + String(cookedNote));
        double midi = Utilities::clip(0, ftom(Utilities::centsToFreq((x + transpOffset) - 1200.0 * octave),
                                              tuning->getGlobalTuningReference()), 128) - 60.0 + (octave * 12.0);
        //need to update centsToFreq to be movable to other As, non-440
        
        midi += (tuning->getTuning()->aPrep->getAbsoluteOffsets().getUnchecked(getCurrentlyPlayingKey()) +
                 tuning->getTuning()->aPrep->getFundamentalOffset());
        
        
        pitchRatio =    powf(2.0f, (midi - (float)sound->midiRootNote + sound->transpose) / 12.0f) *
                            sound->sourceSampleRate *
                            generalSettings->getTuningRatio() /
                            getSampleRate();
    }
    else
    {
        pitchRatio =    powf(2.0f, (cookedNote - (float)sound->midiRootNote + sound->transpose) / 12.0f) *
                            sound->sourceSampleRate *
                            generalSettings->getTuningRatio() /
                            getSampleRate();
    }
        
    bentRatio = pitchbendMultiplier * pitchRatio;
}

void BKPianoSamplerVoice::startNote (const int midi,
                                     const float pitchoffset,
                                     const int pitchWheelValue,
                                     const float gain,
                                     const float velocity,
                                     PianoSamplerNoteDirection direction,
                                     PianoSamplerNoteType type,
                                     BKNoteType bktype,
                                     const uint64 startingPosition,
                                     const uint64 length,  //total desired playlength, including ADSR times
                                     uint64 adsrAttack,
                                     uint64 adsrDecay,
                                     float adsrSustain,
                                     uint64 adsrRelease,
                                     BKSynthesiserSound* s,
                                     float* dynamicGain,
									 float blendGain,
									 BlendronicProcessor::PtrArr blendronic)
{
    if (BKPianoSamplerSound* const sound = dynamic_cast<BKPianoSamplerSound*> (s))
    {
        //DBG("BKPianoSamplerVoice::startNote " + String(midi));
        
        
        currentMidiNoteNumber = midi;
        cookedNote = ((float)midi + pitchoffset);
        pitchWheel = pitchWheelValue;
        
        updatePitch(sound);
        
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
                playEndPosition = (double)(sourceSamplePosition - (playLength + adsrRelease * pitchRatio));
                if (playEndPosition < adsrRelease * pitchRatio) playEndPosition = (double)adsrRelease * pitchRatio;
                /*
                if (totalLength * pitchRatio > sourceSamplePosition)
                {
                    playEndPosition = (double)(sourceSamplePosition - playLength);
                }
                else
                {
                    playEndPosition = (double)adsrRelease * pitchRatio;
                }
                 */
                // DBG("startnote reverse, playEndPosition, startingPosition " + String(playEndPosition/getSampleRate()) + " " + String (sourceSamplePosition/getSampleRate()));
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
        
        dgain = dynamicGain;
        
        noteVelocity = velocity;
        
        lengthTracker = 0.0;
        
        inLoop = false;
        
        adsr.setSampleRate(getSampleRate());
        
        adsr.setAllTimes(adsrAttack / getSampleRate(),
                         adsrDecay / getSampleRate(),
                         adsrSustain,
                         adsrRelease / getSampleRate());
        
        // DBG("startNote, direction = " + String((int)playDirection));
        adsr.keyOn();
        
        if (sound->isSoundfont)
        {
            samplePosition = sourceSamplePosition; //DT addition
            
            noteVelocity *= 0.5;
            
            //cfSamples = 10.0;  //DT: 10 samples too small...
            cfSamples = getSampleRate() / 50.; //20ms; possibly an issue if the loop length is really small, but 20ms is a typical ramp length
                                               //
            sampleEnv.setTime(20.0f);
            loopEnv.setTime(20.0f);
            
            sound->setAllTimes(adsrAttack / getSampleRate(),
                               (sound->hold > 0.0f) ? sound->hold : 0.001f,
                               adsrDecay / getSampleRate(),
                               adsrSustain,
                               adsrRelease / getSampleRate());
            
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
                    loopEnv.setValue(0.0);
                    sampleEnv.setValue(1.0);
                }
                else // loop
                {
                    loopEnv.setValue(1.0f);
                    sampleEnv.setValue(0.0f);
                    
                    loopPosition = sound->loopStart;
//#if REVENV
                    // total length of sound minus the length of the AHDR should be how long sample was sustained (hold in reverse)
//                    double totalLen = totalLength;// * pitchRatio;
//                    double envLen = ((sound->release + sound->decay + sound->attack) * getSampleRate());
//                    double envLenNoRelease = ((sound->release + sound->decay + sound->hold + sound->attack) * getSampleRate());

//                    if (totalLen > envLen)
                    {
                        double holdSamples = 0;//totalLen - envLen;
                        sfzadsr.setAttackTarget(sound->sustain);
                        sfzadsr.setAllTimes(sound->attack,
                                            (holdSamples / getSampleRate()), // hold length in seconds
                                            sound->decay,
                                            1.0f,
                                            sound->release);
                    }
//                    else
//                    {
//                        sfzadsr.setAttackTarget(sound->sustain);
//                        sfzadsr.setAllTimes(0.001f,
//                                            totalLen / getSampleRate(),
//                                            0.001f,
//                                            1.0f,
//                                            0.001f);
//                    }
                    
                    sfzadsr.keyOn();
                    
                    sfzEnvApplied = true;
                    
                }
                
                playLengthSF2 = (totalLength - (sound->attack * getSampleRate())) * pitchRatio;
            }
        }
        noteStartingPosition = sourceSamplePosition;
        noteEndPosition = playEndPosition;
		blendronicGain = blendGain;
		this->blendronic = blendronic;

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
        //DBG("note type: " + cNoteTypes[getNoteType()]);
        //adsr.setReleaseTime(0.003f);
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
    pitchWheel = newValue;
    pitchbendMultiplier = powf(2.0f, (newValue / 8192. - 1.)/6.);
    bentRatio = pitchRatio * pitchbendMultiplier;
    DBG("BKPianoSamplerVoice::pitchWheelMoved : " + String(pitchbendMultiplier));
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
    
    int64 loopStart, loopEnd, start, end, soundLengthMinus1;
    
    int numBlendronics = blendronic.size();
    Array<BKDelayL*> blendronicDelays;
    for (int i = 0; i < numBlendronics; ++i)
        blendronicDelays.add(blendronic.getUnchecked(i)->getDelay()->getDelay().get());
    
    double bentRatio = pitchRatio * pitchbendMultiplier;
    
    start = playingSound->start;
    end = playingSound->end;
    loopStart = playingSound->loopStart;
    loopEnd = playingSound->loopEnd + 1;
    // Trying to save as much time as possible...
    soundLengthMinus1 = playingSound->soundLength - 1;
    const float pan = (playingSound->pan * 0.01f) + 1.0f;

    int addCounter = 0;
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
            loopPosition -= loopEnd - loopStart;
        }
        
        if(loopPosition < 0) loopPosition = 0;
        if(loopPosition > soundLengthMinus1) loopPosition = soundLengthMinus1;
        
        const int posL = (int) loopPosition;
        const float alphaL = (float) (loopPosition - posL);
        const float invAlphaL = 1.0f - alphaL;
        const int nextL = posL + 1 - (int)(posL >= soundLengthMinus1 ? loopEnd - loopStart : 0);
        
        float loopL = (inL [posL] * invAlphaL + inL [nextL] * alphaL);
        float loopR = (inR != nullptr) ? (inR [posL] * invAlphaL + inR [nextL] * alphaL) : loopL;
        
        loopL = loopL * (2.0f - pan);
        loopR = loopR * pan;

        //===========================================
        
        //==============SAMPLE STUFF=================
        if (playDirection == Forward)   samplePosition += bentRatio;
        else                            samplePosition -= bentRatio;
        
        if(samplePosition < 0) samplePosition = 0;
        if(samplePosition > soundLengthMinus1) samplePosition = soundLengthMinus1;
        
        const int posS = (int) samplePosition;
        const float alphaS = (float) (samplePosition - posS);
        const float invAlphaS = 1.0f - alphaS;
        const int nextS = posS + 1 - (int)(posS >= soundLengthMinus1 ? loopEnd - loopStart : 0);

        float sampleL = (inL [posS] * invAlphaS + inL [nextS] * alphaS);
        float sampleR = (inR != nullptr) ? (inR [posS] * invAlphaS + inR [nextS] * alphaS) : sampleL;
        
        sampleL = sampleL * (2.0f - pan);
        sampleR = sampleR * pan;

        //===========================================
        if (playDirection == Forward)
        {
            if (adsr.getState() == BKADSR::IDLE)
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

                sampleEnv.keyOn();
            }
        }
        else
        {
            DBG("Invalid note direction.");
        }
        
        // Check for adsr keyOffs
        if ((playType != Normal) && ((adsr.getState() != BKADSR::RELEASE) && (adsr.getState() != BKADSR::IDLE)))
        {
            if (lengthTracker >= playLength)
            {
                adsr.keyOff();
            }
            
            if (lengthTracker >= playLengthSF2)
            {
                sfzadsr.keyOff();
            }
        }
        
        const float l = noteVelocity * adsr.tick()     * sfzadsr.tick()    * (loopL * loopEnv.tick()       + sampleL * sampleEnv.tick());
        const float r = noteVelocity * adsr.lastOut()  * sfzadsr.lastOut() * (loopR * loopEnv.lastOut()    + sampleR * sampleEnv.lastOut());
        
        float d = 1.0f;
        if (dgain != nullptr) d = *dgain;

        if (outR != nullptr)
        {
            *outL++ += l * lgain * d;
            *outR++ += r * rgain * d;
        }
        else
        {
            *outL++ += ((l * lgain) + (r * rgain)) * 0.5f * d;
        }
        
        for (int i = 0; i < numBlendronics; ++i)
        {
            blendronicDelays.getUnchecked(i)->addSample(l * aGlobalGain * blendronicGain * d, addCounter, 0);
            blendronicDelays.getUnchecked(i)->addSample(r * aGlobalGain * blendronicGain * d, addCounter, 1);
        }
        addCounter++;
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
   
    int numBlendronics = blendronic.size();
    Array<BKDelayL*> blendronicDelays;
    for (int i = 0; i < numBlendronics; ++i)
        blendronicDelays.add(blendronic.getUnchecked(i)->getDelay()->getDelay().get());
    
    double bentRatio = pitchRatio * pitchbendMultiplier;

    int addCounter = 0;
    while (--numSamples >= 0)
    {
        // always increment length
        lengthTracker += bentRatio;
        
        //for reverse notes longer than sample length, write 0's, then continue
        if ((adsr.getState() != BKADSR::IDLE) &&
            (playDirection == Reverse) &&
            (samplePosition > playingSound->soundLength - 1))
        {
            const float l = 0.0f;
            const float r = 0.0f;
            if (outR != nullptr)
            {
                *outL++ += 0.0f;
                *outR++ += 0.0f;
            }
            else
            {
                *outL++ += 0.0f;
            }
            samplePosition -= bentRatio;
            
            for (int i = 0; i < numBlendronics; ++i)
            {
                blendronicDelays[i]->addSample(l, addCounter, 0);
                blendronicDelays[i]->addSample(r, addCounter, 1);
            }
            addCounter++;
            continue;
        }
        
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
        
        float pan = (playingSound->pan * 0.01f) + 1.0f;
        
        sampleL = sampleL * (2.0f - pan);
        sampleR = sampleR * pan;

        //===========================================
        if (playDirection == Forward)
        {
            if (adsr.getState() == BKADSR::IDLE)
            {
                stopNote(0.0f, false);
                break;
            }
            
            if (playType != Normal)
            {
                if ((adsr.getState() != BKADSR::RELEASE) && (lengthTracker >= playLength))
                {
                    adsr.keyOff();
                }
            }
            
        }
        /*
         else if (playDirection == Reverse)
                {
                    sourceSamplePosition -= bentRatio;
                    
                    if (sourceSamplePosition <= playEndPosition)
                    {
                        if ((adsr.getState() != BKADSR::RELEASE) && (adsr.getState() != BKADSR::IDLE))
                        {
                            // DBG("reverse sample adsr.keyOff");
                            adsr.keyOff();
                        }
                    }
                    
                    if(sourceSamplePosition <= 0)
                    {
                        clearCurrentNote();
                    }
                }
        */
        else if (playDirection == Reverse)
        {
            /*
            if(lengthTracker >= playLength + adsr.getReleaseTime() * getSampleRate())
            {
                clearCurrentNote(); break;
            }
             */
            // samplePosition -= bentRatio;
            
            if (samplePosition <= playEndPosition)
            // if (lengthTracker >= playLength + adsr.getReleaseTime() * getSampleRate())
            //if (playType != Normal && (lengthTracker >= playLength))
            {
                if ((adsr.getState() != BKADSR::RELEASE) && (adsr.getState() != BKADSR::IDLE))
                {
                    adsr.keyOff();
                }
            }
            
            if (samplePosition <= 0 || (adsr.getState() == BKADSR::IDLE))
            {
                clearCurrentNote();
            }
            
        }
        else
        {
            DBG("Invalid note direction.");
        }
        
        const float l = noteVelocity * adsr.tick() * (sampleL * sampleEnv.tick());
        const float r = noteVelocity * adsr.lastOut() * (sampleR * sampleEnv.lastOut());

        if (outR != nullptr)
        {
            *outL++ += l * lgain;
            *outR++ += r * rgain;
        }
        else
        {
            *outL++ += ((l * lgain) + (r * rgain)) * 0.5f;
        }
        
        for (int i = 0; i < numBlendronics; ++i)
        {
            blendronicDelays.getUnchecked(i)->addSample(l * aGlobalGain * blendronicGain, addCounter, 0);
            blendronicDelays.getUnchecked(i)->addSample(r * aGlobalGain * blendronicGain, addCounter, 1);
        }
        addCounter++;
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
        
        updatePitch(playingSound);
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

    // Putting blendronics into a temp raw pointer array to avoid taking the extra time
    // to do ReferenceCountedObject stuff in the loop
    int numBlendronics = blendronic.size();
    Array<BKDelayL*> blendronicDelays;
    for (int i = 0; i < numBlendronics; ++i)
        blendronicDelays.add(blendronic.getUnchecked(i)->getDelay()->getDelay().get());
    
    double bentRatio = pitchRatio * pitchbendMultiplier;
    
    int addCounter = 0;
    while (--numSamples >= 0)
    {

        if ((adsr.getState() != BKADSR::IDLE) &&
            (playDirection == Reverse) &&
            (sourceSamplePosition > playingSound->soundLength - 1))
        {
            const float l = 0.0f;
            const float r = 0.0f;
            if (outR != nullptr)
            {
                *outL++ += 0.0f;
                *outR++ += 0.0f;
            }
            else
            {
                *outL++ += 0.0f;
            }
            sourceSamplePosition -= bentRatio;
            
            for (int i = 0; i < numBlendronics; ++i)
            {
                blendronicDelays[i]->addSample(l, addCounter, 0);
                blendronicDelays[i]->addSample(r, addCounter, 1);
            }
            addCounter++;
            continue;
        }
        
        if(sourceSamplePosition < 0) sourceSamplePosition = 0;
        if(sourceSamplePosition > playingSound->soundLength - 2) sourceSamplePosition = playingSound->soundLength - 2;
        
        int pos = (int) sourceSamplePosition;
        const float alpha = (float) (sourceSamplePosition - pos);
        const float invAlpha = 1.0f - alpha;
        int next = pos + 1;

        const float l = (inL [pos] * invAlpha + inL [next] * alpha) * noteVelocity * adsr.tick();
        const float r = ((inR != nullptr) ? (inR [pos] * invAlpha + inR [next] * alpha) : l) * noteVelocity * adsr.lastOut();
        
        if (adsr.getState() == BKADSR::IDLE)
        {
            stopNote (0.0f, false);
            break;
        }

		if (outR != nullptr)
		{
			*outL++ += (l * lgain);
			*outR++ += (r * rgain);
		}
		else
		{
            *outL++ += (l * lgain) + (r * rgain) * 0.5f;
		}
        if (playDirection == Forward)
        {
            sourceSamplePosition += bentRatio;
            
            if (sourceSamplePosition >= playEndPosition)
            {
                if (adsr.getState() != BKADSR::RELEASE)
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
                if ((adsr.getState() != BKADSR::RELEASE) && (adsr.getState() != BKADSR::IDLE))
                {
                    // DBG("reverse sample adsr.keyOff");
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
        
        for (int i = 0; i < numBlendronics; ++i)
        {
            blendronicDelays.getUnchecked(i)->addSample(l * aGlobalGain * blendronicGain, addCounter, 0);
            blendronicDelays.getUnchecked(i)->addSample(r * aGlobalGain * blendronicGain, addCounter, 1);
        }
        addCounter++;
    }
}

