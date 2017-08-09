/*
  ==============================================================================

    BKItemNode.h
    Created: 14 Jul 2017 10:33:43am
    Author:  airship

  ==============================================================================
*/

#pragma once

#include "BKUtilities.h"

class BKItemNode : public Component
{
public:
    
    BKItemNode(NodeOrientation o):
    orientation(o)
    {
        centerX = getWidth()/2;
        centerY = 0;
        radiusX = getWidth()/2;
        radiusY = getHeight();
        rotation = 0;
        startR = -float_Pi/2;
        endR = float_Pi/2;
    }
    
    ~BKItemNode()
    {
        
    }
    
    void paint(Graphics& g) override
    {
        g.fillAll(Colours::lightgrey);
        
        
        /*
        Path node;
        node.addCentredArc(centerX, centerY, radiusX, radiusY,
                           rotation, startR, endR);
        g.fillPath(node);
        node.closeSubPath();
         */
    }
    
    void resized(void) override
    {
        
        
                           
    }
    
    void setOrientation(NodeOrientation o)
    {
        
    }
    
private:
    NodeOrientation orientation;
    
    float centerX, centerY;
    float radiusX, radiusY;
    float rotation, startR, endR;
    
    
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKItemNode)
};
