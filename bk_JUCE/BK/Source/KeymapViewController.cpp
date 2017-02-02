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
    
#if 0
    // Keyboard
    addAndMakeVisible (keyboardComponent = new MidiKeyboardComponent (keyboardState,
                                                                 MidiKeyboardComponent::horizontalKeyboard));
    
    keyboardState.addListener(this);
#endif
    
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
    
#if 0
    int kbX = gXSpacing;
    int kbY = lY * cKeymapParameterTypes.size() +  gYSpacing;
    keyboardComponent->setBounds(kbX, kbY,
                                 getWidth() - 2*gXSpacing, getHeight() - kbY - gYSpacing);
#endif
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
        currentKeymapId = i;
        updateFields(currentKeymapId);
    }
    else if (name == cKeymapParameterTypes[KeymapField])
    {
        Array<int> keys = keymapStringToIntArray(text);
        
        keymapTF[KeymapField]->setText(intArrayToString(keys));
        
        processor.bkKeymaps[currentKeymapId]->setKeymap(keys);
    }
    else
    {
        DBG("Unregistered text field entered input.");
    }
}


void KeymapViewController::updateFields(int keymapId)
{
    Keymap::Ptr km = processor.bkKeymaps[keymapId];
    
    keymapTF[KeymapId]        ->setText( String( keymapId));
    keymapTF[KeymapField]     ->setText( intArrayToString(km->keys()));
    
}

void KeymapViewController::bkMessageReceived (const String& message)
{
    if (message == "keymap/update")
    {
        //currentKeymapId = processor.currentPiano->currentPMap->getKeymapId();
        
        //updateFields(currentKeymapId);
    }
}

#if 0
/** Called when one of the MidiKeyboardState's keys is pressed. */
void KeymapViewController::handleNoteOn (MidiKeyboardState* source,
                   int midiChannel, int midiNoteNumber, float velocity)
{
    DBG("NOTE ON: " + String(midiNoteNumber) + " " + String(velocity));
    
    processor.bkKeymaps[currentKeymapId]->toggleNote(midiNoteNumber - 12);
    
    updateFields(currentKeymapId);
    
}

/** Called when one of the MidiKeyboardState's keys is released. */
void KeymapViewController::handleNoteOff (MidiKeyboardState* source,
                    int midiChannel, int midiNoteNumber, float velocity)
{
    DBG("NOTE OFF: " + String(midiNoteNumber) + " " + String(velocity));
}
#endif


