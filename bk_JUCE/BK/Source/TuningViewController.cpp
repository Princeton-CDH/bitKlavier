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
currentTuningId(0),
currentModTuningId(0)
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
    
    modTuningTF = OwnedArray<BKTextField>();
    modTuningTF.ensureStorageAllocated(cTuningParameterTypes.size());
    
    for (int i = 0; i < cTuningParameterTypes.size(); i++)
    {
        modTuningTF.set(i, new BKTextField());
        addAndMakeVisible(modTuningTF[i]);
        modTuningTF[i]->addListener(this);
        modTuningTF[i]->setName("M"+cTuningParameterTypes[i]);
    }
    
    updateModFields();
    updateFields();
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
    int lY = gComponentLabelHeight + gYSpacing;
    
    float width = getWidth() * 0.25 - gXSpacing;
    
    for (int n = 0; n < cTuningParameterTypes.size(); n++)
    {
        tuningL[n]->setBounds(0, gYSpacing + lY * n, width, tuningL[0]->getHeight());
    }
    
    // Text fields
    int tfY = gComponentTextFieldHeight + gYSpacing;
    
    float height = tuningTF[0]->getHeight();
    width *= 1.5;
    
    for (int n = 0; n < cTuningParameterTypes.size(); n++)
    {
        tuningTF[n]->setBounds(tuningL[0]->getRight()+gXSpacing, gYSpacing + tfY * n, width, height);
        modTuningTF[n]->setBounds(tuningTF[0]->getRight()+gXSpacing, gYSpacing + tfY * n, width, height);
    }
    
}

void TuningViewController::bkTextFieldDidChange(TextEditor& tf)
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
    
    TuningPreparation::Ptr prep = processor.tPreparation[currentTuningId];
    
    TuningPreparation::Ptr mod = processor.modTuning[currentModTuningId];
    
    if (name == cTuningParameterTypes[TuningId])
    {
        if (type == BKParameter)
        {
            currentTuningId = i;
            updateFields();
        }
        else //BKModification
        {
            currentModTuningId = i;
            updateModFields();
        }
    }
    else if (name == cTuningParameterTypes[TuningScale])
    {
        if (type == BKParameter)
        {
            prep->setTuning((TuningSystem)i);
        }
        else    //BKModification
        {
            mod->setTuning((TuningSystem)i);
        }
    }
    else if (name == cTuningParameterTypes[TuningFundamental])
    {
        if (type == BKParameter)
        {
            prep->setFundamental((PitchClass)i);
        }
        else    //BKModification
        {
            mod->setFundamental((PitchClass)i);
        }
    }
    else if (name == cTuningParameterTypes[TuningOffset])
    {
        if (type == BKParameter)
        {
            prep->setFundamentalOffset(f);
        }
        else    //BKModification
        {
            mod->setFundamentalOffset(f);
        }
    }
    else if (name == cTuningParameterTypes[TuningA1IntervalScale])
    {
        if (type == BKParameter)
        {
            prep->setAdaptiveIntervalScale((TuningSystem)i);
        }
        else    //BKModification
        {
            mod->setAdaptiveIntervalScale((TuningSystem)i);
        }
    }
    else if (name == cTuningParameterTypes[TuningA1Inversional])
    {
        if (type == BKParameter)
        {
            prep->setAdaptiveInversional((bool) i);
        }
        else    //BKModification
        {
            mod->setAdaptiveInversional((bool) i);
        }
    }
    else if (name == cTuningParameterTypes[TuningA1AnchorScale])
    {
        if (type == BKParameter)
        {
            prep->setAdaptiveAnchorScale((TuningSystem) i);
        }
        else    //BKModification
        {
            mod->setAdaptiveAnchorScale((TuningSystem) i);
        }
    }
    else if (name == cTuningParameterTypes[TuningA1AnchorFundamental])
    {
        if (type == BKParameter)
        {
            prep->setAdaptiveAnchorFundamental((PitchClass) i);
        }
        else    //BKModification
        {
            mod->setAdaptiveAnchorFundamental((PitchClass) i);
        }
    }
    else if (name == cTuningParameterTypes[TuningA1ClusterThresh])
    {
        if (type == BKParameter)
        {
            prep->setAdaptiveClusterThresh(i);
        }
        else    //BKModification
        {
            mod->setAdaptiveClusterThresh(i);
        }
    }
    else if (name == cTuningParameterTypes[TuningA1History])
    {
        if (type == BKParameter)
        {
            prep->setAdaptiveHistory(i);
        }
        else    //BKModification
        {
            mod->setAdaptiveHistory(i);
        }
    }
    else if (name == cTuningParameterTypes[TuningCustomScale])
    {
        if (type == BKParameter)
        {
            prep->setCustomScale( stringToFloatArray(text));
        }
        else    //BKModification
        {
            mod->setCustomScale( stringToFloatArray(text));
        }
    }
    else
    {
        DBG("Unregistered text field.");
    }
}


void TuningViewController::updateFields()
{
    
    TuningPreparation::Ptr prep = processor.tPreparation[currentTuningId];

    tuningTF[TuningScale]               ->setText( String( prep->getTuning()), false);
    tuningTF[TuningFundamental]         ->setText( String( prep->getFundamental()), false);
    tuningTF[TuningOffset]              ->setText( String( prep->getFundamentalOffset()), false);
    tuningTF[TuningA1IntervalScale]     ->setText( String( prep->getAdaptiveIntervalScale()), false);
    tuningTF[TuningA1Inversional]       ->setText( String( prep->getAdaptiveInversional()), false);
    tuningTF[TuningA1AnchorScale]       ->setText( String( prep->getAdaptiveAnchorScale()), false);
    tuningTF[TuningA1AnchorFundamental] ->setText( String( prep->getAdaptiveAnchorFundamental()), false);
    tuningTF[TuningA1ClusterThresh]     ->setText( String( prep->getAdaptiveClusterThresh()), false);
    tuningTF[TuningA1History]           ->setText( String( prep->getAdaptiveHistory()), false);
    tuningTF[TuningCustomScale]         ->setText( floatArrayToString( prep->getCustomScale()), false);

    
}

void TuningViewController::updateModFields()
{
    
    TuningPreparation::Ptr prep = processor.tPreparation[currentTuningId];
    
    modTuningTF[TuningScale]               ->setText( String( prep->getTuning()), false);
    modTuningTF[TuningFundamental]         ->setText( String( prep->getFundamental()), false);
    modTuningTF[TuningOffset]              ->setText( String( prep->getFundamentalOffset()), false);
    modTuningTF[TuningA1IntervalScale]     ->setText( String( prep->getAdaptiveIntervalScale()), false);
    modTuningTF[TuningA1Inversional]       ->setText( String( prep->getAdaptiveInversional()), false);
    modTuningTF[TuningA1AnchorScale]       ->setText( String( prep->getAdaptiveAnchorScale()), false);
    modTuningTF[TuningA1AnchorFundamental] ->setText( String( prep->getAdaptiveAnchorFundamental()), false);
    modTuningTF[TuningA1ClusterThresh]     ->setText( String( prep->getAdaptiveClusterThresh()), false);
    modTuningTF[TuningA1History]           ->setText( String( prep->getAdaptiveHistory()), false);
    modTuningTF[TuningCustomScale]         ->setText( floatArrayToString( prep->getCustomScale()), false);
    
    
}

void TuningViewController::bkMessageReceived(const String& message)
{
    if (message == "tuning/update")
    {
        //currentTuningId = processor.currentPiano->getPreparation();
        
        updateFields();
    }
}
