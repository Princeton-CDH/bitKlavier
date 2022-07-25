/*
  ==============================================================================

    CompressorProcessor.h
    Created: 20 Jul 2022 10:38:53am
    Author:  Davis Polito

  ==============================================================================
*/
#include "compressor-dsp/include/Compressor.h"
#include "compressor-dsp/include/LevelEnvelopeFollower.h"
#pragma once
class CompressorProcessor
{
public:
    CompressorProcessor();
    ~CompressorProcessor(){};
    //==============================================================================
    Atomic<float> gainReduction;
    Atomic<float> currentInput;
    Atomic<float> currentOutput;
    void prepareToPlay(double sampleRate, int samplesPerBlock);
    void update();
    void process(AudioSampleBuffer& buffer);
    Compressor compressor;
private:
    
    LevelEnvelopeFollower inLevelFollower;
    LevelEnvelopeFollower outLevelFollower;
};
