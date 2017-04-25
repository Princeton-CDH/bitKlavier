/*
  ==============================================================================

    BKConstructionSite.h
    Created: 4 Apr 2017 5:46:38pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef BKCONSTRUCTIONSITE_H_INCLUDED
#define BKCONSTRUCTIONSITE_H_INCLUDED

#include "BKUtilities.h"

#include "BKListener.h"
#include "BKComponent.h"

#include "PluginProcessor.h"

#include "BKGraph.h"

class BKConstructionSite : public BKDraggableComponent, public KeyListener
{
public:
    BKConstructionSite(BKAudioProcessor& p, BKItemGraph* theGraph):
    BKDraggableComponent(false,true,false),
    processor(p),
    graph(theGraph)
    {
        addKeyListener(this);
        
        setWantsKeyboardFocus(true);
        
        graph->deselectAll();
        
        redraw();
    }
    
    ~BKConstructionSite(void)
    {
        
    }
    
    inline void redraw(void)
    {
        
        BKItem::RCArr items = graph->getAllItems();
       
        for (auto item : items)
        {
            graph->removeUI(item);
        }
        
        removeAllChildren();
        
        graph->reconstruct();
        
        processor.currentPiano->configuration->print();
        
        draw();
        
    }

    void paint(Graphics& g) override
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
    
    inline void itemIsBeingDragged(BKItem* thisItem, Point<int> startPosition)
    {
        repaint();
    }
    
private:
    BKAudioProcessor& processor;
    
    Point<int> lastPosition;

    bool connect; int lineOX, lineOY, lineEX, lineEY;
    bool multiple;
    
    BKItem* itemSource;
    BKItem* itemTarget;
    BKItem* itemToSelect;
    BKItem* lastItem;
    
    BKItemGraph* graph;
    
#define AUTO_DRAW 0
#define NUM_COL 6
    void draw(void)
    {
        int keymapCount = 0, prepCount = 0, otherCount = 0, modCount = 0;
        for (auto item : graph->getAllItems())
        {
            BKPreparationType type = item->getType();
            int which = item->getId();
            
            Point<int> xy = processor.currentPiano->configuration->getXY(type, which);
            
            int x = xy.x; int y = xy.y;
            
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
        
        processor.currentPiano->configuration->print();
        
        repaint();
    }
    
    
    inline void prepareItemDrag(BKItem* item, const MouseEvent& e, bool center)
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
    void itemWasDropped(BKPreparationType type, Array<int> data, int x, int y) override
    {
        for (int i = 0; i < data.size(); i++)
        {
            BKItem::Ptr toAdd = new BKItem(type, data[i], processor);
            
            toAdd->setItemBounds(x, (i-1)*25 + y, 150, 20);
            
            if (type == PreparationTypeReset || type == PreparationTypePianoMap || !graph->contains(toAdd)) graph->add(toAdd);

            addAndMakeVisible(toAdd);
        }
    }

    void mouseDown (const MouseEvent& eo) override
    {
        
        MouseEvent e = eo.getEventRelativeTo(this);
        if (e.mods.isCommandDown())
        {
            // begin connector drag
            
            itemSource = dynamic_cast<BKItem*> (e.originalComponent->getParentComponent());
            
            if (itemSource != nullptr)
            {
                connect = true;
                
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
    
    void mouseUp (const MouseEvent& eo) override
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
                item->isDragging = false;
            }
        }
        
    }
    
    void mouseDrag (const MouseEvent& e) override
    {
        for (auto item : graph->getSelectedItems())
            item->performDrag(e);
        repaint();
    }
    
    inline void deleteItem (BKItem* item)
    {
        graph->remove(item);
        removeChildComponent(item);
    }
    
    bool keyPressed (const KeyPress& e, Component*) override
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
    
    inline BKItem* getItemAtPoint(const int X, const int Y)
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
    
    
    JUCE_LEAK_DETECTOR(BKConstructionSite)
};



#endif  // BKCONSTRUCTIONSITE_H_INCLUDED
