/*
  ==============================================================================

    Synchronic.cpp
    Created: 22 Nov 2016 3:46:35pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "Synchronic.h"

SynchronicProcessor::SynchronicProcessor(BKSynthesiser *s, SynchronicPreparation::Ptr prep)
:
    synth(s),
    preparation(prep)
{
    sampleRate = synth->getSampleRate();
    
    clusterTimer = 0;
    phasor = 0;
    
    inPulses = false;
    inCluster = false;
    
    firstNoteTimer = 0;
    
    cluster = Array<int>();
    on = Array<int>();
    
    tempoPeriod = (60.0/preparation->getTempo()) * 1000.0;
    
    clusterThreshold = tempoPeriod * preparation->getClusterThresh();
    clusterThresholdSamples = (clusterThreshold * sampleRate * 0.001);
    
    pulseThreshold = tempoPeriod;
    pulseThresholdSamples = (60.0/preparation->getTempo()) * sampleRate;
}

SynchronicProcessor::~SynchronicProcessor()
{
}

void SynchronicProcessor::playNote(int channel, int note)
{
    PianoSamplerNoteDirection noteDirection = Forward;
    float noteStartPos = 0.0;
    float noteLength = (fabs(preparation->getLengthMultipliers()[length]) * tempoPeriod);
    
    if (preparation->getLengthMultipliers()[length] < 0)
    {
        noteDirection = Reverse;
        noteStartPos = noteLength;
    }
    
    
    synth->keyOn(
                 channel,
                 note + 9,
                 preparation->getAccentMultipliers()[accent] * .5,
                 preparation->getTuningOffsets(),
                 preparation->getBasePitch(),
                 noteDirection,
                 FixedLengthFixedStart,
                 BKNoteTypeNil,
                 noteStartPos, // start
                 noteLength,
                 0,
                 30
                 );
}

void SynchronicProcessor::notePlayed(int noteNumber, int velocity)
{
    if (inCluster)
    {
        // If LastNoteSync mode, reset phasor and multiplier indices.
        if (preparation->getMode() == LastNoteSync)
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
            
            inPrePulses = true;
            pulseThresholdTimer = 0;
        }
        
        
    }
    else
    {
        cluster.clearQuick();
        inPulses = false;
        
        // Start first note timer, since this is beginning of new cluster.
        if (preparation->getMode() == FirstNoteSync)
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

void SynchronicProcessor::renderNextBlock(int channel, int numSamples)
{
    
    int clusterSize = cluster.size();
    
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
            
            if (preparation->getMode() == FirstNoteSync)
            {
                phasor = pulseThresholdSamples;
                
                pulse = 1;
                beat = 0;
                length = 0;
                accent = 0;
            }
            
            inPulses = true;
            
        }
        else
        {
            pulseThresholdTimer += numSamples;
        }
    }
    
    if (inPulses)
    {
        numSamplesBeat = (preparation->getBeatMultipliers()[beat] * sampleRate * (60.0/preparation->getTempo()));
        
        
        if (phasor >= numSamplesBeat)
        {
            phasor -= numSamplesBeat;
            
            if (clusterSize >= preparation->getClusterMin() && clusterSize <= preparation->getClusterMax())
            {
                for (int n = 0; n < clusterSize; n++)
                {
                    playNote(channel, cluster[n]);
                }
            }
            
            if (++beat >= preparation->getBeatMultipliers().size())        beat = 0;
            
            if (++length >= preparation->getLengthMultipliers().size())    length = 0;
            
            if (++accent >= preparation->getAccentMultipliers().size())    accent = 0;
            
            if (++pulse >= preparation->getNumPulses())
            {
                cluster.clearQuick();
                toAdd.clearQuick();
                inPulses = false;
            }
            
            
        }
        
        phasor += numSamples;
        
    }
    
}
