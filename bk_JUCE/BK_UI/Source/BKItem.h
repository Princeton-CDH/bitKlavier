/*
  ==============================================================================

    BKItem.h
    Created: 4 Apr 2017 8:05:05pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef BKITEM_H_INCLUDED
#define BKITEM_H_INCLUDED

#include "BKUtilities.h"
#include "BKComponent.h"

#include "PluginProcessor.h"

class BKItem : public BKDraggableComponent
{
public:
    BKItem(BKPreparationType type, int Id, BKAudioProcessor& p):
    BKDraggableComponent(true,false),
    processor(p),
    type(type),
    Id(Id)
    {
        addAndMakeVisible(fullChild);
        
        addAndMakeVisible(label);
        
        label.setJustificationType(Justification::centred);
        label.setBorderSize(BorderSize<int>(2));
        
        String name = cPreparationTypes[type]+String(Id);
        
        label.setText(name, dontSendNotification);
        
        DBG("constructed: " + name);
        
    }
    
    ~BKItem()
    {
        
    }
    
    void mouseDown(const MouseEvent& e) override
    {
        if (type == PreparationTypeDirect)
        {
            processor.updateState->currentDirectId = Id;
            processor.updateState->directPreparationDidChange = true;
            processor.updateState->currentPreparationDisplay = DisplayDirect;
        }
        else if (type == PreparationTypeSynchronic)
        {
            processor.updateState->currentSynchronicId = Id;
            processor.updateState->synchronicPreparationDidChange = true;
            processor.updateState->currentPreparationDisplay = DisplaySynchronic;
        }
        else if (type == PreparationTypeNostalgic)
        {
            processor.updateState->currentNostalgicId = Id;
            processor.updateState->nostalgicPreparationDidChange = true;
            processor.updateState->currentPreparationDisplay = DisplayNostalgic;
        }
        else if (type == PreparationTypeTuning)
        {
            processor.updateState->currentTuningId = Id;
            processor.updateState->tuningPreparationDidChange = true;
            processor.updateState->currentPreparationDisplay = DisplayTuning;
        }
        else if (type == PreparationTypeTempo)
        {
            processor.updateState->currentTempoId = Id;
            processor.updateState->tempoPreparationDidChange = true;
            processor.updateState->currentPreparationDisplay = DisplayTempo;
        }
        else if (type == PreparationTypeKeymap)
        {
            processor.updateState->currentKeymapId = Id;
            processor.updateState->keymapDidChange = true;
            processor.updateState->currentPreparationDisplay = DisplayKeymap;
        }
        
        
        processor.updateState->displayDidChange = true;
    }
    
    void paint(Graphics& g) override
    {
        g.setColour(Colours::lightblue);
        g.fillAll();
        
        g.setColour(Colours::black);
        g.drawRect(getLocalBounds());
    }
    
    void resized(void) override
    {
        label.setBounds(0,0,getWidth(),getHeight());
        fullChild.setBounds(0,0,getWidth(),getHeight());
    }


private:
    BKAudioProcessor& processor;
    Label label;
    Component fullChild;
    
    BKPreparationType type;
    int Id;
    String name;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKItem)
};



#endif  // BKITEM_H_INCLUDED
