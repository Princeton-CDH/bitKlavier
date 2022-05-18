/*
  ==============================================================================

    PianoObject.h
    Created: 11 May 2022 12:24:31pm
    Author:  Davis Polito

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Piano.h"
#include "BKComponent.h"
#include "PluginProcessor.h"
#include "BKPianoConnection.h"
class PianoObject : public BKDraggableComponent, ReferenceCountedObject {
public:
//    typedef ReferenceCountedArray<PianoObject, CriticalSection> PtrArr;
//    typedef ReferenceCountedObjectPtr<PianoObject> Ptr;
    
    Image image;
    RectanglePlacement placement;
    Piano::Ptr piano;
    BKAudioProcessor &processor;
    std::vector<std::unique_ptr<BKPianoConnection>> pianoConnections;
//    PianoObject() : BKDraggableComponent(true, false, true, 50, 50, 50, 50)
//    {
//        image = ImageCache::getFromMemory(BinaryData::piano_icon_png, BinaryData::piano_icon_pngSize);
//        placement = RectanglePlacement::centred;
//
//        setSize(image.getWidth() *0.25, image.getHeight() *0.25);
//    }
    PianoObject(Piano::Ptr piano, BKAudioProcessor &p) :  BKDraggableComponent(true, false, true, 50, 50, 50, 50), piano(piano), processor(p)
    {
        image = ImageCache::getFromMemory(BinaryData::piano_icon_png, BinaryData::piano_icon_pngSize);
        placement = RectanglePlacement::centred;

        setSize(image.getWidth() *0.25, image.getHeight() *0.25);
        addAndMakeVisible(name);
        name.setText(String(piano->getId()) +  " " + piano->getName(), dontSendNotification);
       // name.setText()
    }
    ~PianoObject(){};
    
    
    
    void paint (juce::Graphics& g) override
    {
        g.setOpacity(1.0f);
        g.drawImage(image, getLocalBounds().toFloat(), placement);
        g.setColour(Colours::transparentWhite);
        g.drawRect(getLocalBounds(),0);
        
        
    }
    
  
    bool operator==(const PianoObject& rhs)
    {
        return piano == rhs.piano;
    }
private:
    BKLabel name;
};
