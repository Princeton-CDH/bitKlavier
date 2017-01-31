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
                                         int Id)
:
Id(Id),
synth(s),
preparation(p),
tuner(preparation->getTuning())
{
    velocities.ensureStorageAllocated(128);
    for (int i = 0; i < 128; i++) {
        velocities.insert(i, 0.);
    }
    
    clusterTimer = 0;
    phasor = 0;
    
    inPulses = false;
    inCluster = false;
    
    firstNoteTimer = 0;
    
    cluster = Array<int>();
    keysDepressed = Array<int>();
    
    skipBeats = 0;
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
    
    //float noteLength = (fabs(preparation->getLengthMultipliers()[length]) * tempoPeriod);
    float noteLength = (fabs(preparation->getLengthMultipliers()[length]) * 50.0);
    
    if (preparation->getLengthMultipliers()[length] < 0)
    {
        noteDirection = Reverse;
        noteStartPos = noteLength;
    }
    
    float offset = tuner.getOffset(note) + preparation->getTranspOffsets()[transp];
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

void SynchronicProcessor::keyPressed(int noteNumber, float velocity)
{
    tuner.setPreparation(preparation->getTuning());
    
    //store velocity
    velocities.set(noteNumber, velocity);
    DBG("adding note " + String(noteNumber) + " " + String(velocity));
    
    //add note to array of depressed notes
    keysDepressed.addIfNotAlreadyThere(noteNumber);
    
    //make sure we aren't playing in NoteOffSync
    if(LastNoteOffSync) shouldPlay = false;
    
    if (inCluster)
    {
        // If LastNoteOnSync mode, reset phasor and multiplier indices.
        if (preparation->getMode() == LastNoteOnSync)
        {
            phasor = pulseThresholdSamples;
            
            if (clusterThresholdSamples > pulseThresholdSamples)
            {
                pulse = 0;
            }
            else
            {
                pulse = 1;
            }
            
            beat = 0;
            length = 0;
            accent = 0;
            transp = 0;
            
            inPrePulses = true;
            pulseThresholdTimer = 0;
        }
        
        
    }
    else
    {
        cluster.clearQuick();
        inPulses = false;
        
        // Start first note timer, since this is beginning of new cluster.
        if (preparation->getMode() == FirstNoteOnSync)
        {
            firstNoteTimer = 0;
        }
        
        inPrePulses = true;
        pulseThresholdTimer = 0;
        
        inCluster = true;
    }
    
    if (inCluster)
    {
        //save note in the cluster, even if it's already there. then cap the cluster to clusterMax
        //this is different than avoiding dupes at this stage (with "addIfNotAlreadyThere") because it allows
        //repeated notes to push older notes out the back.
        //later, we remove dupes so we don't inadvertently play the same note twice in a pulse
        cluster.insert(0, noteNumber);
        if(cluster.size() > 8) cluster.resize(8); //need this cap to be user settable; set to 8 for now to reproduce original bK behavior
        
        clusterThresholdTimer = 0;
    }
}

void SynchronicProcessor::keyReleased(int noteNumber, int channel)
{
    
    //remove key from array of pressed keys
    keysDepressed.removeAllInstancesOf(noteNumber);
    
    // If LastNoteOnSync mode, reset phasor and multiplier indices.
    //only initiate pulses if ALL keys are released
    if (preparation->getMode() == LastNoteOffSync && keysDepressed.size() == 0)
    {
        phasor = pulseThresholdSamples;
        beat = 0;
        length = 0;
        accent = 0;
        transp = 0;
        pulse = 1;
        
        inPulses = true;
    }
}

void SynchronicProcessor::processBlock(int numSamples, int channel)
{
    
    tuner.incrementAdaptiveClusterTime(numSamples);
    
    //remove duplicates from cluster, so we don't play the same note twice in a single pulse
    slimCluster.clearQuick();
    for(int i = 0; i< cluster.size(); i++) slimCluster.addIfNotAlreadyThere(cluster.getUnchecked(i));
    int clusterSize = slimCluster.size();
    
    clusterThresholdSamples = (preparation->getClusterThreshSEC() * sampleRate);
    pulseThresholdSamples = (preparation->getPulseThresh() * sampleRate);
    
    if (inCluster)
    {
        if (clusterThresholdTimer >= clusterThresholdSamples)
        {
            inCluster = false;
        }
        else
        {
            clusterThresholdTimer += numSamples;
            firstNoteTimer += numSamples;
        }
    }
    
    if (inPrePulses)
    {
        if (pulseThresholdTimer >= pulseThresholdSamples)
        {
            inPrePulses = false;
            
            if (preparation->getMode() == FirstNoteOnSync)
            {
                phasor = pulseThresholdSamples;
                
                pulse = 1;
                beat = 0;
                length = 0;
                transp = 0;
                accent = 0;
            }
            
            if (preparation->getMode() != LastNoteOffSync)
            {
                inPulses = true;
            }
            
        }
        else
        {
            pulseThresholdTimer += numSamples;
        }
    }
    
    if (inPulses)
    {
        numSamplesBeat = (preparation->getBeatMultipliers()[beat] * pulseThresholdSamples);
        
        if (phasor >= numSamplesBeat)
        {
            
            phasor -= numSamplesBeat;
            
            DBG("shouldPlay: "      + String((int)shouldPlay) +
                " skipBeats: "      + String(skipBeats) +
                " beatsToSkip: "    + String(preparation->getBeatsToSkip()));
            
            if (!shouldPlay)
            {
                if (++skipBeats >= preparation->getBeatsToSkip())
                {
                    skipBeats = 0;
                    
                    beat = 0;
                    
                    if (preparation->getBeatsToSkip() > 0)
                    {
                        pulse = 0;
                    }
                    else
                    {
                        pulse = 1;
                    }
                    
                    if(keysDepressed.size() == 0) shouldPlay = true;
                    
                }
                else
                {
                    if (++beat >= preparation->getBeatMultipliers().size())         beat = 0;
                }
            }
            
            if (shouldPlay)
            {
                if (clusterSize >= preparation->getClusterMin() && clusterSize <= preparation->getClusterMax())
                {
                    for (int n = 0; n < clusterSize; n++)
                    {
                        playNote(channel,
                                 cluster[n],
                                 velocities.getUnchecked(cluster[n]) * preparation->getAccentMultipliers()[accent]);
                    }
                
                }
                
                if (++beat >= preparation->getBeatMultipliers().size())         beat = 0;
                
                if (++length >= preparation->getLengthMultipliers().size())     length = 0;
                
                if (++accent >= preparation->getAccentMultipliers().size())     accent = 0;
                
                if (++transp >= preparation->getTranspOffsets().size())         transp = 0;
                
                if (pulse++ >= preparation->getNumPulses())
                {
                    cluster.clearQuick();
                    toAdd.clearQuick();
                    inPulses = false;
                    shouldPlay = false;
                }
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
        if (++myBeat >= preparation->getBeatMultipliers().size()) myBeat = 0;
        timeToReturn += (preparation->getBeatMultipliers()[myBeat] * pulseThresholdSamples);
    }
    
    //DBG("time in ms to skipped beat = " + std::to_string(timeToReturn * 1000./sampleRate));
    return timeToReturn * 1000./sampleRate;
}



    


