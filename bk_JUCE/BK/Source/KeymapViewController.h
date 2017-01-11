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

#include "BKViewController.h"

#include "BKLabel.h"

#include "BKTextField.h"

//==============================================================================
/*
*/
class KeymapViewController    : public BKViewController, public ActionListener, public MidiKeyboardStateListener
{
public:
    KeymapViewController(BKAudioProcessor&);
    ~KeymapViewController();

    void paint (Graphics&) override;
    void resized() override;

private:
    int currentKeymapId;
    
    BKAudioProcessor& processor;
    
    OwnedArray<BKLabel> keymapL;
    OwnedArray<BKTextField> keymapTF;
    
    MidiKeyboardState keyboardState;
    
    Component *keyboardComponent;
    
    /** Called when one of the MidiKeyboardState's keys is pressed.
     
     This will be called synchronously when the state is either processing a
     buffer in its MidiKeyboardState::processNextMidiBuffer() method, or
     when a note is being played with its MidiKeyboardState::noteOn() method.
     
     Note that this callback could happen from an audio callback thread, so be
     careful not to block, and avoid any UI activity in the callback.
     */
    void handleNoteOn (MidiKeyboardState* source,
                               int midiChannel, int midiNoteNumber, float velocity) override;
    
    /** Called when one of the MidiKeyboardState's keys is released.
     
     This will be called synchronously when the state is either processing a
     buffer in its MidiKeyboardState::processNextMidiBuffer() method, or
     when a note is being played with its MidiKeyboardState::noteOff() method.
     
     Note that this callback could happen from an audio callback thread, so be
     careful not to block, and avoid any UI activity in the callback.
     */
    void handleNoteOff (MidiKeyboardState* source,
                                int midiChannel, int midiNoteNumber, float velocity) override;
    
    
    void updateFields(int keymapId);
    
    void textFieldDidChange(TextEditor&) override;
    
    void actionListenerCallback (const String& message) override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KeymapViewController)
};


#endif  // KEYMAPVIEWCONTROLLER_H_INCLUDED
