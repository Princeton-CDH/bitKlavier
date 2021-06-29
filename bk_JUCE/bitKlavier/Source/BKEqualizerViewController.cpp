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
    // Filter Selection Buttons
    lowCutButton.setName("Low Cut");
    lowCutButton.setButtonText("Low Cut");
    lowCutButton.setTooltip("Select the Low Cut filter to alter its parameters.");
    lowCutButton.addListener(this);
    addAndMakeVisible(lowCutButton);
    
    peak1Button.setName("Peak 1");
    peak1Button.setButtonText("Peak 1");
    peak1Button.setTooltip("Select the first Peak filter to alter its parameters.");
    peak1Button.addListener(this);
    addAndMakeVisible(peak1Button);
    
    peak2Button.setName("Peak 2");
    peak2Button.setButtonText("Peak 2");
    peak2Button.setTooltip("Select the second Peak filter to alter its parameters.");
    peak2Button.addListener(this);
    addAndMakeVisible(peak2Button);
    
    peak3Button.setName("Peak 3");
    peak3Button.setButtonText("Peak 3");
    peak3Button.setTooltip("Select the third Peak filter to alter its parameters.");
    peak3Button.addListener(this);
    addAndMakeVisible(peak3Button);
    
    highCutButton.setName("High Cut");
    highCutButton.setButtonText("High Cut");
    highCutButton.setTooltip("Select the High Cut filter to alter its parameters.");
    highCutButton.addListener(this);
    addAndMakeVisible(highCutButton);
    
    // grab defaults from here
    BKEqualizer* eq = processor.getBKEqualizer();
    
    // Slider setup -- that's a lot of sliders!
    lowCutFreqSlider = std::make_unique<BKSingleSlider>("Frequency", "Low Cut Frequency", 20.f, 20000.f, eq->getLowCutFreq(), 1.f);
    lowCutFreqSlider->setToolTipString("Adjust the frequency of the low cut");
    lowCutFreqSlider->setSkewFactorFromMidPoint(1000.f);
    lowCutFreqSlider->addMyListener(this);
    addAndMakeVisible(*lowCutFreqSlider);
    
    lowCutSlopeSlider = std::make_unique<BKSingleSlider>("Slope (db/Oct)", "Low Cut Slope", 12, 48, eq->getLowCutSlope(), 12);
    lowCutSlopeSlider->setToolTipString("Adjust the slope of the low cut");
    lowCutSlopeSlider->addMyListener(this);
    addAndMakeVisible(*lowCutSlopeSlider);
    
    highCutFreqSlider = std::make_unique<BKSingleSlider>("Frequency", "High Cut Frequency", 20.f, 20000.f, eq->getHighCutFreq(), 1.f);
    highCutFreqSlider->setToolTipString("Adjust the frequency of the high cut");
    highCutFreqSlider->setSkewFactorFromMidPoint(1000.f);
    highCutFreqSlider->addMyListener(this);
    addAndMakeVisible(*highCutFreqSlider);
    
    highCutSlopeSlider = std::make_unique<BKSingleSlider>("Slope (db/Oct)", "High Cut Slope", 12, 48, eq->getHighCutSlope(), 12);
    highCutSlopeSlider->setToolTipString("Adjust the slope of the high cut");
    highCutSlopeSlider->addMyListener(this);
    addAndMakeVisible(*highCutSlopeSlider);
    
    peak1FreqSlider = std::make_unique<BKSingleSlider>("Frequency", "Peak 1 Frequency", 20.f, 20000.f, eq->getPeak1Freq(), 1.5);
    peak1FreqSlider->setToolTipString("Adjust the frequency of the first peak filter");
    peak1FreqSlider->setSkewFactorFromMidPoint(1000.f);
    peak1FreqSlider->addMyListener(this);
    addAndMakeVisible(*peak1FreqSlider);
    
    peak1GainSlider = std::make_unique<BKSingleSlider>("Gain (db)", "Peak 1 Gain", -24.f, 24.f, eq->getPeak1Gain(), 0.5f);
    peak1GainSlider->setToolTipString("Adjust the cut or boost size of the first peak filter");
    peak1GainSlider->addMyListener(this);
    addAndMakeVisible(*peak1GainSlider);
    
    peak1QualitySlider = std::make_unique<BKSingleSlider>("Quality", "Peak 1 Quality", 0.1f, 10.f, eq->getPeak1Quality(), 0.05f);
    peak1QualitySlider->setToolTipString("Adjust the quality of the first peak filter's cut or boost");
    peak1QualitySlider->addMyListener(this);
    addAndMakeVisible(*peak1QualitySlider);
    
    peak2FreqSlider = std::make_unique<BKSingleSlider>("Frequency", "Peak 2 Frequency", 20.f, 20000.f, eq->getPeak2Freq(), 1.5);
    peak2FreqSlider->setToolTipString("Adjust the frequency of the second peak filter");
    peak2FreqSlider->setSkewFactorFromMidPoint(1000.f);
    peak2FreqSlider->addMyListener(this);
    addAndMakeVisible(*peak2FreqSlider);
    
    peak2GainSlider = std::make_unique<BKSingleSlider>("Gain (db)", "Peak 2 Gain", -24.f, 24.f, eq->getPeak2Gain(), 0.5f);
    peak2GainSlider->setToolTipString("Adjust the cut or boost size of the second peak filter");
    peak2GainSlider->addMyListener(this);
    addAndMakeVisible(*peak2GainSlider);
    
    peak2QualitySlider = std::make_unique<BKSingleSlider>("Quality", "Peak 2 Quality", 0.1f, 10.f, eq->getPeak2Quality(), 0.05f);
    peak2QualitySlider->setToolTipString("Adjust the quality of the second peak filter's cut or boost");
    peak2QualitySlider->addMyListener(this);
    addAndMakeVisible(*peak2QualitySlider);
    
    peak3FreqSlider = std::make_unique<BKSingleSlider>("Frequency", "Peak 3 Frequency", 20.f, 20000.f, eq->getPeak3Freq(), 1.5);
    peak3FreqSlider->setToolTipString("Adjust the frequency of the third peak filter");
    peak3FreqSlider->setSkewFactorFromMidPoint(1000.f);
    peak3FreqSlider->addMyListener(this);
    addAndMakeVisible(*peak3FreqSlider);
    
    peak3GainSlider = std::make_unique<BKSingleSlider>("Gain (db)", "Peak 3 Gain", -24.f, 24.f, eq->getPeak3Gain(), 0.5f);
    peak3GainSlider->setToolTipString("Adjust the cut or boost size of the third peak filter");
    peak3GainSlider->addMyListener(this);
    addAndMakeVisible(*peak3GainSlider);
    
    peak3QualitySlider = std::make_unique<BKSingleSlider>("Quality", "Peak 3 Quality", 0.1f, 10.f, eq->getPeak3Quality(), 0.05f);
    peak3QualitySlider->setToolTipString("Adjust the quality of the third peak filter's cut or boost");
    peak3QualitySlider->addMyListener(this);
    addAndMakeVisible(*peak3QualitySlider);
    
    // start on the low cut page
    display(eq->getLastDisplayed());
}

BKEqualizerViewController::~BKEqualizerViewController()
{
}

void BKEqualizerViewController::paint (juce::Graphics& g)
{
    g.fillAll (Colours::black);   // clear the background
    
    // could eventually be used to draw a spectrum or something maybe...
}

void BKEqualizerViewController::resized()
{
    // Position the buttons at the top
    Rectangle<int> buttonWrapper(50, 10, getWidth() - 100, 50);
    Rectangle<int> buttonWrapperCopy(buttonWrapper); // save to use later
    float spacing = buttonWrapper.getWidth() / 5;
    int padding = spacing - 2; // change the 2 to change padding between buttons
    lowCutButton.setBounds(buttonWrapper.removeFromLeft(spacing).removeFromRight(padding));
    peak1Button.setBounds(buttonWrapper.removeFromLeft(spacing).removeFromRight(padding));
    peak2Button.setBounds(buttonWrapper.removeFromLeft(spacing).removeFromRight(padding));
    peak3Button.setBounds(buttonWrapper.removeFromLeft(spacing).removeFromRight(padding));
    highCutButton.setBounds(buttonWrapper.removeFromLeft(spacing).removeFromRight(padding));
    
    // Position the sliders appropriately
    padding = 50;
    int reduce = 50;
    Rectangle<int> sliderWrapper(buttonWrapperCopy.getX() + reduce,
                                 buttonWrapperCopy.getBottom() + padding,
                                 buttonWrapperCopy.getWidth() - 2 * reduce,
                                 getHeight() - buttonWrapperCopy.getBottom() - 2 * padding);
    
    spacing = sliderWrapper.getHeight() / 3;
    Rectangle<int> firstOfThree(sliderWrapper.removeFromTop(spacing));
    Rectangle<int> secondOfThree(sliderWrapper.removeFromTop(spacing));
    Rectangle<int> thirdOfThree(sliderWrapper.removeFromTop(spacing));
    
    lowCutFreqSlider->setBounds(firstOfThree);
    peak1FreqSlider->setBounds(firstOfThree);
    peak2FreqSlider->setBounds(firstOfThree);
    peak3FreqSlider->setBounds(firstOfThree);
    highCutFreqSlider->setBounds(firstOfThree);
    
    lowCutSlopeSlider->setBounds(secondOfThree);
    peak1GainSlider->setBounds(secondOfThree);
    peak2GainSlider->setBounds(secondOfThree);
    peak3GainSlider->setBounds(secondOfThree);
    highCutSlopeSlider->setBounds(secondOfThree);
    
    peak1QualitySlider->setBounds(thirdOfThree);
    peak2QualitySlider->setBounds(thirdOfThree);
    peak3QualitySlider->setBounds(thirdOfThree);
}

void BKEqualizerViewController::invisible() {
    lowCutFreqSlider->setVisible(false);
    lowCutSlopeSlider->setVisible(false);
    peak1FreqSlider->setVisible(false);
    peak1GainSlider->setVisible(false);
    peak1QualitySlider->setVisible(false);
    peak2FreqSlider->setVisible(false);
    peak2GainSlider->setVisible(false);
    peak2QualitySlider->setVisible(false);
    peak3FreqSlider->setVisible(false);
    peak3GainSlider->setVisible(false);
    peak3QualitySlider->setVisible(false);
    highCutFreqSlider->setVisible(false);
    highCutSlopeSlider->setVisible(false);
}

void BKEqualizerViewController::display(int filterParams) {
    invisible();
    
    if (filterParams == Filters::lowCut) {
        lowCutFreqSlider->setVisible(true);
        lowCutSlopeSlider->setVisible(true);
    }
    else if (filterParams == Filters::peak1) {
        peak1FreqSlider->setVisible(true);
        peak1GainSlider->setVisible(true);
        peak1QualitySlider->setVisible(true);
    }
    else if (filterParams == Filters::peak2) {
        peak2FreqSlider->setVisible(true);
        peak2GainSlider->setVisible(true);
        peak2QualitySlider->setVisible(true);
    }
    else if (filterParams == Filters::peak3) {
        peak3FreqSlider->setVisible(true);
        peak3GainSlider->setVisible(true);
        peak3QualitySlider->setVisible(true);
    }
    else if (filterParams == Filters::highCut) {
        highCutFreqSlider->setVisible(true);
        highCutSlopeSlider->setVisible(true);
    }
    
    BKEqualizer* eq = processor.getBKEqualizer();
    eq->setLastDisplayed(filterParams);
}

void BKEqualizerViewController::buttonClicked(Button *b) {
    if (b == &lowCutButton) {
        display(Filters::lowCut);
        DBG("low cut button clicked");
    }
    else if (b == &peak1Button) {
        display(Filters::peak1);
    }
    else if (b == &peak2Button) {
        display(Filters::peak2);
    }
    else if (b == &peak3Button) {
        display(Filters::peak3);
    }
    else if (b == &highCutButton) {
        display(Filters::highCut);
    }
}

void BKEqualizerViewController::BKSingleSliderValueChanged(BKSingleSlider* slider, String name, double val) {
    
    BKEqualizer* eq = processor.getBKEqualizer();
    
    if (slider == lowCutFreqSlider.get()) {
        float lowCutFreq = lowCutFreqSlider->getValue();
        eq->setLowCutFreq(lowCutFreq);
        DBG("Low Cut Freq: " + String(lowCutFreq));
    }
    else if (slider == lowCutSlopeSlider.get()) {
        int lowCutSlope = lowCutSlopeSlider->getValue();
        eq->setLowCutSlope(lowCutSlope);
        DBG("Low Cut Slope: " + String(lowCutSlope));
    }
    else if (slider == peak1FreqSlider.get()) {
        float peak1Freq = peak1FreqSlider->getValue();
        eq->setPeak1Freq(peak1Freq);
        DBG("Peak 1 Frequency: " + String(peak1Freq));
    }
    else if (slider == peak1GainSlider.get()) {
        float peak1Gain = peak1GainSlider->getValue();
        eq->setPeak1Gain(peak1Gain);
        DBG("Peak 1 Gain: " + String(peak1Gain));
    }
    else if (slider == peak1QualitySlider.get()) {
        float peak1Quality = peak1QualitySlider->getValue();
        eq->setPeak1Quality(peak1Quality);
        DBG("Peak 1 Quality: " + String(peak1Quality));
    }
    else if (slider == peak2FreqSlider.get()) {
        float peak2Freq = peak2FreqSlider->getValue();
        eq->setPeak2Freq(peak2Freq);
        DBG("Peak 2 Frequency: " + String(peak2Freq));
    }
    else if (slider == peak2GainSlider.get()) {
        float peak2Gain = peak2GainSlider->getValue();
        eq->setPeak2Gain(peak2Gain);
        DBG("Peak 2 Gain: " + String(peak2Gain));
    }
    else if (slider == peak2QualitySlider.get()) {
        float peak2Quality = peak2QualitySlider->getValue();
        eq->setPeak2Quality(peak2Quality);
        DBG("Peak 2 Quality: " + String(peak2Quality));
    }
    else if (slider == peak3FreqSlider.get()) {
        float peak3Freq = peak3FreqSlider->getValue();
        eq->setPeak3Freq(peak3Freq);
        DBG("Peak 3 Frequency: " + String(peak3Freq));
    }
    else if (slider == peak3GainSlider.get()) {
        float peak3Gain = peak3GainSlider->getValue();
        eq->setPeak3Gain(peak3Gain);
        DBG("Peak 3 Gain: " + String(peak3Gain));
    }
    else if (slider == peak3QualitySlider.get()) {
        float peak3Quality = peak3QualitySlider->getValue();
        eq->setPeak3Quality(peak3Quality);
        DBG("Peak 3 Quality: " + String(peak3Quality));
    }
    else if (slider == highCutFreqSlider.get()) {
        float highCutFreq = highCutFreqSlider->getValue();
        eq->setHighCutFreq(highCutFreq);
        DBG("High Cut Freq: " + String(highCutFreq));
    }
    else if (slider == highCutSlopeSlider.get()) {
        int highCutSlope = highCutSlopeSlider->getValue();
        eq->setHighCutSlope(highCutSlope);
        DBG("High Cut Slope: " + String(highCutSlope));
    }
}
