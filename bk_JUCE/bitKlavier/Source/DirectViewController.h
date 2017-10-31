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
    
    virtual void update(void) {};
    
private:


};

class DirectPreparationEditor :
public DirectViewController,
public BKEditableComboBoxListener,
public BKSingleSlider::Listener,
public BKStackedSlider::Listener
{
public:
    
    DirectPreparationEditor(BKAudioProcessor&, BKItemGraph* theGraph);
    ~DirectPreparationEditor() {};
    
    void update(void) override;
    
    void bkMessageReceived (const String& message) override;
    void bkComboBoxDidChange (ComboBox* box) override;
    void bkTextFieldDidChange (TextEditor&) override {};
    void buttonClicked (Button* b) override;
    void BKEditableComboBoxChanged(String name, BKEditableComboBox* cb) override;
    void BKSingleSliderValueChanged(String name, double val) override;
    
    void BKStackedSliderValueChanged(String name, Array<float> val) override;
    
    void fillSelectCB(int last, int current);
    
    static void actionButtonCallback(int action, DirectPreparationEditor*);

    
    int addPreparation(void);
    int duplicatePreparation(void);
    void setCurrentId(int Id);
    void deleteCurrent(void);
    
private:
    
    
    
};

class DirectModificationEditor :
public DirectViewController,
public BKEditableComboBoxListener,
public BKSingleSlider::Listener,
public BKStackedSlider::Listener
{
public:
    
    DirectModificationEditor(BKAudioProcessor&, BKItemGraph* theGraph);
    ~DirectModificationEditor() {};
    
    void update(void) override;
    void updateModification(void);
    
    void bkMessageReceived (const String& message) override;
    void bkComboBoxDidChange (ComboBox* box) override;
    void bkTextFieldDidChange (TextEditor&) override {};
    void buttonClicked (Button* b) override;
    void BKEditableComboBoxChanged(String name, BKEditableComboBox* cb) override;
    void BKSingleSliderValueChanged(String name, double val) override;
    void BKStackedSliderValueChanged(String name, Array<float> val) override;
    
    void greyOutAllComponents();
    void highlightModedComponents();
    
    void fillSelectCB(int last, int current);
    
    static void actionButtonCallback(int action, DirectModificationEditor*);
    
    int addPreparation(void);
    int duplicatePreparation(void);
    void setCurrentId(int Id);
    void deleteCurrent(void);
    
private:
    
};

