/*
  ==============================================================================

    BKLookAndFeel.h
    Created: 22 Nov 2016 10:43:57pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef BKLOOKANDFEEL_H_INCLUDED
#define BKLOOKANDFEEL_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

class BKTextFieldLAF : public LookAndFeel_V3
{
public:
    BKTextFieldLAF()
    {
        setColour(TextEditor::backgroundColourId,   Colours::orange);
        setColour(TextEditor::textColourId,         Colours::black);
        
    }
    
    void fillTextEditorBackground (Graphics&, int width, int height, TextEditor&)
    {
        
    }
    
    void drawTextEditorOutline (Graphics&, int width, int height, TextEditor&);
    {
        
    }
};



#endif  // BKLOOKANDFEEL_H_INCLUDED
