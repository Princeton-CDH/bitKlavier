/*
  ==============================================================================

    DirectViewController.cpp
    Created: 30 Nov 2016 5:00:19pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "DirectViewController.h"

#include "Preparation.h"

//==============================================================================
DirectViewController::DirectViewController(BKAudioProcessor& p):
processor(p),
currentDirectId(0),
currentModDirectId(0)
{
    //DirectPreparation::Ptr layer = processor.currentPiano->dPreparation[currentDirectId];
    DirectPreparation::Ptr layer = processor.direct[currentDirectId]->sPrep;
    
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
    
    modDirectTF = OwnedArray<BKTextField>();
    modDirectTF.ensureStorageAllocated(cDirectParameterTypes.size());
    
    for (int i = 0; i < cDirectParameterTypes.size(); i++)
    {
        modDirectTF.set(i, new BKTextField());
        addAndMakeVisible(modDirectTF[i]);
        modDirectTF[i]->addListener(this);
        modDirectTF[i]->setName("M"+cDirectParameterTypes[i]);
    }
    
    updateModFields();
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
    int lY = gComponentLabelHeight + gYSpacing;
    
    float width = getWidth() * 0.25 - gXSpacing;
    
    for (int n = 0; n < cDirectParameterTypes.size(); n++)
    {
        directL[n]->setBounds(0, gYSpacing + lY * n, width, directL[0]->getHeight());
    }
    
    // Text fields
    int tfY = gComponentTextFieldHeight + gYSpacing;
    
    float height = directTF[0]->getHeight();
    width *= 1.5;
    
    for (int n = 0; n < cDirectParameterTypes.size(); n++)
    {
        directTF[n]->setBounds(directL[0]->getRight()+gXSpacing, gYSpacing + tfY * n, width, height);
        modDirectTF[n]->setBounds(directTF[0]->getRight()+gXSpacing, gYSpacing + tfY * n, width, height);
    }
}

void DirectViewController::bkTextFieldDidChange(TextEditor& tf)
{
    String text = tf.getText();
    String name = tf.getName();
    
    BKTextFieldType type = BKParameter;
    
    if (name.startsWithChar('M'))
    {
        type = BKModification;
        name = name.substring(1);
    }
    
    
    float f = text.getFloatValue();
    int i = text.getIntValue();
    
    DBG(name + ": |" + text + "|");
    
    DirectPreparation::Ptr prep = processor.direct[currentDirectId]->sPrep;
    DirectPreparation::Ptr active = processor.direct[currentDirectId]->aPrep;
    DirectModPreparation::Ptr mod = processor.modDirect[currentModDirectId];
    
    if (name == cDirectParameterTypes[DirectId])
    {
        if (type == BKParameter)
        {
            currentDirectId = i;
            updateFields();
        }
        else    //BKModification
        {
            currentModDirectId = i;
            updateModFields();
        }
    }
    else if (name == cDirectParameterTypes[DirectTransposition])
    {
        if (type == BKParameter)
        {
            prep    ->setTransposition(f);
            active  ->setTransposition(f);
        }
        else    //BKModification
        {
            mod     ->setParam(DirectTransposition, text);
        }
    }
    else if (name == cDirectParameterTypes[DirectGain])
    {
        if (type == BKParameter)
        {
            prep    ->setGain(f);
            active  ->setGain(f);
        }
        else    //BKModification
        {
            mod     ->setParam(DirectGain, text);
        }
    }
    else if (name == cDirectParameterTypes[DirectHammerGain])
    {
        if (type == BKParameter)
        {
            prep    ->setHammerGain(f);
            active  ->setHammerGain(f);
        }
        else    //BKModification
        {
            mod     ->setParam(DirectHammerGain, text);
        }
    }
    else if (name == cDirectParameterTypes[DirectResGain])
    {
        if (type == BKParameter)
        {
            prep    ->setResonanceGain(f);
            active  ->setResonanceGain(f);
        }
        else    //BKModification
        {
            mod     ->setParam(DirectResGain, text);
        }
    }
    else if (name == cDirectParameterTypes[DirectTuning])
    {
        if (type == BKParameter)
        {
            prep    ->setTuning(processor.tuning[i]);
            active  ->setTuning(processor.tuning[i]);
        }
        else    //BKModification
        {
            mod     ->setParam(DirectTuning, text);
        }
    }
    else
    {
        DBG("Unregistered text field entered input.");
    }
}


void DirectViewController::updateFields(void)
{
    
    DirectPreparation::Ptr prep = processor.direct[currentDirectId]->aPrep;
    
    directTF[DirectTransposition]       ->setText( String( prep->getTransposition()), false);
    directTF[DirectGain]                ->setText( String( prep->getGain()), false);
    directTF[DirectHammerGain]          ->setText( String( prep->getHammerGain()), false);
    directTF[DirectResGain]             ->setText( String( prep->getResonanceGain()), false);
    directTF[DirectTuning]              ->setText( String( prep->getTuning()->getId()), false);

}

void DirectViewController::updateModFields(void)
{
    DirectModPreparation::Ptr prep = processor.modDirect[currentModDirectId];
    
    modDirectTF[DirectTransposition]       ->setText( prep->getParam(DirectTransposition), false);
    modDirectTF[DirectGain]                ->setText( prep->getParam(DirectGain), false);
    modDirectTF[DirectHammerGain]          ->setText( prep->getParam(DirectHammerGain), false);
    modDirectTF[DirectResGain]             ->setText( prep->getParam(DirectResGain), false);
    modDirectTF[DirectTuning]              ->setText( prep->getParam(DirectTuning), false);
    
}

void DirectViewController::bkMessageReceived (const String& message)
{
    if (message == "direct/update")
    {
        //currentDirectId = processor.currentPiano->getPreparation();
        
        updateFields();
    }
}


