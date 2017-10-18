/*
  ==============================================================================

    BKNumberPad.h
    Created: 17 Oct 2017 6:32:32pm
    Author:  airship

  ==============================================================================
*/

#pragma once

#include "BKUtilities.h"


typedef enum NumberPadButtonType
{
    NumberOne = 0,
    NumberTwo,
    NumberThree,
    NumberDelete,
    
    NumberFour,
    NumberFive,
    NumberSix,
    NumberColon,
    
    NumberSeven,
    NumberEight,
    NumberNine,
    NumberLBracket,
    
    NumberNegative,
    NumberZero,
    NumberDecimal,
    NumberRBracket,
    
    NumberSpace,
    NumberCancel,
    NumberOk,
    
    NumberPadButtonNil
    
} NumberPadButtonType;

static const std::vector<std::string> bkNumberPadText = {
    "1",
    "2",
    "3",
    "Delete",
    
    "4",
    "5",
    "6",
    ":",
    
    "7",
    "8",
    "9",
    "[",
    
    "-",
    "0",
    ".",
    "]",
    
    "Space",
    "Cancel",
    "Ok"
};


class BKNumberPad : public Component, public TextButton::Listener
{
public:
    
    BKNumberPad(void);
    ~BKNumberPad(void);
    
    void resized(void) override;
    void paint(Graphics& g) override;
    
    void setEnabled(NumberPadButtonType type, bool enabled)
    {
        buttons[(int)type]->setEnabled(enabled);
    }
    class Listener
    {
    public:
        
        virtual ~Listener() {};
        
        virtual void numberPadChanged(BKNumberPad*) {};
        virtual void numberPadDismissed(BKNumberPad*) {};
    };
    
    void addListener(Listener* listener)     { listeners.add(listener);      }
    void removeListener(Listener* listener)  { listeners.remove(listener);   }
    
    inline void setText(String text)
    {
        initial = text;
        current = text;
    }
    
    inline String getText(void)
    {
        return current;
    }

private:
    
    String initial;
    String current;
    
    void buttonClicked(Button*) override;
    
    OwnedArray<TextButton> buttons;
    
    ListenerList<Listener> listeners;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BKNumberPad)
};


