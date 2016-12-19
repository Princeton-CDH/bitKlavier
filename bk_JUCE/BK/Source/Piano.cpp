/*
  ==============================================================================

    Piano.cpp
    Created: 17 Dec 2016 12:35:30pm
    Author:  Daniel Trueman

  ==============================================================================
*/

#include "Piano.h"

Piano::Piano(int pianoNum):
isActive(false),
pianoNumber(pianoNum)
{
    
}

Piano::~Piano()
{
    
}

void Piano::setKeymap(Keymap::Ptr km)
{
    pKeymap = km;
    
}

void Piano::removeAllPreparations()
{
    sProcessors.clearQuick();
    nProcessors.clearQuick();
    dProcessors.clearQuick();
    
    isActive = false;
}


void Piano::processBlock(int numSamples, int midiChannel)
{
    for (int layer = 0; layer < sProcessors.size(); layer++)
    {
        sProcessors[layer]->processBlock(numSamples, midiChannel);
    }
    
    for (int layer = 0; layer < nProcessors.size(); layer++)
    {
        nProcessors[layer]->processBlock(numSamples, midiChannel);
    }
    
    for (int layer = 0; layer < dProcessors.size(); layer++)
    {
        dProcessors[layer]->processBlock(numSamples, midiChannel);
    }
}

void Piano::keyPressed(int noteNumber, float velocity, int channel)
{
    for (int layer = 0; layer < sProcessors.size(); layer++)
    {
        if (pKeymap->containsNote(noteNumber)) sProcessors[layer]->keyPressed(noteNumber, velocity);
    }
    
    for (int layer = 0; layer < nProcessors.size(); layer++)
    {
        if (pKeymap->containsNote(noteNumber)) nProcessors[layer]->keyPressed(noteNumber, velocity);
    }
    
    for (int layer = 0; layer < dProcessors.size(); layer++)
    {
        if (pKeymap->containsNote(noteNumber)) dProcessors[layer]->keyPressed(noteNumber, velocity, channel);
    }
}

void Piano::keyReleased(int noteNumber, float velocity, int channel)
{
    for (int layer = 0; layer < sProcessors.size(); layer++)
    {
        if (pKeymap->containsNote(noteNumber)) sProcessors[layer]->keyReleased(noteNumber, velocity);
    }
    
    for (int layer = 0; layer < nProcessors.size(); layer++)
    {
        if (pKeymap->containsNote(noteNumber)) nProcessors[layer]->keyReleased(noteNumber, velocity);
    }
    
    for (int layer = 0; layer < dProcessors.size(); layer++)
    {
        if (pKeymap->containsNote(noteNumber)) dProcessors[layer]->keyReleased(noteNumber, velocity, channel);
    }
}
