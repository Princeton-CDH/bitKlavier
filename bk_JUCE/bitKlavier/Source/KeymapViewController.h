/*
  ==============================================================================

    KeymapViewController.h
    Created: 9 Dec 2016 11:57:38am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef KEYMAPVIEWCONTROLLER_H_INCLUDED
#define KEYMAPVIEWCONTROLLER_H_INCLUDED

#include "BKViewController.h"

#include "BKKeyboard.h"
#include "BKKeyboardState.h"

#include "VelocityCurveGraph.h"


//==============================================================================
/*
*/
class KeymapViewController :
public BKViewController,
public BKKeymapKeyboardStateListener,
public BKEditableComboBoxListener,
public Slider::Listener,
public BKSingleSlider::Listener,
public Timer
#if JUCE_IOS
, 
public WantsBigOne::Listener
#endif
{
public:
    KeymapViewController(BKAudioProcessor&, BKItemGraph* theGraph);
    ~KeymapViewController();

    void paint (Graphics&) override;
    void resized() override;
    
    void reset(void);
    
    void update(void);
    
    void fillSelectCB(int last, int current);
    void fillMidiInputSelectCB(void);
    
    ToggleButton* getMidiEditToggle(void) { return &midiEditToggle; }

    static void actionButtonCallback(int action, KeymapViewController*);
    static void midiInputSelectCallback(int result, KeymapViewController* vc);
    static void keysMenuCallback(int result, KeymapViewController* vc);
    static void harmonizerMenuCallback(int result, KeymapViewController* vc);
    
    int addKeymap(void);
    int duplicateKeymap(void);
    void setCurrentId(int Id);
    void deleteCurrent(void);
    
    void keymapUpdated(TextEditor& tf);
    void updateTargets();
    
    void bkTextFieldDidChange       (TextEditor&)           override;

#if JUCE_IOS
    void iWantTheBigOne(TextEditor*, String name) override;
#endif
    
private:
    
    bool hasBigOne;
    
    juce::AudioPluginInstance::WrapperType wrapperType;
    
    BKLabel     keymapSelectL;
    BKEditableComboBox  selectCB;
    
    ToggleButton invertOnOffToggle;
    ToggleButton midiEditToggle;
    ToggleButton harMidiEditToggle;
    ToggleButton harArrayMidiEditToggle;
    ToggleButton toggleKeysToggle;
    
    BKLabel     keymapL;
    BKTextEditor  keymapTF;
    
    BKKeymapKeyboardState keyboardState;
//    std::unique_ptr<Component> keyboardComponent;
    std::unique_ptr<BKKeymapKeyboardComponent> keyboard;
    BKTextButton keyboardValsTextFieldOpen;
    
    TextButton      midiInputSelectButton;
    TextButton      keysButton;
    TextButton      clearButton;
    BKComboBox      keysCB;

    TextButton      harmonizerMenuButton;

    ToggleButton endKeystrokesToggle;
    ToggleButton ignoreSustainToggle;
    ToggleButton sustainPedalKeysToggle;

    //stuff for trap/mirror/harmonizer tab - TRT

    BKLabel harKeyboardLabel;
    BKKeymapKeyboardState harKeyboardState;
//    std::unique_ptr<Component> harKeyboardComponent;
    std::unique_ptr<BKKeymapKeyboardComponent> harKeyboard;
    BKTextButton harKeyboardAllValsTextFieldOpen;

    BKTextEditor harArrayKeymapTF;
    BKTextEditor harAllKeymapTF;

    BKLabel harArrayKeyboardLabel;
    BKKeymapKeyboardState harArrayKeyboardState;
//    std::unique_ptr<Component> harArrayKeyboardComponent;/
    std::unique_ptr<BKKeymapKeyboardComponent> harArrayKeyboard;
    BKTextButton harArrayKeyboardValsTextFieldOpen;

    //ToggleButton enableHarmonizerToggle; // functionality currently commented out, probably not necessary

    std::unique_ptr<BKSingleSlider> harPostTranspositionSlider;
    std::unique_ptr<BKSingleSlider> harPreTranspositionSlider;
    void sliderValueChanged(Slider* slider) override;
    void BKSingleSliderValueChanged(BKSingleSlider* slider, String name, double val) override; 
    //BKButtonAndMenuLAF transpositionLaf;

    //int harKey; //for rendering harmonizer array stuff for now

    //end
    
    //======================================================================================
    // Velocity Curving UI
    
    //std::unique_ptr<BKSingleSlider> rangeExtendSlider;
    std::unique_ptr<BKSingleSlider> asym_kSlider;
    std::unique_ptr<BKSingleSlider> sym_kSlider;
    std::unique_ptr<BKSingleSlider> scaleSlider;
    std::unique_ptr<BKSingleSlider> offsetSlider;
    ToggleButton velocityInvertToggle;
    
    VelocityCurveGraph velocityCurveGraph;
    
    // This is just for the border
    /* Not sure that it's necessary to have a border around this bit of UI, but these parameters are a bit obscure without something to clarify what they are actually changing. */
    GroupComponent velocityCurveGroup;
    
    //======================================================================================

    bool selectType;
    
    PopupMenu getKeysMenu(void);
    PopupMenu getPitchClassMenu(int offset);
    PopupMenu getMidiInputSelectMenu(void);
    PopupMenu getHarmonizerMenu(Array<int> keyHarmonization);
    
    OwnedArray<ToggleButton> targetControlTBs;
    GroupComponent resonanceTBGroup;
    GroupComponent directTBGroup;
    GroupComponent synchronicTBGroup;
    GroupComponent nostalgicTBGroup;
    GroupComponent blendronicTBGroup;
    GroupComponent tuningTBGroup;
    GroupComponent tempoTBGroup;
    
    void handleKeymapNoteOn (BKKeymapKeyboardState* source, int midiNoteNumber) override;
    void handleKeymapNoteOff (BKKeymapKeyboardState* source, int midiNoteNumber) override;
    void handleKeymapNoteToggled (BKKeymapKeyboardState* source, int midiNoteNumber) override;
    void BKEditableComboBoxChanged(String name, BKEditableComboBox* cb) override;
    void textEditorFocusLost(TextEditor& textEditor) override;
    void textEditorEscapeKeyPressed (TextEditor& textEditor) override;
    void textEditorReturnKeyPressed(TextEditor& textEditor) override;
    void textEditorTextChanged(TextEditor& tf) override;
    
    void bkMessageReceived          (const String& message) override;
    
    void bkComboBoxDidChange        (ComboBox* box)         override;
    void bkButtonClicked            (Button* b)             override;
    
    void timerCallback() override;
    
    bool focusLostByEscapeKey;
    
    bool needsOctaveSlider;

    void displayTab(int tab) override;
    void displayShared(void) override;
    void invisible(void) override;

    void harmonizerKeymapUpdated();
    void harmonizerArrayKeymapUpdated();


#if JUCE_IOS
    Slider octaveSlider;
    BKButtonAndMenuLAF laf;

    Slider harOctaveSlider;
    //BKButtonAndMenuLAF harlaf;
    Slider harArrayOctaveSlider;
#endif
    
    int minKey, maxKey;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KeymapViewController)
};


#endif  // KEYMAPVIEWCONTROLLER_H_INCLUDED
