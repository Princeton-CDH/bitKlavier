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
    BKPianoConnection(BKAudioProcessor &p, Piano::Ptr start, Piano::Ptr end) : processor(p), start(start), end(end), isSelected(false)
    {
        
        
        //addAndMakeVisible(t);
        //t.setText("AYYE");
        t.setColour(juce::Colours::white);
        addAndMakeVisible(l);
        l.setText("KEYS", dontSendNotification);
    };
    
    void setline(Point<float> start, Point<float> end)
    {
        Line<float> line = Line<float>(Point<float>(getLocalBounds().getX(), getLocalBounds().getY() + getLocalBounds().getHeight()/2),
                                       Point<float>(getLocalBounds().getX() + getLocalBounds().getWidth(), getLocalBounds().getY()+ getLocalBounds().getHeight()/2) );
        line1 = Line<float>(Point<float>(line.getStartX(),line.getStartY()),
                                         Point<float>((line.getStartX() + line.getEndX())/2, (line.getStartY() + line.getEndY())/2));
        line2 = Line<float>(Point<float>((line.getStartX() + line.getEndX())/2, (line.getStartY() + line.getEndY())/2),
                                         Point<float>(line.getEndX(),line.getEndY()));
        //DBG("STart: " + String(start.getX()) +"," + String(start.getY()) + "END: " +  String(end.getX()) + "," + String(end.getY()));
        p.startNewSubPath(Point<float>(getLocalBounds().getX(), getLocalBounds().getY()));
        p.addArrow(line1, 1, 10, 10);
        p.addLineSegment(line2, 1);
    }
    
    void paint (juce::Graphics& g) override
    {
        if (isSelected)
        {
            g.setColour(Colours::yellow);
        }
        else
        {
            g.setColour(Colours::antiquewhite);
           
        }
       
        //g.drawRect(getLocalBounds(), 2);
        g.strokePath(p, PathStrokeType(0.5f));
        //Rectangle<float> r = p.getBounds();
        //r.setY(r.getY());
        //r.reduce(2.f, 2.f);
        l.setBounds(line1.getEndX(), line1.getEndY(), 100, 100);
        //DBG("PAINT LINE:" + String(line1.getAngle()));
        //AffineTransform tt = AffineTransform::rotation(Utilities::pi / 2 + line1.getAngle(), (line1.getStartX() + line2.getEndX())/2, (line1.getStartY() + line2.getEndY())/2);
        
        //t.setBoundingBox(r);
        
        //l.setTransform(tt);
        
        
    }

    void resized() override
    {
        
    }
    
    void mouseDown(const MouseEvent &e) override
    {
        //isSelected = true;
    }

    void mouseDoubleClick(const MouseEvent &e) override
    {
        DBG("\"" + String(start->getId()) + " \" connects to \" " + String(end->getId()) + "\"");
        //processor.setCurrentPiano(start->getId());
        for (auto item: start->getItems())
        {
            if (item->getType() == PreparationTypePianoMap)
            {
                int target = item->getPianoTarget();
                if (target == end->getId())
                {

                    for (auto connection : item->getConnections())
                    {
                        DBG("PrepType: " + String(item->getType()) + " Id:" +String(connection->getId()));
                        if(connection->getType() == PreparationTypeKeymap)
                        {

                            DBG("Connection ID" + String(connection->getId()));

                            processor.updateState->comment = item->getCommentText();
                            processor.updateState->setCurrentDisplay(PreparationTypeKeymap, connection->getId());
                            break;
                        }
                    }
                }
            }
        }
    }
    
    
    Line<float> line1;
    Line<float> line2;
private :
    BKAudioProcessor &processor;
    Path p;
    bool isSelected;
    BKLabel l;
    DrawableText t;
    Piano::Ptr start;
    Piano::Ptr end;
    
    
};
