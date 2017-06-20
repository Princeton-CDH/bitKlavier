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
    lastItemId = getSelectedId();
    focusLostByEscapeKey = false;
    nameEditor.setAlpha(1.);
    nameEditor.toFront(true);
    nameEditor.setBounds(getLocalBounds());
    nameEditor.grabKeyboardFocus();
    nameEditor.setText(getItemText(indexOfItemId(lastItemId)));
    Range<int> highlightRange(0, nameEditor.getText().length());
    nameEditor.setHighlightedRegion(highlightRange);
}



void BKEditableComboBox::textEditorReturnKeyPressed(TextEditor& textEditor)
{
    
    nameEditor.toBack();
    nameEditor.setAlpha(0.);

    hidePopup();
    
    listeners.call(&BKEditableComboBoxListener::BKEditableComboBoxChanged,
                   textEditor.getText(),
                   this);
    
    changeItemText(lastItemId, textEditor.getText());
    setSelectedId(lastItemId, dontSendNotification);
}

void BKEditableComboBox::textEditorEscapeKeyPressed (TextEditor &)
{
    focusLostByEscapeKey = true;
    nameEditor.toBack();
    nameEditor.setAlpha(0.);
    nameEditor.focusLost(focusChangedDirectly);
}

void BKEditableComboBox::textEditorFocusLost(TextEditor& textEditor)
{
    nameEditor.toBack();
    nameEditor.setAlpha(0.);
    
    if(!focusLostByEscapeKey)
    {
        focusLostByEscapeKey = false;
        
        listeners.call(&BKEditableComboBoxListener::BKEditableComboBoxChanged,
                       textEditor.getText(),
                       this);
        
        changeItemText(lastItemId, textEditor.getText());
        setSelectedId(lastItemId, dontSendNotification);
    }

     
};
