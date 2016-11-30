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
    
    NostalgicPreparation::Ptr layer = processor.nPreparation[currentNostalgicLayer];
    
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

    }
    else if (name == cNostalgicParameterTypes[NostalgicWaveDistance])
    {
        layer->setWaveDistance(i);
    }
    else if (name == cNostalgicParameterTypes[NostalgicUndertow])
    {
        layer->setUndertow(i);
    }
    else if (name == cNostalgicParameterTypes[NostalgicTransposition])
    {
        layer->setTransposition(f);
    }
    else if (name == cNostalgicParameterTypes[NostalgicGain])
    {
        layer->setGain(f);
    }
    else if (name == cNostalgicParameterTypes[NostalgicLengthMultiplier])
    {
        layer->setLengthMultiplier(f);
    }
    else if (name == cNostalgicParameterTypes[NostalgicBeatsToSkip])
    {
        layer->setBeatsToSkip(i);
    }
    else if (name == cNostalgicParameterTypes[NostalgicMode])
    {
        layer->setMode((NostalgicSyncMode) i);
    }
    else if (name == cNostalgicParameterTypes[NostalgicSyncTarget])
    {
        layer->setSyncTarget(i);
    }
    else if (name == cNostalgicParameterTypes[NostalgicTuningOffsets])
    {
        Array<float> tuningOffsets = stringToFloatArray(text);
        layer->setTuningOffsets(tuningOffsets);
    }
    else if (name == cNostalgicParameterTypes[NostalgicBasePitch])
    {
        layer->setBasePitch(i);
    }
    else
    {
        DBG("Unregistered text field entered input.");
    }
}


void NostalgicViewController::updateFieldsToLayer(int numLayer)
{
    
    currentNostalgicLayer = numLayer;
    
    NostalgicPreparation::Ptr layer = processor.nPreparation[numLayer];
    
    // Set text.
    //sKeymapTF.setText( intArrayToString( processor.sKeymap));
    
    nCurrentLayerTF.setText( String( numLayer));

    nostalgicTF[NostalgicWaveDistance]->setText( String( layer->getWavedistance()));
    nostalgicTF[NostalgicUndertow]->setText( String( layer->getUndertow()));
    nostalgicTF[NostalgicTransposition]->setText( String( layer->getTransposition()));
    nostalgicTF[NostalgicGain]->setText( String( layer->getGain()));
    nostalgicTF[NostalgicLengthMultiplier]->setText( String( layer->getLengthMultiplier()));
    nostalgicTF[NostalgicBeatsToSkip]->setText( String( layer->getBeatsToSkip()));
    nostalgicTF[NostalgicMode]->setText( String( layer->getMode()));
    nostalgicTF[NostalgicSyncTarget]->setText( String( layer->getSyncTarget()));
    nostalgicTF[NostalgicTuningOffsets]->setText( floatArrayToString( layer->getTuningOffsets()));
    nostalgicTF[NostalgicBasePitch]->setText( String( layer->getBasePitch()));
}
