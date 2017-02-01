/*
  ==============================================================================

    Synchronic.cpp
    Created: 22 Nov 2016 3:46:35pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "Synchronic.h"

SynchronicProcessor::SynchronicProcessor(BKSynthesiser *s,
                                         SynchronicPreparation::Ptr p,
                                         SynchronicPreparation::Ptr ap,
                                         int Id)
:
Id(Id),
synth(s),
preparation(p),
active(ap),
tuner(active->getTuning())
{
    velocities.ensureStorageAllocated(128);
    for (int i = 0; i < 128; i++) {
        velocities.insert(i, 0.);
    }
    
    clusterTimer = 0;
    phasor = 0;
    
    inPulses = false;
    inCluster = false;
    
    cluster = Array<int>();
    keysDepressed = Array<int>();
    
    shouldPlay = false;
}



SynchronicProcessor::~SynchronicProcessor()
{
}

void SynchronicProcessor::setCurrentPlaybackSampleRate(double sr)
{
    sampleRate = sr;
    tuner.setCurrentPlaybackSampleRate(sr);
}



void SynchronicProcessor::playNote(int channel, int note, float velocity)
{
    PianoSamplerNoteDirection noteDirection = Forward;
    float noteStartPos = 0.0;
    
    //float noteLength = (fabs(active->getLengthMultipliers()[length]) * tempoPeriod);
    float noteLength = (fabs(active->getLengthMultipliers()[length]) * 50.0);
    
    if (active->getLengthMultipliers()[length] < 0)
    {
        noteDirection = Reverse;
        noteStartPos = noteLength;
    }
    
    float offset = tuner.getOffset(note) + active->getTranspOffsets()[transp];
    int synthNoteNumber = ((float)note + (int)offset);
    offset -= (int)offset;
    
    synth->keyOn(channel,
                 synthNoteNumber,
                 offset,
                 //velocity * preparation->getAccentMultipliers()[accent],
                 velocity,
                 aGlobalGain,
                 noteDirection,
                 FixedLengthFixedStart,
                 Synchronic,
                 Id,
                 noteStartPos, // start
                 noteLength,
                 3,
                 30);
}

void SynchronicProcessor::resetPhase(int skipBeats)
{
    
    beat    = skipBeats % active->getBeatMultipliers().size();
    length  = skipBeats % active->getLengthMultipliers().size();
    accent  = skipBeats % active->getAccentMultipliers().size();
    transp  = skipBeats % active->getTranspOffsets().size();
    
    pulse   = 0;

}

void SynchronicProcessor::keyPressed(int noteNumber, float velocity)
{

    //set tuning
    tuner.setPreparation(active->getTuning());
    
    //store velocity
    velocities.set(noteNumber, velocity);
 
    //add note to array of depressed notes
    keysDepressed.addIfNotAlreadyThere(noteNumber);
    
    //silence pulses if in NoteOffSync
    if(active->getMode() == LastNoteOffSync) shouldPlay = false;
    
    //cluster management
    if(!inCluster) //we have a new cluster
    {
        
        //reset phasor
        phasor = 0;
        
        //clear cluster
        cluster.clearQuick();
        
        //reset parameter counters; need to account for skipBeats
        resetPhase(active->getBeatsToSkip());
        
        //start pulses, unless waiting in noteOff mode
        if(active->getMode() != LastNoteOffSync) shouldPlay = true;
        
        //now we are in a cluster!
        inCluster = true;
        
    }
    else if (active->getMode() == LastNoteOnSync)
    {
        
        //reset phasor if in LastNoteOnSync
        phasor = 0;
        resetPhase(active->getBeatsToSkip());

    }
    
    //at this point, we are in cluster one way or another!
    
    //save note in the cluster, even if it's already there. then cap the cluster to a hard cap (8 for now)
    //this is different than avoiding dupes at this stage (with "addIfNotAlreadyThere") because it allows
    //repeated notes to push older notes out the back.
    //later, we remove dupes so we don't inadvertently play the same note twice in a pulse
    
    cluster.insert(0, noteNumber);
    if(cluster.size() > active->getClusterCap()) cluster.resize(active->getClusterCap());
    
    //why not use clusterMax for this? the intent is different:
    //clusterMax: max number of notes played otherwise shut off pulses
    //clusterCap: the most number of notes allowed in a cluster when playing pulses
    //so, let's say we are playing a rapid passage where successive notes are within the clusterThresh
    //and we want the pulse to emerge when we stop; clusterMax wouldn't allow this to happen
    //if we had exceeded clusterMax in that passage, which is bad, but we still want clusterCap
    //to limit the number of notes included in the cluster.
    //for now, we'll leave clusterCap unexposed, just to avoid confusion for the user. after all,
    //I used it this way for all of the etudes to date! but might want to expose eventually...
    //perhaps call pulseVoices? since it's essentially the number of "voices" in the pulse chord?
    
    //reset the timer for time between notes
    clusterThresholdTimer = 0;
    
}


void SynchronicProcessor::keyReleased(int noteNumber, int channel)
{
    
    //remove key from array of pressed keys
    keysDepressed.removeAllInstancesOf(noteNumber);
    
    // If LastNoteOnSync mode, reset phasor and multiplier indices.
    //only initiate pulses if ALL keys are released
    if (active->getMode() == LastNoteOffSync && keysDepressed.size() == 0)
    {
        phasor = pulseThresholdSamples; //start right away
        resetPhase(active->getBeatsToSkip());

        shouldPlay = true;

    }
}


void SynchronicProcessor::processBlock(int numSamples, int channel)
{
    
    //adaptive tuning timer update
    tuner.incrementAdaptiveClusterTime(numSamples);
    
    //need to do this every block?
    clusterThresholdSamples = (active->getClusterThreshSEC() * sampleRate);
    pulseThresholdSamples = (active->getPulseThresh() * sampleRate);
    
    if (inCluster)
    {
        if (clusterThresholdTimer >= clusterThresholdSamples)
        {
            inCluster = false;
        }
        else
        {
            clusterThresholdTimer += numSamples;
        }
    }
    
    if(shouldPlay)
    {
        
        //remove duplicates from cluster, so we don't play the same note twice in a single pulse
        slimCluster.clearQuick();
        for(int i = 0; i< cluster.size(); i++) slimCluster.addIfNotAlreadyThere(cluster.getUnchecked(i));
        int clusterSize = slimCluster.size();
        
        numSamplesBeat = (active->getBeatMultipliers()[beat] * pulseThresholdSamples);
        
        if (phasor >= numSamplesBeat)
        {
            
            phasor -= numSamplesBeat;
            
            DBG("shouldPlay: "      + String((int)shouldPlay) +
                " accent: "         + String(active->getAccentMultipliers()[accent]) +
                " accent counter: " + String(accent)
                );
            
            if (clusterSize >= active->getClusterMin() && clusterSize <= active->getClusterMax())
            {
                for (int n = 0; n < clusterSize; n++)
                {
                    playNote(channel,
                             cluster[n],
                             velocities.getUnchecked(cluster[n]) * active->getAccentMultipliers()[accent]);
                }
                
            }
            
            if (++beat      >= active->getBeatMultipliers().size())         beat = 0;
            if (++length    >= active->getLengthMultipliers().size())     length = 0;
            if (++accent    >= active->getAccentMultipliers().size())     accent = 0;
            if (++transp    >= active->getTranspOffsets().size())         transp = 0;
            
            if (++pulse     >= active->getNumPulses())
            {
                cluster.clearQuick();
                shouldPlay = false;
            }
        }
        
        phasor += numSamples;
    }
    
}


float SynchronicProcessor::getTimeToBeatMS(float beatsToSkip) //return time in ms to future beat, given beatsToSkip
{
    uint64 timeToReturn = numSamplesBeat - phasor; //next beat
    int myBeat = beat;
    
    while(beatsToSkip-- > 0)
    {
        if (++myBeat >= active->getBeatMultipliers().size()) myBeat = 0;
        timeToReturn += (active->getBeatMultipliers()[myBeat] * pulseThresholdSamples);
    }
    
    //DBG("time in ms to skipped beat = " + std::to_string(timeToReturn * 1000./sampleRate));
    return timeToReturn * 1000./sampleRate;
}



    


