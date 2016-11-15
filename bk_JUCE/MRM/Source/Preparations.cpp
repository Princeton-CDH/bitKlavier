/*
  ==============================================================================

    Preparations.cpp
    Created: 28 Oct 2016 10:42:48am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "Preparations.h"


SynchronicProcessor::SynchronicProcessor()
{    
    clusterTimer = 0;
    phasor = 0;
    
    inPulses = false;
    inCluster = false;
    
    firstNoteTimer = 0;
    
    cluster = Array<int>();
    on = Array<int>();
    
    beatMultipliers = Array<float>();
    accentMultipliers = Array<float>();
    lengthMultipliers = Array<float>();
    tuningOffsets = Array<float>();
}


SynchronicProcessor::~SynchronicProcessor()
{
}

void SynchronicProcessor::set(float t,
         int pulses,
         int cMin,
         int cMax,
         float cThreshold,
         SynchronicSyncMode mode,
         int toSkip,
         Array<float> beats,
         Array<float> lengths,
         Array<float> accents,
         Array<float> tuning,
         int basePitch)
{
    tempo = t;
    tempoPeriod = (60.0/tempo) * 1000.0;
    
    clusterThreshold = tempoPeriod * cThreshold;
    clusterThresholdSamples = (clusterThreshold * sampleRate * 0.001);
    
    pulseThreshold = tempoPeriod;
    pulseThresholdSamples = (60.0/tempo) * sampleRate;
    
    numPulses = pulses;
    clusterMin = cMin;
    clusterMax = cMax;
    
    syncMode = mode;
    pulsesToSkip = toSkip;
    beatMultipliers = beats;
    lengthMultipliers = lengths;
    accentMultipliers = accents;
    tuningOffsets = tuning;
    tuningBasePitch = basePitch;
    
}

void SynchronicProcessor::playNote(int channel, int note)
{
    PianoSamplerNoteDirection noteDirection = Forward;
    float noteStartPos = 0.0;
    float noteLength = (fabs(lengthMultipliers[length]) * tempoPeriod);
    
    if (lengthMultipliers[length] < 0)
    {
        noteDirection = Reverse;
        noteStartPos = noteLength;
    }
    
    
    synth->keyOn(
                 channel,
                 note+9,
                 accentMultipliers[accent],
                 tuningOffsets,
                 tuningBasePitch,
                 noteDirection,
                 FixedLengthFixedStart,
                 BKNoteTypeNil,
                 noteStartPos, // start
                 noteLength
                 );
}

void SynchronicProcessor::attachToSynth(BKSynthesiser *s)
{
    synth = s;
    sampleRate = s->getSampleRate();
}

void SynchronicProcessor::notePlayed(int noteNumber, int velocity)
{
    if (inCluster)
    {
        // If LastNoteSync mode, reset phasor and multiplier indices.
        if (syncMode == LastNoteSync)
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
        on.clearQuick();
        inPulses = false;
        
        // Start first note timer, since this is beginning of new cluster.
        if (syncMode == FirstNoteSync)
        {
            firstNoteTimer = 0;
        }
        
        inPrePulses = true;
        pulseThresholdTimer = 0;
        
        inCluster = true;
    }
    
    if (inCluster)
    {
        clusterThresholdTimer = 0;
        on.add(noteNumber);
    }
    
    for (auto note : cluster)
    {
        DBG("cluster: " + String(note));
    }
   
}

void SynchronicProcessor::renderNextBlock(int channel, int numSamples)
{

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
            
            if (syncMode == FirstNoteSync)
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
        numSamplesBeat = (beatMultipliers[beat] * sampleRate * (60.0/tempo));
        
        if (phasor >= numSamplesBeat)
        {
            phasor -= numSamplesBeat;
            
            if (on.size() >= clusterMin && on.size() <= clusterMax)
            {
                for (auto note : on)
                {
                    playNote(channel, note);
                }
            }
            
            if (++beat >= beatMultipliers.size())        beat = 0;
            
            if (++length >= lengthMultipliers.size())    length = 0;
            
            if (++accent >= accentMultipliers.size())    accent = 0;
            
            if (++pulse >= numPulses)
            {
                on.clearQuick();
                inPulses = false;
            }
            
            
        }
        
        phasor += numSamples;
        
    }

}

