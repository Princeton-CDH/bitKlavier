/*
  ==============================================================================

    PreparationMap.cpp
    Created: 17 Dec 2016 12:35:30pm
    Author:  Daniel Trueman

  ==============================================================================
*/

#include "PreparationMap.h"

PreparationMap::PreparationMap(Keymap::Ptr km,
                               int Id):
isActive(false),
Id(Id),
pKeymap(km)
{

}

PreparationMap::~PreparationMap()
{
    
}

void PreparationMap::prepareToPlay (double sr)
{
    sampleRate = sr;
}

void PreparationMap::setKeymap(Keymap::Ptr km)
{
    pKeymap = km;
}

void PreparationMap::addSynchronic(Synchronic::Ptr s)
{
    synchronic.add(s);
    deactivateIfNecessary();
}

void PreparationMap::setSynchronic(Synchronic::PtrArr s)
{
    synchronic = s;
    deactivateIfNecessary();
}

void PreparationMap::addNostalgic(Nostalgic::Ptr n)
{
    nostalgic.add(n);
    deactivateIfNecessary();
}

void PreparationMap::setNostalgic(Nostalgic::PtrArr n)
{
    nostalgic = n;
    deactivateIfNecessary();
}

void PreparationMap::addDirect(Direct::Ptr d)
{
    direct.add(d);
    deactivateIfNecessary();
}

void PreparationMap::setDirect(Direct::PtrArr d)
{
    direct = d;
    deactivateIfNecessary();
}

Synchronic::PtrArr PreparationMap::getSynchronic(void)
{
    return synchronic;
}

Nostalgic::PtrArr PreparationMap::getNostalgic(void)
{
    return nostalgic;
}

Direct::PtrArr PreparationMap::getDirect(void)
{
    return direct;
}


void PreparationMap::removeAllPreparations()
{
    synchronic.clearQuick();
    nostalgic.clearQuick();
    direct.clearQuick();
    isActive = false;
}



void PreparationMap::deactivateIfNecessary()
{
    if(synchronic.size() == 0 &&
       nostalgic.size() == 0 &&
       direct.size() == 0)
    {
        isActive = false;
    }
    else
    {
        isActive = true;
    }
}


void PreparationMap::processBlock(int numSamples, int midiChannel)
{
    for (int i = synchronic.size(); --i >= 0; )
        synchronic[i]->processor->processBlock(numSamples, midiChannel);
    
    for (int i = nostalgic.size(); --i >= 0; )
        nostalgic[i]->processor->processBlock(numSamples, midiChannel);
    
    for (int i = direct.size(); --i >= 0; )
        direct[i]->processor->processBlock(numSamples, midiChannel);
}


void PreparationMap::keyPressed(int noteNumber, float velocity, int channel)
{
    for (int i = synchronic.size(); --i >= 0; )
    {
        if (synchronic[i]->aPrep->getResetMap()->containsNote(noteNumber)) synchronic[i]->reset();
        if (pKeymap->containsNote(noteNumber))
            synchronic[i]->processor->keyPressed(noteNumber, velocity);
    }
    
    for (int i = nostalgic.size(); --i >= 0; )
    {
        if (nostalgic[i]->aPrep->getResetMap()->containsNote(noteNumber)) nostalgic[i]->reset();
        if (pKeymap->containsNote(noteNumber))
            nostalgic[i]->processor->keyPressed(noteNumber, velocity);
    }
    
    for (int i = direct.size(); --i >= 0; )
    {
        if (direct[i]->aPrep->getResetMap()->containsNote(noteNumber)) direct[i]->reset();
        if (pKeymap->containsNote(noteNumber))
            direct[i]->processor->keyPressed(noteNumber, velocity, channel);
    }
}


void PreparationMap::keyReleased(int noteNumber, float velocity, int channel)
{
    for (int i = synchronic.size(); --i >= 0; )
    {
        if (pKeymap->containsNote(noteNumber))
            synchronic[i]->processor->keyReleased(noteNumber, channel);
    }
    
    for (int i = nostalgic.size(); --i >= 0; )
    {
        if (pKeymap->containsNote(noteNumber))
            nostalgic[i]->processor->keyReleased(noteNumber, channel);
    }
    
    for (int i = direct.size(); --i >= 0; )
    {
        if (pKeymap->containsNote(noteNumber))
            direct[i]->processor->keyReleased(noteNumber, velocity, channel);
    }
}

void PreparationMap::postRelease(int noteNumber, float velocity, int channel)
{
    for (int i = direct.size(); --i >= 0; )
    {
        if (pKeymap->containsNote(noteNumber))
            direct[i]->processor->keyReleased(noteNumber, velocity, channel);
    }
    
    
    for (int i = nostalgic.size(); --i >= 0; )
    {
        if (pKeymap->containsNote(noteNumber))
            nostalgic[i]->processor->keyReleased(noteNumber, channel);
    }
    
}
