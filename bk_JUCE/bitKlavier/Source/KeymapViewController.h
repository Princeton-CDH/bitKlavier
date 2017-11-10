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
public BKEditableComboBoxListener
{
public:
    KeymapViewController(BKAudioProcessor&, BKItemGraph* theGraph);
    ~KeymapViewController();

    void paint (Graphics&) override;
    void resized() override;
    
    void reset(void);
    
    void update(void);
    
    void fillSelectCB(int last, int current);
    
    static void actionButtonCallback(int action, KeymapViewController*);
    
    int addKeymap(void);
    int duplicateKeymap(void);
    void setCurrentId(int Id);
    void deleteCurrent(void);
    
    void keymapUpdated(TextEditor& tf);
    
    void bkTextFieldDidChange       (TextEditor&)           override;

private:
    
    BKLabel     keymapSelectL;
    BKEditableComboBox  selectCB;
    
    BKLabel     keymapL;
    BKTextEditor  keymapTF;
    
    BKKeymapKeyboardState keyboardState;
    ScopedPointer<Component> keyboardComponent;
    BKKeymapKeyboardComponent* keyboard;
    TextButton keyboardValsTextFieldOpen;
    
    void handleKeymapNoteOn (BKKeymapKeyboardState* source, int midiNoteNumber) override;
    void handleKeymapNoteOff (BKKeymapKeyboardState* source, int midiNoteNumber) override;
    void handleKeymapNoteToggled (BKKeymapKeyboardState* source, int midiNoteNumber) override;
    void BKEditableComboBoxChanged(String name, BKEditableComboBox* cb) override;
    void textEditorFocusLost(TextEditor& textEditor) override;
    void textEditorEscapeKeyPressed (TextEditor& textEditor) override;
    
    
    void bkMessageReceived          (const String& message) override;
    
    void bkComboBoxDidChange        (ComboBox* box)         override;
    void bkButtonClicked            (Button* b)             override;
    
    bool focusLostByEscapeKey;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KeymapViewController)
};


#endif  // KEYMAPVIEWCONTROLLER_H_INCLUDED
