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
    
    for (int i = 0; i < cLayerParameterTypes.size(); i++)
    {
        layerTF.set(i, new BKTextField());
        addAndMakeVisible(layerTF[i]);
        layerTF[i]->addListener(this);
        layerTF[i]->setName(cLayerParameterTypes[i]);
    }
    
    
    typeComboBox.setName("CBType");
    for (int i = 0; i < cPreparationTypes.size(); i++)
    {
        typeComboBox.addItem(cPreparationTypes[i], i+1);
    }
    typeComboBox.addSeparator();
    typeComboBox.addListener(this);
    addAndMakeVisible(typeComboBox);
    typeComboBox.setSelectedItemIndex(0);
    
    
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
    
    typeComboBox.setTopLeftPosition(tfX, gYSpacing);
    
    for (int n = 0; n < cLayerParameterTypes.size(); n++)
    {
        layerTF[n]->setTopLeftPosition(tfX, gYSpacing + tfY * (n+1));
    }
    
}

void LayerViewController::textFieldDidChange(TextEditor& tf)
{
    String text = tf.getText();
    String name = tf.getName();
    
    float f = text.getFloatValue();
    int i = text.getIntValue();
    
    DBG(name + ": |" + text + "|");

    if (name == cLayerParameterTypes[LayerNumber])
    {
        current->setLayerNumber(i);
    
        BKPreparationType type = current->getType();
        
        String s = "";
        
        if (type == PreparationTypeSynchronic)
        {
            s = "synchronic/update";
        }
        else if (type == PreparationTypeNostalgic)
        {
            s = "nostalgic/update";
        }
        else if (type == PreparationTypeDirect)
        {
            s = "direct/update";
        }
        else if (type == PreparationTypeTuning)
        {
            s = "tuning/update";
        }
        
        sendActionMessage(s);
    }
    else if (name == cLayerParameterTypes[LayerKeymapId])
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
        else //PreparationTypeTuning
        {
            
        }
        
        sendActionMessage("keymap/update");
        
    }
    else if (name == cLayerParameterTypes[LayerPreparationId])
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
        else //PreparationTypeTuning
        {
            
        }
    }
    else if (name == cLayerParameterTypes[LayerTuningId])
    {
        current->setTuning(i);
        
        BKPreparationType type = current->getType();
        int layerNum = current->getLayerNumber();
        
        //TuningProcessor::Ptr tune = processor.bkTunings[i];
        
        if (type == PreparationTypeSynchronic)
        {
            //processor.sProcessor[layerNum]->setTuning(tune);
        }
        else if (type == PreparationTypeNostalgic)
        {
            //processor.nProcessor[layerNum]->setTuning(tune);
        }
        else if (type == PreparationTypeDirect)
        {
            //processor.dProcessor[layerNum]->setTuning(tune);
        }
        else //PreparationTypeTuning
        {
            sendActionMessage("tuning/update");
        }
    }
    else
    {
        DBG("Unregistered text field entered input.");
    }
}


void LayerViewController::updateFields(void)
{
    
    // Set text.
    //layerTF[LayerType]          ->setText( String(current->getType()));
    layerTF[LayerNumber]        ->setText( String(current->getLayerNumber()));
    layerTF[LayerKeymapId]        ->setText( String(current->getKeymap()));
    layerTF[LayerPreparationId]   ->setText( String(current->getPreparation()));
    layerTF[LayerTuningId]        ->setText( String(current->getTuning()));
    
}

void LayerViewController::comboBoxChanged (ComboBox* box)
{
    if (box->getName() == "CBType")
    {
        int which = box->getSelectedId() - 1;
        
        DBG(cPreparationTypes[which]);
        
        if (which == PreparationTypeDirect)
        {
            
        }
        else if (which == PreparationTypeSynchronic)
        {
            
        }
        else if (which == PreparationTypeNostalgic)
        {
            
        }
        else if (which == PreparationTypeTuning)
        {
            
        }
        
        current->setType((BKPreparationType) which);
    }
}


