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

//==============================================================================
SynchronicViewController::SynchronicViewController(BKAudioProcessor& p)
:
    processor(p),
    currentSynchronicId(0)
{
    SynchronicPreparation::Ptr prep = processor.sPreparation[currentSynchronicId];
    
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
    
    updateFields(currentSynchronicId);
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
    
    float f = text.getFloatValue();
    int i = text.getIntValue();
    
    DBG(name + ": |" + text + "|");
    
    SynchronicPreparation::Ptr prep = processor.sPreparation[currentSynchronicId];
    
    SynchronicPreparation::Ptr active = processor.activeSPreparation[currentSynchronicId];
    
    if (name == cSynchronicParameterTypes[SynchronicId])
    {
        currentSynchronicId = i;
        updateFields(currentSynchronicId);
    }
    else if (name == cSynchronicParameterTypes[SynchronicTempo])
    {
        prep    ->setTempo(f);
        active  ->setTempo(f);
    }
    else if (name == cSynchronicParameterTypes[SynchronicNumPulses])
    {
        prep    ->setNumPulses(i);
        active  ->setNumPulses(i);
    }
    else if (name == cSynchronicParameterTypes[SynchronicClusterMin])
    {
        prep    ->setClusterMin(i);
        active  ->setClusterMin(i);
    }
    else if (name == cSynchronicParameterTypes[SynchronicClusterMax])
    {
        prep    ->setClusterMax(i);
        active  ->setClusterMax(i);
    }
    else if (name == cSynchronicParameterTypes[SynchronicClusterThresh])
    {
        prep    ->setClusterThresh(f);
        active  ->setClusterThresh(f);
    }
    else if (name == cSynchronicParameterTypes[SynchronicMode])
    {
        prep    ->setMode((SynchronicSyncMode) i);
        active  ->setMode((SynchronicSyncMode) i);
    }
    else if (name == cSynchronicParameterTypes[SynchronicBeatsToSkip])
    {
        prep    ->setBeatsToSkip(i);
        active  ->setBeatsToSkip(i);
    }
    else if (name == cSynchronicParameterTypes[SynchronicBeatMultipliers])
    {
        Array<float> beatMults = stringToFloatArray(text);
        prep    ->setBeatMultipliers(beatMults);
        active  ->setBeatMultipliers(beatMults);
    }
    else if (name == cSynchronicParameterTypes[SynchronicLengthMultipliers])
    {
        Array<float> lenMults = stringToFloatArray(text);
        prep    ->setLengthMultipliers(lenMults);
        active  ->setLengthMultipliers(lenMults);
    }
    else if (name == cSynchronicParameterTypes[SynchronicAccentMultipliers])
    {
        Array<float> accentMults = stringToFloatArray(text);
        prep    ->setAccentMultipliers(accentMults);
        active  ->setAccentMultipliers(accentMults);
    }
    else if (name == cSynchronicParameterTypes[SynchronicTranspOffsets])
    {
        Array<float> transpOffsets = stringToFloatArray(text);
        prep    ->setTranspOffsets(transpOffsets);
        active  ->setTranspOffsets(transpOffsets);
    }
    else if (name == cSynchronicParameterTypes[SynchronicTuning])
    {
        prep    ->setTuning(processor.tPreparation[i]);
        active  ->setTuning(processor.tPreparation[i]);
    }
    else if (name == cSynchronicParameterTypes[AT1Mode])
    {
        prep    ->setAdaptiveTempo1Mode((AdaptiveTempo1Mode)i);
        active  ->setAdaptiveTempo1Mode((AdaptiveTempo1Mode)i);
    }
    else if (name == cSynchronicParameterTypes[AT1History])
    {
        prep    ->setAdaptiveTempo1History(i);
        active  ->setAdaptiveTempo1History(i);
    }
    else if (name == cSynchronicParameterTypes[AT1Subdivisions])
    {
        prep    ->setAdaptiveTempo1Subdivisions(f);
        active  ->setAdaptiveTempo1Subdivisions(f);
    }
    else if (name == cSynchronicParameterTypes[AT1Min])
    {
        prep    ->setAdaptiveTempo1Min(f);
        active  ->setAdaptiveTempo1Min(f);
    }
    else if (name == cSynchronicParameterTypes[AT1Max])
    {
        prep    ->setAdaptiveTempo1Max(f);
        active  ->setAdaptiveTempo1Max(f);
    }
    else
    {
        DBG("Unregistered text field entered input.");
    }
}


void SynchronicViewController::updateFields(int synchronicId)
{
    SynchronicPreparation::Ptr prep   = processor.activeSPreparation[synchronicId];
    
    // Set text.
    synchronicTF[SynchronicId]                  ->setText(  String(                 prep->getId()));
    synchronicTF[SynchronicTempo]               ->setText(  String(                 prep->getTempo()));
    synchronicTF[SynchronicNumPulses]           ->setText(  String(                 prep->getNumPulses()));
    synchronicTF[SynchronicClusterMin]          ->setText(  String(                 prep->getClusterMin()));
    synchronicTF[SynchronicClusterMax]          ->setText(  String(                 prep->getClusterMax()));
    synchronicTF[SynchronicClusterThresh]       ->setText(  String(                 prep->getClusterThresh()));
    synchronicTF[SynchronicMode]                ->setText(  String(                 prep->getMode()));
    synchronicTF[SynchronicBeatsToSkip]         ->setText(  String(                 prep->getBeatsToSkip()));
    synchronicTF[SynchronicBeatMultipliers]     ->setText(  floatArrayToString(     prep->getBeatMultipliers()));
    synchronicTF[SynchronicLengthMultipliers]   ->setText(  floatArrayToString(     prep->getLengthMultipliers()));
    synchronicTF[SynchronicAccentMultipliers]   ->setText(  floatArrayToString(     prep->getAccentMultipliers()));
    synchronicTF[SynchronicTranspOffsets]       ->setText(  floatArrayToString(     prep->getTranspOffsets()));
    synchronicTF[SynchronicTuning]              ->setText(  String(                 prep->getTuning()->getId()));
    
    synchronicTF[AT1Mode]            ->setText(  String(                 prep->getAdaptiveTempo1Mode()));
    synchronicTF[AT1History]         ->setText(  String(                 prep->getAdaptiveTempo1History()));
    synchronicTF[AT1Subdivisions]    ->setText(  String(                 prep->getAdaptiveTempo1Subdivisions()));
    synchronicTF[AT1Min]             ->setText(  String(                 prep->getAdaptiveTempo1Min()));
    synchronicTF[AT1Max]             ->setText(  String(                 prep->getAdaptiveTempo1Max()));
    
}


void SynchronicViewController::bkMessageReceived (const String& message)
{
    if (message == "synchronic/update")
    {
        //currentSynchronicId = processor.currentPiano->getPreparation();
        
        updateFields(currentSynchronicId);
    }
}


