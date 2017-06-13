/*
  ==============================================================================

    TuningViewController2.cpp
    Created: 10 Jun 2017 11:24:22am
    Author:  Daniel Trueman

  ==============================================================================
*/

#include "TuningViewController2.h"


//==============================================================================
TuningViewController2::TuningViewController2(BKAudioProcessor& p, BKItemGraph* theGraph):
processor(p),
theGraph(theGraph)
{

    selectCB.setName("Tuning");
    selectCB.addSeparator();
    selectCB.addListener(this);
    selectCB.setSelectedItemIndex(0);
    selectCB.addMyListener(this);
    fillSelectCB();
    addAndMakeVisible(selectCB);
    
    scaleCB.setName("Scale");
    scaleCB.addListener(this);
    addAndMakeVisible(scaleCB);
    
    scaleLabel.setText("Scale", dontSendNotification);
    addAndMakeVisible(scaleLabel);
    
    fundamentalCB.setName("Fundamental");
    fundamentalCB.addListener(this);
    addAndMakeVisible(fundamentalCB);
    
    fundamentalLabel.setText("Fundamental", dontSendNotification);
    addAndMakeVisible(fundamentalLabel);
    
    A1IntervalScaleCB.setName("A1IntervalScale");
    A1IntervalScaleCB.addListener(this);
    addAndMakeVisible(A1IntervalScaleCB);
    
    A1IntervalScaleLabel.setText("Adaptive 1 Scale", dontSendNotification);
    addAndMakeVisible(A1IntervalScaleLabel);
    
    A1Inversional.addListener(this);
    A1Inversional.setButtonText ("inversional?");
    A1Inversional.setToggleState (true, dontSendNotification);
    A1Inversional.setColour(ToggleButton::textColourId, Colours::black);
    A1Inversional.setColour(ToggleButton::tickColourId, Colours::black);
    A1Inversional.setColour(ToggleButton::tickDisabledColourId, Colours::black);
    addAndMakeVisible(A1Inversional);
    
    A1AnchorScaleCB.setName("A1AnchorScale");
    A1AnchorScaleCB.addListener(this);
    addAndMakeVisible(A1AnchorScaleCB);
    
    A1AnchorScaleLabel.setText("Adaptive 1 Anchor Scale", dontSendNotification);
    addAndMakeVisible(A1AnchorScaleLabel);
    
    A1FundamentalCB.setName("A1Fundamental");
    A1FundamentalCB.addListener(this);
    addAndMakeVisible(A1FundamentalCB);
    
    A1FundamentalLabel.setText("Adaptive 1 Anchor Fundamental", dontSendNotification);
    addAndMakeVisible(A1FundamentalLabel);
    
    A1ClusterThresh = new BKSingleSlider("Cluster Threshold", 1, 1000, 0, 1);
    A1ClusterThresh->addMyListener(this);
    addAndMakeVisible(A1ClusterThresh);
    
    A1ClusterMax = new BKSingleSlider("Cluster Maximum", 1, 8, 1, 1);
    A1ClusterMax->addMyListener(this);
    addAndMakeVisible(A1ClusterMax);
    
    fillTuningCB();
    fillFundamentalCB();
    
    // Absolute Tuning Keyboard
    absoluteOffsets.ensureStorageAllocated(128);
    for(int i=0; i<128; i++) absoluteOffsets.add(0.);
    absoluteKeyboard.setName("Key-by-Key Offsets");
    absoluteKeyboard.addMyListener(this);
    addAndMakeVisible(absoluteKeyboard);
    
    //Custom Tuning Keyboard
    customOffsets.ensureStorageAllocated(12);
    for(int i=0; i<12; i++) customOffsets.add(0.);
    customKeyboard.setName("Custom Offsets");
    customKeyboard.addMyListener(this);
    customKeyboard.setAvailableRange(60, 71);
    customKeyboard.useOrderedPairs(false);
    addAndMakeVisible(customKeyboard);
    
    offsetSlider = new BKSingleSlider("Global Offset (cents)", -100, 100, 0, 0.1);
    offsetSlider->addMyListener(this);
    addAndMakeVisible(offsetSlider);

    updateFields();
}

void TuningViewController2::resized()
{
    Rectangle<int> area (getLocalBounds());
    float keyboardHeight = 60;
    Rectangle<int> absoluteKeymapRow = area.removeFromBottom(keyboardHeight + 20);
    
    Rectangle<int> leftColumn = area.removeFromLeft(area.getWidth() * 0.25);
    Rectangle<int> rightColumn = area.removeFromRight(leftColumn.getWidth());
    Rectangle<int> centerRightColumn = area.removeFromRight(leftColumn.getWidth());
    
    //prep select combo box
    selectCB.setBounds(leftColumn.removeFromTop(20));
    leftColumn.removeFromTop(gYSpacing * 2);
    
    scaleLabel.setBounds(leftColumn.removeFromTop(20));
    scaleCB.setBounds(leftColumn.removeFromTop(20));
    leftColumn.removeFromTop(gYSpacing * 2);
    
    fundamentalLabel.setBounds(leftColumn.removeFromTop(20));
    fundamentalCB.setBounds(leftColumn.removeFromTop(20));
    leftColumn.removeFromTop(gYSpacing * 4);
    
    offsetSlider->setBounds(leftColumn.removeFromTop(50));
    
    A1IntervalScaleLabel.setBounds(rightColumn.removeFromTop(20));
    A1IntervalScaleCB.setBounds(rightColumn.removeFromTop(20));
    A1Inversional.setBounds(rightColumn.removeFromTop(20));
    
    rightColumn.removeFromTop(gYSpacing * 4);
    A1ClusterMax->setBounds(rightColumn.removeFromTop(50));
    A1ClusterThresh->setBounds(rightColumn.removeFromTop(50));
    
    rightColumn.removeFromTop(gYSpacing * 4);
    A1AnchorScaleLabel.setBounds(rightColumn.removeFromTop(20));
    A1AnchorScaleCB.setBounds(rightColumn.removeFromTop(20));
    
    rightColumn.removeFromTop(gYSpacing * 2);
    A1FundamentalLabel.setBounds(rightColumn.removeFromTop(20));
    A1FundamentalCB.setBounds(rightColumn.removeFromTop(20));
    
    rightColumn.removeFromBottom(gYSpacing * 2);
    customKeyboard.setBounds(rightColumn.removeFromBottom(80));
    
    absoluteKeyboard.setBounds(absoluteKeymapRow);
}


void TuningViewController2::paint (Graphics& g)
{
    g.fillAll(Colours::lightgrey);
}


void TuningViewController2::fillSelectCB(void)
{
    // Direct menu
    Tuning::PtrArr newpreps = processor.gallery->getAllTuning();
    
    selectCB.clear(dontSendNotification);
    for (int i = 0; i < newpreps.size(); i++)
    {
        String name = newpreps[i]->getName();
        if (name != String::empty)  selectCB.addItem(name, i+1);
        else                        selectCB.addItem(String(i+1), i+1);
    }
    
    selectCB.addItem("New tuning...", newpreps.size()+1);
    
    selectCB.setSelectedItemIndex(processor.updateState->currentTuningId, NotificationType::dontSendNotification);
    
}

void TuningViewController2::fillTuningCB(void)
{
    //cTuningSystemNames
    
    scaleCB.clear(dontSendNotification);
    A1IntervalScaleCB.clear(dontSendNotification);
    A1AnchorScaleCB.clear(dontSendNotification);
    
    for (int i = 0; i < cTuningSystemNames.size(); i++)
    {
        String name = cTuningSystemNames[i];
        scaleCB.addItem(name, i+1);
        A1IntervalScaleCB.addItem(name, i+1);
        A1AnchorScaleCB.addItem(name, i+1);
        
        if(name == "Adaptive Tuning 1" || name == "Adaptive Anchored Tuning 1")
        {
            A1IntervalScaleCB.setItemEnabled(i+1, false);
            A1AnchorScaleCB.setItemEnabled(i+1, false);
        }
    }
}

void TuningViewController2::fillFundamentalCB(void)
{
    fundamentalCB.clear(dontSendNotification);
    A1FundamentalCB.clear(dontSendNotification);
    
    for (int i = 0; i < cFundamentalNames.size(); i++)
    {
        String name = cFundamentalNames[i];
        fundamentalCB.addItem(name, i+1);
        A1FundamentalCB.addItem(name, i+1);
    }
}

void TuningViewController2::bkComboBoxDidChange (ComboBox* box)
{
    String name = box->getName();
    
    TuningPreparation::Ptr prep = processor.gallery->getStaticTuningPreparation(processor.updateState->currentTuningId);
    TuningPreparation::Ptr active = processor.gallery->getActiveTuningPreparation(processor.updateState->currentTuningId);
    
    if (name == "Tuning")
    {
        processor.updateState->currentTuningId = box->getSelectedItemIndex();
        
        if (processor.updateState->currentTuningId == selectCB.getNumItems()-1)
        {
            processor.gallery->addTuning();
            
            fillSelectCB();
        }
        
        //updateFields(sendNotification);
        updateFields();
    }
    else if (name == scaleCB.getName())
    {
        prep->setTuning((TuningSystem)scaleCB.getSelectedItemIndex());
        active->setTuning((TuningSystem)scaleCB.getSelectedItemIndex());
        
        updateComponentVisibility();
        
    }
    else if (name == A1IntervalScaleCB.getName())
    {
        prep->setAdaptiveIntervalScale((TuningSystem)A1IntervalScaleCB.getSelectedItemIndex());
        active->setAdaptiveIntervalScale((TuningSystem)A1IntervalScaleCB.getSelectedItemIndex());
        
        updateComponentVisibility();
    }
    else if (name == A1AnchorScaleCB.getName())
    {
        prep->setAdaptiveAnchorScale((TuningSystem)A1AnchorScaleCB.getSelectedItemIndex());
        active->setAdaptiveAnchorScale((TuningSystem)A1AnchorScaleCB.getSelectedItemIndex());
        
        updateComponentVisibility();
    }
}

void TuningViewController2::updateComponentVisibility()
{
    if(scaleCB.getText() == "Custom")
    {
        customKeyboard.setVisible(true);
    }
    else customKeyboard.setVisible(false);
        
    if(scaleCB.getText() == "Adaptive Tuning 1")
    {
        A1IntervalScaleCB.setVisible(true);
        A1Inversional.setVisible(true);
        A1AnchorScaleCB.setVisible(false);
        A1FundamentalCB.setVisible(false);
        A1ClusterThresh->setVisible(true);
        A1ClusterMax->setVisible(true);
        A1IntervalScaleLabel.setVisible(true);
        A1AnchorScaleLabel.setVisible(false);
        A1FundamentalLabel.setVisible(false);
        
        if(A1IntervalScaleCB.getText() == "Custom")
        {
            customKeyboard.setVisible(true);
        }
    }
    else if(scaleCB.getText() == "Adaptive Anchored Tuning 1")
    {
        A1IntervalScaleCB.setVisible(true);
        A1Inversional.setVisible(true);
        A1AnchorScaleCB.setVisible(true);
        A1FundamentalCB.setVisible(true);
        A1ClusterThresh->setVisible(true);
        A1ClusterMax->setVisible(true);
        A1IntervalScaleLabel.setVisible(true);
        A1AnchorScaleLabel.setVisible(true);
        A1FundamentalLabel.setVisible(true);
        
        if(A1IntervalScaleCB.getText() == "Custom" || A1AnchorScaleCB.getText() == "Custom")
        {
            customKeyboard.setVisible(true);
        }
    }
    else
    {
        A1IntervalScaleCB.setVisible(false);
        A1Inversional.setVisible(false);
        A1AnchorScaleCB.setVisible(false);
        A1FundamentalCB.setVisible(false);
        A1ClusterThresh->setVisible(false);
        A1ClusterMax->setVisible(false);
        A1IntervalScaleLabel.setVisible(false);
        A1AnchorScaleLabel.setVisible(false);
        A1FundamentalLabel.setVisible(false);
    }
}

void TuningViewController2::BKEditableComboBoxChanged(String name, BKEditableComboBox* cb)
{
    processor.gallery->getTuning(processor.updateState->currentTuningId)->setName(name);
}


void TuningViewController2::updateFields(void)
{
    
    TuningPreparation::Ptr prep = processor.gallery->getActiveTuningPreparation(processor.updateState->currentTuningId);
    
    selectCB.setSelectedItemIndex(processor.updateState->currentTuningId, dontSendNotification);
    scaleCB.setSelectedItemIndex(prep->getTuning(), dontSendNotification);
    fundamentalCB.setSelectedItemIndex(prep->getFundamental(), dontSendNotification);
    offsetSlider->setValue(prep->getFundamentalOffset(), dontSendNotification);

    absoluteKeyboard.setAllValues(prep->getAbsoluteOffsetsCents());
    customKeyboard.setActiveValues(prep->getCustomScaleCents());
    
    A1IntervalScaleCB.setSelectedItemIndex(prep->getAdaptiveIntervalScale(), dontSendNotification);
    A1Inversional.setToggleState(prep->getAdaptiveInversional(), dontSendNotification);
    A1AnchorScaleCB.setSelectedItemIndex(prep->getAdaptiveAnchorScale(), dontSendNotification);
    A1FundamentalCB.setSelectedItemIndex(prep->getAdaptiveAnchorFundamental(), dontSendNotification);
    A1ClusterThresh->setValue(prep->getAdaptiveClusterThresh(), dontSendNotification);
    A1ClusterMax->setValue(prep->getAdaptiveHistory(), dontSendNotification);
    
    updateComponentVisibility();
}

void TuningViewController2::keyboardSliderChanged(String name, Array<float> values)
{
    TuningPreparation::Ptr prep = processor.gallery->getStaticTuningPreparation(processor.updateState->currentTuningId);
    TuningPreparation::Ptr active = processor.gallery->getActiveTuningPreparation(processor.updateState->currentTuningId);
 
    if(name == absoluteKeyboard.getName())
    {
        DBG("updating absolute tuning vals");
        prep->setAbsoluteOffsetCents(values);
        active->setAbsoluteOffsetCents(values);
    }
    else if(name == customKeyboard.getName())
    {
        DBG("updating custom tuning vals");
        prep->setCustomScaleCents(values);
        active->setCustomScaleCents(values);
    }
}

void TuningViewController2::BKSingleSliderValueChanged(String name, double val)
{
    TuningPreparation::Ptr prep = processor.gallery->getStaticTuningPreparation(processor.updateState->currentTuningId);
    TuningPreparation::Ptr active = processor.gallery->getActiveTuningPreparation(processor.updateState->currentTuningId);
    
    if(name == offsetSlider->getName()) {
        DBG("got offset " + String(val));
        prep->setFundamentalOffset(val);
        active->setFundamentalOffset(val);
    }
    else if(name == A1ClusterThresh->getName()) {
        DBG("got A1ClusterThresh " + String(val));
        prep->setAdaptiveClusterThresh(val);
        active->setAdaptiveClusterThresh(val);
    }
    else if(name == A1ClusterMax->getName()) {
        DBG("got A1ClusterMax " + String(val));
        prep->setAdaptiveHistory(val);
        active->setAdaptiveHistory(val);
    }
}

void TuningViewController2::bkTextFieldDidChange (TextEditor& textEditor)
{
    
}

void TuningViewController2::textEditorReturnKeyPressed(TextEditor& textEditor)
{
    
}

void TuningViewController2::bkButtonClicked (Button* b)
{
    if (b == &A1Inversional)
    {
        DBG("setting A1Inversional " + String(A1Inversional.getToggleState()));
        
        TuningPreparation::Ptr prep = processor.gallery->getStaticTuningPreparation(processor.updateState->currentTuningId);
        TuningPreparation::Ptr active = processor.gallery->getActiveTuningPreparation(processor.updateState->currentTuningId);

        prep->setAdaptiveInversional(A1Inversional.getToggleState());
        active->setAdaptiveInversional(A1Inversional.getToggleState());
    }
}





