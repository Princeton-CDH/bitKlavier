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
                int layer):
layer(layer),
synth(s),
keymap(km),
preparation(prep)
{
    
}

DirectProcessor::~DirectProcessor(void)
{
    
}

void DirectProcessor::keyPressed(int noteNumber, float velocity)
{
    if (keymap->containsNote(noteNumber))
    {
        
    }
}

void DirectProcessor::keyReleased(int noteNumber, int channel)
{
    if (keymap->containsNote(noteNumber))
    {
        
    }
}

