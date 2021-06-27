/*
  ==============================================================================

    BKEqualizerViewController.cpp
    Created: 26 Jun 2021 3:36:52pm
    Author:  Jeffrey Gordon

  ==============================================================================
*/

#include <JuceHeader.h>
#include "BKEqualizerViewController.h"

//==============================================================================
BKEqualizerViewController::BKEqualizerViewController(BKAudioProcessor& p) :
processor(p)
{
    // grab defaults here
    BKEqualizer* eq = processor.getBKEqualizer();
    float peakFreqDefault = eq->getPeakFreq();
    float peakGainDefault = eq->getPeakGain();
    float peakQualityDefault = eq->getPeakQuality();
    
    // Slider setup
    peakFreqSlider = std::make_unique<BKSingleSlider>("Peak Frequency", "Peak Frequency", 20.f, 20000.f, peakFreqDefault, 1.f);
    peakFreqSlider->setToolTipString("Adjust the frequency of the peak.");
    peakFreqSlider->addMyListener(this);
    addAndMakeVisible(*peakFreqSlider);
    
    peakGainSlider = std::make_unique<BKSingleSlider>("Peak Gain", "Peak Gain", -24.f, 24.f, peakGainDefault, 0.5f);
    peakGainSlider->setToolTipString("Adjust the gain of the peak.");
    peakGainSlider->addMyListener(this);
    addAndMakeVisible(*peakGainSlider);
    
    peakQualitySlider = std::make_unique<BKSingleSlider>("Peak Quality", "Peak Quality", 0.1f, 10.f, peakQualityDefault, 0.05f);
    peakQualitySlider->setToolTipString("Adjust the quality of the peak.");
    peakQualitySlider->addMyListener(this);
    addAndMakeVisible(*peakQualitySlider);
}

BKEqualizerViewController::~BKEqualizerViewController()
{
}

void BKEqualizerViewController::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll (Colours::black);   // clear the background

//    g.setColour (juce::Colours::grey);
//    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component
//
//    g.setColour (juce::Colours::white);
//    g.setFont (14.0f);
//    g.drawText ("BKEqualizerViewController", getLocalBounds(),
//                juce::Justification::centred, true);   // draw some placeholder text
}

void BKEqualizerViewController::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
    peakFreqSlider->setBounds(100, 100, 300, 100);
    peakGainSlider->setBounds(100, 200, 300, 100);
    peakQualitySlider->setBounds(100, 300, 300, 100);

}

void BKEqualizerViewController::BKSingleSliderValueChanged(BKSingleSlider* slider, String name, double val) {
    
    BKEqualizer* eq = processor.getBKEqualizer();
    
    if (slider == peakFreqSlider.get()) {
        float peakFreq = peakFreqSlider->getValue();
        eq->setPeakFreq(peakFreq);
        DBG("Peak Frequency: " + String(peakFreq));
    }
    else if (slider == peakGainSlider.get()) {
        float peakGain = peakGainSlider->getValue();
        eq->setPeakGain(peakGain);
        DBG("Peak Gain: " + String(peakGain));
    }
    else if (slider == peakQualitySlider.get()) {
        float peakQuality = peakQualitySlider->getValue();
        eq->setPeakQuality(peakQuality);
        DBG("Peak Quality: " + String(peakQuality));
    }
}
