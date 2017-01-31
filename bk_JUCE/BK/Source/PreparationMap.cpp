/*
  ==============================================================================

    PreparationMap.cpp
    Created: 17 Dec 2016 12:35:30pm
    Author:  Daniel Trueman

  ==============================================================================
*/

#include "PreparationMap.h"

PreparationMap::PreparationMap(BKSynthesiser *s,
             BKSynthesiser *res,
             BKSynthesiser *ham,
             Keymap::Ptr km,
             int Id):
isActive(false),
Id(Id),
pKeymap(km),
sPreparations(SynchronicPreparation::PtrArr()),
nPreparations(NostalgicPreparation::PtrArr()),
dPreparations(DirectPreparation::PtrArr()),
sProcessor(SynchronicProcessor::CSPtrArr()),
nProcessor(NostalgicProcessor::CSPtrArr()),
dProcessor(DirectProcessor::CSPtrArr()),
synth(s),
resonanceSynth(res),
hammerSynth(ham)
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

void PreparationMap::setSynchronicPreparations(SynchronicPreparation::PtrArr newPreps, SynchronicPreparation::PtrArr newActivePreps)
{
    SynchronicPreparation::PtrArr oldPreps = sPreparations;
    sPreparations = newPreps;
    
    // If a preparation was not previously part of the PreparationMap, add it to a new XProcesssor and add that processor to xProcessor.
    for (int i = sPreparations.size(); --i >= 0;)
    {
        if (!oldPreps.contains(sPreparations[i]))
        {
            SynchronicProcessor::Ptr proc = new SynchronicProcessor(synth, sPreparations[i], newActivePreps[i], i);
            sProcessor.add(proc);
            proc->setCurrentPlaybackSampleRate(sampleRate);
        }
    }
    
    // If a processor contains a pointer to a preparation that is no longer one of the current preparations, remove that processor from xProcessor.
    for (int i = sProcessor.size(); --i >= 0;)
    {
        if (!sPreparations.contains(sProcessor[i]->getPreparation()))
            sProcessor.remove(i);
    }
    
    deactivateIfNecessary();
}

void PreparationMap::setNostalgicPreparations(NostalgicPreparation::PtrArr newPreps, NostalgicPreparation::PtrArr newActivePreps)
{
    NostalgicPreparation::PtrArr oldPreps = nPreparations;
    nPreparations = newPreps;
    
    // If a preparation was not previously part of the PreparationMap, add it to a new XProcesssor and add that processor to xProcessor.
    for (int i = nPreparations.size(); --i >= 0;)
    {
        if (!oldPreps.contains(nPreparations[i]))
        {
            NostalgicProcessor::Ptr proc = new NostalgicProcessor(synth, nPreparations[i], newActivePreps[i], sProcessor, i);
            nProcessor.add(proc);
            proc->setCurrentPlaybackSampleRate(sampleRate);
        }
    }
    
    // If a processor contains a pointer to a preparation that is no longer one of the current preparations, remove that processor from xProcessor.
    for (int i = nProcessor.size(); --i >= 0;)
    {
        if (!nPreparations.contains(nProcessor[i]->getPreparation()))
            nProcessor.remove(i);
    }
    
    deactivateIfNecessary();
}

void PreparationMap::setDirectPreparations(DirectPreparation::PtrArr newPreps, DirectPreparation::PtrArr newActivePreps)
{
    DirectPreparation::PtrArr oldPreps = dPreparations;
    dPreparations = newPreps;
    
    // If a preparation was not previously part of the PreparationMap, add it to a new XProcesssor and add that processor to xProcessor.
    for (int i = dPreparations.size(); --i >= 0;)
    {
        if (!oldPreps.contains(dPreparations[i]))
        {
            DirectProcessor::Ptr proc = new DirectProcessor(synth, resonanceSynth, hammerSynth, dPreparations[i], newActivePreps[i], i);
            dProcessor.add(proc);
            proc->setCurrentPlaybackSampleRate(sampleRate);
        }
    }
    
    // If a processor contains a pointer to a preparation that is no longer one of the current preparations, remove that processor from xProcessor.
    for (int i = dProcessor.size(); --i >= 0;)
    {
        if (!dPreparations.contains(dProcessor[i]->getPreparation()))
            dProcessor.remove(i);
    }
    
    deactivateIfNecessary();
}

SynchronicPreparation::PtrArr PreparationMap::getSynchronicPreparations(void)
{
    return sPreparations;
}

NostalgicPreparation::PtrArr PreparationMap::getNostalgicPreparations(void)
{
    return nPreparations;
}

DirectPreparation::PtrArr PreparationMap::getDirectPreparations(void)
{
    return dPreparations;
}


void PreparationMap::removeAllPreparations()
{
    sPreparations.clearQuick();
    nPreparations.clearQuick();
    dPreparations.clearQuick();
    isActive = false;
}



void PreparationMap::deactivateIfNecessary()
{
    if(sPreparations.size() == 0 &&
       nPreparations.size() == 0 &&
       dPreparations.size() == 0)
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
    for (int i = sProcessor.size(); --i >= 0; )
    {
        sProcessor[i]->processBlock(numSamples, midiChannel);
    }
    
    for (int i = nProcessor.size(); --i >= 0; )
    {
        nProcessor[i]->processBlock(numSamples, midiChannel);
    }
    
    for (int i = dProcessor.size(); --i >= 0; )
    {
        dProcessor[i]->processBlock(numSamples, midiChannel);
    }
}


void PreparationMap::keyPressed(int noteNumber, float velocity, int channel)
{
    for (int i = sProcessor.size(); --i >= 0; )
    {
        if (pKeymap->containsNote(noteNumber)) sProcessor[i]->keyPressed(noteNumber, velocity);
    }
    
    for (int i = nProcessor.size(); --i >= 0; )
    {
        if (pKeymap->containsNote(noteNumber)) nProcessor[i]->keyPressed(noteNumber, velocity);
    }
    
    for (int i = dProcessor.size(); --i >= 0; )
    {
        if (pKeymap->containsNote(noteNumber)) dProcessor[i]->keyPressed(noteNumber, velocity, channel);
    }
}


void PreparationMap::keyReleased(int noteNumber, float velocity, int channel)
{
    for (int i = sProcessor.size(); --i >= 0; )
    {
        if (pKeymap->containsNote(noteNumber)) sProcessor[i]->keyReleased(noteNumber, channel);
    }
    
    for (int i = nProcessor.size(); --i >= 0; )
    {
        if (pKeymap->containsNote(noteNumber)) nProcessor[i]->keyReleased(noteNumber, channel);
    }
    
    for (int i = dProcessor.size(); --i >= 0; )
    {
        if (pKeymap->containsNote(noteNumber)) dProcessor[i]->keyReleased(noteNumber, velocity, channel);
    }
}

void PreparationMap::postRelease(int noteNumber, float velocity, int channel)
{
    for (int i = dProcessor.size(); --i >= 0; )
    {
        if (pKeymap->containsNote(noteNumber)) dProcessor[i]->keyReleased(noteNumber, velocity, channel);
    }
    
    
    for (int i = nProcessor.size(); --i >= 0; )
    {
        if (pKeymap->containsNote(noteNumber)) nProcessor[i]->keyReleased(noteNumber, channel);
    }
    
}
