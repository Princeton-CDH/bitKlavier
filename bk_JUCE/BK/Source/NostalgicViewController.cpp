/*
  ==============================================================================

    NostalgicViewController.cpp
    Created: 30 Nov 2016 9:43:47am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "NostalgicViewController.h"

#include "BKUtilities.h"

//==============================================================================
NostalgicViewController::NostalgicViewController(BKAudioProcessor& p)
:
    processor(p),
    currentNostalgicId(0)
{
    NostalgicPreparation::Ptr layer = processor.nPreparation[currentNostalgicId];
    
    // Labels
    nostalgicL = OwnedArray<BKLabel>();
    nostalgicL.ensureStorageAllocated(cNostalgicParameterTypes.size());
    
    for (int i = 0; i < cNostalgicParameterTypes.size(); i++)
    {
        nostalgicL.set(i, new BKLabel());
        addAndMakeVisible(nostalgicL[i]);
        nostalgicL[i]->setName(cNostalgicParameterTypes[i]);
        nostalgicL[i]->setText(cNostalgicParameterTypes[i], NotificationType::dontSendNotification);
    }
    
    
    
    // Text Fields
    nostalgicTF = OwnedArray<BKTextField>();
    nostalgicTF.ensureStorageAllocated(cNostalgicParameterTypes.size());
    
    for (int i = 0; i < cNostalgicParameterTypes.size(); i++)
    {
        nostalgicTF.set(i, new BKTextField());
        addAndMakeVisible(nostalgicTF[i]);
        nostalgicTF[i]->addListener(this);
        nostalgicTF[i]->setName(cNostalgicParameterTypes[i]);
    }
    
    updateFields();
}

NostalgicViewController::~NostalgicViewController()
{
}

void NostalgicViewController::paint (Graphics& g)
{
    g.setColour(Colours::goldenrod);
    g.drawRect(getLocalBounds(), 1);
}

void NostalgicViewController::resized()
{
    // Labels
    int i = 0;
    int lX = 0;
    int lY = gComponentLabelHeight + gYSpacing;
    
    for (int n = 0; n < cNostalgicParameterTypes.size(); n++)
    {
        nostalgicL[n]->setTopLeftPosition(lX, gYSpacing + lY * n);
    }
    
    // Text fields
    i = 0;
    int tfX = gComponentLabelWidth + gXSpacing;
    int tfY = gComponentTextFieldHeight + gYSpacing;

    for (int n = 0; n < cNostalgicParameterTypes.size(); n++)
    {
        nostalgicTF[n]->setTopLeftPosition(tfX, gYSpacing + tfY * n);
    }
    
}

void NostalgicViewController::bkTextFieldDidChange(TextEditor& tf)
{
    String text = tf.getText();
    String name = tf.getName();
    
    float f = text.getFloatValue();
    int i = text.getIntValue();
    
    DBG(name + ": |" + text + "|");
    
    NostalgicPreparation::Ptr prep = processor.nPreparation[currentNostalgicId];
    
    NostalgicPreparation::Ptr active = processor.activeNPreparation[currentNostalgicId];
    
    if (name == cNostalgicParameterTypes[NostalgicId])
    {
        currentNostalgicId = i;
        updateFields();
    }
    else if (name == cNostalgicParameterTypes[NostalgicWaveDistance])
    {
        prep    ->setWaveDistance(i);
        active  ->setWaveDistance(i);
    }
    else if (name == cNostalgicParameterTypes[NostalgicUndertow])
    {
        prep    ->setUndertow(i);
        active  ->setUndertow(i);
    }
    else if (name == cNostalgicParameterTypes[NostalgicTransposition])
    {
        prep    ->setTransposition(f);
        active  ->setTransposition(f);
    }
    else if (name == cNostalgicParameterTypes[NostalgicGain])
    {
        prep    ->setGain(f);
        active  ->setGain(f);
    }
    else if (name == cNostalgicParameterTypes[NostalgicLengthMultiplier])
    {
        prep    ->setLengthMultiplier(f);
        active  ->setLengthMultiplier(f);
    }
    else if (name == cNostalgicParameterTypes[NostalgicBeatsToSkip])
    {
        prep    ->setBeatsToSkip(i);
        active  ->setBeatsToSkip(i);
    }
    else if (name == cNostalgicParameterTypes[NostalgicMode])
    {
        prep    ->setMode((NostalgicSyncMode) i);
        active  ->setMode((NostalgicSyncMode) i);
    }
    else if (name == cNostalgicParameterTypes[NostalgicSyncTarget])
    {
        prep    ->setSyncTarget(i);
        active  ->setSyncTarget(i);
    }
    else if (name == cNostalgicParameterTypes[NostalgicTuning])
    {
        prep    ->setTuning(processor.tPreparation[i]);
        active  ->setTuning(processor.tPreparation[i]);
    }
    else
    {
        DBG("Unregistered text field entered input.");
    }
}


void NostalgicViewController::updateFields(void)
{
    
    NostalgicPreparation::Ptr prep = processor.activeNPreparation[currentNostalgicId];

    nostalgicTF[NostalgicWaveDistance]      ->setText( String( prep->getWavedistance()), false);
    nostalgicTF[NostalgicUndertow]          ->setText( String( prep->getUndertow()), false);
    nostalgicTF[NostalgicTransposition]     ->setText( String( prep->getTransposition()), false);
    nostalgicTF[NostalgicGain]              ->setText( String( prep->getGain()), false);
    nostalgicTF[NostalgicLengthMultiplier]  ->setText( String( prep->getLengthMultiplier()), false);
    nostalgicTF[NostalgicBeatsToSkip]       ->setText( String( prep->getBeatsToSkip()), false);
    nostalgicTF[NostalgicMode]              ->setText( String( prep->getMode()), false);
    nostalgicTF[NostalgicSyncTarget]        ->setText( String( prep->getSyncTarget()), false);
    nostalgicTF[NostalgicTuning]            ->setText( String(prep->getTuning()->getId()), false);
}

void NostalgicViewController::bkMessageReceived (const String& message)
{
    if (message == "nostalgic/update")
    {
        //currentNostalgicId = processor.currentPiano->getPreparation();
        
        updateFields();
    }
}

