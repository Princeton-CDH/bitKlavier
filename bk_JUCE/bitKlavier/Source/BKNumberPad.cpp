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

void BKNumberPad::setTarget(TextEditor* tf)
{
    target = tf;
}

void BKNumberPad::buttonClicked(Button* button)
{
    for (int i = 0; i < NumberPadButtonNil; ++i)
    {
        if (button == buttons[i]) // found right index
        {
            if (i == NumberDelete)
            {
                int pos = target->getCaretPosition();
                
                String sub1 = target->getText().substring(0, pos - 1), sub2 = target->getText().substring(pos + 1);
                current = sub1+sub2;
                
                target->setCaretPosition(pos-1);
            }
            else if (i == NumberCancel)
            {
                current = initial;
                listeners.call(&Listener::numberPadDismissed, this);
            }
            else if (i == NumberOk)
            {
                listeners.call(&Listener::numberPadDismissed, this);
            }
            else
            {
                target->insertTextAtCaret(bkNumberPadTextToInsert[i]);
                current = target->getText();
                target->setText(current);
                
            }
        }
        
    }
    DBG("current: " + current);
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
