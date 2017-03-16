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
     TempoBPM,
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
    else if (name == cTempoParameterTypes[TempoBPM])
    {
         if (type == BKParameter)
         {
             prep->setTempo(f);
             active->setTempo(f);
             
         }
         else    //BKModification
         {
             mod->setParam(TempoBPM, text);
         }
    }
    else if (name == cTempoParameterTypes[AT1Mode])
    {
        if (type == BKParameter)
        {
            prep->setAdaptiveTempo1Mode(AdaptiveTempo1Mode(i));
            active->setAdaptiveTempo1Mode(AdaptiveTempo1Mode(i));
            
        }
        else    //BKModification
        {
            mod->setParam(AT1Mode, text);
        }
    }
    else if (name == cTempoParameterTypes[AT1History])
    {
        if (type == BKParameter)
        {
            prep->setAdaptiveTempo1History(i);
            active->setAdaptiveTempo1History(i);
            
        }
        else    //BKModification
        {
            mod->setParam(AT1History, text);
        }
    }
    else if (name == cTempoParameterTypes[AT1Subdivisions])
    {
        if (type == BKParameter)
        {
            prep->setAdaptiveTempo1Subdivisions(f);
            active->setAdaptiveTempo1Subdivisions(f);
            
        }
        else    //BKModification
        {
            mod->setParam(AT1Subdivisions, text);
        }
    }
    else if (name == cTempoParameterTypes[AT1Min])
    {
        if (type == BKParameter)
        {
            prep->setAdaptiveTempo1Min(f);
            active->setAdaptiveTempo1Min(f);
            
        }
        else    //BKModification
        {
            mod->setParam(AT1Min, text);
        }
    }
    else if (name == cTempoParameterTypes[AT1Max])
    {
        if (type == BKParameter)
        {
            prep->setAdaptiveTempo1Max(f);
            active->setAdaptiveTempo1Max(f);
            
        }
        else    //BKModification
        {
            mod->setParam(AT1Max, text);
        }
    }
    else if (name == cTempoParameterTypes[TempoSystem])
    {
        if (type == BKParameter)
        {
            prep->setTempoSystem(TempoType(i));
            active->setTempoSystem(TempoType(i));
            
        }
        else    //BKModification
        {
            mod->setParam(TempoSystem, text);
        }
    }

    
    
    else
    {
        DBG("Unregistered text field.");
    }
}


void TempoViewController::updateFields()
{

    TempoPreparation::Ptr prep = processor.tempo[currentTempoId]->aPrep;
    
    tempoTF[TempoBPM]           ->setText( String( prep->getTempo()), false);
    tempoTF[AT1Mode]            ->setText( String( prep->getAdaptiveTempo1Mode()), false);
    tempoTF[AT1History]         ->setText( String( prep->getAdaptiveTempo1History()), false);
    tempoTF[AT1Subdivisions]    ->setText( String( prep->getAdaptiveTempo1Subdivisions()), false);
    tempoTF[AT1Min]             ->setText( String( prep->getAdaptiveTempo1Min()), false);
    tempoTF[AT1Max]             ->setText( String( prep->getAdaptiveTempo1Max()), false);
    tempoTF[TempoSystem]        ->setText( String( prep->getTempoSystem()), false);
    
    
}

void TempoViewController::updateModFields()
{
    
    TempoModPreparation::Ptr prep = processor.modTempo[currentModTempoId];

    modTempoTF[TempoBPM]            ->setText( prep->getParam(TempoBPM), false);
    modTempoTF[AT1Mode]             ->setText( prep->getParam(AT1Mode), false);
    modTempoTF[AT1History]          ->setText( prep->getParam(AT1History), false);
    modTempoTF[AT1Subdivisions]     ->setText( prep->getParam(AT1Subdivisions), false);
    modTempoTF[AT1Min]              ->setText( prep->getParam(AT1Min), false);
    modTempoTF[AT1Max]              ->setText( prep->getParam(AT1Max), false);
    modTempoTF[TempoSystem]         ->setText( prep->getParam(TempoSystem), false);
 
}

void TempoViewController::bkMessageReceived(const String& message)
{
    if (message == "Tempo/update")
    {
        
        updateFields();
    }
}
