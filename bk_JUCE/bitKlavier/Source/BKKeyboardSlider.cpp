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

    keyboard =  (BKKeymapKeyboardComponent*)keyboardComponent.get();
    
    minKey = 21;
    maxKey = 108;
    keyboard->setScrollButtonsVisible(false);
    keyboard->setAvailableRange(minKey, maxKey);
    keyboard->setOctaveForMiddleC(4);
    keyboard->setFundamental(-1); //would be good to wrap all of these into keyboard so that keyboard->setMode(slider) sets all these internally and makes it clear
    keyboard->setAllowDrag(false);
    keyboard->doKeysToggle(false);
    keyboard->addMouseListener(this, true);
    keyboardState.addListener(this);
    keyboardVals.ensureStorageAllocated(128);
    for(int i=0; i<128; i++) keyboardVals.add(0.);
    lastKeyPressed = 0;
    orderedPairs = true;

    showName.setText("unnamed keyboard slider", dontSendNotification);
    showName.setJustificationType(Justification::bottomRight);
    addAndMakeVisible(showName);

    keyboardValueTF.setText(String(0.0, 1));
    keyboardValueTF.setName("KSLIDERTXT");
    keyboardValueTF.addListener(this);
    addAndMakeVisible(keyboardValueTF);

    keyboardValsTextField = new BKTextEditor();
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
    //g.fillAll(Colours::lightgrey);
    keyboardComponent.release();
}

void BKKeyboardSlider::resized()
{
    //Rectangle<int> area (getBounds());
    Rectangle<int> area (getLocalBounds());
    float keyboardHeight = area.getHeight() - 20;
    Rectangle<int> keymapRow = area.removeFromBottom(keyboardHeight + 20);
    
    //absolute keymap
    float keyWidth = keymapRow.getWidth() / round((maxKey - minKey) * 7./12 + 1); //num white keys
    keyboard->setKeyWidth(keyWidth);
    keyboard->setBlackNoteLengthProportion(0.65);
    //keyboardComponent->setBounds(keymapRow.removeFromBottom(keyboardHeight - gYSpacing));
    Rectangle<int> keyboardRect = keymapRow.removeFromBottom(keyboardHeight - gYSpacing);
    //keyboard->setBounds(keymapRow.removeFromBottom(keyboardHeight - gYSpacing));
    keyboard->setBounds(keyboardRect);
    keymapRow.removeFromBottom(gYSpacing);
    Rectangle<int> textSlab (keymapRow.removeFromBottom(20));
    keyboardValueTF.setBounds(textSlab.removeFromRight(50));
    showName.setBounds(textSlab.removeFromRight(125));
    keyboardValsTextFieldOpen.setBounds(textSlab.removeFromLeft(75));
    keyboardValsTextField->setBounds(keyboard->getBounds());
    
    DBG("keywidth: " + String(keyWidth) + " keyheight: " + String(keyboardHeight));
    DBG("keyboardRect: " + rectangleToString(keyboardRect));

}

void BKKeyboardSlider::setFundamental(int fund)
{
    keyboard->setFundamental(fund);
    
    DBG("new fundamental = " + String(fund));
}

void BKKeyboardSlider::setAvailableRange(int min, int max)
{
    minKey = min;
    maxKey = max;
    keyboardSize = max - min; //
    
    //all of the above unnecessary?
    keyboard->setAvailableRange(minKey, maxKey);
}

void BKKeyboardSlider::mouseMove(const MouseEvent& e)
{
    keyboardValueTF.setText(String(keyboard->getLastNoteOverValue(), 1), dontSendNotification);
}

void BKKeyboardSlider::mouseDrag(const MouseEvent& e)
{
    if(e.y >= 0 && e.y <= keyboard->getHeight())
    {
        bool isBlackKey = MidiMessage::isMidiNoteBlack (keyboard->getLastKeySelected());
        float dragPos = (float)e.y / keyboard->getHeight();
        if(isBlackKey) dragPos /= keyboard->getBlackNoteLengthProportion();
        
        dragPos = 1. - 2. * dragPos;
        if(dragPos > 0.) dragPos = dragPos * dragPos;
        else dragPos = -1.* dragPos * dragPos;
        
        //keyboardVals.set(keyboard->getLastKeySelected(),  dragPos * 50.);
        //keyboardVals.set(lastKeyPressed,  dragPos * 50.);
        keyboardValueTF.setText(String(dragPos * 50.0, 1), dontSendNotification);
        //keyboard->setKeyValue(keyboard->getLastKeySelected(), dragPos * 50.);
        keyboard->setKeyValue(lastKeyPressed, dragPos * 50.);
        
        DBG("dragging last key, height " + String(keyboard->getLastKeySelected()) + " " + String(dragPos));
    }
}

void BKKeyboardSlider::mouseUp(const MouseEvent& e)
{
    if(e.mouseWasClicked())
    {
        if(e.mods.isShiftDown())
        {
            //keyboardVals.set(lastKeyPressed,  0.);
            keyboardValueTF.setText(String(0.), dontSendNotification);
            keyboard->setKeyValue(lastKeyPressed, 0.);
        }
    }
    
    listeners.call(&BKKeyboardSlider::Listener::keyboardSliderChanged,
                   getName(),
                   keyboard->getValuesRotatedByFundamental());
    
    keyboard->repaint();
}

void BKKeyboardSlider::mouseDown(const MouseEvent& e)
{
    if (e.originalComponent == &keyboardValueTF || e.originalComponent == &showName)
    {
        inputListeners.call(&WantsKeyboardListener::keyboardSliderWantsKeyboard, this);
    }
    else
    {
        lastKeyPressed = keyboard->getLastNoteOver();
    }
}

void BKKeyboardSlider::bkTextFieldDidChange (TextEditor& textEditor)
{

}

//shouldn't need this, internal to keyboard...
void BKKeyboardSlider::updateDisplay()
{
    for(int i=0; i<128; i++)
    {
        if(keyboardVals.getUnchecked(i) != 0)
            keyboardState.addToKeymap(i);
        else
            keyboardState.removeFromKeymap(i);
    }
    
    keyboard->repaint();
}

void BKKeyboardSlider::textEditorReturnKeyPressed(TextEditor& textEditor)
{
    if(textEditor.getName() == keyboardValsTextField->getName())
    {
        if(orderedPairs) keyboard->setValuesDirectly(stringOrderedPairsToFloatArray(keyboardValsTextField->getText(), 128));

        else keyboard->setValuesDirectly(stringToFloatArray(keyboardValsTextField->getText()));
        
        listeners.call(&BKKeyboardSlider::Listener::keyboardSliderChanged,
                       getName(),
                       keyboard->getValuesRotatedByFundamental());
        
        keyboardValsTextField->setAlpha(0);
        keyboardValsTextField->toBack();
        
        updateDisplay();
    }
    else if(textEditor.getName() == keyboardValueTF.getName())
    {
        keyboard->setKeyValue(lastKeyPressed, keyboardValueTF.getText().getDoubleValue());
        
        listeners.call(&BKKeyboardSlider::Listener::keyboardSliderChanged,
                       getName(),
                       keyboard->getValuesRotatedByFundamental());
        
        keyboard->repaint();
    }

}

void BKKeyboardSlider::textEditorEscapeKeyPressed (TextEditor& textEditor)
{
    focusLostByEscapeKey = true;
    if(textEditor.getName() == keyboardValsTextField->getName())
    {
        keyboardValsTextField->setAlpha(0);
        keyboardValsTextField->toBack();
        unfocusAllComponents();
    }
    else if(textEditor.getName() == keyboardValueTF.getName())
    {
        unfocusAllComponents();
    }
}

void BKKeyboardSlider::textEditorFocusLost(TextEditor& textEditor)
{
#if !JUCE_IOS
    if(!focusLostByEscapeKey)
    {
        textEditorReturnKeyPressed(textEditor);
    }
#endif
}

void BKKeyboardSlider::handleKeymapNoteToggled (BKKeymapKeyboardState* source, int midiNoteNumber)
{

}

void BKKeyboardSlider::bkButtonClicked (Button* b)
{
    if(b->getName() == keyboardValsTextFieldOpen.getName())
    {
        focusLostByEscapeKey = false;
        keyboardValsTextField->setAlpha(1);
#if !JUCE_IOS
        keyboardValsTextField->toFront(true);
#else
        keyboardValsTextField->toFront(false);
        inputListeners.call(&WantsKeyboardListener::keyboardSliderWantsKeyboard, this);
#endif
        if(orderedPairs) {
            keyboardValsTextField->setText(offsetArrayToString2(keyboard->getValuesDirectly()), dontSendNotification);
            //keyboardValsTextField->setText(offsetArrayToString2(keyboard->getAbsoluteValues())
        }
        else
        {
            keyboardValsTextField->setText(floatArrayToString(keyboard->getValuesDirectly()), dontSendNotification);
            //keyboardValsTextField->setText(floatArrayToString(keyboard->getRotatedValues())
        }
    }
}

// * cut this
//index does not correspond to midiNoteNumber here; just a list from minKey to maxKey
Array<float> BKKeyboardSlider::getActiveValues()
{
    Array<float> activeVals;
    activeVals.ensureStorageAllocated(keyboardSize + 1);
    
    int valCounter = 0;
    
    for(int i=minKey; i<=maxKey; i++)
    {
        activeVals.set(valCounter++, keyboardVals.getUnchecked(i));
    }
    
    return activeVals;
}

// * cut this
Array<float> BKKeyboardSlider::getActiveValuesWithFundamentalOffset()
{
    Array<float> activeVals;
    activeVals.ensureStorageAllocated(keyboardSize + 1);
    for(int i=0; i<keyboardSize + 1; i++) activeVals.add(0.);
    
    int fund = keyboard->getFundamental();
    if(fund < 0) fund = 0;
    
    int valCounter = 0;
    
    for(int i=minKey; i<=maxKey; i++)
    {
        int tempCounter = (valCounter + fund) % (keyboardSize + 1);
        activeVals.set(tempCounter, keyboardVals.getUnchecked(i));
        valCounter++;
    }
    
    return activeVals;
}

// * cut this
//all values, indexed by midiNoteNumber
Array<float> BKKeyboardSlider::getAllValues()
{
    
    int fund = keyboard->getFundamental();
    
    if(fund <= 0) return keyboardVals;
    
    Array<float> tempArray;
    
    for(int i=0; i<keyboardVals.size(); i++)
    {
        tempArray.set((i - fund) % (keyboardSize + 1), keyboardVals.getUnchecked(i));
    }
    
    return tempArray;
}

// * cut this
void BKKeyboardSlider::setActiveValsFromString(String s)
{
    Array<float> tempArray = stringToFloatArray(s);
    
    int valCounter = 0;
    
    for(int i=minKey; i<=maxKey; i++)
    {
        if(valCounter < tempArray.size()) keyboardVals.set(i, tempArray.getUnchecked(valCounter++));
        keyboard->setKeyValue(i, keyboardVals.getUnchecked(i));
    }
}

// * cut this
void BKKeyboardSlider::setActiveValsFromStringWithFundamentalOffset(String s)
{
    Array<float> tempArray = stringToFloatArray(s);
    
    int fund = keyboard->getFundamental();
    if(fund < 0) fund = 0;
    
    int valCounter = 0;
    
    for(int i=minKey; i<=maxKey; i++)
    {
        int tempCounter = (valCounter + fund) % tempArray.size();
        if(valCounter < tempArray.size()) {
            keyboardVals.set(i, tempArray.getUnchecked(tempCounter));
            keyboard->setKeyValue(i, keyboardVals.getUnchecked(i));
        }
        valCounter++;
    }
}

// * make it so this and setActiveValues are the same....
void BKKeyboardSlider::setAllValues(Array<float> newvals)
{
    int fund = keyboard->getFundamental();
    
    for(int i=0; i<newvals.size(); i++)
    {
        if(fund <= 0) {
            //keyboardVals.set(i, newvals.getUnchecked(i));
            keyboard->setKeyValue(i, keyboardVals.getUnchecked(i));
        }
        else {
            //keyboardVals.set((i + fund) % (keyboardSize + 1), newvals.getUnchecked(i));
            keyboard->setKeyValue((i + fund) % (keyboardSize + 1), keyboardVals.getUnchecked(i)); //blargh...
        }
    }
    
    updateDisplay();
}

// * cut this
void BKKeyboardSlider::setActiveValues(Array<float> newvals)
{
    int valCounter = 0;
    
    for(int i=minKey; i<=maxKey; i++)
    {
        if(valCounter < newvals.size()) {
            //keyboardVals.set(i, newvals.getUnchecked(valCounter++));
            keyboard->setKeyValue(i, keyboardVals.getUnchecked(i));
        }
    }
    
    updateDisplay();
}

void BKKeyboardSlider::setValues(Array<float> newvals)
{
    int fund = keyboard->getFundamental();
    keyboard->setFundamental(0);
    keyboard->clearKeyValues();
    
    int valCounter = 0;
    
    //for(int i=minKey; i<=maxKey; i++)
    DBG("BKKeyboardSlider::setValues newvals size = " + String(newvals.size()));
    for(int i=0; i<newvals.size(); i++)
    {
        if(valCounter < newvals.size()) {
            //keyboardVals.set(i, newvals.getUnchecked(valCounter++));
            keyboard->setKeyValue(i, newvals.getUnchecked(valCounter++));
        }
    }
    
    if(fund > -1) keyboard->setFundamental(fund);
    
    updateDisplay();
}

void BKKeyboardSlider::setValuesAbsolute(Array<float> newvals)
{
    for(int i=0; i<newvals.size(); i++)
    {
        keyboard->setKeyValue(i, newvals.getUnchecked(i));
    }

    updateDisplay();
}
