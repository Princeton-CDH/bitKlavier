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
class BKEqualizerViewController  : public juce::Component, public BKSingleSlider::Listener
{
public:
    BKEqualizerViewController(BKAudioProcessor& p);
    ~BKEqualizerViewController() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void BKSingleSliderValueChanged(BKSingleSlider* slider, String name, double val) override;
    
    // Store a reference to the processor
    BKAudioProcessor& processor;
    
    // UI elements
    std::unique_ptr<BKSingleSlider> peakFreqSlider;
    std::unique_ptr<BKSingleSlider> peakGainSlider;
    std::unique_ptr<BKSingleSlider> peakQualitySlider;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKEqualizerViewController)
};
