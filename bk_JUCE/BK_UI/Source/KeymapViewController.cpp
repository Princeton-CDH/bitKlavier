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
processor(p),
theGraph(theGraph)
{
    // First row
    addAndMakeVisible(keymapSelectL);
    keymapSelectL.setName("Keymap");
    keymapSelectL.setText("Keymap", NotificationType::dontSendNotification);
    
    addAndMakeVisible(keymapSelectCB);
    keymapSelectCB.setName("Keymap");
    keymapSelectCB.addSeparator();
    keymapSelectCB.addListener(this);
    //keymapSelectCB.setEditableText(true);
    keymapSelectCB.setSelectedItemIndex(0);
    
    // Second row
    addAndMakeVisible(keymapNameL);
    keymapNameL.setName("KeymapName");
    keymapNameL.setText("KeymapName", NotificationType::dontSendNotification);
    
    addAndMakeVisible(keymapNameTF);
    keymapNameTF.addListener(this);
    keymapNameTF.setName("KeymapName");
    
    // Third row
    addAndMakeVisible(keymapL);
    keymapL.setName("KeymapMidi");
    keymapL.setText("KeymapMidi", NotificationType::dontSendNotification);
    
    addAndMakeVisible(keymapTF);
    keymapTF.addListener(this);
    keymapTF.setName("KeymapMidi");
    
    // Keyboard
    addAndMakeVisible (keyboardComponent = new BKKeymapKeyboardComponent (keyboardState,
                                                                 BKKeymapKeyboardComponent::horizontalKeyboard));
    
    
    BKKeymapKeyboardComponent* keyboard =  ((BKKeymapKeyboardComponent*)keyboardComponent);
    
    keyboard->setScrollButtonsVisible(true);
    
    keyboard->setAvailableRange(21, 108);
    
    keyboard->setOctaveForMiddleC(4);
    
    keyboardState.addListener(this);
    
    fillKeymapSelectCB();
    
    updateFields();
}

KeymapViewController::~KeymapViewController()
{
    
}

void KeymapViewController::reset(void)
{
    fillKeymapSelectCB();
    updateFields();
}

void KeymapViewController::paint (Graphics& g)
{
    g.fillAll(Colours::lightgrey);
}

void KeymapViewController::resized()
{
    // First row
    keymapSelectL.setTopLeftPosition(0, gYSpacing);
    keymapSelectCB.setTopLeftPosition(keymapSelectL.getRight()+gXSpacing, keymapSelectL.getY());
    
    // Second row
    keymapNameL.setTopLeftPosition(0, keymapSelectL.getBottom()+gYSpacing);
    keymapNameTF.setTopLeftPosition(keymapNameL.getRight()+gXSpacing, keymapNameL.getY());

    // Third row
    keymapL.setTopLeftPosition(0, keymapNameL.getBottom()+gYSpacing);
    keymapTF.setTopLeftPosition(keymapL.getRight()+gXSpacing, keymapL.getY());
    
    // Keyboard
    BKKeymapKeyboardComponent* keyboard =  ((BKKeymapKeyboardComponent*)keyboardComponent);
    
    float keyboardHeight = 100;
    float keyWidth = (getWidth()-2*gXSpacing) / 29.0;
    
    keyboard->setKeyWidth(keyWidth);
    
    keyboard->setBlackNoteLengthProportion(0.65);
    
    
    keyboardComponent->setBounds(gXSpacing, getBottom()-1.5*keyboardHeight, getWidth()-2*gXSpacing, keyboardHeight);
}

void KeymapViewController::bkComboBoxDidChange        (ComboBox* box)
{
    String name = box->getName();
    
    if (name == "Keymap")
    {
        processor.updateState->currentKeymapId = box->getSelectedItemIndex();
        
        if (processor.updateState->currentKeymapId == keymapSelectCB.getNumItems()-1) // New Keymap
        {
            processor.gallery->addKeymap();
            
            fillKeymapSelectCB();
        }

        updateFields();
    }
}

void KeymapViewController::bkTextFieldDidChange(TextEditor& tf)
{
    String text = tf.getText();
    String name = tf.getName();
    
    //float f = text.getFloatValue();
    int i = text.getIntValue();
    
    DBG(name + ": |" + text + "|");
    
    if (name == "KeymapName")
    {
        processor.gallery->getKeymap(processor.updateState->currentKeymapId)->setName(text);

        int selected = keymapSelectCB.getSelectedId();
        if (selected != keymapSelectCB.getNumItems()) keymapSelectCB.changeItemText(selected, text);
        keymapSelectCB.setSelectedId(selected, dontSendNotification );
    }
    else if (name == "KeymapMidi")
    {
        Array<int> keys = keymapStringToIntArray(text);
        
        keymapTF.setText(intArrayToString(keys));

        // get old keys to send to update
        Array<int> oldKeys = processor.gallery->getKeymap(processor.updateState->currentKeymapId)->keys();
        
        processor.gallery->setKeymap(processor.updateState->currentKeymapId, keys);
        
        BKKeymapKeyboardComponent* keyboard =  ((BKKeymapKeyboardComponent*)keyboardComponent);
        
        keyboard->setKeysInKeymap(keys);
        
        theGraph->update(PreparationTypeKeymap, processor.updateState->currentKeymapId);
    }
    else
    {
        DBG("Unregistered text field entered input.");
    }
}

void KeymapViewController::fillKeymapSelectCB(void)
{
    keymapSelectCB.clear(dontSendNotification);
    
    Keymap::PtrArr keymaps = processor.gallery->getKeymaps();
    
    for (int i = 0; i < keymaps.size(); i++)
    {
        String name = keymaps[i]->getName();
        if (name != String::empty)  keymapSelectCB.addItem(name, i+1);
        else                        keymapSelectCB.addItem(String(i+1), i+1);
    }
    
    keymapSelectCB.addItem("New keymap...", keymaps.size()+1);
    
    keymapSelectCB.setSelectedItemIndex(processor.updateState->currentKeymapId, NotificationType::dontSendNotification);
    
}


void KeymapViewController::updateFields(void)
{
    Keymap::Ptr km = processor.gallery->getKeymap(processor.updateState->currentKeymapId);
    
    keymapTF.setText( intArrayToString(km->keys()));
    
    keymapNameTF.setText(km->getName());
    
    BKKeymapKeyboardComponent* keyboard =  ((BKKeymapKeyboardComponent*)keyboardComponent);
    
    keyboard->setKeysInKeymap(km->keys());
    
}

void KeymapViewController::bkMessageReceived (const String& message)
{
    if (message == "keymap/update")
    {
        //processor.updateState->currentKeymapId = processor.currentPiano->currentPMap->getKeymapId();
        
        //updateFields();
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
    
    updateFields();
    
    BKKeymapKeyboardComponent* keyboard =  ((BKKeymapKeyboardComponent*)keyboardComponent);
    
    keyboard->setKeysInKeymap(processor.gallery->getKeymap(processor.updateState->currentKeymapId)->keys());
    
    theGraph->update(PreparationTypeKeymap, processor.updateState->currentKeymapId);
    
}



