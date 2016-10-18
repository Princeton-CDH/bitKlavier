/*
  ==============================================================================

    BKFixedNoteSynthesiser.cpp
    Created: 18 Oct 2016 12:07:45pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "BKFixedNoteSynthesiser.h"


BKFixedNoteSynthesiserSound::BKFixedNoteSynthesiserSound() {}
BKFixedNoteSynthesiserSound::~BKFixedNoteSynthesiserSound() {}

//==============================================================================
BKFixedNoteSynthesiserVoice::BKFixedNoteSynthesiserVoice()
: currentSampleRate (44100.0),
currentlyPlayingNote (-1),
currentPlayingMidiChannel (0),
noteOnTime (0),
keyIsDown (false),
sustainPedalDown (false),
sostenutoPedalDown (false)
{
}

BKFixedNoteSynthesiserVoice::~BKFixedNoteSynthesiserVoice()
{
}

bool BKFixedNoteSynthesiserVoice::isPlayingChannel (const int midiChannel) const
{
    return currentPlayingMidiChannel == midiChannel;
}

void BKFixedNoteSynthesiserVoice::setCurrentPlaybackSampleRate (const double newRate)
{
    currentSampleRate = newRate;
}

bool BKFixedNoteSynthesiserVoice::isVoiceActive() const
{
    return getCurrentlyPlayingNote() >= 0;
}

void BKFixedNoteSynthesiserVoice::clearCurrentNote()
{
    currentlyPlayingNote = -1;
    currentlyPlayingSound = nullptr;
    currentPlayingMidiChannel = 0;
}

void BKFixedNoteSynthesiserVoice::aftertouchChanged (int) {}
void BKFixedNoteSynthesiserVoice::channelPressureChanged (int) {}

bool BKFixedNoteSynthesiserVoice::wasStartedBefore (const BKFixedNoteSynthesiserVoice& other) const noexcept
{
    return noteOnTime < other.noteOnTime;
    }
    
    void BKFixedNoteSynthesiserVoice::renderNextBlock (AudioBuffer<double>& outputBuffer,
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
    BKFixedNoteSynthesiser::BKFixedNoteSynthesiser()
    : sampleRate (0),
    lastNoteOnCounter (0),
    minimumSubBlockSize (32),
    subBlockSubdivisionIsStrict (false),
    shouldStealNotes (true)
    {
        for (int i = 0; i < numElementsInArray (lastPitchWheelValues); ++i)
            lastPitchWheelValues[i] = 0x2000;
    }
    
    BKFixedNoteSynthesiser::~BKFixedNoteSynthesiser()
    {
    }
    
    //==============================================================================
    BKFixedNoteSynthesiserVoice* BKFixedNoteSynthesiser::getVoice (const int index) const
    {
        const ScopedLock sl (lock);
        return voices [index];
    }
    
    void BKFixedNoteSynthesiser::clearVoices()
    {
        const ScopedLock sl (lock);
        voices.clear();
    }
    
    BKFixedNoteSynthesiserVoice* BKFixedNoteSynthesiser::addVoice (BKFixedNoteSynthesiserVoice* const newVoice)
    {
        const ScopedLock sl (lock);
        newVoice->setCurrentPlaybackSampleRate (sampleRate);
        return voices.add (newVoice);
    }
    
    void BKFixedNoteSynthesiser::removeVoice (const int index)
    {
        const ScopedLock sl (lock);
        voices.remove (index);
    }
    
    void BKFixedNoteSynthesiser::clearSounds()
    {
        const ScopedLock sl (lock);
        sounds.clear();
    }
    
    BKFixedNoteSynthesiserSound* BKFixedNoteSynthesiser::addSound (const BKFixedNoteSynthesiserSound::Ptr& newSound)
    {
        const ScopedLock sl (lock);
        return sounds.add (newSound);
    }
    
    void BKFixedNoteSynthesiser::removeSound (const int index)
    {
        const ScopedLock sl (lock);
        sounds.remove (index);
    }
    
    void BKFixedNoteSynthesiser::setNoteStealingEnabled (const bool shouldSteal)
    {
        shouldStealNotes = shouldSteal;
    }
    
    void BKFixedNoteSynthesiser::setMinimumRenderingSubdivisionSize (int numSamples, bool shouldBeStrict) noexcept
    {
        jassert (numSamples > 0); // it wouldn't make much sense for this to be less than 1
        minimumSubBlockSize = numSamples;
        subBlockSubdivisionIsStrict = shouldBeStrict;
    }
    
    //==============================================================================
    void BKFixedNoteSynthesiser::setCurrentPlaybackSampleRate (const double newRate)
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
    void BKFixedNoteSynthesiser::processNextBlock (AudioBuffer<floatType>& outputAudio,
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
    template void BKFixedNoteSynthesiser::processNextBlock<float> (AudioBuffer<float>& outputAudio,
                                                          const MidiBuffer& midiData,
                                                          int startSample,
                                                          int numSamples);
    template void BKFixedNoteSynthesiser::processNextBlock<double> (AudioBuffer<double>& outputAudio,
                                                           const MidiBuffer& midiData,
                                                           int startSample,
                                                           int numSamples);
    
    void BKFixedNoteSynthesiser::renderVoices (AudioBuffer<float>& buffer, int startSample, int numSamples)
    {
        for (int i = voices.size(); --i >= 0;)
            voices.getUnchecked (i)->renderNextBlock (buffer, startSample, numSamples);
    }
    
    void BKFixedNoteSynthesiser::renderVoices (AudioBuffer<double>& buffer, int startSample, int numSamples)
    {
        for (int i = voices.size(); --i >= 0;)
            voices.getUnchecked (i)->renderNextBlock (buffer, startSample, numSamples);
    }
    
    void BKFixedNoteSynthesiser::handleMidiEvent (const MidiMessage& m)
    {
        const int channel = m.getChannel();
        
        if (m.isNoteOn())
        {
            float time = 3.0;
            keyOn(channel,m.getNoteNumber(), m.getFloatVelocity(), ForwardNormal, (time * getSampleRate()));
            keyOn(channel,m.getNoteNumber(), m.getFloatVelocity(), ForwardFixed, (time * getSampleRate()));
        }
        else if (m.isNoteOff())
        {
            keyOff(channel, m.getNoteNumber(), m.getFloatVelocity(), true);
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
    void BKFixedNoteSynthesiser::keyOn (const int midiChannel,
                                const int midiNoteNumber,
                                const float velocity,
                                PianoSamplerNoteType type,
                                const uint32 length)
    {
        const ScopedLock sl (lock);
        
        for (int i = sounds.size(); --i >= 0;)
        {
            BKFixedNoteSynthesiserSound* const sound = sounds.getUnchecked(i);
            
            // DBG(velocity);
            if (sound->appliesToNote (midiNoteNumber)
                && sound->appliesToVelocity((int)(velocity * 127.0))
                && sound->appliesToChannel (midiChannel))
            {
                // If hitting a note that's still ringing, stop it first (it could be
                // still playing because of the sustain or sostenuto pedal).
                if ((type == ForwardNormal) || (type == ReverseNormal)) {
                    for (int j = voices.size(); --j >= 0;)
                    {
                        BKFixedNoteSynthesiserVoice* const voice = voices.getUnchecked (j);
                        
                        if (voice->getCurrentlyPlayingNote() == midiNoteNumber
                            && voice->isPlayingChannel (midiChannel))
                            stopVoice (voice, 1.0f, true);
                    }
                }
                startVoice (findFreeVoice (sound, midiChannel, midiNoteNumber, shouldStealNotes),
                            sound, midiChannel, midiNoteNumber, velocity, type, length);
            }
        }
    }
    
    void BKFixedNoteSynthesiser::startVoice (BKFixedNoteSynthesiserVoice* const voice,
                                    BKFixedNoteSynthesiserSound* const sound,
                                    const int midiChannel,
                                    const int midiNoteNumber,
                                    const float velocity,
                                    PianoSamplerNoteType type,
                                    const uint32 length)
    {
        if (voice != nullptr && sound != nullptr)
        {
            if (voice->currentlyPlayingSound != nullptr)
                voice->stopNote (0.0f, false);
            
            voice->currentlyPlayingNote = midiNoteNumber;
            voice->length = length;
            voice->type = type;
            voice->currentPlayingMidiChannel = midiChannel;
            voice->noteOnTime = ++lastNoteOnCounter;
            voice->currentlyPlayingSound = sound;
            voice->keyIsDown = true;
            voice->sostenutoPedalDown = false;
            voice->sustainPedalDown = sustainPedalsDown[midiChannel];
            
            voice->startNote (midiNoteNumber, velocity, type, length, sound
                              /*, lastPitchWheelValues [midiChannel - 1]*/);
        }
    }
    
    void BKFixedNoteSynthesiser::stopVoice (BKFixedNoteSynthesiserVoice* voice, float velocity, const bool allowTailOff)
    {
        jassert (voice != nullptr);
        
        voice->stopNote (velocity, allowTailOff);
        
        // the subclass MUST call clearCurrentNote() if it's not tailing off! RTFM for stopNote()!
        jassert (allowTailOff || (voice->getCurrentlyPlayingNote() < 0 && voice->getCurrentlyPlayingSound() == 0));
    }
    
    void BKFixedNoteSynthesiser::keyOff (const int midiChannel,
                                 const int midiNoteNumber,
                                 const float velocity,
                                 bool allowTailOff)
    {
        const ScopedLock sl (lock);
        
        for (int i = voices.size(); --i >= 0;)
        {
            BKFixedNoteSynthesiserVoice* const voice = voices.getUnchecked (i);
            
            if (voice->getCurrentlyPlayingNote() == midiNoteNumber
                && voice->isPlayingChannel (midiChannel))
            {
                if (BKFixedNoteSynthesiserSound* const sound = voice->getCurrentlyPlayingSound())
                {
                    if (sound->appliesToNote (midiNoteNumber)
                        && sound->appliesToChannel (midiChannel))
                    {
                        jassert (! voice->keyIsDown || voice->sustainPedalDown == sustainPedalsDown [midiChannel]);
                        
                        // Let synthesiser know that key is no longer down,
                        voice->keyIsDown = false;
                        
                        
                        if (! ((voice->type == ForwardFixed) || (voice->type == ReverseFixed) || voice->sustainPedalDown || voice->sostenutoPedalDown)) {
                            stopVoice (voice, velocity, allowTailOff);
                        }
                    }
                }
            }
        }
    }
    
    void BKFixedNoteSynthesiser::allNotesOff (const int midiChannel, const bool allowTailOff)
    {
        const ScopedLock sl (lock);
        
        for (int i = voices.size(); --i >= 0;)
        {
            BKFixedNoteSynthesiserVoice* const voice = voices.getUnchecked (i);
            
            if (midiChannel <= 0 || voice->isPlayingChannel (midiChannel))
                voice->stopNote (1.0f, allowTailOff);
        }
        
        sustainPedalsDown.clear();
    }
    
    void BKFixedNoteSynthesiser::handlePitchWheel (const int midiChannel, const int wheelValue)
    {
        const ScopedLock sl (lock);
        
        for (int i = voices.size(); --i >= 0;)
        {
            BKFixedNoteSynthesiserVoice* const voice = voices.getUnchecked (i);
            
            if (midiChannel <= 0 || voice->isPlayingChannel (midiChannel))
                voice->pitchWheelMoved (wheelValue);
        }
    }
    
    void BKFixedNoteSynthesiser::handleController (const int midiChannel,
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
            BKFixedNoteSynthesiserVoice* const voice = voices.getUnchecked (i);
            
            if (midiChannel <= 0 || voice->isPlayingChannel (midiChannel))
                voice->controllerMoved (controllerNumber, controllerValue);
        }
    }
    
    void BKFixedNoteSynthesiser::handleAftertouch (int midiChannel, int midiNoteNumber, int aftertouchValue)
    {
        const ScopedLock sl (lock);
        
        for (int i = voices.size(); --i >= 0;)
        {
            BKFixedNoteSynthesiserVoice* const voice = voices.getUnchecked (i);
            
            if (voice->getCurrentlyPlayingNote() == midiNoteNumber
                && (midiChannel <= 0 || voice->isPlayingChannel (midiChannel)))
                voice->aftertouchChanged (aftertouchValue);
        }
    }
    
    void BKFixedNoteSynthesiser::handleChannelPressure (int midiChannel, int channelPressureValue)
    {
        const ScopedLock sl (lock);
        
        for (int i = voices.size(); --i >= 0;)
        {
            BKFixedNoteSynthesiserVoice* const voice = voices.getUnchecked (i);
            
            if (midiChannel <= 0 || voice->isPlayingChannel (midiChannel))
                voice->channelPressureChanged (channelPressureValue);
        }
    }
    
    void BKFixedNoteSynthesiser::handleSustainPedal (int midiChannel, bool isDown)
    {
        jassert (midiChannel > 0 && midiChannel <= 16);
        const ScopedLock sl (lock);
        
        if (isDown)
        {
            sustainPedalsDown.setBit (midiChannel);
            
            for (int i = voices.size(); --i >= 0;)
            {
                BKFixedNoteSynthesiserVoice* const voice = voices.getUnchecked (i);
                
                if (voice->isPlayingChannel (midiChannel) && voice->isKeyDown())
                    voice->sustainPedalDown = true;
            }
        }
        else
        {
            for (int i = voices.size(); --i >= 0;)
            {
                BKFixedNoteSynthesiserVoice* const voice = voices.getUnchecked (i);
                
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
    
    void BKFixedNoteSynthesiser::handleSostenutoPedal (int midiChannel, bool isDown)
    {
        jassert (midiChannel > 0 && midiChannel <= 16);
        const ScopedLock sl (lock);
        
        for (int i = voices.size(); --i >= 0;)
        {
            BKFixedNoteSynthesiserVoice* const voice = voices.getUnchecked (i);
            
            if (voice->isPlayingChannel (midiChannel))
            {
                if (isDown)
                    voice->sostenutoPedalDown = true;
                else if (voice->sostenutoPedalDown)
                    stopVoice (voice, 1.0f, true);
            }
        }
    }
    
    void BKFixedNoteSynthesiser::handleSoftPedal (int midiChannel, bool /*isDown*/)
    {
        ignoreUnused (midiChannel);
        jassert (midiChannel > 0 && midiChannel <= 16);
    }
    
    void BKFixedNoteSynthesiser::handleProgramChange (int midiChannel, int programNumber)
    {
        ignoreUnused (midiChannel, programNumber);
        jassert (midiChannel > 0 && midiChannel <= 16);
    }
    
    //==============================================================================
    BKFixedNoteSynthesiserVoice* BKFixedNoteSynthesiser::findFreeVoice (BKFixedNoteSynthesiserSound* soundToPlay,
                                                      int midiChannel, int midiNoteNumber,
                                                      const bool stealIfNoneAvailable) const
    {
        const ScopedLock sl (lock);
        
        for (int i = 0; i < voices.size(); ++i)
        {
            BKFixedNoteSynthesiserVoice* const voice = voices.getUnchecked (i);
            
            if ((! voice->isVoiceActive()) && voice->canPlaySound (soundToPlay))
                return voice;
        }
        
        if (stealIfNoneAvailable)
            return findVoiceToSteal (soundToPlay, midiChannel, midiNoteNumber);
        
        return nullptr;
    }
    
    struct VoiceAgeSorter
    {
        static int compareElements (BKFixedNoteSynthesiserVoice* v1, BKFixedNoteSynthesiserVoice* v2) noexcept
        {
            return v1->wasStartedBefore (*v2) ? -1 : (v2->wasStartedBefore (*v1) ? 1 : 0);
        }
    };
    
    BKFixedNoteSynthesiserVoice* BKFixedNoteSynthesiser::findVoiceToSteal (BKFixedNoteSynthesiserSound* soundToPlay,
                                                         int /*midiChannel*/, int midiNoteNumber) const
    {
        // This voice-stealing algorithm applies the following heuristics:
        // - Re-use the oldest notes first
        // - Protect the lowest & topmost notes, even if sustained, but not if they've been released.
        
        // apparently you are trying to render audio without having any voices...
        jassert (voices.size() > 0);
        
        // These are the voices we want to protect (ie: only steal if unavoidable)
        BKFixedNoteSynthesiserVoice* low = nullptr; // Lowest sounding note, might be sustained, but NOT in release phase
        BKFixedNoteSynthesiserVoice* top = nullptr; // Highest sounding note, might be sustained, but NOT in release phase
        
        // this is a list of voices we can steal, sorted by how long they've been running
        Array<BKFixedNoteSynthesiserVoice*> usableVoices;
        usableVoices.ensureStorageAllocated (voices.size());
        
        for (int i = 0; i < voices.size(); ++i)
        {
            BKFixedNoteSynthesiserVoice* const voice = voices.getUnchecked (i);
            
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
            BKFixedNoteSynthesiserVoice* const voice = usableVoices.getUnchecked (i);
            
            if (voice->getCurrentlyPlayingNote() == midiNoteNumber)
                return voice;
        }
        
        // Oldest voice that has been released (no finger on it and not held by sustain pedal)
        for (int i = 0; i < numUsableVoices; ++i)
        {
            BKFixedNoteSynthesiserVoice* const voice = usableVoices.getUnchecked (i);
            
            if (voice != low && voice != top && voice->isPlayingButReleased())
                return voice;
        }
        
        // Oldest voice that doesn't have a finger on it:
        for (int i = 0; i < numUsableVoices; ++i)
        {
            BKFixedNoteSynthesiserVoice* const voice = usableVoices.getUnchecked (i);
            
            if (voice != low && voice != top && ! voice->isKeyDown())
                return voice;
        }
        
        // Oldest voice that isn't protected
        for (int i = 0; i < numUsableVoices; ++i)
        {
            BKFixedNoteSynthesiserVoice* const voice = usableVoices.getUnchecked (i);
            
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
    
