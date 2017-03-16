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
    
    TuningPreparation::Ptr prep = processor.tuning[currentTuningId]->sPrep;
    
    TuningPreparation::Ptr active = processor.tuning[currentTuningId]->aPrep;
    
    TuningModPreparation::Ptr mod = processor.modTuning[currentModTuningId];
    
    if (name == cTuningParameterTypes[TuningId])
    {
        if (type == BKParameter)
        {
            int numTuning = processor.tuning.size();
            
            if ((i+1) > numTuning)
            {
                processor.addTuning();
                currentTuningId = numTuning;
                
            }
            else if (i >= 0)
            {
                currentTuningId = i;
            }
            
            tuningTF[TuningId]->setText(String(currentTuningId), false);
            
            updateFields();
        }
        else // BKModification
        {
            int numMod = processor.modTuning.size();
            
            if ((i+1) > numMod)
            {
                processor.addTuningMod();
                currentModTuningId = numMod;
            }
            else if (i >= 0)
            {
                currentModTuningId = i;
            }
            
            modTuningTF[TuningId]->setText(String(currentModTuningId), false);
            
            updateModFields();
        }
    }
    else if (name == cTuningParameterTypes[TuningScale])
    {
        if (type == BKParameter)
        {
            prep->setTuning((TuningSystem)i);
            active->setTuning((TuningSystem)i);
        }
        else    //BKModification
        {
            mod->setParam(TuningScale, text);
        }
    }
    else if (name == cTuningParameterTypes[TuningFundamental])
    {
        if (type == BKParameter)
        {
            prep->setFundamental((PitchClass)i);
            active->setFundamental((PitchClass)i);
        }
        else    //BKModification
        {
            mod->setParam(TuningFundamental, text);
        }
    }
    else if (name == cTuningParameterTypes[TuningOffset])
    {
        if (type == BKParameter)
        {
            prep->setFundamentalOffset(f);
            active->setFundamentalOffset(f);
        }
        else    //BKModification
        {
            mod->setParam(TuningOffset, text);
        }
    }
    else if (name == cTuningParameterTypes[TuningA1IntervalScale])
    {
        if (type == BKParameter)
        {
            prep->setAdaptiveIntervalScale((TuningSystem)i);
            active->setAdaptiveIntervalScale((TuningSystem)i);
        }
        else    //BKModification
        {
            mod->setParam(TuningA1IntervalScale, text);
        }
    }
    else if (name == cTuningParameterTypes[TuningA1Inversional])
    {
        if (type == BKParameter)
        {
            prep->setAdaptiveInversional((bool) i);
            active->setAdaptiveInversional((bool) i);
        }
        else    //BKModification
        {
            mod->setParam(TuningA1Inversional, text);
        }
    }
    else if (name == cTuningParameterTypes[TuningA1AnchorScale])
    {
        if (type == BKParameter)
        {
            prep->setAdaptiveAnchorScale((TuningSystem) i);
            active->setAdaptiveAnchorScale((TuningSystem) i);
        }
        else    //BKModification
        {
            mod->setParam(TuningA1AnchorScale, text);
        }
    }
    else if (name == cTuningParameterTypes[TuningA1AnchorFundamental])
    {
        if (type == BKParameter)
        {
            prep->setAdaptiveAnchorFundamental((PitchClass) i);
            active->setAdaptiveAnchorFundamental((PitchClass) i);
        }
        else    //BKModification
        {
            mod->setParam(TuningA1AnchorFundamental, text);
        }
    }
    else if (name == cTuningParameterTypes[TuningA1ClusterThresh])
    {
        if (type == BKParameter)
        {
            prep->setAdaptiveClusterThresh(i);
            active->setAdaptiveClusterThresh(i);
        }
        else    //BKModification
        {
            mod->setParam(TuningA1ClusterThresh, text);
        }
    }
    else if (name == cTuningParameterTypes[TuningA1History])
    {
        if (type == BKParameter)
        {
            prep->setAdaptiveHistory(i);
            active->setAdaptiveHistory(i);
        }
        else    //BKModification
        {
            mod->setParam(TuningA1History, text);
        }
    }
    else if (name == cTuningParameterTypes[TuningCustomScale])
    {
        if (type == BKParameter)
        {
            //UI is in cents, internally represented as fractions of MIDI note value
            prep->setCustomScaleCents( stringToFloatArray(text));
            active->setCustomScaleCents( stringToFloatArray(text));
        }
        else    //BKModification
        {
            mod->setParam(TuningCustomScale, text);
        }
    }
    else if (name == cTuningParameterTypes[TuningAbsoluteOffsets])
    {
        if (type == BKParameter)
        {
            //UI is in cents, internally represented as fractions of MIDI note value
            prep->setAbsoluteOffsetCents(stringOrderedPairsToFloatArray(text, 128));
            active->setAbsoluteOffsetCents(stringOrderedPairsToFloatArray(text, 128));
        }
        else    //BKModification
        {
            mod->setParam(TuningAbsoluteOffsets, text);
        }
    }
    else
    {
        DBG("Unregistered text field.");
    }
}


void TuningViewController::updateFields()
{
    
    TuningPreparation::Ptr prep = processor.tuning[currentTuningId]->aPrep;

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
    tuningTF[TuningAbsoluteOffsets]     ->setText( offsetArrayToString( prep->getAbsoluteOffsets()), false);

    
}

void TuningViewController::updateModFields()
{
    
    TuningModPreparation::Ptr prep = processor.modTuning[currentModTuningId];
    
    modTuningTF[TuningScale]               ->setText( prep->getParam(TuningScale), false);
    modTuningTF[TuningFundamental]         ->setText( prep->getParam(TuningFundamental), false);
    modTuningTF[TuningOffset]              ->setText( prep->getParam(TuningOffset), false);
    modTuningTF[TuningA1IntervalScale]     ->setText( prep->getParam(TuningA1IntervalScale), false);
    modTuningTF[TuningA1Inversional]       ->setText( prep->getParam(TuningA1Inversional), false);
    modTuningTF[TuningA1AnchorScale]       ->setText( prep->getParam(TuningA1AnchorScale), false);
    modTuningTF[TuningA1AnchorFundamental] ->setText( prep->getParam(TuningA1AnchorFundamental), false);
    modTuningTF[TuningA1ClusterThresh]     ->setText( prep->getParam(TuningA1ClusterThresh), false);
    modTuningTF[TuningA1History]           ->setText( prep->getParam(TuningA1History), false);
    modTuningTF[TuningCustomScale]         ->setText( prep->getParam(TuningCustomScale), false);
    modTuningTF[TuningAbsoluteOffsets]     ->setText( prep->getParam(TuningAbsoluteOffsets), false);
    
}

void TuningViewController::bkMessageReceived(const String& message)
{
    if (message == "tuning/update")
    {
        
        updateFields();
    }
}
