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
public BKViewController
#if JUCE_IOS
, public WantsBigOne::Listener
#endif
{
public:
    BlendronicViewController(BKAudioProcessor&, BKItemGraph* theGraph);
    ~BlendronicViewController()
    {
        setLookAndFeel(nullptr);
    };
    
//    tempo
//    beats
//    smoothdurations
//    feedbackcoeffs
//    clickgains
//    delaymax
//    feedback gain
//    inputthresh
//    holdmin
//    holdmax
//    vel min
//    vel max
//    num voices
    
    BKEditableComboBox selectCB;
    BKComboBox lengthModeSelectCB;
    
    std::unique_ptr<BKSingleSlider> lengthMultiplierSlider;
    std::unique_ptr<BKSingleSlider> beatsToSkipSlider;
    std::unique_ptr<BKSingleSlider> gainSlider;
    
    std::unique_ptr<BKStackedSlider> transpositionSlider;
    
    std::unique_ptr<BKADSRSlider> reverseADSRSlider;
    std::unique_ptr<BKADSRSlider> undertowADSRSlider;
    
    std::unique_ptr<BKSingleSlider> clusterMinSlider;
    std::unique_ptr<BKSingleSlider> clusterThresholdSlider;
    std::unique_ptr<BKRangeSlider> holdTimeMinMaxSlider;
    std::unique_ptr<BKRangeSlider> velocityMinMaxSlider;
    
    ToggleButton    keyOnResetToggle;
    BKLabel         keyOnResetLabel;
    
    BKLabel         reverseADSRLabel;
    BKLabel         undertowADSRLabel;
    
    void paint (Graphics&) override;
    void resized() override;
    
    void setShowADSR(String name, bool newval);
    
    virtual void update(void) {};
    
    void fillModeSelectCB(void);
    
#if JUCE_IOS
    void iWantTheBigOne(TextEditor*, String name) override;
#endif
    
    void displayTab(int tab) override;
    void displayShared(void) override;
    void invisible(void) override;
    
private:
    
    bool showADSR;
    bool showReverseADSR;
    bool showUndertowADSR;
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BlendronicViewController)
    
    
};

class BlendronicPreparationEditor :
public BlendronicViewController,
public BKEditableComboBoxListener,
public BKSingleSlider::Listener,
public BKStackedSlider::Listener,
public BKADSRSlider::Listener,
public BKRangeSlider::Listener,
//public SliderListener,
public Timer
{
public:
    BlendronicPreparationEditor(BKAudioProcessor&, BKItemGraph* theGraph);
    ~BlendronicPreparationEditor() {};
    
    
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
    void BKStackedSliderValueChanged(String name, Array<float> val) override;
    void BKADSRSliderValueChanged(String name, int attack, int decay, float sustain, int release) override;
    void BKADSRButtonStateChanged(String name, bool mod, bool state) override;
    void BKRangeSliderValueChanged(String name, double minval, double maxval) override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BlendronicPreparationEditor)
    
};

class BlendronicModificationEditor :
public BlendronicViewController,
public BKEditableComboBoxListener,
public BKSingleSlider::Listener,
public BKStackedSlider::Listener,
public BKADSRSlider::Listener,
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
    void BKStackedSliderValueChanged(String name, Array<float> val) override;
    void BKADSRSliderValueChanged(String name, int attack, int decay, float sustain, int release) override;
    void BKADSRButtonStateChanged(String name, bool mod, bool state) override;
    void BKRangeSliderValueChanged(String name, double minval, double maxval) override;
    
    void fillSelectCB(int last, int current);
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

