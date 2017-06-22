/*
  ==============================================================================

    TempoViewController.cpp
    Created: 17 Jun 2017 5:29:25pm
    Author:  Daniel Trueman

  ==============================================================================
*/

#include "TempoViewController.h"


TempoViewController::TempoViewController(BKAudioProcessor& p, BKItemGraph* theGraph):
BKViewController(p, theGraph)
{
    
    selectCB.setName("Tempo");
    selectCB.addSeparator();
    selectCB.addListener(this);
    selectCB.setSelectedItemIndex(0);
    addAndMakeVisible(selectCB);
    selectLabel.setText("Which", dontSendNotification);
    addAndMakeVisible(selectLabel);
    
    modeCB.setName("Mode");
    modeCB.addListener(this);
    fillModeCB();
    addAndMakeVisible(modeCB);
    modeLabel.setText("Mode", dontSendNotification);
    addAndMakeVisible(modeLabel);
    
    tempoSlider = new BKSingleSlider("Tempo", 40, 208, 100, 0.01);
    addAndMakeVisible(tempoSlider);
    
    AT1HistorySlider = new BKSingleSlider("History", 1, 10, 4, 1);
    addAndMakeVisible(AT1HistorySlider);
    
    AT1SubdivisionsSlider = new BKSingleSlider("Subdivisions within Time Difference", 0., 12, 1, 0.01);
    addAndMakeVisible(AT1SubdivisionsSlider);
    
    AT1MinMaxSlider = new BKRangeSlider("Min/Max Time Difference (ms)", 1, 2000, 100, 500, 10);
    AT1MinMaxSlider->setIsMinAlwaysLessThanMax(true);
    addAndMakeVisible(AT1MinMaxSlider);
    
    A1ModeCB.setName("Adaptive Tempo Mode");
    A1ModeCB.addListener(this);
    addAndMakeVisible(A1ModeCB);
    fillA1ModeCB();
    A1ModeLabel.setText("Adaptive Tempo Mode", dontSendNotification);
    addAndMakeVisible(A1ModeLabel);
    
    addAndMakeVisible(A1AdaptedTempo);
    addAndMakeVisible(A1AdaptedPeriodMultiplier);
    
    A1reset.addListener(this);
    A1reset.setButtonText("reset");
    addAndMakeVisible(A1reset);
    
    updateComponentVisibility();
    
    update();
}

void TempoViewController::resized()
{
    Rectangle<int> area (getLocalBounds());
    Rectangle<int> leftColumn = area.removeFromLeft(area.getWidth() * 0.25);
    Rectangle<int> rightColumn = area.removeFromRight(leftColumn.getWidth());
    
    //left column
    selectLabel.setBounds(leftColumn.removeFromTop(20));
    selectCB.setBounds(leftColumn.removeFromTop(20));
    leftColumn.removeFromTop(gYSpacing * 2);
    
    modeLabel.setBounds(leftColumn.removeFromTop(20));
    modeCB.setBounds(leftColumn.removeFromTop(20));
    leftColumn.removeFromTop(gYSpacing * 2);
    
    tempoSlider->setBounds(leftColumn.removeFromTop(50));
    
    //right column; adaptive stuff.
    A1ModeLabel.setBounds(rightColumn.removeFromTop(20));
    A1ModeCB.setBounds(rightColumn.removeFromTop(20));
    rightColumn.removeFromTop(gYSpacing * 2);
    
    rightColumn.removeFromTop(gYSpacing * 4);
    AT1HistorySlider->setBounds(rightColumn.removeFromTop(50));
    AT1SubdivisionsSlider->setBounds(rightColumn.removeFromTop(50));
    AT1MinMaxSlider->setBounds(rightColumn.removeFromTop(80));
    
    rightColumn.removeFromTop(gYSpacing * 4);
    A1AdaptedTempo.setBounds(rightColumn.removeFromTop(20));
    A1AdaptedPeriodMultiplier.setBounds(rightColumn.removeFromTop(20));
    
    rightColumn.removeFromTop(gYSpacing * 2);
    A1reset.setBounds(rightColumn.removeFromTop(20));
    
}


void TempoViewController::paint (Graphics& g)
{
    g.fillAll(Colours::lightgrey);
}

void TempoViewController::fillModeCB(void)
{
    
    modeCB.clear(dontSendNotification);
    
    for (int i = 0; i < cTempoModeTypes.size(); i++)
    {
        String name = cTempoModeTypes[i];
        modeCB.addItem(name, i+1);
    }
    
    modeCB.setSelectedItemIndex(0, dontSendNotification);
}


void TempoViewController::fillA1ModeCB(void)
{
    
    A1ModeCB.clear(dontSendNotification);
    
    for (int i = 0; i < cAdaptiveTempoModeTypes.size(); i++)
    {
        String name = cAdaptiveTempoModeTypes[i];
        A1ModeCB.addItem(name, i+1);
    }
    
    A1ModeCB.setSelectedItemIndex(0, dontSendNotification);
}

void TempoViewController::updateComponentVisibility()
{
    if(modeCB.getText() == "Adaptive Tempo 1")
    {
        AT1HistorySlider->setVisible(true);
        AT1SubdivisionsSlider->setVisible(true);;
        AT1MinMaxSlider->setVisible(true);
        
        A1ModeLabel.setVisible(true);
        A1ModeCB.setVisible(true);
        
        A1AdaptedTempo.setVisible(true);
        A1AdaptedPeriodMultiplier.setVisible(true);
        
        A1reset.setVisible(true);
    }
    else
    {
        AT1HistorySlider->setVisible(false);
        AT1SubdivisionsSlider->setVisible(false);;
        AT1MinMaxSlider->setVisible(false);
        
        A1ModeLabel.setVisible(false);
        A1ModeCB.setVisible(false);
        
        A1AdaptedTempo.setVisible(false);
        A1AdaptedPeriodMultiplier.setVisible(false);
        
        A1reset.setVisible(false);
    }
}


// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ TempoPreparationEditor ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~//

TempoPreparationEditor::TempoPreparationEditor(BKAudioProcessor& p, BKItemGraph* theGraph):
TempoViewController(p, theGraph)
{
    selectCB.addMyListener(this);
    fillSelectCB();
    
    tempoSlider->addMyListener(this);
    AT1HistorySlider->addMyListener(this);
    AT1SubdivisionsSlider->addMyListener(this);
    AT1MinMaxSlider->addMyListener(this);
    
    startTimer(50);
    
    update();
}

void TempoPreparationEditor::timerCallback()
{
    TempoProcessor::Ptr tProcessor = processor.gallery->getTempoProcessor(processor.updateState->currentTempoId);

    if(tProcessor->getPeriodMultiplier() != lastPeriodMultiplier)
    {
        lastPeriodMultiplier = tProcessor->getPeriodMultiplier();
        
        A1AdaptedTempo.setText("Adapted Tempo = " + String(tProcessor->getAdaptedTempo()), dontSendNotification);
        A1AdaptedPeriodMultiplier.setText("Adapted Period Multiplier = " + String(tProcessor->getPeriodMultiplier()), dontSendNotification);
    }
    
}


void TempoPreparationEditor::fillSelectCB(void)
{

    Tempo::PtrArr newpreps = processor.gallery->getAllTempo();
    
    selectCB.clear(dontSendNotification);
    for (int i = 0; i < newpreps.size(); i++)
    {
        String name = newpreps[i]->getName();
        if (name != String::empty)  selectCB.addItem(name, i+1);
        else                        selectCB.addItem(String(i+1), i+1);
    }
    
    selectCB.addItem("New tempo...", newpreps.size()+1);
    
    selectCB.setSelectedItemIndex(processor.updateState->currentTempoId, NotificationType::dontSendNotification);
    
}

void TempoPreparationEditor::bkComboBoxDidChange (ComboBox* box)
{
    String name = box->getName();
    
    TempoPreparation::Ptr prep = processor.gallery->getStaticTempoPreparation(processor.updateState->currentTempoId);
    TempoPreparation::Ptr active = processor.gallery->getStaticTempoPreparation(processor.updateState->currentTempoId);
    
    if (name == selectCB.getName())
    {
        processor.updateState->currentTempoId = box->getSelectedItemIndex();
        
        processor.updateState->idDidChange = true;
        
        if (processor.updateState->currentTempoId == selectCB.getNumItems()-1)
        {
            processor.gallery->addTempo();
            
            fillSelectCB();
        }
        
        update();
        updateComponentVisibility();
    }
    else if (name == modeCB.getName())
    {
        prep->setTempoSystem(TempoType(modeCB.getSelectedItemIndex()));
        active->setTempoSystem(TempoType(modeCB.getSelectedItemIndex()));
        updateComponentVisibility();
    }
    else if (name == A1ModeCB.getName())
    {
        prep->setAdaptiveTempo1Mode((AdaptiveTempo1Mode) A1ModeCB.getSelectedItemIndex());
        active->setAdaptiveTempo1Mode((AdaptiveTempo1Mode) A1ModeCB.getSelectedItemIndex());
    }
}


void TempoPreparationEditor::BKEditableComboBoxChanged(String name, BKEditableComboBox* cb)
{
    processor.gallery->getTempo(processor.updateState->currentTempoId)->setName(name);
}

void TempoPreparationEditor::BKRangeSliderValueChanged(String name, double minval, double maxval)
{
    TempoPreparation::Ptr prep = processor.gallery->getStaticTempoPreparation(processor.updateState->currentTempoId);
    TempoPreparation::Ptr active = processor.gallery->getActiveTempoPreparation(processor.updateState->currentTempoId);
    
    if(name == AT1MinMaxSlider->getName()) {
        DBG("got new AdaptiveTempo 1 time diff min/max " + String(minval) + " " + String(maxval));
        prep->setAdaptiveTempo1Min(minval);
        prep->setAdaptiveTempo1Max(maxval);
        active->setAdaptiveTempo1Min(minval);
        active->setAdaptiveTempo1Max(maxval);
    }
}

void TempoPreparationEditor::update(void)
{
    if (processor.updateState->currentTempoId < 0) return;
    
    fillSelectCB();
    
    TempoPreparation::Ptr prep = processor.gallery->getStaticTempoPreparation(processor.updateState->currentTempoId);
    
    selectCB.setSelectedItemIndex(processor.updateState->currentTempoId, dontSendNotification);
    modeCB.setSelectedItemIndex((int)prep->getTempoSystem(), dontSendNotification);
    tempoSlider->setValue(prep->getTempo(), dontSendNotification);
    
    A1ModeCB.setSelectedItemIndex(prep->getAdaptiveTempo1Mode(), dontSendNotification);
    AT1HistorySlider->setValue(prep->getAdaptiveTempo1History(), dontSendNotification);
    AT1SubdivisionsSlider->setValue(prep->getAdaptiveTempo1Subdivisions(), dontSendNotification);
    AT1MinMaxSlider->setMinValue(prep->getAdaptiveTempo1Min(), dontSendNotification);
    AT1MinMaxSlider->setMaxValue(prep->getAdaptiveTempo1Max(), dontSendNotification);
    
    updateComponentVisibility();
}


void TempoPreparationEditor::BKSingleSliderValueChanged(String name, double val)
{
    TempoPreparation::Ptr prep = processor.gallery->getStaticTempoPreparation(processor.updateState->currentTempoId);
    TempoPreparation::Ptr active = processor.gallery->getActiveTempoPreparation(processor.updateState->currentTempoId);;
    
    if(name == tempoSlider->getName()) {
        DBG("got tempo " + String(val));
        prep->setTempo(val);
        active->setTempo(val);
    }
    else if(name == AT1HistorySlider->getName()) {
        DBG("got A1History " + String(val));
        prep->setAdaptiveTempo1History(val);
        active->setAdaptiveTempo1History(val);
    }
    else if(name == AT1SubdivisionsSlider->getName()) {
        DBG("got A1Subdivisions " + String(val));
        prep->setAdaptiveTempo1Subdivisions(val);
        active->setAdaptiveTempo1Subdivisions(val);
    }
    
}

void TempoPreparationEditor::bkButtonClicked (Button* b)
{
    if (b == &A1reset)
    {
        DBG("resetting A1 tempo multiplier");
        
        TempoProcessor::Ptr tProcessor = processor.gallery->getTempoProcessor(processor.updateState->currentTempoId);
        tProcessor->reset();
    }
}

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ TempoModificationEditor ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~//

TempoModificationEditor::TempoModificationEditor(BKAudioProcessor& p, BKItemGraph* theGraph):
TempoViewController(p, theGraph)
{
    
    fillSelectCB();
    
    selectCB.addMyListener(this);
    tempoSlider->addMyListener(this);
    AT1HistorySlider->addMyListener(this);
    AT1SubdivisionsSlider->addMyListener(this);
    AT1MinMaxSlider->addMyListener(this);
    
    update();
}

void TempoModificationEditor::fillSelectCB(void)
{
    
    StringArray mods = processor.gallery->getAllTempoModNames();
    
    selectCB.clear(dontSendNotification);
    for (int i = 0; i < mods.size(); i++)
    {
        String name = mods[i];
        if (name != String::empty)  selectCB.addItem(name, i+1);
        else                        selectCB.addItem(String(i+1), i+1);
    }
    
    selectCB.addItem("New tempo modification...", mods.size()+1);
    
    selectCB.setSelectedItemIndex(processor.updateState->currentModTempoId, NotificationType::dontSendNotification);
    
}

void TempoModificationEditor::update(void)
{
    fillSelectCB();
    selectCB.setSelectedItemIndex(processor.updateState->currentModTempoId, dontSendNotification);
    
    TempoModPreparation::Ptr mod = processor.gallery->getTempoModPreparation(processor.updateState->currentModTempoId);
    
    // NEED TO MAKE SURE THIS IS LINKED TO RIGHT ITEM, need better way of doing this
    int targetId = processor.currentPiano->getMapper(PreparationTypeTempo, processor.updateState->currentModTempoId)->getId();
    
    TempoPreparation::Ptr prep = processor.gallery->getStaticTempoPreparation(targetId);
    
    String val = mod->getParam(TempoSystem);
    if (val != String::empty)   modeCB.setSelectedItemIndex(val.getIntValue(), dontSendNotification);
    //else                        modeCB.setSelectedItemIndex((int)prep->getTempoSystem(), dontSendNotification);
    
    val = mod->getParam(TempoBPM);
    if (val != String::empty)   tempoSlider->setValue(val.getFloatValue(), dontSendNotification);
    //else                        tempoSlider->setValue(prep->getTempo(), dontSendNotification);
    
    val = mod->getParam(AT1Mode);
    if (val != String::empty)   A1ModeCB.setSelectedItemIndex(val.getIntValue(), dontSendNotification);
    //else                        A1ModeCB.setSelectedItemIndex(prep->getAdaptiveTempo1Mode(), dontSendNotification);
    
    val = mod->getParam(AT1History);
    if (val != String::empty)   AT1HistorySlider->setValue(val.getIntValue(), dontSendNotification);
    //else                        AT1HistorySlider->setValue(prep->getAdaptiveTempo1History(), dontSendNotification);
    
    val = mod->getParam(AT1Subdivisions);
    if (val != String::empty)   AT1SubdivisionsSlider->setValue(val.getFloatValue(), dontSendNotification);
    //else                        AT1SubdivisionsSlider->setValue(prep->getAdaptiveTempo1Subdivisions(), dontSendNotification);
    
    val = mod->getParam(AT1Min);
    if (val != String::empty)   AT1MinMaxSlider->setMinValue(val.getDoubleValue(), dontSendNotification);
    //else                        AT1MinMaxSlider->setMinValue(prep->getAdaptiveTempo1Min(), dontSendNotification);
    
    val = mod->getParam(AT1Max);
    if (val != String::empty)   AT1MinMaxSlider->setMaxValue(val.getDoubleValue(), dontSendNotification);
    //else                        AT1MinMaxSlider->setMaxValue(prep->getAdaptiveTempo1Max(), dontSendNotification);
    
    updateComponentVisibility();
}


void TempoModificationEditor::bkComboBoxDidChange (ComboBox* box)
{
    String name = box->getName();
    
    TempoModPreparation::Ptr mod = processor.gallery->getTempoModPreparation(processor.updateState->currentModTempoId);
    
    if (name == selectCB.getName())
    {
        processor.updateState->currentModTempoId = box->getSelectedItemIndex();
        
        processor.updateState->idDidChange = true;
        
        if (processor.updateState->currentModTempoId == selectCB.getNumItems()-1)
        {
            processor.gallery->addTempoMod();
            
            fillSelectCB();
        }
        
        update();
        updateComponentVisibility();
        
        return;
    }
    
    if (name == modeCB.getName())
    {
        mod->setParam(TempoSystem, String(modeCB.getSelectedItemIndex()));
        
        updateComponentVisibility();
    }
    else if (name == A1ModeCB.getName())
    {
        mod->setParam(AT1Mode, String(A1ModeCB.getSelectedItemIndex()));
    }
    
    updateModification();
    
}


void TempoModificationEditor::BKEditableComboBoxChanged(String name, BKEditableComboBox* cb)
{
    processor.gallery->getTempo(processor.updateState->currentModTempoId)->setName(name);
    
    updateModification();
}

void TempoModificationEditor::BKRangeSliderValueChanged(String name, double minval, double maxval)
{
    TempoModPreparation::Ptr mod = processor.gallery->getTempoModPreparation(processor.updateState->currentModTempoId);

    if(name == AT1MinMaxSlider->getName())
    {
        mod->setParam(AT1Min, String(minval));
        mod->setParam(AT1Max, String(maxval));
    }
    
    updateModification();
}



void TempoModificationEditor::BKSingleSliderValueChanged(String name, double val)
{
    TempoModPreparation::Ptr mod = processor.gallery->getTempoModPreparation(processor.updateState->currentModTempoId);
    
    if(name == tempoSlider->getName())
    {
        mod->setParam(TempoBPM, String(val));
    }
    else if(name == AT1HistorySlider->getName())
    {
        mod->setParam(AT1History, String(val));
    }
    else if(name == AT1SubdivisionsSlider->getName())
    {
        mod->setParam(AT1Subdivisions, String(val));
    }
    
    updateModification();
    
}

void TempoModificationEditor::updateModification(void)
{
    processor.updateState->modificationDidChange = true;
}

