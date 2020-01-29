/*
  ==============================================================================

    SynchronicViewController.h
    Created: 21 Apr 2017 11:17:47pm
    Author:  Daniel Trueman

  ==============================================================================
*/

#ifndef SYNCHRONICVIEWCONTROLLER2_H_INCLUDED
#define SYNCHRONICVIEWCONTROLLER2_H_INCLUDED

#include "BKViewController.h"

class SynchronicPage : public Component
{
public:
    ~SynchronicPage()
    {
        
    }
    
    SynchronicPage()
    {
        
    }
    
    void resized(void) override
    {
        
    }
    
    void paint (Graphics&) override
    {
        
    }
    
private:
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SynchronicPage)
    
    
};

class SynchronicViewController :
public BKMultiSlider::Listener,
public BKViewController
#if JUCE_IOS
, public WantsBigOne::Listener
#endif
{
public:
    SynchronicViewController(BKAudioProcessor&, BKItemGraph* theGraph);
    ~SynchronicViewController()
    {
        setLookAndFeel(nullptr);
        releaseVelocitySetsSynchronicToggle.setLookAndFeel(nullptr);
        onOffSelectCB.setLookAndFeel(nullptr);
        for (int i=0; i<=TargetTypeSynchronicRotate-TargetTypeSynchronicPatternSync; i++)
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
    
    //OwnedArray<BKLabel> synchronicL;
    //OwnedArray<BKTextField> synchronicTF;
    //OwnedArray<BKTextField> modSynchronicTF;
    
    OwnedArray<BKMultiSlider> paramSliders;
    OwnedArray<BKADSRSlider> envelopeSliders;
    
    virtual void multiSliderDidChange(String name, int whichSlider, Array<float> values) = 0;
    virtual void multiSlidersDidChange(String name, Array<Array<float>> values) = 0;
    
    inline void multiSliderValueChanged(String name, int whichSlider, Array<float> values) override
    {
        multiSliderDidChange(name, whichSlider, values);
    }
    
    inline void multiSliderAllValuesChanged(String name, Array<Array<float>> values) override
    {
        multiSlidersDidChange(name, values);
    }
    
    BKEditableComboBox selectCB;
    BKComboBox midiOutputSelectCB;
    BKComboBox modeSelectCB;
    BKComboBox onOffSelectCB;
    
    OwnedArray<BKComboBox> targetControlCBs;
    OwnedArray<BKLabel> targetControlCBLabels;
    GroupComponent targetControlsGroup;

    ToggleButton offsetParamStartToggle;
    ToggleButton releaseVelocitySetsSynchronicToggle;
    std::unique_ptr<BKSingleSlider> howManySlider;
    std::unique_ptr<BKSingleSlider> clusterThreshSlider;
    std::unique_ptr<BKRangeSlider> clusterMinMaxSlider;
    std::unique_ptr<BKSingleSlider> clusterCapSlider;
    
    std::unique_ptr<BKRangeSlider> holdTimeMinMaxSlider;
    std::unique_ptr<BKRangeSlider> velocityMinMaxSlider;

    std::unique_ptr<BKSingleSlider> gainSlider;
    std::unique_ptr<BKSingleSlider> numClusterSlider;
    
    BKLabel modeLabel;
    BKLabel onOffLabel;
    
    BKButtonAndMenuLAF comboBoxRightJustifyLAF;
    
    void setShowADSR(String name, bool newval);
    int visibleADSR;
    
    BKLabel envelopeName;
    
    void displayTab(int tab) override;
    void displayShared(void) override;
    void invisible(void) override;
    
private:
    
    bool showADSR;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SynchronicViewController)
};

class SynchronicPreparationEditor :
public SynchronicViewController,
public BKSingleSlider::Listener,
public BKRangeSlider::Listener,
public BKEditableComboBoxListener,
public BKADSRSlider::Listener,
public Timer
{
public:
    SynchronicPreparationEditor(BKAudioProcessor&, BKItemGraph* theGraph);
    ~SynchronicPreparationEditor() {}
    
    void update(void) override;
    
    void update(NotificationType notify);
    
    void timerCallback() override;
    
    void fillSelectCB(int last, int current);
    void fillMidiOutputSelectCB(void);
    void fillModeSelectCB(void);
    
    static void actionButtonCallback(int action, SynchronicPreparationEditor*);
    
    void closeSubWindow();
    
    int addPreparation(void);
    int duplicatePreparation(void);
    void setCurrentId(int Id);
    void deleteCurrent(void);
    
    
    
private:

    void bkTextFieldDidChange       (TextEditor&)           override;
    void bkMessageReceived          (const String& message) override;
    void bkComboBoxDidChange        (ComboBox* box)         override;
    void BKSingleSliderValueChanged(BKSingleSlider* slider, String name, double val) override;
    void BKRangeSliderValueChanged(String name, double minval, double maxval) override;
    void BKEditableComboBoxChanged(String name, BKEditableComboBox* cb) override;
    void BKADSRSliderValueChanged(String name, int attack, int decay, float sustain, int release) override;
    void BKADSRButtonStateChanged(String name, bool shift, bool state) override;
    
    void buttonClicked (Button* b) override;
    
    void multiSliderDidChange(String name, int whichSlider, Array<float> values) override;
    void multiSlidersDidChange(String name, Array<Array<float>> values) override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SynchronicPreparationEditor)
};



class SynchronicModificationEditor :
public SynchronicViewController,
public BKSingleSlider::Listener,
public BKRangeSlider::Listener,
public BKEditableComboBoxListener,
public BKADSRSlider::Listener,
public Timer
{
public:
    SynchronicModificationEditor(BKAudioProcessor&, BKItemGraph* theGraph);
    ~SynchronicModificationEditor()
    {
        setLookAndFeel(nullptr);
    };
    
    void update(void) override;
    
    void update(NotificationType notify);
    void updateModification(void);
    
    void timerCallback() override;
    
    void fillSelectCB(int last, int current);
    void fillMidiOutputSelectCB(void);
    void fillModeSelectCB(void);
    
    static void actionButtonCallback(int action, SynchronicModificationEditor*);
    
    int addPreparation(void);
    int duplicatePreparation(void);
    void setCurrentId(int Id);
    void deleteCurrent(void);
    
private:
    
    void bkTextFieldDidChange       (TextEditor&)           override;
    void bkMessageReceived          (const String& message) override;
    void bkComboBoxDidChange        (ComboBox* box)         override;
    void BKSingleSliderValueChanged(BKSingleSlider* slider, String name, double val) override;
    void BKRangeSliderValueChanged(String name, double minval, double maxval) override;
    void BKEditableComboBoxChanged(String name, BKEditableComboBox* cb) override;
    void BKADSRSliderValueChanged(String name, int attack, int decay, float sustain, int release) override;
    void BKADSRButtonStateChanged(String name, bool mod, bool state) override;
    void buttonClicked (Button* b) override;
    
    void multiSliderDidChange(String name, int whichSlider, Array<float> values) override;
    void multiSlidersDidChange(String name, Array<Array<float>> values) override;
    
    void greyOutAllComponents();
    void highlightModedComponents();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SynchronicModificationEditor)
};



#endif  // SYNCHRONICVIEWCONTROLLER2_H_INCLUDED
