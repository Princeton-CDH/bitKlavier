/*
  ==============================================================================

    DirectViewController.h
    Created: 7 Jun 2017 1:42:55pm
    Author:  Daniel Trueman

  ==============================================================================
*/

#pragma once

#include "BKViewController.h"


class DirectViewController :
public BKViewController
{
public:
    
    DirectViewController(BKAudioProcessor&, BKItemGraph* theGraph);
    ~DirectViewController() {};
    
    BKEditableComboBox selectCB;
    
    ScopedPointer<BKStackedSlider> transpositionSlider;
    ScopedPointer<BKSingleSlider> gainSlider;
    ScopedPointer<BKSingleSlider> resonanceGainSlider;
    ScopedPointer<BKSingleSlider> hammerGainSlider;
    
    void paint (Graphics&) override;
    void resized() override;
    
    void update(void) ;
    

private:

};

class DirectPreparationEditor :
public DirectViewController,
public BKEditableComboBoxListener,
public BKSingleSliderListener,
public BKStackedSliderListener
{
public:
    
    DirectPreparationEditor(BKAudioProcessor&, BKItemGraph* theGraph);
    ~DirectPreparationEditor() {};
    
    void update(void);
    
    void bkMessageReceived (const String& message) override;
    void bkComboBoxDidChange (ComboBox* box) override;
    void bkTextFieldDidChange (TextEditor&) override {};
    void bkButtonClicked (Button* b) override { };
    void BKEditableComboBoxChanged(String name, BKEditableComboBox* cb) override;
    void BKSingleSliderValueChanged(String name, double val) override;
    void BKStackedSliderValueChanged(String name, Array<float> val) override;
    
    void fillSelectCB(void);
    
private:
    
};

class DirectModificationEditor :
public DirectViewController,
public BKEditableComboBoxListener,
public BKSingleSliderListener,
public BKStackedSliderListener
{
public:
    
    DirectModificationEditor(BKAudioProcessor&, BKItemGraph* theGraph);
    ~DirectModificationEditor() {};
    
    void update(void);
    
    void bkMessageReceived (const String& message) override;
    void bkComboBoxDidChange (ComboBox* box) override;
    void bkTextFieldDidChange (TextEditor&) override {};
    void bkButtonClicked (Button* b) override { };
    void BKEditableComboBoxChanged(String name, BKEditableComboBox* cb) override;
    void BKSingleSliderValueChanged(String name, double val) override;
    void BKStackedSliderValueChanged(String name, Array<float> val) override;
    
    void fillSelectCB(void);
    
private:
    
};

