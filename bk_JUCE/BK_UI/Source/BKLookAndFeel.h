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

class BKButtonAndMenuLAF : public LookAndFeel_V4
{
public:
    
    BKButtonAndMenuLAF()
    {
        setColour(ComboBox::backgroundColourId, Colours::black);
        setColour(ComboBox::textColourId, Colours::white);
        setColour(ComboBox::buttonColourId, Colours::black);

        setColour(PopupMenu::backgroundColourId, Colours::black);
        setColour(PopupMenu::textColourId, Colours::white);
        
        setColour(TextButton::buttonColourId, Colours::black);
        setColour(TextButton::textColourOffId, Colours::white);
        setColour(TextButton::textColourOnId, Colours::white);
        
        setColour(juce::LookAndFeel_V4::ColourScheme::outline, Colours::black);
        getCurrentColourScheme().setUIColour(juce::LookAndFeel_V4::ColourScheme::widgetBackground, Colours::yellow);
        
        
    }
};

#endif  // BKLOOKANDFEEL_H_INCLUDED
