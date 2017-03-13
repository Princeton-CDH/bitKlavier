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
    //tuner->keyOn(noteNumber);
    
    for (auto t : active->getTransposition())
    {
        float offset = t + tuner->getOffset(noteNumber);
        int synthNoteNumber = noteNumber + (int)offset;
        float synthOffset = offset - (int)offset;
        
        synth->keyOn(channel,
                     synthNoteNumber,
                     synthOffset,
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
        
        //store synthNoteNumbers by noteNumber
        keyPlayed[noteNumber].add(synthNoteNumber);
        keyPlayedOffset[noteNumber].add(synthOffset);
        
    }
    
    
    
}

void DirectProcessor::keyReleased(int noteNumber, float velocity, int channel)
{

    /*
     for (auto t : active->getTransposition())
     {
         float offset = t + tuner->getOffset(noteNumber);
         int synthNoteNumber = noteNumber + (int)offset;
         float synthOffset = offset - (int)offset;
         
         //lookup synthNoteNumber by noteNumber, stored at keyPressed time,
         //to make sure we keyOff the right "note" in the event of a preparation change mid-note.
         
         synth->keyOff(channel,
                      MainNote,
                      Id,
                      synthNoteNumber,
                      velocity,
                      true);
     
     */
    
    for (int i = 0; i<keyPlayed[noteNumber].size(); i++)
    {
        int t = keyPlayed[noteNumber].getUnchecked(i);
        float t_offset = keyPlayedOffset[noteNumber].getUnchecked(i);
        
        synth->keyOff(channel,
                     MainNote,
                     Id,
                     t,
                     velocity,
                     true);
        
        //float offset = tuner->getOffset(noteNumber);
        //int synthNoteNumber = noteNumber + (int)offset;
        //float synthOffset = offset - (int)offset;
        
        //only play hammers/resonance for first note in layers of transpositions
        if(i==0)
        {
            float hGain = active->getHammerGain();
            float rGain = active->getResonanceGain();
            
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
    //tuner->incrementAdaptiveClusterTime(numSamples);
}

