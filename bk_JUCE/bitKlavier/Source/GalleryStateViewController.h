/*
  ==============================================================================

    GalleryStateViewController.h
    Created: 5 May 2022 12:40:28pm
    Author:  Davis Polito

  ==============================================================================
*/

#pragma once
#include "BKViewController.h"
#include "BKPianoConnection.h"
#include "PianoObject.h"


class GalleryStateViewController :
public BKViewController
{

public:
    GalleryStateViewController (BKAudioProcessor&, BKItemGraph *theGraph);
    ~GalleryStateViewController()
    {
        
    };
    
    void paint (Graphics&) override;
    void resized() override;
    
    void reset(void);
    
    void update(void);
    
    void getAllPianoMaps();
    
    int getObjectFromPiano(Piano::Ptr pianoTarget);
    void addPianoConnection(PianoObject* start, PianoObject* end)
    {
        DBG("\"" + start->piano->getName() + " \" connects to \" " + end->piano->getName() + "\"");
        DBG("\"" + String(start->piano->getId()) + " \" connects to \" " + String(end->piano->getId()) + "\"");
        BKPianoConnection* connection = new BKPianoConnection(processor, start->piano, end->piano);
        Point<float> startline(start->getX() + start->getWidth()/2, start->getY() + start->getHeight()/2);
        Point<float> endline(end->getX() + end->getWidth()/2, end->getY() + end->getHeight()/2);
        
    
        Line<int> tmpline = Line<int>(startline.toInt(), endline.toInt());
        int length = tmpline.getLength();
        DBG(endline.getDistanceFrom(endline));
        connection->setBounds(startline.getX(), startline.getY(),length, 20);
        connection->drawline();
        addAndMakeVisible(connection);
        connection->toBack();
        AffineTransform tt = AffineTransform::rotation(3 * Utilities::pi/2 + tmpline.getAngle(), startline.getX(), startline.getY());
        connection->setTransform(tt);
        pianoConnections.emplace_back(connection);
    }
    
    void mouseMove (const MouseEvent& eo) override
    {
        MouseEvent e = eo.getEventRelativeTo(this);
        
        
        
        
            e.getEventRelativeTo(this).x;
            e.getEventRelativeTo(this).y;
            
            repaint();
        
    }
//    void findPianoConnections()
//    {
//        for (auto pianomap : piano->pianoConnections)
//        {
//            int target = pianomap.pianoTarget;
//            Piano::Ptr pianoTarget = processor.gallery->getPiano(target);
//            //pianoConnections.insert(std::pair<Piano::Ptr,Piano::Ptr>(piano,pianoTarget));
//            DBG("\"" + piano->getName() + " \" connects to \" " + pianoTarget->getName() + "\"");
//            
//        }
//    }
    
    void buttonClicked(Button *b) override;
 
    void setCurrentPiano(PianoObject* piano) {currentPiano = piano;}
    
    void setCurrentConnection(BKPianoConnection* connection) {currentConnection = connection;}
    
private:
    
    BKPianoConnection* currentConnection;
    PianoObject* currentPiano;
    std::vector<std::unique_ptr<BKPianoConnection>> pianoConnections;
    std::vector<std::unique_ptr<PianoObject>> pianos;
};
