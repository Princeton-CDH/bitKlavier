/*
  ==============================================================================

    BKNumberPad.cpp
    Created: 17 Oct 2017 6:32:32pm
    Author:  airship

  ==============================================================================
*/

#include "BKNumberPad.h"

BKNumberPad::BKNumberPad(void)
{
    for (int i = 0; i < NumberPadButtonNil; ++i)
    {
        buttons.set(i, new TextButton(bkNumberPadText[i]));
        buttons[i]->setButtonText(bkNumberPadText[i]);
        buttons[i]->addListener(this);
        addAndMakeVisible(buttons[i]);
    }
}

BKNumberPad::~BKNumberPad(void)
{
    
}

void BKNumberPad::buttonClicked(Button* button)
{    
    bool sendChange = true;
    
    if (button == buttons[NumberZero])
    {
        current += "0";
    }
    else if (button == buttons[NumberOne])
    {
        current += "1";
    }
    else if (button == buttons[NumberTwo])
    {
        current += "2";
    }
    else if (button == buttons[NumberThree])
    {
        current += "3";
    }
    else if (button == buttons[NumberFour])
    {
        current += "4";
    }
    else if (button == buttons[NumberFive])
    {
        current += "5";
    }
    else if (button == buttons[NumberSix])
    {
        current += "6";
    }
    else if (button == buttons[NumberSeven])
    {
        current += "7";
    }
    else if (button == buttons[NumberEight])
    {
        current += "8";
    }
    else if (button == buttons[NumberNine])
    {
        current += "9";
    }
    else if (button == buttons[NumberNegative])
    {
        current += "-";
    }
    else if (button == buttons[NumberDecimal])
    {
        current += ".";
    }
    else if (button == buttons[NumberColon])
    {
        current += ":";
    }
    else if (button == buttons[NumberLBracket])
    {
        current += "[";
    }
    else if (button == buttons[NumberRBracket])
    {
        current += "]";
    }
    else if (button == buttons[NumberDelete])
    {
        current = current.substring(0, current.length()-1);
    }
    else if (button == buttons[NumberSpace])
    {
        current += " ";
    }
    else if (button == buttons[NumberCancel])
    {
        current = initial;
        listeners.call(&Listener::numberPadDismissed, this);
        
        sendChange = false;
    }
    else if (button == buttons[NumberOk])
    {
        listeners.call(&Listener::numberPadDismissed, this);
        
        sendChange = false;
    }
    
    if (sendChange) listeners.call(&Listener::numberPadChanged, this);
}

void BKNumberPad::resized(void)
{
    float buttonWidth = getWidth() / 4;
    float buttonHeight = getHeight() / 5;
    
    for (int i = 0; i < NumberSpace; ++i)
    {
        buttons[i]->setBounds((i % 4) * buttonWidth, (i / 4) * buttonHeight, buttonWidth, buttonHeight);
    }
    
    buttons[NumberSpace]->setBounds(0, buttonHeight * 4, getWidth() * 0.5, buttonHeight);
    buttons[NumberCancel]->setBounds(getWidth() * 0.5, buttonHeight * 4, getWidth() * 0.25, buttonHeight);
    buttons[NumberOk]->setBounds(getWidth() * 0.75, buttonHeight * 4, getWidth() * 0.25, buttonHeight);
}

void BKNumberPad::paint(Graphics& g)
{
    g.fillAll(Colours::black);
    g.drawRoundedRectangle(getLocalBounds().toFloat(), 2, 1);
    
}
