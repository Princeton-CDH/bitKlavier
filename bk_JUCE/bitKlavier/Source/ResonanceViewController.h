/*
  ==============================================================================

    ResonanceViewController.h
    Created: 24 May 2021 10:48:25pm
    Author:  Theodore R Trevisan

  ==============================================================================
*/

#pragma once

#include "BKViewController.h"

class ResonanceViewController :
    public BKViewController
#if JUCE_IOS
    , public WantsBigOne::Listener
#endif
{
public:
    ResonanceViewController(BKAudioProcessor&, BKItemGraph* theGraph);
    ~ResonanceViewController()
    {
        alternateMod.setLookAndFeel(nullptr);
        setLookAndFeel(nullptr);
    };

    BKButtonAndMenuLAF comboBoxRightJustifyLAF;

    BKEditableComboBox selectCB;
    BKComboBox lengthModeSelectCB;

    std::unique_ptr<BKRangeSlider> startTimeSlider;
    std::unique_ptr<BKSingleSlider> blendGainSlider;
    std::unique_ptr<BKSingleSlider> defGainSlider;
    std::unique_ptr<BKSingleSlider> maxSympStringsSlider;
    
    OwnedArray<BKSubSlider> gainsArray;
    OwnedArray<BKSubSlider> offsetsArray;
    
    BKAbsoluteKeyboardSlider gainsKeyboard;
    BKAbsoluteKeyboardSlider offsetsKeyboard;
    
    
    
    //Array<bool> isActive;
    //bool isActive[52];

    //BKLabel lastNote;

    BKKeymapKeyboardState resonanceKeyboardState;
    BKKeymapKeyboardState fundamentalKeyboardState;
    BKKeymapKeyboardState addKeyboardState;
    BKKeymapKeyboardState ringKeyboardState;
    
    OwnedArray<ToggleButton> fundamentalButtons;
    
    std::unique_ptr<BKKeymapKeyboardComponent> closestKeyboard;
    std::unique_ptr<BKKeymapKeyboardComponent> fundamentalKeyboard;
    std::unique_ptr<BKKeymapKeyboardComponent> addKeyboard;
    std::unique_ptr<BKKeymapKeyboardComponent> ringKeyboard;
    //BKAbsoluteKeyboardSlider closestKeyboard;
    //BKAbsoluteKeyboardSlider fundamentalKeyboard;

    std::unique_ptr<BKADSRSlider> ADSRSlider;

    /* currently not needed but could be useful later
    std::unique_ptr<BKSingleSlider> clusterMinSlider;
    std::unique_ptr<BKSingleSlider> clusterThresholdSlider;
    std::unique_ptr<BKRangeSlider> holdTimeMinMaxSlider;
    std::unique_ptr<BKRangeSlider> velocityMinMaxSlider;
    */
    
    ToggleButton alternateMod;
    
    int NUM_KEYS;

    BKLabel ADSRLabel;
    BKLabel closestKeyLabel;
    BKLabel fundamentalLabel;
    BKLabel gainsLabel;
    BKLabel offsetsLabel;
    BKLabel addLabel;
    BKLabel ringLabel;

    void paint(Graphics&) override;
    void resized() override;

    //void setShowADSR(String name, bool newval); probably not necessary?

    virtual void update(void) {};

//#if JUCE_IOS
//    void iWantTheBigOne(TextEditor*, String name) override;
//#endif

    void displayTab(int tab) override;
    void displayShared(void) override;
    void invisible(void) override;

private:

    bool showADSR;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ResonanceViewController)


};

class ResonancePreparationEditor :
    public ResonanceViewController,
    public BKEditableComboBoxListener,
    public BKSingleSlider::Listener,
    public BKStackedSlider::Listener,
    public BKADSRSlider::Listener,
    public BKRangeSlider::Listener,
    public BKAbsoluteKeyboardSlider::Listener,
    private BKKeymapKeyboardStateListener,
//    public Slider::Listener,
    //public SliderListener,
    public Timer
{
public:
    ResonancePreparationEditor(BKAudioProcessor&, BKItemGraph* theGraph);
    ~ResonancePreparationEditor() { stopTimer(); };


    void update(void) override;

    void fillSelectCB(int last, int current);

    void timerCallback() override;

    static void actionButtonCallback(int action, ResonancePreparationEditor*);

    void closeSubWindow();

    int addPreparation(void);
    int duplicatePreparation(void);
    void setCurrentId(int Id);
    void deleteCurrent(void);


private:

    void bkMessageReceived(const String& message) override;
    void bkComboBoxDidChange(ComboBox* box) override;
    void bkTextFieldDidChange(TextEditor&) override {};
    void buttonClicked(Button* b) override;
    void BKEditableComboBoxChanged(String name, BKEditableComboBox* cb) override;
    void BKSingleSliderValueChanged(BKSingleSlider* slider, String name, double val) override;
    void BKStackedSliderValueChanged(String name, Array<float> val) override;
    void BKADSRSliderValueChanged(String name, int attack, int decay, float sustain, int release) override;
    void BKADSRButtonStateChanged(String name, bool mod, bool state) override;
    void BKRangeSliderValueChanged(String name, double minval, double maxval) override;
    void keyboardSliderChanged(String name, Array<float> values) override;
    void handleKeymapNoteToggled(BKKeymapKeyboardState* source, int midiNoteNumber) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ResonancePreparationEditor)

};



//won't do anything with this for now - just have this as a skeleton for someone doing modification later

class ResonanceModificationEditor :
    public ResonanceViewController,
    public BKEditableComboBoxListener,
    public BKSingleSlider::Listener,
    public BKStackedSlider::Listener,
    public BKADSRSlider::Listener,
    public BKRangeSlider::Listener,
    public BKAbsoluteKeyboardSlider::Listener,
    private BKKeymapKeyboardStateListener,
    //public Slider::Listener,
    public Timer
{
public:
    ResonanceModificationEditor(BKAudioProcessor&, BKItemGraph* theGraph);
    ~ResonanceModificationEditor() {};


    void update(void) override;

    void updateModification(void);

    void bkMessageReceived(const String& message) override;
    void bkComboBoxDidChange(ComboBox* box) override;
    void bkTextFieldDidChange(TextEditor&) override {};
    void buttonClicked(Button* b) override;
    void BKEditableComboBoxChanged(String name, BKEditableComboBox* cb) override;
    void BKSingleSliderValueChanged(BKSingleSlider* slider, String name, double val) override;
    void BKStackedSliderValueChanged(String name, Array<float> val) override;
    void BKADSRSliderValueChanged(String name, int attack, int decay, float sustain, int release) override;
    void BKADSRButtonStateChanged(String name, bool mod, bool state) override;
    void BKRangeSliderValueChanged(String name, double minval, double maxval) override;
    void handleKeymapNoteToggled(BKKeymapKeyboardState* source, int midiNoteNumber) override;
    void keyboardSliderChanged(String name, Array<float> values) override;

    void fillSelectCB(int last, int current);
    void greyOutAllComponents();
    void highlightModedComponents();

    void timerCallback() override;

    static void actionButtonCallback(int action, ResonanceModificationEditor*);

    int addPreparation(void);
    int duplicatePreparation(void);
    void setCurrentId(int Id);
    void deleteCurrent(void);

private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ResonanceModificationEditor)

};
