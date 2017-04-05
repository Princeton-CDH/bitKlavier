/*
  ==============================================================================

    BKItem.h
    Created: 4 Apr 2017 8:05:05pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef BKITEM_H_INCLUDED
#define BKITEM_H_INCLUDED

#include "BKUtilities.h"
#include "BKComponent.h"

class BKItem : public BKDraggableComponent
{
public:
    BKItem(BKPreparationType type, int which, String name):
    BKDraggableComponent(true,false),
    type(type),
    which(which),
    name(name)
    {
        addAndMakeVisible(fullChild);
        
        addAndMakeVisible(label);
        
        label.setJustificationType(Justification::centred);
        label.setBorderSize(BorderSize<int>(2));
        
        label.setText(name, dontSendNotification);
        
    }
    
    ~BKItem()
    {
        
    }
    
    void paint(Graphics& g) override
    {
        g.setColour(Colours::lightblue);
        g.fillAll();
        
        g.setColour(Colours::black);
        g.drawRect(getLocalBounds());
    }
    
    void resized(void) override
    {
        label.setBounds(0,0,getWidth(),getHeight());
        fullChild.setBounds(0,0,getWidth(),getHeight());
    }


private:
    Label label;
    Component fullChild;
    
    BKPreparationType type;
    int which;
    String name;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKItem)
};



#endif  // BKITEM_H_INCLUDED
