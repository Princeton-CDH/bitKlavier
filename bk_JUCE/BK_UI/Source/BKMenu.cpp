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
    //nameEditor.setVisible(true);
    //nameEditor.setOpaque(true);
    //nameEditor.toFront(true);
    nameEditor.setBounds(getLocalBounds());
    nameEditor.grabKeyboardFocus();
    nameEditor.setText(getItemText(getSelectedItemIndex()));
}


void BKEditableComboBox::textEditorReturnKeyPressed(TextEditor& textEditor)
{
    
    DBG("BKEditableComboBox return key pressed");
    
    
    listeners.call(&BKEditableComboBoxListener::BKEditableComboBoxChanged,
                   textEditor.getText(),
                   this);
     
    
    //nameEditor.setVisible(false);
    removeChildComponent(&nameEditor);
};
