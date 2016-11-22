/*
  ==============================================================================

    Processors.h
    Created: 20 Nov 2016 5:48:43pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef PROCESSORS_H_INCLUDED
#define PROCESSORS_H_INCLUDED

#include "BKSynthesiser.h"

#include "Preparations.h"

class SynchronicProcessor
{
    
public:
    SynchronicProcessor(BKSynthesiser *s, SynchronicPreparation::Ptr prep);
    ~SynchronicProcessor();
    
    float clusterThreshold; // beats
    uint64 clusterThresholdSamples;
    uint64 clusterThresholdTimer;
    
    float pulseThreshold; //beats
    uint64 pulseThresholdSamples;
    uint64 pulseThresholdTimer;
    
    int pulse;
    int beat;
    int accent;
    int length;
    int clusterSize;
    
    void renderNextBlock(int channel, int numSamples);
    
    void notePlayed(int noteNumber, int velocity);
    
private:
    
    BKSynthesiser *synth;
    SynchronicPreparation::Ptr preparation;
    
    void playNote(int channel, int note);
    
    double tempoPeriod;
    uint64 tempoPeriodSamples;
    
    bool inPulses, inCluster, inPrePulses;
    
    double sampleRate;
    
    
    
    uint64 firstNoteTimer;
    uint64 tempoPeriodTimer;
    uint64 lastNoteTimer;
    
    Array<float> tuningOffsets;
    int tuningBasePitch;
    
    uint64 clusterTimer;
    uint64 pulseTimer;
    uint64 phasor;
    uint64 numSamplesBeat;
    
    Array<int> cluster;
    Array<int> toAdd;
    Array<int> on;
    
    JUCE_LEAK_DETECTOR(SynchronicProcessor);
    
};


#endif  // PROCESSORS_H_INCLUDED
