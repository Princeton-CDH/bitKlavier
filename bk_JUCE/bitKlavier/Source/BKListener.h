/*
  ==============================================================================

    BKListener.h
    Created: 31 Jan 2017 3:46:11pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef BKLISTENER_H_INCLUDED
#define BKLISTENER_H_INCLUDED

#include "BKUtilities.h"


class BKListener :
public TextEditor::Listener,
public ComboBox::Listener,
public ActionListener,
public ActionBroadcaster,
public TextButton::Listener
{
public:
    BKListener();
    ~BKListener();
    
private:
    // TextEditor input parsing
    virtual void bkTextFieldDidChange   (TextEditor&)           {};
    virtual void bkComboBoxDidChange    (ComboBox*)             {};
    virtual void bkButtonClicked        (Button* b)             {};
    virtual void bkMessageReceived      (const String& message) {};
    
    void textEditorTextChanged      (TextEditor&) override;
    void textEditorFocusLost        (TextEditor&) override;
    void textEditorReturnKeyPressed (TextEditor&) override;
    void textEditorEscapeKeyPressed (TextEditor&) override;
    
    void comboBoxChanged            (ComboBox* comboBoxThatHasChanged) override;
    
    void buttonClicked              (Button* b) override;
    
    void actionListenerCallback     (const String& message) override;
    
    bool shouldChange;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKListener)
};

    
#endif  // BKLISTENER_H_INCLUDED
