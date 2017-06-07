/*
  ==============================================================================

    DirectViewController2.h
    Created: 7 Jun 2017 1:42:55pm
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

class DirectViewController2 :
public BKComponent,
public BKListener,
public BKEditableComboBoxListener,
public BKSingleSliderListener,
public BKStackedSliderListener
{
public:
    
    DirectViewController2(BKAudioProcessor&, BKItemGraph* theGraph);
    ~DirectViewController2() {};
    
    BKEditableComboBox selectCB;
    
    ScopedPointer<BKStackedSlider> transpositionSlider;
    ScopedPointer<BKSingleSlider> gainSlider;
    ScopedPointer<BKSingleSlider> resonanceGainSlider;
    ScopedPointer<BKSingleSlider> hammerGainSlider;
    
    void paint (Graphics&) override;
    void resized() override;
    
    void updateFields();
    
    void bkMessageReceived (const String& message) override;
    void bkComboBoxDidChange (ComboBox* box) override;
    void bkTextFieldDidChange (TextEditor&) override {};
    void bkButtonClicked (Button* b) override { };
    void BKEditableComboBoxChanged(String name, int index) override;
    void BKSingleSliderValueChanged(String name, double val) override;
    void BKStackedSliderValueChanged(String name, Array<float> val) override;
    void fillSelectCB(void);

private:
    
    BKAudioProcessor& processor;
    BKItemGraph* theGraph;

};
