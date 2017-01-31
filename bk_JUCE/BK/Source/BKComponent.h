/*
  ==============================================================================

    BKComponent.h
    Created: 31 Jan 2017 11:02:14am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef BKCOMPONENT_H_INCLUDED
#define BKCOMPONENT_H_INCLUDED

#include "BKUtilities.h"
#include "BKView.h"
//==============================================================================

class BKComponent    : public BKView
{
public:

    BKComponent();
    ~BKComponent();
    
    void paint (Graphics&) override;
    void resized() override;
    
private:
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKComponent)
};


#endif  // BKCOMPONENT_H_INCLUDED
