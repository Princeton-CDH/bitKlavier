/*
  ==============================================================================

    Direct.cpp
    Created: 6 Dec 2016 12:46:37pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "Direct.h"

DirectProcessor::DirectProcessor(BKSynthesiser *s,
                                 BKSynthesiser *res,
                                 BKSynthesiser *ham,
                                 DirectPreparation::Ptr prep,
                                 int Id):
Id(Id),
synth(s),
resonanceSynth(res),
hammerSynth(ham),
preparation(prep),
tuner(preparation->getTuning())
{
    
}

DirectProcessor::~DirectProcessor(void)
{
    
}

void DirectProcessor::setCurrentPlaybackSampleRate(double sr)
{
    sampleRate = sr;
    tuner.setCurrentPlaybackSampleRate(sr);
}

void DirectProcessor::keyPressed(int noteNumber, float velocity, int channel)
{
    tuner.setPreparation(preparation->getTuning());
    
    tuner.keyOn(noteNumber);
    
    float offset = (preparation->getTransposition() + tuner.getOffset(noteNumber));
    int synthNoteNumber = noteNumber + (int)offset;
    offset -= (int)offset;
    
    synth->keyOn(channel,
                 synthNoteNumber,
                 offset,
                 velocity,
                 preparation->getGain() * aGlobalGain,
                 Forward,
                 Normal,
                 Main,
                 Id,
                 0, // start
                 0, // length
                 3,
                 3);
    
}

void DirectProcessor::keyReleased(int noteNumber, float velocity, int channel)
{
    float offset = (preparation->getTransposition() + tuner.getOffset(noteNumber));
    noteNumber += (int)offset;
    
    synth->keyOff(channel,
                  Main,
                  Id,
                  noteNumber,
                  velocity,
                  true);
    
    float hGain = preparation->getHammerGain();
    float rGain = preparation->getResonanceGain();
    
    if (hGain > 0.0f)
    {
        hammerSynth->keyOn(
                                 channel,
                                 noteNumber,
                                 0,
                                 velocity,
                                 hGain,
                                 Forward,
                                 Normal, //FixedLength,
                                 Hammer,
                                 Id,
                                 0,
                                 2000,
                                 3,
                                 3 );
    }
    
    if (rGain > 0.0f)
    {
        resonanceSynth->keyOn(
                                    channel,
                                    noteNumber,
                                    0, 
                                    velocity,
                                    rGain,
                                    Forward,
                                    Normal, //FixedLength,
                                    Resonance,
                                    Id,
                                    0,
                                    2000,
                                    3,
                                    3 );
    }

}

void DirectProcessor::processBlock(int numSamples, int midiChannel)
{
    tuner.incrementAdaptiveClusterTime(numSamples);
}

