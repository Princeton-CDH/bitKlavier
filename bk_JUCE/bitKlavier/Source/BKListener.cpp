/*
  ==============================================================================

    BKListener.cpp
    Created: 31 Jan 2017 3:46:11pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "BKListener.h"

BKListener::BKListener():
shouldChange(false)
{
    
}

BKListener::~BKListener()
{
    
}

void BKListener::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
    bkComboBoxDidChange(comboBoxThatHasChanged);
}

void BKListener::textEditorFocusLost(TextEditor& tf)
{
    if (shouldChange)
    {
        bkTextFieldDidChange(tf);
        shouldChange = false;
    }
}

void BKListener::textEditorReturnKeyPressed(TextEditor& tf)
{
    if (shouldChange)
    {
        bkTextFieldDidChange(tf);
        shouldChange = false;
    }
}

void BKListener::textEditorEscapeKeyPressed(TextEditor& tf)
{
    if (shouldChange)
    {
        bkTextFieldDidChange(tf);
        shouldChange = false;
    }
}

void BKListener::textEditorTextChanged(TextEditor& tf)
{
    shouldChange = true;
}

void BKListener::buttonClicked (Button* b)
{
    bkButtonClicked(b);
}

void BKListener::actionListenerCallback (const String& message)
{
    bkMessageReceived(message);
}
