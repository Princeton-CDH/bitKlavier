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
                                 DirectPreparation::Ptr activePrep,
                                 int Id):
Id(Id),
synth(s),
resonanceSynth(res),
hammerSynth(ham),
active(activePrep),
tuner(active->getTuning()->processor)
{
    
}

DirectProcessor::~DirectProcessor(void)
{
    
}

void DirectProcessor::setCurrentPlaybackSampleRate(double sr)
{
    sampleRate = sr;
    tuner->setCurrentPlaybackSampleRate(sr);
}

void DirectProcessor::keyPressed(int noteNumber, float velocity, int channel)
{
    tuner = active->getTuning()->processor;
    tuner->keyOn(noteNumber);
    
    float offset = (active->getTransposition() + tuner->getOffset(noteNumber));
    int synthNoteNumber = noteNumber + (int)offset;
    offset -= (int)offset;
    
    synth->keyOn(channel,
                 synthNoteNumber,
                 offset,
                 velocity,
                 active->getGain() * aGlobalGain,
                 Forward,
                 Normal,
                 MainNote,
                 Id,
                 0, // start
                 0, // length
                 3,
                 3);
    
}

void DirectProcessor::keyReleased(int noteNumber, float velocity, int channel)
{
    float offset = (active->getTransposition() + tuner->getOffset(noteNumber));
    noteNumber += (int)offset;
    
    synth->keyOff(channel,
                  MainNote,
                  Id,
                  noteNumber,
                  velocity,
                  true);
    
    float hGain = active->getHammerGain();
    float rGain = active->getResonanceGain();
    
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
                                 HammerNote,
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
                                    offset,
                                    velocity,
                                    rGain,
                                    Forward,
                                    Normal, //FixedLength,
                                    ResonanceNote,
                                    Id,
                                    0,
                                    2000,
                                    3,
                                    3 );
    }

}

void DirectProcessor::processBlock(int numSamples, int midiChannel)
{
    tuner->incrementAdaptiveClusterTime(numSamples);
}

