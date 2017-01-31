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
    
    firstNoteTimer = 0;
    
    cluster = Array<int>();
    on = Array<int>();
    
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
                 velocity * active->getAccentMultipliers()[accent],
                 aGlobalGain,
                 noteDirection,
                 FixedLengthFixedStart,
                 Synchronic,
                 Id,
                 noteStartPos, // start
                 noteLength,
                 3,
                 3);
}

void SynchronicProcessor::keyPressed(int noteNumber, float velocity)
{
    tuner.setPreparation(active->getTuning());
    
    velocities.set(noteNumber, velocity);
    
    if (inCluster)
    {
        // If LastNoteOnSync mode, reset phasor and multiplier indices.
        if (active->getMode() == LastNoteOnSync)
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
        if (active->getMode() == FirstNoteOnSync)
        {
            firstNoteTimer = 0;
        }
        
        inPrePulses = true;
        pulseThresholdTimer = 0;
        
        inCluster = true;
    }
    
    if (inCluster)
    {
        cluster.add(noteNumber);
        clusterThresholdTimer = 0;
    }
}

void SynchronicProcessor::keyReleased(int noteNumber, int channel)
{
    // If LastNoteOnSync mode, reset phasor and multiplier indices.
    if (!inPulses && active->getMode() == LastNoteOffSync)
    {
        phasor = pulseThresholdSamples;
        beat = 0;
        length = 0;
        accent = 0;
        transp = 0;
        pulse = 1;
        
        /*
        for (int n = 0; n < cluster.size(); n++)
        {
            playNote(channel, cluster[n]);
        }
         */
        
        inPulses = true;
    }
}

void SynchronicProcessor::processBlock(int numSamples, int channel)
{
    
    tuner.incrementAdaptiveClusterTime(numSamples);
    
    int clusterSize = cluster.size();
    
    clusterThresholdSamples = (active->getClusterThresh() * sampleRate);
    
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
            firstNoteTimer += numSamples;
        }
    }
    
    if (inPrePulses)
    {
        if (pulseThresholdTimer >= pulseThresholdSamples)
        {
            inPrePulses = false;
            
            if (active->getMode() == FirstNoteOnSync)
            {
                phasor = pulseThresholdSamples;
                
                pulse = 1;
                beat = 0;
                length = 0;
                transp = 0;
                accent = 0;
            }
            
            if (active->getMode() != LastNoteOffSync)
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
        numSamplesBeat = (active->getBeatMultipliers()[beat] * pulseThresholdSamples);
        
        if (phasor >= numSamplesBeat)
        {
            phasor -= numSamplesBeat;
            
            if (!shouldPlay)
            {
                if (++skipBeats >= active->getBeatsToSkip())
                {
                    skipBeats = 0;
                    
                    beat = 0;
                    
                    if (active->getBeatsToSkip() > 0)   pulse = 0;
                    else                                pulse = 1;
                    
                    shouldPlay = true;
                    
                }
                else
                {
                    if (++beat >= active->getBeatMultipliers().size())         beat = 0;
                }
            }
            
            if (shouldPlay)
            {
                if (clusterSize >= active->getClusterMin() && clusterSize <= active->getClusterMax())
                {
                    for (int n = 0; n < clusterSize; n++)
                        playNote(channel, cluster[n], velocities.getUnchecked(cluster[n]));
                
                }
                
                if (++beat >= active->getBeatMultipliers().size())         beat = 0;
                
                if (++length >= active->getLengthMultipliers().size())     length = 0;
                
                if (++accent >= active->getAccentMultipliers().size())     accent = 0;
                
                if (++transp >= active->getTranspOffsets().size())         transp = 0;
                
                if (++pulse >= active->getNumPulses())
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
        if (++myBeat >= active->getBeatMultipliers().size()) myBeat = 0;
        timeToReturn += (active->getBeatMultipliers()[myBeat] * pulseThresholdSamples);
    }
    
    //DBG("time in ms to skipped beat = " + std::to_string(timeToReturn * 1000./sampleRate));
    return timeToReturn * 1000./sampleRate;
}



    


