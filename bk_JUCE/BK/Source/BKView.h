/*
  ==============================================================================

    PreparationView.h
    Created: 15 Nov 2016 4:07:21pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef PREPARATIONVIEW_H_INCLUDED
#define PREPARATIONVIEW_H_INCLUDED

#include "BKLookAndFeel.h"

//==============================================================================
/*
*/
class BKView    : public Component
{
    // Doesn't do anything right now... figure this out. New virtual functions?
public:
    BKView()
    {

    }

    ~BKView()
    {
    }

    void paint (Graphics& g) override
    {

    }

    void resized() override
    {
        
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKView)
};


#endif  // PREPARATIONVIEW_H_INCLUDED
