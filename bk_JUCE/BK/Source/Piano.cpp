/*
  ==============================================================================

    Piano.cpp
    Created: 17 Dec 2016 12:35:30pm
    Author:  Daniel Trueman

  ==============================================================================
*/

#include "Piano.h"

Piano::Piano(int pianoNum,
             BKSynthesiser *s,
             BKSynthesiser *res,
             BKSynthesiser *ham):
pKeymap(new Keymap(pianoNum)),
pianoNumber(pianoNum),
synth(s),
resonanceSynth(res),
hammerSynth(ham),
sProcessor(SynchronicProcessor::CSArr()),
nProcessor(NostalgicProcessor::Arr()),
dProcessor(DirectProcessor::Arr()),
sPreparation(SynchronicPreparation::CSArr()),
nPreparation(NostalgicPreparation::CSArr()),
dPreparation(DirectPreparation::CSArr())
{
    sProcessor.ensureStorageAllocated(aMaxNumPreparationsPerPianos);
    nProcessor.ensureStorageAllocated(aMaxNumPreparationsPerPianos);
    dProcessor.ensureStorageAllocated(aMaxNumPreparationsPerPianos);
    
    tPreparation.ensureStorageAllocated(aMaxNumPreparationsPerPianos * 3);
    
    for (int i = 0; i < (3 * aMaxNumPreparationsPerPianos); i++)
    {
        tPreparation.add(new TuningPreparation(i));
    }
    
    for (int i = 0; i < aMaxNumPreparationsPerPianos; i++)
    {
        sPreparation.add(new SynchronicPreparation(i, tPreparation[0]));
        nPreparation.add(new NostalgicPreparation(i, tPreparation[0]));
        dPreparation.add(new DirectPreparation(i, tPreparation[0]));
    }
    
    for (int i = 0; i < aMaxNumPreparationsPerPianos; i++)
    {
        sProcessor.insert(i, new SynchronicProcessor(synth, pKeymap, sPreparation[0], i));
        nProcessor.insert(i, new NostalgicProcessor(synth, pKeymap, nPreparation[0], sProcessor, i));
        dProcessor.insert(i, new DirectProcessor(synth, resonanceSynth, hammerSynth, pKeymap, dPreparation[0], i));
    }
    
    for (int i = 0; i < 128; i++)
    {
        pKeymap->addNote(i);
    }
    
}

Piano::~Piano()
{
    
}

void Piano::setCurrentPlaybackSampleRate(double sr)
{
    sampleRate = sr;
    
    for (int i = 0; i < aMaxNumPreparationsPerPianos; i++)
    {
        sProcessor[i]->setCurrentPlaybackSampleRate(sampleRate);
        nProcessor[i]->setCurrentPlaybackSampleRate(sampleRate);
        dProcessor[i]->setCurrentPlaybackSampleRate(sampleRate);
    }
    
}

void Piano::processBlock(int numSamples, int midiChannel)
{
    for (int layer = 0; layer < aMaxNumPreparationsPerPianos; layer++)
    {
        sProcessor[layer]->processBlock(numSamples, midiChannel); //precede with if sProcessor[layer]->isActive check
        nProcessor[layer]->processBlock(numSamples, midiChannel);
        dProcessor[layer]->processBlock(numSamples, midiChannel);
    }

}

void Piano::keyPressed(int noteNumber, float velocity, int channel)
{
    for (int layer = 0; layer < aMaxNumPreparationsPerPianos; layer++)
    {
        sProcessor[layer]->keyPressed(noteNumber, velocity); //precede with if sProcessor[layer]->isActive check
        nProcessor[layer]->keyPressed(noteNumber, velocity);
        dProcessor[layer]->keyPressed(noteNumber, velocity, channel);
    }
}

void Piano::keyReleased(int noteNumber, float velocity, int channel)
{
    for (int i = 0; i < aMaxNumPreparationsPerPianos; i++)
    {
        sProcessor[i]->keyReleased(noteNumber, channel); //precede with if sProcessor[layer]->isActive check
        nProcessor[i]->keyReleased(noteNumber, channel);
        dProcessor[i]->keyReleased(noteNumber, velocity, channel);
    }
}
