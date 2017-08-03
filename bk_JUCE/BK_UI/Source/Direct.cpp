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
Id(direct->getId()),
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
        float offset = t + tuner->getOffset(noteNumber);
        int synthNoteNumber = noteNumber + (int)offset;
        float synthOffset = offset - (int)offset;
        
        DBG("ON dtransp: " + String(synthNoteNumber) +" offset: " + String(synthOffset));
        
        synth->keyOn(channel,
                     synthNoteNumber,
                     synthOffset,
                     velocity,
                     direct->aPrep->getGain() * aGlobalGain,
                     Forward,
                     Normal,
                     MainNote,
                     Id,
                     0, // start
                     0, // length
                     3,
                     30); //release time
        
        //store synthNoteNumbers by noteNumber
        keyPlayed[noteNumber].add(synthNoteNumber);
        keyPlayedOffset[noteNumber].add(synthOffset);
        
    }
}

void DirectProcessor::keyReleased(int noteNumber, float velocity, int channel)
{
    for (int i = 0; i<keyPlayed[noteNumber].size(); i++)
    {
        int t = keyPlayed[noteNumber].getUnchecked(i);
        float t_offset = keyPlayedOffset[noteNumber].getUnchecked(i);
        
        DBG("OFF dtransp: " + String(t) +" offset: " + String(t_offset));
        
        synth->keyOff(channel,
                     MainNote,
                     Id,
                     t,
                     velocity,
                     true);
        
        //only play hammers/resonance for first note in layers of transpositions
        if(i==0)
        {
            float hGain = direct->aPrep->getHammerGain();
            float rGain = direct->aPrep->getResonanceGain();
            
            if (hGain > 0.0f)
            {
                hammerSynth->keyOn(
                                 channel,
                                 //synthNoteNumber,
                                 t,
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
                                    //synthNoteNumber,
                                    t,
                                    //synthOffset,
                                    t_offset,
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
    }

    keyPlayed[noteNumber].clearQuick();
    keyPlayedOffset[noteNumber].clearQuick();
    
}

void DirectProcessor::processBlock(int numSamples, int midiChannel)
{
    //tuner->processor->incrementAdaptiveClusterTime(numSamples);
}

