/*
  ==============================================================================

    BKEqualizer.h
    Created: 26 Jun 2021 4:47:49pm
    Author:  Jeffrey Gordon

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//#include "PluginProcessor.h"

//==============================================================================
/*
 Class for the equalizer. Attaches to bK in pluginProcessor processBlock() and
 prepareToPlay().
 
 Much of the DSP code here is based off this video:
 https://www.youtube.com/watch?v=i_Iq4_Kd7Rc&t=1596s&ab_channel=freeCodeCamp.org
*/
class BKEqualizer  : public juce::Component
{
public:
    BKEqualizer();
    ~BKEqualizer() override;
    
    // Must be called before playback begins or change to sample settings
    void prepareToPlay(double sampleRate, int samplesPerBlock);
    
    // Called upon initializiation and whenever a user changes parameters with a slider
    void updateCoefficients(double sampleRate);
    
    // Performs the actual processing of audio
    void process(juce::dsp::ProcessContextReplacing<float>& context, int channel);
    
    // Getters and Setters for the view controller
    inline void setLowCutFreq(float lowCutFreq) { this->lowCutFreq = lowCutFreq; }
    inline void setLowCutSlope(int lowCutSlope) {this->lowCutSlope = lowCutSlope; }
    inline void setPeak1Freq(float peak1Freq) { this->peak1Freq = peak1Freq; }
    inline void setPeak1Gain(float peak1Gain) {this->peak1Gain = peak1Gain; }
    inline void setPeak1Quality(float peak1Quality) {this->peak1Quality = peak1Quality; }
    inline void setPeak2Freq(float peak2Freq) { this->peak2Freq = peak2Freq;}
    inline void setPeak2Gain(float peak2Gain) {this->peak2Gain = peak2Gain; }
    inline void setPeak2Quality(float peak2Quality) {this->peak2Quality = peak2Quality; }
    inline void setPeak3Freq(float peak3Freq) { this->peak3Freq = peak3Freq;}
    inline void setPeak3Gain(float peak3Gain) {this->peak3Gain = peak3Gain; }
    inline void setPeak3Quality(float peak3Quality) {this->peak3Quality = peak3Quality; }
    inline void setHighCutFreq(float highCutFreq) { this->highCutFreq = highCutFreq; }
    inline void setHighCutSlope(int highCutSlope) {this->highCutSlope = highCutSlope; }
    
    inline float getLowCutFreq() { return lowCutFreq; }
    inline int getLowCutSlope() { return lowCutSlope; }
    inline float getPeak1Freq() { return peak1Freq; }
    inline float getPeak1Gain() { return peak1Gain; }
    inline float getPeak1Quality() { return peak1Quality; }
    inline float getPeak2Freq() { return peak2Freq; }
    inline float getPeak2Gain() { return peak2Gain; }
    inline float getPeak2Quality() { return peak2Quality; }
    inline float getPeak3Freq() { return peak3Freq; }
    inline float getPeak3Gain() { return peak3Gain; }
    inline float getPeak3Quality() { return peak3Quality; }
    inline float getHighCutFreq() { return highCutFreq; }
    inline int getHighCutSlope() { return highCutSlope; }
    
    inline void setLastDisplayed(int lastDisplayed) { this->lastDisplayed = lastDisplayed; }
    inline int getLastDisplayed() { return lastDisplayed; }
    
    // Return the magnitude corresponding to this frequency based off the parameters of this equalizer
    double magForFreq(double freq);
    
    inline void setSampleRate(double sampleRate) { sr = sampleRate; }

private:
    //BKAudioProcessor& processor; // hmmm this might be the issue, this and plugin processor both depend on each other
    
    double sr;
    
    // Parameters initialized here
    float lowCutFreq = 20;
    int lowCutSlope = 12;
    
    float peak1Freq = 500;
    float peak1Gain = 0;
    float peak1Quality = 1;
    
    float peak2Freq = 1000;
    float peak2Gain = 0;
    float peak2Quality = 1;
    
    float peak3Freq = 5000;
    float peak3Gain = 0;
    float peak3Quality = 1;
    
    float highCutFreq = 20000;
    int highCutSlope = 12;
    
    // DSP stuff
    using Filter = juce::dsp::IIR::Filter<float>;
    using CutFilter = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter>;
    using Chain = juce::dsp::ProcessorChain<CutFilter, Filter, Filter, Filter, CutFilter>;
    Chain leftChain;
    Chain rightChain;
    
    enum ChainPositions {
        LowCut,
        Peak1,
        Peak2,
        Peak3,
        HighCut
    };
    
    enum Slope {
        S12,
        S24,
        S36,
        S48
    };
    
    // Need to save which filter to display outside view controller, so it's here for now
    // initalized to show low cut on first open
    int lastDisplayed = ChainPositions::LowCut;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKEqualizer)
};
