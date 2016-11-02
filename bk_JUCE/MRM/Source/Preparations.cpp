/*
  ==============================================================================

    Preparations.cpp
    Created: 28 Oct 2016 10:42:48am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "Preparations.h"

#include "BKSynthesiser.h"

extern BKSynthesiser mainPianoSynth;

SynchronicProcessor::SynchronicProcessor(double sr)
{
    sampleRate = sr;
    cluster = Array<int>();
    on = Array<int>();
    
    inCluster = Array<int>();
    inCluster.ensureStorageAllocated(88);
    
    inOn = Array<int>();
    inOn.ensureStorageAllocated(88);
}

void SynchronicProcessor::notePlayed(int noteNumber, int velocity)
{
    
    cluster.add(noteNumber);
    inCluster.set(noteNumber,1);
    
    if (clusterTimer >= (clusterThreshold * sampleRate * 0.001))
    {
        if (syncMode == FirstNoteSync)
        {
            phasor = 0;
        }
        
        // Move all notes in cluster to on if
        if ((cluster.size() >= clusterMin) &&
            (cluster.size() <= clusterMax))
        {
            on.swapWith(cluster);
            inOn.swapWith(inCluster);
        }
        
        cluster.clearQuick();
    }
    
    if (syncMode == LastNoteSync)
    {
        phasor = 0;
    }
    
    clusterTimer = 0;

}


bool SynchronicProcessor::tick(int channel, int numSamples)
{
    if (clusterTimer < clusterThreshold)
    {
        clusterTimer += numSamples;
    }
    
    phasor  += numSamples;
    
    if (pulse >= numPulses)
    {
        on.clearQuick();
        pulseTimer = 0;
    }

    if (phasor >= (beatMultipliers[beat] * sampleRate * (60.0/tempo)))
    {
        for (int i = 0; i < on.size(); i++)
        {
            PianoSamplerNoteDirection noteDirection = Forward;
            float noteStartPos = 0.0;
            float noteLength = (fabs(lengthMultipliers[length]) * (60.0/tempo) * 1000.0);
            
            if (lengthMultipliers[length] < 0)
            {
                noteDirection = Reverse;
                noteStartPos = noteLength;
            }
            
            mainPianoSynth.keyOn(
                                 channel,
                                 on[i]+9,
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
        
        if (beat++ >= beatMultipliers.size())        beat = 0;
        
        if (length++ >= lengthMultipliers.size())    length = 0;
        
        if (accent++ >= accentMultipliers.size())    accent = 0;
        
        pulse++;
        
        phasor = 0;
        
        
    }
    
    return false;

}

