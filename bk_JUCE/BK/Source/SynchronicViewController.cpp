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
SynchronicViewController::SynchronicViewController(BKAudioProcessor& p):
processor(p),
currentSynchronicId(0),
currentModSynchronicId(0)
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
        addAndMakeVisible(modSynchronicTF[i]);
        modSynchronicTF[i]->addListener(this);
        modSynchronicTF[i]->setName("M"+cSynchronicParameterTypes[i]);
    }
    
    updateModFields();
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
    int lY = gComponentLabelHeight + gYSpacing;
    
    float width = getWidth() * 0.25 - gXSpacing;
    
    for (int n = 0; n < cSynchronicParameterTypes.size(); n++)
    {
        synchronicL[n]->setBounds(0, gYSpacing + lY * n, width, synchronicL[0]->getHeight());
    }
    
    // Text fields
    int tfY = gComponentTextFieldHeight + gYSpacing;

    float height = synchronicTF[0]->getHeight();
    width *= 1.5;
    
    for (int n = 0; n < cSynchronicParameterTypes.size(); n++)
    {
        synchronicTF[n]->setBounds(synchronicL[0]->getRight()+gXSpacing, gYSpacing + tfY * n, width, height);
        modSynchronicTF[n]->setBounds(synchronicTF[0]->getRight()+gXSpacing, gYSpacing + tfY * n, width, height);
    }

}

void SynchronicViewController::bkTextFieldDidChange(TextEditor& tf)
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
    
    SynchronicPreparation::Ptr prep = processor.synchronic[currentSynchronicId]->sPrep;
    
    SynchronicPreparation::Ptr active = processor.synchronic[currentSynchronicId]->aPrep;
    
    SynchronicModPreparation::Ptr mod = processor.modSynchronic[currentModSynchronicId];
    
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
            mod->setParam( SynchronicTempo, text);
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
            mod->setParam( SynchronicNumPulses, text);
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
            mod->setParam( SynchronicClusterMin, text);
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
            mod->setParam( SynchronicClusterMax, text);
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
            mod->setParam( SynchronicClusterThresh, text);
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
            mod->setParam( SynchronicMode, text);
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
            mod->setParam( SynchronicBeatsToSkip, text);
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
            mod->setParam( SynchronicBeatMultipliers, text);
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
           mod->setParam( SynchronicLengthMultipliers, text);
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
            mod->setParam( SynchronicAccentMultipliers, text);
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
            mod->setParam( SynchronicTranspOffsets, text);
        }
    }
    else if (name == cSynchronicParameterTypes[SynchronicTuning])
    {
        if (type == BKParameter)
        {
            prep    ->setTuning(processor.tuning[i]);
            active  ->setTuning(processor.tuning[i]);
        }
        else // BKModification
        {
            mod->setParam( SynchronicTuning, text);
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
            mod->setParam( AT1Mode, text);
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
            mod->setParam( AT1History, text);
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
            mod->setParam( AT1Subdivisions, text);
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
            mod->setParam( AT1Min, text);
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
            mod->setParam( AT1Max, text);
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
    SynchronicModPreparation::Ptr prep   = processor.modSynchronic[currentModSynchronicId];
    
    modSynchronicTF[SynchronicTempo]               ->setText(  prep->getParam(SynchronicTempo), false);
    modSynchronicTF[SynchronicNumPulses]           ->setText(  prep->getParam(SynchronicNumPulses), false);
    modSynchronicTF[SynchronicClusterMin]          ->setText(  prep->getParam(SynchronicClusterMin), false);
    modSynchronicTF[SynchronicClusterMax]          ->setText(  prep->getParam(SynchronicClusterMax), false);
    modSynchronicTF[SynchronicClusterThresh]       ->setText(  prep->getParam(SynchronicClusterThresh), false);
    modSynchronicTF[SynchronicMode]                ->setText(  prep->getParam(SynchronicMode), false);
    modSynchronicTF[SynchronicBeatsToSkip]         ->setText(  prep->getParam(SynchronicBeatsToSkip), false);
    modSynchronicTF[SynchronicBeatMultipliers]     ->setText(  prep->getParam(SynchronicBeatMultipliers), false);
    modSynchronicTF[SynchronicLengthMultipliers]   ->setText(  prep->getParam(SynchronicLengthMultipliers), false);
    modSynchronicTF[SynchronicAccentMultipliers]   ->setText(  prep->getParam(SynchronicAccentMultipliers), false);
    modSynchronicTF[SynchronicTranspOffsets]       ->setText(  prep->getParam(SynchronicTranspOffsets), false);
    modSynchronicTF[SynchronicTuning]              ->setText(  prep->getParam(SynchronicTuning), false);
    
    modSynchronicTF[AT1Mode]            ->setText(  prep->getParam(AT1Mode), false);
    modSynchronicTF[AT1History]         ->setText(  prep->getParam(AT1History), false);
    modSynchronicTF[AT1Subdivisions]    ->setText(  prep->getParam(AT1Subdivisions), false);
    modSynchronicTF[AT1Min]             ->setText(  prep->getParam(AT1Min), false);
    modSynchronicTF[AT1Max]             ->setText(  prep->getParam(AT1Max), false);
}



void SynchronicViewController::bkMessageReceived (const String& message)
{
    if (message == "synchronic/update")
    {
        //currentSynchronicId = processor.currentPiano->getPreparation();
        
        updateFields();
    }
}


