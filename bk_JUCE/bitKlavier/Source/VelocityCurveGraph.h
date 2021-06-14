/*
  ==============================================================================

    VelocityCurveGraph.h
    Created: 8 Jun 2021 5:28:51pm
    Author:  Jeffrey Gordon

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "BKUtilities.h"

//==============================================================================
/*
*/
class VelocityCurveGraph  : public juce::Component
{
public:
    VelocityCurveGraph()
    {
    }

    ~VelocityCurveGraph() override
    {
    }

    void paint (juce::Graphics& g) override
    {
        int leftPadding = 80; // space for "velocity out" label
        int bottomPadding = 40; // space for "velocity in" label
        int leftAdditional = 30; // additional space for number labels on left
        
        // don't bother if there's not enough room
        if (getWidth() <= leftPadding + 2 || getHeight() <= bottomPadding + 2) return;
        
        // wrapper rectangles for labels
        juce::Rectangle<int> graphArea(leftPadding, 0, getWidth() - leftPadding,
                                       getHeight() - bottomPadding);
        
        juce::Rectangle<int> bottomLabel(leftPadding, getHeight() - bottomPadding,
                                         getWidth() - leftPadding, bottomPadding);
        juce::Rectangle<int> leftLabel(0, 0, leftPadding - leftAdditional,
                                       getHeight() - bottomPadding);
        juce::Rectangle<int> leftNumbers(leftPadding - leftAdditional, 0, leftAdditional,
                                         getHeight() - bottomPadding);
        
        // graph background setup
        g.setColour (juce::Colours::grey);
        g.drawRect(graphArea);
        int graphHeight = graphArea.getHeight();
        int graphWidth = graphArea.getWidth();
        int graphX = graphArea.getX();
        int graphY = graphArea.getY();
        juce::Line<float> hMidLine(graphX, graphHeight / 2,
                                   graphWidth + graphX, graphHeight / 2);
        juce::Line<float> vMidLine(graphWidth / 2 + graphX, graphY,
                                   graphWidth / 2 + graphX, graphHeight);
        const float dashLengths[] = {5, 3};
        g.drawDashedLine(hMidLine, dashLengths, 2);
        g.drawDashedLine(vMidLine, dashLengths, 2);
        
        // graph label setup
        g.drawText("0", bottomLabel, juce::Justification::topLeft);
        g.drawText("0.5", bottomLabel, juce::Justification::centredTop);
        g.drawText("1", bottomLabel, juce::Justification::topRight);
        g.drawText("0", leftNumbers, juce::Justification::bottomRight);
        g.drawText("0.5", leftNumbers, juce::Justification::centredRight);
        g.drawText("1", leftNumbers, juce::Justification::topRight);
        g.setColour(juce::Colours::white);
        g.drawText("Velocity In", bottomLabel, juce::Justification::centredBottom);
        g.drawFittedText("Velocity \nOut", leftLabel, juce::Justification::centred, 2);
        
        // plotter
        g.setColour(juce::Colours::red);
        juce::Path plot;
        //plot.startNewSubPath(leftPadding, graphHeight);
        
        // go pixel by pixel, adding each point to plot
        for (int i = 0; i <= graphWidth; i++) {
            juce::Point<float> toAdd (leftPadding + i, graphHeight - graphHeight
                               * dt_warpscale((float) i / graphWidth, asym_k, sym_k, scale,
                                              offset));
            
            if (toAdd.getY() < 0) toAdd.setY(0);
            if (toAdd.getY() > graphHeight) toAdd.setY(graphHeight);
            
            if (velocityInvert) {
                toAdd.setY(graphHeight - toAdd.getY());
            }
            
            if (i == 0) plot.startNewSubPath(toAdd);
            else plot.lineTo(toAdd);
            //DBG("graphed " + toAdd.toString());
        }
        
        g.strokePath(plot, PathStrokeType(2.0));
    }

    void resized() override
    {
        // This method is where you should set the bounds of any child
        // components that your component contains..
        repaint();

    }
    
    
    // the graph must be re-drawn whenever a parameter changes
    void setAsym_k (float newAsym_k) {
        asym_k = newAsym_k;
        repaint();
    }
    void setSym_k (float newSym_k) {
        sym_k = newSym_k;
        repaint();
    }
    void setScale (float newScale) {
        scale = newScale;
        repaint();
    }
    void setOffset (float newOffset) {
        offset = newOffset;
        repaint();
    }
    void invertVelocities () {
        velocityInvert = !velocityInvert;
        repaint();
    }

private:
    float asym_k = 1;
    float sym_k = 1;
    float scale = 1;
    float offset = 0;
    bool velocityInvert = false;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VelocityCurveGraph)
};

