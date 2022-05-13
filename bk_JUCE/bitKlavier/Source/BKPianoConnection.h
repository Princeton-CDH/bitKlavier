/*
  ==============================================================================

    BKPianoConnection.h
    Created: 9 May 2022 1:49:17pm
    Author:  Davis Polito

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "BKLabel.h"
#include "SpringTuningUtilities.h"
#include "Piano.h"
class BKPianoConnection : public BKComponent, ReferenceCountedObject
{
public :
    
//    typedef ReferenceCountedArray<BKPianoConnection, CriticalSection> PtrArr;
//    typedef ReferenceCountedObjectPtr<BKPianoConnection> Ptr;
    BKPianoConnection(Piano::Ptr start, Piano::Ptr end) : start(start), end(end)
    {
        
        
        //addAndMakeVisible(t);
        //t.setText("AYYE");
        t.setColour(juce::Colours::white);
        addAndMakeVisible(l);
        l.setText("KEYS", dontSendNotification);
    };
    
    void setline(Point<float> start, Point<float> end)
    {
        Line<float> line = Line<float>(start,end);
        line1 = Line<float>(Point<float>(line.getStartX(),line.getStartY()),
                                         Point<float>((line.getStartX() + line.getEndX())/2, (line.getStartY() + line.getEndY())/2));
        line2 = Line<float>(Point<float>((line.getStartX() + line.getEndX())/2, (line.getStartY() + line.getEndY())/2),
                                         Point<float>(line.getEndX(),line.getEndY()));
        //DBG("STart: " + String(start.getX()) +"," + String(start.getY()) + "END: " +  String(end.getX()) + "," + String(end.getY()));
        p.startNewSubPath(start);
        p.addArrow(line1, 1, 10, 10);
        p.addLineSegment(line2, 1);
    }
    
    void paint (juce::Graphics& g) override
    {
        Colour c(juce::Colours::white);
        g.setColour(c);
        g.strokePath(p, PathStrokeType(0.5f));
        //Rectangle<float> r = p.getBounds();
        //r.setY(r.getY());
        //r.reduce(2.f, 2.f);
        l.setBounds(line1.getEndX(), line1.getEndY(), 100, 100);
        DBG("PAINT LINE:" + String(line1.getAngle()));
        AffineTransform tt = AffineTransform::rotation(Utilities::pi / 2 + line1.getAngle(), (line1.getStartX() + line2.getEndX())/2, (line1.getStartY() + line2.getEndY())/2);
        
        //t.setBoundingBox(r);
        
        l.setTransform(tt);
    }

    void resized() override
    {
        
    }
    Line<float> line1;
    Line<float> line2;
private :
    
    Path p;
    
    BKLabel l;
    DrawableText t;
    Piano::Ptr start;
    Piano::Ptr end;
    
    
};
