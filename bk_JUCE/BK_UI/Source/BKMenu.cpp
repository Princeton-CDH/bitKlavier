/*
  ==============================================================================

    BKMenu.cpp
    Created: 18 May 2017 10:08:44pm
    Author:  Daniel Trueman

  ==============================================================================
*/

#include "BKMenu.h"

void BKEditableComboBox::mouseDoubleClick (const MouseEvent &event)
{

    addAndMakeVisible(nameEditor);
    nameEditor.setBounds(getLocalBounds());
    nameEditor.grabKeyboardFocus();
}


void BKEditableComboBox::textEditorReturnKeyPressed(TextEditor& textEditor)
{
    
    listeners.call(&BKEditableComboBoxListener::BKEditableComboBoxChanged,
                   textEditor.getText(),
                   getSelectedItemIndex());
    
    removeChildComponent(&nameEditor);
};
