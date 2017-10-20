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
    int start = target->getHighlightedRegion().getStart();
    int end =   target->getHighlightedRegion().getEnd();
    
    DBG("start: " + String(start) + " end: " + String(end));
    
    for (int i = 0; i < NumberPadButtonNil; ++i)
    {
        if (button == buttons[i])
        {
            if (i == NumberDelete)
            {
                if (end != start)   current = current.replaceSection(start, (end-1)-start, "");
                
                DBG("current here: " + String(current));
                
                current = current.replaceSection(target->getCaretPosition()-1, 1, "");
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
                current += bkNumberPadTextToInsert[i];
            }
            
            target->setText(current, dontSendNotification);
            
            break;
        }
       
    }
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
