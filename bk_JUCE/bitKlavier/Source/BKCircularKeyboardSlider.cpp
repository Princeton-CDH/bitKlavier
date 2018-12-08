/*
  ==============================================================================

    BKCircularKeyboardSlider.cpp
    Created: 29 Jul 2018 2:10:31pm
    Author:  Daniel Trueman

  ==============================================================================
*/

#include "BKCircularKeyboardSlider.h"


BKCircularKeyboardSlider::BKCircularKeyboardSlider(void):
ratio(1.0)
{
    addAndMakeVisible (keyboardComponent =
                       new BKKeymapKeyboardComponent (keyboardState, BKKeymapKeyboardComponent::horizontalKeyboard));

    keyboard =  (BKKeymapKeyboardComponent*)keyboardComponent.get();
    
    // need slider or other interface for octave change

    minKey = 0;
    maxKey = 11;

    keyboard->setRepaintsOnMouseActivity(false);
    keyboard->setScrollButtonsVisible(false);
    keyboard->setAvailableRange(minKey, maxKey);
    keyboard->setAvailableRange(minKey, maxKey);
    keyboard->setFundamental(0);
    keyboard->setAllowDrag(false);
    keyboard->doKeysToggle(false);
    keyboard->addMouseListener(this, true);
    keyboardState.addListener(this);
    lastKeyPressed = 0;

    showName.setText("unnamed keyboard slider", dontSendNotification);
    showName.setJustificationType(Justification::centredRight);
    showName.addMouseListener(this, true);
    addAndMakeVisible(showName);

    keyboardValueTF.setText(String(0.0, 1));
    keyboardValueTF.setName("KSLIDERTXT");
    keyboardValueTF.setTooltip("offset from ET in cents for last key pressed; can enter values here as well");
    keyboardValueTF.addListener(this);
#if JUCE_IOS
    keyboardValueTF.setReadOnly(true);
#endif
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
    keyboardValsTextFieldOpen.setTooltip("click drag on keys to set offsets in cents by key, or press 'edit all' to edit as text");
    addAndMakeVisible(keyboardValsTextFieldOpen);
}

void BKCircularKeyboardSlider::paint (Graphics& g)
{
    //g.fillAll(Colours::lightgrey);
    keyboardComponent.release();
}

void BKCircularKeyboardSlider::resized()
{
    float heightUnit = getHeight() * 0.1;
    float widthUnit = getWidth() * 0.1;
    
    Rectangle<int> area (getLocalBounds());
    float keyboardHeight = 8 * heightUnit;
    Rectangle<int> keymapRow = area.removeFromBottom(10 * heightUnit);
    
    //absolute keymap
    //float keyWidth = keymapRow.getWidth() / round((maxKey - minKey) * 7./12 + 1); //num white keys
    float keyWidth = keymapRow.getWidth() / 7; //num white keys
    keyboard->setKeyWidth(keyWidth);
    keyboard->setBlackNoteLengthProportion(0.65);
    
    Rectangle<int> keyboardRect = keymapRow.removeFromBottom(keyboardHeight);
    
    keyboard->setBounds(keyboardRect);
    
    Rectangle<int> textSlab (keymapRow.removeFromBottom(2*heightUnit + gYSpacing));
    keyboardValueTF.setBounds(textSlab.removeFromRight(ratio * widthUnit));
    showName.setBounds(textSlab.removeFromRight(2*ratio*widthUnit));
    keyboardValsTextFieldOpen.setBounds(textSlab.removeFromLeft(ratio*widthUnit*1.5));
    
#if JUCE_IOS
    keyboardValsTextField->setBounds(keyboard->getBounds());
    keyboardValsTextField->setSize(keyboard->getWidth() * 0.5f, keyboard->getHeight());
#else
    keyboardValsTextField->setBounds(keyboard->getBounds());
#endif
}

void BKCircularKeyboardSlider::setFundamental(int fund)
{
    keyboard->setFundamental(fund);
}

void BKCircularKeyboardSlider::mouseMove(const MouseEvent& e)
{
    keyboardValueTF.setText(String(keyboard->getLastNoteOverValue(), 1), dontSendNotification);
}

void BKCircularKeyboardSlider::mouseDrag(const MouseEvent& e)
{
    if(e.y >= 0 && e.y <= keyboard->getHeight())
    {
        bool isBlackKey = MidiMessage::isMidiNoteBlack (keyboard->getLastKeySelected());
        float dragPos = (float)e.y / keyboard->getHeight();
        if(isBlackKey) dragPos /= keyboard->getBlackNoteLengthProportion();
        
        dragPos = 1. - 2. * dragPos;
        if(dragPos > 0.) dragPos = dragPos * dragPos;
        else dragPos = -1.* dragPos * dragPos;
        
        keyboardValueTF.setText(String(dragPos * 50.0, 1), dontSendNotification);
        keyboard->setKeyValue(lastKeyPressed, dragPos * 50.);
        
        DBG("dragging last key, height " + String(keyboard->getLastKeySelected()) + " " + String(dragPos));
    }
}

void BKCircularKeyboardSlider::mouseUp(const MouseEvent& e)
{
    if(e.mouseWasClicked())
    {
        if(e.mods.isShiftDown())
        {
            keyboardValueTF.setText(String(0.), dontSendNotification);
            keyboard->setKeyValue(lastKeyPressed, 0.);
        }
    }
    
    listeners.call(&BKCircularKeyboardSlider::Listener::keyboardSliderChanged,
                   getName(),
                   keyboard->getValues());
    
    keyboard->repaint();
}

void BKCircularKeyboardSlider::mouseDoubleClick(const MouseEvent& e)
{
#if JUCE_IOS
    lastKeyPressed = -1;
    lastKeyPressed = keyboard->getLastNoteOver();
    
    if (e.eventComponent == keyboard)
    {
        if (lastKeyPressed >= 0)
        {
            hasBigOne = true;
            WantsBigOne::listeners.call(&WantsBigOne::Listener::iWantTheBigOne, &keyboardValueTF,
                                        "value for note " + midiToPitchClass(lastKeyPressed));
        }
    }
#endif
}

void BKCircularKeyboardSlider::mouseDown(const MouseEvent& e)
{

    lastKeyPressed = keyboard->getLastNoteOver();
    
}

void BKCircularKeyboardSlider::bkTextFieldDidChange (TextEditor& textEditor)
{

}


void BKCircularKeyboardSlider::textEditorReturnKeyPressed(TextEditor& textEditor)
{
    if(textEditor.getName() == keyboardValsTextField->getName())
    {

        Array<float> tempVals = stringToFloatArray(keyboardValsTextField->getText());
        
        if(tempVals.size() == 12) {
            keyboard->setValuesRotatedByFundamental(tempVals);
            
            //DBG("textEditorReturnKeyPressed keyboardValsTextField");
            listeners.call(&BKCircularKeyboardSlider::Listener::keyboardSliderChanged,
                           getName(),
                           keyboard->getValues());
            
            keyboardValsTextField->setAlpha(0);
            keyboardValsTextField->toBack();
            unfocusAllComponents();
        }

       //updateDisplay();
    }
    else if(textEditor.getName() == keyboardValueTF.getName())
    {
        if (lastKeyPressed < 0) return;
        
        keyboard->setKeyValue(lastKeyPressed, keyboardValueTF.getText().getDoubleValue());
        
        //DBG("textEditorReturnKeyPressed keyboardValueTF");
        listeners.call(&BKCircularKeyboardSlider::Listener::keyboardSliderChanged,
                       getName(),
                       keyboard->getValues());
        
        keyboard->repaint();
    }

}

void BKCircularKeyboardSlider::textEditorEscapeKeyPressed (TextEditor& textEditor)
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

void BKCircularKeyboardSlider::textEditorFocusLost(TextEditor& textEditor)
{
#if !JUCE_IOS
    if(!focusLostByEscapeKey)
    {
        textEditorReturnKeyPressed(textEditor);
    }
#endif
}

void BKCircularKeyboardSlider::textEditorTextChanged(TextEditor& tf)
{
#if JUCE_IOS
    if (hasBigOne)
    {
        hasBigOne = false;
        textEditorReturnKeyPressed(tf);
    }
#endif
}

void BKCircularKeyboardSlider::handleKeymapNoteToggled (BKKeymapKeyboardState* source, int midiNoteNumber)
{

}

void BKCircularKeyboardSlider::bkButtonClicked (Button* b)
{
    if(b->getName() == keyboardValsTextFieldOpen.getName())
    {
        Array<float> newVals = keyboard->getValues();
        newVals.removeRange(12, 128);
        keyboardValsTextField->setText(floatArrayToString(newVals), dontSendNotification);
        
#if JUCE_IOS
        hasBigOne = true;
        WantsBigOne::listeners.call(&WantsBigOne::Listener::iWantTheBigOne, keyboardValsTextField, "scale offsets");
#else
        
        focusLostByEscapeKey = false;
        
        keyboardValsTextField->setAlpha(1);
        
        keyboardValsTextField->toFront(true);
#endif

    }


}


void BKCircularKeyboardSlider::setValues(Array<float> newvals)
{
    int fund = keyboard->getFundamental();
    keyboard->setFundamental(0);
    keyboard->clearKeyValues();
    
    int valCounter = 0;

    //DBG("BKCircularKeyboardSlider::setValues newvals size = " + String(newvals.size()));
    for(int i=0; i<newvals.size(); i++)
    {
        if(valCounter < newvals.size()) {
            keyboard->setKeyValue(i, newvals.getUnchecked(valCounter++));
            //DBG("new tuning val " + String(i) + " " + String(newvals.getUnchecked(valCounter - 1)));
        }
    }
    
    if(fund > -1) keyboard->setFundamental(fund);

}
