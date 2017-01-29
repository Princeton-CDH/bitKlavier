/*
  ==============================================================================

    GeneralViewController.cpp
    Created: 30 Nov 2016 5:00:39pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "GeneralViewController.h"

//==============================================================================
GeneralViewController::GeneralViewController(BKAudioProcessor& p):
processor(p)
{
    // Labels
    generalL = OwnedArray<BKLabel>();
    generalL.ensureStorageAllocated(cGeneralParameterTypes.size());
    
    for (int i = 0; i < cGeneralParameterTypes.size(); i++)
    {
        generalL.set(i, new BKLabel());
        addAndMakeVisible(generalL[i]);
        generalL[i]->setName(cGeneralParameterTypes[i]);
        generalL[i]->setText(cGeneralParameterTypes[i], NotificationType::dontSendNotification);
    }
    
    // Text Fields
    generalTF = OwnedArray<BKTextField>();
    generalTF.ensureStorageAllocated(cGeneralParameterTypes.size());
    
    for (int i = 0; i < cGeneralParameterTypes.size(); i++)
    {
        generalTF.set(i, new BKTextField());
        addAndMakeVisible(generalTF[i]);
        generalTF[i]->addListener(this);
        generalTF[i]->setName(cGeneralParameterTypes[i]);
    }
    
    updateFields();
    
}

GeneralViewController::~GeneralViewController()
{
}

void GeneralViewController::paint (Graphics& g)
{

    g.setColour(Colours::goldenrod);
    g.drawRect(getLocalBounds(), 1);
}

void GeneralViewController::resized()
{
    // Labels
    int i = 0;
    int lX = 0;
    int lY = gComponentLabelHeight + gYSpacing;

    for (int n = 0; n < cGeneralParameterTypes.size(); n++)
    {
        generalL[n]->setTopLeftPosition(lX, gYSpacing + n * lY);
    }
    
    // Text fields
    i = 0;
    int tfX = gComponentLabelWidth + gXSpacing;
    int tfY = gComponentTextFieldHeight + gYSpacing;

    for (int n = 0; n < cGeneralParameterTypes.size(); n++)
    {
        generalTF[n]->setTopLeftPosition(tfX, gYSpacing + n * tfY);
    }
    
}

void GeneralViewController::bkTextFieldDidChange(TextEditor& tf)
{
    String text = tf.getText();
    String name = tf.getName();
    
    DBG(name + ": |" + text + "|");
    
    float f = text.getFloatValue();
    int i = text.getIntValue();
    
    GeneralSettings::Ptr gen = processor.general;
    
    if (name == cGeneralParameterTypes[GeneralTuningFundamental])
    {
        gen->setTuningFundamental(f);
    }
    else if (name == cGeneralParameterTypes[GeneralGlobalGain])
    {
        gen->setGlobalGain(f);
    }
    else if (name == cGeneralParameterTypes[GeneralSynchronicGain])
    {
        gen->setSynchronicGain(f);
    }
    else if (name == cGeneralParameterTypes[GeneralNostalgicGain])
    {
        gen->setNostalgicGain(f);
    }
    else if (name == cGeneralParameterTypes[GeneralDirectGain])
    {
        gen->setDirectGain(f);
    }
    else if (name == cGeneralParameterTypes[GeneralResonanceGain])
    {
        gen->setResonanceGain(f);
    }
    else if (name == cGeneralParameterTypes[GeneralHammerGain])
    {
        gen->setHammerGain(f);
    }
    else if (name == cGeneralParameterTypes[GeneralTempoMultiplier])
    {
        gen->setTempoMultiplier(f);
    }
    else if (name == cGeneralParameterTypes[GeneralResAndHammer])
    {
        gen->setResonanceAndHammer((bool)i);
    }
    else if (name == cGeneralParameterTypes[GeneralInvertSustain])
    {
        gen->setInvertSustain((bool)i);
    }
    else
    {
        // No other text field.
    }
}

void GeneralViewController::updateFields(void)
{
    GeneralSettings::Ptr gen = processor.general;
    
    generalTF[GeneralTuningFundamental] ->setText(   String( gen->getTuningFundamental()));
    generalTF[GeneralGlobalGain]        ->setText(   String( gen->getGlobalGain()));
    generalTF[GeneralSynchronicGain]    ->setText(   String( gen->getSynchronicGain()));
    generalTF[GeneralNostalgicGain]     ->setText(   String( gen->getNostalgicGain()));
    generalTF[GeneralDirectGain]        ->setText(   String( gen->getDirectGain()));
    generalTF[GeneralResonanceGain]     ->setText(   String( gen->getResonanceGain()));
    generalTF[GeneralHammerGain]        ->setText(   String( gen->getHammerGain()));
    generalTF[GeneralTempoMultiplier]   ->setText(   String( gen->getTempoMultiplier()));
    generalTF[GeneralResAndHammer]      ->setText(   String( gen->getResonanceAndHammer()));
    generalTF[GeneralInvertSustain]     ->setText(   String( gen->getInvertSustain()));
    
}
