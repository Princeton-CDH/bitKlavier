/*
  ==============================================================================

    DirectViewController.cpp
    Created: 30 Nov 2016 5:00:19pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "DirectViewController.h"

#include "Preparation.h"

//==============================================================================
DirectViewController::DirectViewController(BKAudioProcessor& p):
processor(p)
{
    
    // First row
    addAndMakeVisible(selectL);
    selectL.setName("Direct");
    selectL.setText("Direct", NotificationType::dontSendNotification);
    
    addAndMakeVisible(selectCB);
    selectCB.setName("Direct");
    selectCB.addSeparator();
    selectCB.addListener(this);
    //selectCB.setEditableText(true);
    selectCB.setSelectedItemIndex(0);
    
    addAndMakeVisible(modSelectCB);
    modSelectCB.setName("DirectMod");
    modSelectCB.addSeparator();
    modSelectCB.addListener(this);
    //selectCB.setEditableText(true);
    modSelectCB.setSelectedItemIndex(0);
    

    addAndMakeVisible(nameL);
    nameL.setName("Name");
    nameL.setText("Name", NotificationType::dontSendNotification);
    
    addAndMakeVisible(nameTF);
    nameTF.addListener(this);
    nameTF.setName("Name");
    
    addAndMakeVisible(modNameTF);
    modNameTF.addListener(this);
    modNameTF.setName("ModName");

    DirectPreparation::Ptr layer = processor.gallery->getStaticDirectPreparation(processor.updateState->currentDirectId);
    
    // Labels
    directL = OwnedArray<BKLabel>();
    directL.ensureStorageAllocated(cDirectParameterTypes.size());
    
    for (int i = 0; i < cDirectParameterTypes.size(); i++)
    {
        directL.add(new BKLabel());
        addAndMakeVisible(directL[i]);
        directL[i]->setName(cDirectParameterTypes[i]);
        directL[i]->setText(cDirectParameterTypes[i], NotificationType::dontSendNotification);
    }
    
    // Text Fields
    directTF = OwnedArray<BKTextField>();
    directTF.ensureStorageAllocated(cDirectParameterTypes.size());
    
    for (int i = 0; i < cDirectParameterTypes.size(); i++)
    {
        directTF.add(new BKTextField());
        addAndMakeVisible(directTF[i]);
        directTF[i]->addListener(this);
        directTF[i]->setName(cDirectParameterTypes[i]);
    }
    
    modDirectTF = OwnedArray<BKTextField>();
    modDirectTF.ensureStorageAllocated(cDirectParameterTypes.size());
    
    for (int i = 0; i < cDirectParameterTypes.size(); i++)
    {
        modDirectTF.set(i, new BKTextField());
        addAndMakeVisible(modDirectTF[i]);
        modDirectTF[i]->addListener(this);
        modDirectTF[i]->setName("M"+cDirectParameterTypes[i]);
    }
    
    
    directTF[0]->setVisible(false);directL[0]->setVisible(false);modDirectTF[0]->setVisible(0);
    
    fillSelectCB();     fillModSelectCB();
    
    updateModFields();  updateFields();
}

void DirectViewController::reset(void)
{
    processor.updateState->currentDirectId = 0;
    fillSelectCB(); updateFields();
    
    processor.updateState->currentModDirectId = 0;
    fillModSelectCB(); updateModFields();
}

DirectViewController::~DirectViewController()
{
}

void DirectViewController::fillSelectCB(void)
{
    // Direct menu
    Direct::PtrArr direct = processor.gallery->getAllDirect();
    
    selectCB.clear(dontSendNotification);
    for (int i = 0; i < direct.size(); i++)
    {
        String name = direct[i]->getName();
        if (name != String::empty)  selectCB.addItem(name, i+1);
        else                        selectCB.addItem(String(i+1), i+1);
    }
    
    selectCB.addItem("New direct...", direct.size()+1);
    
    selectCB.setSelectedItemIndex(processor.updateState->currentDirectId, NotificationType::dontSendNotification);
    
}

void DirectViewController::fillModSelectCB(void)
{
    // ModDirect menu
    DirectModPreparation::PtrArr modDirect = processor.gallery->getDirectModPreparations();
    
    modSelectCB.clear(dontSendNotification);
    for (int i = 0; i < modDirect.size(); i++)
    {
        String name = modDirect[i]->getName();
        if (name != String::empty)  modSelectCB.addItem(name, i+1);
        else                        modSelectCB.addItem(String(i+1), i+1);
    }
    
    modSelectCB.addItem("New modification...", modDirect.size()+1);
    
    modSelectCB.setSelectedItemIndex(processor.updateState->currentModDirectId, NotificationType::dontSendNotification);
}



void DirectViewController::bkComboBoxDidChange        (ComboBox* box)
{
    String name = box->getName();
    
    if (name == "Direct")
    {
        processor.updateState->currentDirectId = box->getSelectedItemIndex();
        
        if (processor.updateState->currentDirectId == selectCB.getNumItems()-1) // New Direct
        {
            processor.gallery->addDirect();
            
            fillSelectCB();
        }
        
        updateFields();
    }
    else if (name == "DirectMod")
    {
        processor.updateState->currentModDirectId = box->getSelectedItemIndex();
        
        if (processor.updateState->currentModDirectId == modSelectCB.getNumItems()-1) // New Mod
        {
            processor.gallery->addDirectMod();
            
            fillModSelectCB();
        }
        
        updateModFields();
    }
}

void DirectViewController::paint (Graphics& g)
{
    g.setColour(Colours::goldenrod);
    g.drawRect(getLocalBounds(), 1);
}

void DirectViewController::resized()
{
    
    float widthL = getWidth() * 0.25 - gXSpacing;
    float heightL = directL[0]->getHeight();
    float widthTF = widthL * 1.5;
    float heightTF = directTF[0]->getHeight();

    // Menus
    selectL.setBounds(0, gYSpacing, widthL, heightL);
    selectCB.setBounds(selectL.getRight()+gXSpacing, selectL.getY(), widthTF, heightTF);
    modSelectCB.setBounds(selectCB.getRight()+gXSpacing, selectL.getY(), widthTF, heightTF);
    
    // Names
    nameL.setBounds(0, selectL.getBottom()+gYSpacing, widthL, heightL);
    nameTF.setBounds(nameL.getRight()+gXSpacing, nameL.getY(), widthTF, heightTF);
    modNameTF.setBounds(nameTF.getRight()+gXSpacing, nameTF.getY(), widthTF, heightTF);
    
    // Labels
    directL[1]->setBounds(0, nameL.getBottom()+gYSpacing, widthL, heightL);
    
    for (int n = 2; n < cDirectParameterTypes.size(); n++)
    {
        directL[n]->setBounds(0, directL[n-1]->getBottom()+gYSpacing, widthL, heightL);
    }
    
    // Text fields
    directTF[1]->setBounds(directL[1]->getRight()+gXSpacing, directL[1]->getY(), widthTF, heightTF);
    modDirectTF[1]->setBounds(directTF[1]->getRight()+gXSpacing, directL[1]->getY(), widthTF, heightTF);
    
    for (int n = 2; n < cDirectParameterTypes.size(); n++)
    {
        directTF[n]->setBounds(directTF[1]->getX(), directTF[n-1]->getBottom()+gYSpacing, widthTF, heightTF);
        modDirectTF[n]->setBounds(modDirectTF[1]->getX(), modDirectTF[n-1]->getBottom()+gYSpacing, widthTF, heightTF);
    }
}

void DirectViewController::bkTextFieldDidChange(TextEditor& tf)
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
    
    DirectPreparation::Ptr prep = processor.gallery->getStaticDirectPreparation(processor.updateState->currentDirectId);
    DirectPreparation::Ptr active = processor.gallery->getActiveDirectPreparation(processor.updateState->currentDirectId);
    DirectModPreparation::Ptr mod = processor.gallery->getDirectModPreparation(processor.updateState->currentModDirectId);
    
    if (name == "Name")
    {
        processor.gallery->getDirect(processor.updateState->currentDirectId)->setName(text);
        
        int selected = selectCB.getSelectedId();
        if (selected != selectCB.getNumItems()) selectCB.changeItemText(selected, text);
        selectCB.setSelectedId(selected, dontSendNotification );
    }
    else if (name == "ModName")
    {
        processor.gallery->getDirectModPreparation(processor.updateState->currentModDirectId)->setName(text);
        
        int selected = modSelectCB.getSelectedId();
        if (selected != modSelectCB.getNumItems()) modSelectCB.changeItemText(selected, text);
        modSelectCB.setSelectedId(selected, dontSendNotification );
    }
    else if (name == cDirectParameterTypes[DirectTransposition])
    {
        if (type == BKParameter)
        {
            Array<float> transp = stringToFloatArray(text);
            prep    ->setTransposition(transp);
            active  ->setTransposition(transp);
        }
        else    //BKModification
        {
            mod     ->setParam(DirectTransposition, text);
        }
    }
    else if (name == cDirectParameterTypes[DirectGain])
    {
        if (type == BKParameter)
        {
            prep    ->setGain(f);
            active  ->setGain(f);
        }
        else    //BKModification
        {
            mod     ->setParam(DirectGain, text);
        }
    }
    else if (name == cDirectParameterTypes[DirectHammerGain])
    {
        if (type == BKParameter)
        {
            prep    ->setHammerGain(f);
            active  ->setHammerGain(f);
        }
        else    //BKModification
        {
            mod     ->setParam(DirectHammerGain, text);
        }
    }
    else if (name == cDirectParameterTypes[DirectResGain])
    {
        if (type == BKParameter)
        {
            prep    ->setResonanceGain(f);
            active  ->setResonanceGain(f);
        }
        else    //BKModification
        {
            mod     ->setParam(DirectResGain, text);
        }
    }
    else if (name == cDirectParameterTypes[DirectTuning])
    {
        if (type == BKParameter)
        {
            if (i < processor.gallery->getNumTuning())
            {
                processor.gallery->getDirect(processor.updateState->currentDirectId)->setTuning(processor.gallery->getTuning(i));
            }
            else
                tf.setText("0", false);
            
            
        }
        else    //BKModification
        {
            if (i < processor.gallery->getNumTuning())
                mod     ->setParam(DirectTuning, text);
            else
                tf.setText("0", false);
        }
    }
    else
    {
        DBG("Unregistered text field entered input.");
    }
}


void DirectViewController::updateFields(void)
{
    Direct::Ptr dir = processor.gallery->getDirect(processor.updateState->currentDirectId);
    DirectPreparation::Ptr prep = processor.gallery->getActiveDirectPreparation(processor.updateState->currentDirectId);
    
    selectCB.setSelectedItemIndex(processor.updateState->currentDirectId, dontSendNotification);
    
    nameTF.setText(dir->getName(), dontSendNotification);
    
    directTF[DirectTransposition]       ->setText( floatArrayToString( prep->getTransposition()), false);
    directTF[DirectGain]                ->setText( String( prep->getGain()), false);
    directTF[DirectHammerGain]          ->setText( String( prep->getHammerGain()), false);
    directTF[DirectResGain]             ->setText( String( prep->getResonanceGain()), false);
    directTF[DirectTuning]              ->setText( String( prep->getTuning()->getId()), false);

}

void DirectViewController::updateModFields(void)
{
    DirectModPreparation::Ptr prep = processor.gallery->getDirectModPreparation(processor.updateState->currentModDirectId);
    
    modDirectTF[DirectId] -> setText(String(processor.updateState->currentModDirectId), false);
    
    modDirectTF[DirectTransposition]       ->setText( prep->getParam(DirectTransposition), false);
    modDirectTF[DirectGain]                ->setText( prep->getParam(DirectGain), false);
    modDirectTF[DirectHammerGain]          ->setText( prep->getParam(DirectHammerGain), false);
    modDirectTF[DirectResGain]             ->setText( prep->getParam(DirectResGain), false);
    modDirectTF[DirectTuning]              ->setText( prep->getParam(DirectTuning), false);
    
}

void DirectViewController::bkMessageReceived (const String& message)
{
    if (message == "direct/update")
    {
        
        updateFields();
    }
}


