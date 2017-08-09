/*
  ==============================================================================

    BKLookAndFeel.h
    Created: 22 Nov 2016 10:43:57pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef BKLOOKANDFEEL_H_INCLUDED
#define BKLOOKANDFEEL_H_INCLUDED

#include "BKUtilities.h"

class BKTextFieldLAF : public LookAndFeel_V3
{
public:
    
    BKTextFieldLAF()
    {
        setColour(TextEditor::backgroundColourId,   Colours::goldenrod);
        setColour(TextEditor::textColourId,         Colours::black);
        
    }
};

#endif  // BKLOOKANDFEEL_H_INCLUDED
