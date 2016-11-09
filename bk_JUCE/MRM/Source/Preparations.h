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

#include "BKSynthesiser.h"


class SynchronicProcessor
{
    
public:
    SynchronicProcessor();
    ~SynchronicProcessor();
    
    SynchronicSyncMode syncMode;
    
    int clusterMin;
    int clusterMax;
    int pulsesToSkip;
    int numPulses;
    
    float clusterThreshold; // ms
    double clusterThresholdSamples;
    float tempo; // BPM
    
    Array<float> beatMultipliers;
    Array<float> lengthMultipliers;
    Array<float> accentMultipliers;
    
    int pulse;
    int beat;
    int accent;
    int length;
    int clusterSize;
    
    void set(float tempo, int numPulses, int clusterMin, int clusterMax, float clusterThreshold, SynchronicSyncMode mode, int pulsesToSkip, Array<float> beats, Array<float> lengths, Array<float> accents, Array<float> tuning, int basePitch);
    
    void attachToSynth(BKSynthesiser *synth);
    
    bool tick(int channel, int numSamples);
    
    void notePlayed(int noteNumber, int velocity);
    
    void setTuningOffsets(Array<float> newOffsets);
    void setTuningBasePitch(int basePitch);
    
    
private:
    void playNote(int channel, int note);
    
    bool inPulses, tickCT;
    BKSynthesiser *synth;
    double sampleRate;
    
    bool endCluster;
    
    uint64 firstNoteTimer;
    uint64 lastNoteTimer;
    
    Array<float> tuningOffsets;
    int tuningBasePitch;
    
    uint64 clusterTimer;
    uint64 pulseTimer;
    uint64 phasor;
    uint64 numSamplesBeat;
    
    Array<int> cluster;
    Array<int> inCluster;
    
    Array<int> on;
    Array<int> inOn;
    
    JUCE_LEAK_DETECTOR(SynchronicProcessor);
    
};


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


#endif  // PREPARATIONS_H_INCLUDED
