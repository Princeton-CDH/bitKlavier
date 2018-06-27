/*
  ==============================================================================

    Direct.cpp
    Created: 6 Dec 2016 12:46:37pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "Direct.h"

DirectProcessor::DirectProcessor(Direct::Ptr direct,
                                 TuningProcessor::Ptr tuning,
                                 BKSynthesiser *s,
                                 BKSynthesiser *res,
                                 BKSynthesiser *ham):
synth(s),
resonanceSynth(res),
hammerSynth(ham),
direct(direct),
tuner(tuning)
{
    
}

DirectProcessor::~DirectProcessor(void)
{
    
}

void DirectProcessor::keyPressed(int noteNumber, float velocity, int channel)
{
    for (auto t : direct->aPrep->getTransposition())
    {
        float offset = t + tuner->getOffset(noteNumber), synthOffset = offset;
        int synthNoteNumber = noteNumber;
        
        //if (sampleType < BKLoadSoundfont)
        {
            synthNoteNumber += (int)offset;
            synthOffset     -= (int)offset;
        }

        synth->keyOn(channel,
                     noteNumber,
                     synthNoteNumber,
                     synthOffset,
                     velocity,
                     direct->aPrep->getGain() * aGlobalGain,
                     Forward,
                     Normal,
                     MainNote,
                     direct->getId(),
                     0,     // start
                     0,     // length
                     direct->aPrep->getAttack(),
                     direct->aPrep->getDecay(),
                     direct->aPrep->getSustain(),
                     direct->aPrep->getRelease());
        
        
        //store synthNoteNumbers by noteNumber
        keyPlayed[noteNumber].add(synthNoteNumber);
        keyPlayedOffset[noteNumber].add(synthOffset);
        
    }
}

#define HAMMER_GAIN_SCALE 0.02f
#define RES_GAIN_SCALE 0.2f
void DirectProcessor::keyReleased(int noteNumber, float velocity, int channel)
{
    for (int i = 0; i<keyPlayed[noteNumber].size(); i++)
    {
        int t = keyPlayed[noteNumber].getUnchecked(i);
        float t_offset = keyPlayedOffset[noteNumber].getUnchecked(i);
        
        synth->keyOff(channel,
                      MainNote,
                      direct->getId(),
                      noteNumber,
                      t,
                      velocity,
                      true);
    }

    keyPlayed[noteNumber].clearQuick();
    keyPlayedOffset[noteNumber].clearQuick();
    
}

void DirectProcessor::playHammerResonance(int noteNumber, float velocity, int channel)
{
    for (int i = 0; i<keyPlayed[noteNumber].size(); i++)
    {
        int t = keyPlayed[noteNumber].getUnchecked(i);
        float t_offset = keyPlayedOffset[noteNumber].getUnchecked(i);
        
        //only play hammers/resonance for first note in layers of transpositions
        if(i==0)
        {
            float hGain = direct->aPrep->getHammerGain();
            float rGain = direct->aPrep->getResonanceGain();
            
            if (hGain > 0.0f)
            {
                hammerSynth->keyOn(channel,
                                   noteNumber,
                                   t,
                                   0,
                                   velocity,
                                   hGain * HAMMER_GAIN_SCALE,
                                   Forward,
                                   Normal, 
                                   HammerNote,
                                   direct->getId(),
                                   0,
                                   2000,
                                   3,
                                   3 );
            }
            
            if (rGain > 0.0f)
            {
                resonanceSynth->keyOn(channel,
                                      noteNumber,
                                      t,
                                      t_offset,
                                      velocity,
                                      rGain * RES_GAIN_SCALE,
                                      Forward,
                                      Normal,
                                      ResonanceNote,
                                      direct->getId(),
                                      0,
                                      2000,
                                      3,
                                      3 );
                
            }
        }
    }
}

void DirectProcessor::processBlock(int numSamples, int midiChannel, BKSampleLoadType type)
{
    sampleType = type;
    //tuner->processor->incrementAdaptiveClusterTime(numSamples);
}

