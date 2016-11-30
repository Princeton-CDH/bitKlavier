/*
  ==============================================================================

    Synchronic.cpp
    Created: 22 Nov 2016 3:46:35pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "Synchronic.h"

SynchronicProcessor::SynchronicProcessor(BKSynthesiser *s, SynchronicPreparation::Ptr p)
:
    synth(s),
    preparation(p)
{
    sampleRate = synth->getSampleRate();
    
    clusterTimer = 0;
    phasor = 0;
    
    inPulses = false;
    inCluster = false;
    
    firstNoteTimer = 0;
    
    cluster = Array<int>();
    on = Array<int>();
    
    // Initialize keymap.
    keymap = Array<int>();
    keymap.ensureStorageAllocated(128);
    for (int i = 0; i < 128; i++)
    {
        keymap.set(i,0);
    }
}



SynchronicProcessor::~SynchronicProcessor()
{
}



void SynchronicProcessor::playNote(int channel, int note)
{
    PianoSamplerNoteDirection noteDirection = Forward;
    float noteStartPos = 0.0;
    
    //float noteLength = (fabs(preparation->getLengthMultipliers()[length]) * tempoPeriod);
    float noteLength = (fabs(preparation->getLengthMultipliers()[length]) * 50.0); //we can change to tempoPeriod multiplier, but need to think about consistency with older behavior
    
    if (preparation->getLengthMultipliers()[length] < 0)
    {
        noteDirection = Reverse;
        noteStartPos = noteLength;
    }
    
    synth->keyOn(
                 channel,
                 note,
                 preparation->getTranspOffsets()[transp],
                 preparation->getAccentMultipliers()[accent] * aGlobalGain,
                 preparation->getTuningOffsets(),
                 preparation->getBasePitch(),
                 noteDirection,
                 FixedLengthFixedStart,
                 BKNoteTypeNil,
                 noteStartPos, // start
                 noteLength,
                 3,
                 3
                 );
}


void SynchronicProcessor::keyOn(int noteNumber, int velocity)
{
    //if (keymap[noteNumber])
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
    
}

void SynchronicProcessor::processBlock(int numSamples, int channel)
{
    
    int clusterSize = cluster.size();
    
    clusterThresholdSamples = (preparation->getClusterThresh() * sampleRate);
    
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
            
            if (preparation->getMode() == FirstNoteSync)
            {
                phasor = pulseThresholdSamples;
                
                pulse = 1;
                beat = 0;
                length = 0;
                transp = 0;
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
        numSamplesBeat = (preparation->getBeatMultipliers()[beat] * pulseThresholdSamples);
        
        
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
            
            if (++beat >= preparation->getBeatMultipliers().size())         beat = 0;
            
            if (++length >= preparation->getLengthMultipliers().size())     length = 0;
            
            if (++accent >= preparation->getAccentMultipliers().size())     accent = 0;
            
            if (++transp >= preparation->getTranspOffsets().size())         transp = 0;
            
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


