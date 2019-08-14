/*
  ==============================================================================

    DirectViewController.h
    Created: 7 Jun 2017 1:42:55pm
    Author:  Daniel Trueman

  ==============================================================================
*/

#pragma once

#include "BKViewController.h"

//==============================================================================

class DirectViewController :
public BKViewController
#if JUCE_IOS
, public WantsBigOne::Listener
#endif
{
public:
    
    DirectViewController(BKAudioProcessor&, BKItemGraph* theGraph);
    ~DirectViewController()
    {
        setLookAndFeel(nullptr);
    };
    
    BKEditableComboBox selectCB;
    
    std::unique_ptr<BKStackedSlider> transpositionSlider;
    std::unique_ptr<BKSingleSlider> gainSlider;
    std::unique_ptr<BKSingleSlider> resonanceGainSlider;
    std::unique_ptr<BKSingleSlider> hammerGainSlider;
    
    std::unique_ptr<BKADSRSlider> ADSRSlider;
    
    void paint (Graphics&) override;
    void resized() override;
    
    void setShowADSR(bool newval);
    
    virtual void update(void) {};
    
#if JUCE_IOS
    void iWantTheBigOne(TextEditor*, String name) override;
#endif
    
    void displayTab(int tab) override;
    void displayShared(void) override;
    void invisible(void) override;
    
private:
    
    bool showADSR;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DirectViewController)
};

class DirectPreparationEditor :
public DirectViewController,
public BKEditableComboBoxListener,
public BKSingleSlider::Listener,
public BKStackedSlider::Listener,
public BKADSRSlider::Listener
{
public:
    
    DirectPreparationEditor(BKAudioProcessor&, BKItemGraph* theGraph);
    ~DirectPreparationEditor() {};
    
    void update(void) override;
    
    void bkMessageReceived (const String& message) override;
    void bkComboBoxDidChange (ComboBox* box) override;
    
    void buttonClicked (Button* b) override;
    void BKEditableComboBoxChanged(String name, BKEditableComboBox* cb) override;
    void BKSingleSliderValueChanged(BKSingleSlider* slider, String name, double val) override;
    
    void BKStackedSliderValueChanged(String name, Array<float> val) override;
    void BKADSRSliderValueChanged(String name, int attack, int decay, float sustain, int release) override;
    void BKADSRButtonStateChanged(String name, bool mod, bool state) override;
    
    void closeSubWindow();
    
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
public BKStackedSlider::Listener,
public BKADSRSlider::Listener
{
public:
    
    DirectModificationEditor(BKAudioProcessor&, BKItemGraph* theGraph);
    ~DirectModificationEditor()
    {
        setLookAndFeel(nullptr);
    };
    
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

