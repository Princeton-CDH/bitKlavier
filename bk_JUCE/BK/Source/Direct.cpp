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
                                 Keymap::Ptr km,
                                 DirectPreparation::Ptr prep,
                                 int id):
Id(id),
synth(s),
resonanceSynth(res),
hammerSynth(ham),
keymap(km),
preparation(prep),
tuner(preparation->getTuning(), id)
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
    
    if (keymap->containsNote(noteNumber))
    {
        tuner.keyOn(noteNumber);
        synth->keyOn(channel,
                     noteNumber,
                     tuner.getOffset(noteNumber) + preparation->getTransposition(),
                     velocity,
                     preparation->getGain() * aGlobalGain,
                     Forward,
                     Normal,
                     Main,
                     0, // start
                     0, // length
                     3,
                     3);
        
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
                      true);
        
        float hGain = preparation->getHammerGain();
        float rGain = preparation->getResonanceGain();
        
        if (hGain > 0.0f)
        {
            DBG("hammer: " + String(hGain));
            hammerSynth->keyOn(
                                     channel,
                                     noteNumber,
                                     0,
                                     velocity,
                                     hGain,
                                     Forward,
                                     FixedLength,
                                     Hammer,
                                     0,
                                     2000,
                                     3,
                                     3 );
        }
        
        if (rGain > 0.0f)
        {
            DBG("release: " + String(rGain));
            resonanceSynth->keyOn(
                                        channel,
                                        noteNumber,
                                        0, 
                                        velocity,
                                        rGain,
                                        Forward,
                                        FixedLength,
                                        Resonance,
                                        0,
                                        2000,
                                        3,
                                        3 );
        }
    }
}

void DirectProcessor::processBlock(int numSamples, int midiChannel)
{
    tuner.incrementAdaptiveClusterTime(numSamples);
}

