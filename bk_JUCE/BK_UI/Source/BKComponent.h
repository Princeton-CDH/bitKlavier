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
    
    BKDraggableComponent(bool isDrag, bool isDrop, bool isConn):
    isDraggable(isDrag),
    isDroppable(isDrop),
    isConnectable(isConn)
    {
        addMouseListener(this,true);
        
        constrainer.setMinimumOnscreenAmounts(80,100,80,100);
    }
    
    BKDraggableComponent(bool isDrag, bool isDrop, bool isConn,
                         int top, int left, int bottom, int right):
    isDraggable(isDrag),
    isDroppable(isDrop),
    isConnectable(isConn)
    {
        addMouseListener(this,true);
        
        //constrainer.setMinimumOnscreenAmounts(top,left,bottom,right);
    }
    
    ~BKDraggableComponent()
    {
        
    }
    
    inline void setConstrainer(int top, int left, int bottom, int right)
    {
        //constrainer.setMinimumOnscreenAmounts(top, left, bottom, right);
    }
    
    virtual void itemWasDropped(BKPreparationType type, Array<int>, int x, int y){};
    virtual void itemIsBeingDragged(const MouseEvent& e, Point<int> startPosition){};
    virtual void itemWasDragged(const MouseEvent& e){};
    virtual void keyPressedWhileSelected(const KeyPress& e) {};
    
    bool isDragging;
    
    void prepareDrag(const MouseEvent& e)
    {
        isDragging = true;
        
        dragger.startDraggingComponent (this, e);
    }
    
    void performDrag(const MouseEvent& e)
    {
        int X = e.x, Y = e.y;
        
        dragger.dragComponent (this, e, nullptr);

    }
    
protected:
    bool itemIsHovering, isSelected;
    
    bool isDraggable, isDroppable, isConnectable;

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
        BKPreparationType type = (BKPreparationType)data[0];
        data.remove(0);
        
        Point<int> where = dragSourceDetails.localPosition;
        
        if (isDroppable)
        {
            itemWasDropped(type, data, where.getX(), where.getY());
           
            itemIsHovering = false;
            
            repaint();
        }
    }
    
    bool keyPressed(const KeyPress& e) override
    {
        if (isSelected)
        {
            keyPressedWhileSelected(e);
        }
    }
    
    ComponentBoundsConstrainer constrainer;
    ComponentDragger dragger;
    
    Point<int> startPosition;
    Point<int> vector;

    
    JUCE_LEAK_DETECTOR (BKDraggableComponent)
};


#endif  // BKCOMPONENT_H_INCLUDED
