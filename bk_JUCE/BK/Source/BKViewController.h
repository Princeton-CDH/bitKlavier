/*
  ==============================================================================

    BKViewController.h
    Created: 20 Nov 2016 11:06:14am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef BKVIEWCONTROLLER_H_INCLUDED
#define BKVIEWCONTROLLER_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

#include "PluginProcessor.h"

#include "BKView.h"

//==============================================================================
/*
*/
class BKViewController    : public BKView,
                            public TextEditor::Listener,
                            public ComboBox::Listener,
                            public ActionListener,
                            public ActionBroadcaster,
                            public TextButton::Listener
{
public:
    
    
  
    BKViewController();
    ~BKViewController();
    
    void paint (Graphics&) override;
    void resized() override;
    
private:
    // TextEditor input parsing
    virtual void bkTextFieldDidChange   (TextEditor&)           = 0;
    virtual void bkComboBoxDidChange    (ComboBox*)             = 0;
    virtual void bkButtonClicked        (Button* b)             = 0;
    virtual void bkMessageReceived      (const String& message) = 0;
    
    void textEditorTextChanged      (TextEditor&) override;
    void textEditorFocusLost        (TextEditor&) override;
    void textEditorReturnKeyPressed (TextEditor&) override;
    void textEditorEscapeKeyPressed (TextEditor&) override;
    
    void comboBoxChanged            (ComboBox* comboBoxThatHasChanged) override;
    
    void buttonClicked              (Button* b) override;
    
    void actionListenerCallback     (const String& message) override;
    
    bool shouldChange;
    
    
    
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKViewController)
};


#endif  // BKCONTROLLER_H_INCLUDED
