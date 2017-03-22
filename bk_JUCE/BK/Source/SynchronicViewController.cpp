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
    SynchronicPreparation::Ptr prep = processor.gallery->getStaticSynchronicPreparation(currentSynchronicId);
    
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
    
    SynchronicPreparation::Ptr prep = processor.gallery->getStaticSynchronicPreparation(currentSynchronicId);
    
    SynchronicPreparation::Ptr active = processor.gallery->getActiveSynchronicPreparation(currentSynchronicId);
    
    SynchronicModPreparation::Ptr mod = processor.gallery->getSynchronicModPreparation(currentModSynchronicId);
    
    if (name == cSynchronicParameterTypes[SynchronicId])
    {
        
        if (type == BKParameter)
        {
            int numSynchronic = processor.gallery->getNumSynchronic();
        
            if ((i+1) > numSynchronic)
            {
                processor.gallery->addSynchronic();
                currentSynchronicId = numSynchronic;
            
            }
            else if (i >= 0)
            {
                currentSynchronicId = i;
            }
            
            synchronicTF[SynchronicId]->setText(String(currentSynchronicId), false);
            
            updateFields();
        }
        else // BKModification
        {
            int numMod = processor.gallery->getNumSynchronicMod();
            
            if ((i+1) > numMod)
            {
                processor.gallery->addSynchronicMod();
                currentModSynchronicId = numMod;
            }
            else if (i >= 0)
            {
                currentModSynchronicId = i;
            }
            
            modSynchronicTF[SynchronicId]->setText(String(currentModSynchronicId), false);
            
            updateModFields();
        }
    }

    else if (name == cSynchronicParameterTypes[SynchronicTempo])
    {
        if (type == BKParameter)
        {
            if (i < processor.gallery->getNumTempo())
            {
                prep    ->setTempoControl(processor.gallery->getTempo(i));
                active  ->setTempoControl(processor.gallery->getTempo(i));
            }
            else
                tf.setText("0", false);
            
        }
        else // BKModification
        {
            if (i < processor.gallery->getNumTempo())
                mod->setParam( SynchronicTempo, text);
            else
                tf.setText("0", false);
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
        Array<Array<float>> transposition = stringToArrayFloatArray(text);
        
        tf.setText(arrayFloatArrayToString(transposition));
        
        if (type == BKParameter)
        {
            prep    ->setTransposition(transposition);
            active  ->setTransposition(transposition);
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
            if (i < processor.gallery->getNumTuning())
            {
                prep    ->setTuning(processor.gallery->getTuning(i));
                active  ->setTuning(processor.gallery->getTuning(i));
            }
            else
                tf.setText("0", false);

        }
        else // BKModification
        {
            if (i < processor.gallery->getNumTuning())
                mod->setParam( SynchronicTuning, text);
            else
                tf.setText("0", false);
        }
    }
    else
    {
        DBG("Unregistered text field entered input.");
    }
}

void SynchronicViewController::updateFields(void)
{
    SynchronicPreparation::Ptr prep   = processor.gallery->getActiveSynchronicPreparation(currentSynchronicId);

    synchronicTF[SynchronicTempo]               ->setText(  String(                 prep->getTempoControl()->getId()), false);
    synchronicTF[SynchronicNumPulses]           ->setText(  String(                 prep->getNumBeats()), false);
    synchronicTF[SynchronicClusterMin]          ->setText(  String(                 prep->getClusterMin()), false);
    synchronicTF[SynchronicClusterMax]          ->setText(  String(                 prep->getClusterMax()), false);
    synchronicTF[SynchronicClusterThresh]       ->setText(  String(                 prep->getClusterThreshMS()), false);
    synchronicTF[SynchronicMode]                ->setText(  String(                 prep->getMode()), false);
    synchronicTF[SynchronicBeatsToSkip]         ->setText(  String(                 prep->getBeatsToSkip()), false);
    synchronicTF[SynchronicBeatMultipliers]     ->setText(  floatArrayToString(     prep->getBeatMultipliers()), false);
    synchronicTF[SynchronicLengthMultipliers]   ->setText(  floatArrayToString(     prep->getLengthMultipliers()), false);
    synchronicTF[SynchronicAccentMultipliers]   ->setText(  floatArrayToString(     prep->getAccentMultipliers()), false);
    synchronicTF[SynchronicTranspOffsets]       ->setText(  arrayFloatArrayToString(prep->getTransposition()), false);
    synchronicTF[SynchronicTuning]              ->setText(  String(                 prep->getTuning()->getId()), false);
    
}

void SynchronicViewController::updateModFields(void)
{
    // a Modification copy of Preparation to pull values from when updating
    SynchronicModPreparation::Ptr prep   = processor.gallery->getSynchronicModPreparation(currentModSynchronicId);
    
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
}



void SynchronicViewController::bkMessageReceived (const String& message)
{
    if (message == "synchronic/update")
    {
        
        updateFields();
    }
}


