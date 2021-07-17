/*
  ==============================================================================

    EqualizerGraph.h
    Created: 16 Jul 2021 1:46:53pm
    Author:  Jeffrey Gordon

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "BKEqualizer.h"

//==============================================================================
/*
*/
class EqualizerGraph  : public juce::Component
{
public:
    EqualizerGraph()
    {
        // In your constructor, you should add any child components, and
        // initialise any special settings that your component needs.
        eq = nullptr;

    }

    ~EqualizerGraph() override
    {
    }

    void paint (juce::Graphics& g) override
    {
        using namespace juce;

        g.fillAll (Colours::black);   // clear the background
        
        Rectangle<int> bounds(getLocalBounds());
        Rectangle<int> graphArea(bounds);
        Rectangle<int> labelArea = graphArea.removeFromBottom(12);
        graphArea.reduce(4, 0);
        

        g.setColour (juce::Colours::grey);
        g.drawRect (graphArea, 1);   // draw an outline around the component
        g.drawRect(labelArea, 1);
        
        auto w = graphArea.getWidth();
        std::vector<double> mags;
        mags.resize(w);
        
        for (int i = 0; i < w; i++) {
            auto freq = mapToLog10(double(i) / double(w), 20.0, 20000.0);
            double mag = eq->magForFreq(freq);
            
            mags[i] = Decibels::gainToDecibels(mag);
        }
        
        Path responseCurve;
        double outputMin = graphArea.getBottom();
        double outputMax = graphArea.getY();
        auto map = [outputMin, outputMax](double input) {
            double inputMin = -24.0;
            double inputMax = 24.0;
            
            if (input < inputMin) return outputMin;
            else if (input > inputMax) return outputMax;
            else return jmap(input, inputMin, inputMax, outputMin, outputMax);
        };
        
        responseCurve.startNewSubPath(graphArea.getX(), map(mags.front()));
        
        for (size_t i = 1; i < mags.size(); i++) {
            responseCurve.lineTo(graphArea.getX() + i, map(mags[i]));
        }
        
        g.setColour(Colours::white);
        g.strokePath(responseCurve, PathStrokeType(2.f));
    }

    void resized() override
    {
        // This method is where you should set the bounds of any child
        // components that your component contains..
        repaint();
    }
    
    // hmmmm is there a thread safety issue here?
    void updateEQ(BKEqualizer& newEQ) { eq = &newEQ;}

private:
    BKEqualizer* eq;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EqualizerGraph)
};
