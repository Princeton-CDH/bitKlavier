/*
  ==============================================================================

    BKTextField.h
    Created: 15 Nov 2016 4:19:33pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef BKTEXTFIELD_H_INCLUDED
#define BKTEXTFIELD_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class BKTextField    : public TextEditor
{
public:
    BKTextField()
    {
        setSize(150,20);
        setColour(TextEditor::backgroundColourId, Colours::lightgoldenrodyellow);
        setColour(TextEditor::textColourId, Colours::black);
        
        lookAndFeelChanged();
        
        
    }

    ~BKTextField()
    {
        
    }


private: 
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKTextField)
};


#endif  // BKTEXTFIELD_H_INCLUDED
