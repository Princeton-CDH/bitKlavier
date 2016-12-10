/*
  ==============================================================================

    Nostalgic.cpp
    Created: 22 Nov 2016 3:46:45pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "Nostalgic.h"

NostalgicProcessor::NostalgicProcessor(BKSynthesiser *s,
                                       Keymap::Ptr km,
                                       NostalgicPreparation::Ptr prep,
                                       TuningPreparation::Ptr tPrep,
                                       SynchronicProcessor::CSArr& proc,
                                       int id):
Id(id),
synth(s),
keymap(km),
preparation(prep),
tPreparation(tPrep),
tuner(tPrep, id),
syncProcessor(proc)
{
    noteLengthTimers.ensureStorageAllocated(128);
    velocities.ensureStorageAllocated(128);
    reverseLengthTimers.ensureStorageAllocated(128);
    reverseTargetLength.ensureStorageAllocated(128);
    tuningsAtKeyOn.ensureStorageAllocated(128);
    velocitiesAtKeyOn.ensureStorageAllocated(128);
    preparationAtKeyOn.ensureStorageAllocated(128);
    
    for (int i = 0; i < 128; i++)
    {
        noteLengthTimers.insert(i, 0); //initialize timers for all notes
        velocities.insert(i, 0); //store noteOn velocities to set Nostalgic velocities
        reverseLengthTimers.insert(i, 0); //initialize timers for handling wavedistance/undertow
        reverseTargetLength.insert(i, 0);
        tuningsAtKeyOn.insert(i, 0);
        velocitiesAtKeyOn.insert(i, 0);
        preparationAtKeyOn.insert(i, preparation);
    }

}

NostalgicProcessor::~NostalgicProcessor()
{
}

void NostalgicProcessor::playNote(int channel, int note)
{
    
}

//begin reverse note; called when key is released
void NostalgicProcessor::keyReleased(int midiNoteNumber, int midiChannel)
{
    if (keymap->containsNote(midiNoteNumber))
    {
        float duration = 0.0;
        
        if (preparation->getMode() == NoteLengthSync)
        {
            //get length of played notes, subtract wave distance to set nostalgic reverse note length
            duration = noteLengthTimers.getUnchecked(midiNoteNumber) * preparation->getLengthMultiplier() * (1000.0 / sampleRate);
        }
        else //SynchronicSync
        {
            //get time in ms to target beat, summing over skipped beat lengths
            duration = syncProcessor[preparation->getSyncTarget()]->getTimeToBeatMS(preparation->getBeatsToSkip()); // sum
        }
        
        //play nostalgic note
        synth->keyOn(
                     midiChannel,
                     midiNoteNumber,
                     tuner.getOffset(midiNoteNumber) + preparation->getTransposition(),
                     velocities.getUnchecked(midiNoteNumber) * preparation->getGain() * aGlobalGain,
                     Reverse,
                     FixedLengthFixedStart,
                     Nostalgic,
                     duration + preparation->getWavedistance(),
                     duration,                                      // length
                     3,
                     aRampUndertowCrossMS );                        //ramp off
        
        
        // turn note length timers off
        activeNotes.removeFirstMatchingValue(midiNoteNumber);
        noteLengthTimers.set(midiNoteNumber, 0);
        //DBG("nostalgic removed active note " + std::to_string(midiNoteNumber));
        
        
        //time how long the reverse note has played, to trigger undertow note
        activeReverseNotes.addIfNotAlreadyThere(midiNoteNumber);
        reverseLengthTimers.set(midiNoteNumber, 0);
        reverseTargetLength.set(midiNoteNumber, (duration - aRampUndertowCrossMS) * sampleRate/1000.); //to schedule undertow note
        
        //store values for when undertow note is played (in the event the preparation changes in the meantime)
        tuningsAtKeyOn.set(midiNoteNumber, tuner.getOffset(midiNoteNumber));
        velocitiesAtKeyOn.set(midiNoteNumber, velocities.getUnchecked(midiNoteNumber) * preparation->getGain());
        preparationAtKeyOn.set(midiNoteNumber, preparation);
        
        //it might be better to do this by copy, instead of by pointer, in the off chance that the preparation disappears because of a library switch or something...
    }
    
}


//start timer for length of a particular note; called when key is pressed
void NostalgicProcessor::keyPressed(int midiNoteNumber, float midiNoteVelocity)
{
    if (keymap->containsNote(midiNoteNumber))
    {
        activeNotes.addIfNotAlreadyThere(midiNoteNumber);
        noteLengthTimers.set(midiNoteNumber, 0);
        velocities.set(midiNoteNumber, midiNoteVelocity);
        //DBG("nostalgic added active note " + std::to_string(midiNoteNumber) + " " + std::to_string(midiNoteVelocity));
    }
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
        NostalgicPreparation::Ptr noteOnPrep = preparationAtKeyOn.getUnchecked(tempnote);
        
        if (reverseLengthTimers.getUnchecked(tempnote) > reverseTargetLength.getUnchecked(tempnote))
        {
 
            if(noteOnPrep->getUndertow() > 0)
            {
                synth->keyOn(
                             midiChannel,
                             tempnote,
                             tuningsAtKeyOn.getUnchecked(tempnote) + noteOnPrep->getTransposition(),
                             velocitiesAtKeyOn.getUnchecked(tempnote) * aGlobalGain,
                             Forward,
                             FixedLengthFixedStart,
                             Nostalgic,
                             noteOnPrep->getWavedistance(),                        //start position
                             noteOnPrep->getUndertow(),                            //play length
                             aRampUndertowCrossMS,                                 //ramp up length
                             noteOnPrep->getUndertow() - aRampUndertowCrossMS);    //ramp down length
            }
            
            //remove from active notes list
            activeReverseNotes.removeFirstMatchingValue(tempnote);
        }
    }
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
