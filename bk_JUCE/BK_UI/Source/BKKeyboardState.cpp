/*
  ==============================================================================

    BKKeyboardState.cpp
    Created: 27 Mar 2017 12:06:24pm
    Author:  Michael R Mulshine (modified from JUCE source)

  ==============================================================================
*/

#include "BKKeyboardState.h"


BKKeyboardState::BKKeyboardState()
{
    zerostruct (noteStates);
}

BKKeyboardState::~BKKeyboardState()
{
}

//==============================================================================
void BKKeyboardState::reset()
{
    const ScopedLock sl (lock);
    zerostruct (noteStates);
    eventsToAdd.clear();
}

bool BKKeyboardState::isNoteOn (const int midiChannel, const int n) const noexcept
{
    jassert (midiChannel >= 0 && midiChannel <= 16);
    
    return isPositiveAndBelow (n, (int) 128)
    && (noteStates[n] & (1 << (midiChannel - 1))) != 0;
}

bool BKKeyboardState::isNoteOnForChannels (const int midiChannelMask, const int n) const noexcept
{
    return isPositiveAndBelow (n, (int) 128)
    && (noteStates[n] & midiChannelMask) != 0;
}

void BKKeyboardState::noteOn (const int midiChannel, const int midiNoteNumber, const float velocity)
{
    jassert (midiChannel >= 0 && midiChannel <= 16);
    jassert (isPositiveAndBelow (midiNoteNumber, (int) 128));
    
    const ScopedLock sl (lock);
    
    if (isPositiveAndBelow (midiNoteNumber, (int) 128))
    {
        const int timeNow = (int) Time::getMillisecondCounter();
        eventsToAdd.addEvent (MidiMessage::noteOn (midiChannel, midiNoteNumber, velocity), timeNow);
        eventsToAdd.clear (0, timeNow - 500);
        
        noteOnInternal (midiChannel, midiNoteNumber, velocity);
    }
}

void BKKeyboardState::noteOnInternal  (const int midiChannel, const int midiNoteNumber, const float velocity)
{
    if (isPositiveAndBelow (midiNoteNumber, (int) 128))
    {
        noteStates [midiNoteNumber] |= (1 << (midiChannel - 1));
        
        for (int i = listeners.size(); --i >= 0;)
            listeners.getUnchecked(i)->handleNoteOn (this, midiChannel, midiNoteNumber, velocity);
    }
}

void BKKeyboardState::noteOff (const int midiChannel, const int midiNoteNumber, const float velocity)
{
    const ScopedLock sl (lock);
    
    if (isNoteOn (midiChannel, midiNoteNumber))
    {
        const int timeNow = (int) Time::getMillisecondCounter();
        eventsToAdd.addEvent (MidiMessage::noteOff (midiChannel, midiNoteNumber), timeNow);
        eventsToAdd.clear (0, timeNow - 500);
        
        noteOffInternal (midiChannel, midiNoteNumber, velocity);
    }
}

void BKKeyboardState::noteOffInternal  (const int midiChannel, const int midiNoteNumber, const float velocity)
{
    if (isNoteOn (midiChannel, midiNoteNumber))
    {
        noteStates [midiNoteNumber] &= ~(1 << (midiChannel - 1));
        
        for (int i = listeners.size(); --i >= 0;)
            listeners.getUnchecked(i)->handleNoteOff (this, midiChannel, midiNoteNumber, velocity);
    }
}

void BKKeyboardState::allNotesOff (const int midiChannel)
{
    const ScopedLock sl (lock);
    
    if (midiChannel <= 0)
    {
        for (int i = 1; i <= 16; ++i)
            allNotesOff (i);
    }
    else
    {
        for (int i = 0; i < 128; ++i)
            noteOff (midiChannel, i, 0.0f);
    }
}

void BKKeyboardState::processNextMidiEvent (const MidiMessage& message)
{
    if (message.isNoteOn())
    {
        noteOnInternal (message.getChannel(), message.getNoteNumber(), message.getFloatVelocity());
    }
    else if (message.isNoteOff())
    {
        noteOffInternal (message.getChannel(), message.getNoteNumber(), message.getFloatVelocity());
    }
    else if (message.isAllNotesOff())
    {
        for (int i = 0; i < 128; ++i)
            noteOffInternal (message.getChannel(), i, 0.0f);
    }
}

void BKKeyboardState::processNextMidiBuffer (MidiBuffer& buffer,
                                               const int startSample,
                                               const int numSamples,
                                               const bool injectIndirectEvents)
{
    MidiBuffer::Iterator i (buffer);
    MidiMessage message;
    int time;
    
    const ScopedLock sl (lock);
    
    while (i.getNextEvent (message, time))
        processNextMidiEvent (message);
    
    if (injectIndirectEvents)
    {
        MidiBuffer::Iterator i2 (eventsToAdd);
        const int firstEventToAdd = eventsToAdd.getFirstEventTime();
        const double scaleFactor = numSamples / (double) (eventsToAdd.getLastEventTime() + 1 - firstEventToAdd);
        
        while (i2.getNextEvent (message, time))
        {
            const int pos = jlimit (0, numSamples - 1, roundToInt ((time - firstEventToAdd) * scaleFactor));
            buffer.addEvent (message, startSample + pos);
        }
    }
    
    eventsToAdd.clear();
}

//==============================================================================
void BKKeyboardState::addListener (BKKeyboardStateListener* const listener)
{
    const ScopedLock sl (lock);
    listeners.addIfNotAlreadyThere (listener);
}

void BKKeyboardState::removeListener (BKKeyboardStateListener* const listener)
{
    const ScopedLock sl (lock);
    listeners.removeFirstMatchingValue (listener);
}

