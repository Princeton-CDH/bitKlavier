/*
  ==============================================================================

    NostalgicViewController.h
    Created: 24 May 2017 7:13:23pm
    Author:  Daniel Trueman

  ==============================================================================
*/


#pragma once

#include "BKViewController.h"

class NostalgicViewController :
public BKViewController
{
public:
    NostalgicViewController(BKAudioProcessor&, BKItemGraph* theGraph);
    ~NostalgicViewController() {};
    
    BKWaveDistanceUndertowSlider nDisplaySlider;

    BKEditableComboBox selectCB;
    BKComboBox lengthModeSelectCB;
    
    ScopedPointer<BKSingleSlider> lengthMultiplierSlider;
    ScopedPointer<BKSingleSlider> beatsToSkipSlider;
    ScopedPointer<BKSingleSlider> gainSlider;
    
    ScopedPointer<BKStackedSlider> transpositionSlider;
    
    void paint (Graphics&) override;
    void resized() override;
    
    virtual void update(void) {};
    
    void fillModeSelectCB(void);
    
    TextButton clearModsButton;
    
    /*
    void close() override
    {
        
        DBG("====> nostalgic close() <====");
        selectCB.exitModalState(-1);
        selectCB.getRootMenu()->dismissAllActiveMenus();
        //selectCB.hidePopup();
        lengthModeSelectCB.exitModalState(-1);
        lengthModeSelectCB.getRootMenu()->dismissAllActiveMenus();
        //lengthModeSelectCB.hidePopup();
        
        processor.updateState->setCurrentDisplay(DisplayNil);
    }
     */
    
    
    
private:
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NostalgicViewController)
    

};

class NostalgicPreparationEditor :
public NostalgicViewController,
public BKWaveDistanceUndertowSliderListener,
public BKEditableComboBoxListener,
public BKSingleSliderListener,
public BKStackedSliderListener,
//public SliderListener,
public Timer
{
public:
    NostalgicPreparationEditor(BKAudioProcessor&, BKItemGraph* theGraph);
    ~NostalgicPreparationEditor() {};
    
    
    void update(void) override;
    
    void fillSelectCB(int last, int current);
    
    void timerCallback() override;
    
    
private:
    
    void bkMessageReceived (const String& message) override;
    void bkComboBoxDidChange (ComboBox* box) override;
    void bkTextFieldDidChange (TextEditor&) override {};
    void buttonClicked (Button* b) override;
    void BKEditableComboBoxChanged(String name, BKEditableComboBox* cb) override;
    void BKSingleSliderValueChanged(String name, double val) override;
    void BKWaveDistanceUndertowSliderValueChanged(String name, double wavedist, double undertow) override;
    void BKStackedSliderValueChanged(String name, Array<float> val) override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NostalgicPreparationEditor)
    
};

class NostalgicModificationEditor :
public NostalgicViewController,
public BKWaveDistanceUndertowSliderListener,
public BKEditableComboBoxListener,
public BKSingleSliderListener,
public BKStackedSliderListener,
//public SliderListener,
public Timer
{
public:
    NostalgicModificationEditor(BKAudioProcessor&, BKItemGraph* theGraph);
    ~NostalgicModificationEditor() {};
    
    
    void update(void) override;
    
    void updateModification(void);
    
    void bkMessageReceived (const String& message) override;
    void bkComboBoxDidChange (ComboBox* box) override;
    void bkTextFieldDidChange (TextEditor&) override {};
    void buttonClicked (Button* b) override;
    void BKEditableComboBoxChanged(String name, BKEditableComboBox* cb) override;
    void BKSingleSliderValueChanged(String name, double val) override;
    void BKWaveDistanceUndertowSliderValueChanged(String name, double wavedist, double undertow) override;
    void BKStackedSliderValueChanged(String name, Array<float> val) override;
    
    void fillSelectCB(int last, int current);
    void greyOutAllComponents();
    void highlightModedComponents();
    
    void timerCallback() override;
    
    
private:
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NostalgicModificationEditor)
    
};
