/*
  ==============================================================================

    DirectViewController.cpp
    Created: 30 Nov 2016 5:00:19pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "DirectViewController.h"

//==============================================================================
DirectViewController::DirectViewController(BKAudioProcessor& p):
processor(p),
currentDirectId(0)
{
    //DirectPreparation::Ptr layer = processor.currentPiano->dPreparation[currentDirectId];
    DirectPreparation::Ptr layer = processor.dPreparation[currentDirectId];
    
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
    
    updateFields();
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

void DirectViewController::bkTextFieldDidChange(TextEditor& tf)
{
    String text = tf.getText();
    String name = tf.getName();
    
    float f = text.getFloatValue();
    int i = text.getIntValue();
    
    DBG(name + ": |" + text + "|");
    
    DirectPreparation::Ptr prep = processor.dPreparation[currentDirectId];
    DirectPreparation::Ptr active = processor.activeDPreparation[currentDirectId];
    
    if (name == cDirectParameterTypes[DirectId])
    {
        currentDirectId = i;
        updateFields();
    }
    else if (name == cDirectParameterTypes[DirectTransposition])
    {
        DBG("transp: " + String(f));
        prep    ->setTransposition(f);
        active  ->setTransposition(f);
    }
    else if (name == cDirectParameterTypes[DirectGain])
    {
        prep    ->setGain(f);
        active  ->setGain(f);
    }
    else if (name == cDirectParameterTypes[DirectHammerGain])
    {
        prep    ->setHammerGain(f);
        active  ->setHammerGain(f);
    }
    else if (name == cDirectParameterTypes[DirectResGain])
    {
        prep    ->setResonanceGain(f);
        active  ->setResonanceGain(f);
    }
    else if (name == cDirectParameterTypes[DirectTuning])
    {
        prep    ->setTuning(processor.tPreparation[i]);
        active  ->setTuning(processor.tPreparation[i]);
    }
    else
    {
        DBG("Unregistered text field entered input.");
    }
}


void DirectViewController::updateFields(void)
{
    
    DirectPreparation::Ptr prep = processor.activeDPreparation[currentDirectId];
    
    directTF[DirectTransposition]       ->setText( String( prep->getTransposition()), false);
    directTF[DirectGain]                ->setText( String( prep->getGain()), false);
    directTF[DirectHammerGain]          ->setText( String( prep->getHammerGain()), false);
    directTF[DirectResGain]             ->setText( String( prep->getResonanceGain()), false);
    directTF[DirectTuning]              ->setText( String( prep->getTuning()->getId()), false);

}

void DirectViewController::bkMessageReceived (const String& message)
{
    if (message == "direct/update")
    {
        //currentDirectId = processor.currentPiano->getPreparation();
        
        updateFields();
    }
}


