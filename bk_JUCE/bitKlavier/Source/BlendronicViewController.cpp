/*
  ==============================================================================

    BlendronicViewController.cpp
    Created: 6 Sep 2019 3:15:24pm
    Author:  Matthew Wang

  ==============================================================================
*/

#include "BlendronicViewController.h"

BlendronicViewController::BlendronicViewController(BKAudioProcessor& p, BKItemGraph* theGraph):
BKViewController(p, theGraph, 3)
{
    
    setLookAndFeel(&buttonsAndMenusLAF);
    
    iconImageComponent.setImage(ImageCache::getFromMemory(BinaryData::blendronic_icon_png, BinaryData::blendronic_icon_pngSize));
    iconImageComponent.setImagePlacement(RectanglePlacement(juce::RectanglePlacement::stretchToFit));
    iconImageComponent.setAlpha(0.095);
    //addAndMakeVisible(iconImageComponent);
    
    // MultSliders
    paramSliders = OwnedArray<BKMultiSlider>();
    
    int idx = 0;
    for (int i = 0; i < cBlendronicParameterTypes.size(); i++)
    {
        if (cBlendronicDataTypes[i] == BKFloatArr || cBlendronicDataTypes[i] == BKArrFloatArr)
        {
            paramSliders.insert(idx, new BKMultiSlider(HorizontalMultiBarSlider));
            addAndMakeVisible(paramSliders[idx], ALL);
            paramSliders[idx]->setName(cBlendronicParameterTypes[idx+BlendronicBeats]);
            paramSliders[idx]->addMyListener(this);
#if JUCE_IOS
            paramSliders[idx]->addWantsBigOneListener(this);
#endif
            paramSliders[idx]->setMinMaxDefaultInc(cBlendronicDefaultRangeValuesAndInc[i]);
            
            if(paramSliders[idx]->getName() == "beat lengths") // shouldn't all these be referencing cBlendronicParameterTypes??
            {
                paramSliders[idx]->setToolTipString("Determines the beat pattern of changing sequenced parameters, as a multiplier of the pulse length set by Tempo; double-click to edit all or add additional sequence steps");
            }
            else if(paramSliders[idx]->getName() == "delay lengths")
            {
                paramSliders[idx]->setToolTipString("Determines the length of delay, as a multiplier of the pulse length set by Tempo; double-click to edit all or add additional sequence steps");
            }
            else if(paramSliders[idx]->getName() == "smoothing")
            {
                paramSliders[idx]->setToolTipString("Determines duration of smoothing between delay times; double-click to edit all or add additional sequence steps\nUnits: Constant and Proportional multiply the beat length by the given value to set the smooth time; Constant uses just the first beat, Proportional changes the smooth time with every beat, based on the length of that beat; Constant and Proportional Rate do the same, except they divide the beat length by the given value to set the smooth time.");
            }
            else if(paramSliders[idx]->getName() == "feedback coefficients")
            {
                paramSliders[idx]->setToolTipString("Determines delay feedback; double-click to edit all or add additional sequence steps");
            }
            
            idx++;
        }
        
    }
    
    selectCB.setName("Blendronic");
    selectCB.setTooltip("Select from available saved preparation settings");
    selectCB.addSeparator();
    selectCB.addListener(this);
    selectCB.setSelectedItemIndex(0);
    addAndMakeVisible(selectCB);
    
    pulseBeatSmooth.setName("PulseBeatSmooth");
    pulseBeatSmooth.setButtonText("pulse length");
    pulseBeatSmooth.setTooltip("Set base for smoothing time");
    pulseBeatSmooth.addListener(this);
    addAndMakeVisible(pulseBeatSmooth);
    
    constantFullSmooth.setName("MultipleDivideSmooth");
    constantFullSmooth.setButtonText("full delay change");
    constantFullSmooth.setTooltip("Set operation of the smoothing value");
    constantFullSmooth.addListener(this);
    addAndMakeVisible(constantFullSmooth);
    
    smoothTimeLabel.setText("Smooth time across a", dontSendNotification);
    smoothTimeLabel.setTooltip("Set meaning for smoothing time");
    addAndMakeVisible(&smoothTimeLabel, ALL);
    
    smoothEqualLabel.setText("equals", dontSendNotification);
    smoothEqualLabel.setTooltip("Set meaning of the smoothing value");
    addAndMakeVisible(&smoothEqualLabel, ALL);

    smoothValueLabel.setText("times smoothing value.", dontSendNotification);
    smoothValueLabel.setTooltip("Set meaning of the smoothing value");
    addAndMakeVisible(&smoothValueLabel, ALL);
    
//    smoothModeSelectCB.setName("Smooth Mode");
//    smoothModeSelectCB.setTooltip("Determines whether smooth durations are constant or proportional to beat length");
//    smoothModeSelectCB.addSeparator();
//    smoothModeSelectCB.setLookAndFeel(&comboBoxRightJustifyLAF);
//    comboBoxRightJustifyLAF.setComboBoxJustificationType(juce::Justification::centredRight);
//    smoothModeSelectCB.addListener(this);
//    smoothModeSelectCB.setSelectedItemIndex(0);
//    addAndMakeVisible(smoothModeSelectCB);
//
//    smoothModeLabel.setText("smoothing", dontSendNotification);
//    smoothModeLabel.setTooltip("Determines whether smooth durations are constant or proportional to beat length and delta");
//    addAndMakeVisible(&smoothModeLabel, ALL);
//
//    syncModeSelectCB.setName("Sync Mode");
//    syncModeSelectCB.BKSetJustificationType(juce::Justification::centredRight);
//    syncModeSelectCB.setTooltip("Determines which aspect of MIDI signal triggers the Blendronic sequence");
//    syncModeSelectCB.addSeparator();
//    syncModeSelectCB.addListener(this);
//    syncModeSelectCB.setSelectedItemIndex(0);
//    addAndMakeVisible(syncModeSelectCB);
//
//    syncModeLabel.setText("pulse reset by", dontSendNotification);
//    syncModeLabel.setJustificationType(juce::Justification::centredRight);
//    syncModeLabel.setTooltip("Determines which aspect of MIDI signal resets the Blendronic sequence");
//    addAndMakeVisible(&syncModeLabel, ALL);
//
//    clearModeSelectCB.setName("Clear Mode");
//    clearModeSelectCB.setTooltip("Determines which aspect of MIDI signal clears the delay line");
//    clearModeSelectCB.addSeparator();
//    clearModeSelectCB.setLookAndFeel(&comboBoxRightJustifyLAF);
//    comboBoxRightJustifyLAF.setComboBoxJustificationType(juce::Justification::centredRight);
//    clearModeSelectCB.addListener(this);
//    clearModeSelectCB.setSelectedItemIndex(0);
//    addAndMakeVisible(clearModeSelectCB);
//
//    clearModeLabel.setText("clears the delay line", dontSendNotification);
//    clearModeLabel.setTooltip("Determines which aspect of MIDI signal clears the delay line");
//    addAndMakeVisible(&clearModeLabel, ALL);
//
//    closeModeSelectCB.setName("Close Mode");
//    closeModeSelectCB.BKSetJustificationType(juce::Justification::centredRight);
//    closeModeSelectCB.setTooltip("Determines which aspect of MIDI signal clears the delay line");
//    closeModeSelectCB.addSeparator();
//    closeModeSelectCB.addListener(this);
//    closeModeSelectCB.setSelectedItemIndex(0);
//    addAndMakeVisible(closeModeSelectCB);
//
//    closeModeLabel.setText("close the delay line on", dontSendNotification);
//    closeModeLabel.setJustificationType(juce::Justification::centredRight);
//    closeModeLabel.setTooltip("Determines which aspect of MIDI signal close the delay line");
//    addAndMakeVisible(&closeModeLabel, ALL);
//
//    openModeSelectCB.setName("Open Mode");
//    openModeSelectCB.setTooltip("Determines which aspect of MIDI signal opens the delay line");
//    openModeSelectCB.addSeparator();
//    openModeSelectCB.setLookAndFeel(&comboBoxRightJustifyLAF);
//    comboBoxRightJustifyLAF.setComboBoxJustificationType(juce::Justification::centredRight);
//    openModeSelectCB.addListener(this);
//    openModeSelectCB.setSelectedItemIndex(0);
//    addAndMakeVisible(openModeSelectCB);
//
//    openModeLabel.setText("opens the delay line", dontSendNotification);
//    openModeLabel.setTooltip("Determines which aspect of MIDI signal opens the delay line");
//    addAndMakeVisible(&openModeLabel, ALL);
    
    // Target Control CBs
    targetControlCBs = OwnedArray<BKComboBox>();
    for (int i=TargetTypeBlendronicSync; i<=TargetTypeBlendronicOpenCloseOutput; i++)
    {
        targetControlCBs.add(new BKComboBox()); // insert at the end of the array
        targetControlCBs.getLast()->setName(cKeymapTargetTypes[i]);
        targetControlCBs.getLast()->addListener(this);
        targetControlCBs.getLast()->setLookAndFeel(&comboBoxRightJustifyLAF);
        targetControlCBs.getLast()->addItem(cTargetNoteModes[TargetNoteMode::NoteOn], TargetNoteMode::NoteOn + 1);
        targetControlCBs.getLast()->addItem(cTargetNoteModes[TargetNoteMode::NoteOff], TargetNoteMode::NoteOff + 1);
        targetControlCBs.getLast()->addItem(cTargetNoteModes[TargetNoteMode::Both], TargetNoteMode::Both + 1);
        targetControlCBs.getLast()->setSelectedItemIndex(0, dontSendNotification);
        addAndMakeVisible(targetControlCBs.getLast(), ALL);
        
        targetControlCBLabels.add(new BKLabel());
        targetControlCBLabels.getLast()->setText(cKeymapTargetTypes[i], dontSendNotification);
        addAndMakeVisible(targetControlCBLabels.getLast(), ALL);
    }
    
    // border for Target Triggers
    targetControlsGroup.setName("targetGroup");
    targetControlsGroup.setText("Blendronic Target Triggers");
    targetControlsGroup.setTextLabelPosition(Justification::centred);
    targetControlsGroup.setAlpha(0.65);
    addAndMakeVisible(targetControlsGroup);
    
    addAndMakeVisible(&actionButton, ALL);
    actionButton.setButtonText("Action");
    actionButton.setTooltip("Create, duplicate, rename, delete, or reset current settings");
    actionButton.addListener(this);
    
    delayLineDisplay.setNumChannels(1);
    delayLineDisplay.setColours(Colours::lightgrey, Colours::black);
    addAndMakeVisible(&delayLineDisplay);

//    keyThreshSlider = std::make_unique< BKSingleSlider>("cluster threshold", 20, 2000, 200, 10);
//    keyThreshSlider->setToolTipString("successive notes spaced by less than this time (ms) are grouped for determining first note-on and first note-off");
//    keyThreshSlider->setJustifyRight(true);
//    addAndMakeVisible(*keyThreshSlider, ALL);
    
    currentTab = 0;
    displayTab(currentTab);
    
}

void BlendronicViewController::invisible(void)
{
    for (int i = 0; i < paramSliders.size(); i++)
    {
        paramSliders[i]->setVisible(false);
    }
    
    pulseBeatSmooth.setVisible(false);
    constantFullSmooth.setVisible(false);
    smoothTimeLabel.setVisible(false);
    smoothEqualLabel.setVisible(false);
    smoothValueLabel.setVisible(false);
//    smoothModeSelectCB.setVisible(false);
//    smoothModeLabel.setVisible(false);
//    syncModeSelectCB.setVisible(false);
//    syncModeLabel.setVisible(false);
//    clearModeSelectCB.setVisible(false);
//    clearModeLabel.setVisible(false);
//    closeModeSelectCB.setVisible(false);
//    closeModeLabel.setVisible(false);
//    openModeSelectCB.setVisible(false);
//    openModeLabel.setVisible(false);
    
    for (int i=0; i<targetControlCBs.size(); i++)
    {
        targetControlCBs[i]->setVisible(false);
        targetControlCBLabels[i]->setVisible(false);
    }
    targetControlsGroup.setVisible(false);
    
    delayLineDisplay.setVisible(false);
//    keyThreshSlider->setVisible(false);
}

void BlendronicViewController::displayShared(void)
{
    Rectangle<int> area (getBounds());
    
    iconImageComponent.setBounds(area);
    
    area.reduce(10 * processor.paddingScalarX + 4, 10 * processor.paddingScalarY + 4);
    
    Rectangle<int> leftColumn = area.removeFromLeft(area.getWidth() * 0.5);
    Rectangle<int> comboBoxSlice = leftColumn.removeFromTop(gComponentComboBoxHeight);
    comboBoxSlice.removeFromRight(4 + 2.0f * gPaddingConst * processor .paddingScalarX);
    comboBoxSlice.removeFromLeft(gXSpacing);
    hideOrShow.setBounds(comboBoxSlice.removeFromLeft(gComponentComboBoxHeight));
    comboBoxSlice.removeFromLeft(gXSpacing);
    selectCB.setBounds(comboBoxSlice.removeFromLeft(comboBoxSlice.getWidth() / 2.));
    
    actionButton.setBounds(selectCB.getRight()+gXSpacing,
                           selectCB.getY(),
                           selectCB.getWidth() * 0.5,
                           selectCB.getHeight());
    
    actionButton.toFront(false);
    
    leftArrow.setBounds (0, getHeight() * 0.4, 50, 50);
    rightArrow.setBounds (getRight() - 50, getHeight() * 0.4, 50, 50);
    
}

void BlendronicViewController::displayTab(int tab)
{
    currentTab = tab;
    
    invisible();
    displayShared();
    
//    int x0 = leftArrow.getRight() + gXSpacing;
    int y0 = hideOrShow.getBottom() + gYSpacing;
//    int right = rightArrow.getX() - gXSpacing;
//    int width = right - x0;
    int height = getHeight() - y0;
    
    if (tab == 0)
    {
        BlendronicPreparation::Ptr prep = processor.gallery->getStaticBlendronicPreparation(processor.updateState->currentBlendronicId);
    
//        smoothModeSelectCB.setVisible(true);
//        smoothModeLabel.setVisible(true);
        
        pulseBeatSmooth.setVisible(true);
        constantFullSmooth.setVisible(true);
        
        smoothTimeLabel.setVisible(true);
        smoothEqualLabel.setVisible(true);
        smoothValueLabel.setVisible(true);
        
        for (int i = 0; i < paramSliders.size(); i++)
        {
            paramSliders[i]->setVisible(true);
        }
        
        // SET BOUNDS
        int sliderHeight = height * 0.2f;
        
        Rectangle<int> area (getBounds());
        area.removeFromTop(hideOrShow.getBottom() + gYSpacing);
        area.removeFromRight(rightArrow.getWidth() + gXSpacing);
        area.removeFromLeft(leftArrow.getWidth() + gXSpacing);
        
        Rectangle<int> leftColumn (area);
        Rectangle<int> rightColumn (leftColumn.removeFromRight(leftColumn.getWidth()* 0.5));
        
        leftColumn.removeFromRight(processor.paddingScalarX * 20);
        rightColumn.removeFromLeft(processor.paddingScalarX * 20);
        
//        leftColumn.removeFromTop(sliderHeight * 0.5 - gComponentComboBoxHeight);
//        Rectangle<int> smoothModeSelectCBRect (leftColumn.removeFromTop(gComponentComboBoxHeight));
//        Rectangle<int> smoothModeLabelRect (smoothModeSelectCBRect.removeFromRight(smoothModeSelectCBRect.getWidth()*0.5));
//        smoothModeSelectCB.setBounds(smoothModeSelectCBRect);
//        smoothModeLabel.setBounds(smoothModeLabelRect);

        area.removeFromTop(gComponentComboBoxHeight);
        for (int i = 0; i < paramSliders.size(); i++)
        {
            paramSliders[i]->setBounds(area.removeFromTop(sliderHeight));
            if (i == 2)
            {
                Rectangle<int> smoothRect (area.removeFromTop(gComponentComboBoxHeight));
                smoothTimeLabel.setBounds(smoothRect.removeFromLeft(area.getWidth() * 0.2));
                constantFullSmooth.setBounds(smoothRect.removeFromLeft(area.getWidth() * 0.2));
                smoothEqualLabel.setBounds(smoothRect.removeFromLeft(area.getWidth() * 0.075));
                pulseBeatSmooth.setBounds(smoothRect.removeFromLeft(area.getWidth() * 0.15));
                smoothValueLabel.setBounds(smoothRect.removeFromLeft(area.getWidth() * 0.25));
                area.removeFromTop(gComponentComboBoxHeight*0.5);
            }
            area.removeFromTop(gYSpacing);
        }
    }
    else if (tab == 1)
    {
        BlendronicPreparation::Ptr prep = processor.gallery->getStaticBlendronicPreparation(processor.updateState->currentBlendronicId);
        
//        syncModeSelectCB.setVisible(true);
//        syncModeLabel.setVisible(true);
//        clearModeSelectCB.setVisible(true);
//        clearModeLabel.setVisible(true);
//        closeModeSelectCB.setVisible(true);
//        closeModeLabel.setVisible(true);
//        openModeSelectCB.setVisible(true);
//        openModeLabel.setVisible(true);
//        keyThreshSlider->setVisible(true);
        
        delayLineDisplay.setVisible(true);
        
        // SET BOUNDS
        int sliderHeight = height * 0.225f;
        
        Rectangle<int> area (getBounds());
        area.removeFromTop(hideOrShow.getBottom() + gYSpacing);
        area.removeFromRight(rightArrow.getWidth() + gXSpacing);
        area.removeFromLeft(leftArrow.getWidth() + gXSpacing);
        
//        area.removeFromTop(sliderHeight * 0.5 - gComponentComboBoxHeight);
//        Rectangle<int> keyThreshSliderRect (area);
//        keyThreshSlider->setBounds(keyThreshSliderRect.removeFromTop(sliderHeight));
//
//        area.removeFromTop(sliderHeight * 0.5 - gComponentComboBoxHeight);
//        Rectangle<int> leftColumn (area);
//        Rectangle<int> rightColumn (leftColumn.removeFromRight(leftColumn.getWidth()* 0.5));
//
//        leftColumn.removeFromRight(processor.paddingScalarX * 20);
//        rightColumn.removeFromLeft(processor.paddingScalarX * 20);
//
//        leftColumn.removeFromTop(sliderHeight * 0.5 - gComponentComboBoxHeight);
//        Rectangle<int> syncModeSelectCBRect (leftColumn.removeFromTop(gComponentComboBoxHeight));
//        Rectangle<int> syncModeSelectLabel (syncModeSelectCBRect.removeFromLeft(syncModeSelectCBRect.getWidth()*0.5));
//        syncModeSelectCB.setBounds(syncModeSelectCBRect);
//        syncModeLabel.setBounds(syncModeSelectLabel);
//
//        rightColumn.removeFromTop(sliderHeight * 0.5 - gComponentComboBoxHeight);
//        Rectangle<int> clearModeSelectCBRect (rightColumn.removeFromTop(gComponentComboBoxHeight));
//        Rectangle<int> clearModeLabelRect (clearModeSelectCBRect.removeFromRight(clearModeSelectCBRect.getWidth()*0.5));
//        clearModeSelectCB.setBounds(clearModeSelectCBRect);
//        clearModeLabel.setBounds(clearModeLabelRect);
//
//        leftColumn.removeFromTop(sliderHeight * 0.5 - gComponentComboBoxHeight);
//        Rectangle<int> closeModeSelectCBRect (leftColumn.removeFromTop(gComponentComboBoxHeight));
//        Rectangle<int> closeModeSelectLabel (closeModeSelectCBRect.removeFromLeft(closeModeSelectCBRect.getWidth()*0.5));
//        closeModeSelectCB.setBounds(closeModeSelectCBRect);
//        closeModeLabel.setBounds(closeModeSelectLabel);
//
//        rightColumn.removeFromTop(sliderHeight * 0.5 - gComponentComboBoxHeight);
//        Rectangle<int> openModeSelectCBRect (rightColumn.removeFromTop(gComponentComboBoxHeight));
//        Rectangle<int> openModeLabelRect (openModeSelectCBRect.removeFromRight(openModeSelectCBRect.getWidth()*0.5));
//        openModeSelectCB.setBounds(openModeSelectCBRect);
//        openModeLabel.setBounds(openModeLabelRect);
        
        area.removeFromTop(0.5*sliderHeight - gComponentComboBoxHeight);
        Rectangle<int> delayLineDisplayRect (area.removeFromTop(sliderHeight*4));
        delayLineDisplay.setBounds(delayLineDisplayRect);
    }
    else if (tab == 2) // keymap target tab
    {
        // make the combo boxes visible
        for (int i=0; i<targetControlCBs.size(); i++)
        {
            targetControlCBs[i]->setVisible(true);
            targetControlCBLabels[i]->setVisible(true);
        }
        targetControlsGroup.setVisible(true);
        
        Rectangle<int> area (getBounds());
        area.removeFromTop(selectCB.getHeight() + 50 * processor.paddingScalarY + 4 + gYSpacing);
        area.removeFromRight(rightArrow.getWidth());
        area.removeFromLeft(leftArrow.getWidth());
        
        Rectangle<int> leftColumn (area.removeFromLeft(area.getWidth() * 0.5));
        leftColumn.removeFromLeft(leftColumn.getWidth() * 0.5);
        
        leftColumn.removeFromRight(processor.paddingScalarX * 5);
        leftColumn.removeFromLeft(processor.paddingScalarX * 20);
        
        area.removeFromLeft(processor.paddingScalarX * 5); //area is now right column
        area.removeFromRight(processor.paddingScalarX * 20);
        
        int targetControlCBSection = (gComponentComboBoxHeight + gYSpacing) * targetControlCBs.size();
        leftColumn.removeFromTop((leftColumn.getHeight() - targetControlCBSection) / 3.);
        area.removeFromTop((area.getHeight() - targetControlCBSection) / 3.);
        
        for (int i=0; i<targetControlCBs.size(); i++)
        {
            targetControlCBs[i]->setBounds(leftColumn.removeFromTop(gComponentComboBoxHeight));
            leftColumn.removeFromTop(gYSpacing);
            
            targetControlCBLabels[i]->setBounds(area.removeFromTop(gComponentComboBoxHeight));
            area.removeFromTop(gYSpacing);
        }
        
        targetControlsGroup.setBounds(targetControlCBs[0]->getX() - 4 * gXSpacing,
                                      targetControlCBs[0]->getY() - gComponentComboBoxHeight - 2 * gXSpacing,
                                      targetControlCBs[0]->getWidth() * 2 + 8 * gXSpacing,
                                      targetControlCBs[0]->getHeight() * targetControlCBs.size() + 2 * gComponentComboBoxHeight + 4 * gYSpacing);
    }
}

void BlendronicViewController::paint (Graphics& g)
{
    g.fillAll(Colours::black);
}

void BlendronicViewController::resized()
{
    displayShared();
    displayTab(currentTab);
}

#if JUCE_IOS
void BlendronicViewController::iWantTheBigOne(TextEditor* tf, String name)
{
    hideOrShow.setAlwaysOnTop(false);
    rightArrow.setAlwaysOnTop(false);
    leftArrow.setAlwaysOnTop(false);
    bigOne.display(tf, name, getBounds());
}
#endif

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ BlendronicPreparationEditor ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ //

BlendronicPreparationEditor::BlendronicPreparationEditor(BKAudioProcessor& p, BKItemGraph* theGraph):
BlendronicViewController(p, theGraph)
{
    fillSelectCB(-1,-1);

    selectCB.addListener(this);
    selectCB.addMyListener(this);
//    smoothModeSelectCB.addListener(this);
//    syncModeSelectCB.addListener(this);
//    clearModeSelectCB.addListener(this);
//    openModeSelectCB.addListener(this);
//    closeModeSelectCB.addListener(this);
    
//    keyThreshSlider->addMyListener(this);
    
//    fillSmoothModeSelectCB();
//    fillSyncModeSelectCB();
//    fillClearModeSelectCB();
//    fillCloseModeSelectCB();
//    fillOpenModeSelectCB();
    
    startTimer(20);
}

void BlendronicPreparationEditor::BKEditableComboBoxChanged(String name, BKEditableComboBox* cb)
{
    Blendronic::Ptr blendronic = processor.gallery->getBlendronic(processor.updateState->currentBlendronicId);
    
    blendronic->setName(name);
}

void BlendronicPreparationEditor::update(void)
{
    if (processor.updateState->currentBlendronicId < 0) return;

    setSubWindowInFront(false);
    
    BlendronicPreparation::Ptr prep = processor.gallery->getActiveBlendronicPreparation(processor.updateState->currentBlendronicId);
    
    if (prep != nullptr)
    {
        selectCB.setSelectedId(processor.updateState->currentBlendronicId, dontSendNotification);
//        smoothModeSelectCB.setSelectedItemIndex(prep->getSmoothMode(), dontSendNotification);
//        syncModeSelectCB.setSelectedItemIndex(prep->getSyncMode(), dontSendNotification);
//        clearModeSelectCB.setSelectedItemIndex(prep->getClearMode(), dontSendNotification);
//        closeModeSelectCB.setSelectedItemIndex(prep->getCloseMode(), dontSendNotification);
//        openModeSelectCB.setSelectedItemIndex(prep->getOpenMode(), dontSendNotification);
//
        for (int i = TargetTypeBlendronicSync; i <= TargetTypeBlendronicOpenCloseOutput; i++)
        {
            targetControlCBs[i - TargetTypeBlendronicSync]->setSelectedItemIndex
            (prep->getTargetTypeBlendronic(KeymapTargetType(i)), dontSendNotification);
        }
        
//        keyThreshSlider->setValue(prep->getClusterThreshMS(), dontSendNotification);

        for(int i = 0; i < paramSliders.size(); i++)
        {
            if(!paramSliders[i]->getName().compare(cBlendronicParameterTypes[BlendronicBeats]))
            {
                paramSliders[i]->setTo(prep->getBeats(), dontSendNotification);
            }
            
            if(!paramSliders[i]->getName().compare(cBlendronicParameterTypes[BlendronicDelayLengths]))
            {
                paramSliders[i]->setTo(prep->getDelayLengths(), dontSendNotification);
            }

            if(!paramSliders[i]->getName().compare(cBlendronicParameterTypes[BlendronicSmoothValues]))
            {
                paramSliders[i]->setTo(prep->getSmoothValues(), dontSendNotification);
            }

            if(!paramSliders[i]->getName().compare(cBlendronicParameterTypes[BlendronicFeedbackCoeffs]))
            {
                paramSliders[i]->setTo(prep->getFeedbackCoefficients(), dontSendNotification);
            }
        }
    }
    
}

void BlendronicPreparationEditor::bkMessageReceived (const String& message)
{
    if (message == "blendronic/update")
    {
        update();
    }
}

int BlendronicPreparationEditor::addPreparation(void)
{
    processor.gallery->add(PreparationTypeBlendronic);
    
    return processor.gallery->getAllBlendronic().getLast()->getId();
}

int BlendronicPreparationEditor::duplicatePreparation(void)
{
    processor.gallery->duplicate(PreparationTypeBlendronic, processor.updateState->currentBlendronicId);
    
    return processor.gallery->getAllBlendronic().getLast()->getId();
}

void BlendronicPreparationEditor::deleteCurrent(void)
{
    int BlendronicId = selectCB.getSelectedId();
    int index = selectCB.getSelectedItemIndex();
    
    if ((index == 0) && (selectCB.getItemId(index+1) == -1)) return;
    
    processor.gallery->remove(PreparationTypeBlendronic, BlendronicId);
    
    fillSelectCB(0, 0);
    
    int newId = 0;
    
    selectCB.setSelectedId(newId, dontSendNotification);
    
    processor.updateState->currentBlendronicId = -1;
}

void BlendronicPreparationEditor::setCurrentId(int Id)
{
    processor.updateState->currentBlendronicId = Id;
    
    processor.updateState->idDidChange = true;
    
    update();
    
    fillSelectCB(lastId, Id);
    
    lastId = Id;
}

void BlendronicPreparationEditor::actionButtonCallback(int action, BlendronicPreparationEditor* vc)
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
        processor.reset(PreparationTypeBlendronic, processor.updateState->currentBlendronicId);
        vc->update();
    }
    else if (action == 5)
    {
        processor.clear(PreparationTypeBlendronic, processor.updateState->currentBlendronicId);
        vc->update();
    }
    else if (action == 6)
    {
        AlertWindow prompt("", "", AlertWindow::AlertIconType::QuestionIcon);
        
        int Id = processor.updateState->currentBlendronicId;
        Blendronic::Ptr prep = processor.gallery->getBlendronic(Id);
        
        prompt.addTextEditor("name", prep->getName());
        
        prompt.addButton("Ok", 1, KeyPress(KeyPress::returnKey));
        prompt.addButton("Cancel", 2, KeyPress(KeyPress::escapeKey));
        
        int result = prompt.runModalLoop();
        
        String name = prompt.getTextEditorContents("name");
        
        if (result == 1)
        {
            prep->setName(name);
            vc->fillSelectCB(Id, Id);
        }
        
        vc->update();
    }
    else if (action == 7)
    {
        AlertWindow prompt("", "", AlertWindow::AlertIconType::QuestionIcon);
        
        int Id = processor.updateState->currentBlendronicId;
        Blendronic::Ptr prep = processor.gallery->getBlendronic(Id);
        
        prompt.addTextEditor("name", prep->getName());
        
        prompt.addButton("Ok", 1, KeyPress(KeyPress::returnKey));
        prompt.addButton("Cancel", 2, KeyPress(KeyPress::escapeKey));
        
        int result = prompt.runModalLoop();
        
        String name = prompt.getTextEditorContents("name");
        
        DBG("name: " + String(name));
        
        if (result == 1)
        {
            processor.exportPreparation(PreparationTypeBlendronic, Id, name);
        }
    }
    else if (action >= 100)
    {
        int which = action - 100;
        processor.importPreparation(PreparationTypeBlendronic, processor.updateState->currentBlendronicId, which);
        vc->update();
    }
}

void BlendronicPreparationEditor::bkComboBoxDidChange (ComboBox* box)
{
    String name = box->getName();
    int Id = box->getSelectedId();
    int index = box->getSelectedItemIndex();
    
    if (name == "Blendronic")
    {
        setCurrentId(Id);
    }
//    else if (name == "Smooth Mode")
//    {
//        BlendronicPreparation::Ptr prep = processor.gallery->getStaticBlendronicPreparation(processor.updateState->currentBlendronicId);
//        BlendronicPreparation::Ptr active = processor.gallery->getActiveBlendronicPreparation(processor.updateState->currentBlendronicId);
//        
//        prep    ->setSmoothMode( (BlendronicSmoothMode) index);
//        active  ->setSmoothMode( (BlendronicSmoothMode) index);
//        
//        fillSmoothModeSelectCB();
//        
//    }
//    else if (name == "Sync Mode")
//    {
//        BlendronicPreparation::Ptr prep = processor.gallery->getStaticBlendronicPreparation(processor.updateState->currentBlendronicId);
//        BlendronicPreparation::Ptr active = processor.gallery->getActiveBlendronicPreparation(processor.updateState->currentBlendronicId);
//
//        prep    ->setSyncMode( (BlendronicSyncMode) index);
//        active  ->setSyncMode( (BlendronicSyncMode) index);
//
//        fillSyncModeSelectCB();
//
//    }
//    else if (name == "Clear Mode")
//    {
//        BlendronicPreparation::Ptr prep = processor.gallery->getStaticBlendronicPreparation(processor.updateState->currentBlendronicId);
//        BlendronicPreparation::Ptr active = processor.gallery->getActiveBlendronicPreparation(processor.updateState->currentBlendronicId);
//
//        prep    ->setClearMode( (BlendronicClearMode) index);
//        active  ->setClearMode( (BlendronicClearMode) index);
//
//        fillClearModeSelectCB();
//
//    }
//    else if (name == "Open Mode")
//    {
//        BlendronicPreparation::Ptr prep = processor.gallery->getStaticBlendronicPreparation(processor.updateState->currentBlendronicId);
//        BlendronicPreparation::Ptr active = processor.gallery->getActiveBlendronicPreparation(processor.updateState->currentBlendronicId);
//
//        prep    ->setOpenMode( (BlendronicOpenMode) index);
//        active  ->setOpenMode( (BlendronicOpenMode) index);
//
//        fillOpenModeSelectCB();
//
//    }
//    else if (name == "Close Mode")
//    {
//        BlendronicPreparation::Ptr prep = processor.gallery->getStaticBlendronicPreparation(processor.updateState->currentBlendronicId);
//        BlendronicPreparation::Ptr active = processor.gallery->getActiveBlendronicPreparation(processor.updateState->currentBlendronicId);
//
//        prep    ->setCloseMode( (BlendronicCloseMode) index);
//        active  ->setCloseMode( (BlendronicCloseMode) index);
//
//        fillCloseModeSelectCB();
//
//    }
    else // target combo boxes from tab 3
    {
        BlendronicPreparation::Ptr prep = processor.gallery->getStaticBlendronicPreparation(processor.updateState->currentBlendronicId);
        BlendronicPreparation::Ptr active = processor.gallery->getActiveBlendronicPreparation(processor.updateState->currentBlendronicId);
        
        for (int i=0; i<targetControlCBs.size(); i++)
        {
            if (box == targetControlCBs[i])
            {
                int selectedItem = targetControlCBs[i]->getSelectedId() - 1;
                DBG(targetControlCBs[i]->getName() + " " + cTargetNoteModes[selectedItem]);
                
                prep    ->setTargetTypeBlendronic(KeymapTargetType(i + TargetTypeBlendronicSync), (TargetNoteMode)selectedItem);
                active  ->setTargetTypeBlendronic(KeymapTargetType(i + TargetTypeBlendronicSync), (TargetNoteMode)selectedItem);
            }
        }
    }
}

void BlendronicPreparationEditor::BKSingleSliderValueChanged(BKSingleSlider* slider, String name, double val)
{
    BlendronicPreparation::Ptr prep = processor.gallery->getStaticBlendronicPreparation(processor.updateState->currentBlendronicId);
    BlendronicPreparation::Ptr active = processor.gallery->getActiveBlendronicPreparation(processor.updateState->currentBlendronicId);
    
    if (name == "cluster threshold")
    {
        prep->setClusterThresh(val);
        active->setClusterThresh(val);
    }
}

void BlendronicPreparationEditor::fillSelectCB(int last, int current)
{
    selectCB.clear(dontSendNotification);
    
    for (auto prep : processor.gallery->getAllBlendronic())
    {
        int Id = prep->getId();
        
        if (Id == -1) continue;
        
        String name = prep->getName();
        
        if (name != String())  selectCB.addItem(name, Id);
        else                        selectCB.addItem("Blendronic"+String(Id), Id);
        
        selectCB.setItemEnabled(Id, true);
        if (processor.currentPiano->isActive(PreparationTypeBlendronic, Id))
            selectCB.setItemEnabled(Id, false);
    }
    
    if (last != 0)      selectCB.setItemEnabled(last, true);
    if (current != 0)   selectCB.setItemEnabled(current, false);
    
    int selectedId = processor.updateState->currentBlendronicId;
    
    selectCB.setSelectedId(selectedId, NotificationType::dontSendNotification);
    
    selectCB.setItemEnabled(selectedId, false);
    
    lastId = selectedId;
    
}

//void BlendronicPreparationEditor::fillSmoothModeSelectCB()
//{
//    BlendronicPreparation::Ptr prep = processor.gallery->getActiveBlendronicPreparation(processor.updateState->currentBlendronicId);
//
//    smoothModeSelectCB.clear(dontSendNotification);
//
//    smoothModeSelectCB.addItem(cBlendronicSmoothModes[ConstantTimeSmooth], 1);
//    smoothModeSelectCB.addItem(cBlendronicSmoothModes[ConstantRateSmooth], 2);
//    smoothModeSelectCB.addItem(cBlendronicSmoothModes[ProportionalTimeSmooth], 3);
//    smoothModeSelectCB.addItem(cBlendronicSmoothModes[ProportionalRateSmooth], 4);
//
//    smoothModeSelectCB.setSelectedItemIndex(prep->getSmoothMode(), dontSendNotification);
//}

//void BlendronicPreparationEditor::fillSyncModeSelectCB()
//{
//    BlendronicPreparation::Ptr prep = processor.gallery->getActiveBlendronicPreparation(processor.updateState->currentBlendronicId);
//
//    syncModeSelectCB.clear(dontSendNotification);
//
//    for (int i = 0; i <= BlendronicSyncModeNil; ++i)
//    {
//        syncModeSelectCB.addItem(cBlendronicSyncModes[i], i+1);
//    }
//
//    syncModeSelectCB.setSelectedItemIndex(prep->getSyncMode(), dontSendNotification);
//}
//
//void BlendronicPreparationEditor::fillClearModeSelectCB()
//{
//    BlendronicPreparation::Ptr prep = processor.gallery->getActiveBlendronicPreparation(processor.updateState->currentBlendronicId);
//
//    clearModeSelectCB.clear(dontSendNotification);
//
//    for (int i = 0; i <= BlendronicClearModeNil; ++i)
//    {
//        clearModeSelectCB.addItem(cBlendronicClearModes[i], i+1);
//    }
//
//    clearModeSelectCB.setSelectedItemIndex(prep->getClearMode(), dontSendNotification);
//}
//
//void BlendronicPreparationEditor::fillOpenModeSelectCB()
//{
//    BlendronicPreparation::Ptr prep = processor.gallery->getActiveBlendronicPreparation(processor.updateState->currentBlendronicId);
//
//    openModeSelectCB.clear(dontSendNotification);
//
//    for (int i = 0; i <= BlendronicOpenModeNil; ++i)
//    {
//        openModeSelectCB.addItem(cBlendronicOpenModes[i], i+1);
//    }
//
//    openModeSelectCB.setSelectedItemIndex(prep->getOpenMode(), dontSendNotification);
//}
//
//void BlendronicPreparationEditor::fillCloseModeSelectCB()
//{
//    BlendronicPreparation::Ptr prep = processor.gallery->getActiveBlendronicPreparation(processor.updateState->currentBlendronicId);
//
//    closeModeSelectCB.clear(dontSendNotification);
//
//    for (int i = 0; i <= BlendronicCloseModeNil; ++i)
//    {
//        closeModeSelectCB.addItem(cBlendronicCloseModes[i], i+1);
//    }
//
//    closeModeSelectCB.setSelectedItemIndex(prep->getCloseMode(), dontSendNotification);
//}

void BlendronicPreparationEditor::timerCallback()
{
    if (processor.updateState->currentDisplay == DisplayBlendronic)
    {
        BlendronicProcessor::Ptr proc = processor.currentPiano->getBlendronicProcessor(processor.updateState->currentBlendronicId);
        BlendronicPreparation::Ptr prep = processor.gallery->getActiveBlendronicPreparation(processor.updateState->currentBlendronicId);
        if (prep != nullptr && proc != nullptr)
        {
            
//            if(proc->getClusterThresholdTimer() < prep->getClusterThreshMS())
//                keyThreshSlider->setDisplayValue(proc->getClusterThresholdTimer());
//            else keyThreshSlider->setDisplayValue(0);

            if (proc->getActive()) delayLineDisplay.setColours(Colours::black, Colours::lightgrey);
            else delayLineDisplay.setColours(Colours::black, Colours::lightgrey.withMultipliedBrightness(0.6));
            delayLineDisplay.setLineSpacing(proc->getPulseLengthInSamples());
            float maxDelayLength = 0.0f;
            for (auto d : prep->getDelayLengths())
            {
                if (d > maxDelayLength) maxDelayLength = d;
            }
            delayLineDisplay.setMaxDelayLength(maxDelayLength);
            delayLineDisplay.pushBuffer(proc->getDelayBuffer());
            delayLineDisplay.pushSmoothing(proc->getDelayLengthRecord());
            delayLineDisplay.setPulseOffset(proc->getPulseOffset());
            delayLineDisplay.setMarkers(proc->getBeatPositionsInBuffer());
            delayLineDisplay.setPlayheads(Array<uint64>({proc->getCurrentSample(), proc->getDelayedSample()}));
            
            // dim target comboboxes that aren't activated by a Keymap
            for (int i=TargetTypeBlendronicSync; i<=TargetTypeBlendronicOpenCloseOutput; i++)
            {
                bool makeBright = false;
                for (auto km : proc->getKeymaps())
                    if (km->getTargetStates()[(KeymapTargetType) i] == TargetStateEnabled) makeBright = true;
                
                if (makeBright)
                {
                    targetControlCBs[i - TargetTypeBlendronicSync]->setAlpha(1.);
                    targetControlCBLabels[i - TargetTypeBlendronicSync]->setAlpha(1.);
                    
                    targetControlCBs[i - TargetTypeBlendronicSync]->setEnabled(true);
                }
                else
                {
                    targetControlCBs[i - TargetTypeBlendronicSync]->setAlpha(0.25);
                    targetControlCBLabels[i - TargetTypeBlendronicSync]->setAlpha(0.25);
                    
                    targetControlCBs[i - TargetTypeBlendronicSync]->setEnabled(false);
                }
            }
        
            int counter = 0, size = 0;
            
            for (int i = 0; i < paramSliders.size(); i++)
            {
                if(paramSliders[i]->getName() == "beat lengths")
                {
                    size = paramSliders[i]->getNumVisible();
                    counter = proc->getBeatIndex();
                    paramSliders[i]->setCurrentSlider((counter >= size || counter < 0) ? 0 : counter);
                }
                else if(paramSliders[i]->getName() == "delay lengths")
                {
                    size = paramSliders[i]->getNumVisible();
                    counter = proc->getDelayIndex();
                    paramSliders[i]->setCurrentSlider((counter >= size || counter < 0) ? 0 : counter);
                }
                else if(paramSliders[i]->getName() == "smoothing")
                {
                    size = paramSliders[i]->getNumVisible();
                    counter = proc->getSmoothIndex();
                    paramSliders[i]->setCurrentSlider((counter >= size || counter < 0) ? 0 : counter);
                }
                else if(paramSliders[i]->getName() == "feedback coefficients")
                {
                    size = paramSliders[i]->getNumVisible();
                    counter = proc->getFeedbackIndex();
                    paramSliders[i]->setCurrentSlider((counter >= size || counter < 0) ? 0 : counter);
                    
                }
            }
        }
    }
}

void BlendronicPreparationEditor::multiSliderDidChange(String name, int whichSlider, Array<float> values)
{
    BlendronicPreparation::Ptr prep = processor.gallery->getStaticBlendronicPreparation(processor.updateState->currentBlendronicId);
    BlendronicPreparation::Ptr active = processor.gallery->getActiveBlendronicPreparation(processor.updateState->currentBlendronicId);
    
    if (name == "beat lengths")
    {
        prep    ->setBeat(whichSlider, values[0]);
        active  ->setBeat(whichSlider, values[0]);
    }
    else if (name == "delay length")
    {
        prep    ->setDelayLength(whichSlider, values[0]);
        active  ->setDelayLength(whichSlider, values[0]);
    }
    else if (name == "smoothing")
    {
        prep    ->setSmoothValue(whichSlider, values[0]);
        active  ->setSmoothValue(whichSlider, values[0]);
    }
    else if (name == "feedback coefficients")
    {
        prep    ->setFeedbackCoefficient(whichSlider, values[0]);
        active  ->setFeedbackCoefficient(whichSlider, values[0]);
    }
}

void BlendronicPreparationEditor::multiSlidersDidChange(String name, Array<Array<float>> values)
{
    BlendronicPreparation::Ptr prep = processor.gallery->getStaticBlendronicPreparation(processor.updateState->currentBlendronicId);
    BlendronicPreparation::Ptr active = processor.gallery->getActiveBlendronicPreparation(processor.updateState->currentBlendronicId);
    
    Array<float> newvals = Array<float>();
    for(int i=0; i<values.size(); i++) newvals.add(values[i][0]);
    
    if (name == "beat lengths")
    {
        prep    ->setBeats(newvals);
        active  ->setBeats(newvals);
    }
    else if (name == "delay lengths")
    {
        prep    ->setDelayLengths(newvals);
        active  ->setDelayLengths(newvals);
    }
    else if (name == "smoothing")
    {
        prep    ->setSmoothValues(newvals);
        active  ->setSmoothValues(newvals);
    }
    else if (name == "feedback coefficients")
    {
        prep    ->setFeedbackCoefficients(newvals);
        active  ->setFeedbackCoefficients(newvals);
    }
}

void BlendronicPreparationEditor::buttonClicked (Button* b)
{
    BlendronicPreparation::Ptr prep = processor.gallery->getStaticBlendronicPreparation(processor.updateState->currentBlendronicId);
    BlendronicPreparation::Ptr active = processor.gallery->getActiveBlendronicPreparation(processor.updateState->currentBlendronicId);
    
    if (b == &hideOrShow)
    {
        processor.updateState->setCurrentDisplay(DisplayNil);

        setSubWindowInFront(false);
    }
    else if (b == &actionButton)
    {
        getPrepOptionMenu(PreparationTypeBlendronic).showMenuAsync (PopupMenu::Options().withTargetComponent (&actionButton), ModalCallbackFunction::forComponent (actionButtonCallback, this) );
    }
    else if (b == &rightArrow)
    {
        arrowPressed(RightArrow);
        
        DBG("currentTab: " + String(currentTab));
        
        displayTab(currentTab);
    }
    else if (b == &leftArrow)
    {
        arrowPressed(LeftArrow);
        
        DBG("currentTab: " + String(currentTab));
        
        displayTab(currentTab);
    }
    else if (b == &pulseBeatSmooth)
    {
        prep->toggleSmoothBase();
        active->toggleSmoothBase();
        if (active->getSmoothBase() == BlendronicSmoothPulse)
        {
            pulseBeatSmooth.setButtonText("pulse length");
        }
        else pulseBeatSmooth.setButtonText("beat length");
    }
    else if (b == &constantFullSmooth)
    {
        prep->toggleSmoothScale();
        active->toggleSmoothScale();
        if (active->getSmoothScale() == BlendronicSmoothConstant)
        {
            constantFullSmooth.setButtonText("change in delay of 1");
        }
        else constantFullSmooth.setButtonText("full delay change");
    }
}

void BlendronicPreparationEditor::BKRangeSliderValueChanged(String name, double minval, double maxval)
{
    BlendronicPreparation::Ptr prep = processor.gallery->getStaticBlendronicPreparation(processor.updateState->currentBlendronicId);
    BlendronicPreparation::Ptr active = processor.gallery->getActiveBlendronicPreparation(processor.updateState->currentBlendronicId);
    
    if (name == "hold time (ms)")
    {
        prep->setHoldMin(minval);
        active->setHoldMin(minval);
        
        prep->setHoldMax(maxval);
        active->setHoldMax(maxval);
    }
    else if (name == "velocity min/max (0-127)")
    {
        prep->setVelocityMin(minval);
        active->setVelocityMin(minval);
        
        prep->setVelocityMax(maxval);
        active->setVelocityMax(maxval);
        
        DBG("velocity min: " + String(prep->getVelocityMin()));
        DBG("velocity max: " + String(prep->getVelocityMax()));
    }
}

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ BlendronicModificationEditor ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ //

BlendronicModificationEditor::BlendronicModificationEditor(BKAudioProcessor& p, BKItemGraph* theGraph):
BlendronicViewController(p, theGraph)
{
    fillSelectCB(-1,-1);
    
    selectCB.addListener(this);
    selectCB.addMyListener(this);
    
    //startTimer(20);
}

void BlendronicModificationEditor::greyOutAllComponents()
{
//    holdTimeMinMaxSlider->setDim(gModAlpha);
//    velocityMinMaxSlider->setDim(gModAlpha);
}

void BlendronicModificationEditor::highlightModedComponents()
{
    BlendronicModification::Ptr mod = processor.gallery->getBlendronicModification(processor.updateState->currentModBlendronicId);
    
//    if(mod->getDirty(BlendronicWaveDistance))      nDisplaySlider.setBright();
//    if(mod->getDirty(BlendronicUndertow))          nDisplaySlider.setBright();
//    if(mod->getDirty(BlendronicTransposition))     transpositionSlider->setBright();
//    if(mod->getDirty(BlendronicLengthMultiplier))  lengthMultiplierSlider->setBright();
//    if(mod->getDirty(BlendronicBeatsToSkip))       beatsToSkipSlider->setBright();
//    if(mod->getDirty(BlendronicMode))              lengthModeSelectCB.setAlpha(1.);
//    if(mod->getDirty(BlendronicGain))              gainSlider->setBright();
//    if(mod->getDirty(BlendronicReverseADSR))       reverseADSRSlider->setBright();
//    if(mod->getDirty(BlendronicUndertowADSR))      undertowADSRSlider->setBright();
//    if(mod->getDirty(BlendronicHoldMin))           holdTimeMinMaxSlider->setBright();
//    if(mod->getDirty(BlendronicHoldMax))           holdTimeMinMaxSlider->setBright();
//    if(mod->getDirty(BlendronicVelocityMin))       velocityMinMaxSlider->setBright();
//    if(mod->getDirty(BlendronicVelocityMax))       velocityMinMaxSlider->setBright();
//    if(mod->getDirty(BlendronicClusterMin))        clusterMinSlider->setBright();
//    if(mod->getDirty(BlendronicClusterThreshold))  clusterThresholdSlider->setBright();
//    if(mod->getDirty(BlendronicKeyOnReset))        { keyOnResetToggle.setAlpha(1.); keyOnResetLabel.setAlpha(1.); }
}

void BlendronicModificationEditor::update(void)
{
    if (processor.updateState->currentModBlendronicId < 0) return;
    
    
    BlendronicModification::Ptr mod = processor.gallery->getBlendronicModification(processor.updateState->currentModBlendronicId);
    
    if (mod != nullptr)
    {
        fillSelectCB(-1,-1);
        
        greyOutAllComponents();
        highlightModedComponents();
        
        selectCB.setSelectedId(processor.updateState->currentModBlendronicId, dontSendNotification);
        
//        nDisplaySlider.setWaveDistance(mod->getWavedistance(), dontSendNotification);
//
//        nDisplaySlider.setUndertow(mod->getUndertow(), dontSendNotification);
//
//        BlendronicSyncMode mode = (BlendronicSyncMode) mod->getMode();
//        lengthModeSelectCB.setSelectedItemIndex(mode, dontSendNotification);
//
//        if (currentTab == 0)
//        {
//            if(mode == NoteLengthSync)
//            {
//                lengthMultiplierSlider->setVisible(true);
//                beatsToSkipSlider->setVisible(false);
//            }
//            else
//            {
//                lengthMultiplierSlider->setVisible(false);
//                beatsToSkipSlider->setVisible(true);
//            }
//
//        }
//
//        transpositionSlider->setTo(mod->getTransposition(), dontSendNotification);
//
//        lengthMultiplierSlider->setValue(mod->getLengthMultiplier(), dontSendNotification);
//
//        beatsToSkipSlider->setValue(mod->getBeatsToSkip(), dontSendNotification);
//
//        gainSlider->setValue(mod->getGain(), dontSendNotification);
//
//
//        reverseADSRSlider->setValue(mod->getReverseADSRvals(), dontSendNotification);
//
//        undertowADSRSlider->setValue(mod->getUndertowADSRvals(), dontSendNotification);
//
//        holdTimeMinMaxSlider->setMinValue(mod->getHoldMin(), dontSendNotification);
//
//        holdTimeMinMaxSlider->setMaxValue(mod->getHoldMax(), dontSendNotification);
//
//        velocityMinMaxSlider->setMinValue(mod->getVelocityMin(), dontSendNotification);
//
//        velocityMinMaxSlider->setMaxValue(mod->getVelocityMax(), dontSendNotification);
//
//        clusterMinSlider->setValue(mod->getClusterMin(), dontSendNotification);
//
//        clusterThresholdSlider->setValue(mod->getClusterThreshold(), dontSendNotification);
//
//        keyOnResetToggle.setToggleState((bool)mod->getKeyOnReset(), dontSendNotification);
    }
    
    
}


void BlendronicModificationEditor::fillSelectCB(int last, int current)
{
    selectCB.clear(dontSendNotification);
    
    for (auto prep : processor.gallery->getBlendronicModifications())
    {
        int Id = prep->getId();;
        String name = prep->getName();
        
        if (name != String())  selectCB.addItem(name, Id);
        else                        selectCB.addItem("BlendronicMod"+String(Id), Id);
        
        selectCB.setItemEnabled(Id, true);
        if (processor.currentPiano->isActive(PreparationTypeBlendronic, Id))
            selectCB.setItemEnabled(Id, false);
    }
    
    if (last != 0)      selectCB.setItemEnabled(last, true);
    if (current != 0)   selectCB.setItemEnabled(current, false);
    
    int selectedId = processor.updateState->currentBlendronicId;
    
    selectCB.setSelectedId(selectedId, NotificationType::dontSendNotification);
    
    selectCB.setItemEnabled(selectedId, false);
    
    lastId = selectedId;
}

//void BlendronicModificationEditor::fillSmoothModeSelectCB()
//{
//    BlendronicModification::Ptr prep = processor.gallery->getBlendronicModification(processor.updateState->currentModBlendronicId);
//    
//    smoothModeSelectCB.clear(dontSendNotification);
//    
//    smoothModeSelectCB.addItem(cBlendronicSmoothModes[ConstantTimeSmooth], 1);
//    smoothModeSelectCB.addItem(cBlendronicSmoothModes[ConstantRateSmooth], 2);
//    smoothModeSelectCB.addItem(cBlendronicSmoothModes[ProportionalTimeSmooth], 3);
//    smoothModeSelectCB.addItem(cBlendronicSmoothModes[ProportionalRateSmooth], 4);
//    
//    smoothModeSelectCB.setSelectedItemIndex(prep->getSmoothMode(), dontSendNotification);
//}

//void BlendronicModificationEditor::fillSyncModeSelectCB()
//{
//    BlendronicModification::Ptr prep = processor.gallery->getBlendronicModification(processor.updateState->currentModBlendronicId);
//
//    syncModeSelectCB.clear(dontSendNotification);
//
//    for (int i = 0; i <= BlendronicSyncModeNil; ++i)
//    {
//        syncModeSelectCB.addItem(cBlendronicSyncModes[i], i+1);
//    }
//
//    syncModeSelectCB.setSelectedItemIndex(prep->getSyncMode(), dontSendNotification);
//}
//
//void BlendronicModificationEditor::fillClearModeSelectCB()
//{
//    BlendronicModification::Ptr prep = processor.gallery->getBlendronicModification(processor.updateState->currentModBlendronicId);
//
//    clearModeSelectCB.clear(dontSendNotification);
//
//    for (int i = 0; i <= BlendronicClearModeNil; ++i)
//    {
//        clearModeSelectCB.addItem(cBlendronicClearModes[i], i+1);
//    }
//
//    clearModeSelectCB.setSelectedItemIndex(prep->getClearMode(), dontSendNotification);
//}



void BlendronicModificationEditor::timerCallback()
{
    if (processor.updateState->currentDisplay == DisplayBlendronicMod)
    {
        /*
         BlendronicProcessor::Ptr bProcessor = processor.currentPiano->getBlendronicProcessor(processor.updateState->currentBlendronicId);
         
         Array<int> currentPlayPositions = bProcessor->getPlayPositions();
         Array<int> currentUndertowPositions = bProcessor->getUndertowPositions();
         currentPlayPositions.addArray(currentUndertowPositions);
         
         nDisplaySlider.updateSliderPositions(currentPlayPositions);
         */
    }
    
}

void BlendronicModificationEditor::multiSliderDidChange(String name, int whichSlider, Array<float> values)
{
//    BlendronicPreparation::Ptr prep = processor.gallery->getStaticBlendronicPreparation(processor.updateState->currentBlendronicId);
//    BlendronicPreparation::Ptr active = processor.gallery->getActiveBlendronicPreparation(processor.updateState->currentBlendronicId);
//
//    if (name == "Beats")
//    {
//        prep    ->setBeats(values);
//        active  ->setBeats(values);
//    }
//    else if (name == "Smooth Durations")
//    {
//        prep    ->setSmoothDurations(values);
//        active  ->setSmoothDurations(values);
//    }
//    else if (name == "Feedback Coeffs")
//    {
//        prep    ->setFeedbackCoefficients(values);
//        active  ->setFeedbackCoefficients(values);
//    }
}

void BlendronicModificationEditor::multiSlidersDidChange(String name, Array<Array<float>> values)
{
    //    BlendronicPreparation::Ptr prep = processor.gallery->getStaticBlendronicPreparation(processor.updateState->currentBlendronicId);
    //    BlendronicPreparation::Ptr active = processor.gallery->getActiveBlendronicPreparation(processor.updateState->currentBlendronicId);
    //
    //    if (name == "Beats")
    //    {
    //        prep    ->setBeats(values);
    //        active  ->setBeats(values);
    //    }
    //    else if (name == "Smooth Durations")
    //    {
    //        prep    ->setSmoothDurations(values);
    //        active  ->setSmoothDurations(values);
    //    }
    //    else if (name == "Feedback Coeffs")
    //    {
    //        prep    ->setFeedbackCoefficients(values);
    //        active  ->setFeedbackCoefficients(values);
    //    }
}

void BlendronicModificationEditor::BKEditableComboBoxChanged(String name, BKEditableComboBox* cb)
{
    BlendronicModification::Ptr mod = processor.gallery->getBlendronicModification(processor.updateState->currentModBlendronicId);
    
    mod->setName(name);
    
    updateModification();
}

void BlendronicModificationEditor::bkMessageReceived (const String& message)
{
    if (message == "blendronic/update")
    {
        update();
    }
}

int BlendronicModificationEditor::addPreparation(void)
{
    processor.gallery->add(PreparationTypeBlendronicMod);
    
    return processor.gallery->getBlendronicModifications().getLast()->getId();
}

int BlendronicModificationEditor::duplicatePreparation(void)
{
    processor.gallery->duplicate(PreparationTypeBlendronicMod, processor.updateState->currentModBlendronicId);
    
    return processor.gallery->getBlendronicModifications().getLast()->getId();
}

void BlendronicModificationEditor::deleteCurrent(void)
{
    int oldId = selectCB.getSelectedId();
    int index = selectCB.getSelectedItemIndex();
    
    if ((index == 0) && (selectCB.getItemId(index+1) == -1)) return;
    
    processor.gallery->remove(PreparationTypeBlendronicMod, oldId);
    
    fillSelectCB(0, 0);
    
    int newId = 0;
    
    selectCB.setSelectedId(newId, dontSendNotification);
    
    processor.updateState->currentModBlendronicId = -1;
}

void BlendronicModificationEditor::setCurrentId(int Id)
{
    processor.updateState->currentModBlendronicId = Id;
    
    processor.updateState->idDidChange = true;
    
    update();
    
    fillSelectCB(lastId, Id);
    
    lastId = Id;
}

void BlendronicModificationEditor::actionButtonCallback(int action, BlendronicModificationEditor* vc)
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
        processor.clear(PreparationTypeBlendronicMod, processor.updateState->currentModBlendronicId);
        vc->update();
        vc->updateModification();
    }
    else if (action == 6)
    {
        AlertWindow prompt("", "", AlertWindow::AlertIconType::QuestionIcon);
        
        int Id = processor.updateState->currentModBlendronicId;
        BlendronicModification::Ptr prep = processor.gallery->getBlendronicModification(Id);
        
        prompt.addTextEditor("name", prep->getName());
        
        prompt.addButton("Ok", 1, KeyPress(KeyPress::returnKey));
        prompt.addButton("Cancel", 2, KeyPress(KeyPress::escapeKey));
        
        int result = prompt.runModalLoop();
        
        String name = prompt.getTextEditorContents("name");
        
        if (result == 1)
        {
            prep->setName(name);
            vc->fillSelectCB(Id, Id);
        }
        
        vc->update();
    }
    else if (action == 7)
    {
        AlertWindow prompt("", "", AlertWindow::AlertIconType::QuestionIcon);
        
        int Id = processor.updateState->currentModBlendronicId;
        BlendronicModification::Ptr prep = processor.gallery->getBlendronicModification(Id);
        
        prompt.addTextEditor("name", prep->getName());
        
        prompt.addButton("Ok", 1, KeyPress(KeyPress::returnKey));
        prompt.addButton("Cancel", 2, KeyPress(KeyPress::escapeKey));
        
        int result = prompt.runModalLoop();
        
        String name = prompt.getTextEditorContents("name");
        
        DBG("name: " + String(name));
        
        if (result == 1)
        {
            processor.exportPreparation(PreparationTypeBlendronicMod, Id, name);
        }
    }
    else if (action >= 100)
    {
        int which = action - 100;
        processor.importPreparation(PreparationTypeBlendronicMod, processor.updateState->currentModBlendronicId, which);
        vc->update();
    }
}

void BlendronicModificationEditor::bkComboBoxDidChange (ComboBox* box)
{
    String name = box->getName();
    int Id = box->getSelectedId();
    int index = box->getSelectedItemIndex();
    
    if (name == "Blendronic")
    {
        setCurrentId(Id);
    }
    else if (name == "Smooth Mode")
    {
        BlendronicModification::Ptr mod = processor.gallery->getBlendronicModification(processor.updateState->currentModBlendronicId);
        
//        BlendronicSyncMode mode = (BlendronicSyncMode) index;
//
//        mod->setMode(mode);
//        mod->setDirty(BlendronicMode);
//
//        if (currentTab == 0)
//        {
//            if(mode == NoteLengthSync)
//            {
//                lengthMultiplierSlider->setVisible(true);
//                beatsToSkipSlider->setVisible(false);
//            }
//            else
//            {
//                lengthMultiplierSlider->setVisible(false);
//                beatsToSkipSlider->setVisible(true);
//            }
//        }
//
//        updateModification();
//        lengthModeSelectCB.setAlpha(1.);
//
    }
    
    
}

void BlendronicModificationEditor::BKSingleSliderValueChanged(BKSingleSlider* slider, String name, double val)
{
    BlendronicModification::Ptr mod = processor.gallery->getBlendronicModification(processor.updateState->currentModBlendronicId);
    
//    if(name == "note length multiplier")
//    {
//        mod->setLengthMultiplier(val);
//        mod->setDirty(BlendronicLengthMultiplier);
//
//        lengthMultiplierSlider->setBright();
//    }
//    else if(name == "beats to skip")
//    {
//        mod->setBeatsToSkip(val);
//        mod->setDirty(BlendronicBeatsToSkip);
//
//        beatsToSkipSlider->setBright();
//    }
//    else if(name == "gain")
//    {
//        mod->setGain(val);
//        mod->setDirty(BlendronicGain);
//
//        gainSlider->setBright();
//    }
//    else if (name == "cluster min")
//    {
//        mod->setClusterMin(val);
//        mod->setDirty(BlendronicClusterMin);
//
//        DBG("setting cluster min : " + String(val));
//
//        clusterMinSlider->setBright();
//    }
//    else if (name == "cluster thresh")
//    {
//        mod->setClusterThreshold(val);
//        mod->setDirty(BlendronicClusterThreshold);
//
//        DBG("setting cluster thresh : " + String(val));
//
//        clusterThresholdSlider->setBright();
//    }
    
    updateModification();
}

void BlendronicModificationEditor::updateModification(void)
{
    processor.updateState->modificationDidChange = true;
}

void BlendronicModificationEditor::buttonClicked (Button* b)
{
    BlendronicModification::Ptr mod = processor.gallery->getBlendronicModification(processor.updateState->currentModBlendronicId);
    
    if (b == &hideOrShow)
    {
        processor.updateState->setCurrentDisplay(DisplayNil);
    }
    else if (b == &actionButton)
    {
        getModOptionMenu(PreparationTypeBlendronicMod).showMenuAsync (PopupMenu::Options().withTargetComponent (&actionButton), ModalCallbackFunction::forComponent (actionButtonCallback, this) );
    }
//    else if (b == &keyOnResetToggle)
//    {
//        mod->setKeyOnReset(b->getToggleState());
//        mod->setDirty(BlendronicKeyOnReset);
//
//        keyOnResetToggle.setAlpha(1.);
//        keyOnResetLabel.setAlpha(1.);
//    }
    else if (b == &rightArrow)
    {
        arrowPressed(RightArrow);
        
        DBG("currentTab: " + String(currentTab));
        
        displayTab(currentTab);
    }
    else if (b == &leftArrow)
    {
        arrowPressed(LeftArrow);
        
        DBG("currentTab: " + String(currentTab));
        
        displayTab(currentTab);
    }
    
    updateModification();
}

void BlendronicModificationEditor::BKRangeSliderValueChanged(String name, double minval, double maxval)
{
    BlendronicModification::Ptr mod = processor.gallery->getBlendronicModification(processor.updateState->currentModBlendronicId);
    
//    if (name == "hold time (ms)")
//    {
//        mod->setHoldMin(minval);
//        mod->setDirty(BlendronicHoldMin);
//
//        mod->setHoldMax(maxval);
//        mod->setDirty(BlendronicHoldMax);
//
//        holdTimeMinMaxSlider->setBright();
//    }
//    else if (name == "velocity min/max (0-127)")
//    {
//        mod->setVelocityMin(minval);
//        mod->setDirty(BlendronicVelocityMin);
//
//        mod->setVelocityMax(maxval);
//        mod->setDirty(BlendronicVelocityMax);
//
//        velocityMinMaxSlider->setBright();
//    }
}
