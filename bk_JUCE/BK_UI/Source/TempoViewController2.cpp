/*
  ==============================================================================

    TempoViewController2.cpp
    Created: 17 Jun 2017 5:29:25pm
    Author:  Daniel Trueman

  ==============================================================================
*/

#include "TempoViewController2.h"


TempoViewController2::TempoViewController2(BKAudioProcessor& p, BKItemGraph* theGraph):
processor(p),
theGraph(theGraph)
{
    
    selectCB.setName("Tempo");
    selectCB.addSeparator();
    selectCB.addListener(this);
    selectCB.setSelectedItemIndex(0);
    selectCB.addMyListener(this);
    fillSelectCB();
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
    tempoSlider->addMyListener(this);
    addAndMakeVisible(tempoSlider);
    
    AT1HistorySlider = new BKSingleSlider("History", 1, 10, 4, 1);
    AT1HistorySlider->addMyListener(this);
    addAndMakeVisible(AT1HistorySlider);
    
    AT1SubdivisionsSlider = new BKSingleSlider("Subdivisions within Time Difference", 0., 12, 1, 0.01);
    AT1SubdivisionsSlider->addMyListener(this);
    addAndMakeVisible(AT1SubdivisionsSlider);
    
    AT1MinMaxSlider = new BKRangeSlider("Min/Max Time Difference (ms)", 1, 2000, 100, 500, 10);
    AT1MinMaxSlider->addMyListener(this);
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
    
    startTimer(50);
    
    updateFields();
    updateComponentVisibility();
}


void TempoViewController2::resized()
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


void TempoViewController2::paint (Graphics& g)
{
    g.fillAll(Colours::lightgrey);
}


void TempoViewController2::timerCallback()
{
    TempoProcessor::Ptr tProcessor = processor.gallery->getTempoProcessor(processor.updateState->currentTempoId);

    if(tProcessor->getPeriodMultiplier() != lastPeriodMultiplier)
    {
        lastPeriodMultiplier = tProcessor->getPeriodMultiplier();
        
        A1AdaptedTempo.setText("Adapted Tempo = " + String(tProcessor->getAdaptedTempo()), dontSendNotification);
        A1AdaptedPeriodMultiplier.setText("Adapted Period Multiplier = " + String(tProcessor->getPeriodMultiplier()), dontSendNotification);
    }
    
}


void TempoViewController2::fillSelectCB(void)
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


void TempoViewController2::fillModeCB(void)
{

    modeCB.clear(dontSendNotification);
    
    for (int i = 0; i < cTempoModeTypes.size(); i++)
    {
        String name = cTempoModeTypes[i];
        modeCB.addItem(name, i+1);
    }
    
    modeCB.setSelectedItemIndex(0, dontSendNotification);
}


void TempoViewController2::fillA1ModeCB(void)
{
    
    A1ModeCB.clear(dontSendNotification);
    
    for (int i = 0; i < cAdaptiveTempoModeTypes.size(); i++)
    {
        String name = cAdaptiveTempoModeTypes[i];
        A1ModeCB.addItem(name, i+1);
    }
    
    A1ModeCB.setSelectedItemIndex(0, dontSendNotification);
}


void TempoViewController2::bkComboBoxDidChange (ComboBox* box)
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
        
        updateFields();
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


void TempoViewController2::BKEditableComboBoxChanged(String name, BKEditableComboBox* cb)
{
    processor.gallery->getTempo(processor.updateState->currentTempoId)->setName(name);
}

void TempoViewController2::BKRangeSliderValueChanged(String name, double minval, double maxval)
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


void TempoViewController2::updateComponentVisibility()
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


void TempoViewController2::updateFields(void)
{
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


void TempoViewController2::BKSingleSliderValueChanged(String name, double val)
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

void TempoViewController2::bkButtonClicked (Button* b)
{
    if (b == &A1reset)
    {
        DBG("resetting A1 tempo multiplier");
        
        TempoProcessor::Ptr tProcessor = processor.gallery->getTempoProcessor(processor.updateState->currentTempoId);
        tProcessor->reset();
    }
}
