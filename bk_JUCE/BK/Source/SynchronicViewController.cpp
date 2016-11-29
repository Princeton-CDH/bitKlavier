/*
  ==============================================================================

    SynchronicView.cpp
    Created: 15 Nov 2016 4:02:15pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
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
    
    updateFieldsForLayer(currentSynchronicLayer);
}



SynchronicViewController::~SynchronicViewController()
{
    
}

void SynchronicViewController::updateFieldsForLayer(int numLayer)
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
    synchronicTF[SynchronicTuningOffsets]->setText( floatArrayToString( layer->getTuningOffsets()));
    synchronicTF[SynchronicBasePitch]->setText( String( layer->getBasePitch()));
}

void SynchronicViewController::textFieldDidChange(TextEditor& tf)
{
    String text = tf.getText();
    String name = tf.getName();
    
    float f = text.getFloatValue();
    int i = text.getIntValue();
    
    DBG(name + ": |" + text + "|");
    
    //BKState *bk = BKState::getInstance();
    SynchronicPreparation::Ptr layer = processor.sPreparation[currentSynchronicLayer];
    
    if (name == cSynchronicParameterTypes[SynchronicTempo])
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
        for (auto n : lenMults)
        {
            DBG("len: " + String(n));
        }
        DBG("len to string: " + floatArrayToString(lenMults));
        layer->setLengthMultipliers(lenMults);
    }
    else if (name == cSynchronicParameterTypes[SynchronicAccentMultipliers])
    {
        Array<float> accentMults = stringToFloatArray(text);
        layer->setAccentMultipliers(accentMults);
    }
    else if (name == cSynchronicParameterTypes[SynchronicTuningOffsets])
    {
        Array<float> tuningOffsets = stringToFloatArray(text);
        layer->setTuningOffsets(tuningOffsets);
    }
    else if (name == cSynchronicParameterTypes[SynchronicBasePitch])
    {
        layer->setBasePitch(i);
    }
    else if (name == "NumSynchronicLayers")
    {
        processor.numSynchronicLayers = i;
    }
    else if (name == "CurrentSynchronicLayer")
    {
        updateFieldsForLayer(i);
    }
    else if (name == "SynchronicKeymap")
    {
        
    }
    else
    {
        DBG("Unregistered text field entered input.");
    }
}

#if !TEXT_CHANGE_INTERNAL
void SynchronicViewController::textEditorFocusLost(TextEditor& tf)
{
    if (shouldChange)
    {
        textFieldDidChange(tf);
        shouldChange = false;
    }
}

void SynchronicViewController::textEditorReturnKeyPressed(TextEditor& tf)
{
    if (shouldChange)
    {
        textFieldDidChange(tf);
        shouldChange = false;
    }
}

void SynchronicViewController::textEditorEscapeKeyPressed(TextEditor& tf)
{
    if (shouldChange)
    {
        textFieldDidChange(tf);
        shouldChange = false;
    }
}


void SynchronicViewController::textEditorTextChanged(TextEditor& tf)
{
    shouldChange = true;
}

#endif
void SynchronicViewController::paint (Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.setColour (Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component
}

void SynchronicViewController::resized()
{
    int i = 0;
    
    // Labels
    sNumLayersL.setTopLeftPosition(0,
                                   gSynchronicTF_TopOffset + i++ * (gSynchronicL_Height + gSynchronic_YSpacing));
    
    sCurrentLayerL.setTopLeftPosition(0,
                                      gSynchronicTF_TopOffset + i++ * (gSynchronicL_Height + gSynchronic_YSpacing));
    
    sKeymapL.setTopLeftPosition(0,
                                      gSynchronicTF_TopOffset + i++ * (gSynchronicL_Height + gSynchronic_YSpacing));
    
    for (int n = 0; n < cSynchronicParameterTypes.size(); n++)
    {
        synchronicL[n]->setTopLeftPosition(0,
                                   gSynchronicTF_TopOffset + (n+3) * (gSynchronicL_Height + gSynchronic_YSpacing));
    }
    
    // Text fields
    int tfX = gSynchronicL_Width + gSynchronic_XSpacing;
    i = 0;
    sNumLayersTF.setTopLeftPosition(tfX,
                                    gSynchronicTF_TopOffset + i++ * (gSynchronicL_Height + gSynchronic_YSpacing));
    
    sCurrentLayerTF.setTopLeftPosition(tfX,
                                       gSynchronicTF_TopOffset + i++ * (gSynchronicL_Height + gSynchronic_YSpacing));
    
    sKeymapTF.setTopLeftPosition(tfX,
                                       gSynchronicTF_TopOffset + i++ * (gSynchronicL_Height + gSynchronic_YSpacing));
    
    
    for (int n = 0; n < cSynchronicParameterTypes.size(); n++)
    {
        synchronicTF[n]->setTopLeftPosition(tfX,
                                    gSynchronicTF_TopOffset + (n+3) * (gSynchronicL_Height + gSynchronic_YSpacing));
    }

}


