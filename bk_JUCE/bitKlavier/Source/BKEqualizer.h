/*
  ==============================================================================

    BKEqualizer.h
    Created: 26 Jun 2021 4:47:49pm
    Author:  Jeffrey Gordon

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class BKEqualizer  : public juce::Component
{
public:
    BKEqualizer();
    ~BKEqualizer() override;
    
    void prepareToPlay(double sampleRate, int samplesPerBlock);
    void updateCoefficients(double sampleRate);
    void process(juce::dsp::ProcessContextReplacing<float>& context);
    
    inline void setPeakFreq(float peakFreq) { this->peakFreq = peakFreq; }
    inline void setPeakGain(float peakGain) { this->peakGain = peakGain; }
    inline void setPeakQuality(float peakQuality) {this->peakQuality = peakQuality; }
    
    inline float getPeakFreq() { return peakFreq; }
    inline float getPeakGain() { return peakGain; }
    inline float getPeakQuality() { return peakQuality; }

private:
    // Parameters initialized here
    float peakFreq = 750;
    float peakGain = 0;
    float peakQuality = 1;
    
    using Filter = juce::dsp::IIR::Filter<float>;
    using MonoChain = juce::dsp::ProcessorChain<Filter>; // Can add more filters here later
    MonoChain chain;
    
    enum ChainPositions {
        Peak // can add more filters here later
    };
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKEqualizer)
};
