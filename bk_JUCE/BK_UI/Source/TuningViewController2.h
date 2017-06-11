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
#include "BKKeyboard.h"
#include "BKKeyboardState.h"

class TuningViewController2 :
public BKComponent,
public BKListener,
public BKEditableComboBoxListener,
public BKSingleSliderListener,
public BKKeymapKeyboardStateListener
{
public:
    
    TuningViewController2(BKAudioProcessor&, BKItemGraph* theGraph);
    ~TuningViewController2() {};
    
    void paint (Graphics&) override;
    void resized() override;
    
    void updateFields();
    
    //basics
    BKEditableComboBox selectCB;
    BKComboBox scaleCB;
    BKComboBox fundamentalCB;
    ScopedPointer<BKSingleSlider> offsetSlider;
    
    //adaptive tuning 1 stuff
    BKComboBox A1IntervalScaleCB;
    ToggleButton A1Inversional;
    BKComboBox A1AnchorScaleCB;
    BKComboBox A1FundamentalCB;
    ScopedPointer<BKSingleSlider> A1ClusterThresh;
    ScopedPointer<BKSingleSlider> A1History;
    
    //absolute tuning keyboard stuff
    Label absoluteKeyboardName;
    TextEditor absoluteKeyboardValueTF;
    BKKeymapKeyboardState absoluteKeyboardState;
    Component *absoluteKeyboardComponent;
    Array<float> absoluteOffsets;   //for entire keyboard; up to 88 vals
    int lastAbsoluteKeyPressed;
    
    //custom tuning keyboard stuff
    Label customKeyboardName;
    TextEditor customKeyboardValueTF;
    BKKeymapKeyboardState customKeyboardState;
    Component *customKeyboardComponent;
    Array<float> customOffsets;     //for custom tuning; 12 vals
    int lastCustomKeyPressed;
    
    //keymap overrides
    void handleKeymapNoteOn (BKKeymapKeyboardState* source, int midiNoteNumber) override;
    void handleKeymapNoteOff (BKKeymapKeyboardState* source, int midiNoteNumber) override;
    void handleKeymapNoteToggled (BKKeymapKeyboardState* source, int midiNoteNumber) override;
    
    //other overrides
    void bkMessageReceived (const String& message) override {};
    void bkComboBoxDidChange (ComboBox* box) override;
    void bkTextFieldDidChange (TextEditor&) override {};
    void bkButtonClicked (Button* b) override { };
    void BKEditableComboBoxChanged(String name, BKEditableComboBox* cb) override;
    void BKSingleSliderValueChanged(String name, double val) override {};
    void fillSelectCB(void);
    
private:
    
    BKAudioProcessor& processor;
    BKItemGraph* theGraph;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TuningViewController2)
    
};
