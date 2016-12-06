/*
  ==============================================================================

    DirectViewController.cpp
    Created: 30 Nov 2016 5:00:19pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "DirectViewController.h"

//==============================================================================
DirectViewController::DirectViewController(BKAudioProcessor& p):
processor(p),
currentDirectLayer(0)
{
    DirectPreparation::Ptr layer = processor.dPreparation[currentDirectLayer];
    
    // Labels
    directL = OwnedArray<BKLabel>();
    directL.ensureStorageAllocated(cDirectParameterTypes.size());
    
    for (int i = 0; i < cDirectParameterTypes.size(); i++)
    {
        directL.set(i, new BKLabel());
        addAndMakeVisible(directL[i]);
        directL[i]->setName(cDirectParameterTypes[i]);
        directL[i]->setText(cDirectParameterTypes[i], NotificationType::dontSendNotification);
    }
    
    // Text Fields
    directTF = OwnedArray<BKTextField>();
    directTF.ensureStorageAllocated(cDirectParameterTypes.size());
    
    for (int i = 0; i < cDirectParameterTypes.size(); i++)
    {
        directTF.set(i, new BKTextField());
        addAndMakeVisible(directTF[i]);
        directTF[i]->addListener(this);
        directTF[i]->setName(cDirectParameterTypes[i]);
    }
    
    updateFieldsToLayer(currentDirectLayer);
}

DirectViewController::~DirectViewController()
{
}

void DirectViewController::paint (Graphics& g)
{
    g.setColour(Colours::goldenrod);
    g.drawRect(getLocalBounds(), 1);
}

void DirectViewController::resized()
{
    // Labels
    int i = 0;
    int lX = 0;
    int lY = gComponentLabelHeight + gYSpacing;
    
    for (int n = 0; n < cDirectParameterTypes.size(); n++)
    {
        directL[n]->setTopLeftPosition(lX, gYSpacing + lY * n);
    }
    
    // Text fields
    i = 0;
    int tfX = gComponentLabelWidth + gXSpacing;
    int tfY = gComponentTextFieldHeight + gYSpacing;
    
    for (int n = 0; n < cDirectParameterTypes.size(); n++)
    {
        directTF[n]->setTopLeftPosition(tfX, gYSpacing + tfY * n);
    }
    
}

void DirectViewController::textFieldDidChange(TextEditor& tf)
{
    String text = tf.getText();
    String name = tf.getName();
    
    float f = text.getFloatValue();
    int i = text.getIntValue();
    
    DBG(name + ": |" + text + "|");
    
    DirectProcessor::Ptr   proc = processor.dProcessor[currentDirectLayer];
    
    DirectPreparation::Ptr prep = proc->getPreparation();
    
    Keymap::Ptr keymap             = proc->getKeymap();
    
    
    if (name == cDirectParameterTypes[DirectNumLayers])
    {
        processor.numDirectLayers = i;
    }
    else if (name == cDirectParameterTypes[DirectCurrentLayer])
    {
        updateFieldsToLayer(i);
    }
    else if (name == cDirectParameterTypes[DirectKeymap])
    {
        Array<int> keys = stringToIntArray(text);
        
        keymap->clear();
        for (auto note : keys)
        {
            keymap->addNote(note);
        }
        
    }
    else if (name == cDirectParameterTypes[DirectTransposition])
    {
        prep->setTransposition(f);
    }
    else if (name == cDirectParameterTypes[DirectGain])
    {
        prep->setGain(f);
    }
    else if (name == cDirectParameterTypes[DirectOverlay])
    {
        prep->setOverlay((bool)i);
    }
    else
    {
        DBG("Unregistered text field entered input.");
    }
}


void DirectViewController::updateFieldsToLayer(int numLayer)
{
    
    currentDirectLayer = numLayer;
    
    DirectProcessor::Ptr   proc = processor.dProcessor[currentDirectLayer];
    
    DirectPreparation::Ptr prep = proc->getPreparation();
    
    Keymap::Ptr keymap             = proc->getKeymap();
    
    // Set text.
    directTF[DirectNumLayers]         ->setText( String( processor.numDirectLayers));
    directTF[DirectKeymap]            ->setText( intArrayToString( keymap->keys()));
    directTF[DirectCurrentLayer]      ->setText( String( numLayer));
    
    directTF[DirectTransposition]     ->setText( String( prep->getTransposition()));
    directTF[DirectGain]              ->setText( String( prep->getGain()));
    directTF[DirectOverlay]           ->setText( String( prep->getOverlay()));

}
