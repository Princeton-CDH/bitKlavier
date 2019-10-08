/*
  ==============================================================================

    BlendronicViewController.cpp
    Created: 6 Sep 2019 3:15:24pm
    Author:  Matthew Wang

  ==============================================================================
*/

#include "BlendronicViewController.h"

BlendronicViewController::BlendronicViewController(BKAudioProcessor& p, BKItemGraph* theGraph):
BKViewController(p, theGraph, 2)
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
            
            if(paramSliders[idx]->getName() == "beats")
            {
                paramSliders[idx]->setToolTipString("Determines delay times in terms of beats; double-click to edit all or add additional sequence steps");
            }
            else if(paramSliders[idx]->getName() == "smooth timings")
            {
                paramSliders[idx]->setToolTipString("Determines duration of smoothing between delay times; double-click to edit all or add additional sequence steps\nUnits: Constant Time uses pulses; Constant Rate uses beat length change per pulse; Proportional Time uses a proportion of the current beat length; Proportional Rate uses beat length change per current beat length");
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
    
    smoothModeSelectCB.setName("Smooth Mode");
    smoothModeSelectCB.setTooltip("Determines whether smooth durations are constant or proportional to beat length");
    smoothModeSelectCB.addSeparator();
    smoothModeSelectCB.setLookAndFeel(&comboBoxRightJustifyLAF);
    comboBoxRightJustifyLAF.setComboBoxJustificationType(juce::Justification::centredRight);
    smoothModeSelectCB.addListener(this);
    smoothModeSelectCB.setSelectedItemIndex(0);
    addAndMakeVisible(smoothModeSelectCB);
    
    smoothModeLabel.setText("smoothing", dontSendNotification);
    smoothModeLabel.setTooltip("Determines whether smooth durations are constant or proportional to beat length and delta");
    addAndMakeVisible(&smoothModeLabel, ALL);
    
    syncModeSelectCB.setName("Sync Mode");
    syncModeSelectCB.BKSetJustificationType(juce::Justification::centredRight);
    syncModeSelectCB.setTooltip("Determines which aspect of MIDI signal triggers the Blendronic sequence");
    syncModeSelectCB.addSeparator();
    syncModeSelectCB.addListener(this);
    syncModeSelectCB.setSelectedItemIndex(0);
    addAndMakeVisible(syncModeSelectCB);
    
    syncModeLabel.setText("pulse reset by", dontSendNotification);
    syncModeLabel.setJustificationType(juce::Justification::centredRight);
    syncModeLabel.setTooltip("Determines which aspect of MIDI signal resets the Blendronic sequence");
    addAndMakeVisible(&syncModeLabel, ALL);
    
    clearModeSelectCB.setName("Clear Mode");
    clearModeSelectCB.setTooltip("Determines which aspect of MIDI signal clears the delay line");
    clearModeSelectCB.addSeparator();
    clearModeSelectCB.setLookAndFeel(&comboBoxRightJustifyLAF);
    comboBoxRightJustifyLAF.setComboBoxJustificationType(juce::Justification::centredRight);
    clearModeSelectCB.addListener(this);
    clearModeSelectCB.setSelectedItemIndex(0);
    addAndMakeVisible(clearModeSelectCB);
    
    clearModeLabel.setText("clears the delay line", dontSendNotification);
    clearModeLabel.setTooltip("Determines which aspect of MIDI signal clears the delay line");
    addAndMakeVisible(&clearModeLabel, ALL);
    
    closeModeSelectCB.setName("Close Mode");
    closeModeSelectCB.BKSetJustificationType(juce::Justification::centredRight);
    closeModeSelectCB.setTooltip("Determines which aspect of MIDI signal clears the delay line");
    closeModeSelectCB.addSeparator();
    closeModeSelectCB.addListener(this);
    closeModeSelectCB.setSelectedItemIndex(0);
    addAndMakeVisible(closeModeSelectCB);
    
    closeModeLabel.setText("close the delay line on", dontSendNotification);
    closeModeLabel.setJustificationType(juce::Justification::centredRight);
    closeModeLabel.setTooltip("Determines which aspect of MIDI signal close the delay line");
    addAndMakeVisible(&closeModeLabel, ALL);
    
    openModeSelectCB.setName("Open Mode");
    openModeSelectCB.setTooltip("Determines which aspect of MIDI signal opens the delay line");
    openModeSelectCB.addSeparator();
    openModeSelectCB.setLookAndFeel(&comboBoxRightJustifyLAF);
    comboBoxRightJustifyLAF.setComboBoxJustificationType(juce::Justification::centredRight);
    openModeSelectCB.addListener(this);
    openModeSelectCB.setSelectedItemIndex(0);
    addAndMakeVisible(openModeSelectCB);
    
    openModeLabel.setText("opens the delay line", dontSendNotification);
    openModeLabel.setTooltip("Determines which aspect of MIDI signal opens the delay line");
    addAndMakeVisible(&openModeLabel, ALL);
    
    addAndMakeVisible(&actionButton, ALL);
    actionButton.setButtonText("Action");
    actionButton.setTooltip("Create, duplicate, rename, delete, or reset current settings");
    actionButton.addListener(this);
    
    currentTab = 0;
    displayTab(currentTab);
    
}

void BlendronicViewController::invisible(void)
{
    for (int i = 0; i < paramSliders.size(); i++)
    {
        paramSliders[i]->setVisible(false);
    }
    syncModeSelectCB.setVisible(false);
    syncModeLabel.setVisible(false);
    smoothModeSelectCB.setVisible(false);
    smoothModeLabel.setVisible(false);
    clearModeSelectCB.setVisible(false);
    clearModeLabel.setVisible(false);
    closeModeSelectCB.setVisible(false);
    closeModeLabel.setVisible(false);
    openModeSelectCB.setVisible(false);
    openModeLabel.setVisible(false);
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
    
    int x0 = leftArrow.getRight() + gXSpacing;
    int y0 = hideOrShow.getBottom() + gYSpacing;
    int right = rightArrow.getX() - gXSpacing;
    int width = right - x0;
    int height = getHeight() - y0;
    
    if (tab == 0)
    {
        BlendronicPreparation::Ptr prep = processor.gallery->getStaticBlendronicPreparation(processor.updateState->currentBlendronicId);
    
        smoothModeSelectCB.setVisible(true);
        smoothModeLabel.setVisible(true);
        for (int i = 0; i < paramSliders.size(); i++)
        {
            paramSliders[i]->setVisible(true);
        }
        
        // SET BOUNDS
        int sliderHeight = height * 0.225f;
        
        Rectangle<int> area (getBounds());
        area.removeFromTop(hideOrShow.getBottom() + gYSpacing);
        area.removeFromRight(rightArrow.getWidth() + gXSpacing);
        area.removeFromLeft(leftArrow.getWidth() + gXSpacing);
        
        Rectangle<int> leftColumn (area);
        Rectangle<int> rightColumn (leftColumn.removeFromRight(leftColumn.getWidth()* 0.5));
        
        leftColumn.removeFromRight(processor.paddingScalarX * 20);
        rightColumn.removeFromLeft(processor.paddingScalarX * 20);
        
        leftColumn.removeFromTop(sliderHeight * 0.5 - gComponentComboBoxHeight);
        Rectangle<int> smoothModeSelectCBRect (leftColumn.removeFromTop(gComponentComboBoxHeight));
        Rectangle<int> smoothModeLabelRect (smoothModeSelectCBRect.removeFromRight(smoothModeSelectCBRect.getWidth()*0.5));
        smoothModeSelectCB.setBounds(smoothModeSelectCBRect);
        smoothModeLabel.setBounds(smoothModeLabelRect);

        area.removeFromTop(sliderHeight + gYSpacing - gComponentComboBoxHeight);
        for (int i = 0; i < paramSliders.size(); i++)
        {
            paramSliders[i]->setBounds(area.removeFromTop(sliderHeight));
            area.removeFromTop(gYSpacing);
        }
    }
    else if (tab == 1)
    {
        BlendronicPreparation::Ptr prep = processor.gallery->getStaticBlendronicPreparation(processor.updateState->currentBlendronicId);
        
        syncModeSelectCB.setVisible(true);
        syncModeLabel.setVisible(true);
        clearModeSelectCB.setVisible(true);
        clearModeLabel.setVisible(true);
        closeModeSelectCB.setVisible(true);
        closeModeLabel.setVisible(true);
        openModeSelectCB.setVisible(true);
        openModeLabel.setVisible(true);
        
        // SET BOUNDS
        int sliderHeight = height * 0.225f;
        
        Rectangle<int> area (getBounds());
        area.removeFromTop(hideOrShow.getBottom() + gYSpacing);
        area.removeFromRight(rightArrow.getWidth() + gXSpacing);
        area.removeFromLeft(leftArrow.getWidth() + gXSpacing);
        
        Rectangle<int> leftColumn (area);
        Rectangle<int> rightColumn (leftColumn.removeFromRight(leftColumn.getWidth()* 0.5));
        
        leftColumn.removeFromRight(processor.paddingScalarX * 20);
        rightColumn.removeFromLeft(processor.paddingScalarX * 20);
        
        leftColumn.removeFromTop(sliderHeight * 0.5 - gComponentComboBoxHeight);
        Rectangle<int> syncModeSelectCBRect (leftColumn.removeFromTop(gComponentComboBoxHeight));
        Rectangle<int> syncModeSelectLabel (syncModeSelectCBRect.removeFromLeft(syncModeSelectCBRect.getWidth()*0.5));
        syncModeSelectCB.setBounds(syncModeSelectCBRect);
        syncModeLabel.setBounds(syncModeSelectLabel);
        
        rightColumn.removeFromTop(sliderHeight * 0.5 - gComponentComboBoxHeight);
        Rectangle<int> clearModeSelectCBRect (rightColumn.removeFromTop(gComponentComboBoxHeight));
        Rectangle<int> clearModeLabelRect (clearModeSelectCBRect.removeFromRight(clearModeSelectCBRect.getWidth()*0.5));
        clearModeSelectCB.setBounds(clearModeSelectCBRect);
        clearModeLabel.setBounds(clearModeLabelRect);
        
        leftColumn.removeFromTop(sliderHeight * 0.5 - gComponentComboBoxHeight);
        Rectangle<int> closeModeSelectCBRect (leftColumn.removeFromTop(gComponentComboBoxHeight));
        Rectangle<int> closeModeSelectLabel (closeModeSelectCBRect.removeFromLeft(closeModeSelectCBRect.getWidth()*0.5));
        closeModeSelectCB.setBounds(closeModeSelectCBRect);
        closeModeLabel.setBounds(closeModeSelectLabel);
        
        rightColumn.removeFromTop(sliderHeight * 0.5 - gComponentComboBoxHeight);
        Rectangle<int> openModeSelectCBRect (rightColumn.removeFromTop(gComponentComboBoxHeight));
        Rectangle<int> openModeLabelRect (openModeSelectCBRect.removeFromRight(openModeSelectCBRect.getWidth()*0.5));
        openModeSelectCB.setBounds(openModeSelectCBRect);
        openModeLabel.setBounds(openModeLabelRect);
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
    smoothModeSelectCB.addListener(this);
    
    fillSmoothModeSelectCB();
    fillSyncModeSelectCB();
    fillClearModeSelectCB();
    fillCloseModeSelectCB();
    fillOpenModeSelectCB();
    
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
        smoothModeSelectCB.setSelectedItemIndex(prep->getSmoothMode(), dontSendNotification);
        syncModeSelectCB.setSelectedItemIndex(prep->getSyncMode(), dontSendNotification);

        for(int i = 0; i < paramSliders.size(); i++)
        {
            if(!paramSliders[i]->getName().compare(cBlendronicParameterTypes[BlendronicBeats]))
            {
                paramSliders[i]->setTo(prep->getBeats(), dontSendNotification);
            }

            if(!paramSliders[i]->getName().compare(cBlendronicParameterTypes[BlendronicSmoothDurations]))
            {
                paramSliders[i]->setTo(prep->getSmoothDurations(), dontSendNotification);
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
    else if (name == "Smooth Mode")
    {
        BlendronicPreparation::Ptr prep = processor.gallery->getStaticBlendronicPreparation(processor.updateState->currentBlendronicId);
        BlendronicPreparation::Ptr active = processor.gallery->getActiveBlendronicPreparation(processor.updateState->currentBlendronicId);
        
        prep    ->setSmoothMode( (BlendronicSmoothMode) index);
        active  ->setSmoothMode( (BlendronicSmoothMode) index);
        
        fillSmoothModeSelectCB();
        
    }
    else if (name == "Sync Mode")
    {
        BlendronicPreparation::Ptr prep = processor.gallery->getStaticBlendronicPreparation(processor.updateState->currentBlendronicId);
        BlendronicPreparation::Ptr active = processor.gallery->getActiveBlendronicPreparation(processor.updateState->currentBlendronicId);
        
        prep    ->setSyncMode( (BlendronicSyncMode) index);
        active  ->setSyncMode( (BlendronicSyncMode) index);
        
        fillSyncModeSelectCB();
        
    }
    else if (name == "Clear Mode")
    {
        BlendronicPreparation::Ptr prep = processor.gallery->getStaticBlendronicPreparation(processor.updateState->currentBlendronicId);
        BlendronicPreparation::Ptr active = processor.gallery->getActiveBlendronicPreparation(processor.updateState->currentBlendronicId);
        
        prep    ->setClearMode( (BlendronicClearMode) index);
        active  ->setClearMode( (BlendronicClearMode) index);
        
        fillClearModeSelectCB();
        
    }
    else if (name == "Open Mode")
    {
        BlendronicPreparation::Ptr prep = processor.gallery->getStaticBlendronicPreparation(processor.updateState->currentBlendronicId);
        BlendronicPreparation::Ptr active = processor.gallery->getActiveBlendronicPreparation(processor.updateState->currentBlendronicId);
        
        prep    ->setOpenMode( (BlendronicOpenMode) index);
        active  ->setOpenMode( (BlendronicOpenMode) index);
        
        fillOpenModeSelectCB();
        
    }
    else if (name == "Close Mode")
    {
        BlendronicPreparation::Ptr prep = processor.gallery->getStaticBlendronicPreparation(processor.updateState->currentBlendronicId);
        BlendronicPreparation::Ptr active = processor.gallery->getActiveBlendronicPreparation(processor.updateState->currentBlendronicId);
        
        prep    ->setCloseMode( (BlendronicCloseMode) index);
        active  ->setCloseMode( (BlendronicCloseMode) index);
        
        fillCloseModeSelectCB();
        
    }
}

void BlendronicPreparationEditor::BKSingleSliderValueChanged(BKSingleSlider* slider, String name, double val)
{
    BlendronicPreparation::Ptr prep = processor.gallery->getStaticBlendronicPreparation(processor.updateState->currentBlendronicId);
    BlendronicPreparation::Ptr active = processor.gallery->getActiveBlendronicPreparation(processor.updateState->currentBlendronicId);
    
//    if(name == "note length multiplier")
//    {
//        //DBG("note length multiplier " + String(val));
//        prep->setLengthMultiplier(val);
//        active->setLengthMultiplier(val);
//    }
//    else if(name == "beats to skip")
//    {
//        //DBG("beats to skip " + String(val));
//        prep->setBeatsToSkip(val);
//        active->setBeatsToSkip(val);
//    }
//    else if (name == "cluster min")
//    {
//        prep->setClusterMin(val);
//        active->setClusterMin(val);
//    }
//    else if (name == "cluster thresh")
//    {
//        prep->setClusterThreshold(val);
//        active->setClusterThreshold(val);
//
//        DBG("setting cluster thresh : " + String(val));
//    }
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

void BlendronicPreparationEditor::fillSmoothModeSelectCB()
{
    BlendronicPreparation::Ptr prep = processor.gallery->getActiveBlendronicPreparation(processor.updateState->currentBlendronicId);
    
    smoothModeSelectCB.clear(dontSendNotification);
    
    smoothModeSelectCB.addItem(cBlendronicSmoothModes[ConstantTimeSmooth], 1);
    smoothModeSelectCB.addItem(cBlendronicSmoothModes[ConstantRateSmooth], 2);
    smoothModeSelectCB.addItem(cBlendronicSmoothModes[ProportionalTimeSmooth], 3);
    smoothModeSelectCB.addItem(cBlendronicSmoothModes[ProportionalRateSmooth], 4);
    
    smoothModeSelectCB.setSelectedItemIndex(prep->getSmoothMode(), dontSendNotification);
}

void BlendronicPreparationEditor::fillSyncModeSelectCB()
{
    BlendronicPreparation::Ptr prep = processor.gallery->getActiveBlendronicPreparation(processor.updateState->currentBlendronicId);
    
    syncModeSelectCB.clear(dontSendNotification);
    
    for (int i = 0; i <= BlendronicSyncModeNil; ++i)
    {
        syncModeSelectCB.addItem(cBlendronicSyncModes[i], i+1);
    }
    
    syncModeSelectCB.setSelectedItemIndex(prep->getSyncMode(), dontSendNotification);
}

void BlendronicPreparationEditor::fillClearModeSelectCB()
{
    BlendronicPreparation::Ptr prep = processor.gallery->getActiveBlendronicPreparation(processor.updateState->currentBlendronicId);
    
    clearModeSelectCB.clear(dontSendNotification);
    
    for (int i = 0; i <= BlendronicClearModeNil; ++i)
    {
        clearModeSelectCB.addItem(cBlendronicClearModes[i], i+1);
    }
    
    clearModeSelectCB.setSelectedItemIndex(prep->getClearMode(), dontSendNotification);
}

void BlendronicPreparationEditor::fillOpenModeSelectCB()
{
    BlendronicPreparation::Ptr prep = processor.gallery->getActiveBlendronicPreparation(processor.updateState->currentBlendronicId);
    
    openModeSelectCB.clear(dontSendNotification);
    
    for (int i = 0; i <= BlendronicOpenModeNil; ++i)
    {
        openModeSelectCB.addItem(cBlendronicOpenModes[i], i+1);
    }
    
    openModeSelectCB.setSelectedItemIndex(prep->getOpenMode(), dontSendNotification);
}

void BlendronicPreparationEditor::fillCloseModeSelectCB()
{
    BlendronicPreparation::Ptr prep = processor.gallery->getActiveBlendronicPreparation(processor.updateState->currentBlendronicId);
    
    closeModeSelectCB.clear(dontSendNotification);
    
    for (int i = 0; i <= BlendronicCloseModeNil; ++i)
    {
        closeModeSelectCB.addItem(cBlendronicCloseModes[i], i+1);
    }
    
    closeModeSelectCB.setSelectedItemIndex(prep->getCloseMode(), dontSendNotification);
}

void BlendronicPreparationEditor::timerCallback()
{
    if (processor.updateState->currentDisplay == DisplayBlendronic)
    {
        BlendronicProcessor::Ptr bProcessor = processor.currentPiano->getBlendronicProcessor(processor.updateState->currentBlendronicId);
        
        if (bProcessor != nullptr)
        {
//            Array<int> currentPlayPositions = bProcessor->getPlayPositions();
//            Array<int> currentUndertowPositions = bProcessor->getUndertowPositions();
//            currentPlayPositions.addArray(currentUndertowPositions);
//
//            nDisplaySlider.updateSliderPositions(currentPlayPositions);
//
//            holdTimeMinMaxSlider->setDisplayValue(bProcessor->getHoldTime());
//            if(bProcessor->getNumActiveNotes())
//            {
//                velocityMinMaxSlider->setDisplayValue(bProcessor->getLastVelocity() * 127.);
//                clusterMinSlider->setDisplayValue(bProcessor->getCurrentClusterSize() + 1);
//                //clusterThresholdSlider->setDisplayValue(bProcessor->getClusterThresholdTimer());
//                if(bProcessor->getCurrentClusterSize()) clusterThresholdSlider->setDisplayValue(bProcessor->getClusterThresholdTimer());
//                else clusterThresholdSlider->setDisplayValue(0);
//            }
//            else
//            {
//                velocityMinMaxSlider->setDisplayValue(0);
//                clusterMinSlider->setDisplayValue(0);
//                clusterThresholdSlider->setDisplayValue(0);
//            }
            
            
            
            BlendronicPreparation::Ptr active = processor.gallery->getActiveBlendronicPreparation(processor.updateState->currentBlendronicId);
//            if(active->getMode() == NoteLengthSync)
//            {
//                holdTimeMinMaxSlider->setBright();
//                holdTimeMinMaxSlider->setEnabled(true);
//
//                clusterMinSlider->setBright();
//                clusterMinSlider->setEnabled(true);
//
//                if(active->getClusterMin() > 1)
//                {
//                    clusterThresholdSlider->setBright();
//                    clusterThresholdSlider->setEnabled(true);
//                }
//                else
//                {
//                    clusterThresholdSlider->setDim(gModAlpha);
//                    clusterThresholdSlider->setEnabled(false);
//                }
//            }
//            else
//            {
//                holdTimeMinMaxSlider->setDim(gModAlpha);
//                holdTimeMinMaxSlider->setEnabled(false);
//
//                clusterMinSlider->setDim(gModAlpha);
//                clusterMinSlider->setEnabled(false);
//
//                clusterThresholdSlider->setDim(gModAlpha);
//                clusterThresholdSlider->setEnabled(false);
//            }
            
            
        }
    }
}

void BlendronicPreparationEditor::multiSliderDidChange(String name, int whichSlider, Array<float> values)
{
    BlendronicPreparation::Ptr prep = processor.gallery->getStaticBlendronicPreparation(processor.updateState->currentBlendronicId);
    BlendronicPreparation::Ptr active = processor.gallery->getActiveBlendronicPreparation(processor.updateState->currentBlendronicId);
    
    if (name == "beats")
    {
        prep    ->setBeat(whichSlider, values[0]);
        active  ->setBeat(whichSlider, values[0]);
    }
    else if (name == "smooth timings")
    {
        prep    ->setSmoothDuration(whichSlider, values[0]);
        active  ->setSmoothDuration(whichSlider, values[0]);
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
    
    if (name == "beats")
    {
        prep    ->setBeats(newvals);
        active  ->setBeats(newvals);
    }
    else if (name == "smooth timings")
    {
        prep    ->setSmoothDurations(newvals);
        active  ->setSmoothDurations(newvals);
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
//    else if (b == &keyOnResetToggle)
//    {
//        bool state = b->getToggleState();
//        prep->setKeyOnReset(state);
//        active->setKeyOnReset(state);
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
    holdTimeMinMaxSlider->setDim(gModAlpha);
    velocityMinMaxSlider->setDim(gModAlpha);
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

void BlendronicModificationEditor::fillSmoothModeSelectCB()
{
    BlendronicModification::Ptr prep = processor.gallery->getBlendronicModification(processor.updateState->currentModBlendronicId);
    
    smoothModeSelectCB.clear(dontSendNotification);
    
    smoothModeSelectCB.addItem(cBlendronicSmoothModes[ConstantTimeSmooth], 1);
    smoothModeSelectCB.addItem(cBlendronicSmoothModes[ConstantRateSmooth], 2);
    smoothModeSelectCB.addItem(cBlendronicSmoothModes[ProportionalTimeSmooth], 3);
    smoothModeSelectCB.addItem(cBlendronicSmoothModes[ProportionalRateSmooth], 4);
    
    smoothModeSelectCB.setSelectedItemIndex(prep->getSmoothMode(), dontSendNotification);
}

void BlendronicModificationEditor::fillSyncModeSelectCB()
{
    BlendronicModification::Ptr prep = processor.gallery->getBlendronicModification(processor.updateState->currentModBlendronicId);
    
    syncModeSelectCB.clear(dontSendNotification);
    
    for (int i = 0; i <= BlendronicSyncModeNil; ++i)
    {
        syncModeSelectCB.addItem(cBlendronicSyncModes[i], i+1);
    }
    
    syncModeSelectCB.setSelectedItemIndex(prep->getSyncMode(), dontSendNotification);
}

void BlendronicModificationEditor::fillClearModeSelectCB()
{
    BlendronicModification::Ptr prep = processor.gallery->getBlendronicModification(processor.updateState->currentModBlendronicId);
    
    clearModeSelectCB.clear(dontSendNotification);
    
    for (int i = 0; i <= BlendronicClearModeNil; ++i)
    {
        clearModeSelectCB.addItem(cBlendronicClearModes[i], i+1);
    }
    
    clearModeSelectCB.setSelectedItemIndex(prep->getClearMode(), dontSendNotification);
}



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