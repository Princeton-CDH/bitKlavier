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

#include "BKTextField.h"

#include "BKLabel.h"

#include "BKMenu.h"

#include "BKLookAndFeel.h"

//==============================================================================

class BKComponent    : public Component
{
public:

    BKComponent()
    {
        
    }
    
    ~BKComponent()
    {
        
    }
    
    void paint (Graphics& g) override
    {
        g.setColour(Colours::goldenrod);
        g.drawRect(getLocalBounds(), 1);
    }
    
    void resized() override
    {
        
    }
    
private:
    
    JUCE_LEAK_DETECTOR (BKComponent)
};

class BKDraggableComponent    : public BKComponent
{
public:
    
    BKDraggableComponent(int top, int left, int bottom, int right)
    {
        addMouseListener(this,true);
        constrainer.setMinimumOnscreenAmounts(top, left, bottom, right);
    }
    
    ~BKDraggableComponent()
    {
        
    }
    
    void setConstrainer(int top, int left, int bottom, int right)
    {
        constrainer.setMinimumOnscreenAmounts(top, left, bottom, right);
    }
    
    void prepareDrag(const MouseEvent& e)
    {
        DBG("preparedrag");
        dragger.startDraggingComponent (this, e);
    }
    
    void performDrag(const MouseEvent& e)
    {
        DBG("drag");
        dragger.dragComponent (this, e, &constrainer);

    }
    
private:
    void mouseDown (const MouseEvent& e) override
    {
        prepareDrag(e);
    }
    
    void mouseDrag (const MouseEvent& e) override
    {
        performDrag(e);
    }
    
    ComponentBoundsConstrainer constrainer;
    ComponentDragger dragger;
    
    JUCE_LEAK_DETECTOR (BKDraggableComponent)
};

#endif  // BKCOMPONENT_H_INCLUDED
