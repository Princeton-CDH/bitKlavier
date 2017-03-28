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
KeymapViewController::KeymapViewController(BKAudioProcessor& p):
currentKeymapId(1),
processor(p)
{
    // Labels
    keymapL = OwnedArray<BKLabel>();
    keymapL.ensureStorageAllocated(cKeymapParameterTypes.size());
    
    
    for (int i = 0; i < cKeymapParameterTypes.size(); i++)
    {
        keymapL.set(i, new BKLabel());
        addAndMakeVisible(keymapL[i]);
        keymapL[i]->setName(cKeymapParameterTypes[i]);
        keymapL[i]->setText(cKeymapParameterTypes[i], NotificationType::dontSendNotification);
    }
    
    // Text Fields
    keymapTF = OwnedArray<BKTextField>();
    keymapTF.ensureStorageAllocated(cKeymapParameterTypes.size());
    
    for (int i = 0; i < cKeymapParameterTypes.size(); i++)
    {
        keymapTF.set(i, new BKTextField());
        addAndMakeVisible(keymapTF[i]);
        keymapTF[i]->addListener(this);
        keymapTF[i]->setName(cKeymapParameterTypes[i]);
    }
    
    // Keyboard
    addAndMakeVisible (keyboardComponent = new BKKeymapKeyboardComponent (keyboardState,
                                                                 BKKeymapKeyboardComponent::horizontalKeyboard));
    
    
    BKKeymapKeyboardComponent* keyboard =  ((BKKeymapKeyboardComponent*)keyboardComponent);
    
    keyboard->setScrollButtonsVisible(true);
    
    keyboard->setAvailableRange(21, 109);
    
    keyboard->setOctaveForMiddleC(4);
    
    keyboardState.addListener(this);
    
    updateFields(currentKeymapId);
}

KeymapViewController::~KeymapViewController()
{
    
}

void KeymapViewController::paint (Graphics& g)
{
    g.setColour(Colours::goldenrod);
    g.drawRect(getLocalBounds(), 1);
}

void KeymapViewController::resized()
{
    // Labels
    int i = 0;
    int lX = 0;
    int lY = gComponentLabelHeight + gYSpacing;
    
    for (int n = 0; n < cKeymapParameterTypes.size(); n++)
    {
        keymapL[n]->setTopLeftPosition(lX, gYSpacing + lY * n);
    }
    
    // Text fields
    i = 0;
    int tfX = gComponentLabelWidth + gXSpacing;
    int tfY = gComponentTextFieldHeight + gYSpacing;
    
    for (int n = 0; n < cKeymapParameterTypes.size(); n++)
    {
        keymapTF[n]->setTopLeftPosition(tfX, gYSpacing + tfY * n);
    }
    
    BKKeymapKeyboardComponent* keyboard =  ((BKKeymapKeyboardComponent*)keyboardComponent);
    
    float keyboardHeight = 200;
    float keyWidth = (getWidth()-2*gXSpacing) / 15.0;
    
    keyboard->setKeyWidth(keyWidth);
    
    keyboard->setBlackNoteLengthProportion(0.65);
    
    
    keyboardComponent->setBounds(gXSpacing, 250, getWidth()-2*gXSpacing, keyboardHeight);
}

void KeymapViewController::bkTextFieldDidChange(TextEditor& tf)
{
    String text = tf.getText();
    String name = tf.getName();
    
    //float f = text.getFloatValue();
    int i = text.getIntValue();
    
    DBG(name + ": |" + text + "|");
    
    if (name == cKeymapParameterTypes[KeymapId])
    {
        int numKeymaps = processor.gallery->getNumKeymaps();

        if ((i+1) > numKeymaps)
        {
            // :D
            processor.gallery->addKeymap();
            currentKeymapId = numKeymaps; // last index
        }
        else if (i >= 0)
        {
            currentKeymapId = i;
        }
        
        updateFields(currentKeymapId);
        
    }
    else if (name == cKeymapParameterTypes[KeymapField])
    {
        Array<int> keys = keymapStringToIntArray(text);
        
        keymapTF[KeymapField]->setText(intArrayToString(keys));
        
        processor.gallery->setKeymap(currentKeymapId, keys);
        
        BKKeymapKeyboardComponent* keyboard =  ((BKKeymapKeyboardComponent*)keyboardComponent);
        
        keyboard->setKeysInKeymap(keys);
        
    }
    else
    {
        DBG("Unregistered text field entered input.");
    }
}


void KeymapViewController::updateFields(int keymapId)
{
    Keymap::Ptr km = processor.gallery->getKeymap(keymapId);
    
    keymapTF[KeymapId]        ->setText( String( keymapId));
    keymapTF[KeymapField]     ->setText( intArrayToString(km->keys()));
    
    BKKeymapKeyboardComponent* keyboard =  ((BKKeymapKeyboardComponent*)keyboardComponent);
    
    keyboard->setKeysInKeymap(km->keys());
    
}

void KeymapViewController::bkMessageReceived (const String& message)
{
    if (message == "keymap/update")
    {
        //currentKeymapId = processor.currentPiano->currentPMap->getKeymapId();
        
        //updateFields(currentKeymapId);
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
    DBG("toggled? +" + String(midiNoteNumber));
    
    processor.gallery->getKeymap(currentKeymapId)->toggleNote(midiNoteNumber);
    
    updateFields(currentKeymapId);
    
    BKKeymapKeyboardComponent* keyboard =  ((BKKeymapKeyboardComponent*)keyboardComponent);
    
    keyboard->setKeysInKeymap(processor.gallery->getKeymap(currentKeymapId)->keys());
    
}



