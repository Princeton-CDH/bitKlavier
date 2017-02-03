/*
  ==============================================================================

    NostalgicViewController.cpp
    Created: 30 Nov 2016 9:43:47am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "NostalgicViewController.h"

#include "BKUtilities.h"

#include "Preparation.h"

//==============================================================================
NostalgicViewController::NostalgicViewController(BKAudioProcessor& p):
processor(p),
currentNostalgicId(0),
currentModNostalgicId(0)
{
    NostalgicPreparation::Ptr layer = processor.nostalgic[currentNostalgicId]->sPrep;
    
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
    
    modNostalgicTF = OwnedArray<BKTextField>();
    modNostalgicTF.ensureStorageAllocated(cNostalgicParameterTypes.size());
    
    for (int i = 0; i < cNostalgicParameterTypes.size(); i++)
    {
        modNostalgicTF.set(i, new BKTextField());
        addAndMakeVisible(modNostalgicTF[i]);
        modNostalgicTF[i]->addListener(this);
        modNostalgicTF[i]->setName("M"+cNostalgicParameterTypes[i]);
    }
    
    updateModFields();
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
    int lY = gComponentLabelHeight + gYSpacing;
    
    float width = getWidth() * 0.25 - gXSpacing;
    
    for (int n = 0; n < cNostalgicParameterTypes.size(); n++)
    {
        nostalgicL[n]->setBounds(0, gYSpacing + lY * n, width, nostalgicL[0]->getHeight());
    }
    
    // Text fields
    int tfY = gComponentTextFieldHeight + gYSpacing;
    
    float height = nostalgicTF[0]->getHeight();
    width *= 1.5;
    
    for (int n = 0; n < cNostalgicParameterTypes.size(); n++)
    {
        nostalgicTF[n]->setBounds(nostalgicL[0]->getRight()+gXSpacing, gYSpacing + tfY * n, width, height);
        modNostalgicTF[n]->setBounds(nostalgicTF[0]->getRight()+gXSpacing, gYSpacing + tfY * n, width, height);
    }
    
}

void NostalgicViewController::bkTextFieldDidChange(TextEditor& tf)
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
    
    NostalgicPreparation::Ptr prep = processor.nostalgic[currentNostalgicId]->sPrep;
    
    NostalgicPreparation::Ptr active = processor.nostalgic[currentNostalgicId]->aPrep;
    
    NostalgicPreparation::Ptr mod = processor.modNostalgic[currentModNostalgicId];
    
    if (name == cNostalgicParameterTypes[NostalgicId])
    {
        if (type == BKParameter)
        {
            currentNostalgicId = i;
            updateFields();
        }
        else    //BKModification
        {
            currentModNostalgicId = i;
            updateModFields();
        }
    }
    else if (name == cNostalgicParameterTypes[NostalgicWaveDistance])
    {
        if (type == BKParameter)
        {
            prep    ->setWaveDistance(i);
            active  ->setWaveDistance(i);
        }
        else    //BKModification
        {
            mod->setWaveDistance(i);
        }
    }
    else if (name == cNostalgicParameterTypes[NostalgicUndertow])
    {
        if (type == BKParameter)
        {
            prep    ->setUndertow(i);
            active  ->setUndertow(i);
        }
        else    //BKModification
        {
            mod->setUndertow(i);
        }
    }
    else if (name == cNostalgicParameterTypes[NostalgicTransposition])
    {
        if (type == BKParameter)
        {
            prep    ->setTransposition(f);
            active  ->setTransposition(f);
        }
        else    //BKModification
        {
            mod->setTransposition(f); 
        }
    }
    else if (name == cNostalgicParameterTypes[NostalgicGain])
    {
        if (type == BKParameter)
        {
            prep    ->setGain(f);
            active  ->setGain(f);
        }
        else    //BKModification
        {
            mod->setGain(f);
        }
    }
    else if (name == cNostalgicParameterTypes[NostalgicLengthMultiplier])
    {
        if (type == BKParameter)
        {
            prep    ->setLengthMultiplier(f);
            active  ->setLengthMultiplier(f);
        }
        else    //BKModification
        {
            mod->setLengthMultiplier(f);
        }
    }
    else if (name == cNostalgicParameterTypes[NostalgicBeatsToSkip])
    {
        if (type == BKParameter)
        {
            prep    ->setBeatsToSkip(i);
            active  ->setBeatsToSkip(i);
        }
        else    //BKModification
        {
            mod->setBeatsToSkip(i);
        }
    }
    else if (name == cNostalgicParameterTypes[NostalgicMode])
    {
        if (type == BKParameter)
        {
            prep    ->setMode((NostalgicSyncMode) i);
            active  ->setMode((NostalgicSyncMode) i);
        }
        else    //BKModification
        {
            mod->setMode((NostalgicSyncMode) i);
        }
    }
    else if (name == cNostalgicParameterTypes[NostalgicSyncTarget])
    {
        if (type == BKParameter)
        {
            prep    ->setSyncTarget(i);
            active  ->setSyncTarget(i);
        }
        else    //BKModification
        {
            mod->setSyncTarget(i);
        }
    }
    else if (name == cNostalgicParameterTypes[NostalgicTuning])
    {
        if (type == BKParameter)
        {
            prep    ->setTuning(processor.tPreparation[i]);
            active  ->setTuning(processor.tPreparation[i]);
        }
        else    //BKModification
        {
            mod->setTuning(processor.tPreparation[i]);
        }
    }
    else
    {
        DBG("Unregistered text field entered input.");
    }
}


void NostalgicViewController::updateFields(void)
{
    
    NostalgicPreparation::Ptr prep = processor.nostalgic[currentNostalgicId]->aPrep;

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

void NostalgicViewController::updateModFields(void)
{
    
    NostalgicPreparation::Ptr prep = processor.nostalgic[currentModNostalgicId]->aPrep;
    
    modNostalgicTF[NostalgicWaveDistance]      ->setText( String( prep->getWavedistance()), false);
    modNostalgicTF[NostalgicUndertow]          ->setText( String( prep->getUndertow()), false);
    modNostalgicTF[NostalgicTransposition]     ->setText( String( prep->getTransposition()), false);
    modNostalgicTF[NostalgicGain]              ->setText( String( prep->getGain()), false);
    modNostalgicTF[NostalgicLengthMultiplier]  ->setText( String( prep->getLengthMultiplier()), false);
    modNostalgicTF[NostalgicBeatsToSkip]       ->setText( String( prep->getBeatsToSkip()), false);
    modNostalgicTF[NostalgicMode]              ->setText( String( prep->getMode()), false);
    modNostalgicTF[NostalgicSyncTarget]        ->setText( String( prep->getSyncTarget()), false);
    modNostalgicTF[NostalgicTuning]            ->setText( String(prep->getTuning()->getId()), false);
}

void NostalgicViewController::bkMessageReceived (const String& message)
{
    if (message == "nostalgic/update")
    {
        //currentNostalgicId = processor.currentPiano->getPreparation();
        
        updateFields();
    }
}

