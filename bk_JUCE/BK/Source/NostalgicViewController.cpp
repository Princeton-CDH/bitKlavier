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
    currentNostalgicLayer(0)
{
    NostalgicPreparation::Ptr layer = processor.nPreparation[currentNostalgicLayer];
    
    // Labels
    nostalgicL = OwnedArray<BKLabel>();
    nostalgicL.ensureStorageAllocated(cNostalgicParameterTypes.size());
    
    addAndMakeVisible(nKeymapL);
    nKeymapL.setName("NostalgicKeymap");
    nKeymapL.setText("NostalgicKeymap", NotificationType::dontSendNotification);
    
    
    addAndMakeVisible(nNumLayersL);
    nNumLayersL.setName("NumNostalgicLayers");
    nNumLayersL.setText("NumNostalgicLayers", NotificationType::dontSendNotification);
    
    
    addAndMakeVisible(nCurrentLayerL);
    nCurrentLayerL.setName("CurrentNostalgicLayer");
    nCurrentLayerL.setText("CurrentNostalgicLayer", NotificationType::dontSendNotification);
    
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
    
    
    addAndMakeVisible(nNumLayersTF);
    nNumLayersTF.addListener(this);
    nNumLayersTF.setName("NumNostalgicLayers");
    nNumLayersTF.setText( String( processor.numNostalgicLayers));
    
    
    addAndMakeVisible(nKeymapTF);
    nKeymapTF.addListener(this);
    nKeymapTF.setName("NostalgicKeymap");
    
    addAndMakeVisible(nCurrentLayerTF);
    nCurrentLayerTF.addListener(this);
    nCurrentLayerTF.setName("CurrentNostalgicLayer");
    
    for (int i = 0; i < cNostalgicParameterTypes.size(); i++)
    {
        nostalgicTF.set(i, new BKTextField());
        addAndMakeVisible(nostalgicTF[i]);
        nostalgicTF[i]->addListener(this);
        nostalgicTF[i]->setName(cNostalgicParameterTypes[i]);
    }
    
    updateFieldsToLayer(currentNostalgicLayer);
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
    int lY = gComponentLabelHeight + gComponentYSpacing;
    nNumLayersL.setTopLeftPosition(lX,      gComponentTopOffset + i++ * lY);
    
    nCurrentLayerL.setTopLeftPosition(lX,   gComponentTopOffset + i++ * lY);
    
    nKeymapL.setTopLeftPosition(lX,         gComponentTopOffset + i++ * lY);
    
    for (int n = 0; n < cNostalgicParameterTypes.size(); n++)
    {
        nostalgicL[n]->setTopLeftPosition(lX, gComponentTopOffset + (n+3) * lY);
    }
    
    // Text fields
    i = 0;
    int tfX = gComponentLabelWidth + gComponentXSpacing;
    int tfY = gComponentTextFieldHeight + gComponentYSpacing;
    nNumLayersTF.setTopLeftPosition(tfX, gComponentTopOffset + i++ * tfY);
    
    nCurrentLayerTF.setTopLeftPosition(tfX, gComponentTopOffset + i++ * tfY);
    
    nKeymapTF.setTopLeftPosition(tfX, gComponentTopOffset + i++ * tfY);
    
    
    for (int n = 0; n < cNostalgicParameterTypes.size(); n++)
    {
        nostalgicTF[n]->setTopLeftPosition(tfX, gComponentTopOffset + (n+3) * tfY);
    }
    
}

void NostalgicViewController::textFieldDidChange(TextEditor& tf)
{
    String text = tf.getText();
    String name = tf.getName();
    
    float f = text.getFloatValue();
    int i = text.getIntValue();
    
    DBG(name + ": |" + text + "|");
    
    NostalgicProcessor::Ptr   proc = processor.nProcessor[currentNostalgicLayer];
    
    NostalgicPreparation::Ptr prep = proc->getPreparation();
    
    Keymap::Ptr keymap             = proc->getKeymap();
    
    
    if (name == "NumNostalgicLayers")
    {
        processor.numNostalgicLayers = i;
    }
    else if (name == "CurrentNostalgicLayer")
    {
        updateFieldsToLayer(i);
    }
    else if (name == "NostalgicKeymap")
    {
        Array<int> keys = stringToIntArray(text);
        
        keymap->clear();
        for (auto note : keys)
        {
            keymap->addNote(note);
        }
        
    }
    else if (name == cNostalgicParameterTypes[NostalgicWaveDistance])
    {
        prep->setWaveDistance(i);
    }
    else if (name == cNostalgicParameterTypes[NostalgicUndertow])
    {
        prep->setUndertow(i);
    }
    else if (name == cNostalgicParameterTypes[NostalgicTransposition])
    {
        prep->setTransposition(f);
    }
    else if (name == cNostalgicParameterTypes[NostalgicGain])
    {
        prep->setGain(f);
    }
    else if (name == cNostalgicParameterTypes[NostalgicLengthMultiplier])
    {
        prep->setLengthMultiplier(f);
    }
    else if (name == cNostalgicParameterTypes[NostalgicBeatsToSkip])
    {
        prep->setBeatsToSkip(i);
    }
    else if (name == cNostalgicParameterTypes[NostalgicMode])
    {
        prep->setMode((NostalgicSyncMode) i);
    }
    else if (name == cNostalgicParameterTypes[NostalgicSyncTarget])
    {
        prep->setSyncTarget(i);
    }
    else if (name == cNostalgicParameterTypes[NostalgicTuningOffsets])
    {
        Array<float> tuningOffsets = stringToFloatArray(text);
        prep->setTuningOffsets(tuningOffsets);
    }
    else if (name == cNostalgicParameterTypes[NostalgicBasePitch])
    {
        prep->setBasePitch(i);
    }
    else
    {
        DBG("Unregistered text field entered input.");
    }
}


void NostalgicViewController::updateFieldsToLayer(int numLayer)
{
    
    currentNostalgicLayer = numLayer;
    
    NostalgicProcessor::Ptr   proc = processor.nProcessor[currentNostalgicLayer];
    
    NostalgicPreparation::Ptr prep = proc->getPreparation();
    
    Keymap::Ptr keymap             = proc->getKeymap();
    
    // Set text.
    nKeymapTF.setText( intArrayToString( keymap->keys()));
    
    nCurrentLayerTF.setText( String( numLayer));

    nostalgicTF[NostalgicWaveDistance]      ->setText( String( prep->getWavedistance()));
    nostalgicTF[NostalgicUndertow]          ->setText( String( prep->getUndertow()));
    nostalgicTF[NostalgicTransposition]     ->setText( String( prep->getTransposition()));
    nostalgicTF[NostalgicGain]              ->setText( String( prep->getGain()));
    nostalgicTF[NostalgicLengthMultiplier]  ->setText( String( prep->getLengthMultiplier()));
    nostalgicTF[NostalgicBeatsToSkip]       ->setText( String( prep->getBeatsToSkip()));
    nostalgicTF[NostalgicMode]              ->setText( String( prep->getMode()));
    nostalgicTF[NostalgicSyncTarget]        ->setText( String( prep->getSyncTarget()));
    nostalgicTF[NostalgicTuningOffsets]     ->setText( floatArrayToString( prep->getTuningOffsets()));
    nostalgicTF[NostalgicBasePitch]         ->setText( String( prep->getBasePitch()));
}
