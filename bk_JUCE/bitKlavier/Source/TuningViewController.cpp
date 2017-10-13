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
    
    //A1reset.setButtonText("reset");
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
    
    addAndMakeVisible(actionButton);
    actionButton.setButtonText("Action");
    actionButton.addListener(this);
}

void TuningViewController::resized()
{
    Rectangle<int> area (getLocalBounds());
    
    float paddingScalarX = (float)(getTopLevelComponent()->getWidth() - processor.uiMinWidth) / (processor.uiWidth - processor.uiMinWidth);
    float paddingScalarY = (float)(getTopLevelComponent()->getHeight() - processor.uiMinHeight) / (processor.uiHeight - processor.uiMinHeight);
    
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
    
    actionButton.setBounds(selectCB.getRight()+gXSpacing,
                           selectCB.getY(),
                           selectCB.getWidth() * 0.5,
                           selectCB.getHeight());
    
    comboBoxSlice.removeFromLeft(gXSpacing);
    A1reset.setBounds(comboBoxSlice.removeFromLeft(90));
    
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
    fillSelectCB(-1,-1);
    
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
    if (processor.updateState->currentDisplay == DisplayTuning)
    {
        TuningProcessor::Ptr tProcessor = processor.currentPiano->getTuningProcessor(processor.updateState->currentTuningId);
        
        if (tProcessor != nullptr)
        {
            if (tProcessor->getLastNoteTuning() != lastNoteTuningSave)
            {
                lastNoteTuningSave = tProcessor->getLastNoteTuning();
                lastNote.setText("last note: " + String(lastNoteTuningSave), dontSendNotification);
                lastInterval.setText("last interval: "  + String(tProcessor->getLastIntervalTuning()), dontSendNotification);
            }
        }
        
    }
}

int TuningPreparationEditor::addPreparation(void)
{
    processor.gallery->add(PreparationTypeTuning);
    
    return processor.gallery->getAllTuning().getLast()->getId();
}

int TuningPreparationEditor::duplicatePreparation(void)
{
    processor.gallery->duplicate(PreparationTypeTuning, processor.updateState->currentTuningId);
    
    return processor.gallery->getAllTuning().getLast()->getId();
}

void TuningPreparationEditor::deleteCurrent(void)
{
    int TuningId = selectCB.getSelectedId();
    int index = selectCB.getSelectedItemIndex();
    
    if ((index == 0) && (selectCB.getItemId(index+1) == -1)) return;
    
    processor.gallery->remove(PreparationTypeTuning, TuningId);
    
    fillSelectCB(0, 0);
    
    int newId = 0;
    
    selectCB.setSelectedId(newId, dontSendNotification);
    
    processor.updateState->currentTuningId = -1;
}

void TuningPreparationEditor::setCurrentId(int Id)
{
    processor.updateState->currentTuningId = Id;
    
    processor.updateState->idDidChange = true;
    
    update();
    
    fillSelectCB(lastId, Id);
    
    lastId = Id;
}

void TuningPreparationEditor::actionButtonCallback(int action, TuningPreparationEditor* vc)
{
    BKAudioProcessor& processor = vc->processor;
    if (action == 1)
    {
        int Id = vc->addPreparation();
        vc->setCurrentId(Id);
    }
    else if (action == 2)
    {
        int Id = vc->duplicatePreparation();
        vc->setCurrentId(Id);
    }
    else if (action == 3)
    {
        vc->deleteCurrent();
    }
    else if (action == 4)
    {
        processor.reset(PreparationTypeTuning, processor.updateState->currentTuningId);
        vc->update();
    }
    else if (action == 5)
    {
        processor.clear(PreparationTypeTuning, processor.updateState->currentTuningId);
        vc->update();
    }
}


void TuningPreparationEditor::bkComboBoxDidChange (ComboBox* box)
{
    String name = box->getName();
    int index = box->getSelectedItemIndex();
    int Id = box->getSelectedId();
    
    TuningPreparation::Ptr prep = processor.gallery->getStaticTuningPreparation(processor.updateState->currentTuningId);
    TuningPreparation::Ptr active = processor.gallery->getActiveTuningPreparation(processor.updateState->currentTuningId);
    
    if (name == selectCB.getName())
    {
        setCurrentId(Id);
    }
    else if (name == scaleCB.getName())
    {
        prep->setTuning((TuningSystem) index);
        active->setTuning((TuningSystem) index);
        
        DBG("setting tungin to :" + String(index));
        
        Tuning::Ptr currentTuning = processor.gallery->getTuning(processor.updateState->currentTuningId);
        DBG("current tuning from processor = " + String(processor.updateState->currentTuningId));
        customKeyboard.setValues(currentTuning->getCurrentScaleCents());
        
        updateComponentVisibility();
        
    }
    else if (name == fundamentalCB.getName())
    {
        prep->setFundamental((PitchClass) index);
        active->setFundamental((PitchClass) index);
        
        customKeyboard.setFundamental(index);
        
        updateComponentVisibility();
        
    }
    else if (name == A1IntervalScaleCB.getName())
    {
        prep->setAdaptiveIntervalScale((TuningSystem) index);
        active->setAdaptiveIntervalScale((TuningSystem) index);
        
        updateComponentVisibility();
    }
    else if (name == A1AnchorScaleCB.getName())
    {
        prep->setAdaptiveAnchorScale((TuningSystem) index);
        active->setAdaptiveAnchorScale((TuningSystem) index);
        
        updateComponentVisibility();
    }
    else if (name == A1FundamentalCB.getName())
    {
        prep->setAdaptiveAnchorFundamental((PitchClass) index);
        active->setAdaptiveAnchorFundamental((PitchClass) index);
        
        updateComponentVisibility();
        
    }
    
    processor.gallery->setGalleryDirty(true);
}


void TuningPreparationEditor::fillSelectCB(int last, int current)
{
    selectCB.clear(dontSendNotification);
    
    for (auto prep : processor.gallery->getAllTuning())
    {
        int Id = prep->getId();
        
        if (Id == -1) continue;
        
        String name = prep->getName();
        
        if (name != String::empty)  selectCB.addItem(name, Id);
        else                        selectCB.addItem("Tuning"+String(Id), Id);
        
        selectCB.setItemEnabled(Id, true);
        if (processor.currentPiano->isActive(PreparationTypeTuning, Id))
            selectCB.setItemEnabled(Id, false);
    }
    
    if (last != 0)      selectCB.setItemEnabled(last, true);
    if (current != 0)   selectCB.setItemEnabled(current, false);
    
    int selectedId = processor.updateState->currentTuningId;
    
    selectCB.setSelectedId(selectedId, NotificationType::dontSendNotification);
    
    selectCB.setItemEnabled(selectedId, false);

    lastId = selectedId;
}


void TuningPreparationEditor::BKEditableComboBoxChanged(String name, BKEditableComboBox* cb)
{
    Tuning::Ptr tuning = processor.gallery->getTuning(processor.updateState->currentTuningId);

    tuning->setName(name);
    
    processor.gallery->setGalleryDirty(true);
}


void TuningPreparationEditor::update(void)
{
    if (processor.updateState->currentTuningId < 0) return;
    
    TuningPreparation::Ptr prep = processor.gallery->getActiveTuningPreparation(processor.updateState->currentTuningId);
    
    if (prep != nullptr)
    {
        selectCB.setSelectedId(processor.updateState->currentTuningId, dontSendNotification);
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
        
        prep->setTuning((TuningSystem)customIndex);
        active->setTuning((TuningSystem)customIndex);
        
        DBG("keyboardSliderChanged values.size() = " + String(values.size()));
        prep->setCustomScaleCents(values);
        active->setCustomScaleCents(values);
    }
    
    processor.gallery->setGalleryDirty(true);
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
    
    processor.gallery->setGalleryDirty(true);
}

void TuningPreparationEditor::buttonClicked (Button* b)
{
    if (b == &A1Inversional)
    {
        DBG("setting A1Inversional " + String((int)A1Inversional.getToggleState()));
        
        TuningPreparation::Ptr prep = processor.gallery->getStaticTuningPreparation(processor.updateState->currentTuningId);
        TuningPreparation::Ptr active = processor.gallery->getActiveTuningPreparation(processor.updateState->currentTuningId);

        prep->setAdaptiveInversional(A1Inversional.getToggleState());
        active->setAdaptiveInversional(A1Inversional.getToggleState());
        
        processor.gallery->setGalleryDirty(true);
    }
    else if (b == &A1reset)
    {
        DBG("resetting A1");
        
        TuningProcessor::Ptr tProcessor = processor.currentPiano->getTuningProcessor(processor.updateState->currentTuningId);
        tProcessor->adaptiveReset();
    }
    else if (b == &hideOrShow)
    {
        processor.updateState->setCurrentDisplay(DisplayNil);
    }
    else if (b == &actionButton)
    {
        getPrepOptionMenu().showMenuAsync (PopupMenu::Options().withTargetComponent (&actionButton), ModalCallbackFunction::forComponent (actionButtonCallback, this) );
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
    
    fillSelectCB(-1,-1);
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
    A1ClusterThresh->setDim(gModAlpha);
    A1ClusterMax->setDim(gModAlpha);
    absoluteKeyboard.setAlpha(gModAlpha);
    customKeyboard.setAlpha(gModAlpha);
    offsetSlider->setDim(gModAlpha);
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
    if(mod->getParam(TuningA1ClusterThresh) != "")      A1ClusterThresh->setBright();;
    if(mod->getParam(TuningA1History) != "")            A1ClusterMax->setBright();;
    if(mod->getParam(TuningAbsoluteOffsets) != "")      absoluteKeyboard.setAlpha(1);
    if(mod->getParam(TuningCustomScale) != "")          customKeyboard.setAlpha(1);
    if(mod->getParam(TuningOffset) != "")               offsetSlider->setBright();
}

void TuningModificationEditor::update(void)
{
    TuningModPreparation::Ptr mod = processor.gallery->getTuningModPreparation(processor.updateState->currentModTuningId);
    
    if (mod != nullptr)
    {
        
        greyOutAllComponents();
        highlightModedComponents();
        
        selectCB.setSelectedId(processor.updateState->currentModTuningId, dontSendNotification);
        
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
        A1reset.setVisible(false);
    }
    
}

void TuningModificationEditor::fillSelectCB(int last, int current)
{
    selectCB.clear(dontSendNotification);
    
    for (auto prep : processor.gallery->getTuningModPreparations())
    {
        int Id = prep->getId();;
        String name = prep->getName();
        
        if (name != String::empty)  selectCB.addItem(name, Id);
        else                        selectCB.addItem("TuningMod"+String(Id), Id);
        
        selectCB.setItemEnabled(Id, true);
        if (processor.currentPiano->isActive(PreparationTypeTuning, Id))
            selectCB.setItemEnabled(Id, false);
    }
    
    if (last != 0)      selectCB.setItemEnabled(last, true);
    if (current != 0)   selectCB.setItemEnabled(current, false);
    
    int selectedId = processor.updateState->currentTuningId;
    
    selectCB.setSelectedId(selectedId, NotificationType::dontSendNotification);
    
    selectCB.setItemEnabled(selectedId, false);
    
    lastId = selectedId;
}

int TuningModificationEditor::addPreparation(void)
{
    processor.gallery->add(PreparationTypeTuningMod);
    
    return processor.gallery->getTuningModPreparations().getLast()->getId();
}

int TuningModificationEditor::duplicatePreparation(void)
{
    processor.gallery->duplicate(PreparationTypeTuningMod, processor.updateState->currentModTuningId);
    
    return processor.gallery->getTuningModPreparations().getLast()->getId();
}

void TuningModificationEditor::deleteCurrent(void)
{
    int oldId = selectCB.getSelectedId();
    int index = selectCB.getSelectedItemIndex();
    
    if ((index == 0) && (selectCB.getItemId(index+1) == -1)) return;
    
    processor.gallery->remove(PreparationTypeTuningMod, oldId);
    
    fillSelectCB(0, 0);
    
    int newId = 0;
    
    selectCB.setSelectedId(newId, dontSendNotification);
    
    processor.updateState->currentModTuningId = -1;
}

void TuningModificationEditor::setCurrentId(int Id)
{
    processor.updateState->currentModTuningId = Id;
    
    processor.updateState->idDidChange = true;
    
    update();
    
    fillSelectCB(lastId, Id);
    
    lastId = Id;
}

void TuningModificationEditor::actionButtonCallback(int action, TuningModificationEditor* vc)
{
    BKAudioProcessor& processor = vc->processor;
    
    if (action == 1)
    {
        int Id = vc->addPreparation();
        vc->setCurrentId(Id);
    }
    else if (action == 2)
    {
        int Id = vc->duplicatePreparation();
        vc->setCurrentId(Id);
    }
    else if (action == 3)
    {
        vc->deleteCurrent();
    }
    else if (action == 5)
    {
        processor.clear(PreparationTypeTuningMod, processor.updateState->currentModTuningId);
        vc->update();
        vc->updateModification();
    }
}

void TuningModificationEditor::bkComboBoxDidChange (ComboBox* box)
{
    String name = box->getName();
    int index = box->getSelectedItemIndex();
    int Id = box->getSelectedId();
    
    TuningModPreparation::Ptr mod = processor.gallery->getTuningModPreparation(processor.updateState->currentModTuningId);
    
    if (name == selectCB.getName())
    {
        setCurrentId(Id);
    }
    else if (name == scaleCB.getName())
    {
        mod->setParam(TuningScale, String(index));
        scaleCB.setAlpha(1.);
        
        Tuning::Ptr currentTuning = processor.gallery->getTuning(processor.updateState->currentTuningId);
        customKeyboard.setValues(currentTuning->getCurrentScaleCents());
    }
    else if (name == fundamentalCB.getName())
    {
        mod->setParam(TuningFundamental, String(index));
        fundamentalCB.setAlpha(1.);
        
        customKeyboard.setFundamental(index);
    }
    else if (name == A1IntervalScaleCB.getName())
    {
        mod->setParam(TuningA1IntervalScale, String(index));
        A1IntervalScaleCB.setAlpha(1.);
        A1IntervalScaleLabel.setAlpha(1);
    }
    else if (name == A1AnchorScaleCB.getName())
    {
        mod->setParam(TuningA1AnchorScale, String(index));
        A1AnchorScaleCB.setAlpha(1.);
        A1AnchorScaleLabel.setAlpha(1);
    }
    else if (name == A1FundamentalCB.getName())
    {
        mod->setParam(TuningA1AnchorFundamental, String(index));
        A1FundamentalCB.setAlpha(1.);
    }
    
    if (name != selectCB.getName()) updateModification();
    
    updateComponentVisibility();
    A1reset.setVisible(false);
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
        offsetSlider->setBright();
    }
    else if(name == A1ClusterThresh->getName())
    {
        mod->setParam(TuningA1ClusterThresh, String(val));
        A1ClusterThresh->setBright();
    }
    else if(name == A1ClusterMax->getName())
    {
        mod->setParam(TuningA1History, String(val));
        A1ClusterMax->setBright();
    }
    
    updateModification();
}

void TuningModificationEditor::updateModification(void)
{
    processor.updateState->modificationDidChange = true;
}

void TuningModificationEditor::buttonClicked (Button* b)
{
    TuningModPreparation::Ptr mod = processor.gallery->getTuningModPreparation(processor.updateState->currentModTuningId);
    
    if (b == &A1Inversional)
    {
        mod->setParam(TuningA1Inversional, String((int)A1Inversional.getToggleState()));
        A1Inversional.setAlpha(1.);
    }
    else if (b == &A1reset)
    {
        // N/A in mod
    }
    else if (b == &hideOrShow)
    {
        processor.updateState->setCurrentDisplay(DisplayNil);
    }
    else if (b == &actionButton)
    {
        getModOptionMenu().showMenuAsync (PopupMenu::Options().withTargetComponent (&actionButton), ModalCallbackFunction::forComponent (actionButtonCallback, this) );
    }
    
    updateModification();
}






