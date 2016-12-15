/*
  ==============================================================================

    LayerViewController.cpp
    Created: 8 Dec 2016 12:54:41am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "LayerViewController.h"

//==============================================================================
LayerViewController::LayerViewController(BKAudioProcessor& p, Layer::Ptr l):
processor(p),
current(l),
layerL(OwnedArray<BKLabel>()),
layerTF(OwnedArray<BKTextField>())
{
    // Labels
    layerL = OwnedArray<BKLabel>();
    layerL.ensureStorageAllocated(cLayerParameterTypes.size());
    
    for (int i = 0; i < cLayerParameterTypes.size(); i++)
    {
        layerL.set(i, new BKLabel());
        addAndMakeVisible(layerL[i]);
        layerL[i]->setName(cLayerParameterTypes[i]);
        layerL[i]->setText(cLayerParameterTypes[i], NotificationType::dontSendNotification);
    }
    
    // Text Fields
    layerTF = OwnedArray<BKTextField>();
    layerTF.ensureStorageAllocated(cLayerParameterTypes.size());
    
    for (int i = 0; i < cLayerParameterTypes.size()-2; i++)
    {
        layerTF.set(i, new BKTextField());
        addAndMakeVisible(layerTF[i]);
        layerTF[i]->addListener(this);
        layerTF[i]->setName(cLayerParameterTypes[i+2]);
    }
    
    
    for (int i = 0; i < cLayerCBType.size(); i++)
    {
        layerCB.set(i, new BKComboBox());
        layerCB[i]->setName(cLayerCBType[i]);
        layerCB[i]->addSeparator();
        layerCB[i]->addListener(this);
        addAndMakeVisible(layerCB[i]);
    }
    
    for (int i = 0; i < cPreparationTypes.size(); i++)
    {
        layerCB[LayerCBType]->addItem(cPreparationTypes[i], i+1);
    }
    
    for (int i = 0; i < aMaxNumLayers; i++)
    {
        layerCB[LayerCBNumber]->addItem(cLayerNumberName[i], i+1);
    }
    
    layerCB[LayerCBType]->setSelectedItemIndex(0);
    layerCB[LayerCBNumber]->setSelectedItemIndex(0);
    
    updateFields();
}

LayerViewController::~LayerViewController()
{
}

void LayerViewController::paint (Graphics& g)
{
    g.setColour(Colours::goldenrod);
    g.drawRect(getLocalBounds(), 1);
}

void LayerViewController::resized()
{
   
    
    // Labels
    int i = 0;
    int lX = 0;
    int lY = gComponentLabelHeight + gYSpacing;
    
    for (int n = 0; n < cLayerParameterTypes.size(); n++)
    {
        layerL[n]->setTopLeftPosition(lX, gYSpacing + lY * n);
    }
    
    // Text fields
    i = 0;
    int tfX = gComponentLabelWidth + gXSpacing;
    int tfY = gComponentTextFieldHeight + gYSpacing;
    
    for (int n = 0; n < cLayerCBType.size(); n++)
    {
        layerCB[n]->setTopLeftPosition(tfX, gYSpacing + tfY * n);
    }
    
    for (int n = 0; n < cLayerParameterTypes.size()-2; n++)
    {
        layerTF[n]->setTopLeftPosition(tfX, gYSpacing + tfY * (n+2));
    }
    
}

void LayerViewController::textFieldDidChange(TextEditor& tf)
{
    String text = tf.getText();
    String name = tf.getName();
    
    float f = text.getFloatValue();
    int i = text.getIntValue();
    
    DBG(name + ": |" + text + "|");

    if (name == cLayerParameterTypes[LayerKeymapId+2])
    {
        current->setKeymap(i);
        
        BKPreparationType type = current->getType();
        int layerNum = current->getLayerNumber();
        
        Keymap::Ptr km = processor.bkKeymaps[i];
        
        if (type == PreparationTypeSynchronic)
        {
            processor.sProcessor[layerNum]->setKeymap(km);
        }
        else if (type == PreparationTypeNostalgic)
        {
            processor.nProcessor[layerNum]->setKeymap(km);
        }
        else if (type == PreparationTypeDirect)
        {
            processor.dProcessor[layerNum]->setKeymap(km);
        }
        
        sendActionMessage("keymap/update");
        
    }
    else if (name == cLayerParameterTypes[LayerPreparationId+2])
    {
        current->setPreparation(i);
        
        BKPreparationType type = current->getType();
        int layerNum = current->getLayerNumber();
        
        if (type == PreparationTypeSynchronic)
        {
            SynchronicPreparation::Ptr prep = processor.sPreparation[i];
            processor.sProcessor[layerNum]->setPreparation(prep);
            sendActionMessage("synchronic/update");
        }
        else if (type == PreparationTypeNostalgic)
        {
            NostalgicPreparation::Ptr prep = processor.nPreparation[i];
            processor.nProcessor[layerNum]->setPreparation(prep);
            sendActionMessage("nostalgic/update");
        }
        else if (type == PreparationTypeDirect)
        {
            DirectPreparation::Ptr prep = processor.dPreparation[i];
            processor.dProcessor[layerNum]->setPreparation(prep);
            sendActionMessage("direct/update");
        }
    }
    else
    {
        DBG("Unregistered text field entered input.");
    }
}

void LayerViewController::switchToLayer(BKPreparationType type, int layer)
{
    TuningPreparation::Ptr tuning;
    
    if (type == PreparationTypeDirect)
    {
        DirectProcessor::Ptr proc = processor.dProcessor[layer];
        tuning = proc->getPreparation()->getTuning();
        current->setPreparation(proc->getPreparationId());
        current->setKeymap(proc->getKeymapId());
        sendActionMessage("direct/update");
    }
    else if (type == PreparationTypeSynchronic)
    {
        SynchronicProcessor::Ptr proc = processor.sProcessor[layer];
        tuning = proc->getPreparation()->getTuning();
        current->setPreparation(proc->getPreparationId());
        current->setKeymap(proc->getKeymapId());
        sendActionMessage("synchronic/update");
    }
    else if (type == PreparationTypeNostalgic)
    {
        NostalgicProcessor::Ptr proc = processor.nProcessor[layer];
        tuning = proc->getPreparation()->getTuning();
        current->setPreparation(proc->getPreparationId());
        current->setKeymap(proc->getKeymapId());
        sendActionMessage("nostalgic/update");
    }
    
    current->setTuning(tuning->getId());
    sendActionMessage("tuning/update");
}


void LayerViewController::updateFields(void)
{
    
    // Set text.
    layerTF[LayerKeymapId]        ->setText( String(current->getKeymap()));
    layerTF[LayerPreparationId]   ->setText( String(current->getPreparation()));
}

void LayerViewController::comboBoxChanged (ComboBox* box)
{
    if (box->getName() == "LayerType")
    {
        int which = box->getSelectedId() - 1;
        
        DBG(cPreparationTypes[which]);
        
        current->setType((BKPreparationType) which);
        current->setLayerNumber(0);
        
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
    else if (box->getName() == "LayerNumber")
    {
        int whichLayer = box->getSelectedId();
        
        DBG("which: "+String(whichLayer));
        
        current->setLayerNumber(whichLayer);
        
        BKPreparationType type = current->getType();
        
        String s = "";
        
        if (type == PreparationTypeSynchronic)
        {
            SynchronicProcessor::Ptr proc = processor.sProcessor[whichLayer];
            current->setPreparation(proc->getPreparationId());
            current->setKeymap(proc->getKeymapId());
            sendActionMessage("synchronic/update");
        }
        else if (type == PreparationTypeNostalgic)
        {
            NostalgicProcessor::Ptr proc = processor.nProcessor[whichLayer];
            current->setPreparation(proc->getPreparationId());
            current->setKeymap(proc->getKeymapId());
            sendActionMessage("nostalgic/update");
        }
        else if (type == PreparationTypeDirect)
        {
            DirectProcessor::Ptr proc = processor.dProcessor[whichLayer];
            current->setPreparation(proc->getPreparationId());
            current->setKeymap(proc->getKeymapId());
            sendActionMessage("direct/update");
        }
        
        sendActionMessage("keymap/update");
        updateFields();
    }
}


