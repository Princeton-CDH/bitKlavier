/*
  ==============================================================================

    NostalgicViewController.cpp
    Created: 24 May 2017 7:13:23pm
    Author:  Daniel Trueman

  ==============================================================================
*/

#include "NostalgicViewController.h"

NostalgicViewController::NostalgicViewController(BKAudioProcessor& p, BKItemGraph* theGraph):
BKViewController(p, theGraph)
{
    addAndMakeVisible(nDisplaySlider);
    
    selectCB.setName("Nostalgic");
    selectCB.addSeparator();
    selectCB.setSelectedItemIndex(0);
    
    addAndMakeVisible(selectCB);
    
    lengthModeSelectCB.setName("Length Mode");
    lengthModeSelectCB.addSeparator();
    lengthModeSelectCB.setSelectedItemIndex(0);
    fillModeSelectCB();
    addAndMakeVisible(lengthModeSelectCB);
    
    transpositionSlider = new BKStackedSlider("transpositions", -12, 12, -12, 12, 0, 0.01);
    addAndMakeVisible(transpositionSlider);
    
    lengthMultiplierSlider = new BKSingleSlider("note length multiplier", 0, 10, 1, 0.01);
    lengthMultiplierSlider->setSkewFactorFromMidPoint(1.);
    addAndMakeVisible(lengthMultiplierSlider);
    
    beatsToSkipSlider = new BKSingleSlider("beats to skip", 0, 10, 0, 1);
    addAndMakeVisible(beatsToSkipSlider);
    beatsToSkipSlider->setVisible(false);
    
    gainSlider = new BKSingleSlider("gain", 0, 10, 1, 0.01);
    gainSlider->setSkewFactorFromMidPoint(1.);
    addAndMakeVisible(gainSlider);
}

void NostalgicViewController::paint (Graphics& g)
{
    g.fillAll(Colours::transparentWhite);
}

void NostalgicViewController::resized()
{
    Rectangle<int> area (getLocalBounds());
    
    Rectangle<int> displayRow = area.removeFromBottom(area.getHeight() * 0.5);
    nDisplaySlider.setBounds(displayRow);
    
    Rectangle<int> leftColumn = area.removeFromLeft(area.getWidth() * 0.5);
    selectCB.setBounds(leftColumn.removeFromTop(20));
    lengthModeSelectCB.setBounds(area.removeFromTop(20));
    lengthMultiplierSlider->setBounds(area.removeFromTop(40));
    beatsToSkipSlider->setBounds(lengthMultiplierSlider->getBounds());
    gainSlider->setBounds(area.removeFromBottom(40));
    
    transpositionSlider->setBounds(leftColumn.removeFromTop(40));
    
}


void NostalgicViewController::fillModeSelectCB(void)
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

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ NostalgicPreparationEditor ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ //

NostalgicPreparationEditor::NostalgicPreparationEditor(BKAudioProcessor& p, BKItemGraph* theGraph):
NostalgicViewController(p, theGraph)
{
    fillSelectCB();
    
    nDisplaySlider.addMyListener(this);
    selectCB.addListener(this);
    selectCB.addMyListener(this);
    lengthModeSelectCB.addListener(this);
    transpositionSlider->addMyListener(this);
    lengthMultiplierSlider->addMyListener(this);
    beatsToSkipSlider->addMyListener(this);
    
    gainSlider->addMyListener(this);
    
    startTimer(20);
}

void NostalgicPreparationEditor::BKWaveDistanceUndertowSliderValueChanged(String name, double wavedist, double undertow)
{
    
    NostalgicPreparation::Ptr prep = processor.gallery->getStaticNostalgicPreparation(processor.updateState->currentNostalgicId);
    NostalgicPreparation::Ptr active = processor.gallery->getActiveNostalgicPreparation(processor.updateState->currentNostalgicId);
    
    prep    ->setWaveDistance(wavedist);
    active  ->setWaveDistance(wavedist);
    prep    ->setUndertow(undertow);
    active  ->setUndertow(undertow);
    
}

void NostalgicPreparationEditor::BKEditableComboBoxChanged(String name, BKEditableComboBox* cb)
{
    processor.gallery->getNostalgic(processor.updateState->currentNostalgicId)->setName(name);
}

void NostalgicPreparationEditor::update(void)
{
    if (processor.updateState->currentNostalgicId < 0) return;
    
    fillSelectCB();
    
    NostalgicPreparation::Ptr prep = processor.gallery->getActiveNostalgicPreparation(processor.updateState->currentNostalgicId);
    
    nDisplaySlider.setWaveDistance(prep->getWavedistance(), dontSendNotification);
    nDisplaySlider.setUndertow(prep->getUndertow(), dontSendNotification);
    
    selectCB.setSelectedItemIndex(processor.updateState->currentNostalgicId, dontSendNotification);
    lengthModeSelectCB.setSelectedItemIndex(prep->getMode(), dontSendNotification);
    
    transpositionSlider->setValue(prep->getTransposition(), dontSendNotification);
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

void NostalgicPreparationEditor::bkMessageReceived (const String& message)
{
    if (message == "nostalgic/update")
    {
        
        update();
    }
}

void NostalgicPreparationEditor::bkComboBoxDidChange (ComboBox* box)
{
    String name = box->getName();
    
    if (name == "Nostalgic")
    {
        processor.updateState->currentNostalgicId = box->getSelectedItemIndex();
        
        processor.updateState->idDidChange = true;
        
        if (processor.updateState->currentNostalgicId == selectCB.getNumItems()-1)
        {
            processor.gallery->addNostalgic();
            
            fillSelectCB();
        }
        
        //update(sendNotification);
        update();
        
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

void NostalgicPreparationEditor::BKSingleSliderValueChanged(String name, double val)
{
    NostalgicPreparation::Ptr prep = processor.gallery->getStaticNostalgicPreparation(processor.updateState->currentNostalgicId);
    NostalgicPreparation::Ptr active = processor.gallery->getActiveNostalgicPreparation(processor.updateState->currentNostalgicId);
    
    if(name == "note length multiplier")
    {
        //DBG("note length multiplier " + String(val));
        prep->setLengthMultiplier(val);
        active->setLengthMultiplier(val);
    }
    else if(name == "beats to skip")
    {
        //DBG("beats to skip " + String(val));
        prep->setBeatsToSkip(val);
        active->setBeatsToSkip(val);
    }
    else if(name == "gain")
    {
        //DBG("gain " + String(val));
        prep->setGain(val);
        active->setGain(val);
    }
}

void NostalgicPreparationEditor::BKStackedSliderValueChanged(String name, Array<float> val)
{
    NostalgicPreparation::Ptr prep = processor.gallery->getStaticNostalgicPreparation(processor.updateState->currentNostalgicId);
    NostalgicPreparation::Ptr active = processor.gallery->getActiveNostalgicPreparation(processor.updateState->currentNostalgicId);

    prep->setTransposition(val);
    active->setTransposition(val);
}

void NostalgicPreparationEditor::fillSelectCB(void)
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

void NostalgicPreparationEditor::timerCallback()
{
    NostalgicProcessor::Ptr nProcessor = processor.gallery->getNostalgicProcessor(processor.updateState->currentNostalgicId);
    
    Array<int> currentPlayPositions = nProcessor->getPlayPositions();
    Array<int> currentUndertowPositions = nProcessor->getUndertowPositions();
    currentPlayPositions.addArray(currentUndertowPositions);
    
    nDisplaySlider.updateSliderPositions(currentPlayPositions);
    
}

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ NostalgicModificationEditor ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ //

NostalgicModificationEditor::NostalgicModificationEditor(BKAudioProcessor& p, BKItemGraph* theGraph):
NostalgicViewController(p, theGraph)
{
    fillSelectCB();
    
    nDisplaySlider.addMyListener(this);
    selectCB.addListener(this);
    selectCB.addMyListener(this);
    lengthModeSelectCB.addListener(this);
    transpositionSlider->addMyListener(this);
    lengthMultiplierSlider->addMyListener(this);
    beatsToSkipSlider->addMyListener(this);
    
    gainSlider->addMyListener(this);
    
    startTimer(20);
}

void NostalgicModificationEditor::BKWaveDistanceUndertowSliderValueChanged(String name, double wavedist, double undertow)
{
    
    NostalgicPreparation::Ptr prep = processor.gallery->getStaticNostalgicPreparation(processor.updateState->currentNostalgicId);
    NostalgicPreparation::Ptr active = processor.gallery->getActiveNostalgicPreparation(processor.updateState->currentNostalgicId);
    
    prep    ->setWaveDistance(wavedist);
    active  ->setWaveDistance(wavedist);
    prep    ->setUndertow(undertow);
    active  ->setUndertow(undertow);
    
}

void NostalgicModificationEditor::BKEditableComboBoxChanged(String name, BKEditableComboBox* cb)
{
    processor.gallery->getNostalgic(processor.updateState->currentNostalgicId)->setName(name);
}

void NostalgicModificationEditor::update(void)
{
    if (processor.updateState->currentNostalgicId < 0) return;
    
    fillSelectCB();
    
    NostalgicPreparation::Ptr prep = processor.gallery->getActiveNostalgicPreparation(processor.updateState->currentNostalgicId);
    
    nDisplaySlider.setWaveDistance(prep->getWavedistance(), dontSendNotification);
    nDisplaySlider.setUndertow(prep->getUndertow(), dontSendNotification);
    
    selectCB.setSelectedItemIndex(processor.updateState->currentNostalgicId, dontSendNotification);
    lengthModeSelectCB.setSelectedItemIndex(prep->getMode(), dontSendNotification);
    
    transpositionSlider->setValue(prep->getTransposition(), dontSendNotification);
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

void NostalgicModificationEditor::bkMessageReceived (const String& message)
{
    if (message == "nostalgic/update")
    {
        
        update();
    }
}

void NostalgicModificationEditor::bkComboBoxDidChange (ComboBox* box)
{
    String name = box->getName();
    
    if (name == "Nostalgic")
    {
        processor.updateState->currentNostalgicId = box->getSelectedItemIndex();
        
        processor.updateState->idDidChange = true;
        
        if (processor.updateState->currentNostalgicId == selectCB.getNumItems()-1)
        {
            processor.gallery->addNostalgic();
            
            fillSelectCB();
        }
        
        //update(sendNotification);
        update();
        
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

void NostalgicModificationEditor::BKSingleSliderValueChanged(String name, double val)
{
    NostalgicPreparation::Ptr prep = processor.gallery->getStaticNostalgicPreparation(processor.updateState->currentNostalgicId);
    NostalgicPreparation::Ptr active = processor.gallery->getActiveNostalgicPreparation(processor.updateState->currentNostalgicId);
    
    if(name == "note length multiplier")
    {
        //DBG("note length multiplier " + String(val));
        prep->setLengthMultiplier(val);
        active->setLengthMultiplier(val);
    }
    else if(name == "beats to skip")
    {
        //DBG("beats to skip " + String(val));
        prep->setBeatsToSkip(val);
        active->setBeatsToSkip(val);
    }
    else if(name == "gain")
    {
        //DBG("gain " + String(val));
        prep->setGain(val);
        active->setGain(val);
    }
}

void NostalgicModificationEditor::BKStackedSliderValueChanged(String name, Array<float> val)
{
    NostalgicPreparation::Ptr prep = processor.gallery->getStaticNostalgicPreparation(processor.updateState->currentNostalgicId);
    NostalgicPreparation::Ptr active = processor.gallery->getActiveNostalgicPreparation(processor.updateState->currentNostalgicId);
    
    prep->setTransposition(val);
    active->setTransposition(val);
}

void NostalgicModificationEditor::fillSelectCB(void)
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

void NostalgicModificationEditor::timerCallback()
{
    NostalgicProcessor::Ptr nProcessor = processor.gallery->getNostalgicProcessor(processor.updateState->currentNostalgicId);
    
    Array<int> currentPlayPositions = nProcessor->getPlayPositions();
    Array<int> currentUndertowPositions = nProcessor->getUndertowPositions();
    currentPlayPositions.addArray(currentUndertowPositions);
    
    nDisplaySlider.updateSliderPositions(currentPlayPositions);
    
}
