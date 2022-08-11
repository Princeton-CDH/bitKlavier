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
    powerButton("powerButton", DrawableButton::ButtonStyle::ImageOnButtonBackground),
    backGroundApp(Colour(Constants::Colors::bg_App))
    {
        setLookAndFeel(&laf);
        startTimerHz(60);
        setGUIState(true);
        initWidgets();
    }
    ~BKCompressorView(){};
    void resized() override;
    void paint(Graphics& g) override;
    void timerCallback() override;

    void update()
    {
        inGainLSlider.getSlider()->setValue(processor.compressor.getInput());
        makeupGainLSlider.getSlider()->setValue(processor.compressor.getMakeup());
        treshLSlider.getSlider()->setValue(processor.compressor.getThreshold());
        ratioLSlider.getSlider()->setValue(processor.compressor.getRatio());
        kneeLSlider.getSlider()->setValue(processor.compressor.getKnee());
        attackLSlider.getSlider()->setValue(processor.compressor.getAttack());
        releaseLSlider.getSlider()->setValue(processor.compressor.getRelease());
        mixLSlider.getSlider()->setValue(processor.compressor.getMix());
        lahButton.setEnabled(!processor.compressor.getLookahead());
        autoReleaseButton.setEnabled(!processor.compressor.getAutoRelease());
        autoMakeupButton.setEnabled(!processor.compressor.getAutoMakeup());
        autoAttackButton.setEnabled(!processor.compressor.getAutoAttack());
        powerButton.setEnabled(!processor.compressor.getPower());
    }

private:
    BKButtonAndMenuLAF laf;
    void buttonClicked(Button* b) override;
    void LabeledSliderValueChanged(LabeledSlider* slider, String name, double val) override;
    CompressorProcessor &processor;
    void initWidgets();
    void setGUIState(bool powerButton);
    
    
    Colour backGroundApp;
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
