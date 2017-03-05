/*
  ==============================================================================

    TempoViewController.cpp
    Created: 5 Mar 2017 9:28:27am
    Author:  Daniel Trueman

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "TempoViewController.h"

//==============================================================================
TempoViewController::TempoViewController(BKAudioProcessor& p):
processor(p),
currentTempoId(0),
currentModTempoId(0)
{
    // Labels
    tempoL = OwnedArray<BKLabel>();
    tempoL.ensureStorageAllocated(cTempoParameterTypes.size());
    
    for (int i = 0; i < cTempoParameterTypes.size(); i++)
    {
        tempoL.set(i, new BKLabel());
        addAndMakeVisible(tempoL[i]);
        tempoL[i]->setName(cTempoParameterTypes[i]);
        tempoL[i]->setText(cTempoParameterTypes[i], NotificationType::dontSendNotification);
    }
    
    // Text Fields
    tempoTF = OwnedArray<BKTextField>();
    tempoTF.ensureStorageAllocated(cTempoParameterTypes.size());
    
    for (int i = 0; i < cTempoParameterTypes.size(); i++)
    {
        tempoTF.set(i, new BKTextField());
        addAndMakeVisible(tempoTF[i]);
        tempoTF[i]->addListener(this);
        tempoTF[i]->setName(cTempoParameterTypes[i]);
    }
    
    modTempoTF = OwnedArray<BKTextField>();
    modTempoTF.ensureStorageAllocated(cTempoParameterTypes.size());
    
    for (int i = 0; i < cTempoParameterTypes.size(); i++)
    {
        modTempoTF.set(i, new BKTextField());
        addAndMakeVisible(modTempoTF[i]);
        modTempoTF[i]->addListener(this);
        modTempoTF[i]->setName("M"+cTempoParameterTypes[i]);
    }
    
    updateModFields();
    updateFields();
}

TempoViewController::~TempoViewController()
{
}

void TempoViewController::paint (Graphics& g)
{
    g.setColour(Colours::goldenrod);
    g.drawRect(getLocalBounds(), 1);
}

void TempoViewController::resized()
{
    // Labels
    int lY = gComponentLabelHeight + gYSpacing;
    
    float width = getWidth() * 0.25 - gXSpacing;
    
    for (int n = 0; n < cTempoParameterTypes.size(); n++)
    {
        tempoL[n]->setBounds(0, gYSpacing + lY * n, width, tempoL[0]->getHeight());
    }
    
    // Text fields
    int tfY = gComponentTextFieldHeight + gYSpacing;
    
    float height = tempoTF[0]->getHeight();
    width *= 1.5;
    
    for (int n = 0; n < cTempoParameterTypes.size(); n++)
    {
        tempoTF[n]->setBounds(tempoL[0]->getRight()+gXSpacing, gYSpacing + tfY * n, width, height);
        modTempoTF[n]->setBounds(tempoTF[0]->getRight()+gXSpacing, gYSpacing + tfY * n, width, height);
    }
    
}

void TempoViewController::bkTextFieldDidChange(TextEditor& tf)
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
    
    TempoPreparation::Ptr       prep    = processor.tempo[currentTempoId]->sPrep;
    TempoPreparation::Ptr       active  = processor.tempo[currentTempoId]->aPrep;
    TempoModPreparation::Ptr    mod     = processor.modTempo[currentModTempoId];
    
    /*
     TempoId = 0,
     Tempo,
     AT1Mode,
     AT1History,
     AT1Subdivisions,
     AT1Min,
     AT1Max,
     TempoParameterTypeNil
    */ 
 
    if (name == cTempoParameterTypes[TempoId])
    {
        if (type == BKParameter)
        {
            int numTempo = processor.tempo.size();
            
            if ((i+1) > numTempo)
            {
                processor.addTempo();
                currentTempoId = numTempo;
                
            }
            else if (i >= 0)
            {
                currentTempoId = i;
            }
            
            tempoTF[TempoId]->setText(String(currentTempoId), false);
            
            updateFields();
        }
        else // BKModification
        {
            int numMod = processor.modTempo.size();
            
            if ((i+1) > numMod)
            {
                processor.addTempoMod();
                currentModTempoId = numMod;
            }
            else if (i >= 0)
            {
                currentModTempoId = i;
            }
            
            modTempoTF[TempoId]->setText(String(currentModTempoId), false);
            
            updateModFields();
        }
    }
    /*
     else if (name == cTempoParameterTypes[TempoReset])
     {
     if (type == BKParameter)
     {
     
     // ** actually do nothing here; TempoReset is Mod only
     //Array<int> keys = keymapStringToIntArray(text);
     //Array<int> keys = stringToIntArray(text);
     //prep->getResetMap()->setKeymap(keys);
     //active->getResetMap()->setKeymap(keys);
     //TempoTF[TempoReset]->setText( intArrayToString(prep->getResetMap()->keys()));
     
     }
     else    //BKModification
     {
     mod->setParam(TempoReset, text);
     }
     }
     */
    else
    {
        DBG("Unregistered text field.");
    }
}


void TempoViewController::updateFields()
{
    
    TempoPreparation::Ptr prep = processor.tempo[currentTempoId]->aPrep;
    
    //tempoTF[TempoScale]               ->setText( String( prep->getTempo()), false);
    //TempoTF[TempoReset]         ->setText( intArrayToString( prep->getResetMap()->keys()), false);
    
    
}

void TempoViewController::updateModFields()
{
    
    TempoModPreparation::Ptr prep = processor.modTempo[currentModTempoId];
    
    /*
    modTempoTF[TempoScale]               ->setText( prep->getParam(TempoScale), false);
    modTempoTF[TempoFundamental]         ->setText( prep->getParam(TempoFundamental), false);
    modTempoTF[TempoOffset]              ->setText( prep->getParam(TempoOffset), false);
    modTempoTF[TempoA1IntervalScale]     ->setText( prep->getParam(TempoA1IntervalScale), false);
    modTempoTF[TempoA1Inversional]       ->setText( prep->getParam(TempoA1Inversional), false);
    modTempoTF[TempoA1AnchorScale]       ->setText( prep->getParam(TempoA1AnchorScale), false);
    modTempoTF[TempoA1AnchorFundamental] ->setText( prep->getParam(TempoA1AnchorFundamental), false);
    modTempoTF[TempoA1ClusterThresh]     ->setText( prep->getParam(TempoA1ClusterThresh), false);
    modTempoTF[TempoA1History]           ->setText( prep->getParam(TempoA1History), false);
    modTempoTF[TempoCustomScale]         ->setText( prep->getParam(TempoCustomScale), false);
    modTempoTF[TempoAbsoluteOffsets]     ->setText( prep->getParam(TempoAbsoluteOffsets), false);
    //modTempoTF[TempoReset]         ->setText( prep->getParam(TempoReset), false);
     */
    
    
}

void TempoViewController::bkMessageReceived(const String& message)
{
    if (message == "Tempo/update")
    {
        
        updateFields();
    }
}
