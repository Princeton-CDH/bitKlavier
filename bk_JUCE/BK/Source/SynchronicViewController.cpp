/*
  ==============================================================================

    SynchronicView.cpp
    Created: 15 Nov 2016 4:02:15pm
    Author:  Michael R Mulshine

  ==============================================================================
*/
#include "SynchronicViewController.h"

#include "BKUtilities.h"

#include "Keymap.h"

#include "Preparation.h"

//==============================================================================
SynchronicViewController::SynchronicViewController(BKAudioProcessor& p)
:
    processor(p),
    currentSynchronicId(0)
{
    SynchronicPreparation::Ptr prep = processor.synchronic[currentSynchronicId]->sPrep;
    
     // Labels
    synchronicL = OwnedArray<BKLabel>();
    synchronicL.ensureStorageAllocated(cSynchronicParameterTypes.size());
    
    for (int i = 0; i < cSynchronicParameterTypes.size(); i++)
    {
        synchronicL.set(i, new BKLabel());
        addAndMakeVisible(synchronicL[i]);
        synchronicL[i]->setName(cSynchronicParameterTypes[i]);
        synchronicL[i]->setText(cSynchronicParameterTypes[i], NotificationType::dontSendNotification);
    }
    
    // Text Fields
    synchronicTF = OwnedArray<BKTextField>();
    synchronicTF.ensureStorageAllocated(cSynchronicParameterTypes.size());
    
    for (int i = 0; i < cSynchronicParameterTypes.size(); i++)
    {
        synchronicTF.set(i, new BKTextField());
        addAndMakeVisible(synchronicTF[i]);
        synchronicTF[i]->addListener(this);
        synchronicTF[i]->setName(cSynchronicParameterTypes[i]);
    }
    
    modSynchronicTF = OwnedArray<BKTextField>();
    modSynchronicTF.ensureStorageAllocated(cSynchronicParameterTypes.size());
    
    for (int i = 0; i < cSynchronicParameterTypes.size(); i++)
    {
        modSynchronicTF.set(i, new BKTextField());
        addAndMakeVisible(synchronicTF[i]);
        modSynchronicTF[i]->addListener(this);
        modSynchronicTF[i]->setName("M"+cSynchronicParameterTypes[i]);
    }
    
    updateFields();
}



SynchronicViewController::~SynchronicViewController()
{
    
}

void SynchronicViewController::paint (Graphics& g)
{
    g.setColour(Colours::goldenrod);
    g.drawRect(getLocalBounds(), 1);
}

void SynchronicViewController::resized()
{
    // Labels
    int i = 0;
    int lX = 0;
    int lY = gComponentLabelHeight + gYSpacing;
    
    for (int n = 0; n < cSynchronicParameterTypes.size(); n++)
    {
        synchronicL[n]->setTopLeftPosition(lX, gYSpacing + lY * n);
    }
    
    // Text fields
    i = 0;
    int tfX = gComponentLabelWidth + gXSpacing;
    int tfY = gComponentTextFieldHeight + gYSpacing;

    
    for (int n = 0; n < cSynchronicParameterTypes.size(); n++)
    {
        synchronicTF[n]->setTopLeftPosition(tfX, gYSpacing + tfY * n);
    }

}

void SynchronicViewController::bkTextFieldDidChange(TextEditor& tf)
{
    
    String text = tf.getText();
    String name = tf.getName();

    BKTextFieldType type = BKParameter;
    
    if (name.startsWithChar('M')) type = BKModification;
    
    name = name.substring(1);
    
    float f = text.getFloatValue();
    int i = text.getIntValue();
    
    DBG(name + ": |" + text + "|");
    
    SynchronicPreparation::Ptr prep = processor.synchronic[currentSynchronicId]->sPrep;
    
    SynchronicPreparation::Ptr active = processor.synchronic[currentSynchronicId]->aPrep;
    
    
    
    if (name == cSynchronicParameterTypes[SynchronicId])
    {
        if (type == BKParameter)
        {
            currentSynchronicId = i;
            updateFields();
        }
        else // BKModification
        {
            currentModSynchronicId = i;
            updateModFields();
        }
    }
    else if (name == cSynchronicParameterTypes[SynchronicTempo])
    {
        if (type == BKParameter)
        {
            prep    ->setTempo(f);
            active  ->setTempo(f);
        }
        else // BKModification
        {
            
        }
    }
    else if (name == cSynchronicParameterTypes[SynchronicNumPulses])
    {
        if (type == BKParameter)
        {
            prep    ->setNumBeats(i);
            active  ->setNumBeats(i);
        }
        else // BKModification
        {
            
        }
    }
    else if (name == cSynchronicParameterTypes[SynchronicClusterMin])
    {
        if (type == BKParameter)
        {
            prep    ->setClusterMin(i);
            active  ->setClusterMin(i);
        }
        else // BKModification
        {
            
        }
    }
    else if (name == cSynchronicParameterTypes[SynchronicClusterMax])
    {
        if (type == BKParameter)
        {
            prep    ->setClusterMax(i);
            active  ->setClusterMax(i);
        }
        else // BKModification
        {
            
        }
    }
    else if (name == cSynchronicParameterTypes[SynchronicClusterThresh])
    {
        if (type == BKParameter)
        {
            prep    ->setClusterThresh(f);
            active  ->setClusterThresh(f);
        }
        else // BKModification
        {
            
        }
    }
    else if (name == cSynchronicParameterTypes[SynchronicMode])
    {
        if (type == BKParameter)
        {
            prep    ->setMode((SynchronicSyncMode) i);
            active  ->setMode((SynchronicSyncMode) i);
        }
        else // BKModification
        {
            
        }
    }
    else if (name == cSynchronicParameterTypes[SynchronicBeatsToSkip])
    {
        if (type == BKParameter)
        {
            prep    ->setBeatsToSkip(i);
            active  ->setBeatsToSkip(i);
        }
        else // BKModification
        {
            
        }
    }
    else if (name == cSynchronicParameterTypes[SynchronicBeatMultipliers])
    {
        Array<float> beatMults = stringToFloatArray(text);
        if (type == BKParameter)
        {
            prep    ->setBeatMultipliers(beatMults);
            active  ->setBeatMultipliers(beatMults);
        }
        else // BKModification
        {
            
        }
    }
    else if (name == cSynchronicParameterTypes[SynchronicLengthMultipliers])
    {
        Array<float> lenMults = stringToFloatArray(text);
        if (type == BKParameter)
        {
            prep    ->setLengthMultipliers(lenMults);
            active  ->setLengthMultipliers(lenMults);
        }
        else // BKModification
        {
            
        }
    }
    else if (name == cSynchronicParameterTypes[SynchronicAccentMultipliers])
    {
        Array<float> accentMults = stringToFloatArray(text);
        if (type == BKParameter)
        {
            prep    ->setAccentMultipliers(accentMults);
            active  ->setAccentMultipliers(accentMults);
        }
        else // BKModification
        {
            
        }
    }
    else if (name == cSynchronicParameterTypes[SynchronicTranspOffsets])
    {
        Array<float> transpOffsets = stringToFloatArray(text);
        if (type == BKParameter)
        {
            prep    ->setTranspOffsets(transpOffsets);
            active  ->setTranspOffsets(transpOffsets);
        }
        else // BKModification
        {
            
        }
    }
    else if (name == cSynchronicParameterTypes[SynchronicTuning])
    {
        if (type == BKParameter)
        {
            prep    ->setTuning(processor.tPreparation[i]);
            active  ->setTuning(processor.tPreparation[i]);
        }
        else // BKModification
        {
            
        }
    }
    else if (name == cSynchronicParameterTypes[AT1Mode])
    {
        if (type == BKParameter)
        {
            prep    ->setAdaptiveTempo1Mode((AdaptiveTempo1Mode)i);
            active  ->setAdaptiveTempo1Mode((AdaptiveTempo1Mode)i);
        }
        else // BKModification
        {
            
        }
    }
    else if (name == cSynchronicParameterTypes[AT1History])
    {
        if (type == BKParameter)
        {
            prep    ->setAdaptiveTempo1History(i);
            active  ->setAdaptiveTempo1History(i);
        }
        else // BKModification
        {
            
        }
    }
    else if (name == cSynchronicParameterTypes[AT1Subdivisions])
    {
        if (type == BKParameter)
        {
            prep    ->setAdaptiveTempo1Subdivisions(f);
            active  ->setAdaptiveTempo1Subdivisions(f);
        }
        else // BKModification
        {
            
        }
    }
    else if (name == cSynchronicParameterTypes[AT1Min])
    {
        if (type == BKParameter)
        {
            prep    ->setAdaptiveTempo1Min(f);
            active  ->setAdaptiveTempo1Min(f);
        }
        else // BKModification
        {
            
        }
    }
    else if (name == cSynchronicParameterTypes[AT1Max])
    {
        if (type == BKParameter)
        {
            prep    ->setAdaptiveTempo1Max(f);
            active  ->setAdaptiveTempo1Max(f);
        }
        else // BKModification
        {
        
        }
    }
    else
    {
        DBG("Unregistered text field entered input.");
    }
}


void SynchronicViewController::updateFields(void)
{
    SynchronicPreparation::Ptr prep   = processor.synchronic[currentSynchronicId]->aPrep;

    synchronicTF[SynchronicTempo]               ->setText(  String(                 prep->getTempo()), false);
    synchronicTF[SynchronicNumPulses]           ->setText(  String(                 prep->getNumBeats()), false);
    synchronicTF[SynchronicClusterMin]          ->setText(  String(                 prep->getClusterMin()), false);
    synchronicTF[SynchronicClusterMax]          ->setText(  String(                 prep->getClusterMax()), false);
    synchronicTF[SynchronicClusterThresh]       ->setText(  String(                 prep->getClusterThreshMS()), false);
    synchronicTF[SynchronicMode]                ->setText(  String(                 prep->getMode()), false);
    synchronicTF[SynchronicBeatsToSkip]         ->setText(  String(                 prep->getBeatsToSkip()), false);
    synchronicTF[SynchronicBeatMultipliers]     ->setText(  floatArrayToString(     prep->getBeatMultipliers()), false);
    synchronicTF[SynchronicLengthMultipliers]   ->setText(  floatArrayToString(     prep->getLengthMultipliers()), false);
    synchronicTF[SynchronicAccentMultipliers]   ->setText(  floatArrayToString(     prep->getAccentMultipliers()), false);
    synchronicTF[SynchronicTranspOffsets]       ->setText(  floatArrayToString(     prep->getTranspOffsets()), false);
    synchronicTF[SynchronicTuning]              ->setText(  String(                 prep->getTuning()->getId()), false);
    
    synchronicTF[AT1Mode]            ->setText(  String(                 prep->getAdaptiveTempo1Mode()), false);
    synchronicTF[AT1History]         ->setText(  String(                 prep->getAdaptiveTempo1History()), false);
    synchronicTF[AT1Subdivisions]    ->setText(  String(                 prep->getAdaptiveTempo1Subdivisions()), false);
    synchronicTF[AT1Min]             ->setText(  String(                 prep->getAdaptiveTempo1Min()), false);
    synchronicTF[AT1Max]             ->setText(  String(                 prep->getAdaptiveTempo1Max()), false);
}

void SynchronicViewController::updateModFields(void)
{
    // a Modification copy of Preparation to pull values from when updating
    SynchronicPreparation::Ptr prep   = processor.synchronic[currentSynchronicId]->aPrep;
    
    modSynchronicTF[SynchronicTempo]               ->setText(  String(                 prep->getTempo()), false);
    modSynchronicTF[SynchronicNumPulses]           ->setText(  String(                 prep->getNumBeats()), false);
    modSynchronicTF[SynchronicClusterMin]          ->setText(  String(                 prep->getClusterMin()), false);
    modSynchronicTF[SynchronicClusterMax]          ->setText(  String(                 prep->getClusterMax()), false);
    modSynchronicTF[SynchronicClusterThresh]       ->setText(  String(                 prep->getClusterThreshMS()), false);
    modSynchronicTF[SynchronicMode]                ->setText(  String(                 prep->getMode()), false);
    modSynchronicTF[SynchronicBeatsToSkip]         ->setText(  String(                 prep->getBeatsToSkip()), false);
    modSynchronicTF[SynchronicBeatMultipliers]     ->setText(  floatArrayToString(     prep->getBeatMultipliers()), false);
    modSynchronicTF[SynchronicLengthMultipliers]   ->setText(  floatArrayToString(     prep->getLengthMultipliers()), false);
    modSynchronicTF[SynchronicAccentMultipliers]   ->setText(  floatArrayToString(     prep->getAccentMultipliers()), false);
    modSynchronicTF[SynchronicTranspOffsets]       ->setText(  floatArrayToString(     prep->getTranspOffsets()), false);
    modSynchronicTF[SynchronicTuning]              ->setText(  String(                 prep->getTuning()->getId()), false);
    
    modSynchronicTF[AT1Mode]            ->setText(  String(                 prep->getAdaptiveTempo1Mode()), false);
    modSynchronicTF[AT1History]         ->setText(  String(                 prep->getAdaptiveTempo1History()), false);
    modSynchronicTF[AT1Subdivisions]    ->setText(  String(                 prep->getAdaptiveTempo1Subdivisions()), false);
    modSynchronicTF[AT1Min]             ->setText(  String(                 prep->getAdaptiveTempo1Min()), false);
    modSynchronicTF[AT1Max]             ->setText(  String(                 prep->getAdaptiveTempo1Max()), false);
}



void SynchronicViewController::bkMessageReceived (const String& message)
{
    if (message == "synchronic/update")
    {
        //currentSynchronicId = processor.currentPiano->getPreparation();
        
        updateFields();
    }
}


