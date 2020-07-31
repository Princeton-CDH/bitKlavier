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


//==============================================================================
/*
*/
class KeymapViewController :
public BKViewController,
public BKKeymapKeyboardStateListener,
public BKEditableComboBoxListener,
public Timer
#if JUCE_IOS
, public Slider::Listener,
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
    void fillTargetSelectCB(void);
    
    ToggleButton* getMidiEditToggle(void) { return &midiEditToggle; }

    static void actionButtonCallback(int action, KeymapViewController*);
    static void midiInputSelectCallback(int result, KeymapViewController* vc);
    static void targetsMenuCallback(int result, KeymapViewController* vc);
    static void keysMenuCallback(int result, KeymapViewController* vc);
    
    int addKeymap(void);
    int duplicateKeymap(void);
    void setCurrentId(int Id);
    void deleteCurrent(void);
    
    void keymapUpdated(TextEditor& tf);
    void hideUnconnectedTargets();
    
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
    
    BKLabel     keymapL;
    BKTextEditor  keymapTF;
    
    BKKeymapKeyboardState keyboardState;
    std::unique_ptr<Component> keyboardComponent;
    BKKeymapKeyboardComponent* keyboard;
    BKTextButton keyboardValsTextFieldOpen;
    
    TextButton      midiInputSelectButton;
    TextButton      targetsButton;
    TextButton      keysButton;
    TextButton      clearButton;
    BKComboBox      keysCB;

    ToggleButton endKeystrokesToggle;

    //stuff for trap/mirror/harmonizer tab - TRT
    BKLabel harKeymapL;
    BKTextEditor harKeymapTF;

    BKKeymapKeyboardState harKeyboardState;
    std::unique_ptr<Component> harKeyboardComponent;
    BKKeymapKeyboardComponent* harKeyboard;
    BKTextButton harKeyboardValsTextFieldOpen;

    BKLabel harArrayKeymapL;
    BKTextEditor harArrayKeymapTF;

    BKKeymapKeyboardState harArrayKeyboardState;
    std::unique_ptr<Component> harArrayKeyboardComponent;
    BKKeymapKeyboardComponent* harArrayKeyboard;
    BKTextButton harArrayKeyboardValsTextFieldOpen;

    ToggleButton enableHarmonizerToggle;

    int harKey; //for rendering harmonizer array stuff for now

    //end

    bool selectType;
    
    PopupMenu getKeysMenu(void);
    PopupMenu getPitchClassMenu(int offset);
    PopupMenu getTargetsMenu(void);
    PopupMenu getMidiInputSelectMenu(void);
    
    OwnedArray<ToggleButton> targetControlTBs;
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
    
    void updateKeymapTargets(void);
    
    bool focusLostByEscapeKey;
    
    bool needsOctaveSlider;

    void displayTab(int tab) override;
    void displayShared(void) override;
    void invisible(void) override;


#if JUCE_IOS
    Slider octaveSlider;
    void sliderValueChanged     (Slider* slider)                override;
    BKButtonAndMenuLAF laf;

    Slider harOctaveSlider;
    //BKButtonAndMenuLAF harlaf;
    Slider harArrayOctaveSlider;
#endif
    
    int minKey, maxKey;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KeymapViewController)
};


#endif  // KEYMAPVIEWCONTROLLER_H_INCLUDED
