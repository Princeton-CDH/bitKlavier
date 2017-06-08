/*
  ==============================================================================

    BKConstructionSite.cpp
    Created: 4 Apr 2017 5:46:38pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "BKConstructionSite.h"

#define AUTO_DRAW 0
#define NUM_COL 6

BKConstructionSite::BKConstructionSite(BKAudioProcessor& p, BKItemGraph* theGraph, Viewport* viewPort):
BKDraggableComponent(false,true,false),
processor(p),
graph(theGraph),
viewPort(viewPort)
{
    addKeyListener(this);
    
    setWantsKeyboardFocus(true);
    
    graph->deselectAll();
    
    redraw();
    
    leftMost = 0;
    rightMost = 1000;
    topMost = 0;
    bottomMost = 700;
    
    setBounds(leftMost, topMost, rightMost, bottomMost);
}

BKConstructionSite::~BKConstructionSite(void)
{
    
}

void BKConstructionSite::redraw(void)
{
    BKItem::RCArr items = graph->getAllItems();
    
    for (auto item : items)
    {
        graph->removeUI(item);
    }
    
    removeAllChildren();
    
    graph->reconstruct();
    
    draw();
    
}

void BKConstructionSite::paint(Graphics& g)
{
    g.setColour(Colours::lightgrey);
    g.fillAll();
    
    
    g.setColour(Colours::white);
    g.drawRect(getLocalBounds(), 1);
    
    if (itemIsHovering)
    {
        g.setColour(Colours::black);
        g.drawRect(getLocalBounds(), 3);
    }
    
    if (connect)
    {
        g.setColour(Colours::white);
        g.drawLine(lineOX, lineOY, lineEX, lineEY, 3);
    }
    
    for (auto line : graph->getLines())
    {
        g.setColour(Colours::black);
        g.drawLine(line.getStartX(), line.getStartY(), line.getEndX(), line.getEndY());
    }
}

void BKConstructionSite::itemIsBeingDragged(BKItem* thisItem, Point<int> startPosition)
{
    repaint();
}

void BKConstructionSite::pianoMapDidChange(BKItem* thisItem)
{
    graph->update(PreparationTypePianoMap,thisItem->getId());
}

void BKConstructionSite::draw(void)
{
    int keymapCount = 0, prepCount = 0, otherCount = 0, modCount = 0;
    for (auto item : graph->getAllItems())
    {
        BKPreparationType type = item->getType();
        int which = item->getId();
        
        Point<int> xy = processor.currentPiano->configuration->getXY(type, which);
        
        int x = xy.x; int y = xy.y;
        
        DBG("DRAW X: " + String(x) + " Y: " + String(y));
        
        if (type == PreparationTypeKeymap)
        {
#if AUTO_DRAW
            int col = (int)(keymapCount / NUM_COL);
            int row = keymapCount % NUM_COL;
            
            int X = 10 + (row * 155);
            int Y = 50 + (col * 25);
            
            item->setItemBounds(X, Y, 100, 40);
            
            keymapCount++;
#else
            
            item->setBounds(x, y, 100, 40);
#endif
            
        }
        else if (type <= PreparationTypeNostalgic)
        {
#if AUTO_DRAW
            int col = (int)(prepCount / NUM_COL);
            int row = prepCount % NUM_COL;
            
            int X = 10 + (row * 155);
            int Y = 200 + (col * 25);
            
            item->setItemBounds(X, Y, 150, 20);
            
            prepCount++;
#else
            item->setBounds(x, y, 150, 20);
#endif
            
        }
        else if (type > PreparationTypeKeymap)
        {
#if AUTO_DRAW
            int col = (int)(modCount / NUM_COL);
            int row = modCount % NUM_COL;
            
            int X = 95 + (row * 155);
            int Y = 125 + (col * 25);
            
            
            item->setItemBounds(X, Y, 130, 20);
            
            modCount++;
#else
            item->setBounds(x, y, 130, 20);
#endif
        }
        else
        {
#if AUTO_DRAW
            int col = (int)(otherCount / NUM_COL);
            int row = otherCount % NUM_COL;
            
            int X = 10 + (row * 155);
            int Y = 350 + (col * 25);
            
            item->setItemBounds(X, Y, 150, 20);
            
            otherCount++;
#else
            item->setBounds(x, y, 150, 20);
#endif
            
        }
        
        addAndMakeVisible(item);
    }
    
    repaint();
}


void BKConstructionSite::prepareItemDrag(BKItem* item, const MouseEvent& e, bool center)
{
    if (center)
    {
        float X = item->getPosition().getX() + item->getWidth() / 2.0f;
        float Y = item->getPosition().getY() + item->getHeight() / 2.0f;
        Point<float>pos(X,Y);
        MouseEvent newEvent = e.withNewPosition(pos);
        
        item->prepareDrag(newEvent);
    }
    else
    {
        item->prepareDrag(e);
    }
}

// Drag interface
void BKConstructionSite::itemWasDropped(BKPreparationType type, Array<int> data, int x, int y)
{
    for (int i = 0; i < data.size(); i++)
    {
        BKItem::Ptr toAdd = new BKItem(type, data[i], processor);
        
        toAdd->setItemBounds(x, (i-1)*25 + y, 150, 20);
        
        if (type == PreparationTypeReset || type == PreparationTypePianoMap || !graph->contains(toAdd)) graph->add(toAdd);
        
        addAndMakeVisible(toAdd);
    }
}

void BKConstructionSite::mouseDown (const MouseEvent& eo)
{
    MouseEvent e = eo.getEventRelativeTo(this);
    
    //addAndMakeVisible(lasso = new LassoComponent<BKItem>());
    
    if (e.mods.isCommandDown())
    {
        // begin connector drag
        
        itemSource = dynamic_cast<BKItem*> (e.originalComponent->getParentComponent());
        
        if (itemSource != nullptr)
        {
            connect = true;
            
            DBG("ORIGIN: " + String(lineOX) + " " + String(lineOY));
            lineOX = e.x;
            lineOY = e.y;
        }
    }
    else if (e.mods.isShiftDown())
    {
        // also select this item
        itemToSelect = dynamic_cast<BKItem*> (e.originalComponent->getParentComponent());
        
        if (itemToSelect != nullptr)
        {
            if (!itemToSelect->getSelected())   graph->select(itemToSelect);
            else                                graph->deselect(itemToSelect);
        }
        
        for (auto item : graph->getSelectedItems())
        {
            prepareItemDrag(item, e, true);
        }
        
    }
    else
    {
        itemToSelect = dynamic_cast<BKItem*> (e.originalComponent->getParentComponent());
        
        if (itemToSelect != nullptr)
        {
            if (!itemToSelect->getSelected())
            {
                graph->deselectAll();
                graph->select(itemToSelect);
            }
        }
        else
        {
            graph->deselectAll();
        }
        
        for (auto item : graph->getSelectedItems())
        {
            prepareItemDrag(item, e, true);
        }
    }
    
}

void BKConstructionSite::mouseUp (const MouseEvent& eo)
{
    MouseEvent e = eo.getEventRelativeTo(this);
    
    connect = false;
    
    if (e.mods.isCommandDown())
    {
        
        int X = e.x;
        int Y = e.y;
        
        itemTarget = getItemAtPoint(X, Y);
        
        if (itemTarget != nullptr)
        {
            graph->connect(itemSource, itemTarget);
            graph->drawLine(lineOX, lineOY, X, Y);
            
            repaint();
        }
    }
    
    for (auto item : graph->getSelectedItems())
    {
        if (item->isDragging)
        {
            int X = item->getX(); int Y = item->getY();
            DBG("SET X: " + String(X) + " Y: " + String(Y));
            processor.currentPiano->configuration->setItemXY(item->getType(), item->getId(), X, Y);
            item->isDragging = false;
        }
    }
    
}

void BKConstructionSite::mouseDrag (const MouseEvent& e)
{
    if (!e.mods.isCommandDown())
    {
        bool resizeX = false, resizeY = false;
        for (auto item : graph->getSelectedItems())
        {
            item->performDrag(e);
            
            int X = item->getX(); int Y = item->getY();
            
            if (X < viewPort->getViewPositionX() || X > viewPort->getViewWidth())
            {
                if (X < leftMost) leftMost = X;
                else if (X > rightMost) rightMost = X;
                resizeX = true;
            }
            
            if (Y < viewPort->getViewPositionY() || Y > viewPort->getViewHeight())
            {
                if (Y < topMost)    topMost = Y;
                else if (Y > bottomMost) bottomMost = Y;
                resizeY = true;
            }
        }
     
        setBounds(leftMost, topMost, rightMost - leftMost, bottomMost - topMost);
        
        /*
        if (resizeX)
        {
            for (auto item : graph->getSelectedItems())
            {
                item->setTopLeftPosition(item->getX()+10, item->getY());
            }
            resizeX = false;
        }
        
        if (resizeY)
        {
            setSize(getWidth(), getHeight()+abs(offsetY));
            for (auto item : graph->getSelectedItems())
            {
                item->setTopLeftPosition(item->getX(), item->getY()+10);

            }
            resizeY = false;
        }
         */
    }
    
    lineEX = e.getEventRelativeTo(this).x;
    lineEY = e.getEventRelativeTo(this).y;
    
    DBG("DRAG: "+String(lineEX) + " " +String(lineEY));
    
    repaint();
}

void BKConstructionSite::deleteItem (BKItem* item)
{
    graph->remove(item);
    removeChildComponent(item);
}

bool BKConstructionSite::keyPressed (const KeyPress& e, Component*)
{
    DBG(String(e.getKeyCode()));
    
    if (e.isKeyCode(127))
    {
        BKItem::PtrArr selectedItems = graph->getSelectedItems();
        
        for (int i = selectedItems.size(); --i >= 0;)
        {
            deleteItem(selectedItems[i]);
        }
        
        repaint();
    }
}

BKItem* BKConstructionSite::getItemAtPoint(const int X, const int Y)
{
    BKItem* theItem = nullptr;
    
    if (itemSource != nullptr)
    {
        int which = 0;
        
        for (auto item : graph->getAllItems())
        {
            int left = item->getX(); int right = left + item->getWidth();
            int top = item->getY(); int bottom = top + item->getHeight();
            
            if (X >= left && X <= right && Y >= top && Y <= bottom) // is in item
            {
                theItem = dynamic_cast<BKItem*> (item);
                break;
            }
            
            which++;
        }
    }
    
    return theItem;
}
