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
    auto peakCoefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, peakFreq, peakQuality,
                                                                                juce::Decibels::decibelsToGain(peakGain));
    
    *chain.get<ChainPositions::Peak>().coefficients = *peakCoefficients;
}

void BKEqualizer::process(juce::dsp::ProcessContextReplacing<float>& context) {
    chain.process(context);
}
