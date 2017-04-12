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
            graph->remove(item);
        }
        
        removeAllChildren();
        
        graph->reconstruct();
        
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
            DBG("drawStartX: " + String(line.getStartX()) + " drawStartY: " + String(line.getStartY()) +
                " drawEndX: " + String(line.getEndX()) + " drawEndY: " + String(line.getEndY()));
            
            g.setColour(Colours::black);
            g.drawLine(line.getStartX(), line.getStartY(), line.getEndX(), line.getEndY());
        }
    }
    
private:
    BKAudioProcessor& processor;
    

    bool connect; int lineOX, lineOY, lineEX, lineEY;
    
    BKItem* itemSource;
    BKItem* itemTarget;
    BKItem* itemToSelect;
    
    BKItemGraph* graph;
    
    
#define NUM_COL 6
    void draw(void)
    {
        int keymapCount = 0, prepCount = 0, otherCount = 0, modCount = 0;
        for (auto item : graph->getAllItems())
        {
            BKPreparationType type = item->getType();
            int which = item->getId();
            
            if (type == PreparationTypeKeymap)
            {
                int col = (int)(keymapCount / NUM_COL);
                int row = keymapCount % NUM_COL;
                
                item->setBounds(10 + (row * 155), 50 + (col * 25), 150, 20);

                keymapCount++;
            }
            else if (type <= PreparationTypeNostalgic)
            {
                int col = (int)(prepCount / NUM_COL);
                int row = prepCount % NUM_COL;
                
                item->setBounds(10 + (row * 155), 200 + (col * 25), 150, 20);
                
                prepCount++;
            }
            else if (type > PreparationTypeKeymap)
            {
                int col = (int)(modCount / NUM_COL);
                int row = modCount % NUM_COL;
                
                item->setBounds(95 + (row * 155), 125 + (col * 25), 130, 20);
                
                modCount++;
            }
            else
            {
                int col = (int)(otherCount / NUM_COL);
                int row = otherCount % NUM_COL;
                
                item->setBounds(10 + (row * 155), 350 + (col * 25), 150, 20);
                otherCount++;
            }
            
            addAndMakeVisible(item);
        }
        
        repaint();
    }
    
    
    
    // Drag interface
    void itemWasDropped(BKPreparationType type, Array<int> data, int x, int y) override
    {
        for (int i = 0; i < data.size(); i++)
        {
            BKItem::Ptr toAdd = new BKItem(type, data[i], processor);
            
            toAdd->setBounds(x, (i-1)*25 + y, 150, 20);
            
            if (type == PreparationTypeReset || type == PreparationTypePianoMap || !graph->contains(toAdd)) graph->add(toAdd);

            addAndMakeVisible(toAdd);
        }
    }

    void mouseDown (const MouseEvent& eo) override
    {
        itemToSelect = nullptr;
        
        MouseEvent e = eo.getEventRelativeTo(this);
        if (e.mods.isCommandDown())
        {
            // begin connector drag
            
            itemSource = dynamic_cast<BKItem*> (e.originalComponent->getParentComponent());
            
            if (itemSource != nullptr)
            {
                DBG("source type: " + cPreparationTypes[itemSource->getType()]  + " ID: " + String(itemSource->getId()) );
                
                connect = true;
                
                lineOX = e.x;
                lineOY = e.y;
            }
        }
        else if (e.mods.isShiftDown())
        {
            // also select this item
            itemToSelect = dynamic_cast<BKItem*> (e.originalComponent->getParentComponent());
            
            if (itemToSelect != nullptr && !itemToSelect->getSelected())
                graph->select(dynamic_cast<BKItem*> (e.originalComponent->getParentComponent()));
        }
        else
        {
            // deselect all other items
            graph->deselectAll();
            
            itemToSelect = dynamic_cast<BKItem*> (e.originalComponent->getParentComponent());
            
            if (itemToSelect != nullptr && !itemToSelect->getSelected())
            {
                graph->select(itemToSelect);
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
                DBG("target type: " + cPreparationTypes[itemTarget->getType()]  + " ID: " + String(itemTarget->getId()) );
                
                graph->connect(itemSource, itemTarget);
                graph->drawLine(lineOX, lineOY, X, Y);
                
                repaint();
            }
        }
        
        
    }
    
    void mouseDrag (const MouseEvent& e) override
    {
        lineEX = e.getEventRelativeTo(this).x;
        lineEY = e.getEventRelativeTo(this).y;
        
        if (connect) repaint();
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
                    // start from this item
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
