/*
  ==============================================================================

    GalleryStateViewController.cpp
    Created: 5 May 2022 12:40:28pm
    Author:  Davis Polito

  ==============================================================================
*/

#include "GalleryStateViewController.h"
GalleryStateViewController::GalleryStateViewController(BKAudioProcessor& p, BKItemGraph *theGraph) : BKViewController(p, theGraph, 1)
{
    setLookAndFeel(&buttonsAndMenusLAF);
    
    //addAndMakeVisible(connection.get());
}

void GalleryStateViewController::paint(Graphics& g)
{
    g.fillAll(Colours::black);
    Rectangle<int> area(getBounds());
    
    area.reduce(10 * processor.paddingScalarX + 4, 10 * processor.paddingScalarY + 4);
    Rectangle<int> leftColumn = area.removeFromLeft(area.getWidth() * 0.5);
    leftColumn.removeFromRight(4 + 2.0f * gPaddingConst * processor.paddingScalarX);
    leftColumn.removeFromLeft(gXSpacing);
    Rectangle<int> xbutton = leftColumn.removeFromTop(gComponentComboBoxHeight);
    hideOrShow.setBounds(xbutton.removeFromLeft(gComponentComboBoxHeight));
    
    Point<float> center = getBounds().getCentre().toFloat();
   
    for (int i = 0; i < pianos.size(); i++)
    {
        DBG("pianoPos" + String(pianos[i]->getX()) + "," + String(pianos[i]->getY()));
    }
    for (int i = 0; i < pianoConnections.size(); i++)
    {
//        pianoConnections[i]->setBounds(pianoConnections[i]->line.getStartX(),
//                                      pianoConnections[i]->line.getStartY(),
//                                      pianoConnections[i]->line.getStartX() + pianoConnections[i]->line.getEndX(),
//                                      pianoConnections[i]->line.getStartY() + pianoConnections[i]->line.getEndY());
        //pianoConnections[i]->setBounds(0,0, 300, 300);
        
       pianoConnections[i]->setBounds(0,0,2000,2000);
    }

    
    
}

void GalleryStateViewController::resized()
{
    
    //will have to put big '#if 0' here eventually
   
}

void GalleryStateViewController::getAllPianoMaps()
{
    
//    for (auto item : theGraph->getItems())
//    {
//        if (item->getType() == PreparationTypePianoMap)
//        {
//            int target = item->getPianoTarget();
//        }
//    }
    
    for (auto piano : processor.gallery->getPianos())
    {
        PianoObject* n = new PianoObject(piano, processor);
        pianos.emplace_back(n);
        
        
        addAndMakeVisible(n);
        
    }
    
    
    Point<float> center = getBounds().getCentre().toFloat();
    //center.setXY(1000, 1000);
    Point<float> P1 = Point<float>(center.getX(), center.getY() - 300);
    
    for (int i = 0; i < pianos.size(); i++)
    {

        Point<float> newPoint = Point<float>(300 * cos(2 * Utilities::pi * i/pianos.size()) + center.getX(), 300 * sin(2 * Utilities::pi * i/pianos.size()) + center.getY());
        pianos[i]->setTopLeftPosition(newPoint.toInt());

    }
    
    for (int i =0; i < pianos.size(); i++)
    {
        for (auto item : pianos[i]->piano->getItems())
        {
            if (item->getType() == PreparationTypePianoMap)
            {
                int target = item->getPianoTarget();
                Piano::Ptr pianoTarget = processor.gallery->getPiano(target);
                addPianoConnection(pianos[i].get(), pianos[getObjectFromPiano(pianoTarget)].get());
                
            }
        }
    }
    
    
}

int GalleryStateViewController::getObjectFromPiano(Piano::Ptr pianoTarget)
{
    for (int i = 0; i < pianos.size(); i++)
    {
        if (pianos[i]->piano == pianoTarget)
        {
            return i;
        }
    }
}

void GalleryStateViewController::update()
{
    //pianoConnections.clear();
    pianos.clear();
    pianoConnections.clear();
    getAllPianoMaps();
   
    
}
