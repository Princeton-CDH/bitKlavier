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

    nameEditor.setAlpha(1.);
    //nameEditor.setVisible(true);
    nameEditor.toFront(true);
    nameEditor.setBounds(getLocalBounds());
    nameEditor.grabKeyboardFocus();
    nameEditor.setText(getItemText(getSelectedItemIndex()));
}


void BKEditableComboBox::textEditorReturnKeyPressed(TextEditor& textEditor)
{
    
    nameEditor.toBack();
    //nameEditor.setVisible(false);
    nameEditor.setAlpha(0.);
    hidePopup();
    
    listeners.call(&BKEditableComboBoxListener::BKEditableComboBoxChanged,
                   textEditor.getText(),
                   this);
};
