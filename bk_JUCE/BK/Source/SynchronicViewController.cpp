/*
  ==============================================================================

    SynchronicView.cpp
    Created: 15 Nov 2016 4:02:15pm
    Author:  Michael R Mulshine

  ==============================================================================
*/
#include "SynchronicViewController.h"

#include "BKUtilities.h"

//==============================================================================
SynchronicViewController::SynchronicViewController(BKAudioProcessor& p)
:
    processor(p),
    currentSynchronicLayer(0)
{
    
    SynchronicPreparation::Ptr layer = processor.sPreparation[currentSynchronicLayer];
    
     // Labels
    synchronicL = OwnedArray<BKLabel>();
    synchronicL.ensureStorageAllocated(cSynchronicParameterTypes.size());

    addAndMakeVisible(sKeymapL);
    sKeymapL.setName("SynchronicKeymap");
    sKeymapL.setText("SynchronicKeymap", NotificationType::dontSendNotification);
    
    
    addAndMakeVisible(sNumLayersL);
    sNumLayersL.setName("NumSynchronicLayers");
    sNumLayersL.setText("NumSynchronicLayers", NotificationType::dontSendNotification);
    
    
    addAndMakeVisible(sCurrentLayerL);
    sCurrentLayerL.setName("CurrentSynchronicLayer");
    sCurrentLayerL.setText("CurrentSynchronicLayer", NotificationType::dontSendNotification);
    
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
    
    
    addAndMakeVisible(sNumLayersTF);
    sNumLayersTF.addListener(this);
    sNumLayersTF.setName("NumSynchronicLayers");
    sNumLayersTF.setText( String( processor.numSynchronicLayers));
    
    
    addAndMakeVisible(sKeymapTF);
    sKeymapTF.addListener(this);
    sKeymapTF.setName("SynchronicKeymap");
    
    addAndMakeVisible(sCurrentLayerTF);
    sCurrentLayerTF.addListener(this);
    sCurrentLayerTF.setName("CurrentSynchronicLayer");
    
    for (int i = 0; i < cSynchronicParameterTypes.size(); i++)
    {
        synchronicTF.set(i, new BKTextField());
        addAndMakeVisible(synchronicTF[i]);
        synchronicTF[i]->addListener(this);
        synchronicTF[i]->setName(cSynchronicParameterTypes[i]);
    }
    
    updateFieldsToLayer(currentSynchronicLayer);
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
    int lY = gComponentLabelHeight + gComponentYSpacing;
    
    sNumLayersL.setTopLeftPosition(lX,      gComponentTopOffset + i++ * lY);
    
    sCurrentLayerL.setTopLeftPosition(lX,   gComponentTopOffset + i++ * lY);
    
    sKeymapL.setTopLeftPosition(lX,         gComponentTopOffset + i++ * lY);
    
    for (int n = 0; n < cSynchronicParameterTypes.size(); n++)
    {
        synchronicL[n]->setTopLeftPosition(lX, gComponentTopOffset + (n+3) * lY);
    }
    
    // Text fields
    i = 0;
    int tfX = gComponentLabelWidth + gComponentXSpacing;
    int tfY = gComponentTextFieldHeight + gComponentYSpacing;
    sNumLayersTF.setTopLeftPosition(tfX, gComponentTopOffset + i++ * tfY);
    
    sCurrentLayerTF.setTopLeftPosition(tfX, gComponentTopOffset + i++ * tfY);
    
    sKeymapTF.setTopLeftPosition(tfX, gComponentTopOffset + i++ * tfY);
    
    
    for (int n = 0; n < cSynchronicParameterTypes.size(); n++)
    {
        synchronicTF[n]->setTopLeftPosition(tfX, gComponentTopOffset + (n+3) * tfY);
    }

}

void SynchronicViewController::textFieldDidChange(TextEditor& tf)
{
    String text = tf.getText();
    String name = tf.getName();
    
    float f = text.getFloatValue();
    int i = text.getIntValue();
    
    DBG(name + ": |" + text + "|");
    
    SynchronicPreparation::Ptr layer = processor.sPreparation[currentSynchronicLayer];
    
    if (name == "NumSynchronicLayers")
    {
        processor.numSynchronicLayers = i;
    }
    else if (name == "CurrentSynchronicLayer")
    {
        updateFieldsToLayer(i);
    }
    else if (name == "SynchronicKeymap")
    {
        
    }
    else if (name == cSynchronicParameterTypes[SynchronicTempo])
    {
        layer->setTempo(f);
    }
    else if (name == cSynchronicParameterTypes[SynchronicNumPulses])
    {
        layer->setNumPulses(i);
    }
    else if (name == cSynchronicParameterTypes[SynchronicClusterMin])
    {
        layer->setClusterMin(i);
    }
    else if (name == cSynchronicParameterTypes[SynchronicClusterMax])
    {
        layer->setClusterMax(i);
    }
    else if (name == cSynchronicParameterTypes[SynchronicClusterThresh])
    {
        layer->setClusterThresh(f);
    }
    else if (name == cSynchronicParameterTypes[SynchronicMode])
    {
        layer->setMode((SynchronicSyncMode) i);
    }
    else if (name == cSynchronicParameterTypes[SynchronicBeatsToSkip])
    {
        layer->setBeatsToSkip(i);
    }
    else if (name == cSynchronicParameterTypes[SynchronicBeatMultipliers])
    {
        Array<float> beatMults = stringToFloatArray(text);
        layer->setBeatMultipliers(beatMults);
    }
    else if (name == cSynchronicParameterTypes[SynchronicLengthMultipliers])
    {
        Array<float> lenMults = stringToFloatArray(text);
        layer->setLengthMultipliers(lenMults);
    }
    else if (name == cSynchronicParameterTypes[SynchronicAccentMultipliers])
    {
        Array<float> accentMults = stringToFloatArray(text);
        layer->setAccentMultipliers(accentMults);
    }
#if OFFSETS
    else if (name == cSynchronicParameterTypes[SynchronicTranspOffsets])
    {
        Array<float> transpOffsets = stringToFloatArray(text);
        layer->setTranspOffsets(transpOffsets);
    }
#endif
    else if (name == cSynchronicParameterTypes[SynchronicTuningOffsets])
    {
        Array<float> tuningOffsets = stringToFloatArray(text);
        layer->setTuningOffsets(tuningOffsets);
    }

    else if (name == cSynchronicParameterTypes[SynchronicBasePitch])
    {
        layer->setBasePitch(i);
    }
    else
    {
        DBG("Unregistered text field entered input.");
    }
}


void SynchronicViewController::updateFieldsToLayer(int numLayer)
{
    
    currentSynchronicLayer = numLayer;
    
    SynchronicPreparation::Ptr layer = processor.sPreparation[numLayer];
    
    // Set text.
    //sKeymapTF.setText( intArrayToString( processor.sKeymap));
    
    sCurrentLayerTF.setText( String( numLayer));
    
    synchronicTF[SynchronicTempo]->setText( String( layer->getTempo()));
    synchronicTF[SynchronicNumPulses]->setText( String( layer->getNumPulses()));
    synchronicTF[SynchronicClusterMin]->setText( String( layer->getClusterMin()));
    synchronicTF[SynchronicClusterMax]->setText( String( layer->getClusterMax()));
    synchronicTF[SynchronicClusterThresh]->setText( String( layer->getClusterThresh()));
    synchronicTF[SynchronicMode]->setText( String( layer->getMode()));
    synchronicTF[SynchronicBeatsToSkip]->setText( String( layer->getBeatsToSkip()));
    synchronicTF[SynchronicBeatMultipliers]->setText( floatArrayToString( layer->getBeatMultipliers()));
    synchronicTF[SynchronicLengthMultipliers]->setText( floatArrayToString( layer->getLengthMultipliers()));
    synchronicTF[SynchronicAccentMultipliers]->setText( floatArrayToString( layer->getAccentMultipliers()));
#if OFFSETS
    synchronicTF[SynchronicTranspOffsets]->setText( floatArrayToString( layer->getTranspOffsets()));
#endif
    synchronicTF[SynchronicTuningOffsets]->setText( floatArrayToString( layer->getTuningOffsets()));
    synchronicTF[SynchronicBasePitch]->setText( String( layer->getBasePitch()));
}



