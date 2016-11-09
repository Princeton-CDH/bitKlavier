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
    endCluster = false;
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
    numPulses = pulses;
    clusterMin = cMin;
    clusterMax = cMax;
    clusterThreshold = cThreshold;
    clusterThresholdSamples = (clusterThreshold * sampleRate * 0.001);
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
    float noteLength = (fabs(lengthMultipliers[length]) * (60.0/tempo) * 1000.0);
    
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
    
    if (endCluster)
    {
        endCluster = false;
        on.clearQuick();
    }
    
    if (syncMode == FirstNoteSync)
    {
        if (!cluster.size())
        {
            clusterTimer = 0;
        }
    }
    else
    {
        clusterTimer = 0;
    }
    
    cluster.add(noteNumber);
}

void SynchronicProcessor::renderNextBlock(int channel, int numSamples)
{
    
    if (!endCluster)
    {
        if (clusterTimer >= clusterThresholdSamples)
        {
            endCluster = true;
            
            if ((cluster.size() >= clusterMin) &&
                (cluster.size() <= clusterMax))
            {
                on.clearQuick();
                
                for (auto n : cluster)
                {
                    on.add(n);
                }
                //on.swapWith(cluster);
                
                inPulses = true;
            }
            
            cluster.clearQuick();
            
            pulse = 1;
            beat = 0;
            length = 0;
            accent = 0;
            
            phasor = clusterThresholdSamples;
            
        }
        else
        {
            clusterTimer += numSamples;
        }
    }
    
    
    if (inPulses)
    {
        numSamplesBeat = (beatMultipliers[beat] * sampleRate * (60.0/tempo));
        
        if (phasor >= numSamplesBeat)
        {
            phasor -= numSamplesBeat;
            
            for (auto note : on)
            {
                playNote(channel, note);
            }
            
            if (++beat >= beatMultipliers.size())        beat = 0;
            
            if (++length >= lengthMultipliers.size())    length = 0;
            
            if (++accent >= accentMultipliers.size())    accent = 0;
            
            if (++pulse >= numPulses)
            {
                on.clearQuick();
                pulse = 0;
                inPulses = false;
            }
            
        }

        phasor += numSamples;
        
    }
    
    
    
}

