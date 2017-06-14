/*
  ==============================================================================

    BKKeyboardSlider.cpp
    Created: 12 Jun 2017 7:10:27am
    Author:  Daniel Trueman

  ==============================================================================
*/

#include "BKKeyboardSlider.h"

BKKeyboardSlider::BKKeyboardSlider()
{
    
    addAndMakeVisible (keyboardComponent =
                       new BKKeymapKeyboardComponent (keyboardState, BKKeymapKeyboardComponent::horizontalKeyboard));
    BKKeymapKeyboardComponent* keyboard =  ((BKKeymapKeyboardComponent*)keyboardComponent);
    minKey = 21;
    maxKey = 108;
    keyboard->setScrollButtonsVisible(false);
    keyboard->setAvailableRange(minKey, maxKey);
    keyboard->setOctaveForMiddleC(4);
    keyboard->addMouseListener(this, true);
    keyboardState.addListener(this);
    keyboardVals.ensureStorageAllocated(128);
    for(int i=0; i<128; i++) keyboardVals.add(0.);
    lastKeyPressed = 0;
    orderedPairs = true;

    showName.setText("unnamed keyboard slider", dontSendNotification);
    showName.setJustificationType(Justification::bottomRight);
    addAndMakeVisible(showName);

    keyboardValueTF.setText(String(0.00f));
    keyboardValueTF.setName("KSLIDERTXT");
    keyboardValueTF.addListener(this);
    addAndMakeVisible(keyboardValueTF);

    keyboardValsTextField = new TextEditor();
    keyboardValsTextField->setMultiLine(true);
    keyboardValsTextField->setName("KSLIDERTXTEDITALL");
    keyboardValsTextField->addListener(this);
    addAndMakeVisible(keyboardValsTextField);
    keyboardValsTextField->setAlpha(0);
    keyboardValsTextField->toBack();

    keyboardValsTextFieldOpen.setName("KSLIDERTXTEDITALLBUTTON");
    keyboardValsTextFieldOpen.addListener(this);
    keyboardValsTextFieldOpen.setButtonText("edit all");
    addAndMakeVisible(keyboardValsTextFieldOpen);

}

void BKKeyboardSlider::paint (Graphics& g)
{
    g.fillAll(Colours::lightgrey);
}

void BKKeyboardSlider::resized()
{
    Rectangle<int> area (getLocalBounds());
    float keyboardHeight = 60;
    Rectangle<int> keymapRow = area.removeFromBottom(keyboardHeight + 20);
    
    //absolute keymap
    BKKeymapKeyboardComponent* keyboard =  ((BKKeymapKeyboardComponent*)keyboardComponent);
    //float keyWidth = keymapRow.getWidth() / 52.0; //number of white keys
    float keyWidth = keymapRow.getWidth() / round((maxKey - minKey) * 7./12 + 1); //num white keys
    keyboard->setKeyWidth(keyWidth);
    keyboard->setBlackNoteLengthProportion(0.65);
    keyboardComponent->setBounds(keymapRow.removeFromBottom(keyboardHeight));
    Rectangle<int> textSlab (keymapRow.removeFromBottom(20));
    keyboardValueTF.setBounds(textSlab.removeFromRight(50));
    showName.setBounds(textSlab.removeFromRight(125));
    keyboardValsTextFieldOpen.setBounds(textSlab.removeFromLeft(75));
    keyboardValsTextField->setBounds(keyboardComponent->getBounds());
}

void BKKeyboardSlider::setAvailableRange(int min, int max)
{
    minKey = min;
    maxKey = max;
    keyboardSize = max - min;
    BKKeymapKeyboardComponent* keyboard =  ((BKKeymapKeyboardComponent*)keyboardComponent);
    keyboard->setAvailableRange(minKey, maxKey);
    //resized();
}

void BKKeyboardSlider::mouseMove(const MouseEvent& e)
{
    BKKeymapKeyboardComponent* keyboard =  ((BKKeymapKeyboardComponent*)keyboardComponent);
    if(keyboard->getLastNoteOver() >= 0 && keyboard->getLastNoteOver() < 128){
        keyboardValueTF.setText(String(keyboardVals.getUnchecked(keyboard->getLastNoteOver())), dontSendNotification);
    }
}

void BKKeyboardSlider::bkTextFieldDidChange (TextEditor& textEditor)
{

}

void BKKeyboardSlider::updateDisplay()
{
    for(int i=0; i<128; i++)
    {
        if(keyboardVals.getUnchecked(i) != 0)
            keyboardState.addToKeymap(i);
        else
            keyboardState.removeFromKeymap(i);
    }
    
    BKKeymapKeyboardComponent* keyboard =  ((BKKeymapKeyboardComponent*)keyboardComponent);
    keyboard->repaint();
}

void BKKeyboardSlider::textEditorReturnKeyPressed(TextEditor& textEditor)
{
    if(textEditor.getName() == keyboardValsTextField->getName())
    {
        if(orderedPairs)
        {
            
            keyboardVals = stringOrderedPairsToFloatArray(keyboardValsTextField->getText(), 128);
            
            listeners.call(&BKKeyboardSliderListener::keyboardSliderChanged,
                           getName(),
                           keyboardVals);
        }
        else
        {
            setActiveValsFromString(keyboardValsTextField->getText());
            
            listeners.call(&BKKeyboardSliderListener::keyboardSliderChanged,
                           getName(),
                           getActiveValues());
        }
        
        keyboardValsTextField->setAlpha(0);
        keyboardValsTextField->toBack();
        
        updateDisplay();
    }
    else if(textEditor.getName() == keyboardValueTF.getName())
    {
        //DBG("keyboardValueTF did change ");
        if(keyboardState.isInKeymap(lastKeyPressed))
        {
            keyboardVals.set(lastKeyPressed, keyboardValueTF.getText().getDoubleValue());
            
            if(orderedPairs)
            {
                listeners.call(&BKKeyboardSliderListener::keyboardSliderChanged,
                               getName(),
                               keyboardVals);
            }
            else
            {
                listeners.call(&BKKeyboardSliderListener::keyboardSliderChanged,
                               getName(),
                               getActiveValues());
            }
        }
    }
}

void BKKeyboardSlider::handleKeymapNoteToggled (BKKeymapKeyboardState* source, int midiNoteNumber)
{

    lastKeyPressed = midiNoteNumber;
    
    DBG("lastKeyPressed = " + String(lastKeyPressed));
    
    if(keyboardState.isInKeymap(lastKeyPressed))
    {
        keyboardVals.set(lastKeyPressed, keyboardValueTF.getText().getDoubleValue());
    }
    else
    {
        keyboardVals.set(lastKeyPressed, 0.);
    }
}

void BKKeyboardSlider::bkButtonClicked (Button* b)
{
    if(b->getName() == keyboardValsTextFieldOpen.getName())
    {
        keyboardValsTextField->setAlpha(1);
        keyboardValsTextField->toFront(true);
        if(orderedPairs) {
            keyboardValsTextField->setText(offsetArrayToString2(keyboardVals), dontSendNotification);
        }
        else
        {
            keyboardValsTextField->setText(floatArrayToString(getActiveValues()), dontSendNotification);
        }
        
        
    }
}

//index does not correspond to midiNoteNumber here; just a list from minKey to maxKey
Array<float> BKKeyboardSlider::getActiveValues()
{
    Array<float> activeVals;
    activeVals.ensureStorageAllocated(keyboardSize);
    
    int valCounter = 0;
    
    for(int i=minKey; i<=maxKey; i++)
    {
        activeVals.set(valCounter++, keyboardVals.getUnchecked(i));
    }
    
    return activeVals;
}

//all values, indexed by midiNoteNumber
Array<float> BKKeyboardSlider::getAllValues()
{
    return keyboardVals;
}

void BKKeyboardSlider::setActiveValsFromString(String s)
{
    Array<float> tempArray = stringToFloatArray(s);
    
    int valCounter = 0;
    
    for(int i=minKey; i<=maxKey; i++)
    {
        if(valCounter < tempArray.size()) keyboardVals.set(i, tempArray.getUnchecked(valCounter++));
    }
}

void BKKeyboardSlider::setAllValues(Array<float> newvals)
{
    for(int i=0; i<newvals.size(); i++)
    {
        keyboardVals.set(i, newvals.getUnchecked(i));
    }
    
    updateDisplay();
}

void BKKeyboardSlider::setActiveValues(Array<float> newvals)
{
    int valCounter = 0;
    
    for(int i=minKey; i<=maxKey; i++)
    {
        if(valCounter < newvals.size()) keyboardVals.set(i, newvals.getUnchecked(valCounter++));
    }
    
    updateDisplay();
}
