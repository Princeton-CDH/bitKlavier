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
    
    inCluster = Array<int>();
    inCluster.ensureStorageAllocated(88);
    
    inOn = Array<int>();
    inOn.ensureStorageAllocated(88);
    
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

void SynchronicProcessor::attachToSynth(BKSynthesiser *s)
{
    synth = s;
    sampleRate = s->getSampleRate();
}

void SynchronicProcessor::notePlayed(int noteNumber, int velocity)
{
    on.clearQuick();
    
    if (!cluster.size())
    {
        firstNoteTimer = 0;
    }
    
    cluster.add(noteNumber);
    
    DBG("CLUSTER: ");
    for (auto n : cluster)
    {
        DBG(String(n));
    }
    
    
    endCluster = false;
    lastNoteTimer = 0;
    clusterTimer = 0;
    
}

void SynchronicProcessor::setTuningOffsets(Array<float> newOffsets)
{
    tuningOffsets.swapWith(newOffsets);
}

void SynchronicProcessor::setTuningBasePitch(int basePitch)
{
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

bool SynchronicProcessor::tick(int channel, int numSamples)
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
                inPulses = true;
            }
            
            cluster.clearQuick();
            
            pulse = 1;
            
            beat = 0;
            length = 0;
            accent = 0;
            
            if (syncMode == FirstNoteSync)
            {
                phasor = firstNoteTimer;
                numSamplesBeat = (beatMultipliers[beat] * sampleRate * (60.0/tempo));
                if (phasor > numSamplesBeat)
                {
                    phasor -= numSamplesBeat;
                }
            }
            else if (syncMode == LastNoteSync)
            {
                phasor = lastNoteTimer;
                
            }
        }
        else
        {
            clusterTimer += numSamples;
            firstNoteTimer += numSamples;
            lastNoteTimer += numSamples;
        }
    }
    
    if (pulse >= numPulses)
    {
        on.clearQuick();
        pulse = 0;
        inPulses = false;
    }
    
    if (inPulses)
    {
        /*
        if (tempo == 120.0)
        {
            DBG("ONE: " + String(phasor));
        }
        else
        {
            DBG("TWO: " + String(phasor));
        }
        */
        numSamplesBeat = (beatMultipliers[beat] * sampleRate * (60.0/tempo));
        
        if (phasor >= numSamplesBeat)
        {
            phasor -= numSamplesBeat;
            
            pulse += 1;
            
            for (auto note : on)
            {
                playNote(channel, note);
                
            }
            
            /*
            if (tempo == 120.0)
            {
                DBG("ONE: " + String(pulse));
            }
            else
            {
                DBG("TWO: " + String(pulse));
            }
             */
            
            
            if (++beat >= beatMultipliers.size())        beat = 0;
            
            if (++length >= lengthMultipliers.size())    length = 0;
            
            if (++accent >= accentMultipliers.size())    accent = 0;
            
        }

        phasor += numSamples;
        
    }
    
   
    
    
    
    return false;

}

