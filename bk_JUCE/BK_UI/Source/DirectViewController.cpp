/*
  ==============================================================================

    DirectViewController.cpp
    Created: 7 Jun 2017 1:42:55pm
    Author:  Daniel Trueman

  ==============================================================================
*/

#include "DirectViewController.h"

DirectViewController::DirectViewController(BKAudioProcessor& p, BKItemGraph* theGraph):
BKViewController(p, theGraph)
{
    selectCB.setName("Direct");
    selectCB.addSeparator();
    selectCB.setSelectedItemIndex(0);
    addAndMakeVisible(selectCB);
    
    transpositionSlider = new BKStackedSlider("transpositions", -12, 12, -12, 12, 0, 0.01);
    addAndMakeVisible(transpositionSlider);

    gainSlider = new BKSingleSlider("gain", 0, 10, 1, 0.01);
    gainSlider->setSkewFactorFromMidPoint(1.);
    addAndMakeVisible(gainSlider);
    
    resonanceGainSlider = new BKSingleSlider("resonance gain", 0, 10, 1, 0.01);
    resonanceGainSlider->setSkewFactorFromMidPoint(1.);
    addAndMakeVisible(resonanceGainSlider);
    
    hammerGainSlider = new BKSingleSlider("hammer gain", 0, 10, 1, 0.01);
    hammerGainSlider->setSkewFactorFromMidPoint(1.);
    addAndMakeVisible(hammerGainSlider);
    
}

void DirectViewController::paint (Graphics& g)
{
    g.fillAll(Colours::transparentWhite);
}

void DirectViewController::resized()
{
    Rectangle<int> area (getLocalBounds());
    
    Rectangle<int> displayRow = area.removeFromBottom(area.getHeight() * 0.5);
    
    Rectangle<int> leftColumn = area.removeFromLeft(area.getWidth() * 0.5);
    selectCB.setBounds(leftColumn.removeFromTop(20));
    transpositionSlider->setBounds(leftColumn.removeFromTop(40));
    
    resonanceGainSlider->setBounds(area.removeFromTop(40));
    hammerGainSlider->setBounds(area.removeFromTop(40));
    gainSlider->setBounds(area.removeFromBottom(40));
    
}

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ DirectPreparationEditor ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~//
DirectPreparationEditor::DirectPreparationEditor(BKAudioProcessor& p, BKItemGraph* theGraph):
DirectViewController(p, theGraph)
{
    selectCB.addMyListener(this);
    
    fillSelectCB();
    
    transpositionSlider->addMyListener(this);
    
    gainSlider->addMyListener(this);
    
    resonanceGainSlider->addMyListener(this);
    
    hammerGainSlider->addMyListener(this);
}

void DirectPreparationEditor::update(void)
{
    
    if (processor.updateState->currentDirectId < 0) return;
    
    fillSelectCB();
    
    DirectPreparation::Ptr prep = processor.gallery->getActiveDirectPreparation(processor.updateState->currentDirectId);

    selectCB.setSelectedItemIndex(processor.updateState->currentDirectId, dontSendNotification);
    
    transpositionSlider->setValue(prep->getTransposition(), dontSendNotification);
    resonanceGainSlider->setValue(prep->getResonanceGain(), dontSendNotification);
    hammerGainSlider->setValue(prep->getHammerGain(), dontSendNotification);
    gainSlider->setValue(prep->getGain(), dontSendNotification);
    
}

void DirectPreparationEditor::bkMessageReceived (const String& message)
{
    if (message == "direct/update")
    {
        
        update();
    }
}

void DirectPreparationEditor::bkComboBoxDidChange (ComboBox* box)
{
    String name = box->getName();
    
    if (name == "Direct")
    {
        processor.updateState->currentDirectId = box->getSelectedItemIndex();
        
        processor.updateState->idDidChange = true;
        
        if (processor.updateState->currentDirectId == selectCB.getNumItems()-1)
        {
            processor.gallery->addDirect();
            
            fillSelectCB();
        }
        
        update();
    }
}

void DirectPreparationEditor::BKEditableComboBoxChanged(String name, BKEditableComboBox* cb)
{
    processor.gallery->getDirect(processor.updateState->currentDirectId)->setName(name);
}


void DirectPreparationEditor::BKSingleSliderValueChanged(String name, double val)
{
    DirectPreparation::Ptr prep = processor.gallery->getStaticDirectPreparation(processor.updateState->currentDirectId);
    DirectPreparation::Ptr active = processor.gallery->getActiveDirectPreparation(processor.updateState->currentDirectId);
    
    if(name == "resonance gain")
    {
        //DBG("note length multiplier " + String(val));
        prep->setResonanceGain(val);
        active->setResonanceGain(val);
    }
    else if(name == "hammer gain")
    {
        //DBG("beats to skip " + String(val));
        prep->setHammerGain(val);
        active->setHammerGain(val);
    }
    else if(name == "gain")
    {
        //DBG("gain " + String(val));
        prep->setGain(val);
        active->setGain(val);
    }
}

void DirectPreparationEditor::BKStackedSliderValueChanged(String name, Array<float> val)
{
    DirectPreparation::Ptr prep = processor.gallery->getStaticDirectPreparation(processor.updateState->currentDirectId);
    DirectPreparation::Ptr active = processor.gallery->getActiveDirectPreparation(processor.updateState->currentDirectId);
    
    prep->setTransposition(val);
    active->setTransposition(val);
}

void DirectPreparationEditor::fillSelectCB(void)
{
    // Direct menu
    Direct::PtrArr newpreps = processor.gallery->getAllDirect();
    
    selectCB.clear(dontSendNotification);
    for (int i = 0; i < newpreps.size(); i++)
    {
        String name = newpreps[i]->getName();
        if (name != String::empty)  selectCB.addItem(name, i+1);
        else                        selectCB.addItem(String(i+1), i+1);
    }
    
    selectCB.addItem("New direct...", newpreps.size()+1);
    
    selectCB.setSelectedItemIndex(processor.updateState->currentDirectId, NotificationType::dontSendNotification);
    
}

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ DirectModificationEditor ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~//
DirectModificationEditor::DirectModificationEditor(BKAudioProcessor& p, BKItemGraph* theGraph):
DirectViewController(p, theGraph)
{
    selectCB.addMyListener(this);
    selectCB.addListener(this);
    
    fillSelectCB();
    
    transpositionSlider->addMyListener(this);
    
    gainSlider->addMyListener(this);
    
    resonanceGainSlider->addMyListener(this);
    
    hammerGainSlider->addMyListener(this);
}

void DirectModificationEditor::update(void)
{
    
    if (processor.updateState->currentDirectId < 0) return;
    
    fillSelectCB();
    
    DirectPreparation::Ptr prep = processor.gallery->getActiveDirectPreparation(processor.updateState->currentDirectId);
    
    selectCB.setSelectedItemIndex(processor.updateState->currentDirectId, dontSendNotification);
    
    transpositionSlider->setValue(prep->getTransposition(), dontSendNotification);
    resonanceGainSlider->setValue(prep->getResonanceGain(), dontSendNotification);
    hammerGainSlider->setValue(prep->getHammerGain(), dontSendNotification);
    gainSlider->setValue(prep->getGain(), dontSendNotification);
    
}

void DirectModificationEditor::bkMessageReceived (const String& message)
{
    if (message == "direct/update")
    {
        
        update();
    }
}

void DirectModificationEditor::bkComboBoxDidChange (ComboBox* box)
{
    String name = box->getName();
    
    if (name == "Direct")
    {
        processor.updateState->currentDirectId = box->getSelectedItemIndex();
        
        processor.updateState->idDidChange = true;
        
        if (processor.updateState->currentDirectId == selectCB.getNumItems()-1)
        {
            processor.gallery->addDirect();
            
            fillSelectCB();
        }
        
        update();
    }
}

void DirectModificationEditor::BKEditableComboBoxChanged(String name, BKEditableComboBox* cb)
{
    processor.gallery->getDirect(processor.updateState->currentDirectId)->setName(name);
}


void DirectModificationEditor::BKSingleSliderValueChanged(String name, double val)
{
    DirectPreparation::Ptr prep = processor.gallery->getStaticDirectPreparation(processor.updateState->currentDirectId);
    DirectPreparation::Ptr active = processor.gallery->getActiveDirectPreparation(processor.updateState->currentDirectId);
    
    if(name == "resonance gain")
    {
        //DBG("note length multiplier " + String(val));
        prep->setResonanceGain(val);
        active->setResonanceGain(val);
    }
    else if(name == "hammer gain")
    {
        //DBG("beats to skip " + String(val));
        prep->setHammerGain(val);
        active->setHammerGain(val);
    }
    else if(name == "gain")
    {
        //DBG("gain " + String(val));
        prep->setGain(val);
        active->setGain(val);
    }
}

void DirectModificationEditor::BKStackedSliderValueChanged(String name, Array<float> val)
{
    DirectPreparation::Ptr prep = processor.gallery->getStaticDirectPreparation(processor.updateState->currentDirectId);
    DirectPreparation::Ptr active = processor.gallery->getActiveDirectPreparation(processor.updateState->currentDirectId);
    
    prep->setTransposition(val);
    active->setTransposition(val);
}

void DirectModificationEditor::fillSelectCB(void)
{
    // Direct menu
    Direct::PtrArr newpreps = processor.gallery->getAllDirect();
    
    selectCB.clear(dontSendNotification);
    for (int i = 0; i < newpreps.size(); i++)
    {
        String name = newpreps[i]->getName();
        if (name != String::empty)  selectCB.addItem(name, i+1);
        else                        selectCB.addItem(String(i+1), i+1);
    }
    
    selectCB.addItem("New direct...", newpreps.size()+1);
    
    selectCB.setSelectedItemIndex(processor.updateState->currentDirectId, NotificationType::dontSendNotification);
    
}
