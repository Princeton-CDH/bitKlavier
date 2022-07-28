/*
  ==============================================================================

    CompressorProcessor.cpp
    Created: 20 Jul 2022 10:38:53am
    Author:  Davis Polito

  ==============================================================================
*/

#include "CompressorProcessor.h"
CompressorProcessor::CompressorProcessor(){
    gainReduction.set(0.0f);
    currentInput.set(-std::numeric_limits<float>::infinity());
    currentOutput.set(-std::numeric_limits<float>::infinity());
}
void CompressorProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    //Prepare dsp classes
    compressor.prepare({sampleRate, static_cast<uint32>(samplesPerBlock), 2});
    inLevelFollower.prepare(sampleRate);
    outLevelFollower.prepare(sampleRate);
    inLevelFollower.setPeakDecay(0.3f);
    outLevelFollower.setPeakDecay(0.3f);
//    //Notify host about latency
//    if (*parameters.getRawParameterValue("lookahead") > 0.5f)
//        setLatencySamples(static_cast<int>(0.005 * sampleRate));
//    else
//        setLatencySamples(0);
}
void CompressorProcessor::process(AudioBuffer<float>& buffer)
{
    ScopedNoDenormals noDenormals;
    //const auto totalNumInputChannels = getTotalNumInputChannels();
    //const auto totalNumOutputChannels = getTotalNumOutputChannels();
    const auto numSamples = buffer.getNumSamples();

    // Clear buffer
        for (auto i = 0; i < buffer.getNumChannels(); ++i)
            buffer.clear(i, 0, buffer.getNumSamples());
    //Update input peak metering
    inLevelFollower.updatePeak(buffer.getArrayOfReadPointers(), buffer.getNumChannels(), numSamples);
    currentInput.set(Decibels::gainToDecibels(inLevelFollower.getPeak()));

    // Do compressor processing
    compressor.process(buffer);

    // Update gain reduction metering
    gainReduction.set(compressor.getMaxGainReduction());

    // Update output peak metering
    outLevelFollower.updatePeak(buffer.getArrayOfReadPointers(), buffer.getNumChannels(), numSamples);
    currentOutput = Decibels::gainToDecibels(outLevelFollower.getPeak());
}
