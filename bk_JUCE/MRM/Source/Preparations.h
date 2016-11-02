/*
  ==============================================================================

    Preparations.h
    Created: 28 Oct 2016 10:42:48am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef PREPARATIONS_H_INCLUDED
#define PREPARATIONS_H_INCLUDED

#include "AudioConstants.h"

#if 0
class SynchronicPreparation
{
    
public:
    SynchronicPreparation();
    ~SynchronicPreparation();
    
    SynchronicSyncMode syncMode;
    //OtherMode otherMode; //for tethered to keyboard/metro or not
    
    int clusterMin;
    int clusterMax;
    int pulsesToSkip;
    int numPulses;
    
    float clusterThreshold; // ms
    float tempo; // BPM
    
    Array<float> beatMultipliers;
    Array<float> lengthMultipliers;
    Array<float> accentMultipliers;
    
    int pulse;
    int beat;
    int accent;
    int length;
    int clusterSize;
    
    JUCE_LEAK_DETECTOR(SynchronicPreparation);
};
#endif

class SynchronicProcessor
{
    
public:
    SynchronicProcessor(double sampleRate);
    ~SynchronicProcessor();
    
    SynchronicSyncMode syncMode;
    
    int clusterMin;
    int clusterMax;
    int pulsesToSkip;
    int numPulses;
    
    float clusterThreshold; // ms
    float tempo; // BPM
    
    Array<float> beatMultipliers;
    Array<float> lengthMultipliers;
    Array<float> accentMultipliers;
    
    int pulse;
    int beat;
    int accent;
    int length;
    int clusterSize;
    
    bool tick(int channel, int numSamples);
    
    void notePlayed(int noteNumber, int velocity);
    
    
private:
    double sampleRate;
    
    Array<float> tuningOffsets;
    int tuningBasePitch;
    
    uint64 clusterTimer;
    uint64 pulseTimer;
    uint64 phasor;
    
    Array<int> cluster;
    Array<int> inCluster;
    
    Array<int> on;
    Array<int> inOn;
    
    JUCE_LEAK_DETECTOR(SynchronicProcessor);
    
};


#endif  // PREPARATIONS_H_INCLUDED
