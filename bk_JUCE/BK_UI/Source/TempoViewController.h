/*
  ==============================================================================

    TempoViewController.h
    Created: 17 Jun 2017 5:29:25pm
    Author:  Daniel Trueman

  ==============================================================================
*/

#pragma once

#include "BKViewController.h"

class TempoViewController : public BKViewController
{
public:
    
    TempoViewController(BKAudioProcessor&, BKItemGraph* theGraph);
    ~TempoViewController() {};
    
    void paint (Graphics&) override;
    void resized() override;
    
    virtual void update(void){};
    
protected:
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
    
    void fillModeCB(void);
    void fillA1ModeCB(void);
    
    void updateComponentVisibility();
    
    ImageComponent iconImageComponent;
    TextButton hideOrShow;
    
    BKButtonAndMenuLAF buttonsAndMenusLAF;
    
private:
    
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TempoViewController)
};

class TempoPreparationEditor :
public TempoViewController,
public BKEditableComboBoxListener,
public BKRangeSliderListener,
public BKSingleSliderListener,
public Timer
{
public:
    
    TempoPreparationEditor(BKAudioProcessor&, BKItemGraph* theGraph);
    ~TempoPreparationEditor(){};
    
    void timerCallback() override;
    
    void update(void) override;
    
    void fillSelectCB(void);

private:
    float lastPeriodMultiplier;
    
    void bkComboBoxDidChange (ComboBox* box) override;
    void bkButtonClicked (Button* b) override;
    void BKEditableComboBoxChanged(String name, BKEditableComboBox* cb) override;
    void BKSingleSliderValueChanged(String name, double val) override;
    void BKRangeSliderValueChanged(String name, double minval, double maxval) override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TempoPreparationEditor)
    
};

class TempoModificationEditor :
public TempoViewController,
public BKEditableComboBoxListener,
public BKRangeSliderListener,
public BKSingleSliderListener,
private Timer
{
public:
    
    TempoModificationEditor(BKAudioProcessor&, BKItemGraph* theGraph);
    ~TempoModificationEditor(){};
    
    void update(void) override;
    void updateModification(void);
    
    void fillSelectCB(void);
    
    void timerCallback(void) override {};
private:
    void bkComboBoxDidChange (ComboBox* box) override;
    void bkButtonClicked (Button* b) override;
    void BKEditableComboBoxChanged(String name, BKEditableComboBox* cb) override;
    void BKSingleSliderValueChanged(String name, double val) override;
    void BKRangeSliderValueChanged(String name, double minval, double maxval) override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TempoModificationEditor)
    
};
