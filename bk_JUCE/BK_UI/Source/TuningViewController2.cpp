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

    selectCB.setName("Tuning");
    selectCB.addSeparator();
    selectCB.addListener(this);
    selectCB.setSelectedItemIndex(0);
    selectCB.addMyListener(this);
    fillSelectCB();
    addAndMakeVisible(selectCB);
     
    
    // Absolute Tuning Keyboard
    addAndMakeVisible (absoluteKeyboardComponent =
                       new BKKeymapKeyboardComponent (absoluteKeyboardState, BKKeymapKeyboardComponent::horizontalKeyboard));
    BKKeymapKeyboardComponent* keyboard =  ((BKKeymapKeyboardComponent*)absoluteKeyboardComponent);
    keyboard->setScrollButtonsVisible(false);
    keyboard->setAvailableRange(21, 108);
    keyboard->setOctaveForMiddleC(4);
    keyboard->addMouseListener(this, true);
    absoluteKeyboardState.addListener(this);
    absoluteOffsets.ensureStorageAllocated(128);
    for(int i=0; i<128; i++) absoluteOffsets.add(0.);
    lastAbsoluteKeyPressed = 0;
    
    absoluteKeyboardName.setText("key-by-key tuning offsets", dontSendNotification);
    absoluteKeyboardName.setJustificationType(Justification::bottomRight);
    addAndMakeVisible(absoluteKeyboardName);
    
    absoluteKeyboardValueTF.setText(String(0.00f));
    absoluteKeyboardValueTF.setName("ABSTXT");
    absoluteKeyboardValueTF.addListener(this);
    addAndMakeVisible(absoluteKeyboardValueTF);
    
    absoluteValsTextField = new TextEditor();
    absoluteValsTextField->setMultiLine(true);
    absoluteValsTextField->setName("ABSTXTEDITALL");
    absoluteValsTextField->addListener(this);
    addAndMakeVisible(absoluteValsTextField);
    absoluteValsTextField->setAlpha(0);
    absoluteValsTextField->toBack();
    
    
    //Custom Tuning Keyboard
    customOffsets.ensureStorageAllocated(12);
    for(int i=0; i<12; i++) customOffsets.add(0.);
    lastCustomKeyPressed = 0;
    
    absoluteValsTextFieldOpen.setName("ABSTXTEDITALLBUTTON");
    absoluteValsTextFieldOpen.addListener(this);
    absoluteValsTextFieldOpen.setButtonText("edit all offsets");
    addAndMakeVisible(absoluteValsTextFieldOpen);

    updateFields();
}

void TuningViewController2::resized()
{
    Rectangle<int> area (getLocalBounds());
    float keyboardHeight = 60;
    Rectangle<int> absoluteKeymapRow = area.removeFromBottom(keyboardHeight + 20);
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
    Rectangle<int> absoluteTextSlab (absoluteKeymapRow.removeFromBottom(20));
    absoluteKeyboardValueTF.setBounds(absoluteTextSlab.removeFromRight(75));
    absoluteKeyboardName.setBounds(absoluteTextSlab.removeFromRight(200));
    absoluteValsTextFieldOpen.setBounds(absoluteTextSlab.removeFromLeft(100));
    absoluteValsTextField->setBounds(absoluteKeyboardComponent->getBounds());
}

void TuningViewController2::mouseMove(const MouseEvent& e)
{
    if(e.eventComponent == absoluteKeyboardComponent)
    {
        BKKeymapKeyboardComponent* keyboard =  ((BKKeymapKeyboardComponent*)absoluteKeyboardComponent);
        //DBG("last key over " + String(keyboard->getLastNoteOver()));
        if(keyboard->getLastNoteOver() >= 0 && keyboard->getLastNoteOver() < 128){
            absoluteKeyboardValueTF.setText(String(absoluteOffsets.getUnchecked(keyboard->getLastNoteOver())), dontSendNotification);
        }
    }
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

void TuningViewController2::bkTextFieldDidChange (TextEditor& textEditor)
{
    if(textEditor.getName() == absoluteKeyboardValueTF.getName())
    {
        if(absoluteKeyboardState.isInKeymap(lastAbsoluteKeyPressed))
        {
            absoluteOffsets.set(lastAbsoluteKeyPressed, absoluteKeyboardValueTF.getText().getDoubleValue());
            //absoluteValsTextField->setText(offsetArrayToString2(absoluteOffsets), dontSendNotification);
        }
    }
}

void TuningViewController2::textEditorReturnKeyPressed(TextEditor& textEditor)
{
    if(textEditor.getName() == absoluteValsTextField->getName())
    {
        absoluteOffsets = stringOrderedPairsToFloatArray(absoluteValsTextField->getText(), 128);
        absoluteValsTextField->setAlpha(0);
        absoluteValsTextField->toBack();
        
        for(int i=0; i<128; i++)
        {
            if(absoluteOffsets.getUnchecked(i) != 0)
                absoluteKeyboardState.addToKeymap(i);
        }

    }
}

void TuningViewController2::bkComboBoxDidChange (ComboBox* box)
{
    String name = box->getName();
    
    if (name == "Tuning")
    {
        processor.updateState->currentTuningId = box->getSelectedItemIndex();
        
        if (processor.updateState->currentTuningId == selectCB.getNumItems()-1)
        {
            processor.gallery->addDirect();
            
            fillSelectCB();
        }
        
        //updateFields(sendNotification);
        updateFields();
    }
}

void TuningViewController2::BKEditableComboBoxChanged(String name, BKEditableComboBox* cb)
{
    processor.gallery->getTuning(processor.updateState->currentTuningId)->setName(name);
}

void TuningViewController2::bkButtonClicked (Button* b)
{
    if(b->getName() == absoluteValsTextFieldOpen.getName())
    {
        absoluteValsTextField->setAlpha(1);
        absoluteValsTextField->toFront(true);
        absoluteValsTextField->setText(offsetArrayToString2(absoluteOffsets), dontSendNotification);
    }
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
        lastAbsoluteKeyPressed = midiNoteNumber;
        DBG("lastAbsoluteKeyPressed = " + String(lastAbsoluteKeyPressed));
        
        if(absoluteKeyboardState.isInKeymap(lastAbsoluteKeyPressed))
        {
            absoluteOffsets.set(lastAbsoluteKeyPressed, absoluteKeyboardValueTF.getText().getDoubleValue());
        }
        else
        {
            absoluteOffsets.set(lastAbsoluteKeyPressed, 0.);
        }
    }
    else if(source == &customKeyboardState)
    {
        DBG("toggled custom keyboard " + String(midiNoteNumber));
        lastCustomKeyPressed = midiNoteNumber;
    }
    
}



