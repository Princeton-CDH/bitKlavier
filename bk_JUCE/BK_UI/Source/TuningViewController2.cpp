/*
  ==============================================================================

    TuningViewController2.cpp
    Created: 10 Jun 2017 11:24:22am
    Author:  Daniel Trueman

  ==============================================================================
*/

#include "TuningViewController2.h"

/*
 TODO
 
 => cleanup: both this, and the keyboard and keyboardslider classes....
 
 */


//==============================================================================
TuningViewController2::TuningViewController2(BKAudioProcessor& p, BKItemGraph* theGraph):
processor(p),
theGraph(theGraph)
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
    selectCB.addMyListener(this);
    fillSelectCB();
    addAndMakeVisible(selectCB);
    
    scaleCB.setName("Scale");
    scaleCB.addListener(this);
    addAndMakeVisible(scaleCB);
    
    scaleLabel.setText("Scale", dontSendNotification);
    //addAndMakeVisible(scaleLabel);
    
    fundamentalCB.setName("Fundamental");
    fundamentalCB.addListener(this);
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
    
    A1Inversional.addListener(this);
    A1Inversional.setButtonText ("invert");
    A1Inversional.setToggleState (true, dontSendNotification);
    //buttonsAndMenusLAF.setToggleBoxTextToRightBool(false);
    A1Inversional.setColour(ToggleButton::textColourId, Colours::white);
    A1Inversional.setColour(ToggleButton::tickColourId, Colours::white);
    A1Inversional.setColour(ToggleButton::tickDisabledColourId, Colours::white);
    addAndMakeVisible(A1Inversional);
    
    A1AnchorScaleCB.setName("A1AnchorScale");
    A1AnchorScaleCB.addListener(this);
    addAndMakeVisible(A1AnchorScaleCB);
    
    A1AnchorScaleLabel.setText("Anchor Scale:", dontSendNotification);
    //A1AnchorScaleLabel.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(A1AnchorScaleLabel);
    
    A1FundamentalCB.setName("A1Fundamental");
    A1FundamentalCB.addListener(this);
    addAndMakeVisible(A1FundamentalCB);
    
    //A1FundamentalLabel.setText("Adaptive 1 Anchor Fundamental", dontSendNotification);
    //addAndMakeVisible(A1FundamentalLabel);
    
    A1ClusterThresh = new BKSingleSlider("Cluster Threshold", 1, 1000, 0, 1);
    A1ClusterThresh->setJustifyRight(false);
    A1ClusterThresh->addMyListener(this);
    addAndMakeVisible(A1ClusterThresh);
    
    A1ClusterMax = new BKSingleSlider("Cluster Maximum", 1, 8, 1, 1);
    A1ClusterMax->setJustifyRight(false);
    A1ClusterMax->addMyListener(this);
    addAndMakeVisible(A1ClusterMax);
    
    A1reset.addListener(this);
    A1reset.setButtonText("reset");
    addAndMakeVisible(A1reset);
    
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
    customKeyboard.setName("Temperament Offsets");
    customKeyboard.addMyListener(this);
    //customKeyboard.setAvailableRange(60, 71);
    customKeyboard.setAvailableRange(0, 11);
    customKeyboard.useOrderedPairs(false);
    customKeyboard.setFundamental(0);
    addAndMakeVisible(customKeyboard);
    
    offsetSlider = new BKSingleSlider("Global Offset (cents)", -100, 100, 0, 0.1);
    offsetSlider->addMyListener(this);
    addAndMakeVisible(offsetSlider);
    
    lastNote.setText("last note: ", dontSendNotification);
    lastInterval.setText("last interval: ", dontSendNotification);
    lastInterval.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(lastNote);
    addAndMakeVisible(lastInterval);
    
    addAndMakeVisible(hideOrShow);
    hideOrShow.setName("hideOrShow");
    hideOrShow.addListener(this);
    hideOrShow.setButtonText(" X ");
    
    startTimer(50);

    updateFields();
}

void TuningViewController2::resized()
{
    Rectangle<int> area (getLocalBounds());
    
    float paddingScalarX = (float)(getTopLevelComponent()->getWidth() - gMainComponentMinWidth) / (gMainComponentWidth - gMainComponentMinWidth);
    float paddingScalarY = (float)(getTopLevelComponent()->getHeight() - gMainComponentMinHeight) / (gMainComponentHeight - gMainComponentMinHeight);
    
    iconImageComponent.setBounds(area);
    area.reduce(10 * paddingScalarX + 4, 10 * paddingScalarY + 4);
    
    float keyboardHeight = 60 + 50 * paddingScalarY;
    Rectangle<int> absoluteKeymapRow = area.removeFromBottom(keyboardHeight);
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


void TuningViewController2::paint (Graphics& g)
{
    //g.fillAll(Colours::lightgrey);
    //g.fillAll(Colours::transparentWhite);
    g.fillAll(Colours::black);
}

void TuningViewController2::timerCallback()
{
    TuningProcessor::Ptr tProcessor = processor.gallery->getTuningProcessor(processor.updateState->currentTuningId);
    if(tProcessor->getLastNoteTuning() != lastNoteTuningSave)
    {
        lastNoteTuningSave = tProcessor->getLastNoteTuning();
        lastNote.setText("last note: " + String(lastNoteTuningSave, 3), dontSendNotification);
        lastInterval.setText("last interval: "  + String(tProcessor->getLastIntervalTuning(), 3), dontSendNotification);
    }
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
                
        if(name == "Custom") {
            customIndex = i;
            DBG("assigned to customIndex " + String(customIndex));
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
    
    if (name == selectCB.getName())
    {
        processor.updateState->currentTuningId = box->getSelectedItemIndex();
        
        processor.updateState->idDidChange = true;
        
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

void TuningViewController2::updateComponentVisibility()
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

void TuningViewController2::BKEditableComboBoxChanged(String name, BKEditableComboBox* cb)
{
    processor.gallery->getTuning(processor.updateState->currentTuningId)->setName(name);
}


void TuningViewController2::updateFields(void)
{
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
        scaleCB.setSelectedItemIndex(customIndex, dontSendNotification);

        prep->setCustomScaleCents(values);
        active->setCustomScaleCents(values);
        
        prep->setTuning((TuningSystem)customIndex);
        active->setTuning((TuningSystem)customIndex);
    }
}

void TuningViewController2::BKSingleSliderValueChanged(String name, double val)
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





