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

class BKDraggableComponent    : public BKComponent, public DragAndDropTarget
{
public:
    
    BKDraggableComponent(bool isDrag, bool isDrop):
    isDraggable(isDrag),
    isDroppable(isDrop)
    {
        addMouseListener(this,true);
        
        constrainer.setMinimumOnscreenAmounts(50,50,50,50);
    }
    
    BKDraggableComponent(bool isDrag, bool isDrop,
                         int top, int left, int bottom, int right):
    isDraggable(isDrag),
    isDroppable(isDrop)
    {
        addMouseListener(this,true);
        
        constrainer.setMinimumOnscreenAmounts(top,left,bottom,right);
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
        dragger.startDraggingComponent (this, e);
    }
    
    void performDrag(const MouseEvent& e)
    {
        dragger.dragComponent (this, e, &constrainer);
    }
    
    virtual void itemWasDropped(Array<int>){};
    
protected:
    bool itemIsHovering, isSelected;
    
private:
    // Drag interface
    bool isInterestedInDragSource (const SourceDetails& /*dragSourceDetails*/) override
    {
        // should check drag source to see if its what we want...
        return true;
    }
    
    void itemDragEnter (const SourceDetails& /*dragSourceDetails*/) override
    {
        itemIsHovering = true;
        if (isDroppable) repaint();
    }
    
    void itemDragMove (const SourceDetails& /*dragSourceDetails*/) override
    {
        
    }
    
    void itemDragExit (const SourceDetails& /*dragSourceDetails*/) override
    {
        itemIsHovering = false;
        if (isDroppable) repaint();
    }
    
    void itemDropped (const SourceDetails& dragSourceDetails) override
    {
        String received = dragSourceDetails.description.toString();
        DBG("Items dropped: " + received);
        
        Array<int> data = stringToIntArray(received);
        
        if (isDroppable)
        {
            itemWasDropped(data);
           
            itemIsHovering = false;
            
            repaint();
        }
    }
    
    void mouseDown (const MouseEvent& e) override
    {
        if (isDraggable && e.mods.isShiftDown())
        {
            prepareDrag(e);
        }
        else
        {
            isSelected = true;
            
            repaint();
        }
    }
    
    void mouseDrag (const MouseEvent& e) override
    {
        if (isDraggable && e.mods.isShiftDown()) performDrag(e);
    }
    
    ComponentBoundsConstrainer constrainer;
    ComponentDragger dragger;
    
    
    bool isDraggable, isDroppable;
    
    JUCE_LEAK_DETECTOR (BKDraggableComponent)
};

#endif  // BKCOMPONENT_H_INCLUDED
