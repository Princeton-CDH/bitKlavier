/*
  ==============================================================================

    BKEqualizerViewController.h
    Created: 26 Jun 2021 3:36:52pm
    Author:  Jeffrey Gordon

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "BKSlider.h"
#include "BKViewController.h"

//==============================================================================
/*
*/
class BKEqualizerViewController  : public juce::Component, public BKSingleSlider::Listener, public TextButton::Listener
{
public:
    BKEqualizerViewController(BKAudioProcessor& p);
    ~BKEqualizerViewController() override;

    void paint (juce::Graphics&) override;
    
    // Set the size of all components
    void resized() override;

private:
    // makes all components invisible - called by display
    void invisible();
    
    // display the parameter sliders corresponding to a particular filter
    void display(int filterParams);
    
    // invoked when a parameter slider changes
    void BKSingleSliderValueChanged(BKSingleSlider* slider, String name, double val) override;
    
    // invoked when a filter is selected
    void buttonClicked(Button* b) override;
    
    
    // Store a reference to the processor
    BKAudioProcessor& processor;
    
    // UI elements
    TextButton lowCutButton;
    TextButton peak1Button;
    TextButton peak2Button;
    TextButton peak3Button;
    TextButton highCutButton;
    
//    std::unique_ptr<BKSingleSlider> peakFreqSlider;
//    std::unique_ptr<BKSingleSlider> peakGainSlider;
//    std::unique_ptr<BKSingleSlider> peakQualitySlider;
//
    
    std::unique_ptr<BKSingleSlider> lowCutFreqSlider;
    std::unique_ptr<BKSingleSlider> lowCutSlopeSlider;
    
    std::unique_ptr<BKSingleSlider> peak1FreqSlider;
    std::unique_ptr<BKSingleSlider> peak1GainSlider;
    std::unique_ptr<BKSingleSlider> peak1QualitySlider;
    
    std::unique_ptr<BKSingleSlider> peak2FreqSlider;
    std::unique_ptr<BKSingleSlider> peak2GainSlider;
    std::unique_ptr<BKSingleSlider> peak2QualitySlider;
    
    std::unique_ptr<BKSingleSlider> peak3FreqSlider;
    std::unique_ptr<BKSingleSlider> peak3GainSlider;
    std::unique_ptr<BKSingleSlider> peak3QualitySlider;
    
    std::unique_ptr<BKSingleSlider> highCutFreqSlider;
    std::unique_ptr<BKSingleSlider> highCutSlopeSlider;
    
    enum Filters {
        lowCut,
        peak1,
        peak2,
        peak3,
        highCut
    };
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKEqualizerViewController)
};
