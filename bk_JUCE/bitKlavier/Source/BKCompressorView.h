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
class BKCompressorView : public juce::Component, public LabeledSlider::Listener, public juce::Timer, BKListener
{
public:
    BKCompressorView(CompressorProcessor& p, BKAudioProcessor& bkp) :
    processor(p),
    bkp(bkp),
    backGroundApp(Colour(Constants::Colors::bg_App)),
    meter(&selectCB)
    {
        setLookAndFeel(&laf);
        startTimerHz(60);
        setGUIState(false);
        initWidgets();
    }
    ~BKCompressorView()
    {
        setLookAndFeel(nullptr);
    };
    
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
    void bkButtonClicked(Button* b) override;
    void bkComboBoxDidChange(ComboBox* box) override {return;};
    void LabeledSliderValueChanged(LabeledSlider* slider, String name, double val) override;
    void bkMessageReceived(const String& message) override {return;}
    void comboBoxChanged (ComboBox* comboBoxThatHasChanged) override;
   
    void actionListenerCallback (const String& message) override {};
    CompressorProcessor &processor;
    BKAudioProcessor &bkp;
    void initWidgets();
    void setGUIState(bool powerButton);
    void fillSelectCB(int last, int current);
    static void actionButtonCallback(int action,BKCompressorView*);
    StringArray compressorNames;
    String name;
    String url;
    int numCompressorPresets = 0;
    int selectedPresetId = 1;
    const int builtInPresets = 2; 
    PopupMenu getExportedPrepsMenu();
    PopupMenu getPrepOptionMenu( bool singlePrep);
    Colour backGroundApp;
    //Widgets
    BKComboBox selectCB;
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
    TextButton powerButton;
    TextButton actionButton;
    
    std::unique_ptr<AudioProcessorValueTreeState::ButtonAttachment> lahAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::ButtonAttachment> autoAttackAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::ButtonAttachment> autoReleaseAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::ButtonAttachment> autoMakeupAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::ButtonAttachment> powerAttachment;

};
