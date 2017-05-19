/*
  ==============================================================================

    BKMenu.cpp
    Created: 18 May 2017 10:08:44pm
    Author:  Daniel Trueman

  ==============================================================================
*/

#include "BKMenu.h"


void BKEditableComboBox::showModifyPopupMenu()
{
    PopupMenu m;
    m.setLookAndFeel (&getLookAndFeel());
    m.addItem (1, translate ("edit item name"), true, false);
    m.addSeparator();
    
    m.showMenuAsync (PopupMenu::Options(),
                     ModalCallbackFunction::forComponent (controlClickCallback, this));
}

void BKEditableComboBox::controlClickCallback (const int result, BKEditableComboBox* cbox)
{

    switch (result)
    {
        case 1:   DBG("editing name"); break;
            
        default:  break;
    }
}

void BKEditableComboBox::mouseDoubleClick (const MouseEvent &event)
{
    /*
    if(event.mouseWasClicked())
    {
        if(event.mods.isCtrlDown())
        {
            showModifyPopupMenu();
        }
    }
    else showPopup();
     */
    DBG("dbl click editing name");
    //nameEditor.setVisible(true);
    //nameEditor.toFront(true);
    addAndMakeVisible(nameEditor);
    nameEditor.setBounds(getLocalBounds());
    nameEditor.grabKeyboardFocus();
}

/*
void BKEditableComboBox::resized()
{
    //nameEditor.setVisible(false);
    //nameEditor.setBounds(getLocalBounds());
}
 */

void BKEditableComboBox::textEditorReturnKeyPressed(TextEditor& textEditor)
{
    DBG("returned from naming box");
    
    listeners.call(&BKEditableComboBoxListener::BKEditableComboBoxChanged,
                   textEditor.getText(),
                   getSelectedItemIndex());
    
    removeChildComponent(&nameEditor);
    //nameEditor.setVisible(false);
    //nameEditor.toBack();
};
