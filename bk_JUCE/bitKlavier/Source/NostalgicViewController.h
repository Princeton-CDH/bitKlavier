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
#if JUCE_IOS
, public WantsBigOne::Listener
#endif
{
public:
    NostalgicViewController(BKAudioProcessor&, BKItemGraph* theGraph);
    ~NostalgicViewController()
    {
        alternateMod.setLookAndFeel(nullptr);
        setLookAndFeel(nullptr);
        for (int i=0; i<=TargetTypeNostalgicClear-TargetTypeNostalgicClear; i++)
        {
            targetControlCBs[i]->setLookAndFeel(nullptr);
        }
    };
    
    BKButtonAndMenuLAF comboBoxRightJustifyLAF;
    
    BKWaveDistanceUndertowSlider nDisplaySlider;

    BKEditableComboBox selectCB;
    BKComboBox lengthModeSelectCB;
    
    std::unique_ptr<BKSingleSlider> lengthMultiplierSlider;
    std::unique_ptr<BKSingleSlider> beatsToSkipSlider;
    std::unique_ptr<BKSingleSlider> gainSlider;
    
    std::unique_ptr<BKSingleSlider> blendronicGainSlider;
    
    std::unique_ptr<BKStackedSlider> transpositionSlider;
    ToggleButton transpUsesTuning;

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
    
    OwnedArray<BKComboBox> targetControlCBs;
    OwnedArray<BKLabel> targetControlCBLabels;
    
    ToggleButton alternateMod;
    
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
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NostalgicViewController)
    

};

class NostalgicPreparationEditor :
public NostalgicViewController,
public BKWaveDistanceUndertowSlider::Listener,
public BKEditableComboBoxListener,
public BKSingleSlider::Listener,
public BKStackedSlider::Listener,
public BKADSRSlider::Listener,
public BKRangeSlider::Listener,
//public SliderListener,
public Timer
{
public:
    NostalgicPreparationEditor(BKAudioProcessor&, BKItemGraph* theGraph);
    ~NostalgicPreparationEditor() { stopTimer(); };
    
    
    void update(void) override;
    
    void fillSelectCB(int last, int current);
    
    void timerCallback() override;
    
    static void actionButtonCallback(int action, NostalgicPreparationEditor*);
    
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
    void BKWaveDistanceUndertowSliderValueChanged(String name, double wavedist, double undertow) override;
    void BKStackedSliderValueChanged(String name, Array<float> val) override;
    void BKADSRSliderValueChanged(String name, int attack, int decay, float sustain, int release) override;
    void BKADSRButtonStateChanged(String name, bool mod, bool state) override;
    void BKRangeSliderValueChanged(String name, double minval, double maxval) override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NostalgicPreparationEditor)
    
};

class NostalgicModificationEditor :
public NostalgicViewController,
public BKWaveDistanceUndertowSlider::Listener,
public BKEditableComboBoxListener,
public BKSingleSlider::Listener,
public BKStackedSlider::Listener,
public BKADSRSlider::Listener,
public BKRangeSlider::Listener,
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
    void BKSingleSliderValueChanged(BKSingleSlider* slider, String name, double val) override;
    void BKWaveDistanceUndertowSliderValueChanged(String name, double wavedist, double undertow) override;
    void BKStackedSliderValueChanged(String name, Array<float> val) override;
    void BKADSRSliderValueChanged(String name, int attack, int decay, float sustain, int release) override;
    void BKADSRButtonStateChanged(String name, bool mod, bool state) override;
    void BKRangeSliderValueChanged(String name, double minval, double maxval) override;
    
    void fillSelectCB(int last, int current);
    void greyOutAllComponents();
    void highlightModedComponents();
    
    void timerCallback() override;
    
    static void actionButtonCallback(int action, NostalgicModificationEditor*);
    
    int addPreparation(void);
    int duplicatePreparation(void);
    void setCurrentId(int Id);
    void deleteCurrent(void);
    
private:
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NostalgicModificationEditor)
    
};
