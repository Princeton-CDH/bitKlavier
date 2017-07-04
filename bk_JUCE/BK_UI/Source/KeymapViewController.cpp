/*
  ==============================================================================

    KeymapViewController.cpp
    Created: 9 Dec 2016 11:57:38am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "KeymapViewController.h"

//==============================================================================
KeymapViewController::KeymapViewController(BKAudioProcessor& p, BKItemGraph* theGraph):
BKViewController(p, theGraph)
{
    setLookAndFeel(&buttonsAndMenusLAF);
    
    iconImageComponent.setImage(ImageCache::getFromMemory(BinaryData::keymap_icon_png, BinaryData::keymap_icon_pngSize));
    iconImageComponent.setImagePlacement(RectanglePlacement(juce::RectanglePlacement::stretchToFit));
    iconImageComponent.setAlpha(0.095);
    addAndMakeVisible(iconImageComponent);
    
    selectCB.setName("Keymap");
    selectCB.addSeparator();
    selectCB.addListener(this);
    selectCB.setSelectedItemIndex(0);
    addAndMakeVisible(selectCB);
    
    addAndMakeVisible(keymapTF);
    keymapTF.addListener(this);
    keymapTF.setName("KeymapMidi");
    keymapTF.setMultiLine(true);
    
    // Keyboard
    addAndMakeVisible (keyboardComponent = new BKKeymapKeyboardComponent (keyboardState,
                                                                 BKKeymapKeyboardComponent::horizontalKeyboard));
    
    keyboard = (BKKeymapKeyboardComponent*)keyboardComponent.get();
    keyboard->setScrollButtonsVisible(true);
    keyboard->setAvailableRange(21, 108);
    keyboard->setAllowDrag(true);
    keyboard->setOctaveForMiddleC(4);
    keyboardState.addListener(this);
    
    keyboardValsTextFieldOpen.setName("KSLIDERTXTEDITALLBUTTON");
    keyboardValsTextFieldOpen.addListener(this);
    keyboardValsTextFieldOpen.setButtonText("edit all");
    addAndMakeVisible(keyboardValsTextFieldOpen);
    keymapTF.setVisible(false);
    keymapTF.toBack();
    
    addAndMakeVisible(hideOrShow);
    hideOrShow.setName("hideOrShow");
    hideOrShow.addListener(this);
    hideOrShow.setButtonText(" X ");
    
    fillKeymapSelectCB();

    update();
}

KeymapViewController::~KeymapViewController()
{
}

void KeymapViewController::reset(void)
{
    fillKeymapSelectCB();
    update();
}

void KeymapViewController::paint (Graphics& g)
{
    g.fillAll(Colours::black);
}

void KeymapViewController::resized()
{
    Rectangle<int> area (getLocalBounds());
    
    float paddingScalarX = (float)(getTopLevelComponent()->getWidth() - gMainComponentMinWidth) / (gMainComponentWidth - gMainComponentMinWidth);
    float paddingScalarY = (float)(getTopLevelComponent()->getHeight() - gMainComponentMinHeight) / (gMainComponentHeight - gMainComponentMinHeight);
    
    iconImageComponent.setBounds(area);
    area.reduce(10 * paddingScalarX + 4, 10 * paddingScalarY + 4);
    
    float keyboardHeight = 86; // + 36 * paddingScalarY;
    Rectangle<int> keyboardRow = area.removeFromBottom(keyboardHeight);
    keyboardRow.reduce(gXSpacing, 0);
    keyboard->setBounds(keyboardRow);
    keymapTF.setBounds(keyboardRow);
    
    area.removeFromBottom(gYSpacing);
    Rectangle<int> textButtonSlab = area.removeFromBottom(gComponentComboBoxHeight);
    textButtonSlab.removeFromLeft(gXSpacing);
    keyboardValsTextFieldOpen.setBounds(textButtonSlab.removeFromLeft(75));
    
    Rectangle<int> leftColumn = area.removeFromLeft(area.getWidth() * 0.5);
    Rectangle<int> comboBoxSlice = leftColumn.removeFromTop(gComponentComboBoxHeight);
    comboBoxSlice.removeFromRight(4 + 2.*gPaddingConst * paddingScalarX);
    comboBoxSlice.removeFromLeft(gXSpacing);
    hideOrShow.setBounds(comboBoxSlice.removeFromLeft(gComponentComboBoxHeight));
    comboBoxSlice.removeFromLeft(gXSpacing);
    selectCB.setBounds(comboBoxSlice.removeFromLeft(comboBoxSlice.getWidth() / 2.));
    
}

void KeymapViewController::bkComboBoxDidChange        (ComboBox* box)
{
    String name = box->getName();
    
    if (name == "Keymap")
    {
        // Remove current from list of actives
        processor.updateState->removeActive(PreparationTypeKeymap, processor.updateState->currentKeymapId);
        
        // Set new current
        processor.updateState->currentKeymapId = processor.gallery->getIdFromIndex(PreparationTypeKeymap, box->getSelectedItemIndex());
        
        // Add new current from list of actives
        processor.updateState->addActive(PreparationTypeKeymap, processor.updateState->currentKeymapId);
        
        processor.updateState->idDidChange = true;
        
        if (processor.updateState->currentKeymapId == selectCB.getNumItems()-1) // New Keymap
        {
            processor.gallery->addKeymap();
        }

        fillKeymapSelectCB();
        
        update();
    }
}

void KeymapViewController::fillKeymapSelectCB(void)
{
    selectCB.clear(dontSendNotification);
    
    Array<int> index = processor.gallery->getIndexList(PreparationTypeKeymap);
    
    for (int i = 0; i < index.size(); i++)
    {
        int Id = index[i];
        String name = processor.gallery->getKeymap(Id)->getName();
        if (name != String::empty)  selectCB.addItem(name, i+1);
        else                        selectCB.addItem(String(i+1), i+1);
        
        selectCB.setItemEnabled(i+1, true);
        if (processor.updateState->isActive(PreparationTypeKeymap, Id) &&
            (Id != processor.updateState->currentKeymapId))
        {
            selectCB.setItemEnabled(i+1, false);
        }
    }
    
    selectCB.addItem("New keymap...", index.size()+1);
    
    int currentId = processor.updateState->currentKeymapId;
    
    selectCB.setSelectedItemIndex(processor.gallery->getIndexFromId(PreparationTypeKeymap, currentId), NotificationType::dontSendNotification);
    
}

void KeymapViewController::bkButtonClicked (Button* b)
{
    if (b == &hideOrShow)
    {
        processor.updateState->setCurrentDisplay(DisplayNil);
    }
    else if(b->getName() == keyboardValsTextFieldOpen.getName())
    {
        keymapTF.setVisible(true);
        keymapTF.toFront(true);
        focusLostByEscapeKey = false;
    }
}


void KeymapViewController::BKEditableComboBoxChanged(String name, BKEditableComboBox* cb)
{
    processor.gallery->getDirect(processor.updateState->currentDirectId)->setName(name);
}

void KeymapViewController::bkTextFieldDidChange(TextEditor& tf)
{

    String name = tf.getName();
    
    if (name == "KeymapMidi")
    {
        keymapUpdated(tf);
    }
    else
    {
        DBG("Unregistered text field entered input.");
    }
}

void KeymapViewController::keymapUpdated(TextEditor& tf)
{
    String text = tf.getText();
    Array<int> keys = keymapStringToIntArray(text);
    
    keymapTF.setText(intArrayToString(keys));
    
    // get old keys to send to update
    Array<int> oldKeys = processor.gallery->getKeymap(processor.updateState->currentKeymapId)->keys();
    
    processor.gallery->setKeymap(processor.updateState->currentKeymapId, keys);
    
    BKKeymapKeyboardComponent* keyboard =  (BKKeymapKeyboardComponent*)keyboardComponent.get();
    
    keyboard->setKeysInKeymap(keys);
    
    theGraph->update(PreparationTypeKeymap, processor.updateState->currentKeymapId);
    
    keymapTF.setVisible(false);
    keymapTF.toBack();

}

void KeymapViewController::textEditorFocusLost(TextEditor& tf)
{
    DBG("textEditorFocusLost");
    if(!focusLostByEscapeKey) {
        keymapUpdated(tf);
    }
}

void KeymapViewController::textEditorEscapeKeyPressed (TextEditor& textEditor)
{
    focusLostByEscapeKey = true;
    keymapTF.setVisible(false);
    keymapTF.toBack();
    unfocusAllComponents();
}

void KeymapViewController::update(void)
{
    if (processor.updateState->currentKeymapId < 0) return;
    
    Keymap::Ptr km = processor.gallery->getKeymap(processor.updateState->currentKeymapId);
    
    if (km != nullptr)
    {
        fillKeymapSelectCB();
        
        selectCB.setSelectedItemIndex(processor.gallery->getIndexFromId(PreparationTypeKeymap, processor.updateState->currentKeymapId), dontSendNotification);
        
        keymapTF.setText( intArrayToString(km->keys()));
        
        keymapNameTF.setText(km->getName());
        
        BKKeymapKeyboardComponent* keyboard =  (BKKeymapKeyboardComponent*)keyboardComponent.get();
        
        keyboard->setKeysInKeymap(km->keys());
    }
    
}

void KeymapViewController::bkMessageReceived (const String& message)
{
    if (message == "keymap/update")
    {
        //processor.updateState->currentKeymapId = processor.currentPiano->currentPMap->getKeymapId();
        
        //update();
    }
}


void KeymapViewController::handleKeymapNoteOn (BKKeymapKeyboardState* source, int midiNoteNumber)
{
    
}

void KeymapViewController::handleKeymapNoteOff (BKKeymapKeyboardState* source, int midiNoteNumber)
{
    
}

void KeymapViewController::handleKeymapNoteToggled (BKKeymapKeyboardState* source, int midiNoteNumber)
{
    
    Array<int> oldKeys = processor.gallery->getKeymap(processor.updateState->currentKeymapId)->keys();
    
    processor.gallery->getKeymap(processor.updateState->currentKeymapId)->toggleNote(midiNoteNumber);
    
    update();
    
    BKKeymapKeyboardComponent* keyboard =  (BKKeymapKeyboardComponent*)keyboardComponent.get();
    
    keyboard->setKeysInKeymap(processor.gallery->getKeymap(processor.updateState->currentKeymapId)->keys());
    
    theGraph->update(PreparationTypeKeymap, processor.updateState->currentKeymapId);
    
}



