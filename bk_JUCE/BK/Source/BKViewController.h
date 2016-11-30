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
                            public TextEditor::Listener
{
public:
    BKViewController();
    ~BKViewController();
    
    void paint (Graphics&) override;
    void resized() override;
    
private:
    // TextEditor input parsing
    virtual void textFieldDidChange(TextEditor&) = 0;
    
    void textEditorTextChanged(TextEditor&) override;
    void textEditorFocusLost(TextEditor&) override;
    void textEditorReturnKeyPressed(TextEditor&) override;
    void textEditorEscapeKeyPressed(TextEditor&) override;
    
    bool shouldChange;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKViewController)
};


#endif  // BKCONTROLLER_H_INCLUDED
