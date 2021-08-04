/*
  ==============================================================================

    BKEqualizer.h
    Created: 26 Jun 2021 4:47:49pm
    Author:  Jeffrey Gordon

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "BKUtilities.h"

//==============================================================================
/*
 Class for the equalizer. Attaches to bK in pluginProcessor processBlock() and
 prepareToPlay(). See GeneralViewController and EqualizerGraph for UI.
 
 Much of the DSP code here is based off this video:
 https://www.youtube.com/watch?v=i_Iq4_Kd7Rc&t=1596s&ab_channel=freeCodeCamp.org
*/
class BKEqualizer  : public juce::Component
{
public:
    BKEqualizer();
    ~BKEqualizer() override;
    
    // IMPORTANT: set the sample rate before doing anything else!
    inline void setSampleRate(double sampleRate) { this->sampleRate = sampleRate; DBG("BKEqualizer setting sr to" + String(sampleRate)); }
    inline double getSampleRate() { return sampleRate; }
    
    // Must be called before playback begins or change to sample settings
    void prepareToPlay(int samplesPerBlock);
    
    // Called upon initializiation and whenever a user changes parameters with a slider
    void updateCoefficients();
    
    // Performs the actual processing of audio
    void process(AudioSampleBuffer& buffer);
    
    void restoreDefaultLowCut()
    {
        lowCutFreq = 20;
        lowCutSlope = 12;
        paramsChanged = true;
    }
    
    void restoreDefaultPeak1()
    {
        peak1Freq = 500;
        peak1Gain = 0;
        peak1Quality = 1;
        paramsChanged = true;
    }
    
    void restoreDefaultPeak2()
    {
        peak2Freq = 1000;
        peak2Gain = 0;
        peak2Quality = 1;
        paramsChanged = true;
    }
    
    void restoreDefaultPeak3()
    {
        peak3Freq = 5000;
        peak3Gain = 0;
        peak3Quality = 1;
        paramsChanged = true;
    }
    
    void restoreDefaultHighCut()
    {
        highCutFreq = 20000;
        highCutSlope = 12;
        paramsChanged = true;
    }
    
    void restoreDefaultState()
    {
        restoreDefaultLowCut();
        restoreDefaultPeak1();
        restoreDefaultPeak2();
        restoreDefaultPeak3();
        restoreDefaultHighCut();
    }
    
    // Getters and Setters for the view controller
    inline void setBypassed(bool bypass)
    {
        this->bypassed = bypass;
        paramsChanged = true;
    }
    inline void setLowCutBypassed(bool bypass)
    {
        this->lowCutBypassed = bypass;
        paramsChanged = true;
    }
    inline void setPeak1Bypassed(bool bypass)
    {
        this->peak1Bypassed = bypass;
        paramsChanged = true;
    }
    inline void setPeak2Bypassed(bool bypass)
    {
        this->peak2Bypassed = bypass;
        paramsChanged = true;
    }
    inline void setPeak3Bypassed(bool bypass)
    {
        this->peak3Bypassed = bypass;
        paramsChanged = true;
    }
    inline void setHighCutBypassed(bool bypass)
    {
        this->highCutBypassed = bypass;
        paramsChanged = true;
    }
    inline void setLowCutFreq(float lowCutFreq)
    {
        this->lowCutFreq = lowCutFreq;
        paramsChanged = true;
    }
    inline void setLowCutSlope(int lowCutSlope)
    {
        this->lowCutSlope = lowCutSlope;
        paramsChanged = true;
    }
    inline void setPeak1Freq(float peak1Freq)
    {
        this->peak1Freq = peak1Freq;
        paramsChanged = true;
    }
    inline void setPeak1Gain(float peak1Gain)
    {
        this->peak1Gain = peak1Gain;
        paramsChanged = true;
    }
    inline void setPeak1Quality(float peak1Quality)
    {
        this->peak1Quality = peak1Quality;
        paramsChanged = true;
    }
    inline void setPeak2Freq(float peak2Freq)
    {
        this->peak2Freq = peak2Freq;
        paramsChanged = true;
    }
    inline void setPeak2Gain(float peak2Gain)
    {
        this->peak2Gain = peak2Gain;
        paramsChanged = true;
    }
    inline void setPeak2Quality(float peak2Quality)
    {
        this->peak2Quality = peak2Quality;
        paramsChanged = true;
    }
    inline void setPeak3Freq(float peak3Freq)
    {
        this->peak3Freq = peak3Freq;
        paramsChanged = true;
    }
    inline void setPeak3Gain(float peak3Gain)
    {
        this->peak3Gain = peak3Gain;
        paramsChanged = true;
    }
    inline void setPeak3Quality(float peak3Quality)
    {
        this->peak3Quality = peak3Quality;
        paramsChanged = true;
    }
    inline void setHighCutFreq(float highCutFreq)
    {
        this->highCutFreq = highCutFreq;
        paramsChanged = true;
    }
    inline void setHighCutSlope(int highCutSlope)
    {
        this->highCutSlope = highCutSlope;
        paramsChanged = true;
    }
    
    inline bool getBypassed() { return bypassed; }
    inline bool getLowCutBypassed() { return lowCutBypassed; }
    inline bool getPeak1Bypassed() { return peak1Bypassed; }
    inline bool getPeak2Bypassed() { return peak2Bypassed; }
    inline bool getPeak3Bypassed() { return peak3Bypassed; }
    inline bool getHighCutBypassed() { return highCutBypassed; }
    
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
    
    // Return the magnitude corresponding to this frequency based off the curent parameters of this equalizer
    double magForFreq(double freq);
    
    // Create a copy of an equalizer using =
    inline BKEqualizer& operator=(BKEqualizer& other) {
        // Guard self reference
        if (this == &other) return *this;
        
        this->sampleRate = other.sampleRate;
        
        this->bypassed = other.bypassed;
        this->lowCutBypassed = other.lowCutBypassed;
        this->peak1Bypassed = other.peak1Bypassed;
        this->peak1Bypassed = other.peak1Bypassed;
        this->peak1Bypassed = other.peak1Bypassed;
        this->highCutBypassed = other.highCutBypassed;
        
        this->lowCutFreq = other.lowCutFreq;
        this->lowCutSlope = other.lowCutSlope;
        this->peak1Freq = other.peak1Freq;
        this->peak1Gain = other.peak1Gain;
        this->peak1Quality = other.peak1Quality;
        this->peak2Freq = other.peak2Freq;
        this->peak2Gain = other.peak2Gain;
        this->peak2Quality = other.peak2Quality;
        this->peak3Freq = other.peak3Freq;
        this->peak3Gain = other.peak3Gain;
        this->peak3Quality = other.peak3Quality;
        this->highCutFreq = other.highCutFreq;
        this->highCutSlope = other.highCutSlope;
        
        this->updateCoefficients();
        
        return *this;
    }
    
    inline void setState(XmlElement* e) {
        bypassed = e->getBoolAttribute(ptagEqualizer_bypassed);
        lowCutBypassed = e->getBoolAttribute(ptagEqualizer_lowCutBypassed);
        peak1Bypassed = e->getBoolAttribute(ptagEqualizer_peak1Bypassed);
        peak2Bypassed = e->getBoolAttribute(ptagEqualizer_peak2Bypassed);
        peak3Bypassed = e->getBoolAttribute(ptagEqualizer_peak3Bypassed);
        highCutBypassed = e->getBoolAttribute(ptagEqualizer_highCutBypassed);
        
        lowCutFreq = e->getStringAttribute(ptagEqualizer_lowCutFreq).getFloatValue();
        lowCutSlope = e->getStringAttribute(ptagEqualizer_lowCutSlope).getFloatValue();
        peak1Freq = e->getStringAttribute(ptagEqualizer_peak1Freq).getFloatValue();
        peak1Gain = e->getStringAttribute(ptagEqualizer_peak1Gain).getFloatValue();
        peak1Quality = e->getStringAttribute(ptagEqualizer_peak1Quality).getFloatValue();
        peak2Freq = e->getStringAttribute(ptagEqualizer_peak2Freq).getFloatValue();
        peak2Gain = e->getStringAttribute(ptagEqualizer_peak2Gain).getFloatValue();
        peak2Quality = e->getStringAttribute(ptagEqualizer_peak2Quality).getFloatValue();
        peak3Freq = e->getStringAttribute(ptagEqualizer_peak3Freq).getFloatValue();
        peak3Gain = e->getStringAttribute(ptagEqualizer_peak3Gain).getFloatValue();
        peak3Quality = e->getStringAttribute(ptagEqualizer_peak3Quality).getFloatValue();
        highCutFreq = e->getStringAttribute(ptagEqualizer_highCutFreq).getFloatValue();
        highCutSlope = e->getStringAttribute(ptagEqualizer_highCutSlope).getFloatValue();
    }
    
    inline ValueTree getState(void) {
        ValueTree vt(vtagEqualizer);
        
        vt.setProperty(ptagEqualizer_bypassed, bypassed, 0);
        vt.setProperty(ptagEqualizer_lowCutBypassed, lowCutBypassed, 0);
        vt.setProperty(ptagEqualizer_peak1Bypassed, peak1Bypassed, 0);
        vt.setProperty(ptagEqualizer_peak2Bypassed, peak2Bypassed, 0);
        vt.setProperty(ptagEqualizer_peak3Bypassed, peak3Bypassed, 0);
        vt.setProperty(ptagEqualizer_highCutBypassed, highCutBypassed, 0);
        
        vt.setProperty(ptagEqualizer_lowCutFreq, lowCutFreq, 0);
        vt.setProperty(ptagEqualizer_lowCutSlope, lowCutSlope, 0);
        vt.setProperty(ptagEqualizer_peak1Freq, peak1Freq, 0);
        vt.setProperty(ptagEqualizer_peak1Gain, peak1Gain, 0);
        vt.setProperty(ptagEqualizer_peak1Quality, peak1Quality, 0);
        vt.setProperty(ptagEqualizer_peak2Freq, peak2Freq, 0);
        vt.setProperty(ptagEqualizer_peak2Gain, peak2Gain, 0);
        vt.setProperty(ptagEqualizer_peak2Quality, peak2Quality, 0);
        vt.setProperty(ptagEqualizer_peak3Freq, peak3Freq, 0);
        vt.setProperty(ptagEqualizer_peak3Gain, peak3Gain, 0);
        vt.setProperty(ptagEqualizer_peak3Quality, peak3Quality, 0);
        vt.setProperty(ptagEqualizer_highCutFreq, highCutFreq, 0);
        vt.setProperty(ptagEqualizer_highCutSlope, highCutSlope, 0);
        
        return vt;
    }

private:
    double sampleRate;
    
    bool paramsChanged;
    
    bool bypassed = true;
    
    // Parameters initialized here
    float lowCutFreq;
    int lowCutSlope;
    bool lowCutBypassed = false;
    
    float peak1Freq;
    float peak1Gain;
    float peak1Quality;
    bool peak1Bypassed = false;
    
    float peak2Freq;
    float peak2Gain;
    float peak2Quality;
    bool peak2Bypassed = false;
    
    float peak3Freq;
    float peak3Gain;
    float peak3Quality;
    bool peak3Bypassed = false;
    
    float highCutFreq;
    int highCutSlope;
    bool highCutBypassed = false;
    
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
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKEqualizer)
};
