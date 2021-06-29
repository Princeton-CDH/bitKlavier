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
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

}

BKEqualizer::~BKEqualizer()
{
}

void BKEqualizer::prepareToPlay(double sampleRate, int samplesPerBlock) {
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 1;
    spec.sampleRate = sampleRate;
    
    chain.prepare(spec);
    
    updateCoefficients(sampleRate);
}

void BKEqualizer::updateCoefficients(double sampleRate) {
    // calculate peak filters' coefficients
    auto peak1Coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, peak1Freq, peak1Quality,
                                                                                juce::Decibels::decibelsToGain(peak1Gain));
    auto peak2Coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, peak2Freq, peak2Quality,
                                                                                juce::Decibels::decibelsToGain(peak2Gain));
    auto peak3Coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, peak3Freq, peak3Quality,
                                                                                juce::Decibels::decibelsToGain(peak3Gain));
    
    *chain.get<ChainPositions::Peak1>().coefficients = *peak1Coefficients;
    *chain.get<ChainPositions::Peak2>().coefficients = *peak2Coefficients;
    *chain.get<ChainPositions::Peak3>().coefficients = *peak3Coefficients;
    
    // calculate low cut filter coefficients
    jassert(lowCutSlope % 12 == 0 && lowCutSlope >= 12 && lowCutSlope <= 48);
    auto lowCutCoefficients = juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(lowCutFreq, sampleRate, lowCutSlope / 6);
    
    auto& lowCutChain = chain.get<ChainPositions::LowCut>();
    lowCutChain.setBypassed<Slope::S12>(true);
    lowCutChain.setBypassed<Slope::S24>(true);
    lowCutChain.setBypassed<Slope::S36>(true);
    lowCutChain.setBypassed<Slope::S48>(true);
    
    // breaks omitted on purpose to facilitate fall through
    switch(lowCutSlope) {
        case 48: {
            *lowCutChain.get<Slope::S48>().coefficients = *lowCutCoefficients[Slope::S48];
            lowCutChain.setBypassed<Slope::S48>(false);
        }
        case 36: {
            *lowCutChain.get<Slope::S36>().coefficients = *lowCutCoefficients[Slope::S36];
            lowCutChain.setBypassed<Slope::S36>(false);
        }
        case 24: {
            *lowCutChain.get<Slope::S24>().coefficients = *lowCutCoefficients[Slope::S24];
            lowCutChain.setBypassed<Slope::S24>(false);
        }
        case 12: {
            *lowCutChain.get<Slope::S12>().coefficients = *lowCutCoefficients[Slope::S12];
            lowCutChain.setBypassed<Slope::S12>(false);
        }
    }
    
    // calculate high cut filter coefficients
    jassert(highCutSlope % 12 == 0 && highCutSlope >= 12 && highCutSlope <= 48);
    auto highCutCoefficients = juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(highCutFreq, sampleRate, highCutSlope / 6);
    
    auto& highCutChain = chain.get<ChainPositions::HighCut>();
    highCutChain.setBypassed<Slope::S12>(true);
    highCutChain.setBypassed<Slope::S24>(true);
    highCutChain.setBypassed<Slope::S36>(true);
    highCutChain.setBypassed<Slope::S48>(true);
    
    // breaks omitted on purpose to facilitate fall through
    switch(highCutSlope) {
        case 48: {
            *highCutChain.get<Slope::S48>().coefficients = *highCutCoefficients[Slope::S48];
            highCutChain.setBypassed<Slope::S48>(false);
        }
        case 36: {
            *highCutChain.get<Slope::S36>().coefficients = *highCutCoefficients[Slope::S36];
            highCutChain.setBypassed<Slope::S36>(false);
        }
        case 24: {
            *highCutChain.get<Slope::S24>().coefficients = *highCutCoefficients[Slope::S24];
            highCutChain.setBypassed<Slope::S24>(false);
        }
        case 12: {
            *highCutChain.get<Slope::S12>().coefficients = *highCutCoefficients[Slope::S12];
            highCutChain.setBypassed<Slope::S12>(false);
        }
    }
}

void BKEqualizer::process(juce::dsp::ProcessContextReplacing<float>& context) {
    chain.process(context);
}
