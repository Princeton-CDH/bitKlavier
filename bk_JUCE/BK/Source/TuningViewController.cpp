/*
  ==============================================================================

    TuningViewController.cpp
    Created: 14 Dec 2016 12:25:56pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "TuningViewController.h"

//==============================================================================
TuningViewController::TuningViewController(BKAudioProcessor& p):
processor(p),
currentTuningId(0)
{
    TuningPreparation::Ptr layer = processor.tPreparation[currentTuningId];
    
    // Labels
    tuningL = OwnedArray<BKLabel>();
    tuningL.ensureStorageAllocated(cTuningParameterTypes.size());
    
    for (int i = 0; i < cTuningParameterTypes.size(); i++)
    {
        tuningL.set(i, new BKLabel());
        addAndMakeVisible(tuningL[i]);
        tuningL[i]->setName(cTuningParameterTypes[i]);
        tuningL[i]->setText(cTuningParameterTypes[i], NotificationType::dontSendNotification);
    }
    
    // Text Fields
    tuningTF = OwnedArray<BKTextField>();
    tuningTF.ensureStorageAllocated(cTuningParameterTypes.size());
    
    for (int i = 0; i < cTuningParameterTypes.size(); i++)
    {
        tuningTF.set(i, new BKTextField());
        addAndMakeVisible(tuningTF[i]);
        tuningTF[i]->addListener(this);
        tuningTF[i]->setName(cTuningParameterTypes[i]);
    }
    
    updateFields(currentTuningId);
}

TuningViewController::~TuningViewController()
{
}

void TuningViewController::paint (Graphics& g)
{
    g.setColour(Colours::goldenrod);
    g.drawRect(getLocalBounds(), 1);
}

void TuningViewController::resized()
{
    // Labels
    int i = 0;
    int lX = 0;
    int lY = gComponentLabelHeight + gYSpacing;
    
    for (int n = 0; n < cTuningParameterTypes.size(); n++)
    {
        tuningL[n]->setTopLeftPosition(lX, gYSpacing + lY * n);
    }
    
    // Text fields
    i = 0;
    int tfX = gComponentLabelWidth + gXSpacing;
    int tfY = gComponentTextFieldHeight + gYSpacing;
    
    for (int n = 0; n < cTuningParameterTypes.size(); n++)
    {
        tuningTF[n]->setTopLeftPosition(tfX, gYSpacing + tfY * n);
    }
    
}

void TuningViewController::textFieldDidChange(TextEditor& tf)
{
    String text = tf.getText();
    String name = tf.getName();
    
    float f = text.getFloatValue();
    int i = text.getIntValue();
    
    DBG(name + ": |" + text + "|");
    
    TuningPreparation::Ptr prep = processor.tPreparation[currentTuningId];
    
    if (name == cTuningParameterTypes[TuningId])
    {
        currentTuningId = i;
        updateFields(currentTuningId);
    }
    else if (name == cTuningParameterTypes[TuningScale])
    {
        prep->setTuning((TuningSystem)i);
    }
    else if (name == cTuningParameterTypes[TuningFundamental])
    {
        prep->setFundamental((PitchClass)i);
    }
    else if (name == cTuningParameterTypes[TuningOffset])
    {
        prep->setFundamentalOffset(f);
    }
    else if (name == cTuningParameterTypes[TuningA1IntervalScale])
    {
        prep->setAdaptiveIntervalScale((TuningSystem)i);
    }
    else if (name == cTuningParameterTypes[TuningA1Inversional])
    {
        prep->setAdaptiveInversional((bool) i);
    }
    else if (name == cTuningParameterTypes[TuningA1AnchorScale])
    {
        prep->setAdaptiveAnchorScale((TuningSystem) i);
    }
    else if (name == cTuningParameterTypes[TuningA1AnchorFundamental])
    {
        prep->setAdaptiveAnchorFundamental((PitchClass) i);
    }
    else if (name == cTuningParameterTypes[TuningA1ClusterThresh])
    {
        prep->setAdaptiveClusterThresh(i);
    }
    else if (name == cTuningParameterTypes[TuningA1History])
    {
        prep->setAdaptiveHistory(i);
    }
    else if (name == cTuningParameterTypes[TuningCustomScale])
    {
        prep->setCustomScale( stringToFloatArray(text));
    }
    else
    {
        DBG("Unregistered text field.");
    }
}


void TuningViewController::updateFields(int tuningId)
{
    
    TuningPreparation::Ptr prep = processor.tPreparation[tuningId];
    
    tuningTF[TuningId]                  ->setText( String( tuningId));
    tuningTF[TuningScale]               ->setText( String( prep->getTuning()));
    tuningTF[TuningFundamental]         ->setText( String( prep->getFundamental()));
    tuningTF[TuningOffset]              ->setText( String( prep->getFundamentalOffset()));
    tuningTF[TuningA1IntervalScale]     ->setText( String( prep->getAdaptiveIntervalScale()));
    tuningTF[TuningA1Inversional]       ->setText( String( prep->getAdaptiveInversional()));
    tuningTF[TuningA1AnchorScale]       ->setText( String( prep->getAdaptiveAnchorScale()));
    tuningTF[TuningA1AnchorFundamental] ->setText( String( prep->getAdaptiveAnchorFundamental()));
    tuningTF[TuningA1ClusterThresh]     ->setText( String( prep->getAdaptiveClusterThresh()));
    tuningTF[TuningA1History]           ->setText( String( prep->getAdaptiveHistory()));
    tuningTF[TuningCustomScale]         ->setText( floatArrayToString( prep->getCustomScale()));

    
}

void TuningViewController::actionListenerCallback (const String& message)
{
    if (message == "tuning/update")
    {
        //currentTuningId = processor.currentPiano->getPreparation();
        
        updateFields(currentTuningId);
    }
}

void TuningViewController::comboBoxDidChange (ComboBox* box)
{
    
}
