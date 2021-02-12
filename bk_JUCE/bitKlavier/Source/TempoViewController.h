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
#if JUCE_IOS
, public WantsBigOne::Listener
#endif
{
public:
    
    TempoViewController(BKAudioProcessor&, BKItemGraph* theGraph);
    ~TempoViewController()
    {
        alternateMod.setLookAndFeel(nullptr);
        setLookAndFeel(nullptr);
    };
    
    void paint (Graphics&) override;
    void resized() override;
    
    virtual void update(void){};
    
#if JUCE_IOS
    void iWantTheBigOne(TextEditor*, String name) override;
#endif
    
protected:
    BKLabel selectLabel;
    BKEditableComboBox selectCB;
    
    BKLabel modeLabel;
    BKComboBox modeCB;
    
    std::unique_ptr<BKSingleSlider> tempoSlider;
    std::unique_ptr<BKSingleSlider> subSlider;
    
    std::unique_ptr<BKSingleSlider> AT1HistorySlider;
    std::unique_ptr<BKSingleSlider> AT1SubdivisionsSlider;
    std::unique_ptr<BKRangeSlider> AT1MinMaxSlider; //need to have "MinAlwaysLessThanMax" mode
    
    BKLabel A1ModeLabel;
    BKComboBox A1ModeCB;
    
    BKLabel A1AdaptedTempo;
    BKLabel A1AdaptedPeriodMultiplier;
    
    BKTextButton A1reset;
    
    BKLabel attachKeymap;
    
    ToggleButton alternateMod;
    
    void fillModeCB(void);
    void fillA1ModeCB(void);
    
    void updateComponentVisibility();
    
private:
    
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TempoViewController)
};

class TempoPreparationEditor :
public TempoViewController,
public BKEditableComboBoxListener,
public BKRangeSlider::Listener,
public BKSingleSlider::Listener,
public Timer
{
public:
    
    TempoPreparationEditor(BKAudioProcessor&, BKItemGraph* theGraph);
    ~TempoPreparationEditor() { stopTimer(); };
    
    void timerCallback() override;
    
    void update(void) override;
    
    void fillSelectCB(int last, int current);
    
    static void actionButtonCallback(int action, TempoPreparationEditor*);
    
    int addPreparation(void);
    int duplicatePreparation(void);
    void setCurrentId(int Id);
    void deleteCurrent(void);

private:
    float lastPeriodMultiplier;
    
    void bkComboBoxDidChange (ComboBox* box) override;
    void buttonClicked (Button* b) override;
    void BKEditableComboBoxChanged(String name, BKEditableComboBox* cb) override;
    void BKSingleSliderValueChanged(BKSingleSlider* slider, String name, double val) override;
    void BKRangeSliderValueChanged(String name, double minval, double maxval) override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TempoPreparationEditor)
    
};

class TempoModificationEditor :
public TempoViewController,
public BKEditableComboBoxListener,
public BKRangeSlider::Listener,
public BKSingleSlider::Listener,
private Timer
{
public:
    
    TempoModificationEditor(BKAudioProcessor&, BKItemGraph* theGraph);
    ~TempoModificationEditor()
    {
        setLookAndFeel(nullptr);
    };
    
    void update(void) override;
    void updateModification(void);
    
    void fillSelectCB(int last, int current);
    
    void timerCallback(void) override
    {

    }
    
    static void actionButtonCallback(int action, TempoModificationEditor*);
    
    int addPreparation(void);
    int duplicatePreparation(void);
    void setCurrentId(int Id);
    void deleteCurrent(void);
    
private:
    void bkComboBoxDidChange (ComboBox* box) override;
    void buttonClicked (Button* b) override;
    void BKEditableComboBoxChanged(String name, BKEditableComboBox* cb) override;
    void BKSingleSliderValueChanged(BKSingleSlider* slider, String name, double val) override;
    void BKRangeSliderValueChanged(String name, double minval, double maxval) override;
    
    void greyOutAllComponents();
    void highlightModedComponents();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TempoModificationEditor)
    
};
