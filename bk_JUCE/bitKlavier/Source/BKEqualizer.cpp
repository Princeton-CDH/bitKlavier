/*
  ==============================================================================

    BKEqualizer.cpp
    Created: 26 Jun 2021 4:47:49pm
    Author:  Jeffrey Gordon

  ==============================================================================
*/

#include <JuceHeader.h>
#include "BKEqualizer.h"

//==============================================================================
BKEqualizer::BKEqualizer()
{
    restoreDefaultState();
}

BKEqualizer::~BKEqualizer()
{
}

void BKEqualizer::prepareToPlay(int samplesPerBlock) {
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 1;
    spec.sampleRate = sampleRate;
    
    leftChain.prepare(spec);
    rightChain.prepare(spec);
    
    updateCoefficients();
}

void BKEqualizer::updateCoefficients() {
    
    paramsChanged = false;
    
    // calculate and set peak filters' coefficients
    auto peak1Coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, peak1Freq, peak1Quality, juce::Decibels::decibelsToGain(peak1Gain));
    auto peak2Coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, peak2Freq, peak2Quality, juce::Decibels::decibelsToGain(peak2Gain));
    auto peak3Coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, peak3Freq, peak3Quality, juce::Decibels::decibelsToGain(peak3Gain));
    
    *leftChain.get<ChainPositions::Peak1>().coefficients = *peak1Coefficients;
    *leftChain.get<ChainPositions::Peak2>().coefficients = *peak2Coefficients;
    *leftChain.get<ChainPositions::Peak3>().coefficients = *peak3Coefficients;
    *rightChain.get<ChainPositions::Peak1>().coefficients = *peak1Coefficients;
    *rightChain.get<ChainPositions::Peak2>().coefficients = *peak2Coefficients;
    *rightChain.get<ChainPositions::Peak3>().coefficients = *peak3Coefficients;
    
    leftChain.setBypassed<ChainPositions::Peak1>(peak1Bypassed || bypassed);
    leftChain.setBypassed<ChainPositions::Peak2>(peak2Bypassed || bypassed);
    leftChain.setBypassed<ChainPositions::Peak3>(peak3Bypassed || bypassed);
    rightChain.setBypassed<ChainPositions::Peak1>(peak1Bypassed || bypassed);
    rightChain.setBypassed<ChainPositions::Peak2>(peak2Bypassed || bypassed);
    rightChain.setBypassed<ChainPositions::Peak3>(peak3Bypassed || bypassed);
    
    // calculate and set low cut filter coefficients
    jassert(lowCutSlope % 12 == 0 && lowCutSlope >= 12 && lowCutSlope <= 48);
    auto lowCutCoefficients = juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(lowCutFreq, sampleRate, lowCutSlope / 6);

    auto& leftLowCutChain = leftChain.get<ChainPositions::LowCut>();
    leftLowCutChain.setBypassed<Slope::S12>(true);
    leftLowCutChain.setBypassed<Slope::S24>(true);
    leftLowCutChain.setBypassed<Slope::S36>(true);
    leftLowCutChain.setBypassed<Slope::S48>(true);
    auto& rightLowCutChain = rightChain.get<ChainPositions::LowCut>();
    rightLowCutChain.setBypassed<Slope::S12>(true);
    rightLowCutChain.setBypassed<Slope::S24>(true);
    rightLowCutChain.setBypassed<Slope::S36>(true);
    rightLowCutChain.setBypassed<Slope::S48>(true);
    
    // breaks omitted on purpose to facilitate fall through
    switch(lowCutSlope) {
        case 48: {
            *leftLowCutChain.get<Slope::S48>().coefficients =
            *lowCutCoefficients[Slope::S48];
            leftLowCutChain.setBypassed<Slope::S48>(lowCutBypassed || bypassed);
            *rightLowCutChain.get<Slope::S48>().coefficients =
            *lowCutCoefficients[Slope::S48];
            rightLowCutChain.setBypassed<Slope::S48>(lowCutBypassed || bypassed);
        }
        case 36: {
            *leftLowCutChain.get<Slope::S36>().coefficients =
            *lowCutCoefficients[Slope::S36];
            leftLowCutChain.setBypassed<Slope::S36>(lowCutBypassed || bypassed);
            *rightLowCutChain.get<Slope::S36>().coefficients =
            *lowCutCoefficients[Slope::S36];
            rightLowCutChain.setBypassed<Slope::S36>(lowCutBypassed || bypassed);
        }
        case 24: {
            *leftLowCutChain.get<Slope::S24>().coefficients =
            *lowCutCoefficients[Slope::S24];
            leftLowCutChain.setBypassed<Slope::S24>(lowCutBypassed || bypassed);
            *rightLowCutChain.get<Slope::S24>().coefficients =
            *lowCutCoefficients[Slope::S24];
            rightLowCutChain.setBypassed<Slope::S24>(lowCutBypassed || bypassed);
        }
        case 12: {
            *leftLowCutChain.get<Slope::S12>().coefficients =
            *lowCutCoefficients[Slope::S12];
            leftLowCutChain.setBypassed<Slope::S12>(lowCutBypassed || bypassed);
            *rightLowCutChain.get<Slope::S12>().coefficients =
            *lowCutCoefficients[Slope::S12];
            rightLowCutChain.setBypassed<Slope::S12>(lowCutBypassed || bypassed);
        }
    }
    
    // calculate and set high cut filter coefficients
    jassert(highCutSlope % 12 == 0 && highCutSlope >= 12 && highCutSlope <= 48);
    auto highCutCoefficients = juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(highCutFreq, sampleRate, highCutSlope / 6);
    
    auto& leftHighCutChain = leftChain.get<ChainPositions::HighCut>();
    leftHighCutChain.setBypassed<Slope::S12>(true);
    leftHighCutChain.setBypassed<Slope::S24>(true);
    leftHighCutChain.setBypassed<Slope::S36>(true);
    leftHighCutChain.setBypassed<Slope::S48>(true);
    auto& rightHighCutChain = rightChain.get<ChainPositions::HighCut>();
    rightHighCutChain.setBypassed<Slope::S12>(true);
    rightHighCutChain.setBypassed<Slope::S24>(true);
    rightHighCutChain.setBypassed<Slope::S36>(true);
    rightHighCutChain.setBypassed<Slope::S48>(true);
    
    // breaks omitted on purpose to facilitate fall through
    switch(highCutSlope) {
        case 48: {
            *leftHighCutChain.get<Slope::S48>().coefficients = *highCutCoefficients[Slope::S48];
            leftHighCutChain.setBypassed<Slope::S48>(highCutBypassed || bypassed);
            *rightHighCutChain.get<Slope::S48>().coefficients = *highCutCoefficients[Slope::S48];
            rightHighCutChain.setBypassed<Slope::S48>(highCutBypassed || bypassed);
        }
        case 36: {
            *leftHighCutChain.get<Slope::S36>().coefficients = *highCutCoefficients[Slope::S36];
            leftHighCutChain.setBypassed<Slope::S36>(highCutBypassed || bypassed);
            *rightHighCutChain.get<Slope::S36>().coefficients = *highCutCoefficients[Slope::S36];
            rightHighCutChain.setBypassed<Slope::S36>(highCutBypassed || bypassed);
        }
        case 24: {
            *leftHighCutChain.get<Slope::S24>().coefficients = *highCutCoefficients[Slope::S24];
            leftHighCutChain.setBypassed<Slope::S24>(highCutBypassed || bypassed);
            *rightHighCutChain.get<Slope::S24>().coefficients = *highCutCoefficients[Slope::S24];
            rightHighCutChain.setBypassed<Slope::S24>(highCutBypassed || bypassed);
        }
        case 12: {
            *leftHighCutChain.get<Slope::S12>().coefficients = *highCutCoefficients[Slope::S12];
            leftHighCutChain.setBypassed<Slope::S12>(highCutBypassed || bypassed);
            *rightHighCutChain.get<Slope::S12>().coefficients = *highCutCoefficients[Slope::S12];
            rightHighCutChain.setBypassed<Slope::S12>(highCutBypassed || bypassed);
        }
    }
}

void BKEqualizer::process(AudioSampleBuffer& buffer)
{
    if (paramsChanged) updateCoefficients();
    
    juce::dsp::AudioBlock<float> block(buffer);
    auto leftBlock = block.getSingleChannelBlock(0);
    juce::dsp::ProcessContextReplacing<float> leftContext(leftBlock);
    leftChain.process(leftContext);
    if (buffer.getNumChannels() > 1)
    {
        auto rightBlock = block.getSingleChannelBlock(1);
        juce::dsp::ProcessContextReplacing<float> rightContext(rightBlock);
        rightChain.process(rightContext);
    }
}

double BKEqualizer::magForFreq(double freq) {
    double mag = 1.f;
    
    // Since leftChain and rightChain use the same coefficients, it's fine to just get them from left
    if (!leftChain.isBypassed<ChainPositions::Peak1>())
        mag *= leftChain.get<ChainPositions::Peak1>()
        .coefficients->getMagnitudeForFrequency(freq, sampleRate);
    if (!leftChain.isBypassed<ChainPositions::Peak2>())
        mag *= leftChain.get<ChainPositions::Peak2>()
        .coefficients->getMagnitudeForFrequency(freq, sampleRate);
    if (!leftChain.isBypassed<ChainPositions::Peak3>())
        mag *= leftChain.get<ChainPositions::Peak3>()
        .coefficients->getMagnitudeForFrequency(freq, sampleRate);
    
    if(!leftChain.get<ChainPositions::LowCut>().isBypassed<0>())
        mag *= leftChain.get<ChainPositions::LowCut>().get<0>()
        .coefficients->getMagnitudeForFrequency(freq, sampleRate);
    if(!leftChain.get<ChainPositions::LowCut>().isBypassed<1>())
        mag *= leftChain.get<ChainPositions::LowCut>().get<1>()
        .coefficients->getMagnitudeForFrequency(freq, sampleRate);
    if(!leftChain.get<ChainPositions::LowCut>().isBypassed<2>())
        mag *= leftChain.get<ChainPositions::LowCut>().get<2>()
        .coefficients->getMagnitudeForFrequency(freq, sampleRate);
    if(!leftChain.get<ChainPositions::LowCut>().isBypassed<3>())
        mag *= leftChain.get<ChainPositions::LowCut>().get<3>()
        .coefficients->getMagnitudeForFrequency(freq, sampleRate);
    
    if(!leftChain.get<ChainPositions::HighCut>().isBypassed<0>())
        mag *= leftChain.get<ChainPositions::HighCut>().get<0>()
        .coefficients->getMagnitudeForFrequency(freq, sampleRate);
    if(!leftChain.get<ChainPositions::HighCut>().isBypassed<1>())
        mag *= leftChain.get<ChainPositions::HighCut>().get<1>()
        .coefficients->getMagnitudeForFrequency(freq, sampleRate);
    if(!leftChain.get<ChainPositions::HighCut>().isBypassed<2>())
        mag *= leftChain.get<ChainPositions::HighCut>().get<2>()
        .coefficients->getMagnitudeForFrequency(freq, sampleRate);
    if(!leftChain.get<ChainPositions::HighCut>().isBypassed<3>())
        mag *= leftChain.get<ChainPositions::HighCut>().get<3>()
        .coefficients->getMagnitudeForFrequency(freq, sampleRate);
    
    return mag;
}

//BKEqualizer& BKEqualizer::operator=(const BKEqualizer& other) {
//    // Guard self reference
//    if (this == &other) return *this;
//    
//    other.setSampleRate();
//}
