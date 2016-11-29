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
    
    // Text Fields
    addAndMakeVisible(sKeymapTF);
    sKeymapTF.addListener(this);
    sKeymapTF.setName("SynchronicKeymap");
    //sKeymapTF.setText( intArrayToString( processor.sKeymap));
    
    
    addAndMakeVisible(sNumLayersTF);
    sNumLayersTF.addListener(this);
    sNumLayersTF.setName("NumSynchronicLayers");
    sNumLayersTF.setText( String( processor.numSynchronicLayers));
    
    addAndMakeVisible(sCurrentLayerTF);
    sCurrentLayerTF.addListener(this);
    sCurrentLayerTF.setName("CurrentSynchronicLayer");
    sCurrentLayerTF.setText( String( currentSynchronicLayer));
    
    addAndMakeVisible(sTempoTF);
    sTempoTF.addListener(this);
    sTempoTF.setName( cSynchronicParameterTypes[SynchronicTempo]);
    sTempoTF.setText( String( layer->getTempo()));
    
    addAndMakeVisible(sNumPulsesTF);
    sNumPulsesTF.addListener(this);
    sNumPulsesTF.setName(cSynchronicParameterTypes[SynchronicNumPulses]);
    sNumPulsesTF.setText( String( layer->getNumPulses()));
    
    addAndMakeVisible(sClusterMinTF);
    sClusterMinTF.addListener(this);
    sClusterMinTF.setName(cSynchronicParameterTypes[SynchronicClusterMin]);
    sClusterMinTF.setText( String( layer->getClusterMin()));
    
    addAndMakeVisible(sClusterMaxTF);
    sClusterMaxTF.addListener(this);
    sClusterMaxTF.setName(cSynchronicParameterTypes[SynchronicClusterMax]);
    sClusterMaxTF.setText( String( layer->getClusterMax()));
    
    addAndMakeVisible(sClusterThreshTF);
    sClusterThreshTF.addListener(this);
    sClusterThreshTF.setName(cSynchronicParameterTypes[SynchronicClusterThresh]);
    sClusterThreshTF.setText( String( layer->getClusterThresh()));
    
    addAndMakeVisible(sModeTF);
    sModeTF.addListener(this);
    sModeTF.setName(cSynchronicParameterTypes[SynchronicMode]);
    sModeTF.setText( String( layer->getMode()));
    
    addAndMakeVisible(sBeatsToSkipTF);
    sBeatsToSkipTF.addListener(this);
    sBeatsToSkipTF.setName(cSynchronicParameterTypes[SynchronicBeatsToSkip]);
    sBeatsToSkipTF.setText( String( layer->getBeatsToSkip()));
    
    addAndMakeVisible(sBeatMultipliersTF);
    sBeatMultipliersTF.addListener(this);
    sBeatMultipliersTF.setName(cSynchronicParameterTypes[SynchronicBeatMultipliers]);
    sBeatMultipliersTF.setText( floatArrayToString( layer->getBeatMultipliers()));
    
    addAndMakeVisible(sLengthMultipliersTF);
    sLengthMultipliersTF.addListener(this);
    sLengthMultipliersTF.setName(cSynchronicParameterTypes[SynchronicLengthMultipliers]);
    sLengthMultipliersTF.setText( floatArrayToString( layer->getLengthMultipliers()));
    
    addAndMakeVisible(sAccentMultipliersTF);
    sAccentMultipliersTF.addListener(this);
    sAccentMultipliersTF.setName(cSynchronicParameterTypes[SynchronicAccentMultipliers]);
    sAccentMultipliersTF.setText( floatArrayToString( layer->getAccentMultipliers()));
    
    addAndMakeVisible(sTuningOffsetsTF);
    sTuningOffsetsTF.addListener(this);
    sTuningOffsetsTF.setName(cSynchronicParameterTypes[SynchronicTuningOffsets]);
    sTuningOffsetsTF.setText( floatArrayToString( layer->getTuningOffsets()));
    
    addAndMakeVisible(sBasePitchTF);
    sBasePitchTF.addListener(this);
    sBasePitchTF.setName(cSynchronicParameterTypes[SynchronicBasePitch]);
    sBasePitchTF.setText( String( layer->getBasePitch()));
    
    // Labels
    addAndMakeVisible(sKeymapL);
    sKeymapL.setName("SynchronicKeymap");
    sKeymapL.setText("SynchronicKeymap", NotificationType::dontSendNotification);
    
    
    addAndMakeVisible(sNumLayersL);
    sNumLayersL.setName("NumSynchronicLayers");
    sNumLayersL.setText("NumSynchronicLayers", NotificationType::dontSendNotification);
    
    
    addAndMakeVisible(sCurrentLayerL);
    sCurrentLayerL.setName("CurrentSynchronicLayer");
    sCurrentLayerL.setText("CurrentSynchronicLayer", NotificationType::dontSendNotification);
    
    
    addAndMakeVisible(sTempoL);
    sTempoL.setName(cSynchronicParameterTypes[SynchronicTempo]);
    sTempoL.setText(cSynchronicParameterTypes[SynchronicTempo], NotificationType::dontSendNotification);
    
    
    addAndMakeVisible(sNumPulsesL);
    sNumPulsesL.setName(cSynchronicParameterTypes[SynchronicNumPulses]);
    sNumPulsesL.setText(cSynchronicParameterTypes[SynchronicNumPulses], NotificationType::dontSendNotification);
    
    
    addAndMakeVisible(sClusterMinL);
    sClusterMinL.setName(cSynchronicParameterTypes[SynchronicClusterMin]);
    sClusterMinL.setText(cSynchronicParameterTypes[SynchronicClusterMin], NotificationType::dontSendNotification);
    
    addAndMakeVisible(sClusterMaxL);
    sClusterMaxL.setName(cSynchronicParameterTypes[SynchronicClusterMax]);
    sClusterMaxL.setText(cSynchronicParameterTypes[SynchronicClusterMax], NotificationType::dontSendNotification);
    
    
    addAndMakeVisible(sClusterThreshL);
    sClusterThreshL.setName(cSynchronicParameterTypes[SynchronicClusterThresh]);
    sClusterThreshL.setText(cSynchronicParameterTypes[SynchronicClusterThresh], NotificationType::dontSendNotification);
    
    
    addAndMakeVisible(sModeL);
    sModeL.setName(cSynchronicParameterTypes[SynchronicMode]);
    sModeL.setText(cSynchronicParameterTypes[SynchronicMode], NotificationType::dontSendNotification);
    
    
    addAndMakeVisible(sBeatsToSkipL);
    sBeatsToSkipL.setName(cSynchronicParameterTypes[SynchronicBeatsToSkip]);
    sBeatsToSkipL.setText(cSynchronicParameterTypes[SynchronicBeatsToSkip], NotificationType::dontSendNotification);
    
    
    addAndMakeVisible(sBeatMultipliersL);
    sBeatMultipliersL.setColour(TextEditor::backgroundColourId, Colours::lightgoldenrodyellow);
    sBeatMultipliersL.setText(cSynchronicParameterTypes[SynchronicBeatMultipliers], NotificationType::dontSendNotification);
    
    
    addAndMakeVisible(sLengthMultipliersL);
    sLengthMultipliersL.setName(cSynchronicParameterTypes[SynchronicLengthMultipliers]);
    sLengthMultipliersL.setText(cSynchronicParameterTypes[SynchronicLengthMultipliers], NotificationType::dontSendNotification);
    
    
    addAndMakeVisible(sAccentMultipliersL);
    sAccentMultipliersL.setName(cSynchronicParameterTypes[SynchronicAccentMultipliers]);
    sAccentMultipliersL.setText(cSynchronicParameterTypes[SynchronicAccentMultipliers], NotificationType::dontSendNotification);
    
    
    addAndMakeVisible(sTuningOffsetsL);
    sTuningOffsetsL.setName(cSynchronicParameterTypes[SynchronicTuningOffsets]);
    sTuningOffsetsL.setText(cSynchronicParameterTypes[SynchronicTuningOffsets], NotificationType::dontSendNotification);
    
    
    addAndMakeVisible(sBasePitchL);
    sBasePitchL.setName(cSynchronicParameterTypes[SynchronicBasePitch]);
    sBasePitchL.setText(cSynchronicParameterTypes[SynchronicBasePitch], NotificationType::dontSendNotification);

}



SynchronicViewController::~SynchronicViewController()
{
    
}

void SynchronicViewController::switchToLayer(int numLayer)
{
    
    currentSynchronicLayer = numLayer;
    
    SynchronicPreparation::Ptr layer = processor.sPreparation[numLayer];
    
    // Set text.
    //sKeymapTF.setText( intArrayToString( processor.sKeymap));
    
    sCurrentLayerTF.setText( String( numLayer));

    sTempoTF.setText( String( layer->getTempo()));
    
    sNumPulsesTF.setText( String( layer->getNumPulses()));

    sClusterMinTF.setText( String( layer->getClusterMin()));

    sClusterMaxTF.setText( String( layer->getClusterMax()));
;
    sClusterThreshTF.setText( String( layer->getClusterThresh()));

    sModeTF.setText( String( layer->getMode()));

    sBeatsToSkipTF.setText( String( layer->getBeatsToSkip()));
    
    sBeatMultipliersTF.setText( floatArrayToString( layer->getBeatMultipliers()));

    sLengthMultipliersTF.setText( floatArrayToString( layer->getLengthMultipliers()));

    sAccentMultipliersTF.setText( floatArrayToString( layer->getAccentMultipliers()));

    sTuningOffsetsTF.setText( floatArrayToString( layer->getTuningOffsets()));
    
    sBasePitchTF.setText( String( layer->getBasePitch()));
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
        //processor.numSynchronicLayers = i;
    }
    else if (name == "CurrentSynchronicLayer")
    {
        switchToLayer(i);
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
    
    sTempoL.setTopLeftPosition(0,
                               gSynchronicTF_TopOffset + i++ * (gSynchronicL_Height + gSynchronic_YSpacing));
    
    sNumPulsesL.setTopLeftPosition(0,
                                   gSynchronicTF_TopOffset + i++ * (gSynchronicL_Height + gSynchronic_YSpacing));
    
    sClusterMinL.setTopLeftPosition(0,
                                    gSynchronicTF_TopOffset + i++ * (gSynchronicL_Height + gSynchronic_YSpacing));
    
    sClusterMaxL.setTopLeftPosition(0,
                                    gSynchronicTF_TopOffset + i++ * (gSynchronicL_Height + gSynchronic_YSpacing));
    
    sClusterThreshL.setTopLeftPosition(0,
                                       gSynchronicTF_TopOffset + i++ * (gSynchronicL_Height + gSynchronic_YSpacing));
    
    sModeL.setTopLeftPosition(0,
                              gSynchronicTF_TopOffset + i++ * (gSynchronicL_Height + gSynchronic_YSpacing));
    
    sBeatsToSkipL.setTopLeftPosition(0,
                                     gSynchronicTF_TopOffset + i++ * (gSynchronicL_Height + gSynchronic_YSpacing));
    
    sBeatMultipliersL.setTopLeftPosition(0,
                                         gSynchronicTF_TopOffset + i++ * (gSynchronicL_Height + gSynchronic_YSpacing));
    
    sLengthMultipliersL.setTopLeftPosition(0,
                                           gSynchronicTF_TopOffset + i++ * (gSynchronicL_Height + gSynchronic_YSpacing));
    
    sAccentMultipliersL.setTopLeftPosition(0,
                                           gSynchronicTF_TopOffset + i++ * (gSynchronicL_Height + gSynchronic_YSpacing));
    
    sTuningOffsetsL.setTopLeftPosition(0,
                                       gSynchronicTF_TopOffset + i++ * (gSynchronicL_Height + gSynchronic_YSpacing));
    
    sBasePitchL.setTopLeftPosition(0,
                          gSynchronicTF_TopOffset + i++ * (gSynchronicL_Height + gSynchronic_YSpacing));
    
    // Text fields
    int tfX = gSynchronicL_Width + gSynchronic_XSpacing;
    i = 0;
    sNumLayersTF.setTopLeftPosition(tfX,
                                    gSynchronicTF_TopOffset + i++ * (gSynchronicL_Height + gSynchronic_YSpacing));
    
    sCurrentLayerTF.setTopLeftPosition(tfX,
                                       gSynchronicTF_TopOffset + i++ * (gSynchronicL_Height + gSynchronic_YSpacing));
    
    sKeymapTF.setTopLeftPosition(tfX,
                                       gSynchronicTF_TopOffset + i++ * (gSynchronicL_Height + gSynchronic_YSpacing));
    
    sTempoTF.setTopLeftPosition(tfX,
                                gSynchronicTF_TopOffset + i++ * (gSynchronicL_Height + gSynchronic_YSpacing));
    
    sNumPulsesTF.setTopLeftPosition(tfX,
                                    gSynchronicTF_TopOffset + i++ * (gSynchronicL_Height + gSynchronic_YSpacing));
    
    sClusterMinTF.setTopLeftPosition(tfX,
                                     gSynchronicTF_TopOffset + i++ * (gSynchronicL_Height + gSynchronic_YSpacing));
    
    sClusterMaxTF.setTopLeftPosition(tfX,
                                     gSynchronicTF_TopOffset + i++ * (gSynchronicL_Height + gSynchronic_YSpacing));
    
    sClusterThreshTF.setTopLeftPosition(tfX,
                                        gSynchronicTF_TopOffset + i++ * (gSynchronicL_Height + gSynchronic_YSpacing));
    
    sModeTF.setTopLeftPosition(tfX,
                               gSynchronicTF_TopOffset + i++ * (gSynchronicL_Height + gSynchronic_YSpacing));
    
    sBeatsToSkipTF.setTopLeftPosition(tfX,
                                       gSynchronicTF_TopOffset + i++ * (gSynchronicL_Height + gSynchronic_YSpacing));
    
    sBeatMultipliersTF.setTopLeftPosition(tfX,
                                           gSynchronicTF_TopOffset + i++ * (gSynchronicL_Height + gSynchronic_YSpacing));
    
    sLengthMultipliersTF.setTopLeftPosition(tfX,
                                            gSynchronicTF_TopOffset + i++ * (gSynchronicL_Height + gSynchronic_YSpacing));
    
    sAccentMultipliersTF.setTopLeftPosition(tfX,
                                            gSynchronicTF_TopOffset + i++ * (gSynchronicL_Height + gSynchronic_YSpacing));
    
    sTuningOffsetsTF.setTopLeftPosition(tfX,
                                        gSynchronicTF_TopOffset + i++ * (gSynchronicL_Height + gSynchronic_YSpacing));
    
    sBasePitchTF.setTopLeftPosition(tfX,
                           gSynchronicTF_TopOffset + i++ * (gSynchronicL_Height + gSynchronic_YSpacing));

}


