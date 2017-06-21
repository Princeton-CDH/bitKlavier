/*
  ==============================================================================

    TuningViewController2.h
    Created: 10 Jun 2017 11:24:22am
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
#include "BKKeyboardSlider.h"

class TuningViewController2 :
public BKComponent,
public BKListener,
public BKEditableComboBoxListener,
public BKSingleSliderListener,
public BKKeyboardSliderListener,
public Timer
{
public:
    
    TuningViewController2(BKAudioProcessor&, BKItemGraph* theGraph);
    ~TuningViewController2() {};
    
    void paint (Graphics&) override;
    void resized() override;
    void timerCallback() override;
    
    void updateFields();
    
private:
    
    BKAudioProcessor& processor;
    BKItemGraph* theGraph;
    
    //basics
    BKEditableComboBox selectCB;
    
    BKLabel scaleLabel;
    BKComboBox scaleCB;
    int customIndex; //index of Custom tuning in combobox
    
    BKLabel fundamentalLabel;
    BKComboBox fundamentalCB;
    
    ScopedPointer<BKSingleSlider> offsetSlider;
    
    //adaptive tuning 1 stuff
    BKLabel A1IntervalScaleLabel;
    BKComboBox A1IntervalScaleCB;
    
    ToggleButton A1Inversional;
    
    BKLabel A1AnchorScaleLabel;
    BKComboBox A1AnchorScaleCB;
    
    BKLabel A1FundamentalLabel;
    BKComboBox A1FundamentalCB;
    
    ScopedPointer<BKSingleSlider> A1ClusterThresh;
    ScopedPointer<BKSingleSlider> A1ClusterMax;
    
    TextButton A1reset;

    Array<float> absoluteOffsets;   //for entire keyboard; up to 128 vals
    Array<float> customOffsets;     //for custom tuning; 12 vals
    
    BKLabel lastNote;
    BKLabel lastInterval;
    float lastNoteTuningSave = -.1;
    
    BKKeyboardSlider absoluteKeyboard;
    BKKeyboardSlider customKeyboard;
    
    //other overrides
    void bkMessageReceived (const String& message) override {};
    void bkComboBoxDidChange (ComboBox* box) override;
    void bkTextFieldDidChange (TextEditor& txt) override;
    void bkButtonClicked (Button* b) override;
    void BKEditableComboBoxChanged(String name, BKEditableComboBox* cb) override;
    void BKSingleSliderValueChanged(String name, double val) override;
    void textEditorReturnKeyPressed(TextEditor& textEditor) override;
    void keyboardSliderChanged(String name, Array<float> values) override;
    void mouseMove(const MouseEvent& e) override {};
    
    void fillSelectCB(void);
    void fillTuningCB(void);
    void fillFundamentalCB(void);
    
    void updateComponentVisibility();
    
    BKButtonAndMenuLAF buttonsAndMenusLAF;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TuningViewController2)
    
};