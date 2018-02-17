/*
 ==============================================================================
 
 BKSynthesiser.cpp
 Created: 19 Oct 2016 9:59:49am
 Author:  Michael R Mulshine
 
 ==============================================================================
 */

#include "BKSynthesiser.h"

BKSynthesiserSound::BKSynthesiserSound() {}
BKSynthesiserSound::~BKSynthesiserSound() {}

//==============================================================================
BKSynthesiserVoice::BKSynthesiserVoice()
: currentSampleRate (44100.0),
currentlyPlayingNote (-1),
currentPlayingMidiChannel (0),
noteOnTime (0),
keyIsDown (false),
sustainPedalDown (false),
sostenutoPedalDown (false)
{
}


BKSynthesiserVoice::~BKSynthesiserVoice()
{
}

bool BKSynthesiserVoice::isPlayingChannel (const int midiChannel) const
{
    return currentPlayingMidiChannel == midiChannel;
}

void BKSynthesiserVoice::setCurrentPlaybackSampleRate (const double newRate)
{
    currentSampleRate = newRate;
}

bool BKSynthesiserVoice::isVoiceActive() const
{
    return getCurrentlyPlayingNote() >= 0;
}

void BKSynthesiserVoice::clearCurrentNote()
{
    currentlyPlayingNote = -1;
    currentlyPlayingSound = nullptr;
    currentPlayingMidiChannel = 0;
}

void BKSynthesiserVoice::aftertouchChanged (int) {}
void BKSynthesiserVoice::channelPressureChanged (int) {}

bool BKSynthesiserVoice::wasStartedBefore (const BKSynthesiserVoice& other) const noexcept
{
    return noteOnTime < other.noteOnTime;
    }
    
    void BKSynthesiserVoice::renderNextBlock (AudioBuffer<double>& outputBuffer,
                                              int startSample, int numSamples)
    {
        AudioBuffer<double> subBuffer (outputBuffer.getArrayOfWritePointers(),
                                       outputBuffer.getNumChannels(),
                                       startSample, numSamples);
        
        tempBuffer.makeCopyOf (subBuffer);
        renderNextBlock (tempBuffer, 0, numSamples);
        subBuffer.makeCopyOf (tempBuffer);
    }
    
    //==============================================================================
    BKSynthesiser::BKSynthesiser(GeneralSettings::Ptr gen):
    generalSettings(gen),
    sampleRate (0),
    lastNoteOnCounter (0),
    minimumSubBlockSize (32),
    subBlockSubdivisionIsStrict (false),
    shouldStealNotes (true)
    {
        for (int i = 0; i < numElementsInArray (lastPitchWheelValues); ++i)
            lastPitchWheelValues[i] = 0x2000;
        
    }
    
    BKSynthesiser::BKSynthesiser(void):
    sampleRate (0),
    lastNoteOnCounter (0),
    minimumSubBlockSize (32),
    subBlockSubdivisionIsStrict (false),
    shouldStealNotes (true)
    {
        for (int i = 0; i < numElementsInArray (lastPitchWheelValues); ++i)
            lastPitchWheelValues[i] = 0x2000;
        
    }
    
    void BKSynthesiser::setGeneralSettings(GeneralSettings::Ptr gen)
    {
        generalSettings = gen;
    }
    
    void BKSynthesiser::updateGeneralSettings(GeneralSettings::Ptr gen)
    {
        for (int i = voices.size(); --i >= 0;)
        {
            voices.getUnchecked(i)->setGeneralSettings(gen);
        }
    }
    
    BKSynthesiser::~BKSynthesiser()
    {
    }
    
    //==============================================================================
    BKSynthesiserVoice* BKSynthesiser::getVoice (const int index) const
    {
        const ScopedLock sl (lock);
        return voices [index];
    }
    
    void BKSynthesiser::clearVoices()
    {
        const ScopedLock sl (lock);
        voices.clear();
    }
    
    BKSynthesiserVoice* BKSynthesiser::addVoice (BKSynthesiserVoice* const newVoice)
    {
        const ScopedLock sl (lock);
        newVoice->setCurrentPlaybackSampleRate (sampleRate);
        return voices.add (newVoice);
    }
    
    void BKSynthesiser::removeVoice (const int index)
    {
        const ScopedLock sl (lock);
        voices.remove (index);
    }
    
    void BKSynthesiser::clearSounds()
    {
        const ScopedLock sl (lock);
        sounds.clear();
    }
    
    BKSynthesiserSound* BKSynthesiser::addSound (const BKSynthesiserSound::Ptr& newSound)
    {
        const ScopedLock sl (lock);
        return sounds.add (newSound);
    }
    
    void BKSynthesiser::removeSound (const int index)
    {
        const ScopedLock sl (lock);
        sounds.remove (index);
    }
    
    void BKSynthesiser::setNoteStealingEnabled (const bool shouldSteal)
    {
        shouldStealNotes = shouldSteal;
    }
    
    void BKSynthesiser::setMinimumRenderingSubdivisionSize (int numSamples, bool shouldBeStrict) noexcept
    {
        jassert (numSamples > 0); // it wouldn't make much sense for this to be less than 1
        minimumSubBlockSize = numSamples;
        subBlockSubdivisionIsStrict = shouldBeStrict;
    }
    
    //==============================================================================
    void BKSynthesiser::setCurrentPlaybackSampleRate (const double newRate)
    {
        if (sampleRate != newRate)
        {
            const ScopedLock sl (lock);
            
            allNotesOff (0, false);
            
            sampleRate = newRate;
            
            for (int i = voices.size(); --i >= 0;)
                voices.getUnchecked (i)->setCurrentPlaybackSampleRate (newRate);
        }
    }
    
    template <typename floatType>
    void BKSynthesiser::processNextBlock (AudioBuffer<floatType>& outputAudio,
                                          const MidiBuffer& midiData,
                                          int startSample,
                                          int numSamples)
    {
        // must set the sample rate before using this!
        jassert (sampleRate != 0);
        
        MidiBuffer::Iterator midiIterator (midiData);
        midiIterator.setNextSamplePosition (startSample);
        
        bool firstEvent = true;
        int midiEventPos;
        MidiMessage m;
        
        const ScopedLock sl (lock);
        
        while (numSamples > 0)
        {
            if (! midiIterator.getNextEvent (m, midiEventPos))
            {
                renderVoices (outputAudio, startSample, numSamples);
                return;
            }
            
            const int samplesToNextMidiMessage = midiEventPos - startSample;
            
            if (samplesToNextMidiMessage >= numSamples)
            {
                renderVoices (outputAudio, startSample, numSamples);
                handleMidiEvent (m);
                break;
            }
            
            if (samplesToNextMidiMessage < ((firstEvent && ! subBlockSubdivisionIsStrict) ? 1 : minimumSubBlockSize))
            {
                handleMidiEvent (m);
                continue;
            }
            
            firstEvent = false;
            
            renderVoices (outputAudio, startSample, samplesToNextMidiMessage);
            handleMidiEvent (m);
            startSample += samplesToNextMidiMessage;
            numSamples  -= samplesToNextMidiMessage;
        }
        
        while (midiIterator.getNextEvent (m, midiEventPos))
            handleMidiEvent (m);
    }
    
    // explicit template instantiation
    template void BKSynthesiser::processNextBlock<float> (AudioBuffer<float>& outputAudio,
                                                          const MidiBuffer& midiData,
                                                          int startSample,
                                                          int numSamples);
    template void BKSynthesiser::processNextBlock<double> (AudioBuffer<double>& outputAudio,
                                                           const MidiBuffer& midiData,
                                                           int startSample,
                                                           int numSamples);
    
    void BKSynthesiser::renderVoices (AudioBuffer<float>& buffer, int startSample, int numSamples)
    {
        for (int i = voices.size(); --i >= 0;)
            voices.getUnchecked (i)->renderNextBlock (buffer, startSample, numSamples);
    }
    
    void BKSynthesiser::renderVoices (AudioBuffer<double>& buffer, int startSample, int numSamples)
    {
        for (int i = voices.size(); --i >= 0;)
            voices.getUnchecked (i)->renderNextBlock (buffer, startSample, numSamples);
    }
    
    void BKSynthesiser::handleMidiEvent (const MidiMessage& m)
    {
        const int channel = m.getChannel();
        
        if (m.isNoteOn())
        {
            
        }
        else if (m.isNoteOff())
        {
            
        }
        else if (m.isAllNotesOff() || m.isAllSoundOff())
        {
            allNotesOff (channel, true);
        }
        else if (m.isPitchWheel())
        {
            const int wheelPos = m.getPitchWheelValue();
            lastPitchWheelValues [channel - 1] = wheelPos;
            handlePitchWheel (channel, wheelPos);
        }
        else if (m.isAftertouch())
        {
            handleAftertouch (channel, m.getNoteNumber(), m.getAfterTouchValue());
        }
        else if (m.isChannelPressure())
        {
            handleChannelPressure (channel, m.getChannelPressureValue());
        }
        else if (m.isController())
        {
            handleController (channel, m.getControllerNumber(), m.getControllerValue());
        }
        else if (m.isProgramChange())
        {
            handleProgramChange (channel, m.getProgramChangeNumber());
        }
    }
    
    //==============================================================================
    void BKSynthesiser::keyOn (const int midiChannel,
                               const int keyNoteNumber,
                               const int midiNoteNumber,
                               const float transp,
                               const float velocity,
                               const float gain,
                               PianoSamplerNoteDirection direction,
                               PianoSamplerNoteType type,
                               BKNoteType bktype,
                               int layer,
                               const float startingPositionMS,
                               const float lengthMS,
                               const float rampOnMS, //included in lengthMS
                               const float rampOffMS //included in lengthMS
                               )
    {
        const ScopedLock sl (lock);
        
        int noteNumber = midiNoteNumber;
        
        // ADDED THIS
        if (noteNumber > 108 || noteNumber < 21) return;
        
        float transposition = transp;
        
        for (int i = sounds.size(); --i >= 0;)
        {
            BKSynthesiserSound* const sound = sounds.getUnchecked(i);
            
            // Check if sound applies to note, velocity, and channel.
            if (sound->appliesToNote (noteNumber)
                && sound->appliesToVelocity((int)(velocity * 127.0))
                && sound->appliesToChannel (midiChannel))
            {
                //DBG("BKSynthesiser::keyOn " + String(noteNumber));
                startVoice (findFreeVoice (sound, midiChannel, noteNumber, shouldStealNotes),
                            sound,
                            midiChannel,
                            keyNoteNumber,
                            noteNumber,
                            transposition,
                            velocity * gain,
                            direction,
                            type,
                            bktype,
                            layer,
                            (uint64)((startingPositionMS * 0.001f) * getSampleRate()),
                            (uint64)(lengthMS*0.001f* getSampleRate()),
                            rampOnMS*0.001f* getSampleRate(),
                            rampOffMS*0.001f* getSampleRate());
                
            }
        }
    }
    
    void BKSynthesiser::startVoice (BKSynthesiserVoice* const voice,
                                    BKSynthesiserSound* const sound,
                                    const int midiChannel,
                                    const int keyNoteNumber,
                                    const int midiNoteNumber,
                                    const float midiNoteNumberOffset,
                                    const float volume,
                                    PianoSamplerNoteDirection direction,
                                    PianoSamplerNoteType type,
                                    BKNoteType bktype,
                                    int layer,
                                    const uint64 startingPosition,
                                    const uint64 length,
                                    int voiceRampOn,
                                    int voiceRampOff
                                    )
    {
        if (voice != nullptr && sound != nullptr)
        {
            if (voice->currentlyPlayingSound != nullptr)
                voice->stopNote (0.0f, false);
            
#if CRAY_COOL_MUSIC_MAKER
            voice->currentlyPlayingNote = (float)midiNoteNumber + midiNoteNumberOffset;
#else
            voice->currentlyPlayingNote = midiNoteNumber; //midiNoteNumber + (int)midiNoteNumberOffset)
#endif      
            voice->layerId = layerToLayerId(bktype, layer);
            voice->length = (int)length;
            voice->type = type;
            voice->bktype = bktype;
            voice->currentPlayingMidiChannel = midiChannel;
            voice->noteOnTime = ++lastNoteOnCounter;
            voice->currentlyPlayingSound = sound;
            voice->keyIsDown = true;
            voice->sostenutoPedalDown = false;
            voice->sustainPedalDown = sustainPedalsDown[midiChannel];
            
            voice->currentlyPlayingKey = keyNoteNumber; //keep track of which physical key is associated with this voice
            
            float gain = volume;
            
            gain *= generalSettings->getGlobalGain();

            voice->startNote (
                              (float)midiNoteNumber+midiNoteNumberOffset,
                              gain,
                              direction,
                              type,
                              bktype,
                              startingPosition,
                              length,
                              voiceRampOn,
                              voiceRampOff,
                              sound);
        }
    }
    
    void BKSynthesiser::stopVoice (BKSynthesiserVoice* voice, float velocity, const bool allowTailOff)
    {
        jassert (voice != nullptr);
        
        voice->stopNote (velocity, allowTailOff);
        
        // the subclass MUST call clearCurrentNote() if it's not tailing off! RTFM for stopNote()!
        jassert (allowTailOff || (voice->getCurrentlyPlayingNote() < 0 && voice->getCurrentlyPlayingSound() == 0));
    }
    
    void BKSynthesiser::keyOff (const int midiChannel,
                                const BKNoteType type,
                                const int layerId,
                                const int keyNoteNumber,
                                const int midiNoteNumber,
                                const float velocity,
                                bool allowTailOff)
    {
        const ScopedLock sl (lock);
        
        for (int i = voices.size(); --i >= 0;)
        {
            BKSynthesiserVoice* const voice = voices.getUnchecked (i);
            
            if (voice->getCurrentlyPlayingNote() == midiNoteNumber
                && voice->getCurrentlyPlayingKey() == keyNoteNumber
                && voice->isPlayingChannel (midiChannel)
                && (voice->layerId == layerToLayerId(type, layerId))) //need to add transposition level as well here
            {
                
                if (BKSynthesiserSound* const sound = voice->getCurrentlyPlayingSound())
                {
                    if (sound->appliesToNote (midiNoteNumber) 
                        && sound->appliesToChannel (midiChannel))
                    {
                        jassert (! voice->keyIsDown || voice->sustainPedalDown == sustainPedalsDown [midiChannel]);
                        
                        // Let synthesiser know that key is no longer down,
                        voice->keyIsDown = false;
                        
                        
                        if (! ((voice->type == FixedLengthFixedStart) || (voice->type == FixedLength) || voice->sostenutoPedalDown)) {
                            //DBG("BKSynthesiser::stopVoice " + String(midiNoteNumber));
                            stopVoice (voice, velocity, allowTailOff);
                        }
                    }
                }
            }
        }
    }
    
    
    void BKSynthesiser::allNotesOff (const int midiChannel, const bool allowTailOff)
    {
        const ScopedLock sl (lock);
        
        for (int i = voices.size(); --i >= 0;)
        {
            BKSynthesiserVoice* const voice = voices.getUnchecked (i);
            
            if (midiChannel <= 0 || voice->isPlayingChannel (midiChannel))
                voice->stopNote (1.0f, allowTailOff);
        }
        
        sustainPedalsDown.clear();
    }
    
    void BKSynthesiser::handlePitchWheel (const int midiChannel, const int wheelValue)
    {
        const ScopedLock sl (lock);
        
        for (int i = voices.size(); --i >= 0;)
        {
            BKSynthesiserVoice* const voice = voices.getUnchecked (i);
            
            if (midiChannel <= 0 || voice->isPlayingChannel (midiChannel))
                voice->pitchWheelMoved (wheelValue);
        }
    }
    
    void BKSynthesiser::handleController (const int midiChannel,
                                          const int controllerNumber,
                                          const int controllerValue)
    {
        switch (controllerNumber)
        {
            case 0x40:  handleSustainPedal   (midiChannel, controllerValue >= 64); break;
            case 0x42:  handleSostenutoPedal (midiChannel, controllerValue >= 64); break;
            case 0x43:  handleSoftPedal      (midiChannel, controllerValue >= 64); break;
            default:    break;
        }
        
        const ScopedLock sl (lock);
        
        for (int i = voices.size(); --i >= 0;)
        {
            BKSynthesiserVoice* const voice = voices.getUnchecked (i);
            
            if (midiChannel <= 0 || voice->isPlayingChannel (midiChannel))
                voice->controllerMoved (controllerNumber, controllerValue);
        }
    }
    
    void BKSynthesiser::handleAftertouch (int midiChannel, int midiNoteNumber, int aftertouchValue)
    {
        const ScopedLock sl (lock);
        
        for (int i = voices.size(); --i >= 0;)
        {
            BKSynthesiserVoice* const voice = voices.getUnchecked (i);
            
            if (voice->getCurrentlyPlayingNote() == midiNoteNumber
                && (midiChannel <= 0 || voice->isPlayingChannel (midiChannel)))
                voice->aftertouchChanged (aftertouchValue);
        }
    }
    
    void BKSynthesiser::handleChannelPressure (int midiChannel, int channelPressureValue)
    {
        const ScopedLock sl (lock);
        
        for (int i = voices.size(); --i >= 0;)
        {
            BKSynthesiserVoice* const voice = voices.getUnchecked (i);
            
            if (midiChannel <= 0 || voice->isPlayingChannel (midiChannel))
                voice->channelPressureChanged (channelPressureValue);
        }
    }
    
    void BKSynthesiser::handleSustainPedal (int midiChannel, bool isDown)
    {
        jassert (midiChannel > 0 && midiChannel <= 16);
        const ScopedLock sl (lock);
        
        // Invert sustain should be dealt with around here? 
        if (isDown)
        {
            sustainPedalsDown.setBit (midiChannel);
            
            for (int i = voices.size(); --i >= 0;)
            {
                BKSynthesiserVoice* const voice = voices.getUnchecked (i);
                
                if (voice->isPlayingChannel (midiChannel) && voice->isKeyDown())
                    voice->sustainPedalDown = true;
            }
        }
        else
        {
            for (int i = voices.size(); --i >= 0;)
            {
                BKSynthesiserVoice* const voice = voices.getUnchecked (i);
                
                if (voice->isPlayingChannel (midiChannel))
                {
                    voice->sustainPedalDown = false;
                    
                    if (! voice->isKeyDown())
                        stopVoice (voice, 1.0f, true);
                }
            }
            
            sustainPedalsDown.clearBit (midiChannel);
        }
    }
    
    void BKSynthesiser::handleSostenutoPedal (int midiChannel, bool isDown)
    {
        jassert (midiChannel > 0 && midiChannel <= 16);
        const ScopedLock sl (lock);
        
        for (int i = voices.size(); --i >= 0;)
        {
            BKSynthesiserVoice* const voice = voices.getUnchecked (i);
            
            if (voice->isPlayingChannel (midiChannel))
            {
                if (isDown)
                    voice->sostenutoPedalDown = true;
                else if (voice->sostenutoPedalDown)
                    stopVoice (voice, 1.0f, true);
            }
        }
    }
    
    void BKSynthesiser::handleSoftPedal (int midiChannel, bool /*isDown*/)
    {
        ignoreUnused (midiChannel);
        jassert (midiChannel > 0 && midiChannel <= 16);
    }
    
    void BKSynthesiser::handleProgramChange (int midiChannel, int programNumber)
    {
        ignoreUnused (midiChannel, programNumber);
        jassert (midiChannel > 0 && midiChannel <= 16);
    }
    
    //==============================================================================
    BKSynthesiserVoice* BKSynthesiser::findFreeVoice (BKSynthesiserSound* soundToPlay,
                                                      int midiChannel, int midiNoteNumber,
                                                      const bool stealIfNoneAvailable) const
    {
        const ScopedLock sl (lock);
        
        for (int i = 0; i < voices.size(); ++i)
        {
            BKSynthesiserVoice* const voice = voices.getUnchecked (i);
            
            if ((! voice->isVoiceActive()) && voice->canPlaySound (soundToPlay))
                return voice;
        }
        
        if (stealIfNoneAvailable)
            return findVoiceToSteal (soundToPlay, midiChannel, midiNoteNumber);
        
        return nullptr;
    }
    
    struct VoiceAgeSorter
    {
        static int compareElements (BKSynthesiserVoice* v1, BKSynthesiserVoice* v2) noexcept
        {
            return v1->wasStartedBefore (*v2) ? -1 : (v2->wasStartedBefore (*v1) ? 1 : 0);
        }
    };
    
    BKSynthesiserVoice* BKSynthesiser::findVoiceToSteal (BKSynthesiserSound* soundToPlay,
                                                         int /*midiChannel*/, int midiNoteNumber) const
    {
        // This voice-stealing algorithm applies the following heuristics:
        // - Re-use the oldest notes first
        // - Protect the lowest & topmost notes, even if sustained, but not if they've been released.
        
        // apparently you are trying to render audio without having any voices...
        jassert (voices.size() > 0);
        
        // These are the voices we want to protect (ie: only steal if unavoidable)
        BKSynthesiserVoice* low = nullptr; // Lowest sounding note, might be sustained, but NOT in release phase
        BKSynthesiserVoice* top = nullptr; // Highest sounding note, might be sustained, but NOT in release phase
        
        // this is a list of voices we can steal, sorted by how long they've been running
        Array<BKSynthesiserVoice*> usableVoices;
        usableVoices.ensureStorageAllocated (voices.size());
        
        for (int i = 0; i < voices.size(); ++i)
        {
            BKSynthesiserVoice* const voice = voices.getUnchecked (i);
            
            if (voice->canPlaySound (soundToPlay))
            {
                jassert (voice->isVoiceActive()); // We wouldn't be here otherwise
                
                VoiceAgeSorter sorter;
                usableVoices.addSorted (sorter, voice);
                
                if (! voice->isPlayingButReleased()) // Don't protect released notes
                {
                    const int note = voice->getCurrentlyPlayingNote();
                    
                    if (low == nullptr || note < low->getCurrentlyPlayingNote())
                        low = voice;
                    
                    if (top == nullptr || note > top->getCurrentlyPlayingNote())
                        top = voice;
                }
            }
        }
        
        // Eliminate pathological cases (ie: only 1 note playing): we always give precedence to the lowest note(s)
        if (top == low)
            top = nullptr;
        
        const int numUsableVoices = usableVoices.size();
        
        // The oldest note that's playing with the target pitch is ideal..
        for (int i = 0; i < numUsableVoices; ++i)
        {
            BKSynthesiserVoice* const voice = usableVoices.getUnchecked (i);
            
            if (voice->getCurrentlyPlayingNote() == midiNoteNumber)
                return voice;
        }
        
        // Oldest voice that has been released (no finger on it and not held by sustain pedal)
        for (int i = 0; i < numUsableVoices; ++i)
        {
            BKSynthesiserVoice* const voice = usableVoices.getUnchecked (i);
            
            if (voice != low && voice != top && voice->isPlayingButReleased())
                return voice;
        }
        
        // Oldest voice that doesn't have a finger on it:
        for (int i = 0; i < numUsableVoices; ++i)
        {
            BKSynthesiserVoice* const voice = usableVoices.getUnchecked (i);
            
            if (voice != low && voice != top && ! voice->isKeyDown())
                return voice;
        }
        
        // Oldest voice that isn't protected
        for (int i = 0; i < numUsableVoices; ++i)
        {
            BKSynthesiserVoice* const voice = usableVoices.getUnchecked (i);
            
            if (voice != low && voice != top)
                return voice;
        }
        
        // We've only got "protected" voices now: lowest note takes priority
        jassert (low != nullptr);
        
        // Duophonic synth: give priority to the bass note:
        if (top != nullptr)
            return top;
        
        return low;
    }
    
