/*
  ==============================================================================

    DirectViewController2.cpp
    Created: 7 Jun 2017 1:42:55pm
    Author:  Daniel Trueman

  ==============================================================================
*/

#include "DirectViewController2.h"

DirectViewController2::DirectViewController2(BKAudioProcessor& p, BKItemGraph* theGraph):
processor(p),
theGraph(theGraph)
{
    selectCB.setName("Direct");
    selectCB.addSeparator();
    selectCB.addListener(this);
    selectCB.setSelectedItemIndex(0);
    selectCB.addMyListener(this);
    fillSelectCB();
    addAndMakeVisible(selectCB);
    
    transpositionSlider = new BKStackedSlider("transpositions", -12, 12, -12, 12, 0, 0.01);
    transpositionSlider->addMyListener(this);
    addAndMakeVisible(transpositionSlider);

    gainSlider = new BKSingleSlider("gain", 0, 10, 1, 0.01);
    gainSlider->setSkewFactorFromMidPoint(1.);
    gainSlider->addMyListener(this);
    addAndMakeVisible(gainSlider);
    
    resonanceGainSlider = new BKSingleSlider("resonance gain", 0, 10, 1, 0.01);
    resonanceGainSlider->setSkewFactorFromMidPoint(1.);
    resonanceGainSlider->addMyListener(this);
    addAndMakeVisible(resonanceGainSlider);
    
    hammerGainSlider = new BKSingleSlider("hammer gain", 0, 10, 1, 0.01);
    hammerGainSlider->setSkewFactorFromMidPoint(1.);
    hammerGainSlider->addMyListener(this);
    addAndMakeVisible(hammerGainSlider);
    
}

void DirectViewController2::paint (Graphics& g)
{
    g.setColour(Colours::goldenrod);
    g.drawRect(getLocalBounds(), 1);
}

void DirectViewController2::resized()
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

void DirectViewController2::updateFields(void)
{
    
    DirectPreparation::Ptr prep = processor.gallery->getActiveDirectPreparation(processor.updateState->currentDirectId);

    selectCB.setSelectedItemIndex(processor.updateState->currentDirectId, dontSendNotification);
    
    transpositionSlider->setValue(prep->getTransposition(), dontSendNotification);
    resonanceGainSlider->setValue(prep->getResonanceGain(), dontSendNotification);
    hammerGainSlider->setValue(prep->getHammerGain(), dontSendNotification);
    gainSlider->setValue(prep->getGain(), dontSendNotification);
    
}

void DirectViewController2::bkMessageReceived (const String& message)
{
    if (message == "direct/update")
    {
        
        updateFields();
    }
}

void DirectViewController2::bkComboBoxDidChange (ComboBox* box)
{
    String name = box->getName();
    
    if (name == "Direct")
    {
        processor.updateState->currentDirectId = box->getSelectedItemIndex();
        
        if (processor.updateState->currentDirectId == selectCB.getNumItems()-1)
        {
            processor.gallery->addDirect();
            
            fillSelectCB();
        }
        
        //updateFields(sendNotification);
        updateFields();
    }
}

void DirectViewController2::BKEditableComboBoxChanged(String name, BKEditableComboBox* cb)
{
    processor.gallery->getDirect(processor.updateState->currentDirectId)->setName(name);
    
    //int selected = selectCB.getSelectedId();
    //if (selected != selectCB.getNumItems()) selectCB.changeItemText(selected, name);
    //selectCB.setSelectedId(selected, dontSendNotification );
}


void DirectViewController2::BKSingleSliderValueChanged(String name, double val)
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

void DirectViewController2::BKStackedSliderValueChanged(String name, Array<float> val)
{
    DirectPreparation::Ptr prep = processor.gallery->getStaticDirectPreparation(processor.updateState->currentDirectId);
    DirectPreparation::Ptr active = processor.gallery->getActiveDirectPreparation(processor.updateState->currentDirectId);
    
    prep->setTransposition(val);
    active->setTransposition(val);
}

void DirectViewController2::fillSelectCB(void)
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
