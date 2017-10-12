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


class MouseHoldListener : public Component, private Timer
{
public:
    
    MouseHoldListener()
    {
        mouseDownComponent = new Component();
        mouseDownComponent->setSize(5,5);
        addAndMakeVisible(mouseDownComponent);
        
        startTimer(50);
    }
    
    ~MouseHoldListener(void)
    {
        
    }
    
    virtual void mouseHold(Component* frame, bool on) = 0;
    
    void mouseClicked(int x, int y, Time time)
    {
        onItem = false;
        mouseIsDown = true;
        mouseDownTime = time;
        mouseHoldHappened = false;
        mouseDragHappened = false;
        mouseDownComponent->setTopLeftPosition(x, y);
    }
    
    void mouseReleased()
    {
        onItem = false;
        mouseIsDown = false;
    }
    
    void mouseDragged()
    {
        mouseDragHappened = true;
    }
    
    void setMouseDownOnItem(bool on) { onItem = on; }
    
    Component* getMouseFrame(void)
    {
        return mouseDownComponent;
    }
    
private:
    
    void timerCallback(void) override
    {
        mouseDownDuration = Time::getCurrentTime() - mouseDownTime;
        
        if (mouseIsDown && !mouseDragHappened && !mouseHoldHappened && (mouseDownDuration.inMilliseconds() > 300))
        {
            mouseHoldHappened = true;
            
            mouseHold(mouseDownComponent, onItem);
        }
    }

    Time mouseDownTime;
    RelativeTime mouseDownDuration;
    bool mouseHoldHappened;
    bool mouseDragHappened;
    bool mouseIsDown;
    ScopedPointer<Component> mouseDownComponent;
    bool onItem;
    
    
    JUCE_LEAK_DETECTOR (MouseHoldListener)
};

class BKDraggableComponent    : public BKComponent, public DragAndDropTarget
{
public:
    
    BKDraggableComponent(bool isDrag, bool isDrop, bool isConn):
    isDraggable(isDrag),
    isDroppable(isDrop),
    isConnectable(isConn)
    {
        constrainer.setMinimumOnscreenAmounts(50,50,50,50);
        addMouseListener(this,true);
    }
    
    BKDraggableComponent(bool isDrag, bool isDrop, bool isConn,
                         int top, int left, int bottom, int right):
    isDraggable(isDrag),
    isDroppable(isDrop),
    isConnectable(isConn)
    {
        constrainer.setMinimumOnscreenAmounts(top,left,bottom,right);
        addMouseListener(this,true);
    }
    
    ~BKDraggableComponent()
    {
        
    }
    
    inline void setConstrainer(int top, int left, int bottom, int right)
    {
        
    }
    
    virtual void itemWasDropped(BKPreparationType type, Array<int>, int x, int y){};
    virtual void itemIsBeingDragged(const MouseEvent& e){};
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
        dragger.dragComponent (this, e, &constrainer);
        
        itemIsBeingDragged(e);
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
    
    ComponentBoundsConstrainer constrainer;
    ComponentDragger dragger;
    
    Point<int> startPosition;
    Point<int> vector;

    
    JUCE_LEAK_DETECTOR (BKDraggableComponent)
};


#endif  // BKCOMPONENT_H_INCLUDED
