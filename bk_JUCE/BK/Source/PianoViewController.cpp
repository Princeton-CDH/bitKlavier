/*
  ==============================================================================

    PianoViewController.cpp
    Created: 8 Dec 2016 12:54:41am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "PianoViewController.h"

//==============================================================================
PianoViewController::PianoViewController(BKAudioProcessor& p, Piano::Ptr l):
processor(p),
current(l),
pianoL(OwnedArray<BKLabel>()),
pianoTF(OwnedArray<BKTextField>())
{
    // Labels
    pianoL = OwnedArray<BKLabel>();
    pianoL.ensureStorageAllocated(cPianoParameterTypes.size());
    
    for (int i = 0; i < cPianoParameterTypes.size(); i++)
    {
        pianoL.set(i, new BKLabel());
        addAndMakeVisible(pianoL[i]);
        pianoL[i]->setName(cPianoParameterTypes[i]);
        pianoL[i]->setText(cPianoParameterTypes[i], NotificationType::dontSendNotification);
    }
    
    // Text Fields
    pianoTF = OwnedArray<BKTextField>();
    pianoTF.ensureStorageAllocated(cPianoParameterTypes.size());
    
    for (int i = 0; i < cPianoParameterTypes.size()-2; i++)
    {
        pianoTF.set(i, new BKTextField());
        addAndMakeVisible(pianoTF[i]);
        pianoTF[i]->addListener(this);
        pianoTF[i]->setName(cPianoParameterTypes[i+2]);
    }
    
    
    for (int i = 0; i < cPianoCBType.size(); i++)
    {
        pianoCB.set(i, new BKComboBox());
        pianoCB[i]->setName(cPianoCBType[i]);
        pianoCB[i]->addSeparator();
        pianoCB[i]->addListener(this);
        addAndMakeVisible(pianoCB[i]);
    }
    
    for (int i = 0; i < cPreparationTypes.size(); i++)
    {
        pianoCB[PianoCBType]->addItem(cPreparationTypes[i], i+1);
    }
    
    for (int i = 0; i < aMaxNumPianos; i++)
    {
        pianoCB[PianoCBNumber]->addItem(cPianoNumberName[i], i+1);
    }
    
    pianoCB[PianoCBType]->setSelectedItemIndex(0);
    pianoCB[PianoCBNumber]->setSelectedItemIndex(0);
    
    updateFields();
}

PianoViewController::~PianoViewController()
{
}

void PianoViewController::paint (Graphics& g)
{
    g.setColour(Colours::goldenrod);
    g.drawRect(getLocalBounds(), 1);
}

void PianoViewController::resized()
{
   
    
    // Labels
    int i = 0;
    int lX = 0;
    int lY = gComponentLabelHeight + gYSpacing;
    
    for (int n = 0; n < cPianoParameterTypes.size(); n++)
    {
        pianoL[n]->setTopLeftPosition(lX, gYSpacing + lY * n);
    }
    
    // Text fields
    i = 0;
    int tfX = gComponentLabelWidth + gXSpacing;
    int tfY = gComponentTextFieldHeight + gYSpacing;
    
    for (int n = 0; n < cPianoCBType.size(); n++)
    {
        pianoCB[n]->setTopLeftPosition(tfX, gYSpacing + tfY * n);
    }
    
    for (int n = 0; n < cPianoParameterTypes.size()-2; n++)
    {
        pianoTF[n]->setTopLeftPosition(tfX, gYSpacing + tfY * (n+2));
    }
    
}

void PianoViewController::textFieldDidChange(TextEditor& tf)
{
    String text = tf.getText();
    String name = tf.getName();
    
    float f = text.getFloatValue();
    int i = text.getIntValue();
    
    DBG(name + ": |" + text + "|");

    if (name == cPianoParameterTypes[PianoKeymapId+2])
    {
        current->setKeymap(i);
        
        BKPreparationType type = current->getType();
        int pianoNum = current->getPianoNumber();
        
        Keymap::Ptr km = processor.bkKeymaps[i];
        
        if (type == PreparationTypeSynchronic)
        {
            processor.sProcessor[pianoNum]->setKeymap(km);
        }
        else if (type == PreparationTypeNostalgic)
        {
            processor.nProcessor[pianoNum]->setKeymap(km);
        }
        else if (type == PreparationTypeDirect)
        {
            processor.dProcessor[pianoNum]->setKeymap(km);
        }
        
        sendActionMessage("keymap/update");
        
    }
    else if (name == cPianoParameterTypes[PianoPreparationId+2])
    {
        current->setPreparation(i);
        
        BKPreparationType type = current->getType();
        int pianoNum = current->getPianoNumber();
        
        if (type == PreparationTypeSynchronic)
        {
            SynchronicPreparation::Ptr prep = processor.sPreparation[i];
            processor.sProcessor[pianoNum]->setPreparation(prep);
            sendActionMessage("synchronic/update");
        }
        else if (type == PreparationTypeNostalgic)
        {
            NostalgicPreparation::Ptr prep = processor.nPreparation[i];
            processor.nProcessor[pianoNum]->setPreparation(prep);
            sendActionMessage("nostalgic/update");
        }
        else if (type == PreparationTypeDirect)
        {
            DirectPreparation::Ptr prep = processor.dPreparation[i];
            processor.dProcessor[pianoNum]->setPreparation(prep);
            sendActionMessage("direct/update");
        }
    }
    else
    {
        DBG("Unregistered text field entered input.");
    }
}

void PianoViewController::switchToPiano(BKPreparationType type, int piano)
{
    TuningPreparation::Ptr tuning;
    
    if (type == PreparationTypeDirect)
    {
        DirectProcessor::Ptr proc = processor.dProcessor[piano];
        tuning = proc->getPreparation()->getTuning();
        current->setPreparation(proc->getPreparationId());
        current->setKeymap(proc->getKeymapId());
        sendActionMessage("direct/update");
    }
    else if (type == PreparationTypeSynchronic)
    {
        SynchronicProcessor::Ptr proc = processor.sProcessor[piano];
        tuning = proc->getPreparation()->getTuning();
        current->setPreparation(proc->getPreparationId());
        current->setKeymap(proc->getKeymapId());
        sendActionMessage("synchronic/update");
    }
    else if (type == PreparationTypeNostalgic)
    {
        NostalgicProcessor::Ptr proc = processor.nProcessor[piano];
        tuning = proc->getPreparation()->getTuning();
        current->setPreparation(proc->getPreparationId());
        current->setKeymap(proc->getKeymapId());
        sendActionMessage("nostalgic/update");
    }
    
    current->setTuning(tuning->getId());
    sendActionMessage("tuning/update");
}


void PianoViewController::updateFields(void)
{
    
    // Set text.
    pianoTF[PianoKeymapId]        ->setText( String(current->getKeymap()));
    pianoTF[PianoPreparationId]   ->setText( String(current->getPreparation()));
}

void PianoViewController::comboBoxChanged (ComboBox* box)
{
    if (box->getName() == "PianoType")
    {
        int which = box->getSelectedId() - 1;
        
        DBG(cPreparationTypes[which]);
        
        current->setType((BKPreparationType) which);
        current->setPianoNumber(0);
        
        if (which == PreparationTypeDirect)
        {
            DirectProcessor::Ptr proc = processor.dProcessor[0];
            current->setPreparation(proc->getPreparationId());
            current->setKeymap(proc->getKeymapId());
            sendActionMessage("direct/update");
        }
        else if (which == PreparationTypeSynchronic)
        {
            SynchronicProcessor::Ptr proc = processor.sProcessor[0];
            current->setPreparation(proc->getPreparationId());
            current->setKeymap(proc->getKeymapId());
            sendActionMessage("synchronic/update");
        }
        else if (which == PreparationTypeNostalgic)
        {
            NostalgicProcessor::Ptr proc = processor.nProcessor[0];
            current->setPreparation(proc->getPreparationId());
            current->setKeymap(proc->getKeymapId());
            sendActionMessage("nostalgic/update");
        }

        updateFields();
    }
    else if (box->getName() == "PianoNumber")
    {
        int whichPiano = box->getSelectedId();
        
        DBG("which: "+String(whichPiano));
        
        current->setPianoNumber(whichPiano);
        
        BKPreparationType type = current->getType();
        
        String s = "";
        
        if (type == PreparationTypeSynchronic)
        {
            SynchronicProcessor::Ptr proc = processor.sProcessor[whichPiano];
            current->setPreparation(proc->getPreparationId());
            current->setKeymap(proc->getKeymapId());
            sendActionMessage("synchronic/update");
        }
        else if (type == PreparationTypeNostalgic)
        {
            NostalgicProcessor::Ptr proc = processor.nProcessor[whichPiano];
            current->setPreparation(proc->getPreparationId());
            current->setKeymap(proc->getKeymapId());
            sendActionMessage("nostalgic/update");
        }
        else if (type == PreparationTypeDirect)
        {
            DirectProcessor::Ptr proc = processor.dProcessor[whichPiano];
            current->setPreparation(proc->getPreparationId());
            current->setKeymap(proc->getKeymapId());
            sendActionMessage("direct/update");
        }
        
        sendActionMessage("keymap/update");
        updateFields();
    }
}

