/*
  ==============================================================================

    TuningViewController.h
    Created: 10 Jun 2017 11:24:22am
    Author:  Daniel Trueman

  ==============================================================================
*/

#pragma once

#include "BKViewController.h"

class TuningViewController :
public BKViewController
#if JUCE_IOS
, public WantsBigOne::Listener
#endif
{
public:
    
    TuningViewController(BKAudioProcessor&, BKItemGraph* theGraph);
    ~TuningViewController()
    {
        setLookAndFeel(nullptr);
    };
    
    void paint (Graphics&) override;
    void resized() override;
    
    virtual void update(void) {};
    
#if JUCE_IOS
    void iWantTheBigOne(TextEditor*, String name) override;
#endif
    
protected:
    //basics
    BKEditableComboBox selectCB;
    
    BKLabel scaleLabel;
    BKComboBox scaleCB;
    
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
    
    BKTextButton A1reset;

    Array<float> absoluteOffsets;   //for entire keyboard; up to 128 vals
    Array<float> customOffsets;     //for custom tuning; 12 vals
    
    BKLabel lastNote;
    BKLabel lastInterval;
    float lastNoteTuningSave = -.1;
    
    BKLabel currentFundamental;
    
    BKAbsoluteKeyboardSlider absoluteKeyboard;
    BKCircularKeyboardSlider customKeyboard; 
    
    ImageComponent iconImageComponent;
    BKButtonAndMenuLAF buttonsAndMenusLAF;
    
    BKComboBox nToneRootCB;
    BKComboBox nToneRootOctaveCB;
    ScopedPointer<BKSingleSlider> nToneSemitoneWidthSlider;

    //other overrides
    
    void fillTuningCB(void);
    void fillFundamentalCB(void);
    
    void updateComponentVisibility();
    
    int customIndex; //index of Custom tuning in combobox
private:
    
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TuningViewController)
    
};

class TuningPreparationEditor :
public TuningViewController,
public BKEditableComboBoxListener,
public BKSingleSlider::Listener,
//public BKKeyboardSlider::Listener,
public BKCircularKeyboardSlider::Listener,
public BKAbsoluteKeyboardSlider::Listener,
public Timer
{
public:
    
    TuningPreparationEditor(BKAudioProcessor&, BKItemGraph* theGraph);
    ~TuningPreparationEditor() {setLookAndFeel(nullptr);};
    
    void timerCallback() override;
    
    void update(void) override;
    
    void fillSelectCB(int last, int current);
    
    static void actionButtonCallback(int action, TuningPreparationEditor*);
    
    int addPreparation(void);
    int duplicatePreparation(void);
    void setCurrentId(int Id);
    void deleteCurrent(void);
    
private:
    
    void bkComboBoxDidChange (ComboBox* box) override;
    void buttonClicked (Button* b) override;
    void BKEditableComboBoxChanged(String name, BKEditableComboBox* cb) override;
    void BKSingleSliderValueChanged(String name, double val) override;
    void keyboardSliderChanged(String name, Array<float> values) override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TuningPreparationEditor)
    
};

class TuningModificationEditor :
public TuningViewController,
public BKEditableComboBoxListener,
public BKSingleSlider::Listener,
//public BKKeyboardSlider::Listener,
public BKCircularKeyboardSlider::Listener,
public BKAbsoluteKeyboardSlider::Listener
{
public:
    
    TuningModificationEditor(BKAudioProcessor&, BKItemGraph* theGraph);
    ~TuningModificationEditor()
    {
        setLookAndFeel(nullptr);
    };
    
    void update(void) override;
    void updateModification(void);
    
    void fillSelectCB(int last, int current);
    
    static void actionButtonCallback(int action, TuningModificationEditor*);
    
    int addPreparation(void);
    int duplicatePreparation(void);
    void setCurrentId(int Id);
    void deleteCurrent(void);
    
private:
    
    void bkComboBoxDidChange (ComboBox* box) override;
    void buttonClicked (Button* b) override;
    void BKEditableComboBoxChanged(String name, BKEditableComboBox* cb) override;
    void BKSingleSliderValueChanged(String name, double val) override;
    void keyboardSliderChanged(String name, Array<float> values) override;
    
    void greyOutAllComponents();
    void highlightModedComponents();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TuningModificationEditor)
    
};
