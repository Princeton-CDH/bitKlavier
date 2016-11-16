/*
  ==============================================================================

    PreparationView.h
    Created: 15 Nov 2016 4:07:21pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef PREPARATIONVIEW_H_INCLUDED
#define PREPARATIONVIEW_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class PreparationView    : public Component
{
public:
    PreparationView()
    {
        // In your constructor, you should add any child components, and
        // initialise any special settings that your component needs.

    }

    ~PreparationView()
    {
    }

    void paint (Graphics& g) override
    {
        g.fillAll(Colours::transparentWhite);

    }

    void resized() override
    {
        

    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PreparationView)
};


#endif  // PREPARATIONVIEW_H_INCLUDED
