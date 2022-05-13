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
        BKPianoConnection* connection = new BKPianoConnection(start->piano, end->piano);
        
        connection->setline(Point<float>(start->getX() + start->getWidth()/2, start->getY() + start->getHeight()/2), Point<float>(end->getX() + end->getWidth()/2, end->getY() + end->getHeight()/2));
        addAndMakeVisible(connection);
        connection->toBack();
        pianoConnections.emplace_back(connection);
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
    
private:
    std::vector<std::unique_ptr<BKPianoConnection>> pianoConnections;
    std::vector<std::unique_ptr<PianoObject>> pianos;
};
