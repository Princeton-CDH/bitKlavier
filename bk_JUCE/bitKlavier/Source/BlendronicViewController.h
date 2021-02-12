/*
  ==============================================================================

    BlendronicViewController.h
    Created: 6 Sep 2019 3:15:24pm
    Author:  Matthew Wang

  ==============================================================================
*/

#pragma once

#include "BKViewController.h"

class BlendronicViewController :
public BKMultiSlider::Listener,
public BKViewController
#if JUCE_IOS
, public WantsBigOne::Listener
#endif
{
public:
    BlendronicViewController(BKAudioProcessor&, BKItemGraph* theGraph);
    ~BlendronicViewController()
    {
        alternateMod.setLookAndFeel(nullptr);
        setLookAndFeel(nullptr);
//        smoothModeSelectCB.setLookAndFeel(nullptr);
//        clearModeSelectCB.setLookAndFeel(nullptr);
//        openModeSelectCB.setLookAndFeel(nullptr);
        for (int i=0; i<=TargetTypeBlendronicOpenCloseOutput-TargetTypeBlendronicPatternSync; i++)
        {
            targetControlCBs[i]->setLookAndFeel(nullptr);
        }
    };
    
    void paint (Graphics&) override;
    void resized() override;
    
    virtual void update(void) {};
    
#if JUCE_IOS
    void iWantTheBigOne(TextEditor*, String name) override;
#endif
protected:

    OwnedArray<BKMultiSlider> paramSliders;
    
    /*
    virtual void multiSliderDidChange(String name, int whichSlider, Array<float> values) = 0;
    //virtual void multiSlidersDidChange(String name, Array<Array<float>> values) = 0;
    virtual void multiSlidersDidChange(String name, Array<Array<float>> values, Array<bool> states) = 0;
    
    inline void multiSliderValueChanged(String name, int whichSlider, Array<float> values) override
    {
        multiSliderDidChange(name, whichSlider, values);
    }
    
   */
    virtual void multiSliderDidChange(String name, int whichSlider, Array<float> values) = 0;
    virtual void multiSlidersDidChange(String name, Array<Array<float>> values, Array<bool> states) = 0;
    
    inline void multiSliderValueChanged(String name, int whichSlider, Array<float> values) override
    {
        multiSliderDidChange(name, whichSlider, values);
    }
    
    inline void multiSliderAllValuesChanged(String name, Array<Array<float>> values, Array<bool> states) override
    {
        multiSlidersDidChange(name, values, states);
    }
    
    BKEditableComboBox selectCB;
    
    OwnedArray<BKComboBox> targetControlCBs;
    OwnedArray<BKLabel> targetControlCBLabels;
    GroupComponent targetControlsGroup;
    
    BlendronicDisplay delayLineDisplay;
    
    BKButtonAndMenuLAF comboBoxRightJustifyLAF;
    
    std::unique_ptr<BKSingleSlider> gainSlider;
    
    std::unique_ptr<BKSingleSlider> bufferSizeSlider;
    
    ToggleButton alternateMod;
    
    void displayTab(int tab) override;
    void displayShared(void) override;
    void invisible(void) override;
    
private:
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BlendronicViewController)

};

class BlendronicPreparationEditor :
public BlendronicViewController,
public BKEditableComboBoxListener,
public BKSingleSlider::Listener,
public BKRangeSlider::Listener,
//public SliderListener,
public Timer
{
public:
    BlendronicPreparationEditor(BKAudioProcessor&, BKItemGraph* theGraph);
    ~BlendronicPreparationEditor() { stopTimer(); };
    
    
    void update(void) override;
    
    void fillSelectCB(int last, int current);
    
    void timerCallback() override;
    
    static void actionButtonCallback(int action, BlendronicPreparationEditor*);
    
    void closeSubWindow();
    
    int addPreparation(void);
    int duplicatePreparation(void);
    void setCurrentId(int Id);
    void deleteCurrent(void);
    
private:
    
    void bkMessageReceived (const String& message) override;
    void bkComboBoxDidChange (ComboBox* box) override;
    void bkTextFieldDidChange (TextEditor&) override {};
    void buttonClicked (Button* b) override;
    void BKEditableComboBoxChanged(String name, BKEditableComboBox* cb) override;
    void BKSingleSliderValueChanged(BKSingleSlider* slider, String name, double val) override;
    void BKRangeSliderValueChanged(String name, double minval, double maxval) override;
    
    void multiSliderDidChange(String name, int whichSlider, Array<float> values) override;
    //void multiSlidersDidChange(String name, Array<Array<float>> values) override;
    void multiSlidersDidChange(String name, Array<Array<float>> values, Array<bool> states) override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BlendronicPreparationEditor)
    
};

class BlendronicModificationEditor :
public BlendronicViewController,
public BKEditableComboBoxListener,
public BKSingleSlider::Listener,
public BKRangeSlider::Listener,
//public SliderListener,
public Timer
{
public:
    BlendronicModificationEditor(BKAudioProcessor&, BKItemGraph* theGraph);
    ~BlendronicModificationEditor() {};
    
    
    void update(void) override;
    
    void updateModification(void);
    
    void bkMessageReceived (const String& message) override;
    void bkComboBoxDidChange (ComboBox* box) override;
    void bkTextFieldDidChange (TextEditor&) override {};
    void buttonClicked (Button* b) override;
    void BKEditableComboBoxChanged(String name, BKEditableComboBox* cb) override;
    void BKSingleSliderValueChanged(BKSingleSlider* slider, String name, double val) override;
    void BKRangeSliderValueChanged(String name, double minval, double maxval) override;
    
    void multiSliderDidChange(String name, int whichSlider, Array<float> values) override;
    //void multiSlidersDidChange(String name, Array<Array<float>> values) override;
    void multiSlidersDidChange(String name, Array<Array<float>> values, Array<bool> states) override;
    
    void fillSelectCB(int last, int current);
//    void fillSmoothModeSelectCB(void);
//    void fillSyncModeSelectCB(void);
//    void fillClearModeSelectCB(void);
//    void fillOpenModeSelectCB(void);
//    void fillCloseModeSelectCB(void);
    
    void greyOutAllComponents();
    void highlightModedComponents();
    
    void timerCallback() override;
    
    static void actionButtonCallback(int action, BlendronicModificationEditor*);
    
    int addPreparation(void);
    int duplicatePreparation(void);
    void setCurrentId(int Id);
    void deleteCurrent(void);
    
private:
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BlendronicModificationEditor)
    
};

