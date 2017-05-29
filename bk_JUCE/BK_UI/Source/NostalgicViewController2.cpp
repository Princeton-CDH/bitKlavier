/*
  ==============================================================================

    NostalgicViewController2.cpp
    Created: 24 May 2017 7:13:23pm
    Author:  Daniel Trueman

  ==============================================================================
*/

#include "NostalgicViewController2.h"

NostalgicViewController2::NostalgicViewController2(BKAudioProcessor& p, BKItemGraph* theGraph):
processor(p),
theGraph(theGraph)
{
    nDisplaySlider.addMyListener(this);
    addAndMakeVisible(nDisplaySlider);
    
    
    selectCB.setName("Nostalgic");
    selectCB.addSeparator();
    selectCB.addListener(this);
    selectCB.setSelectedItemIndex(0);
    selectCB.addMyListener(this);
    fillSelectCB();
    addAndMakeVisible(selectCB);
    
    lengthModeSelectCB.setName("Length Mode");
    lengthModeSelectCB.addSeparator();
    lengthModeSelectCB.addListener(this);
    lengthModeSelectCB.setSelectedItemIndex(0);
    fillModeSelectCB();
    addAndMakeVisible(lengthModeSelectCB);
    
    transpositionSlider = new BKSingleSlider("transposition", -12, 12, 0, 0.01);
    transpositionSlider->addMyListener(this);
    addAndMakeVisible(transpositionSlider);
    
    lengthMultiplierSlider = new BKSingleSlider("note length multiplier", 0, 10, 1, 0.01);
    lengthMultiplierSlider->addMyListener(this);
    addAndMakeVisible(lengthMultiplierSlider);
    
    beatsToSkipSlider = new BKSingleSlider("beats to skip", 0, 10, 0, 1);
    beatsToSkipSlider->addMyListener(this);
    addAndMakeVisible(beatsToSkipSlider);
    beatsToSkipSlider->setVisible(false);
    
    gainSlider = new BKSingleSlider("gain", 0, 10, 1, 0.01);
    gainSlider->setSkewFactorFromMidPoint(1.);
    gainSlider->addMyListener(this);
    addAndMakeVisible(gainSlider);
    
    startTimer(20);
}

void NostalgicViewController2::paint (Graphics& g)
{
    g.setColour(Colours::goldenrod);
    g.drawRect(getLocalBounds(), 1);
}

void NostalgicViewController2::resized()
{
    Rectangle<int> area (getLocalBounds());
    
    Rectangle<int> displayRow = area.removeFromBottom(area.getHeight() * 0.5);
    nDisplaySlider.setBounds(displayRow);
    
    selectCB.setBounds(area.removeFromTop(20));
    lengthModeSelectCB.setBounds(area.removeFromTop(20));
    
    int sliderHeight = area.getHeight() / 3;
    transpositionSlider->setBounds(area.removeFromTop(sliderHeight));
    lengthMultiplierSlider->setBounds(area.removeFromTop(sliderHeight));
    beatsToSkipSlider->setBounds(lengthMultiplierSlider->getBounds());
    gainSlider->setBounds(area.removeFromTop(sliderHeight));
}

void NostalgicViewController2::BKWaveDistanceUndertowSliderValueChanged(String name, double wavedist, double undertow)
{
    DBG("got new nostalgic slider vals " + String(wavedist) + " " + String(undertow));
    
    NostalgicPreparation::Ptr prep = processor.gallery->getStaticNostalgicPreparation(processor.updateState->currentNostalgicId);
    NostalgicPreparation::Ptr active = processor.gallery->getActiveNostalgicPreparation(processor.updateState->currentNostalgicId);
    
    prep    ->setWaveDistance(wavedist);
    active  ->setWaveDistance(wavedist);
    prep    ->setUndertow(undertow);
    active  ->setUndertow(undertow);
    
}

void NostalgicViewController2::BKEditableComboBoxChanged(String name, int index)
{
    processor.gallery->getNostalgic(processor.updateState->currentNostalgicId)->setName(name);
    
    int selected = selectCB.getSelectedId();
    if (selected != selectCB.getNumItems()) selectCB.changeItemText(selected, name);
    selectCB.setSelectedId(selected, dontSendNotification );
}

void NostalgicViewController2::updateFields(void)
{
    
    NostalgicPreparation::Ptr prep = processor.gallery->getActiveNostalgicPreparation(processor.updateState->currentNostalgicId);
    
    nDisplaySlider.setWaveDistance(prep->getWavedistance(), dontSendNotification);
    nDisplaySlider.setUndertow(prep->getUndertow(), dontSendNotification);
    
    selectCB.setSelectedItemIndex(processor.updateState->currentNostalgicId, dontSendNotification);
    lengthModeSelectCB.setSelectedItemIndex(prep->getMode(), dontSendNotification);
    
    Array<float> tempTransp = prep->getTransposition();
    transpositionSlider->setValue(tempTransp.getFirst(), dontSendNotification);
    lengthMultiplierSlider->setValue(prep->getLengthMultiplier(), dontSendNotification);
    beatsToSkipSlider->setValue(prep->getBeatsToSkip(), dontSendNotification);
    gainSlider->setValue(prep->getGain(), dontSendNotification);
    
    if(prep->getMode() == NoteLengthSync)
    {
        lengthMultiplierSlider->setVisible(true);
        beatsToSkipSlider->setVisible(false);
    }
    else
    {
        lengthMultiplierSlider->setVisible(false);
        beatsToSkipSlider->setVisible(true);
    }

}

void NostalgicViewController2::bkMessageReceived (const String& message)
{
    if (message == "nostalgic/update")
    {
        
        updateFields();
    }
}

void NostalgicViewController2::bkComboBoxDidChange (ComboBox* box)
{
    String name = box->getName();
    
    if (name == "Nostalgic")
    {
        processor.updateState->currentNostalgicId = box->getSelectedItemIndex();
        
        if (processor.updateState->currentNostalgicId == selectCB.getNumItems()-1)
        {
            processor.gallery->addNostalgic();
            
            fillSelectCB();
        }
        
        //updateFields(sendNotification);
        updateFields();
        
    }
    else if (name == "Length Mode")
    {
        NostalgicPreparation::Ptr prep = processor.gallery->getStaticNostalgicPreparation(processor.updateState->currentNostalgicId);
        NostalgicPreparation::Ptr active = processor.gallery->getActiveNostalgicPreparation(processor.updateState->currentNostalgicId);
        
        prep    ->setMode((NostalgicSyncMode) box->getSelectedItemIndex());
        active  ->setMode((NostalgicSyncMode) box->getSelectedItemIndex());
        
        if(prep->getMode() == NoteLengthSync)
        {
            lengthMultiplierSlider->setVisible(true);
            beatsToSkipSlider->setVisible(false);
        }
        else
        {
            lengthMultiplierSlider->setVisible(false);
            beatsToSkipSlider->setVisible(true);
        }

    }
}

void NostalgicViewController2::BKSingleSliderValueChanged(String name, double val)
{
    NostalgicPreparation::Ptr prep = processor.gallery->getStaticNostalgicPreparation(processor.updateState->currentNostalgicId);
    NostalgicPreparation::Ptr active = processor.gallery->getActiveNostalgicPreparation(processor.updateState->currentNostalgicId);
    
    if(name == "transposition") {
        DBG("transposition " + String(val));
        Array<float> newvals;
        newvals.add(val);
        prep->setTransposition(newvals);
        active->setTransposition(newvals);
    }
    else if(name == "note length multiplier")
    {
        DBG("note length multiplier " + String(val));
        prep->setLengthMultiplier(val);
        active->setLengthMultiplier(val);
    }
    else if(name == "beats to skip")
    {
        DBG("beats to skip " + String(val));
        prep->setBeatsToSkip(val);
        active->setBeatsToSkip(val);
    }
    else if(name == "gain")
    {
        DBG("gain " + String(val));
        prep->setGain(val);
        active->setGain(val);
    }
}

void NostalgicViewController2::fillSelectCB(void)
{
    // Direct menu
    Nostalgic::PtrArr newpreps = processor.gallery->getAllNostalgic();
    
    selectCB.clear(dontSendNotification);
    for (int i = 0; i < newpreps.size(); i++)
    {
        String name = newpreps[i]->getName();
        if (name != String::empty)  selectCB.addItem(name, i+1);
        else                        selectCB.addItem(String(i+1), i+1);
    }
    
    selectCB.addItem("New nostalgic...", newpreps.size()+1);
    
    selectCB.setSelectedItemIndex(processor.updateState->currentNostalgicId, NotificationType::dontSendNotification);
    
}

void NostalgicViewController2::fillModeSelectCB(void)
{
    
    lengthModeSelectCB.clear(dontSendNotification);
    for (int i = 0; i < cNostalgicSyncModes.size(); i++)
    {
        String name = cNostalgicSyncModes[i];
        if (name != String::empty)  lengthModeSelectCB.addItem(name, i+1);
        else                        lengthModeSelectCB.addItem(String(i+1), i+1);
    }
    
    lengthModeSelectCB.setSelectedItemIndex(0, NotificationType::dontSendNotification);
}

void NostalgicViewController2::timerCallback()
{
    NostalgicProcessor::Ptr nProcessor = processor.gallery->getNostalgicProcessor(processor.updateState->currentNostalgicId);
    
    Array<int> currentPlayPositions = nProcessor->getPlayPositions();
    Array<int> currentUndertowPositions = nProcessor->getUndertowPositions();
    currentPlayPositions.addArray(currentUndertowPositions);
    
    nDisplaySlider.updateSliderPositions(currentPlayPositions);
    
}
