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

class BKConstructionSite : public BKDraggableComponent
{
public:
    BKConstructionSite(BKAudioProcessor& p):
    BKDraggableComponent(false,true,false),
    processor(p),
    graph(processor)
    {
        
    }
    
    ~BKConstructionSite(void)
    {
        
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
        
        for (auto line : graph.getLines())
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
    
    BKItemGraph graph;
    
    
    // Drag interface
    void itemWasDropped(BKPreparationType type, Array<int> data, int x, int y) override
    {
        for (int i = 0; i < data.size(); i++)
        {
            BKItem::Ptr toAdd = new BKItem(type, data[i], processor);
            
            toAdd->setBounds(x, (i-1)*25 + y, 150, 20);
            
            if (!graph.contains(toAdd)) graph.add(toAdd);

            addAndMakeVisible(toAdd);
        }
    }

    void mouseDown (const MouseEvent& eo) override
    {
        MouseEvent e= eo.getEventRelativeTo(this);
        if (e.mods.isShiftDown())
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
        
    }
    
    void mouseDrag (const MouseEvent& e) override
    {
        lineEX = e.getEventRelativeTo(this).x;
        lineEY = e.getEventRelativeTo(this).y;
        
        if (connect) repaint();
    }
    
    inline BKItem* getItemAtPoint(const int X, const int Y)
    {
        BKItem* theItem = nullptr;
        
        if (itemSource != nullptr)
        {
            int which = 0;
            
            for (auto item : graph.getAllItems())
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
    
    void mouseUp (const MouseEvent& eo) override
    {
        MouseEvent e = eo.getEventRelativeTo(this);
        if (e.mods.isShiftDown())
        {
            connect = false;
            
            int X = e.x;
            int Y = e.y;
            
            itemTarget = getItemAtPoint(X, Y);
            
            if (itemTarget != nullptr)
            {
                DBG("target type: " + cPreparationTypes[itemTarget->getType()]  + " ID: " + String(itemTarget->getId()) );
                
                graph.connect(itemSource, itemTarget);
                graph.drawLine(lineOX, lineOY, X, Y);
                
                repaint();
            }
        }
        
        
    }
    
    JUCE_LEAK_DETECTOR(BKConstructionSite)
};



#endif  // BKCONSTRUCTIONSITE_H_INCLUDED
