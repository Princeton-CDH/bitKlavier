/*
  ==============================================================================

    BKCompressorView.h
    Created: 18 Jul 2022 11:01:12am
    Author:  Davis Polito

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "compressor-gui/include/LabeledSlider.h"
#include "compressor-gui/include/Meter.h"
#include "compressor-gui/include/Constants.h"
#include "BKLookAndFeel.h"
#include "CompressorProcessor.h"
class BKCompressorView : public juce::Component, public Button::Listener, public LabeledSlider::Listener, public juce::Timer
{
public:
    BKCompressorView(CompressorProcessor& p) :
    processor(p),
    powerButton("powerButton", DrawableButton::ButtonStyle::ImageOnButtonBackground)
    {
        setLookAndFeel(&laf);
        startTimerHz(60);
    }
    ~BKCompressorView(){};
    void resized() override;
    void paint(Graphics& g) override;
    void timerCallback() override;


private:
    void buttonClicked(Button* b) override;
    void LabeledSliderValueChanged(LabeledSlider* slider, String name, double val) override;
    CompressorProcessor &processor;
    void initWidgets();
    void setGUIState(bool powerButton);
    BKButtonAndMenuLAF laf;
    //Widgets
    MeterBackground meterbg;
    Meter meter;

    LabeledSlider inGainLSlider;
    LabeledSlider makeupGainLSlider;
    LabeledSlider treshLSlider;
    LabeledSlider ratioLSlider;
    LabeledSlider kneeLSlider;
    LabeledSlider attackLSlider;
    LabeledSlider releaseLSlider;
    LabeledSlider mixLSlider;

    TextButton lahButton;
    TextButton autoAttackButton;
    TextButton autoReleaseButton;
    TextButton autoMakeupButton;
    DrawableButton powerButton;

    std::unique_ptr<AudioProcessorValueTreeState::ButtonAttachment> lahAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::ButtonAttachment> autoAttackAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::ButtonAttachment> autoReleaseAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::ButtonAttachment> autoMakeupAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::ButtonAttachment> powerAttachment;

};
