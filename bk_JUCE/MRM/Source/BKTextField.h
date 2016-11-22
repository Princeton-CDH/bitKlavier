/*
  ==============================================================================

    BKTextField.h
    Created: 15 Nov 2016 4:19:33pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef BKTEXTFIELD_H_INCLUDED
#define BKTEXTFIELD_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class BKTextField    : public TextEditor, public TextEditor::Listener
{
public:
    BKTextField()
    {
        setSize(300,20);
        setColour(TextEditor::backgroundColourId, Colours::lightgoldenrodyellow);
        setColour(TextEditor::textColourId, Colours::black);
#if TEXT_CHANGE_INTERNAL
        addListener(this);
#endif
        lookAndFeelChanged();
    }

    ~BKTextField()
    {
        
    }
    
#if TEXT_CHANGE_INTERNAL
    
    void textEditorFocusLost(TextEditor& tf)
    {
        if (shouldChange)
        {
            textFieldDidChange(tf);
            shouldChange = false;
        }
    }
    
    void textEditorReturnKeyPressed(TextEditor& tf)
    {
        if (shouldChange)
        {
            textFieldDidChange(tf);
            shouldChange = false;
        }
    }
    
    void textEditorEscapeKeyPressed(TextEditor& tf)
    {
        if (shouldChange)
        {
            textFieldDidChange(tf);
            shouldChange = false;
        }
    }
    
    
    void textEditorTextChanged(TextEditor& tf)
    {
        shouldChange = true;
    }
    
    
    class JUCE_API  BKListener
    {
    public:
        /** Destructor. */
        virtual ~BKListener()  {}
        
        /** Called when the user inputs text. **/
        virtual void bkTextFieldDidChange(BKTextField&) {}
    };
    
    /** Registers a listener to be told when things happen to the text.
     @see removeListener
     */
    void addListener (BKListener* newBKListener);
    
    /** Deregisters a listener.
     @see addListener
     */
    void removeListener (BKListener* bkListenerToRemove);
    
    
#endif
    
private:
#if TEXT_CHANGE_INTERNAL
    bool shouldChange;
#endif
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKTextField)
    
};


#endif  // BKTEXTFIELD_H_INCLUDED
