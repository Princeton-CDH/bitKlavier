/*
  ==============================================================================

    ResonanceViewController.cpp
    Created: 24 May 2021 10:48:25pm
    Author:  Theodore R Trevisan

  ==============================================================================
*/

#include "ResonanceViewController.h"

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////ResonanceViewController/////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

ResonanceViewController::ResonanceViewController(BKAudioProcessor& p, BKItemGraph* theGraph) :
    BKViewController(p, theGraph, 2),
#if JUCE_IOS
gainsKeyboard(false, true),
offsetsKeyboard(false, true)
#else
gainsKeyboard(false, false),
offsetsKeyboard(false, true)
#endif
{
    setLookAndFeel(&buttonsAndMenusLAF);

    iconImageComponent.setImage(ImageCache::getFromMemory(BinaryData::direct_icon_png, BinaryData::direct_icon_pngSize));
    iconImageComponent.setImagePlacement(RectanglePlacement(juce::RectanglePlacement::stretchToFit));
    iconImageComponent.setAlpha(0.095);

    selectCB.setName("Resonance");
    selectCB.addSeparator();
    selectCB.setSelectedItemIndex(0);
    selectCB.addListener(this);
    selectCB.setTooltip("Select from available saved preparation settings");
    addAndMakeVisible(selectCB);

    defGainSlider = std::make_unique<BKSingleSlider>("resonant note volume (dB)", cResonanceDefGain, -100, 24, 0, 0.01, "-inf");
    defGainSlider->setJustifyRight(false);
    defGainSlider->setToolTipString("Adjusts overall volume of resonant notes");
    addAndMakeVisible(*defGainSlider);
    
    blendGainSlider = std::make_unique<BKSingleSlider>("blendronic gain", cBlendronicOutGain, -100, 24, 0, 0.01, "-inf");
    blendGainSlider->setJustifyRight(false);
    blendGainSlider->setToolTipString("Adjusts overall volume of resonant notes");
    addAndMakeVisible(*blendGainSlider);

//    startTimeSlider = std::make_unique<BKSingleSlider>("start time (ms)", cResonanceStartTime, -4000, 4000, 2000, 1);
    
    startTimeSlider = std::make_unique<BKRangeSlider>("start time (ms)", 0, 4000, 0, 4000, 1);
    startTimeSlider->setJustifyRight(false);
    startTimeSlider->setToolTipString("Adjusts start time into sample of resonant notes");
    addAndMakeVisible(*startTimeSlider);

    maxSympStringsSlider =std::make_unique<BKSingleSlider>("max sympathetic strings", "maxsymp", 0, 20, 20, 1, "-inf");
    maxSympStringsSlider->setJustifyRight(false);
    maxSympStringsSlider->setToolTipString("Adjusts maximum number of sympathetic strings");
    addAndMakeVisible(*maxSympStringsSlider);

    ADSRSlider = std::make_unique<BKADSRSlider>("Resonance envelope");
    ADSRSlider->setButtonText("edit resonance envelope");
    ADSRSlider->setToolTip("ADSR settings for resonant notes");
    ADSRSlider->setButtonMode(false);
    addAndMakeVisible(*ADSRSlider);

    ADSRLabel.setText("ADSR", dontSendNotification);
    ADSRLabel.setJustificationType(Justification::centred);
    addAndMakeVisible(&ADSRLabel, ALL);
    
    NUM_KEYS = 52;
    closestKeyboard = std::make_unique<BKKeymapKeyboardComponent> (resonanceKeyboardState, BKKeymapKeyboardComponent::horizontalKeyboard);
    closestKeyboard->setName("absolute");
//    closestKeyboard.setAlpha(1);
    //closestKeyboard->setAvailableRange(24, 24+NUM_KEYS);
    closestKeyboard->setAvailableRange(0, NUM_KEYS);
    closestKeyboard->setOctaveForMiddleC(4);
    closestKeyboard->setScrollButtonsVisible(true);
    closestKeyboard->setOctaveForMiddleC(5);
    addAndMakeVisible(*closestKeyboard);
    
    fundamentalKeyboard = std::make_unique<BKKeymapKeyboardComponent> (fundamentalKeyboardState, BKKeymapKeyboardComponent::horizontalKeyboard);
    fundamentalKeyboard->setName("fundamental");
//    closestKeyboard.setAlpha(1);
    //fundamentalKeyboard->setAvailableRange(24, 24+NUM_KEYS);
    fundamentalKeyboard->setAvailableRange(0, NUM_KEYS);
    fundamentalKeyboard->setOctaveForMiddleC(4);
    fundamentalKeyboard->setScrollButtonsVisible(false);
    fundamentalKeyboard->setOctaveForMiddleC(5);
    fundamentalKeyboard->setKeysInKeymap(0);
    addAndMakeVisible(*fundamentalKeyboard);
    
    addAndMakeVisible(&actionButton, ALL);
    actionButton.setButtonText("Action");
    actionButton.setTooltip("Create, duplicate, rename, delete, or reset current settings");
    actionButton.addListener(this);
    
    //gainsKeyboard.setAvailableRange(24, 24+NUM_KEYS);
    gainsKeyboard.setAvailableRange(0, NUM_KEYS);
    gainsKeyboard.setName("gain");
    gainsKeyboard.addMouseListener(this, true);
    gainsKeyboard.setOctaveForMiddleC(5);
    gainsKeyboard.setMinMidMaxValues(0.1, 1., 10., 2); // min, mid, max, display resolution
    gainsKeyboard.disableAllKeys();
    addAndMakeVisible(&gainsKeyboard);

    //offsetsKeyboard.setAvailableRange(24, 24+NUM_KEYS);
    offsetsKeyboard.setAvailableRange(0, NUM_KEYS);
    offsetsKeyboard.setName("offset");
    offsetsKeyboard.addMouseListener(this, true);
    offsetsKeyboard.setOctaveForMiddleC(5);
    offsetsKeyboard.disableAllKeys();
    addAndMakeVisible(&offsetsKeyboard);


//    float offsetMin  = -100.;
//   float offsetMax  = 100.;
//   float offsetIncrement = 0.1;
//   float offsetDefault = 0.;
//
//   float gainMin = 0.1;
//   float gainMax = 10.;
//   float gainIncrement = 0.1;
//   float gainDefault = 1;
//  String subSliderName = "add subslider";
//   int  sliderHeight = 60;
//
//    for (int i = 0; i < NUM_KEYS; i++)
//    {
//        BKSubSlider* refSlider = new BKSubSlider(Slider::LinearBarVertical,
//                                                                 offsetMin,
//                                                                 offsetMax,
//                                                                 offsetDefault,
//                                                                 offsetIncrement,
//                                                                 20,
//                                                                sliderHeight);
////        refSlider->setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxLeft, true, 0,0);
//        refSlider->addMouseListener(this, true);
//        refSlider->setName("offset" + String(i));
//        refSlider->setTooltip(String(refSlider->getValue()));
////        refSlider->setInterceptsMouseClicks(false, false);
////        refSlider->setLookAndFeel(&displaySliderLookAndFeel);
//        addAndMakeVisible(*refSlider);
//
//        offsetsArray.add(refSlider);
//
//        BKSubSlider* refgainSlider = new BKSubSlider(Slider::LinearBarVertical,
//                                                                 gainMin,
//                                                                 gainMax,
//                                                                 gainDefault,
//                                                                 gainIncrement,
//                                                                 20,
//                                                                sliderHeight);
//        refgainSlider->setName("gain" + String(i));
//        addAndMakeVisible(*refgainSlider);
//        gainsArray.add(refgainSlider);
//
//        isActive[i] = false;
////        gainsArray.add(std::make_unique<BKSingleSlider>("gain" + String(i), 10, -100, 24, 1, 0.01, "-inf"));
//
//    }
    
    closestKeyLabel.setText("Resonance: ", dontSendNotification);
    closestKeyLabel.setJustificationType(Justification::right);
    addAndMakeVisible(closestKeyLabel);
    
    fundamentalLabel.setText("Fundamental: ", dontSendNotification);
    fundamentalLabel.setJustificationType(Justification::right);
    addAndMakeVisible(fundamentalLabel);
    
    gainsLabel.setText("Gains: ", dontSendNotification);
    gainsLabel.setJustificationType(Justification::right);
    addAndMakeVisible(gainsLabel);
    
    offsetsLabel.setText("Offsets: ", dontSendNotification);
    offsetsLabel.setJustificationType(Justification::right);
    addAndMakeVisible(offsetsLabel);


    //will need to do more with blendronic, modifications, etc eventually

    currentTab = 0;
    displayTab(currentTab);
}

void ResonanceViewController::paint(Graphics& g)
{
    g.fillAll(Colours::black);
}

void ResonanceViewController::resized()
{
    displayShared();
    displayTab(currentTab);

    //will have to put big '#if 0' here eventually
}

void ResonanceViewController::displayTab(int tab)
{
    currentTab = tab;

    invisible();
    displayShared();

//    int x0 = leftArrow.getRight() + gXSpacing;
//    int y0 = hideOrShow.getBottom() + gYSpacing;
//    int right = rightArrow.getX() - gXSpacing;
//    int width = right - x0;
//    int height = getHeight() - y0;

    Rectangle<int> area(getLocalBounds());
    area.removeFromLeft(leftArrow.getWidth());
    area.removeFromRight(rightArrow.getWidth());
    
//    area.removeFromTop(gComponentComboBoxHeight);

     if (tab == 0){
        
#if JUCE_IOS
//        area.removeFromTop(gComponentComboBoxHeight);
        area.reduce(0.f, area.getHeight() * 0.2f);
#endif
        
        ADSRSlider->setVisible(true);
        ADSRLabel.setVisible(true);

        //area = (getLocalBounds());
       area.reduce(20 * processor.paddingScalarX + 4, 20 * processor.paddingScalarY + 4);
        
        //single parameter sliders

        int columnHeight = area.getHeight();
        
        area.removeFromTop(columnHeight/5);
 
        int columnWidth = area.getWidth()/2;
        
        Rectangle<int> firstRow = area.removeFromTop(columnHeight / 5);
        defGainSlider->setBounds(firstRow.removeFromLeft(columnWidth));
        defGainSlider->setVisible(true);
        
        blendGainSlider->setBounds(firstRow);
        blendGainSlider->setVisible(true);
        
        Rectangle<int> secondRow = area.removeFromTop(columnHeight / 5);

        startTimeSlider->setBounds(secondRow.removeFromLeft(columnWidth));
        startTimeSlider->setVisible(true);
        
        maxSympStringsSlider->setBounds(secondRow);
        maxSympStringsSlider->setVisible(true);
                
        ADSRLabel.setBounds(area.removeFromTop(columnHeight / 8));
        ADSRSlider->setBounds(area.removeFromTop(columnHeight / 5));
    }
    else if (tab == 1){
        iconImageComponent.setBounds(area);
//        area.reduce(x0 + 10 * processor.paddingScalarX + 4, 10 * processor.paddingScalarY + 4);
        
#if JUCE_IOS
//        area.removeFromTop(gComponentComboBoxHeight);
        area.reduce(0.f, area.getHeight() * 0.2f);
#endif
        area.removeFromBottom(40 * processor.paddingScalarY);
        
        float keyboardHeight = 80 * processor.paddingScalarY;
        float columnWidth = area.getWidth() * 0.15 + processor.paddingScalarX;
        
        Rectangle<int> fundamentalKeyboardRow = area.removeFromBottom(keyboardHeight);
        
        fundamentalLabel.setBounds(fundamentalKeyboardRow.removeFromLeft(columnWidth));
        fundamentalLabel.setVisible(true);
        
//        float keyWidth = fundamentalKeyboardRow.getWidth() / round((NUM_KEYS) * 7. / 12); //num white keys, gives error
        
        float keyWidth = fundamentalKeyboardRow.getWidth() / 31.; // 31 is number of white keys
        
        DBG("Keyboard row width: " + String(fundamentalKeyboardRow.getWidth()));
        DBG("Keyboard width" + String(fundamentalKeyboard->getWidth()));

        fundamentalKeyboard->setKeyWidth(keyWidth);
        fundamentalKeyboard->setBlackNoteLengthProportion(0.6);
        fundamentalKeyboard->setBounds(fundamentalKeyboardRow);
        fundamentalKeyboard->setVisible(true);
        
        area.removeFromBottom(processor.paddingScalarX * 10);
        area.removeFromBottom(0.2 * keyboardHeight + gYSpacing);
        
        Rectangle<int> closestKeyboardRow = area.removeFromBottom(keyboardHeight);
        
        closestKeyLabel.setBounds(closestKeyboardRow.removeFromLeft(columnWidth));
        closestKeyLabel.setVisible(true);

        closestKeyboard->setKeyWidth(keyWidth);
        closestKeyboard->setBlackNoteLengthProportion(0.6);
        closestKeyboard->setBounds(closestKeyboardRow);
        closestKeyboard->setVisible(true);
    
        area.removeFromBottom(processor.paddingScalarY * 10);
        area.removeFromBottom(0.2 * keyboardHeight + gYSpacing);

        float multiHeight = 100 + processor.paddingScalarY;
        
        Rectangle<int> offsetsRow = area.removeFromBottom(multiHeight);
        offsetsLabel.setBounds(offsetsRow.removeFromLeft(columnWidth));
        offsetsLabel.setVisible(true);
        
//        float sliderWidth = offsetsRow.getWidth()/ (NUM_KEYS * 1.);
//
//        DBG("Slider width:" + String(sliderWidth));
//        DBG("Offsets row width: " + String(offsetsRow.getWidth()));
//
//        float sum = 0;
//
//        for (int i = 0; i < NUM_KEYS; i++) {
//            offsetsArray[i]->setBounds(offsetsRow.removeFromLeft(sliderWidth));
//            sum += sliderWidth;
//            if (isActive[i])
//                offsetsArray[i]->setVisible(true);
//        }
//
//        DBG("slider width sum: " + String(sum));
        
        offsetsKeyboard.setBounds(offsetsRow);
        offsetsKeyboard.setVisible(true);
        
        area.removeFromBottom(10 * processor.paddingScalarY);
        
        Rectangle<int> gainsRow = area.removeFromBottom(multiHeight);
        
        gainsLabel.setBounds(gainsRow.removeFromLeft(columnWidth));
        gainsLabel.setVisible(true);
        
//        for (int i = 0; i < NUM_KEYS; i++) {
//            gainsArray[i]->setBounds(gainsRow.removeFromLeft(sliderWidth));
//            if (isActive[i])
//                gainsArray[i]->setVisible(true);
//        }
        
        gainsKeyboard.setBounds(gainsRow);
        gainsKeyboard.setVisible(true);

        area.removeFromBottom(10 * processor.paddingScalarY);
        
    }

    //all the display code is in displayShared for now, some will get moved here for when multiple tabs are implemented
}

void ResonanceViewController::displayShared(void)
{
    Rectangle<int> area(getBounds());

    iconImageComponent.setBounds(area);

    area.reduce(10 * processor.paddingScalarX + 4, 10 * processor.paddingScalarY + 4);

    Rectangle<int> leftColumn = area.removeFromLeft(area.getWidth() * 0.5);
    Rectangle<int> comboBoxSlice = leftColumn.removeFromTop(gComponentComboBoxHeight);
    comboBoxSlice.removeFromRight(4 + 2.0f * gPaddingConst * processor.paddingScalarX);
    comboBoxSlice.removeFromLeft(gXSpacing);
    hideOrShow.setBounds(comboBoxSlice.removeFromLeft(gComponentComboBoxHeight));
    comboBoxSlice.removeFromLeft(gXSpacing);
    selectCB.setBounds(comboBoxSlice.removeFromLeft(comboBoxSlice.getWidth() / 2.));

    actionButton.setBounds(selectCB.getRight()+gXSpacing,
                           selectCB.getY(),
                           selectCB.getWidth() * 0.5,
                           selectCB.getHeight());
    
    comboBoxSlice.removeFromLeft(gXSpacing);

    Rectangle<int> modeSlice = area.removeFromTop(gComponentComboBoxHeight);
    modeSlice.removeFromRight(gXSpacing);
    lengthModeSelectCB.setBounds(modeSlice.removeFromRight(modeSlice.getWidth() / 2.));

    actionButton.toFront(false);
    
    leftArrow.setBounds(0, getHeight() * 0.4, 50, 50);
    rightArrow.setBounds(getRight() - 50, getHeight() * 0.4, 50, 50);
    
    
}
void ResonanceViewController::invisible(void)
{
    defGainSlider->setVisible(false);
    startTimeSlider->setVisible(false);
    maxSympStringsSlider->setVisible(false);
    blendGainSlider->setVisible(false);
    
    gainsLabel.setVisible(false);
    offsetsLabel.setVisible(false);
    ADSRSlider->setVisible(false);
    ADSRLabel.setVisible(false);
    
    closestKeyboard->setVisible(false);
    fundamentalKeyboard->setVisible(false);
    
    gainsKeyboard.setVisible(false);
    offsetsKeyboard.setVisible(false);
    
//    for (int i = 0; i < NUM_KEYS; i++) {
//        offsetsArray[i]->setVisible(false);
//        gainsArray[i]->setVisible(false);
//    }

    closestKeyLabel.setVisible(false);
    fundamentalLabel.setVisible(false);

}
//
//void ResonanceViewController::mouseEnter(const MouseEvent& e)
//{
//}




////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////ResonancePreparationEditor////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

ResonancePreparationEditor::ResonancePreparationEditor(BKAudioProcessor& p, BKItemGraph* theGraph) :
    ResonanceViewController(p, theGraph)
{
    selectCB.addMyListener(this);
    selectCB.addListener(this);

    fillSelectCB(-1, -1);

    defGainSlider->addMyListener(this);
    startTimeSlider->addMyListener(this);
    blendGainSlider->addMyListener(this);
    maxSympStringsSlider->addMyListener(this);

    ADSRSlider->addMyListener(this);
    resonanceKeyboardState.addListener(this);
    fundamentalKeyboardState.addListener(this);
    
    offsetsKeyboard.addMyListener(this);
    gainsKeyboard.addMyListener(this);

    // startTimer(30);
}

void ResonancePreparationEditor::update()
{
    if (processor.updateState->currentResonanceId < 0) return;
    ADSRSlider->setIsButtonOnly(true);

    ResonancePreparation::Ptr prep = processor.gallery->getResonancePreparation(processor.updateState->currentResonanceId);

    if (prep != nullptr)
    {
        selectCB.setSelectedId(processor.updateState->currentResonanceId, dontSendNotification);

        defGainSlider->setValue(prep->getDefGain(), dontSendNotification);
        blendGainSlider->setValue(prep->getBlendGain(), dontSendNotification);
        maxSympStringsSlider->setValue(prep->getMaxSympStrings(), dontSendNotification);
        
        startTimeSlider->setMinValue(prep->getMinStartTime(), dontSendNotification);
        startTimeSlider->setMaxValue(prep->getMaxStartTime(), dontSendNotification);
    
        Array<float> ADSRarray = prep->getADSRvals();
        ADSRSlider->setAttackValue(ADSRarray[0], dontSendNotification);
        ADSRSlider->setDecayValue(ADSRarray[1], dontSendNotification);
        ADSRSlider->setSustainValue(ADSRarray[2], dontSendNotification);
        ADSRSlider->setReleaseValue(ADSRarray[3], dontSendNotification);
        
//        for (int i = 0; i < NUM_KEYS; i++){
//            if (prep->isActive(i + 24)) {
//                gainsArray[i]->setVisible(true);
//                gainsArray[i]->setValue(prep->getGain(i), dontSendNotification);
//                offsetsArray[i]->setVisible(true);
//                offsetsArray[i]->setValue(prep->getOffset(i), dontSendNotification);
//                isActive[i] = true;
//            }
//            else {
//                gainsArray[i]->setVisible(false);
//                offsetsArray[i]->setVisible(false);
//                isActive[i] = false;
//            }
//        }
//
        Array<float> offsets;
        Array<float> gains;
//
//        for (int i = 0; i < 24; i++) {
//            offsets.add(0);
//            gains.add(0);
//        }
//
        /*
        for (int i = 24; i < 24+NUM_KEYS; i++) {
            offsets.add(prep->getOffset(i));
            gains.add(prep->getGain(i));
        }
         */
        
//        Array<float> offsets = Array<float>(prep->getOffsets());
//        Array<float> gains = Array<float>(prep->getGains());

      //        Array<float> gains;
        offsetsKeyboard.setValues(offsets);
        gainsKeyboard.setValues(gains);

        /*
        closestKeyboard->setKeysInKeymap(prep->getKeys());
        fundamentalKeyboard->setKeysInKeymap({prep->getFundamental()});
        */
    }
}

void ResonancePreparationEditor::fillSelectCB(int last, int current)
{   selectCB.clear(dontSendNotification);
    
    for (auto prep : processor.gallery->getAllResonance())
    {
        int Id = prep->getId();;
        String name = prep->getName();
        
        if (name != String())  selectCB.addItem(name, Id);
        else                        selectCB.addItem("ResonanceMod"+String(Id), Id);
        
        selectCB.setItemEnabled(Id, true);
        if (processor.currentPiano->isActive(PreparationTypeResonance, Id))
            selectCB.setItemEnabled(Id, false);
    }
    
    if (last != 0)      selectCB.setItemEnabled(last, true);
    if (current != 0)   selectCB.setItemEnabled(current, false);
    
    int selectedId = processor.updateState->currentResonanceId;
    
    selectCB.setSelectedId(selectedId, NotificationType::dontSendNotification);
    
    selectCB.setItemEnabled(selectedId, false);
    
    lastId = selectedId;
}

void ResonancePreparationEditor::timerCallback()
{
//    Resonance::Ptr rs = processor.gallery->getResonance(processor.updateState->currentId);
//
//    DBG("Ringing: " + rs->getRinging());
}

void ResonancePreparationEditor::actionButtonCallback(int action, ResonancePreparationEditor* vc)
{
    if (vc == nullptr)
    {
        PopupMenu::dismissAllActiveMenus();
        return;
    }
    
    BKAudioProcessor& processor = vc->processor;
    if (action == 1)
    {
        int Id = vc->addPreparation();
        vc->setCurrentId(Id);
        processor.saveGalleryToHistory("New Resonance Preparation");
    }
    else if (action == 2)
    {
        int Id = vc->duplicatePreparation();
        vc->setCurrentId(Id);
        processor.saveGalleryToHistory("Duplicate Resonance Preparation");
    }
    else if (action == 3)
    {
        vc->deleteCurrent();
        processor.saveGalleryToHistory("Delete Resonance Preparation");
    }
    else if (action == 4)
    {
        processor.reset(PreparationTypeTempo, processor.updateState->currentResonanceId);
        vc->update();
    }
    else if (action == 5)
    {
        processor.clear(PreparationTypeTempo, processor.updateState->currentResonanceId);
        vc->update();
        processor.saveGalleryToHistory("Clear Resonance Preparation");
    }
    else if (action == 6)
    {
        AlertWindow prompt("", "", AlertWindow::AlertIconType::QuestionIcon);
        
        int Id = processor.updateState->currentTempoId;
        Tempo::Ptr prep = processor.gallery->getTempo(Id);
        
        prompt.addTextEditor("name", prep->getName());
        
        prompt.addButton("Ok", 1, KeyPress(KeyPress::returnKey));
        prompt.addButton("Cancel", 2, KeyPress(KeyPress::escapeKey));
        
        int result = prompt.runModalLoop();
        
        String name = prompt.getTextEditorContents("name");
        
        if (result == 1)
        {
            prep->setName(name);
            vc->fillSelectCB(Id, Id);
            processor.saveGalleryToHistory("Rename Resonance Preparation");
        }
        
        vc->update();
    }
    else if (action == 7)
    {
        AlertWindow prompt("", "", AlertWindow::AlertIconType::QuestionIcon);
        
        int Id = processor.updateState->currentTempoId;
        Tempo::Ptr prep = processor.gallery->getTempo(Id);
        
        prompt.addTextEditor("name", prep->getName());
        
        prompt.addButton("Ok", 1, KeyPress(KeyPress::returnKey));
        prompt.addButton("Cancel", 2, KeyPress(KeyPress::escapeKey));
        
        int result = prompt.runModalLoop();
        
        String name = prompt.getTextEditorContents("name");
        
        DBG("name: " + String(name));
        
        if (result == 1)
        {
            processor.exportPreparation(PreparationTypeResonance, Id, name);
        }
    }
    else if (action >= 100)
    {
        int which = action - 100;
        processor.importPreparation(PreparationTypeTempo, processor.updateState->currentTempoId, which);
        vc->update();
        processor.saveGalleryToHistory("Import Tempo Preparation");
    }
}

void ResonancePreparationEditor::closeSubWindow()
{
    ADSRSlider->setIsButtonOnly(true);
    //setShowADSR(false);
    setSubWindowInFront(false);
}

int ResonancePreparationEditor::addPreparation(void)
{
    processor.gallery->add(PreparationTypeResonance);

    return processor.gallery->getAllResonance().getLast()->getId();
}

int ResonancePreparationEditor::duplicatePreparation(void)
{
    processor.gallery->duplicate(PreparationTypeResonance, processor.updateState->currentResonanceId);

    return processor.gallery->getAllResonance().getLast()->getId();
}

void ResonancePreparationEditor::setCurrentId(int Id)
{
    processor.updateState->currentResonanceId = Id;

    processor.updateState->idDidChange = true;

    update();

    fillSelectCB(lastId, Id);

    lastId = Id;
}

void ResonancePreparationEditor::deleteCurrent(void)
{
    int ResonanceId = selectCB.getSelectedId();
    int index = selectCB.getSelectedItemIndex();

    if ((index == 0) && (selectCB.getItemId(index + 1) == -1)) return;

    processor.gallery->remove(PreparationTypeResonance, ResonanceId);

    fillSelectCB(0, 0);

    selectCB.setSelectedItemIndex(0, dontSendNotification);
    int newId = selectCB.getSelectedId();

    setCurrentId(newId);
}

void ResonancePreparationEditor::bkMessageReceived(const String& message)
{
    if (message == "resonance/update")
    {
        update();
    }
}

void ResonancePreparationEditor::bkComboBoxDidChange(ComboBox* box)
{
    //will need to fill this in eventually, should be fine empty for now
}

void ResonancePreparationEditor::buttonClicked(Button* b)
{
    if (b == &hideOrShow)
    {
        processor.updateState->setCurrentDisplay(DisplayNil);

        ADSRSlider->setIsButtonOnly(true);
        //setShowADSR(false);
        setSubWindowInFront(false);

    }
    else if (b == &actionButton)
    {
        bool single = processor.gallery->getAllResonance().size() == 2;
        getPrepOptionMenu(PreparationTypeResonance, single).showMenuAsync (PopupMenu::Options().withTargetComponent (&actionButton), ModalCallbackFunction::forComponent (actionButtonCallback, this) );
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
        processor.updateState->editsMade = true;
}

void ResonancePreparationEditor::BKEditableComboBoxChanged(String name, BKEditableComboBox* cb)
{
    //will need to fill this in eventually, should be fine empty for now
}

void ResonancePreparationEditor::BKSingleSliderValueChanged(BKSingleSlider* slider, String name, double val)
{
    ResonancePreparation::Ptr prep = processor.gallery->getResonancePreparation(processor.updateState->currentResonanceId);

    if (name == defGainSlider->getName())
    {
        DBG("set resonant note volume to " + String(val));
        prep->setDefGain(val);
    }
    else if (name == blendGainSlider->getName())
    {
        DBG("set blendronic gain to " + String(val));
        prep->setBlendGain(val);
    }
    else if (name == maxSympStringsSlider->getName())
    {
        DBG("set max symp strings to " + String(val));
        prep->setMaxSympStrings(val);
//    }
//    else if (slider == offsetsArray[0]){
//        DBG("first offset slider changed");
//    }
//    else {
//        for (int i = 0; i < offsetsArray.size(); i++){
//            if (name == offsetsArray[i]->getName()) {
//                prep->setOffset(i, val);
//                DBG("set" + String(i) + "offset to " + String(val));
//            }
//        }
//        for (int i = 0; i < gainsArray.size(); i++){
//            if (name == gainsArray[i]->getName()) {
//                prep->setGain(i, val);
//                DBG("set" + String(i) + "gain to " + String(val));
//            }
//        }
}
    processor.updateState->editsMade = true;
}

void ResonancePreparationEditor::BKStackedSliderValueChanged(String name, Array<float> val)
{
    DBG("received overtone slider " + name);

    ResonancePreparation::Ptr prep = processor.gallery->getResonancePreparation(processor.updateState->currentResonanceId);

    //prep->setDistances(val);

    processor.updateState->editsMade = true;
}

void ResonancePreparationEditor::BKADSRSliderValueChanged(String name, int attack, int decay, float sustain, int release)
{
    DBG("received ADSR slider " + name);

    ResonancePreparation::Ptr prep = processor.gallery->getResonancePreparation(processor.updateState->currentResonanceId);

    prep->setADSRvals(attack, decay, sustain, release);

    processor.updateState->editsMade = true;
}

void ResonancePreparationEditor::BKADSRButtonStateChanged(String name, bool mod, bool state)
{
    //will figure out later, probably not necessary right now

    //setShowADSR(name, !state);
    //setSubWindowInFront(!state);
}

void ResonancePreparationEditor::BKRangeSliderValueChanged(String name, double minval, double maxval)
{
    ResonancePreparation::Ptr prep = processor.gallery->getResonancePreparation(processor.updateState->currentResonanceId);
    
    if(name == startTimeSlider->getName()) {
        DBG("got new AdaptiveTempo 1 time diff min/max " + String(minval) + " " + String(maxval));
        prep->setMinStartTime(minval);
        prep->setMaxStartTime(maxval);
    }
    
    processor.updateState->editsMade = true;
}

void ResonancePreparationEditor::handleKeymapNoteToggled(BKKeymapKeyboardState* source, int midiNoteNumber) {
    ResonancePreparation::Ptr prep = processor.gallery->getResonancePreparation(processor.updateState->currentResonanceId);
    if (source == &resonanceKeyboardState)
    {
        /*
        if (prep->isActive(midiNoteNumber)) {
            prep->removeActive(midiNoteNumber);
            isActive[midiNoteNumber - 24] = false;
            DBG("removing active" + String(midiNoteNumber));
        }
        else {
            prep->addActive(midiNoteNumber, 1.0, 0);
            isActive[midiNoteNumber - 24] = true;
            DBG("adding active"+ String(midiNoteNumber));
        }
         */
        
        //closestKeyboard->setKeysInKeymap(prep->getKeys());
        DBG("resonanceKeyboardState " + String(midiNoteNumber));
        prep->toggleResonanceKey(midiNoteNumber);
        closestKeyboard->setKeysInKeymap(prep->getResonanceKeys());
        
        offsetsKeyboard.setKeys(prep->getResonanceKeys());
        offsetsKeyboard.setValues(prep->getOffsets());
        
        gainsKeyboard.setKeys(prep->getResonanceKeys());
        gainsKeyboard.setValues(prep->getGains());
         

    }
    else if (source == &fundamentalKeyboardState)
    {
        prep->setFundamentalKey(midiNoteNumber); 
        fundamentalKeyboard->setKeysInKeymap({midiNoteNumber});
        DBG("fundamental key toggled " + String(midiNoteNumber));
    }
    update();

}

void ResonancePreparationEditor::keyboardSliderChanged(String name, Array<float> values)
{
        ResonancePreparation::Ptr prep = processor.gallery->getResonancePreparation(processor.updateState->currentResonanceId);
     
        if(name == gainsKeyboard.getName())
        {
            DBG("updating gain vals");
            prep->setGains(values);
            
            float sum = 0;
            for (float i : values) {
                //DBG(String(i));
                if(i != 0) {
                    DBG(String(sum) + " = " + String(i));
                }
                sum+=1;
            }
            //DBG("sum: " + String(sum));
        }
        else if(name == offsetsKeyboard.getName())
        {
            DBG("updating offset vals");
            prep->setOffsets(values);
            
            float sum = 0;
            for (float i : values) {
                //DBG(String(i));
                if(i != 0) {
                    DBG(String(sum) + " = " + String(i));
                }
                sum+=1;
            }
            
        }
        processor.gallery->setGalleryDirty(true);
        
        processor.updateState->editsMade = true;
    }

