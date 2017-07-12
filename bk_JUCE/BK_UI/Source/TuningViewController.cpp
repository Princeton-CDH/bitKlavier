/*
  ==============================================================================

    TuningViewController.cpp
    Created: 10 Jun 2017 11:24:22am
    Author:  Daniel Trueman

  ==============================================================================
*/

#include "TuningViewController.h"

TuningViewController::TuningViewController(BKAudioProcessor& p, BKItemGraph* theGraph):
BKViewController(p,theGraph)
{
    
    setLookAndFeel(&buttonsAndMenusLAF);
    
    iconImageComponent.setImage(ImageCache::getFromMemory(BinaryData::tuning_icon_png, BinaryData::tuning_icon_pngSize));
    iconImageComponent.setImagePlacement(RectanglePlacement(juce::RectanglePlacement::stretchToFit));
    iconImageComponent.setAlpha(0.095);
    addAndMakeVisible(iconImageComponent);
    
    selectCB.setName("Tuning");
    selectCB.addSeparator();
    selectCB.addListener(this);
    selectCB.setSelectedItemIndex(0);
    addAndMakeVisible(selectCB);
    
    scaleCB.setName("Scale");
    addAndMakeVisible(scaleCB);
    
    scaleLabel.setText("Scale", dontSendNotification);
    //addAndMakeVisible(scaleLabel);
    
    fundamentalCB.setName("Fundamental");
    addAndMakeVisible(fundamentalCB);
    
    fundamentalLabel.setText("Fundamental", dontSendNotification);
    //addAndMakeVisible(fundamentalLabel);
    
    A1IntervalScaleCB.setName("A1IntervalScale");
    //A1IntervalScaleCB.BKSetJustificationType(juce::Justification::centredRight);
    A1IntervalScaleCB.addListener(this);
    addAndMakeVisible(A1IntervalScaleCB);
    
    A1IntervalScaleLabel.setText("Adaptive Scale:", dontSendNotification);
    //A1IntervalScaleLabel.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(A1IntervalScaleLabel);
    
    A1Inversional.setButtonText ("invert");
    A1Inversional.setToggleState (true, dontSendNotification);
    //buttonsAndMenusLAF.setToggleBoxTextToRightBool(false);
    A1Inversional.setColour(ToggleButton::textColourId, Colours::white);
    A1Inversional.setColour(ToggleButton::tickColourId, Colours::white);
    A1Inversional.setColour(ToggleButton::tickDisabledColourId, Colours::white);
    addAndMakeVisible(A1Inversional);
    
    A1AnchorScaleCB.setName("A1AnchorScale");
    addAndMakeVisible(A1AnchorScaleCB);
    
    A1AnchorScaleLabel.setText("Anchor Scale:", dontSendNotification);
    //A1AnchorScaleLabel.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(A1AnchorScaleLabel);
    
    A1FundamentalCB.setName("A1Fundamental");
    addAndMakeVisible(A1FundamentalCB);
    
    //A1FundamentalLabel.setText("Adaptive 1 Anchor Fundamental", dontSendNotification);
    //addAndMakeVisible(A1FundamentalLabel);
    
    A1ClusterThresh = new BKSingleSlider("Cluster Threshold", 1, 1000, 0, 1);
    A1ClusterThresh->setJustifyRight(false);
    addAndMakeVisible(A1ClusterThresh);
    
    A1ClusterMax = new BKSingleSlider("Cluster Maximum", 1, 8, 1, 1);
    A1ClusterMax->setJustifyRight(false);
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
    lastInterval.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(lastNote);
    addAndMakeVisible(lastInterval);
    
    addAndMakeVisible(hideOrShow);
    hideOrShow.setName("hideOrShow");
    hideOrShow.setButtonText(" X ");
}

void TuningViewController::resized()
{
    Rectangle<int> area (getLocalBounds());
    
    float paddingScalarX = (float)(getTopLevelComponent()->getWidth() - gMainComponentMinWidth) / (gMainComponentWidth - gMainComponentMinWidth);
    float paddingScalarY = (float)(getTopLevelComponent()->getHeight() - gMainComponentMinHeight) / (gMainComponentHeight - gMainComponentMinHeight);
    
    iconImageComponent.setBounds(area);
    area.reduce(10 * paddingScalarX + 4, 10 * paddingScalarY + 4);
    
    float keyboardHeight = 60 + 50 * paddingScalarY;
    Rectangle<int> absoluteKeymapRow = area.removeFromBottom(keyboardHeight);
    absoluteKeymapRow.reduce(gXSpacing, 0);
    absoluteKeyboard.setBounds(absoluteKeymapRow);
    
    Rectangle<int> leftColumn = area.removeFromLeft(area.getWidth() * 0.5);
    Rectangle<int> comboBoxSlice = leftColumn.removeFromTop(gComponentComboBoxHeight);
    comboBoxSlice.removeFromRight(gXSpacing + 2.*gPaddingConst * paddingScalarX);
    hideOrShow.setBounds(comboBoxSlice.removeFromLeft(gComponentComboBoxHeight));
    comboBoxSlice.removeFromLeft(gXSpacing);
    selectCB.setBounds(comboBoxSlice.removeFromLeft(comboBoxSlice.getWidth() / 2.));
    comboBoxSlice.removeFromLeft(gXSpacing);
    A1reset.setBounds(comboBoxSlice.removeFromLeft(45));
    
    /* *** above here should be generic (mostly) to all prep layouts *** */
    /* ***         below here will be specific to each prep          *** */
    
    // ********* right column
    
    Rectangle<int> modeSlice = area.removeFromTop(gComponentComboBoxHeight);
    modeSlice.removeFromLeft(gXSpacing + 2.*gPaddingConst * paddingScalarX);
    modeSlice.removeFromRight(gXSpacing);
    scaleCB.setBounds(modeSlice.removeFromLeft(modeSlice.getWidth() / 2.));
    
    modeSlice.removeFromLeft(gXSpacing + gPaddingConst * paddingScalarX);
    fundamentalCB.setBounds(modeSlice);
    
    int customKeyboardHeight = 60 + 70. * paddingScalarY;
    int extraY = (area.getHeight() - (customKeyboardHeight + gComponentSingleSliderHeight + gComponentTextFieldHeight + gYSpacing * 4)) * 0.25;
    
    area.removeFromTop(extraY + gYSpacing);
    Rectangle<int> customKeyboardSlice = area.removeFromTop(customKeyboardHeight);
    customKeyboardSlice.removeFromLeft(gXSpacing + 2.*gPaddingConst * paddingScalarX);
    customKeyboardSlice.removeFromRight(gXSpacing);
    customKeyboard.setBounds(customKeyboardSlice);
    
    area.removeFromTop(extraY + gYSpacing);
    Rectangle<int> offsetSliderSlice = area.removeFromTop(gComponentSingleSliderHeight);
    offsetSliderSlice.removeFromLeft(gXSpacing + 2.*gPaddingConst * paddingScalarX - gComponentSingleSliderXOffset);
    offsetSliderSlice.removeFromRight(gXSpacing - gComponentSingleSliderXOffset);
    offsetSlider->setBounds(offsetSliderSlice);
    
    area.removeFromTop(extraY + gYSpacing);
    Rectangle<int> lastNoteSlice = area.removeFromTop(gComponentTextFieldHeight);
    lastNoteSlice.removeFromLeft(gXSpacing + 2.*gPaddingConst * paddingScalarX);
    lastNoteSlice.removeFromRight(gXSpacing);
    lastNote.setBounds(lastNoteSlice.removeFromLeft(lastNoteSlice.getWidth() * 0.5));
    lastInterval.setBounds(lastNoteSlice);
    
    // ********* left column
    
    extraY = (leftColumn.getHeight() -
              (gComponentComboBoxHeight * 2 +
               gComponentSingleSliderHeight * 2 +
               gYSpacing * 5)) * 0.25;
    
    //DBG("extraY = " + String(extraY));
    
    leftColumn.removeFromTop(extraY + gYSpacing);
    Rectangle<int> A1IntervalScaleCBSlice = leftColumn.removeFromTop(gComponentComboBoxHeight);
    A1IntervalScaleCBSlice.removeFromRight(gXSpacing + 2.*gPaddingConst * paddingScalarX);
    A1IntervalScaleCBSlice.removeFromLeft(gXSpacing);
    int tempwidth = A1IntervalScaleCBSlice.getWidth() / 3.;
    A1Inversional.setBounds(A1IntervalScaleCBSlice.removeFromRight(tempwidth));
    A1IntervalScaleCB.setBounds(A1IntervalScaleCBSlice.removeFromRight(tempwidth));
    A1IntervalScaleLabel.setBounds(A1IntervalScaleCBSlice);
    
    leftColumn.removeFromTop(extraY + gYSpacing);
    Rectangle<int> A1ClusterMaxSlice = leftColumn.removeFromTop(gComponentSingleSliderHeight);
    A1ClusterMaxSlice.removeFromRight(gXSpacing + 2.*gPaddingConst * paddingScalarX - gComponentSingleSliderXOffset);
    //A1ClusterMaxSlice.removeFromLeft(gXSpacing);
    A1ClusterMax->setBounds(A1ClusterMaxSlice);
    
    leftColumn.removeFromTop(gYSpacing);
    Rectangle<int> A1ClusterThreshSlice = leftColumn.removeFromTop(gComponentSingleSliderHeight);
    A1ClusterThreshSlice.removeFromRight(gXSpacing + 2.*gPaddingConst * paddingScalarX - gComponentSingleSliderXOffset);
    //A1ClusterThreshSlice.removeFromLeft(gXSpacing);
    A1ClusterThresh->setBounds(A1ClusterThreshSlice);
    
    leftColumn.removeFromTop(extraY + gYSpacing);
    Rectangle<int> A1AnchorScaleCBSlice = leftColumn.removeFromTop(gComponentComboBoxHeight);
    A1AnchorScaleCBSlice.removeFromRight(gXSpacing + 2.*gPaddingConst * paddingScalarX);
    A1AnchorScaleCBSlice.removeFromLeft(gXSpacing);
    tempwidth = A1AnchorScaleCBSlice.getWidth() / 3.;
    A1AnchorScaleLabel.setBounds(A1AnchorScaleCBSlice.removeFromLeft(tempwidth));
    A1AnchorScaleCB.setBounds(A1AnchorScaleCBSlice.removeFromLeft(tempwidth));
    A1AnchorScaleCBSlice.removeFromLeft(gXSpacing);
    A1FundamentalCB.setBounds(A1AnchorScaleCBSlice);
    
}


void TuningViewController::paint (Graphics& g)
{
    //g.fillAll(Colours::lightgrey);
    //g.fillAll(Colours::transparentWhite);
    g.fillAll(Colours::black);
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
    
    customKeyboard.addMyListener(this);
    
    offsetSlider->addMyListener(this);
    
    hideOrShow.addListener(this);
    
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
        int index = box->getSelectedItemIndex();
        
        int oldId = processor.updateState->currentTuningId;
        int newId = processor.gallery->getIdFromIndex(PreparationTypeTuning, index);
        
        if (index == selectCB.getNumItems()-1)
        {
            processor.gallery->addTuning();
            
            processor.gallery->setEditted(PreparationTypeTuning, oldId, true);
            
            processor.gallery->getAllTuning().getLast()->editted = true;
            
            newId = processor.gallery->getAllTuning().getLast()->getId();
        }
        
        processor.updateState->currentTuningId = newId;
        
        processor.updateState->removeActive(PreparationTypeTuning, oldId);
        
        if (!processor.gallery->getTuning(oldId)->editted)
        {
            processor.updateState->removePreparation(PreparationTypeTuning, oldId);
            
            processor.gallery->remove(PreparationTypeTuning, oldId);
        }
        
        processor.updateState->addActive(PreparationTypeTuning, newId);
        
        processor.updateState->idDidChange = true;
        
        fillSelectCB();
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
    
    if (name != selectCB.getName())
    {
        Tuning::Ptr tuning = processor.gallery->getTuning(processor.updateState->currentTuningId);
        tuning->editted = true;
    }
}


void TuningPreparationEditor::fillSelectCB(void)
{
    selectCB.clear(dontSendNotification);
    
    Array<int> index = processor.gallery->getIndexList(PreparationTypeTuning);
    
    for (int i = 0; i < index.size(); i++)
    {
        int Id = index[i];
        String name = processor.gallery->getTuning(Id)->getName();
        if (name != String::empty)  selectCB.addItem(name, i+1);
        else                        selectCB.addItem(String(i+1), i+1);
        
        selectCB.setItemEnabled(i+1, true);
        if (processor.updateState->isActive(PreparationTypeTuning, Id) &&
            (Id != processor.updateState->currentTuningId))
        {
            selectCB.setItemEnabled(i+1, false);
        }
    }
    
    selectCB.addSeparator();
    selectCB.addItem("New tuning...", index.size()+1);
    
    int currentId = processor.updateState->currentTuningId;
    
    selectCB.setSelectedItemIndex(processor.gallery->getIndexFromId(PreparationTypeTuning, currentId), NotificationType::dontSendNotification);
    
}


void TuningPreparationEditor::BKEditableComboBoxChanged(String name, BKEditableComboBox* cb)
{
    Tuning::Ptr tuning = processor.gallery->getTuning(processor.updateState->currentTuningId);
    tuning->editted = true;
    
    tuning->setName(name);
}


void TuningPreparationEditor::update(void)
{
    if (processor.updateState->currentTuningId < 0) return;
    
    fillSelectCB();
    
    TuningPreparation::Ptr prep = processor.gallery->getActiveTuningPreparation(processor.updateState->currentTuningId);
    
    if (prep != nullptr)
    {
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
    
}

void TuningPreparationEditor::keyboardSliderChanged(String name, Array<float> values)
{
    Tuning::Ptr tuning = processor.gallery->getTuning(processor.updateState->currentTuningId);
    tuning->editted = true;
    
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
    Tuning::Ptr tuning = processor.gallery->getTuning(processor.updateState->currentTuningId);
    tuning->editted = true;
    
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

void TuningPreparationEditor::buttonClicked (Button* b)
{
    Tuning::Ptr tuning = processor.gallery->getTuning(processor.updateState->currentTuningId);
    tuning->editted = true;
    
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
    else if (b == &hideOrShow)
    {
        processor.updateState->setCurrentDisplay(DisplayNil);
    }
}

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ TuningModificationEditor ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ //
TuningModificationEditor::TuningModificationEditor(BKAudioProcessor& p, BKItemGraph* theGraph):
TuningViewController(p, theGraph)
{
    
    lastNote.setVisible(false);
    lastInterval.setVisible(false);
    A1reset.setVisible(false);
    greyOutAllComponents();
    
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
    customKeyboard.addMyListener(this);
    offsetSlider->addMyListener(this);
    hideOrShow.addListener(this);

    update();
}

void TuningModificationEditor::greyOutAllComponents()
{
    scaleCB.setAlpha(gModAlpha);
    fundamentalCB.setAlpha(gModAlpha);
    A1IntervalScaleCB.setAlpha(gModAlpha);
    A1Inversional.setAlpha(gModAlpha);
    A1AnchorScaleCB.setAlpha(gModAlpha);
    A1FundamentalCB.setAlpha(gModAlpha);
    A1ClusterThresh->setAlpha(gModAlpha);
    A1ClusterMax->setAlpha(gModAlpha);
    absoluteKeyboard.setAlpha(gModAlpha);
    customKeyboard.setAlpha(gModAlpha);
    offsetSlider->setAlpha(gModAlpha);
    A1IntervalScaleLabel.setAlpha(gModAlpha);
    A1AnchorScaleLabel.setAlpha(gModAlpha);
}

void TuningModificationEditor::highlightModedComponents()
{
    TuningModPreparation::Ptr mod = processor.gallery->getTuningModPreparation(processor.updateState->currentModTuningId);
    
    if(mod->getParam(TuningScale) != "")                scaleCB.setAlpha(1.);
    if(mod->getParam(TuningFundamental) != "")          fundamentalCB.setAlpha(1);
    if(mod->getParam(TuningA1IntervalScale) != "")      { A1IntervalScaleCB.setAlpha(1); A1IntervalScaleLabel.setAlpha(1); }
    if(mod->getParam(TuningA1Inversional) != "")        A1Inversional.setAlpha(1);
    if(mod->getParam(TuningA1AnchorScale) != "")        { A1AnchorScaleCB.setAlpha(1); A1AnchorScaleLabel.setAlpha(1); }
    if(mod->getParam(TuningA1AnchorFundamental) != "")  A1FundamentalCB.setAlpha(1);
    if(mod->getParam(TuningA1ClusterThresh) != "")      A1ClusterThresh->setAlpha(1);
    if(mod->getParam(TuningA1History) != "")            A1ClusterMax->setAlpha(1);
    if(mod->getParam(TuningAbsoluteOffsets) != "")      absoluteKeyboard.setAlpha(1);
    if(mod->getParam(TuningCustomScale) != "")          customKeyboard.setAlpha(1);
    if(mod->getParam(TuningOffset) != "")               offsetSlider->setAlpha(1);
}

void TuningModificationEditor::update(void)
{
    TuningModPreparation::Ptr mod = processor.gallery->getTuningModPreparation(processor.updateState->currentModTuningId);
    
    if (mod != nullptr)
    {
        fillSelectCB();
        
        greyOutAllComponents();
        highlightModedComponents();
        
        selectCB.setSelectedItemIndex(processor.updateState->currentModTuningId, dontSendNotification);
        
        String val = mod->getParam(TuningScale);
        scaleCB.setSelectedItemIndex(val.getIntValue(), dontSendNotification);
        //                       scaleCB.setSelectedItemIndex(prep->getTuning(), dontSendNotification);
        
        val = mod->getParam(TuningFundamental);
        fundamentalCB.setSelectedItemIndex(val.getIntValue(), dontSendNotification);
        //                       fundamentalCB.setSelectedItemIndex(prep->getFundamental(), dontSendNotification);
        
        val = mod->getParam(TuningOffset);
        offsetSlider->setValue(val.getFloatValue() * 100., dontSendNotification);
        //                       offsetSlider->setValue(prep->getFundamentalOffset() * 100., dontSendNotification);
        
        val = mod->getParam(TuningAbsoluteOffsets);
        absoluteKeyboard.setValues(stringToFloatArray(val));
        //                       absoluteKeyboard.setValues(prep->getAbsoluteOffsetsCents());
        
        val = mod->getParam(TuningCustomScale);
        customKeyboard.setValues(stringToFloatArray(val));
        
        val = mod->getParam(TuningA1IntervalScale);
        A1IntervalScaleCB.setSelectedItemIndex(val.getIntValue(), dontSendNotification);
        //                       A1IntervalScaleCB.setSelectedItemIndex(prep->getAdaptiveIntervalScale(), dontSendNotification);
        
        val = mod->getParam(TuningA1Inversional);
        A1Inversional.setToggleState((bool)val.getIntValue(), dontSendNotification);
        //                       A1Inversional.setToggleState(prep->getAdaptiveInversional(), dontSendNotification);
        
        val = mod->getParam(TuningA1AnchorScale);
        A1AnchorScaleCB.setSelectedItemIndex(val.getIntValue(), dontSendNotification);
        //                       A1AnchorScaleCB.setSelectedItemIndex(prep->getAdaptiveAnchorScale(), dontSendNotification);
        
        val = mod->getParam(TuningA1AnchorFundamental);
        A1FundamentalCB.setSelectedItemIndex(val.getIntValue(), dontSendNotification);
        //                       A1FundamentalCB.setSelectedItemIndex(prep->getAdaptiveAnchorFundamental(), dontSendNotification);
        
        val = mod->getParam(TuningA1ClusterThresh);
        A1ClusterThresh->setValue(val.getLargeIntValue(), dontSendNotification);
        //                       A1ClusterThresh->setValue(prep->getAdaptiveClusterThresh(), dontSendNotification);
        
        val = mod->getParam(TuningA1History);
        A1ClusterMax->setValue(val.getIntValue(), dontSendNotification);
        //                       A1ClusterMax->setValue(prep->getAdaptiveHistory(), dontSendNotification);
        
        updateComponentVisibility();
    }
    
}

void TuningModificationEditor::fillSelectCB(void)
{
    selectCB.clear(dontSendNotification);
    
    Array<int> index = processor.gallery->getIndexList(PreparationTypeTuningMod);
    
    for (int i = 0; i < index.size(); i++)
    {
        int Id = index[i];
        String name = processor.gallery->getTuningModPreparation(Id)->getName();
        if (name != String::empty)  selectCB.addItem(name, i+1);
        else                        selectCB.addItem(String(i+1), i+1);
        
        selectCB.setItemEnabled(i+1, true);
        if (processor.updateState->isActive(PreparationTypeTuningMod, Id) &&
            (Id != processor.updateState->currentModTuningId))
        {
            selectCB.setItemEnabled(i+1, false);
        }
    }
    
    selectCB.addSeparator();
    selectCB.addItem("New tuning modification...", index.size()+1);
    
    int currentId = processor.updateState->currentModTuningId;
    
    selectCB.setSelectedItemIndex(processor.gallery->getIndexFromId(PreparationTypeTuningMod, currentId), NotificationType::dontSendNotification);
    
}

void TuningModificationEditor::bkComboBoxDidChange (ComboBox* box)
{
    String name = box->getName();
    
    TuningModPreparation::Ptr mod = processor.gallery->getTuningModPreparation(processor.updateState->currentModTuningId);
    
    if (name == selectCB.getName())
    {
        int index = box->getSelectedItemIndex();
        
        int oldId = processor.updateState->currentModTuningId;
        int newId = processor.gallery->getIdFromIndex(PreparationTypeTuningMod, index);
        
        if (index == selectCB.getNumItems()-1)
        {
            processor.gallery->addTuningMod();
            
            processor.gallery->setEditted(PreparationTypeTuningMod, oldId, true);
            
            processor.gallery->getTuningModPreparations().getLast()->editted = true;
            
            newId = processor.gallery->getTuningModPreparations().getLast()->getId();
        }
        
        processor.updateState->currentModTuningId = newId;
        
        processor.updateState->removeActive(PreparationTypeTuningMod, oldId);
        
        if (!processor.gallery->getTuningModPreparation(oldId)->editted)
        {
            processor.updateState->removePreparation(PreparationTypeTuningMod, oldId);
            
            processor.gallery->remove(PreparationTypeTuningMod, oldId);
        }
        
        processor.updateState->addActive(PreparationTypeTuningMod, newId);
        
        processor.updateState->idDidChange = true;
        
        fillSelectCB();
    }
    else if (name == scaleCB.getName())
    {
        mod->setParam(TuningScale, String(scaleCB.getSelectedItemIndex()));
        scaleCB.setAlpha(1.);
        
        Tuning::Ptr currentTuning = processor.gallery->getTuning(processor.updateState->currentTuningId);
        customKeyboard.setValues(currentTuning->getCurrentScaleCents());
    }
    else if (name == fundamentalCB.getName())
    {
        mod->setParam(TuningFundamental, String(fundamentalCB.getSelectedItemIndex()));
        fundamentalCB.setAlpha(1.);
        
        customKeyboard.setFundamental(fundamentalCB.getSelectedItemIndex());
    }
    else if (name == A1IntervalScaleCB.getName())
    {
        mod->setParam(TuningA1IntervalScale, String(A1IntervalScaleCB.getSelectedItemIndex()));
        A1IntervalScaleCB.setAlpha(1.);
        A1IntervalScaleLabel.setAlpha(1);
    }
    else if (name == A1AnchorScaleCB.getName())
    {
        mod->setParam(TuningA1AnchorScale, String(A1AnchorScaleCB.getSelectedItemIndex()));
        A1AnchorScaleCB.setAlpha(1.);
        A1AnchorScaleLabel.setAlpha(1);
    }
    else if (name == A1FundamentalCB.getName())
    {
        mod->setParam(TuningA1AnchorFundamental, String(A1FundamentalCB.getSelectedItemIndex()));
        A1FundamentalCB.setAlpha(1.);
    }
    
    if (name != selectCB.getName()) updateModification();
    
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
        absoluteKeyboard.setAlpha(1.);
        
    }
    else if(name == customKeyboard.getName())
    {
        scaleCB.setSelectedItemIndex(customIndex, dontSendNotification);
        
        mod->setParam(TuningCustomScale, floatArrayToString(values));
        mod->setParam(TuningScale, String(customIndex));
        customKeyboard.setAlpha(1.);
    }
    
    updateModification();
}

void TuningModificationEditor::BKSingleSliderValueChanged(String name, double val)
{
    TuningModPreparation::Ptr mod = processor.gallery->getTuningModPreparation(processor.updateState->currentModTuningId);
    
    if(name == offsetSlider->getName())
    {
        mod->setParam(TuningOffset, String(val * 0.01));
        offsetSlider->setAlpha(1.);
    }
    else if(name == A1ClusterThresh->getName())
    {
        mod->setParam(TuningA1ClusterThresh, String(val));
        A1ClusterThresh->setAlpha(1.);
    }
    else if(name == A1ClusterMax->getName())
    {
        mod->setParam(TuningA1History, String(val));
        A1ClusterMax->setAlpha(1.);
    }
    
    updateModification();
}

void TuningModificationEditor::updateModification(void)
{
    TuningModPreparation::Ptr mod = processor.gallery->getTuningModPreparation(processor.updateState->currentModTuningId);
    mod->editted = true;
    
    processor.updateState->modificationDidChange = true;
}

void TuningModificationEditor::buttonClicked (Button* b)
{
    TuningModPreparation::Ptr mod = processor.gallery->getTuningModPreparation(processor.updateState->currentModTuningId);
    
    if (b == &A1Inversional)
    {
        mod->setParam(TuningA1Inversional, String(A1Inversional.getToggleState()));
        A1Inversional.setAlpha(1.);
    }
    else if (b == &A1reset)
    {
        // reset mod
    }
    else if (b == &hideOrShow)
    {
        processor.updateState->setCurrentDisplay(DisplayNil);
    }
    
    updateModification();
}






