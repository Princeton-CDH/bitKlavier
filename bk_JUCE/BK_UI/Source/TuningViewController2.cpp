/*
  ==============================================================================

    TuningViewController2.cpp
    Created: 10 Jun 2017 11:24:22am
    Author:  Daniel Trueman

  ==============================================================================
*/

#include "TuningViewController2.h"


//==============================================================================
TuningViewController2::TuningViewController2(BKAudioProcessor& p, BKItemGraph* theGraph):
processor(p),
theGraph(theGraph)
{

    
    addAndMakeVisible(selectCB);
    selectCB.setName("Keymap");
    selectCB.addSeparator();
    selectCB.addListener(this);
    //keymapSelectCB.setEditableText(true);
    selectCB.setSelectedItemIndex(0);
     
    
    // Absolute Keyboard
    addAndMakeVisible (absoluteKeyboardComponent =
                       new BKKeymapKeyboardComponent (absoluteKeyboardState, BKKeymapKeyboardComponent::horizontalKeyboard));
    BKKeymapKeyboardComponent* keyboard =  ((BKKeymapKeyboardComponent*)absoluteKeyboardComponent);
    keyboard->setScrollButtonsVisible(false);
    keyboard->setAvailableRange(21, 108);
    keyboard->setOctaveForMiddleC(4);
    absoluteKeyboardState.addListener(this);
    absoluteOffsets.ensureStorageAllocated(128);
    for(int i=0; i<128; i++) absoluteOffsets.add(0.);
    lastAbsoluteKeyPressed = 0;
    
    lastCustomKeyPressed = 0;
    
    fillSelectCB();
    
    updateFields();
}

void TuningViewController2::resized()
{
    Rectangle<int> area (getLocalBounds());
    float keyboardHeight = 60;
    Rectangle<int> absoluteKeymapRow = area.removeFromBottom(keyboardHeight);
    Rectangle<int> leftColumn = area.removeFromLeft(area.getWidth() * 0.5);
    
    //prep select combo box
    selectCB.setBounds(leftColumn.removeFromTop(20));
    
    //tuning offset slider
    //offsetSlider->setBounds(leftColumn.removeFromTop(40));

    //absolute keymap
    BKKeymapKeyboardComponent* keyboard =  ((BKKeymapKeyboardComponent*)absoluteKeyboardComponent);
    float keyWidth = absoluteKeymapRow.getWidth() / 52.0; //number of white keys
    keyboard->setKeyWidth(keyWidth);
    keyboard->setBlackNoteLengthProportion(0.65);
    absoluteKeyboardComponent->setBounds(absoluteKeymapRow.removeFromBottom(keyboardHeight));
}


void TuningViewController2::paint (Graphics& g)
{
    g.fillAll(Colours::lightgrey);
}


void TuningViewController2::fillSelectCB(void)
{
    // Direct menu
    Tuning::PtrArr newpreps = processor.gallery->getAllTuning();
    
    selectCB.clear(dontSendNotification);
    for (int i = 0; i < newpreps.size(); i++)
    {
        String name = newpreps[i]->getName();
        if (name != String::empty)  selectCB.addItem(name, i+1);
        else                        selectCB.addItem(String(i+1), i+1);
    }
    
    selectCB.addItem("New tuning...", newpreps.size()+1);
    
    selectCB.setSelectedItemIndex(processor.updateState->currentTuningId, NotificationType::dontSendNotification);
    
}

void TuningViewController2::BKEditableComboBoxChanged(String name, BKEditableComboBox* cb)
{
    DBG("editable CB changed " + name);
    
    processor.gallery->getTuning(processor.updateState->currentTuningId)->setName(name);

    int selected = selectCB.getSelectedId();
    if (selected != selectCB.getNumItems()) selectCB.changeItemText(selected, name);
    selectCB.setSelectedId(selected, dontSendNotification );
}


void TuningViewController2::updateFields(void)
{
    
    TuningPreparation::Ptr prep = processor.gallery->getActiveTuningPreparation(processor.updateState->currentTuningId);
    
    selectCB.setSelectedItemIndex(processor.updateState->currentTuningId, dontSendNotification);
    
    //offsetSlider->setValue(prep->getFundamentalOffset(), dontSendNotification);

    
}

void TuningViewController2::handleKeymapNoteOn (BKKeymapKeyboardState* source, int midiNoteNumber)
{

}

void TuningViewController2::handleKeymapNoteOff (BKKeymapKeyboardState* source, int midiNoteNumber)
{

}

void TuningViewController2::handleKeymapNoteToggled (BKKeymapKeyboardState* source, int midiNoteNumber)
{
    if(source == &absoluteKeyboardState)
    {
        DBG("toggled absolute keyboard " + String(midiNoteNumber));
        lastAbsoluteKeyPressed = midiNoteNumber;
    }
    else if(source == &customKeyboardState)
    {
        DBG("toggled custom keyboard " + String(midiNoteNumber));
        lastCustomKeyPressed = midiNoteNumber;
    }
    
}


