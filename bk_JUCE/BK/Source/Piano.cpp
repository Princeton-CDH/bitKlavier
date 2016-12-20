/*
  ==============================================================================

    Piano.cpp
    Created: 17 Dec 2016 12:35:30pm
    Author:  Daniel Trueman

  ==============================================================================
*/

#include "Piano.h"

Piano::Piano(BKSynthesiser *s,
             BKSynthesiser *res,
             BKSynthesiser *ham,
             int pianoNum):
pianoNumber(pianoNum),
isActive(false),
synth(s),
resonanceSynth(res),
hammerSynth(ham)
{
    //allocate memory for Processors
    sProcessor.ensureStorageAllocated(aMaxNumPreparationsPerPianos);
    nProcessor.ensureStorageAllocated(aMaxNumPreparationsPerPianos);
    dProcessor.ensureStorageAllocated(aMaxNumPreparationsPerPianos);
    
    //initialize Processors
    for (int i = 0; i < aMaxNumPreparationsPerPianos; i++)
    {
        sProcessor.insert(i, new SynchronicProcessor(synth,
                                                     new SynchronicPreparation(0, new TuningPreparation(0)),
                                                     i));
        nProcessor.insert(i, new NostalgicProcessor(synth,
                                                    new NostalgicPreparation(0, new TuningPreparation(0)),
                                                    sProcessor,
                                                    i));
        dProcessor.insert(i, new DirectProcessor(synth,
                                                 resonanceSynth,
                                                 hammerSynth,
                                                 new DirectPreparation(0, new TuningPreparation(0)),
                                                 i));
    }
    
}

Piano::~Piano()
{
    
}

void Piano::prepareToPlay (double sr)
{
    sampleRate = sr;
    
    for (int i = 0; i < aMaxNumPreparationsPerPianos; i++)
    {
        sProcessor[i]->setCurrentPlaybackSampleRate(sampleRate);
        nProcessor[i]->setCurrentPlaybackSampleRate(sampleRate);
        dProcessor[i]->setCurrentPlaybackSampleRate(sampleRate);
    }
}

void Piano::setKeymap(Keymap::Ptr km)
{
    pKeymap = km;
    
}

void Piano::removeAllPreparations()
{
    
    isActive = false;
}

//perhaps we can refactor these by Processor/Preparation type?
void Piano::addSynchronic(SynchronicPreparation::Ptr sp)
{
    //add Preparation and make sure this Piano is active
    sPreparations.addIfNotAlreadyThere(sp);
    isActive = true;
    
    //add a new Processor if necessary
    if(sProcessor.size() < sPreparations.size())
    {
        sProcessor.add(new SynchronicProcessor(synth, sp, sPreparations.size()));
        sProcessor[sProcessor.size() - 1]->setCurrentPlaybackSampleRate(sampleRate);
    }
    //or assign preparation to existing processor
    else sProcessor[sPreparations.indexOf(sp)]->setPreparation(sp);

}
void Piano::addNostalgic(NostalgicPreparation::Ptr np)
{
    nPreparations.addIfNotAlreadyThere(np);
    isActive = true;
    if(nProcessor.size() < nPreparations.size()) {
        nProcessor.add(new NostalgicProcessor(synth, np, sProcessor, nPreparations.size()));
        nProcessor[nProcessor.size() - 1]->setCurrentPlaybackSampleRate(sampleRate);
    }
    else nProcessor[nPreparations.indexOf(np)]->setPreparation(np);
}

void Piano::addDirect(DirectPreparation::Ptr dp)
{
    dPreparations.addIfNotAlreadyThere(dp);
    isActive = true;
    if(dProcessor.size() < dPreparations.size()) {
        dProcessor.add(new DirectProcessor(synth, resonanceSynth, hammerSynth, dp, dPreparations.size()));
        dProcessor[dProcessor.size() - 1]->setCurrentPlaybackSampleRate(sampleRate);
    }
    else dProcessor[dPreparations.indexOf(dp)]->setPreparation(dp);
}

void Piano::removeSynchronic(SynchronicPreparation::Ptr sp)
{
    sPreparations.removeFirstMatchingValue(sp);
    deactivateIfNecessary();
}

void Piano::removeNostalgic(NostalgicPreparation::Ptr np)
{
    nPreparations.removeFirstMatchingValue(np);
    deactivateIfNecessary();
}

void Piano::removeDirect(DirectPreparation::Ptr dp)
{
    dPreparations.removeFirstMatchingValue(dp);
    deactivateIfNecessary();
}

void Piano::deactivateIfNecessary()
{
    if(sPreparations.size() == 0 &&
       nPreparations.size() == 0 &&
       dPreparations.size() == 0)
        isActive = false;
}


void Piano::processBlock(int numSamples, int midiChannel)
{
    for (int layer = 0; layer < sPreparations.size(); layer++)
    {
        sProcessor[layer]->processBlock(numSamples, midiChannel);
    }
    
    for (int layer = 0; layer < nPreparations.size(); layer++)
    {
        nProcessor[layer]->processBlock(numSamples, midiChannel);
    }
    
    for (int layer = 0; layer < dPreparations.size(); layer++)
    {
        dProcessor[layer]->processBlock(numSamples, midiChannel);
    }
}

void Piano::keyPressed(int noteNumber, float velocity, int channel)
{
    for (int layer = 0; layer < sPreparations.size(); layer++)
    {
        if (pKeymap->containsNote(noteNumber)) sProcessor[layer]->keyPressed(noteNumber, velocity);
    }
    
    for (int layer = 0; layer < nPreparations.size(); layer++)
    {
        if (pKeymap->containsNote(noteNumber)) nProcessor[layer]->keyPressed(noteNumber, velocity);
    }
    
    for (int layer = 0; layer < dPreparations.size(); layer++)
    {
        if (pKeymap->containsNote(noteNumber)) dProcessor[layer]->keyPressed(noteNumber, velocity, channel);
    }
}

void Piano::keyReleased(int noteNumber, float velocity, int channel)
{
    for (int layer = 0; layer < sPreparations.size(); layer++)
    {
        if (pKeymap->containsNote(noteNumber)) sProcessor[layer]->keyReleased(noteNumber, velocity);
    }
    
    for (int layer = 0; layer < nPreparations.size(); layer++)
    {
        if (pKeymap->containsNote(noteNumber)) nProcessor[layer]->keyReleased(noteNumber, velocity);
    }
    
    for (int layer = 0; layer < dPreparations.size(); layer++)
    {
        if (pKeymap->containsNote(noteNumber)) dProcessor[layer]->keyReleased(noteNumber, velocity, channel);
    }
}
