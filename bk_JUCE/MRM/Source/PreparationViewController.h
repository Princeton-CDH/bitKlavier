/*
  ==============================================================================

    PreparationViewController.h
    Created: 20 Nov 2016 11:06:14am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef PREPARATIONVIEWCONTROLLER_H_INCLUDED
#define PREPARATIONVIEWCONTROLLER_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

#include "PluginProcessor.h"

//==============================================================================
/*
*/
class PreparationViewController    : public Component
{
public:
    PreparationViewController();
    ~PreparationViewController();

    void paint (Graphics&) override;
    void resized() override;
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PreparationViewController)
};


#endif  // PREPARATIONVIEWCONTROLLER_H_INCLUDED
