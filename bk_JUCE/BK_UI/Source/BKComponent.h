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
        
        constrainer.setMinimumOnscreenAmounts(50,50,50,50);
    }
    
    BKDraggableComponent(bool isDrag, bool isDrop, bool isConn,
                         int top, int left, int bottom, int right):
    isDraggable(isDrag),
    isDroppable(isDrop),
    isConnectable(isConn)
    {
        addMouseListener(this,true);
        
        constrainer.setMinimumOnscreenAmounts(top,left,bottom,right);
    }
    
    ~BKDraggableComponent()
    {
        
    }
    
    inline void setConstrainer(int top, int left, int bottom, int right)
    {
        constrainer.setMinimumOnscreenAmounts(top, left, bottom, right);
    }
    
    virtual void itemWasDropped(BKPreparationType type, Array<int>, int x, int y){};
    virtual void itemIsBeingDragged(const MouseEvent& e){};
    
    
protected:
    bool itemIsHovering, isSelected;
    
    void prepareDrag(const MouseEvent& e)
    {
        dragger.startDraggingComponent (this, e);
    }
    
    void performDrag(const MouseEvent& e)
    {
        dragger.dragComponent (this, e, &constrainer);
        
        itemIsBeingDragged(e);
    }
    
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
    
    void mouseDown (const MouseEvent& e) override
    {
        if (isConnectable && e.mods.isShiftDown())
        {
            // start drawing line
        }
        else if (isDraggable)
        {
            prepareDrag(e);
        }

        isSelected = true;
    }
    
    void mouseDrag (const MouseEvent& e) override
    {
        if (e.mods.isShiftDown())
        {
            
        }
        else if (isDraggable)
        {
            performDrag(e);
        }
    }
    
    void mouseUp(const MouseEvent& e) override
    {
        
    }
    
    ComponentBoundsConstrainer constrainer;
    ComponentDragger dragger;
    
    
    bool isDraggable, isDroppable, isConnectable;
    
    JUCE_LEAK_DETECTOR (BKDraggableComponent)
};

/*
class ConnectorComponent   : public Component
{
public:
    ConnectorComponent (FilterGraph& graph_)
    : sourceItemId (0),
    destItemId (0),
    sourceFilterChannel (0),
    destFilterChannel (0),
    graph (graph_),
    lastInputX (0),
    lastInputY (0),
    lastOutputX (0),
    lastOutputY (0)
    {
        setAlwaysOnTop (true);
    }
    
    void setInput (const uint32 sourceItemId_, const int sourceFilterChannel_)
    {
        if (sourceItemId != sourceItemId_ || sourceFilterChannel != sourceFilterChannel_)
        {
            sourceItemId = sourceItemId_;
            sourceFilterChannel = sourceFilterChannel_;
            update();
        }
    }
    
    void setOutput (const uint32 destItemId_, const int destFilterChannel_)
    {
        if (destItemId != destItemId_ || destFilterChannel != destFilterChannel_)
        {
            destItemId = destItemId_;
            destFilterChannel = destFilterChannel_;
            update();
        }
    }
    
    void dragStart (int x, int y)
    {
        lastInputX = (float) x;
        lastInputY = (float) y;
        resizeToFit();
    }
    
    void dragEnd (int x, int y)
    {
        lastOutputX = (float) x;
        lastOutputY = (float) y;
        resizeToFit();
    }
    
    void update()
    {
        float x1, y1, x2, y2;
        getPoints (x1, y1, x2, y2);
        
        if (lastInputX != x1
            || lastInputY != y1
            || lastOutputX != x2
            || lastOutputY != y2)
        {
            resizeToFit();
        }
    }
    
    void resizeToFit()
    {
        float x1, y1, x2, y2;
        getPoints (x1, y1, x2, y2);
        
        const Rectangle<int> newBounds ((int) jmin (x1, x2) - 4,
                                        (int) jmin (y1, y2) - 4,
                                        (int) std::abs (x1 - x2) + 8,
                                        (int) std::abs (y1 - y2) + 8);
        
        if (newBounds != getBounds())
            setBounds (newBounds);
        else
            resized();
        
        repaint();
    }
    
    void getPoints (float& x1, float& y1, float& x2, float& y2) const
    {
        x1 = lastInputX;
        y1 = lastInputY;
        x2 = lastOutputX;
        y2 = lastOutputY;
        
        if (GraphEditorPanel* const hostPanel = getGraphPanel())
        {
            if (FilterComponent* srcFilterComp = hostPanel->getComponentForFilter (sourceItemId))
                srcFilterComp->getPinPos (sourceFilterChannel, false, x1, y1);
            
            if (FilterComponent* dstFilterComp = hostPanel->getComponentForFilter (destItemId))
                dstFilterComp->getPinPos (destFilterChannel, true, x2, y2);
        }
    }
    
    void paint (Graphics& g) override
    {
        if (sourceFilterChannel == FilterGraph::midiChannelNumber
            || destFilterChannel == FilterGraph::midiChannelNumber)
        {
            g.setColour (Colours::red);
        }
        else
        {
            g.setColour (Colours::green);
        }
        
        g.fillPath (linePath);
    }
    
    bool hitTest (int x, int y) override
    {
        if (hitPath.contains ((float) x, (float) y))
        {
            double distanceFromStart, distanceFromEnd;
            getDistancesFromEnds (x, y, distanceFromStart, distanceFromEnd);
            
            // avoid clicking the connector when over a pin
            return distanceFromStart > 7.0 && distanceFromEnd > 7.0;
        }
        
        return false;
    }
    
    void mouseDown (const MouseEvent&) override
    {
        dragging = false;
    }
    
    void mouseDrag (const MouseEvent& e) override
    {
        if (dragging)
        {
            getGraphPanel()->dragConnector (e);
        }
        else if (e.mouseWasDraggedSinceMouseDown())
        {
            dragging = true;
            
            graph.removeConnection (sourceItemId, sourceFilterChannel, destItemId, destFilterChannel);
            
            double distanceFromStart, distanceFromEnd;
            getDistancesFromEnds (e.x, e.y, distanceFromStart, distanceFromEnd);
            const bool isNearerSource = (distanceFromStart < distanceFromEnd);
            
            getGraphPanel()->beginConnectorDrag (isNearerSource ? 0 : sourceItemId,
                                                 sourceFilterChannel,
                                                 isNearerSource ? destItemId : 0,
                                                 destFilterChannel,
                                                 e);
        }
    }
    
    void mouseUp (const MouseEvent& e) override
    {
        if (dragging)
            getGraphPanel()->endDraggingConnector (e);
    }
    
    void resized() override
    {
        float x1, y1, x2, y2;
        getPoints (x1, y1, x2, y2);
        
        lastInputX = x1;
        lastInputY = y1;
        lastOutputX = x2;
        lastOutputY = y2;
        
        x1 -= getX();
        y1 -= getY();
        x2 -= getX();
        y2 -= getY();
        
        linePath.clear();
        linePath.startNewSubPath (x1, y1);
        linePath.cubicTo (x1, y1 + (y2 - y1) * 0.33f,
                          x2, y1 + (y2 - y1) * 0.66f,
                          x2, y2);
        
        PathStrokeType wideStroke (8.0f);
        wideStroke.createStrokedPath (hitPath, linePath);
        
        PathStrokeType stroke (2.5f);
        stroke.createStrokedPath (linePath, linePath);
        
        const float arrowW = 5.0f;
        const float arrowL = 4.0f;
        
        Path arrow;
        arrow.addTriangle (-arrowL, arrowW,
                           -arrowL, -arrowW,
                           arrowL, 0.0f);
        
        arrow.applyTransform (AffineTransform()
                              .rotated (float_Pi * 0.5f - (float) atan2 (x2 - x1, y2 - y1))
                              .translated ((x1 + x2) * 0.5f,
                                           (y1 + y2) * 0.5f));
        
        linePath.addPath (arrow);
        linePath.setUsingNonZeroWinding (true);
    }
    
    uint32 sourceItemId, destItemId;
    int sourceFilterChannel, destFilterChannel;
    
private:
    Array<Array<int>>& graph;
    float lastInputX, lastInputY, lastOutputX, lastOutputY;
    Path linePath, hitPath;
    bool dragging;
    
    GraphEditorPanel* getGraphPanel() const noexcept
    {
        return findParentComponentOfClass<GraphEditorPanel>();
    }
    
    void getDistancesFromEnds (int x, int y, double& distanceFromStart, double& distanceFromEnd) const
    {
        float x1, y1, x2, y2;
        getPoints (x1, y1, x2, y2);
        
        distanceFromStart = juce_hypot (x - (x1 - getX()), y - (y1 - getY()));
        distanceFromEnd = juce_hypot (x - (x2 - getX()), y - (y2 - getY()));
    }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ConnectorComponent)
};

*/

#endif  // BKCOMPONENT_H_INCLUDED
