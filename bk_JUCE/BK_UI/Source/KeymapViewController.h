/*
  ==============================================================================

    KeymapViewController.h
    Created: 9 Dec 2016 11:57:38am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef KEYMAPVIEWCONTROLLER_H_INCLUDED
#define KEYMAPVIEWCONTROLLER_H_INCLUDED

#include "BKUtilities.h"

#include "PluginProcessor.h"

#include "BKListener.h"
#include "BKComponent.h"


#include "BKKeyboard.h"
#include "BKKeyboardState.h"

//==============================================================================
/*
*/
class KeymapViewController    : public BKComponent, public BKListener, public BKKeymapKeyboardStateListener
{
public:
    KeymapViewController(BKAudioProcessor&);
    ~KeymapViewController();

    void paint (Graphics&) override;
    void resized() override;
    
    void reset(void);

private:
    int currentKeymapId;
    
    BKAudioProcessor& processor;
    
    BKLabel     keymapSelectL;
    BKComboBox  keymapSelectCB;
    
    BKLabel     keymapNameL;
    BKTextField keymapNameTF;
    
    BKLabel     keymapL;
    BKTextField keymapTF;

    BKKeymapKeyboardState keyboardState;
    
    Component *keyboardComponent;
    
    void handleKeymapNoteOn (BKKeymapKeyboardState* source, int midiNoteNumber) override;
    
    void handleKeymapNoteOff (BKKeymapKeyboardState* source, int midiNoteNumber) override;
    
    void handleKeymapNoteToggled (BKKeymapKeyboardState* source, int midiNoteNumber) override;
    
    
    void bkTextFieldDidChange       (TextEditor&)           override;
    void bkMessageReceived          (const String& message) override;
    
    void bkComboBoxDidChange        (ComboBox* box)         override;
    void bkButtonClicked            (Button* b)             override { };
    
    
    void updateFields(void);
    void fillKeymapSelectCB(void);
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KeymapViewController)
};


#endif  // KEYMAPVIEWCONTROLLER_H_INCLUDED
