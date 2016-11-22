/*
  ==============================================================================

    Nostalgic.cpp
    Created: 22 Nov 2016 3:46:45pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "Nostalgic.h"



NostalgicProcessor::NostalgicProcessor()
{
    
    noteLengthTimers.ensureStorageAllocated(128);
    velocities.ensureStorageAllocated(128);
    reverseLengthTimers.ensureStorageAllocated(128);
    reverseTargetLength.ensureStorageAllocated(128);
    undertowVelocities.ensureStorageAllocated(128);
    
    keymap.ensureStorageAllocated(128);
    
    for(int i=0;i<128;i++)
    {
        noteLengthTimers.insert(i, 0); //initialize timers for all notes
        velocities.insert(i, 0); //store noteOn velocities to set Nostalgic velocities
        reverseLengthTimers.insert(i, 0); //initialize timers for handling wavedistance/undertow
        reverseTargetLength.insert(i, 0);
        undertowVelocities.insert(i, 0);
        
        keymap.insert(i, FALSE); //keymap is all off to start
    }
    
    waveDistance = 200; //for testing
    undertow = 1000; //again, for testing
    /*
     one thing i discovered is that the original bK actually plays the forward undertow
     sample for TWICE this value; the first half at steady gain, and then the second
     half with a ramp down. i'm not sure why, and i'm not sure i want to keep that
     behavior, but if we don't, then the instrument will sound different when we import
     old presets
     --dt
     */
    
    
}

NostalgicProcessor::~NostalgicProcessor() {}


//get pointer to synth
void NostalgicProcessor::attachToSynth(BKSynthesiser *s)
{
    synth = s;
    sampleRate = s->getSampleRate(); //we'll have to take care to make sure this gets updated as needed
}


//begin reverse note; called when key is released
void NostalgicProcessor::playNote(int midiNoteNumber, int midiChannel) {
    
    //get length of played notes, subtract wave distance to set nostalgic reverse note length
    float tempDuration = getNoteLengthTimer(midiNoteNumber) * (1000. / sampleRate);
    //DBG("nostalgic note duration (ms) = " + std::to_string(tempDuration));
    
    //play nostalgic note
    synth->keyOn(
                 midiChannel,
                 midiNoteNumber,
                 getVelocity(midiNoteNumber),
                 tuningOffsets,
                 tuningBasePitch,
                 Reverse,
                 FixedLengthFixedStart,
                 Nostalgic,
                 //(tempDuration + getWaveDistance()) * synth->getPlaybackRate(midiNoteNumber), // start
                 tempDuration + getWaveDistance(),
                 tempDuration, // length
                 30, //ramp up (ms)
                 aRampUndertowCrossMS); //ramp off
    
    noteLengthTimerOff(midiNoteNumber);
    
    //time how long the reverse note has played, to trigger undertow note
    reverseNoteLengthTimerOn(midiNoteNumber, tempDuration);
    undertowVelocities.set(midiNoteNumber, getVelocity(midiNoteNumber));
    
}


//start timer for length of a particular note; called when key is pressed
void NostalgicProcessor::noteLengthTimerOn(int midiNoteNumber, float midiNoteVelocity)
{
    activeNotes.addIfNotAlreadyThere(midiNoteNumber);
    noteLengthTimers.set(midiNoteNumber, 0);
    velocities.set(midiNoteNumber, midiNoteVelocity);
    //DBG("nostalgic added active note " + std::to_string(midiNoteNumber) + " " + std::to_string(midiNoteVelocity));
}


//clear note and reset timer; called when key is released
void NostalgicProcessor::noteLengthTimerOff(int midiNoteNumber)
{
    activeNotes.removeFirstMatchingValue(midiNoteNumber);
    noteLengthTimers.set(midiNoteNumber, 0);
    //DBG("nostalgic removed active note " + std::to_string(midiNoteNumber));
}


//start timer for length of a particular note; called when key is pressed
void NostalgicProcessor::reverseNoteLengthTimerOn(int midiNoteNumber, float noteLength)
{
    activeReverseNotes.addIfNotAlreadyThere(midiNoteNumber);
    reverseLengthTimers.set(midiNoteNumber, 0);
    reverseTargetLength.set(midiNoteNumber, (noteLength - aRampUndertowCrossMS) * sampleRate/1000.); //to schedule undertow note
    
    //store target note length
    //DBG("nostalgic added active reverse note " + std::to_string(midiNoteNumber));
}


//turn on/off particular key in keymap
void NostalgicProcessor::keymapSet(int midiNoteNumber, bool on)
{
    keymap.set(midiNoteNumber, on);
}


//main scheduling function
void NostalgicProcessor::processBlock(int numSamples, int midiChannel)
{
    
    incrementTimers(numSamples);
    
    //check timers to see if any are at an undertow turnaround point, then call keyOn(forward) and keyOff, with 50ms ramps
    for(int i = (activeReverseNotes.size() - 1); i >= 0; --i)
    {
        int tempnote = activeReverseNotes.getUnchecked(i);
        
        if (getReverseNoteLengthTimer(tempnote) > reverseTargetLength.getUnchecked(tempnote))
        {
            //play forward note for undertow dur and ramp
            //DBG("nostalgic forward note " + std::to_string(tempnote) + " " + std::to_string(getVelocity(tempnote)));
            synth->keyOn(
                         midiChannel,
                         tempnote,
                         undertowVelocities.getUnchecked(tempnote),
                         tuningOffsets,
                         tuningBasePitch,
                         Forward,
                         FixedLengthFixedStart,
                         Nostalgic,
                         getWaveDistance(), // start
                         getUndertow(), // length
                         aRampUndertowCrossMS, //ramp up
                         getUndertow() - aRampUndertowCrossMS); //ramp down
            
            //remove from active notes list
            activeReverseNotes.removeFirstMatchingValue(tempnote);
        }
    }
}


//get length of note
int NostalgicProcessor::getNoteLengthTimer(int midiNoteNumber) const noexcept
{
    return noteLengthTimers.getUnchecked(midiNoteNumber);
}

float NostalgicProcessor::getVelocity(int midiNoteNumber) const noexcept
{
    return velocities.getUnchecked(midiNoteNumber);
}


//get length of note
int NostalgicProcessor::getReverseNoteLengthTimer(int midiNoteNumber) const noexcept
{
    return reverseLengthTimers.getUnchecked(midiNoteNumber);
}


//increment timers for all active notes, and all currently reversing notes
void NostalgicProcessor::incrementTimers(int numSamples)
{
    for(int i = (activeNotes.size() - 1); i >= 0; --i)
    {
        noteLengthTimers.set(activeNotes.getUnchecked(i),
                             noteLengthTimers.getUnchecked(activeNotes.getUnchecked(i)) + numSamples);
    }
    
    for(int i = (activeReverseNotes.size() - 1); i >= 0; --i)
    {
        reverseLengthTimers.set(activeReverseNotes.getUnchecked(i),
                                reverseLengthTimers.getUnchecked(activeReverseNotes.getUnchecked(i)) + numSamples);
    }
}
