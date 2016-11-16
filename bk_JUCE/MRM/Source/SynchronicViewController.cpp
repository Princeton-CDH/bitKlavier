/*
  ==============================================================================

    SynchronicView.cpp
    Created: 15 Nov 2016 4:02:15pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "SynchronicViewController.h"

#include "UIState.h"

//==============================================================================
SynchronicViewController::SynchronicViewController()
{
    // Text Fields
    addAndMakeVisible(sNumLayersTF);
    sNumLayersTF.addListener(this);
    sNumLayersTF.setName("NumSynchronicLayers");
    
    addAndMakeVisible(sCurrentLayerTF);
    sCurrentLayerTF.addListener(this);
    sCurrentLayerTF.setName("CurrentSynchronicLayer");
    
    addAndMakeVisible(sTempoTF);
    sTempoTF.addListener(this);
    sTempoTF.setName(cSynchronicParameterTypes[SynchronicTempo]);
    
    addAndMakeVisible(sNumPulsesTF);
    sNumPulsesTF.addListener(this);
    sNumPulsesTF.setName(cSynchronicParameterTypes[SynchronicNumPulses]);
    
    addAndMakeVisible(sClusterMinTF);
    sClusterMinTF.addListener(this);
    sClusterMinTF.setName(cSynchronicParameterTypes[SynchronicClusterMin]);
    
    addAndMakeVisible(sClusterMaxTF);
    sClusterMaxTF.addListener(this);
    sClusterMaxTF.setName(cSynchronicParameterTypes[SynchronicClusterMax]);
    
    addAndMakeVisible(sClusterThreshTF);
    sClusterThreshTF.addListener(this);
    sClusterThreshTF.setName(cSynchronicParameterTypes[SynchronicClusterThresh]);
    
    addAndMakeVisible(sModeTF);
    sModeTF.addListener(this);
    sModeTF.setName(cSynchronicParameterTypes[SynchronicMode]);
    
    addAndMakeVisible(sBeatsToSkipTF);
    sBeatsToSkipTF.addListener(this);
    sBeatsToSkipTF.setName(cSynchronicParameterTypes[SynchronicBeatsToSkip]);
    
    addAndMakeVisible(sBeatMultipliersTF);
    sBeatMultipliersTF.addListener(this);
    sBeatMultipliersTF.setName(cSynchronicParameterTypes[SynchronicBeatMultipliers]);
    
    addAndMakeVisible(sLengthMultipliersTF);
    sLengthMultipliersTF.addListener(this);
    sLengthMultipliersTF.setName(cSynchronicParameterTypes[SynchronicLengthMultipliers]);
    
    addAndMakeVisible(sAccentMultipliersTF);
    sAccentMultipliersTF.addListener(this);
    sAccentMultipliersTF.setName(cSynchronicParameterTypes[SynchronicAccentMultipliers]);
    
    addAndMakeVisible(sTuningOffsetsTF);
    sTuningOffsetsTF.addListener(this);
    sTuningOffsetsTF.setName(cSynchronicParameterTypes[SynchronicTuningOffsets]);
    
    addAndMakeVisible(sBasePitchTF);
    sBasePitchTF.addListener(this);
    sBasePitchTF.setName(cSynchronicParameterTypes[SynchronicBasePitch]);
    
    // Labels
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
    sNumLayersL.setText(cSynchronicParameterTypes[SynchronicNumPulses], NotificationType::dontSendNotification);
    
    
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

void SynchronicViewController::textEditorTextChanged (TextEditor& tf)
{
    String tfText = tf.getText();
    String tfName = tf.getName();
    
    
    float f = tfText.getFloatValue();
    int i = tfText.getIntValue();
    
    
    DBG(tfName + ": " + tfText);
    
    UIState *state = UIState::getInstance();
    
    if (tfName == cSynchronicParameterTypes[SynchronicTempo])
    {
        state->sTempo = f;
    }
    else if (tfName == cSynchronicParameterTypes[SynchronicNumPulses])
    {
        state->sNumPulses = i;
    }
    else if (tfName == cSynchronicParameterTypes[SynchronicClusterMin])
    {
        state->sClusterMin = i;
    }
    else if (tfName == cSynchronicParameterTypes[SynchronicClusterMax])
    {
        state->sClusterMax = i;
    }
    else if (tfName == cSynchronicParameterTypes[SynchronicClusterThresh])
    {
        state->sClusterThresh = f;
    }
    else if (tfName == cSynchronicParameterTypes[SynchronicMode])
    {
        state->sMode = (SynchronicSyncMode)i;
    }
    else if (tfName == cSynchronicParameterTypes[SynchronicBeatsToSkip])
    {
        state->sBeatsToSkip = f;
    }
    else if (tfName == cSynchronicParameterTypes[SynchronicBeatMultipliers])
    {
        //sBeatMultipliers = ;
    }
    else if (tfName == cSynchronicParameterTypes[SynchronicLengthMultipliers])
    {
        
    }
    else if (tfName == cSynchronicParameterTypes[SynchronicAccentMultipliers])
    {
        
    }
    else if (tfName == cSynchronicParameterTypes[SynchronicTuningOffsets])
    {
        
    }
    else if (tfName == cSynchronicParameterTypes[SynchronicBasePitch])
    {
        state->sBasePitch = i;
    }
    else if (tfName == "NumSynchronicLayers")
    {
        state->numSynchronicLayers = i;
    }
    else if (tfName == "CurrentSynchronicLayer")
    {
        state->currentSynchronicLayer = i;
    }
    else
    {
        DBG("Unregistered text field entered input.");
    }
    
    /// say that state has been updated
}

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
                                   i++ * (gSynchronicL_Height + gSynchronic_YSpacing));
    
    sCurrentLayerL.setTopLeftPosition(0,
                                      i++ * (gSynchronicL_Height + gSynchronic_YSpacing));
    
    sTempoL.setTopLeftPosition(0,
                               i++ * (gSynchronicL_Height + gSynchronic_YSpacing));
    
    sNumPulsesL.setTopLeftPosition(0,
                                   i++ * (gSynchronicL_Height + gSynchronic_YSpacing));
    
    sClusterMinL.setTopLeftPosition(0,
                                    i++ * (gSynchronicL_Height + gSynchronic_YSpacing));
    
    sClusterMaxL.setTopLeftPosition(0,
                                    i++ * (gSynchronicL_Height + gSynchronic_YSpacing));
    
    sClusterThreshL.setTopLeftPosition(0,
                                       i++ * (gSynchronicL_Height + gSynchronic_YSpacing));
    
    sModeL.setTopLeftPosition(0,
                              i++ * (gSynchronicL_Height + gSynchronic_YSpacing));
    
    sBeatsToSkipL.setTopLeftPosition(0,
                                     i++ * (gSynchronicL_Height + gSynchronic_YSpacing));
    
    sBeatMultipliersL.setTopLeftPosition(0,
                                         i++ * (gSynchronicL_Height + gSynchronic_YSpacing));
    
    sLengthMultipliersL.setTopLeftPosition(0,
                                           i++ * (gSynchronicL_Height + gSynchronic_YSpacing));
    
    sAccentMultipliersL.setTopLeftPosition(0,
                                           i++ * (gSynchronicL_Height + gSynchronic_YSpacing));
    
    sTuningOffsetsL.setTopLeftPosition(0,
                                       i++ * (gSynchronicL_Height + gSynchronic_YSpacing));
    
    sBasePitchL.setTopLeftPosition(0,
                          i++ * (gSynchronicL_Height + gSynchronic_YSpacing));
    
    // Text fields
    int tfX = gSynchronicL_Width + gSynchronic_XSpacing;
    i = 0;
    sNumLayersTF.setTopLeftPosition(tfX,
                                    i++ * (gSynchronicL_Height + gSynchronic_YSpacing));
    
    sCurrentLayerTF.setTopLeftPosition(tfX,
                                       i++ * (gSynchronicL_Height + gSynchronic_YSpacing));
    
    sTempoTF.setTopLeftPosition(tfX,
                                i++ * (gSynchronicL_Height + gSynchronic_YSpacing));
    
    sNumPulsesTF.setTopLeftPosition(tfX,
                                    i++ * (gSynchronicL_Height + gSynchronic_YSpacing));
    
    sClusterMinTF.setTopLeftPosition(tfX,
                                     i++ * (gSynchronicL_Height + gSynchronic_YSpacing));
    
    sClusterMaxTF.setTopLeftPosition(tfX,
                                     i++ * (gSynchronicL_Height + gSynchronic_YSpacing));
    
    sClusterThreshTF.setTopLeftPosition(tfX,
                                        i++ * (gSynchronicL_Height + gSynchronic_YSpacing));
    
    sModeTF.setTopLeftPosition(tfX,
                               i++ * (gSynchronicL_Height + gSynchronic_YSpacing));
    
    sBeatsToSkipTF.setTopLeftPosition(tfX,
                                      i++ * (gSynchronicL_Height + gSynchronic_YSpacing));
    
    sBeatMultipliersTF.setTopLeftPosition(tfX,
                                          i++ * (gSynchronicL_Height + gSynchronic_YSpacing));
    
    sLengthMultipliersTF.setTopLeftPosition(tfX,
                                            i++ * (gSynchronicL_Height + gSynchronic_YSpacing));
    
    sAccentMultipliersTF.setTopLeftPosition(tfX,
                                            i++ * (gSynchronicL_Height + gSynchronic_YSpacing));
    
    sTuningOffsetsTF.setTopLeftPosition(tfX,
                                        i++ * (gSynchronicL_Height + gSynchronic_YSpacing));
    
    sBasePitchTF.setTopLeftPosition(tfX,
                           i++ * (gSynchronicL_Height + gSynchronic_YSpacing));

}


