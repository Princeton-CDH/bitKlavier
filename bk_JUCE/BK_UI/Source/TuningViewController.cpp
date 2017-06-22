/*
  ==============================================================================

    TuningViewController.cpp
    Created: 10 Jun 2017 11:24:22am
    Author:  Daniel Trueman

  ==============================================================================
*/

#include "TuningViewController.h"

/*
 TODO
 
 => cleanup: both this, and the keyboard and keyboardslider classes....
 
 */


//==============================================================================
TuningViewController::TuningViewController(BKAudioProcessor& p, BKItemGraph* theGraph):
BKViewController(p, theGraph)
{

    selectCB.setName("Tuning");
    selectCB.addSeparator();
    selectCB.setSelectedItemIndex(0);
    addAndMakeVisible(selectCB);
    
    scaleCB.setName("Scale");
    addAndMakeVisible(scaleCB);
    
    scaleLabel.setText("Scale", dontSendNotification);
    addAndMakeVisible(scaleLabel);
    
    fundamentalCB.setName("Fundamental");
    addAndMakeVisible(fundamentalCB);
    
    fundamentalLabel.setText("Fundamental", dontSendNotification);
    addAndMakeVisible(fundamentalLabel);
    
    A1IntervalScaleCB.setName("A1IntervalScale");
    addAndMakeVisible(A1IntervalScaleCB);
    
    A1IntervalScaleLabel.setText("Adaptive 1 Scale", dontSendNotification);
    addAndMakeVisible(A1IntervalScaleLabel);
    
    A1Inversional.setButtonText ("inversional?");
    A1Inversional.setToggleState (true, dontSendNotification);
    A1Inversional.setColour(ToggleButton::textColourId, Colours::black);
    A1Inversional.setColour(ToggleButton::tickColourId, Colours::black);
    A1Inversional.setColour(ToggleButton::tickDisabledColourId, Colours::black);
    addAndMakeVisible(A1Inversional);
    
    A1AnchorScaleCB.setName("A1AnchorScale");
    addAndMakeVisible(A1AnchorScaleCB);
    
    A1AnchorScaleLabel.setText("Adaptive 1 Anchor Scale", dontSendNotification);
    addAndMakeVisible(A1AnchorScaleLabel);
    
    A1FundamentalCB.setName("A1Fundamental");
    addAndMakeVisible(A1FundamentalCB);
    
    A1FundamentalLabel.setText("Adaptive 1 Anchor Fundamental", dontSendNotification);
    addAndMakeVisible(A1FundamentalLabel);
    
    A1ClusterThresh = new BKSingleSlider("Cluster Threshold", 1, 1000, 0, 1);
    addAndMakeVisible(A1ClusterThresh);
    
    A1ClusterMax = new BKSingleSlider("Cluster Maximum", 1, 8, 1, 1);
    addAndMakeVisible(A1ClusterMax);
    
    A1reset.setButtonText("reset");
    addAndMakeVisible(A1reset);
    
    fillTuningCB();
    fillFundamentalCB();
    
    // Absolute Tuning Keyboard
    absoluteOffsets.ensureStorageAllocated(128);
    for(int i=0; i<128; i++) absoluteOffsets.add(0.);
    absoluteKeyboard.setName("Key-by-Key Offsets");
    addAndMakeVisible(absoluteKeyboard);
    
    //Custom Tuning Keyboard
    customOffsets.ensureStorageAllocated(12);
    for(int i=0; i<12; i++) customOffsets.add(0.);
    customKeyboard.setName("Temperament Offsets");
    //customKeyboard.setAvailableRange(60, 71);
    customKeyboard.setAvailableRange(0, 11);
    customKeyboard.useOrderedPairs(false);
    customKeyboard.setFundamental(0);
    addAndMakeVisible(customKeyboard);
    
    offsetSlider = new BKSingleSlider("Global Offset (cents)", -100, 100, 0, 0.1);
    addAndMakeVisible(offsetSlider);
    
    lastNote.setText("last note: ", dontSendNotification);
    lastInterval.setText("last interval: ", dontSendNotification);
    addAndMakeVisible(lastNote);
    addAndMakeVisible(lastInterval);
    
    update();
}

void TuningViewController::resized()
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
    leftColumn.removeFromTop(gYSpacing * 2);
    
    customKeyboard.setBounds(leftColumn.removeFromTop(120));
    leftColumn.removeFromTop(gYSpacing * 4);
    
    offsetSlider->setBounds(leftColumn.removeFromTop(50));
    leftColumn.removeFromTop(gYSpacing * 4);
    
    lastNote.setBounds(leftColumn.removeFromTop(20));
    leftColumn.removeFromTop(gYSpacing * 2);
    
    lastInterval.setBounds(leftColumn.removeFromTop(20));
    leftColumn.removeFromTop(gYSpacing * 2);
    
    
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
    
    rightColumn.removeFromTop(gYSpacing * 2);
    A1reset.setBounds(rightColumn.removeFromTop(20));
    
    absoluteKeyboard.setBounds(absoluteKeymapRow);
}


void TuningViewController::paint (Graphics& g)
{
    g.fillAll(Colours::lightgrey);
}

void TuningViewController::fillTuningCB(void)
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
                
        if(name == "Custom") {
            customIndex = i;
            DBG("assigned to customIndex " + String(customIndex));
        }
    }
}

void TuningViewController::fillFundamentalCB(void)
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

void TuningViewController::updateComponentVisibility()
{
        
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
        A1reset.setVisible(true);

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
        A1reset.setVisible(true);

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
        A1reset.setVisible(false);
    }
}

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ TuningPreparationEditor ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ //

TuningPreparationEditor::TuningPreparationEditor(BKAudioProcessor& p, BKItemGraph* theGraph):
TuningViewController(p, theGraph)
{
    fillSelectCB();
    
    selectCB.addMyListener(this);
    
    selectCB.addListener(this);

    scaleCB.addListener(this);
    
    fundamentalCB.addListener(this);

    A1IntervalScaleCB.addListener(this);
    
    A1Inversional.addListener(this);
    
    A1AnchorScaleCB.addListener(this);
    
    A1FundamentalCB.addListener(this);
    
    A1ClusterThresh->addMyListener(this);
    
    A1ClusterMax->addMyListener(this);
    
    A1reset.addListener(this);
    
    absoluteKeyboard.addMyListener(this);
    
    offsetSlider->addMyListener(this);
    
    startTimer(50);
    
    update();
}

void TuningPreparationEditor::timerCallback()
{
    TuningProcessor::Ptr tProcessor = processor.gallery->getTuningProcessor(processor.updateState->currentTuningId);
    if(tProcessor->getLastNoteTuning() != lastNoteTuningSave)
    {
        lastNoteTuningSave = tProcessor->getLastNoteTuning();
        lastNote.setText("last note: " + String(lastNoteTuningSave), dontSendNotification);
        lastInterval.setText("last interval: "  + String(tProcessor->getLastIntervalTuning()), dontSendNotification);
    }
}


void TuningPreparationEditor::bkComboBoxDidChange (ComboBox* box)
{
    String name = box->getName();
    
    TuningPreparation::Ptr prep = processor.gallery->getStaticTuningPreparation(processor.updateState->currentTuningId);
    TuningPreparation::Ptr active = processor.gallery->getActiveTuningPreparation(processor.updateState->currentTuningId);
    
    if (name == selectCB.getName())
    {
        processor.updateState->currentTuningId = box->getSelectedItemIndex();
        
        processor.updateState->idDidChange = true;
        
        if (processor.updateState->currentTuningId == selectCB.getNumItems()-1)
        {
            processor.gallery->addTuning();
            
            fillSelectCB();
        }
        
        //update(sendNotification);
        update();
    }
    else if (name == scaleCB.getName())
    {
        prep->setTuning((TuningSystem)scaleCB.getSelectedItemIndex());
        active->setTuning((TuningSystem)scaleCB.getSelectedItemIndex());
        
        Tuning::Ptr currentTuning = processor.gallery->getTuning(processor.updateState->currentTuningId);
        customKeyboard.setValues(currentTuning->getCurrentScaleCents());
        
        updateComponentVisibility();
        
    }
    else if (name == fundamentalCB.getName())
    {
        prep->setFundamental((PitchClass)fundamentalCB.getSelectedItemIndex());
        active->setFundamental((PitchClass)fundamentalCB.getSelectedItemIndex());
        
        customKeyboard.setFundamental(fundamentalCB.getSelectedItemIndex());
        
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
    else if (name == A1FundamentalCB.getName())
    {
        prep->setAdaptiveAnchorFundamental((PitchClass)A1FundamentalCB.getSelectedItemIndex());
        active->setAdaptiveAnchorFundamental((PitchClass)A1FundamentalCB.getSelectedItemIndex());
        
        updateComponentVisibility();
        
    }
}


void TuningPreparationEditor::fillSelectCB(void)
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


void TuningPreparationEditor::BKEditableComboBoxChanged(String name, BKEditableComboBox* cb)
{
    processor.gallery->getTuning(processor.updateState->currentTuningId)->setName(name);
}


void TuningPreparationEditor::update(void)
{
    if (processor.updateState->currentTuningId < 0) return;
    
    fillSelectCB();
    
    TuningPreparation::Ptr prep = processor.gallery->getActiveTuningPreparation(processor.updateState->currentTuningId);
    
    selectCB.setSelectedItemIndex(processor.updateState->currentTuningId, dontSendNotification);
    scaleCB.setSelectedItemIndex(prep->getTuning(), dontSendNotification);
    fundamentalCB.setSelectedItemIndex(prep->getFundamental(), dontSendNotification);
    offsetSlider->setValue(prep->getFundamentalOffset() * 100., dontSendNotification);

    absoluteKeyboard.setValues(prep->getAbsoluteOffsetsCents());
    Tuning::Ptr currentTuning = processor.gallery->getTuning(processor.updateState->currentTuningId);
    customKeyboard.setValues(currentTuning->getCurrentScaleCents());
    
    A1IntervalScaleCB.setSelectedItemIndex(prep->getAdaptiveIntervalScale(), dontSendNotification);
    A1Inversional.setToggleState(prep->getAdaptiveInversional(), dontSendNotification);
    A1AnchorScaleCB.setSelectedItemIndex(prep->getAdaptiveAnchorScale(), dontSendNotification);
    A1FundamentalCB.setSelectedItemIndex(prep->getAdaptiveAnchorFundamental(), dontSendNotification);
    A1ClusterThresh->setValue(prep->getAdaptiveClusterThresh(), dontSendNotification);
    A1ClusterMax->setValue(prep->getAdaptiveHistory(), dontSendNotification);
    
    updateComponentVisibility();
}

void TuningPreparationEditor::keyboardSliderChanged(String name, Array<float> values)
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
        scaleCB.setSelectedItemIndex(customIndex, dontSendNotification);

        prep->setCustomScaleCents(values);
        active->setCustomScaleCents(values);
        
        prep->setTuning((TuningSystem)customIndex);
        active->setTuning((TuningSystem)customIndex);
    }
}

void TuningPreparationEditor::BKSingleSliderValueChanged(String name, double val)
{
    TuningPreparation::Ptr prep = processor.gallery->getStaticTuningPreparation(processor.updateState->currentTuningId);
    TuningPreparation::Ptr active = processor.gallery->getActiveTuningPreparation(processor.updateState->currentTuningId);
    
    if(name == offsetSlider->getName()) {
        DBG("got offset " + String(val));
        prep->setFundamentalOffset(val * 0.01);
        active->setFundamentalOffset(val * 0.01);
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

void TuningPreparationEditor::bkButtonClicked (Button* b)
{
    if (b == &A1Inversional)
    {
        DBG("setting A1Inversional " + String(A1Inversional.getToggleState()));
        
        TuningPreparation::Ptr prep = processor.gallery->getStaticTuningPreparation(processor.updateState->currentTuningId);
        TuningPreparation::Ptr active = processor.gallery->getActiveTuningPreparation(processor.updateState->currentTuningId);

        prep->setAdaptiveInversional(A1Inversional.getToggleState());
        active->setAdaptiveInversional(A1Inversional.getToggleState());
    }
    else if (b == &A1reset)
    {
        DBG("resetting A1");
        
        TuningProcessor::Ptr tProcessor = processor.gallery->getTuningProcessor(processor.updateState->currentTuningId);
        tProcessor->adaptiveReset();
    }
}

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ TuningModificationEditor ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ //
TuningModificationEditor::TuningModificationEditor(BKAudioProcessor& p, BKItemGraph* theGraph):
TuningViewController(p, theGraph)
{
    fillSelectCB();
    
    selectCB.addMyListener(this);
    
    selectCB.addListener(this);
    
    scaleCB.addListener(this);
    
    fundamentalCB.addListener(this);
    
    A1IntervalScaleCB.addListener(this);
    
    A1Inversional.addListener(this);
    
    A1AnchorScaleCB.addListener(this);
    
    A1FundamentalCB.addListener(this);
    
    A1ClusterThresh->addMyListener(this);
    
    A1ClusterMax->addMyListener(this);
    
    A1reset.addListener(this);
    
    absoluteKeyboard.addMyListener(this);
    
    offsetSlider->addMyListener(this);
    
    update();
}


void TuningModificationEditor::update(void)
{
    TuningModPreparation::Ptr mod = processor.gallery->getTuningModPreparation(processor.updateState->currentModTuningId);
    
    TuningPreparation::Ptr prep = processor.gallery->getActiveTuningPreparation(processor.updateState->currentTuningId);
    
    fillSelectCB();
    
    selectCB.setSelectedItemIndex(processor.updateState->currentModTuningId, dontSendNotification);
    
    String val = mod->getParam(TuningScale);
    if (val != String::empty)   scaleCB.setSelectedItemIndex(val.getIntValue(), dontSendNotification);
    //else                        scaleCB.setSelectedItemIndex(prep->getTuning(), dontSendNotification);
    
    val = mod->getParam(TuningFundamental);
    if (val != String::empty)   fundamentalCB.setSelectedItemIndex(val.getIntValue(), dontSendNotification);
    //else                        fundamentalCB.setSelectedItemIndex(prep->getFundamental(), dontSendNotification);
    
    val = mod->getParam(TuningOffset);
    if (val != String::empty)   offsetSlider->setValue(val.getFloatValue() * 100., dontSendNotification);
    //else                        offsetSlider->setValue(prep->getFundamentalOffset() * 100., dontSendNotification);
    
    val = mod->getParam(TuningAbsoluteOffsets);
    if (val != String::empty)   absoluteKeyboard.setValues(stringToFloatArray(val));
    //else                        absoluteKeyboard.setValues(prep->getAbsoluteOffsetsCents());
    
    val = mod->getParam(TuningA1IntervalScale);
    if (val != String::empty)   A1IntervalScaleCB.setSelectedItemIndex(val.getIntValue(), dontSendNotification);
    //else                        A1IntervalScaleCB.setSelectedItemIndex(prep->getAdaptiveIntervalScale(), dontSendNotification);
    
    val = mod->getParam(TuningA1Inversional);
    if (val != String::empty)   A1Inversional.setToggleState((bool)val.getIntValue(), dontSendNotification);
    //else                        A1Inversional.setToggleState(prep->getAdaptiveInversional(), dontSendNotification);
    
    val = mod->getParam(TuningA1AnchorScale);
    if (val != String::empty)   A1AnchorScaleCB.setSelectedItemIndex(val.getIntValue(), dontSendNotification);
    //else                        A1AnchorScaleCB.setSelectedItemIndex(prep->getAdaptiveAnchorScale(), dontSendNotification);
    
    val = mod->getParam(TuningA1AnchorFundamental);
    if (val != String::empty)   A1FundamentalCB.setSelectedItemIndex(val.getIntValue(), dontSendNotification);
    //else                        A1FundamentalCB.setSelectedItemIndex(prep->getAdaptiveAnchorFundamental(), dontSendNotification);
    
    val = mod->getParam(TuningA1ClusterThresh);
    if (val != String::empty)   A1ClusterThresh->setValue(val.getLargeIntValue(), dontSendNotification);
    //else                        A1ClusterThresh->setValue(prep->getAdaptiveClusterThresh(), dontSendNotification);
    
    val = mod->getParam(TuningA1ClusterThresh);
    if (val != String::empty)   A1ClusterMax->setValue(val.getIntValue(), dontSendNotification);
    //else                        A1ClusterMax->setValue(prep->getAdaptiveHistory(), dontSendNotification);
    
    updateComponentVisibility();
}

void TuningModificationEditor::fillSelectCB(void)
{
    // Direct menu
    StringArray mods = processor.gallery->getAllTuningModNames();
    
    selectCB.clear(dontSendNotification);
    for (int i = 0; i < mods.size(); i++)
    {
        String name = mods[i];
        if (name != String::empty)  selectCB.addItem(name, i+1);
        else                        selectCB.addItem(String(i+1), i+1);
    }
    
    selectCB.addItem("New tuning...", mods.size()+1);
    
    selectCB.setSelectedItemIndex(processor.updateState->currentModTuningId, NotificationType::dontSendNotification);
    
}

void TuningModificationEditor::bkComboBoxDidChange (ComboBox* box)
{
    String name = box->getName();
    
    TuningModPreparation::Ptr mod = processor.gallery->getTuningModPreparation(processor.updateState->currentModTuningId);
    
    if (name == selectCB.getName())
    {
        processor.updateState->currentModTuningId = box->getSelectedItemIndex();
        
        processor.updateState->idDidChange = true;
        
        if (processor.updateState->currentTuningId == selectCB.getNumItems()-1)
        {
            processor.gallery->addTuningMod();
            
            fillSelectCB();
        }
        
        //update(sendNotification);
        update();
        return;
    }
    
    if (name == scaleCB.getName())
    {
        mod->setParam(TuningScale, String(scaleCB.getSelectedItemIndex()));
        
        Tuning::Ptr currentTuning = processor.gallery->getTuning(processor.updateState->currentTuningId);
        customKeyboard.setValues(currentTuning->getCurrentScaleCents());
    }
    else if (name == fundamentalCB.getName())
    {
        mod->setParam(TuningScale, String(fundamentalCB.getSelectedItemIndex()));
        
        customKeyboard.setFundamental(fundamentalCB.getSelectedItemIndex());
    }
    else if (name == A1IntervalScaleCB.getName())
    {
        mod->setParam(TuningA1IntervalScale, String(A1IntervalScaleCB.getSelectedItemIndex()));
    }
    else if (name == A1AnchorScaleCB.getName())
    {
        mod->setParam(TuningA1AnchorScale, String(A1AnchorScaleCB.getSelectedItemIndex()));
    }
    else if (name == A1FundamentalCB.getName())
    {
        mod->setParam(TuningA1AnchorFundamental, String(A1FundamentalCB.getSelectedItemIndex()));
    }
    
    updateModification();
    
    updateComponentVisibility();
}

void TuningModificationEditor::BKEditableComboBoxChanged(String name, BKEditableComboBox* cb)
{
    processor.gallery->getTuningModPreparation(processor.updateState->currentModTuningId)->setName(name);
    
    updateModification();
}

void TuningModificationEditor::keyboardSliderChanged(String name, Array<float> values)
{
    TuningModPreparation::Ptr mod = processor.gallery->getTuningModPreparation(processor.updateState->currentModTuningId);
    
    if(name == absoluteKeyboard.getName())
    {
        mod->setParam(TuningAbsoluteOffsets, floatArrayToString(values));
    }
    else if(name == customKeyboard.getName())
    {
        scaleCB.setSelectedItemIndex(customIndex, dontSendNotification);
        
        mod->setParam(TuningCustomScale, floatArrayToString(values));
        mod->setParam(TuningScale, String(customIndex));
    }
    
    updateModification();
}

void TuningModificationEditor::BKSingleSliderValueChanged(String name, double val)
{
    TuningModPreparation::Ptr mod = processor.gallery->getTuningModPreparation(processor.updateState->currentModTuningId);
    
    if(name == offsetSlider->getName())
    {
        mod->setParam(TuningOffset, String(val));
    }
    else if(name == A1ClusterThresh->getName())
    {
        mod->setParam(TuningA1ClusterThresh, String(val));
    }
    else if(name == A1ClusterMax->getName())
    {
        mod->setParam(TuningA1History, String(val));
    }
    
    updateModification();
}

void TuningModificationEditor::updateModification(void)
{
    processor.updateState->modificationDidChange = true;
}






