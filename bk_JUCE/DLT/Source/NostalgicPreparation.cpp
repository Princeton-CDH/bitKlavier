//
//  NostalgicPreparation.cpp
//  MRM
//
//  Created by Daniel Trueman on 11/1/16.
//
//

#include "NostalgicPreparation.h"

//constructor
NostalgicPreparation::NostalgicPreparation()
{
    
    for(int i=0;i<128;i++)
    {
        timers.insert(i, 0); //initialize timers for all notes
        DBG("nostalgic initialized timer " + std::to_string(i) + " to " + std::to_string( timers.getUnchecked(i) ) );
        
        velocities.insert(i, 0); //store noteOn velocities to set Nostalgic velocities
    }

}

//destructor
NostalgicPreparation::~NostalgicPreparation() {}

//start timer for length of a particular note
void NostalgicPreparation::startTimer(int midiNoteNumber, float midiNoteVelocity)
{
    activeNotes.addIfNotAlreadyThere(midiNoteNumber);
    velocities.set(midiNoteNumber, midiNoteVelocity);
    DBG("nostalgic added active note " + std::to_string(midiNoteNumber) + " " + std::to_string(midiNoteVelocity));
}


//get length of note
int NostalgicPreparation::getTimer(int midiNoteNumber)
{
    return timers.getUnchecked(midiNoteNumber);
}

float NostalgicPreparation::getVelocity(int midiNoteNumber)
{
    return velocities.getUnchecked(midiNoteNumber);
}

//increment timer for all active notes
void NostalgicPreparation::incrementTimer(int numSamples)
{
    for(int i=0;i<activeNotes.size();i++)
    {
        timers.set(activeNotes.getUnchecked(i), timers.getUnchecked(activeNotes.getUnchecked(i)) + numSamples);
    }
}


//clear note and reset timer
void NostalgicPreparation::clearNote(int midiNoteNumber)
{
    activeNotes.removeFirstMatchingValue(midiNoteNumber);
    timers.set(midiNoteNumber, 0);
    DBG("nostalgic removed active note " + std::to_string(midiNoteNumber));
}
