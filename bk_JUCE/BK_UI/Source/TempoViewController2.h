/*
  ==============================================================================

    TempoViewController2.h
    Created: 17 Jun 2017 5:29:25pm
    Author:  Daniel Trueman

  ==============================================================================
*/

#pragma once

#include "BKUtilities.h"
#include "PluginProcessor.h"
#include "BKListener.h"
#include "BKComponent.h"
#include "BKSlider.h"
#include "BKGraph.h"

class TempoViewController2 :
public BKComponent,
public BKListener,
public BKEditableComboBoxListener,
public BKRangeSliderListener,
public BKSingleSliderListener,
public Timer
{
public:
    
    TempoViewController2(BKAudioProcessor&, BKItemGraph* theGraph);
    ~TempoViewController2() {};
    
    void paint (Graphics&) override;
    void resized() override;
    void timerCallback() override;
    
    void updateFields();
    
    
private:
    
    BKAudioProcessor& processor;
    BKItemGraph* theGraph;
    
    BKLabel selectLabel;
    BKEditableComboBox selectCB;
    
    BKLabel modeLabel;
    BKComboBox modeCB;
    
    ScopedPointer<BKSingleSlider> tempoSlider;
    
    ScopedPointer<BKSingleSlider> AT1HistorySlider;
    ScopedPointer<BKSingleSlider> AT1SubdivisionsSlider;
    ScopedPointer<BKRangeSlider> AT1MinMaxSlider; //need to have "MinAlwaysLessThanMax" mode
    
    BKLabel A1ModeLabel;
    BKComboBox A1ModeCB;
    
    BKLabel A1AdaptedTempo;
    BKLabel A1AdaptedPeriodMultiplier;
    
    TextButton A1reset;
    
    float lastPeriodMultiplier;
    
    void bkMessageReceived (const String& message) override {};
    void bkComboBoxDidChange (ComboBox* box) override;
    void bkTextFieldDidChange (TextEditor& txt) override {};
    void bkButtonClicked (Button* b) override;
    void BKEditableComboBoxChanged(String name, BKEditableComboBox* cb) override;
    void BKSingleSliderValueChanged(String name, double val) override;
    void BKRangeSliderValueChanged(String name, double minval, double maxval) override;
    void textEditorReturnKeyPressed(TextEditor& textEditor) override {};
    
    void fillSelectCB(void);
    void fillModeCB(void);
    void fillA1ModeCB(void);
    
    void updateComponentVisibility();
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TempoViewController2)
};
