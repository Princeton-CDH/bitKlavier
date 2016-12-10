/*
  ==============================================================================

    Direct.cpp
    Created: 6 Dec 2016 12:46:37pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "Direct.h"


DirectProcessor::DirectProcessor(BKSynthesiser *s,
                Keymap::Ptr km,
                DirectPreparation::Ptr prep,
                TuningPreparation::Ptr tPrep,
                int id):
Id(id),
synth(s),
keymap(km),
preparation(prep),
tPreparation(tPrep),
tuner(tPrep, id)
{
    
}

DirectProcessor::~DirectProcessor(void)
{
    
}

void DirectProcessor::keyPressed(int noteNumber, float velocity, int channel)
{
    if (keymap->containsNote(noteNumber))
    {
        synth->keyOn(
                     channel,
                     noteNumber,
                     tuner.getOffset(noteNumber), //will need to add Direct Transp here
                     velocity * aGlobalGain,
                     Forward,
                     Normal,
                     BKNoteTypeNil,
                     1000, // start
                     1000, // length
                     3,
                     3 );
        
    }
}

void DirectProcessor::keyReleased(int noteNumber, float velocity, int channel)
{
    if (keymap->containsNote(noteNumber))
    {
        synth->keyOff(
                      channel,
                      noteNumber,
                      velocity,
                      true
                      );
    }
}

