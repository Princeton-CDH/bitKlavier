/*
  ==============================================================================

    BKLabel.h
    Created: 15 Nov 2016 4:26:25pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef BKLABEL_H_INCLUDED
#define BKLABEL_H_INCLUDED

#include "BKLookAndFeel.h"
#include "BKComponent.h"

//==============================================================================
/*
*/
class BKLabel    : public Label
{
public:
    BKLabel()
    {
        setSize(150,18);
        setColour(Label::backgroundColourId, Colours::transparentWhite);
        setColour(Label::textColourId, Colours::antiquewhite);
        
        
        lookAndFeelChanged();
        
    }

    ~BKLabel()
    {
        
    }


private:
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKLabel)
};


#endif  // BKLABEL_H_INCLUDED
