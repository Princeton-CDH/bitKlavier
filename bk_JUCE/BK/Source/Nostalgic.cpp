/*
  ==============================================================================

    Nostalgic.cpp
    Created: 22 Nov 2016 3:46:45pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "Nostalgic.h"

NostalgicProcessor::NostalgicProcessor(BKSynthesiser *s, NostalgicPreparation::Ptr prep)
:
    synth(s),
    preparation(prep)
{
    sampleRate = synth->getSampleRate();
    
    noteLengthTimers.ensureStorageAllocated(128);
    velocities.ensureStorageAllocated(128);
    reverseLengthTimers.ensureStorageAllocated(128);
    reverseTargetLength.ensureStorageAllocated(128);
    undertowVelocities.ensureStorageAllocated(128);
    undertowPreparations.ensureStorageAllocated(128);
    
    for(int i=0;i<128;i++)
    {
        noteLengthTimers.insert(i, 0); //initialize timers for all notes
        velocities.insert(i, 0); //store noteOn velocities to set Nostalgic velocities
        reverseLengthTimers.insert(i, 0); //initialize timers for handling wavedistance/undertow
        reverseTargetLength.insert(i, 0);
        undertowVelocities.insert(i, 0);
        undertowPreparations.insert(i, preparation);
    }

}

NostalgicProcessor::~NostalgicProcessor() {}



//begin reverse note; called when key is released
void NostalgicProcessor::playNote(int midiNoteNumber, int midiChannel) {
    
    //get length of played notes, subtract wave distance to set nostalgic reverse note length
    float tempDuration = getNoteLengthTimer(midiNoteNumber) * (1000. / sampleRate);
    //DBG("nostalgic note duration (ms) = " + std::to_string(tempDuration));
    
    //play nostalgic note
    synth->keyOn(
                 midiChannel,
                 midiNoteNumber + preparation->getTransposition(), //need to store this, so that undertow retains this in the event of a preparation change
                 getVelocity(midiNoteNumber) * preparation->getGain(),
                 preparation->getTuningOffsets(),
                 preparation->getBasePitch(),
                 Reverse,
                 FixedLengthFixedStart,
                 Nostalgic,
                 tempDuration + preparation->getWavedistance(),
                 tempDuration, // length
                 30, //ramp up (ms)
                 aRampUndertowCrossMS); //ramp off
    
    noteLengthTimerOff(midiNoteNumber);
    
    //time how long the reverse note has played, to trigger undertow note
    reverseNoteLengthTimerOn(midiNoteNumber, tempDuration);
    
    //store values for when undertow note is played (in the event the preparation changes in the meantime)
    undertowVelocities.set(midiNoteNumber, getVelocity(midiNoteNumber) * preparation->getGain());
    undertowPreparations.set(midiNoteNumber, preparation); //it might be better to do this by copy, instead of by pointer, in the off chance that the preparation disappears because of a library switch or something...
    
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
}


//main scheduling function
void NostalgicProcessor::processBlock(int numSamples, int midiChannel)
{
    
    incrementTimers(numSamples);
    
    //check timers to see if any are at an undertow turnaround point, then call keyOn(forward) and keyOff, with 50ms ramps
    for(int i = (activeReverseNotes.size() - 1); i >= 0; --i)
    {
        int tempnote = activeReverseNotes.getUnchecked(i);
        
        //need to use preparation values from when note was played, stored in undertowPreparations
        NostalgicPreparation::Ptr undertowPrep = undertowPreparations.getUnchecked(tempnote);
        
        if (getReverseNoteLengthTimer(tempnote) > reverseTargetLength.getUnchecked(tempnote))
        {
 
            if(undertowPrep->getUndertow() > 0) {
                synth->keyOn(
                             midiChannel,
                             tempnote,
                             undertowVelocities.getUnchecked(tempnote),
                             undertowPrep->getTuningOffsets(),
                             undertowPrep->getBasePitch(),
                             Forward,
                             FixedLengthFixedStart,
                             Nostalgic,
                             undertowPrep->getWavedistance(),                        //start position
                             undertowPrep->getUndertow(),                            //play length
                             aRampUndertowCrossMS,                                   //ramp up length
                             undertowPrep->getUndertow() - aRampUndertowCrossMS);    //ramp down length
            }
            
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


//get length of reverse note
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
