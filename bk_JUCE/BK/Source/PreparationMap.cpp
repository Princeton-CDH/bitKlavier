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

void PreparationMap::setSynchronicPreparations(Synchronic::PtrArr s)
{
    Synchronic::PtrArr oldSynchronic = synchronic;
    synchronic = s;
    
    
    // If a preparation was not previously part of the PreparationMap, add it to a new XProcesssor and add that processor to xProcessor.
    for (int i = synchronic.size(); --i >= 0;)
    {
        if (!oldSynchronic.contains(synchronic[i]))
        {
            synchronic.add(synchronic[i]);
            //synchronic[i]->processor->setCurrentPlaybackSampleRate(sampleRate);
        }
    }
    
    // If a processor contains a pointer to a preparation that is no longer one of the current preparations, remove that processor from xProcessor.
    for (int i = synchronic.size(); --i >= 0;)
    {
        if (!synchronic.contains(synchronic[i]))
            synchronic.remove(i);
    }
    
    deactivateIfNecessary();
     
     
}

void PreparationMap::setNostalgicPreparations(Nostalgic::PtrArr n)
{
    Nostalgic::PtrArr oldNostalgic = nostalgic;
    nostalgic = n;
    
    // If a preparation was not previously part of the PreparationMap, add it to a new XProcesssor and add that processor to xProcessor.
    for (int i = nostalgic.size(); --i >= 0;)
    {
        if (!oldNostalgic.contains(nostalgic[i]))
        {
            nostalgic.add(nostalgic[i]);
            //nostalgic[i]->setCurrentPlaybackSampleRate(sampleRate);
        }
    }
    
    // If a processor contains a pointer to a preparation that is no longer one of the current preparations, remove that processor from xProcessor.
    for (int i = nostalgic.size(); --i >= 0;)
    {
        if (!nostalgic.contains(nostalgic[i]))
            nostalgic.remove(i);
    }
    
    deactivateIfNecessary();
    
}

void PreparationMap::setDirectPreparations(Direct::PtrArr d)
{
    Direct::PtrArr oldDirect = direct;
    direct = d;
    
    // If a preparation was not previously part of the PreparationMap, add it to a new XProcesssor and add that processor to xProcessor.
    for (int i = direct.size(); --i >= 0;)
    {
        if (!oldDirect.contains(direct[i]))
        {
            direct.add(direct[i]);
            //direct[i]->setCurrentPlaybackSampleRate(sampleRate);
        }
    }
    
    // If a processor contains a pointer to a preparation that is no longer one of the current preparations, remove that processor from xProcessor.
    for (int i = direct.size(); --i >= 0;)
    {
        if (!direct.contains(direct[i]))
            direct.remove(i);
    }
    
    deactivateIfNecessary();
}

Synchronic::PtrArr PreparationMap::getSynchronicPreparations(void)
{
    return synchronic;
}

Nostalgic::PtrArr PreparationMap::getNostalgicPreparations(void)
{
    return nostalgic;
}

Direct::PtrArr PreparationMap::getDirectPreparations(void)
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
        if (pKeymap->containsNote(noteNumber))
            synchronic[i]->processor->keyPressed(noteNumber, velocity);
    }
    
    for (int i = nostalgic.size(); --i >= 0; )
    {
        if (pKeymap->containsNote(noteNumber))
            nostalgic[i]->processor->keyPressed(noteNumber, velocity);
    }
    
    for (int i = direct.size(); --i >= 0; )
    {
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
