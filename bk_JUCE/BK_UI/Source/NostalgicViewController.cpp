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
NostalgicViewController::NostalgicViewController(BKAudioProcessor& p, BKItemGraph* theGraph):
processor(p),
theGraph(theGraph)
{
    NostalgicPreparation::Ptr layer = processor.gallery->getStaticNostalgicPreparation(processor.updateState->currentNostalgicId);
    
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
    g.fillAll(Colours::transparentWhite);
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
    
    NostalgicPreparation::Ptr prep = processor.gallery->getStaticNostalgicPreparation(processor.updateState->currentNostalgicId);
    
    NostalgicPreparation::Ptr active = processor.gallery->getActiveNostalgicPreparation(processor.updateState->currentNostalgicId);
    
    NostalgicModPreparation::Ptr mod = processor.gallery->getNostalgicModPreparation(processor.updateState->currentModNostalgicId);
    
    if (name == cNostalgicParameterTypes[NostalgicId])
    {
        if (type == BKParameter)
        {
            int numNostalgic = processor.gallery->getNumNostalgic();
            
            if ((i+1) > numNostalgic)
            {
                processor.gallery->addNostalgic();
                processor.updateState->currentNostalgicId = numNostalgic;
                
            }
            else if (i >= 0)
            {
                processor.updateState->currentNostalgicId = i;
            }
            
            nostalgicTF[NostalgicId]->setText(String(processor.updateState->currentNostalgicId), false);
            
            updateFields();
        }
        else // BKModification
        {
            int numMod = processor.gallery->getNumNostalgicMod();
            
            if ((i+1) > numMod)
            {
                processor.gallery->addNostalgicMod();
                processor.updateState->currentModNostalgicId = numMod;
            }
            else if (i >= 0)
            {
                processor.updateState->currentModNostalgicId = i;
            }
            
            modNostalgicTF[NostalgicId]->setText(String(processor.updateState->currentModNostalgicId), false);
            
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
            mod->setParam(NostalgicWaveDistance, text);
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
             mod->setParam(NostalgicUndertow, text);
        }
    }
    else if (name == cNostalgicParameterTypes[NostalgicTransposition])
    {
        if (type == BKParameter)
        {
            prep    ->setTransposition(stringToFloatArray(text));
            active  ->setTransposition(stringToFloatArray(text));
        }
        else    //BKModification
        {
             mod->setParam(NostalgicTransposition, text);
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
             mod->setParam(NostalgicGain, text);
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
             mod->setParam(NostalgicLengthMultiplier, text);
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
             mod->setParam(NostalgicBeatsToSkip, text);
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
             mod->setParam(NostalgicMode, text);
        }
    }
    else if (name == cNostalgicParameterTypes[NostalgicSyncTarget])
    {
        if (type == BKParameter)
        {
            if (i <= processor.gallery->getNumSynchronic()-1)
            {
                prep    ->setSyncTarget(i);
                prep    ->setSyncTargetProcessor(processor.gallery->getSynchronicProcessor(i));
                active  ->setSyncTarget(i);
                active  ->setSyncTargetProcessor(processor.gallery->getSynchronicProcessor(i));
            }
            else
            {
                nostalgicTF[NostalgicSyncTarget]->undo();
            }
        }
        else    //BKModification
        {
             mod->setParam(NostalgicSyncTarget, text);
        }
    }
    else if (name == cNostalgicParameterTypes[NostalgicTuning])
    {
        if (type == BKParameter)
        {
            if (i < processor.gallery->getNumTuning())
            {
                prep    ->setTuning(processor.gallery->getTuning(i));
                active  ->setTuning(processor.gallery->getTuning(i));
            }
            else
                tf.setText("0", false);
        }
        else    //BKModification
        {
            if (i < processor.gallery->getNumTuning())
                mod->setParam(NostalgicTuning, text);
            else
                tf.setText("0", false);
        }
    }
    else
    {
        DBG("Unregistered text field entered input.");
    }
    
    if (type == BKModification) theGraph->updateMod(PreparationTypeNostalgic, processor.updateState->currentModNostalgicId);
}


void NostalgicViewController::updateFields(void)
{
    NostalgicPreparation::Ptr prep = processor.gallery->getActiveNostalgicPreparation(processor.updateState->currentNostalgicId);

    nostalgicTF[NostalgicWaveDistance]      ->setText( String( prep->getWavedistance()), false);
    nostalgicTF[NostalgicUndertow]          ->setText( String( prep->getUndertow()), false);
    nostalgicTF[NostalgicTransposition]     ->setText( floatArrayToString( prep->getTransposition()), false);
    nostalgicTF[NostalgicGain]              ->setText( String( prep->getGain()), false);
    nostalgicTF[NostalgicLengthMultiplier]  ->setText( String( prep->getLengthMultiplier()), false);
    nostalgicTF[NostalgicBeatsToSkip]       ->setText( String( prep->getBeatsToSkip()), false);
    nostalgicTF[NostalgicMode]              ->setText( String( prep->getMode()), false);
    nostalgicTF[NostalgicSyncTarget]        ->setText( String( prep->getSyncTarget()), false);
    nostalgicTF[NostalgicTuning]            ->setText( String(prep->getTuning()->getId()), false);
}

void NostalgicViewController::updateModFields(void)
{
    
    NostalgicModPreparation::Ptr prep = processor.gallery->getNostalgicModPreparation(processor.updateState->currentModNostalgicId);
    
    modNostalgicTF[NostalgicId] -> setText(String(processor.updateState->currentModNostalgicId), false);
    modNostalgicTF[NostalgicWaveDistance]      ->setText( prep->getParam(NostalgicWaveDistance), false);
    modNostalgicTF[NostalgicUndertow]          ->setText( prep->getParam(NostalgicUndertow), false);
    modNostalgicTF[NostalgicTransposition]     ->setText( prep->getParam(NostalgicTransposition), false);
    modNostalgicTF[NostalgicGain]              ->setText( prep->getParam(NostalgicGain), false);
    modNostalgicTF[NostalgicLengthMultiplier]  ->setText( prep->getParam(NostalgicLengthMultiplier), false);
    modNostalgicTF[NostalgicBeatsToSkip]       ->setText( prep->getParam(NostalgicBeatsToSkip), false);
    modNostalgicTF[NostalgicMode]              ->setText( prep->getParam(NostalgicMode), false);
    modNostalgicTF[NostalgicSyncTarget]        ->setText( prep->getParam(NostalgicSyncTarget), false);
    modNostalgicTF[NostalgicTuning]            ->setText( prep->getParam(NostalgicTuning), false);
}

void NostalgicViewController::bkMessageReceived (const String& message)
{
    if (message == "nostalgic/update")
    {
        updateFields();
    }
}

