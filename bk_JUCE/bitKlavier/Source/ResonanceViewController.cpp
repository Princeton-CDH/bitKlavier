/*
  ==============================================================================

    ResonanceViewController.cpp
    Created: 24 May 2021 10:48:25pm
    Author:  Theodore R Trevisan

  ==============================================================================
*/

#include "ResonanceViewController.h"
#include "GenericPreparation.h"
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////ResonanceViewController/////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

ResonanceViewController::ResonanceViewController(BKAudioProcessor& p, BKItemGraph* theGraph) :
    BKViewController(p, theGraph, 3),
#if JUCE_IOS
gainsKeyboard(false, true),
offsetsKeyboard(false, true)
#else
gainsKeyboard(false, false),
offsetsKeyboard(false, true)
#endif
{
    setLookAndFeel(&buttonsAndMenusLAF);

    iconImageComponent.setImage(ImageCache::getFromMemory(BinaryData::resonance_icon_png, BinaryData::resonance_icon_pngSize));
    iconImageComponent.setImagePlacement(RectanglePlacement(juce::RectanglePlacement::stretchToFit));
    iconImageComponent.setAlpha(0.095);

    selectCB.setName("Resonance");
    selectCB.addSeparator();
    selectCB.setSelectedItemIndex(0);
    selectCB.addListener(this);
    selectCB.setTooltip("Select from available saved preparation settings");
    addAndMakeVisible(selectCB);

    defGainSlider = std::make_unique<BKSingleSlider>("volume (dBFS)", cResonanceDefGain, -100, 24, 0, 0.01, "-inf");
    defGainSlider->setJustifyRight(false);
    defGainSlider->setToolTipString("Adjusts overall volume of resonant notes");
    addAndMakeVisible(*defGainSlider);
    
    blendGainSlider = std::make_unique<BKSingleSlider>("blendronic gain (dBFS)", cBlendronicOutGain, -100, 24, 0, 0.01, "-inf");
    blendGainSlider->setJustifyRight(false);
    blendGainSlider->setToolTipString("Adjusts volume sent to blendronic, if attached");
    addAndMakeVisible(*blendGainSlider);

//    startTimeSlider = std::make_unique<BKSingleSlider>("start time (ms)", cResonanceStartTime, -4000, 4000, 2000, 1);
    
    startTimeSlider = std::make_unique<BKRangeSlider>("start time (ms)", 0, 4000, 0, 4000, 1);
    startTimeSlider->setJustifyRight(false);
    startTimeSlider->setToolTipString("sets range for start time into sample of resonant notes; velocity will set where in this range to start");
    addAndMakeVisible(*startTimeSlider);

    maxSympStringsSlider =std::make_unique<BKSingleSlider>("max sympathetic strings", "maxsymp", 0, 20, 8, 1, "-inf");
    maxSympStringsSlider->setJustifyRight(false);
    maxSympStringsSlider->setToolTipString("sets maximum number of sympathetic strings");
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
    //closestKeyboard->setOctaveForMiddleC(4);
    closestKeyboard->setScrollButtonsVisible(false);
    closestKeyboard->setOctaveForMiddleC(5);
    addAndMakeVisible(*closestKeyboard);
    
    
    fundamentalKeyboard = std::make_unique<BKKeymapKeyboardComponent> (fundamentalKeyboardState, BKKeymapKeyboardComponent::horizontalKeyboard);
    fundamentalKeyboard->setName("fundamental");
//    closestKeyboard.setAlpha(1);
    //fundamentalKeyboard->setAvailableRange(24, 24+NUM_KEYS);
    fundamentalKeyboard->setAvailableRange(0, NUM_KEYS);
    //fundamentalKeyboard->setOctaveForMiddleC(4);
    //fundamentalKeyboard.setScrollButtonsVisible(false);
    fundamentalKeyboard->setOctaveForMiddleC(5);
    fundamentalKeyboard->setKeysInKeymap(0);
    addAndMakeVisible(*fundamentalKeyboard);
    
    addKeyboard = std::make_unique<BKKeymapKeyboardComponent> (addKeyboardState, BKKeymapKeyboardComponent::horizontalKeyboard);
    addKeyboard->setName("add");
    addKeyboard->setScrollButtonsVisible(false);
    addKeyboard->setAvailableRange(21, 108);
    //addKeyboard->setOctaveForMiddleC(5);
    addKeyboard->setOctaveForMiddleC(4);
    addKeyboard->setKeysInKeymap(0);
    addAndMakeVisible(*addKeyboard);
    
    ringKeyboard = std::make_unique<BKKeymapKeyboardComponent> (ringKeyboardState, BKKeymapKeyboardComponent::horizontalKeyboard);
    ringKeyboard->setName("ring");
    ringKeyboard->setScrollButtonsVisible(false);
    ringKeyboard->setAvailableRange(21, 108);
    //ringKeyboard->setOctaveForMiddleC(5);
    ringKeyboard->setOctaveForMiddleC(4);
    ringKeyboard->setKeysInKeymap(0);
    addAndMakeVisible(*ringKeyboard);
    
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

    closestKeyLabel.setText("Resonant Keys: ", dontSendNotification);
    closestKeyLabel.setJustificationType(Justification::right);
    closestKeyLabel.setTooltip("keys that should resonate when struck; partials");
    addAndMakeVisible(closestKeyLabel);
    
    fundamentalLabel.setText("Held Key: ", dontSendNotification);
    fundamentalLabel.setJustificationType(Justification::right);
    fundamentalLabel.setTooltip("Key that defines resonance/partial structure; usually the lowest, but not necessarily!");
    addAndMakeVisible(fundamentalLabel);
    
    gainsLabel.setText("Gains: ", dontSendNotification);
    gainsLabel.setJustificationType(Justification::right);
    gainsLabel.setTooltip("gain multiplier for this resonance");
    addAndMakeVisible(gainsLabel);
    
    offsetsLabel.setText("Offsets: ", dontSendNotification);
    offsetsLabel.setJustificationType(Justification::right);
    offsetsLabel.setTooltip("offset in cents from ET for this resonance");
    addAndMakeVisible(offsetsLabel);
    
    addLabel.setText("Held Keys: ", dontSendNotification);
    addLabel.setJustificationType(Justification::centred);
    addLabel.setTooltip("Keys added to sympathetic resonance");
    addAndMakeVisible(addLabel);
    
    ringLabel.setText("Ringing Keys: ", dontSendNotification);
    ringLabel.setJustificationType(Justification::centred);
    ringLabel.setTooltip("Keys currently ringing");
    addAndMakeVisible(ringLabel);
    
    
    alternateMod.setButtonText ("alternate mod");
    alternateMod.setTooltip("activating this mod will alternate between modding and reseting attached preparations");
    alternateMod.setToggleState (false, dontSendNotification);
    addChildComponent(&alternateMod, ALL);
    alternateMod.setLookAndFeel(&buttonsAndMenusLAF2);

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

//#if JUCE_IOS
//void ResonanceViewController::iWantTheBigOne(TextEditor* tf, String name)
//{
//    hideOrShow.setAlwaysOnTop(false);
//    rightArrow.setAlwaysOnTop(false);
//    leftArrow.setAlwaysOnTop(false);
//    bigOne.display(tf, name, getBounds());
//}
//#endif

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
        defGainSlider->setBounds(firstRow.removeFromLeft(columnWidth - 5 * gYSpacing));
        defGainSlider->setVisible(true);
        
        //blendGainSlider->setBounds(firstRow);
         blendGainSlider->setBounds(firstRow.removeFromRight(columnWidth - 5 * gYSpacing));
        blendGainSlider->setVisible(true);
        
        Rectangle<int> secondRow = area.removeFromTop(columnHeight / 5);

        startTimeSlider->setBounds(secondRow.removeFromLeft(columnWidth - 5 * gYSpacing));
        startTimeSlider->setVisible(true);
        
        maxSympStringsSlider->setBounds(secondRow.removeFromRight(columnWidth - 5 * gYSpacing));
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
        
    } else if (tab == 2)
    {
        iconImageComponent.setBounds(area);
//        area.reduce(x0 + 10 * processor.paddingScalarX + 4, 10 * processor.paddingScalarY + 4);
        
#if JUCE_IOS
//        area.removeFromTop(gComponentComboBoxHeight);
        area.reduce(0.f, area.getHeight() * 0.2f);
#endif
        area.removeFromBottom(40 * processor.paddingScalarY);
        
        float keyboardHeight = 80 * processor.paddingScalarY;
        // float columnWidth = area.getWidth() * 0.15 + processor.paddingScalarX;
        
        Rectangle<int> addKeyboardRow = area.removeFromBottom(keyboardHeight * 2);
        addLabel.setBounds(addKeyboardRow.removeFromTop(keyboardHeight * 0.5));
        addLabel.setVisible(true);
        
        //float keyWidth = addKeyboardRow.getWidth() / 49.5; // 62 is number of white keys
        float keyWidth = addKeyboardRow.getWidth() / 52.0; // 62 is number of white keys
        
        DBG("Keyboard row width: " + String(addKeyboardRow.getWidth()));
        DBG("Keyboard width" + String(addKeyboard->getWidth()));
        
        
        addKeyboard->setKeyWidth(keyWidth);
        addKeyboard->setBlackNoteLengthProportion(0.6);
        addKeyboard->setBounds(addKeyboardRow);
        addKeyboard->setVisible(true);
        
        area.removeFromBottom(processor.paddingScalarX * 10);
        area.removeFromBottom(0.2 * keyboardHeight + gYSpacing);
        
       
        
        Rectangle<int> ringKeyboardRow = area.removeFromBottom(keyboardHeight * 2);
        ringLabel.setBounds(ringKeyboardRow.removeFromTop(keyboardHeight * 0.5));
        ringLabel.setVisible(true);
        
        keyWidth = ringKeyboardRow.getWidth() / 52.0; // 62 is number of white keys
        
        DBG("Keyboard row width: " + String(ringKeyboardRow.getWidth()));
        DBG("Keyboard width" + String(ringKeyboard->getWidth()));
        
        
        ringKeyboard->setKeyWidth(keyWidth);
        ringKeyboard->setBlackNoteLengthProportion(0.6);
        ringKeyboard->setBounds(ringKeyboardRow);
        ringKeyboard->setVisible(true);
        
        area.removeFromBottom(processor.paddingScalarX * 10);
        area.removeFromBottom(0.2 * keyboardHeight + gYSpacing);

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
    
    alternateMod.setBounds(actionButton.getRight()+gXSpacing,
                           actionButton.getY(),
                           selectCB.getWidth(),
                           actionButton.getHeight());
    
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
    addKeyboard->setVisible(false);
    ringKeyboard->setVisible(false);
    
    gainsKeyboard.setVisible(false);
    offsetsKeyboard.setVisible(false);
    
//    for (int i = 0; i < NUM_KEYS; i++) {
//        offsetsArray[i]->setVisible(false);
//        gainsArray[i]->setVisible(false);
//    }

    closestKeyLabel.setVisible(false);
    fundamentalLabel.setVisible(false);
    addLabel.setVisible(false);
    ringLabel.setVisible(false);
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
    addKeyboardState.addListener(this);
    offsetsKeyboard.addMyListener(this);
    gainsKeyboard.addMyListener(this);
    
    startTimer(100);
}

void ResonancePreparationEditor::update()
{
    if (processor.updateState->currentResonanceId < 0) return;
    ADSRSlider->setIsButtonOnly(true);

    GenericPreparation::Ptr _prep = processor.gallery->getPreparationOfType(BKPreparationType::PreparationTypeResonance, processor.updateState->currentResonanceId);
    ResonancePreparation* prep = dynamic_cast<ResonancePreparation*>(_prep.get());
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
        
        fundamentalKeyboard->setKeysInKeymap({prep->getFundamentalKey()});
        closestKeyboard->setKeysInKeymap(prep->getResonanceKeys());
        //addKeyboard->setKeysInKeymap(prep->getSympStrings());
        
        
        offsetsKeyboard.setKeys(prep->getResonanceKeys());
        offsetsKeyboard.setValues(prep->getOffsets());
        
        gainsKeyboard.setKeys(prep->getResonanceKeys());
        gainsKeyboard.setValues(prep->getGains());
       
        
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
        //Array<float> offsets;
        //Array<float> gains;
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
        //offsetsKeyboard.setValues(offsets);
        //gainsKeyboard.setValues(gains);

        /*
        closestKeyboard->setKeysInKeymap(prep->getKeys());
        fundamentalKeyboard->setKeysInKeymap({prep->getFundamental()});
        */
    }
}

void ResonancePreparationEditor::fillSelectCB(int last, int current)
{   selectCB.clear(dontSendNotification);
    
    for (auto prep : *processor.gallery->getAllPreparationsOfType(BKPreparationType::PreparationTypeResonance))
    {
        
        int Id = prep->getId();
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
    if (processor.updateState->currentDisplay == DisplayResonance)
    {
        ResonanceProcessor* proc = dynamic_cast<ResonanceProcessor*>(processor.currentPiano->getProcessorOfType(processor.updateState->currentResonanceId, PreparationTypeResonance).get());
        GenericPreparation::Ptr _prep = processor.gallery->getPreparationOfType(BKPreparationType::PreparationTypeResonance, processor.updateState->currentResonanceId);
        ResonancePreparation* prep = dynamic_cast<ResonancePreparation*>(_prep.get());
        if (prep != nullptr && proc != nullptr)
        {
            if (prep->defaultGain.didChange())
                defGainSlider->setValue(prep->defaultGain.value, dontSendNotification);
            if (prep->rBlendronicGain.didChange())
                blendGainSlider->setValue(prep->rBlendronicGain.value, dontSendNotification);
            
            if (prep->rAttack.didChange())
                ADSRSlider->setAttackValue(prep->rAttack.value, dontSendNotification);
            if (prep->rDecay.didChange())
                ADSRSlider->setDecayValue(prep->rDecay.value, dontSendNotification);
            if (prep->rSustain.didChange())
                ADSRSlider->setSustainValue(prep->rSustain.value, dontSendNotification);
            if (prep->rRelease.didChange())
                ADSRSlider->setReleaseValue(prep->rRelease.value, dontSendNotification);
            
            if (prep->rMaxSympStrings.didChange())
                maxSympStringsSlider->setValue(prep->rMaxSympStrings.value, dontSendNotification);
            if (prep->rMinStartTimeMS.didChange())
                startTimeSlider->setMinValue(prep->rMinStartTimeMS.value, dontSendNotification);
            if (prep->rMaxStartTimeMS.didChange())
                startTimeSlider->setMaxValue(prep->rMaxStartTimeMS.value, dontSendNotification);
            
            if (prep->rFundamentalKey.didChange()) fundamentalKeyboard->setKeysInKeymap(prep->rFundamentalKey.value);
            if (prep->rResonanceKeys.didChange())
            {
                closestKeyboard->setKeysInKeymap(prep->rResonanceKeys.value);
                offsetsKeyboard.setKeys(prep->getResonanceKeys());
                gainsKeyboard.setKeys(prep->getResonanceKeys());
            }
            if (prep->rOffsetsKeys.didChange())
                offsetsKeyboard.setValues(prep->getOffsets());
            if (prep->rGainsKeys.didChange())
                gainsKeyboard.setValues(prep->getGains());
            if (currentTab == 2)
            {
                addKeyboard->setKeysInKeymap(prep->getHeldKeys());
                ringKeyboard->setKeysInKeymap(prep->getRingingStrings());
            }
        }
    }
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
        processor.reset(PreparationTypeResonance, processor.updateState->currentResonanceId);
        vc->update();
    }
    else if (action == 5)
    {
        processor.clear(PreparationTypeResonance, processor.updateState->currentResonanceId);
        vc->update();
        processor.saveGalleryToHistory("Clear Resonance Preparation");
    }
    else if (action == 6)
    {
        AlertWindow prompt("", "", AlertWindow::AlertIconType::QuestionIcon);
        
        int Id = processor.updateState->currentResonanceId;
        GenericPreparation::Ptr prep = processor.gallery->getPreparationOfType(PreparationTypeResonance, Id);
        
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
        
        int Id = processor.updateState->currentResonanceId;
        GenericPreparation::Ptr prep = processor.gallery->getPreparationOfType(PreparationTypeResonance, Id);
        
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
        processor.importPreparation(PreparationTypeResonance, processor.updateState->currentResonanceId, which);
        vc->update();
        processor.saveGalleryToHistory("Import Resonance Preparation");
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
    return processor.gallery->add(PreparationTypeResonance);
}

int ResonancePreparationEditor::duplicatePreparation(void)
{
    return  processor.gallery->duplicate(PreparationTypeResonance, processor.updateState->currentResonanceId);;
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
    String name = box->getName();
    int Id = box->getSelectedId();
    // int index = box->getSelectedItemIndex();
    
    if (name == "Resonance")
    {
        setCurrentId(Id);
    }
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
        bool single = processor.gallery->getAllPreparationsOfType(PreparationTypeResonance)->size() == 2;
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
    GenericPreparation::Ptr _prep = processor.gallery->getPreparationOfType(BKPreparationType::PreparationTypeResonance, processor.updateState->currentResonanceId);
    ResonancePreparation* prep = dynamic_cast<ResonancePreparation*>(_prep.get());

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

    }
    processor.updateState->editsMade = true;
}

void ResonancePreparationEditor::BKStackedSliderValueChanged(String name, Array<float> val)
{
    DBG("received overtone slider " + name);

    GenericPreparation::Ptr _prep = processor.gallery->getPreparationOfType(BKPreparationType::PreparationTypeResonance, processor.updateState->currentResonanceId);
    ResonancePreparation* prep = dynamic_cast<ResonancePreparation*>(_prep.get());

    //prep->setDistances(val);

    processor.updateState->editsMade = true;
}

void ResonancePreparationEditor::BKADSRSliderValueChanged(String name, int attack, int decay, float sustain, int release)
{
    DBG("received ADSR slider " + name);

    GenericPreparation::Ptr _prep = processor.gallery->getPreparationOfType(BKPreparationType::PreparationTypeResonance, processor.updateState->currentResonanceId);
    ResonancePreparation* prep = dynamic_cast<ResonancePreparation*>(_prep.get());

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
    GenericPreparation::Ptr _prep = processor.gallery->getPreparationOfType(BKPreparationType::PreparationTypeResonance, processor.updateState->currentResonanceId);
    ResonancePreparation* prep = dynamic_cast<ResonancePreparation*>(_prep.get());
    if(name == startTimeSlider->getName()) {
        //DBG("got new AdaptiveTempo 1 time diff min/max " + String(minval) + " " + String(maxval));
        prep->setMinStartTime(minval);
        prep->setMaxStartTime(maxval);
    }
    
    processor.updateState->editsMade = true;
}

void ResonancePreparationEditor::handleKeymapNoteToggled(BKKeymapKeyboardState* source, int midiNoteNumber) {
    
    DBG("ResonancePreparationEditor::handleKeymapNoteToggled " + String(midiNoteNumber));
    
    GenericPreparation::Ptr _prep = processor.gallery->getPreparationOfType(BKPreparationType::PreparationTypeResonance, processor.updateState->currentResonanceId);
    ResonancePreparation* prep = dynamic_cast<ResonancePreparation*>(_prep.get());
    
    if (source == &resonanceKeyboardState)
    {
        //closestKeyboard->setKeysInKeymap(prep->getKeys());
        //DBG("resonanceKeyboardState " + String(midiNoteNumber));
        prep->toggleResonanceKey(midiNoteNumber); // for some reason we're off an octave here
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
        //DBG("fundamental key toggled " + String(midiNoteNumber));
    }
    
    else if (source == &addKeyboardState )
    {
        if (prep->rActiveHeldKeys.arrayContains(midiNoteNumber))
        {
            // clear this held string's partial
            prep->rActiveHeldKeys.arrayRemoveAllInstancesOf(midiNoteNumber);
            prep->removeSympStrings(midiNoteNumber, 0);
            prep->sympStrings.remove(midiNoteNumber);
        } else
        {
            prep->addSympStrings(midiNoteNumber, 127);
            prep->addHeldKey(midiNoteNumber);
        }
           
        addKeyboard->setKeysInKeymap(prep->rActiveHeldKeys.value);
    }
    update();

}

void ResonancePreparationEditor::keyboardSliderChanged(String name, Array<float> values)
{
    GenericPreparation::Ptr _prep = processor.gallery->getPreparationOfType(BKPreparationType::PreparationTypeResonance, processor.updateState->currentResonanceId);
    ResonancePreparation* prep = dynamic_cast<ResonancePreparation*>(_prep.get());
 
    if(name == gainsKeyboard.getName())
    {
        DBG("updating gain vals");
        prep->setGains(values);
    }
    else if(name == offsetsKeyboard.getName())
    {
        DBG("updating offset vals");
        prep->setOffsets(values);
    }
    processor.gallery->setGalleryDirty(true);
    
    processor.updateState->editsMade = true;
}

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ResonanceModificationEditor ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~//
ResonanceModificationEditor::ResonanceModificationEditor(BKAudioProcessor& p, BKItemGraph* theGraph):
ResonanceViewController(p, theGraph)
{
    selectCB.addListener(this);
    selectCB.addMyListener(this);
    
    fillSelectCB(-1,-1);
    
    defGainSlider->addMyListener(this);
    startTimeSlider->addMyListener(this);
    blendGainSlider->addMyListener(this);
    maxSympStringsSlider->addMyListener(this);

    ADSRSlider->addMyListener(this);
    
    //closestKeyboard->addMyListener(this);
    //fundamentalKeyboard->addMyListener(this);
    resonanceKeyboardState.addListener(this);
    fundamentalKeyboardState.addListener(this);
    
    addKeyboardState.addListener(this);
    
    offsetsKeyboard.addMyListener(this);
    gainsKeyboard.addMyListener(this);
    
    alternateMod.addListener(this);
    alternateMod.setVisible(true);
    
    defGainSlider->addModdableComponentListener(this);
    blendGainSlider->addModdableComponentListener(this);
    maxSympStringsSlider->addModdableComponentListener(this);
    
    /*
     gainSlider->addModdableComponentListener(this);
     resonanceGainSlider->addModdableComponentListener(this);
     hammerGainSlider->addModdableComponentListener(this);
     blendronicGainSlider->addModdableComponentListener(this);
     */
}

void ResonanceModificationEditor::greyOutAllComponents()
{
    defGainSlider->setDim(gModAlpha);
    startTimeSlider->setDim(gModAlpha);
    blendGainSlider->setDim(gModAlpha);
    maxSympStringsSlider->setDim(gModAlpha);
    ADSRSlider->setDim(gModAlpha);
    closestKeyboard->setAlpha(gModAlpha);
    fundamentalKeyboard->setAlpha(gModAlpha);
    offsetsKeyboard.setDim(gModAlpha);
    gainsKeyboard.setDim(gModAlpha);
    addKeyboard->setAlpha(gModAlpha);
    ringKeyboard->setAlpha(gModAlpha);
}

void ResonanceModificationEditor::highlightModedComponents()
{
    ResonanceModification::Ptr mod = processor.gallery->getResonanceModification(processor.updateState->currentModResonanceId);

    if(mod->getDirty(ResonanceGain))            defGainSlider->setBright();
    if(mod->getDirty(ResonanceBlendronicGain))  blendGainSlider->setBright();
    if(mod->getDirty(ResonanceMinStartTime))    startTimeSlider->setBright();
    if(mod->getDirty(ResonanceMaxStartTime))    startTimeSlider->setBright();
    if(mod->getDirty(ResonanceFundamental))     fundamentalKeyboard->setAlpha(1.);
    if(mod->getDirty(ResonanceMaxSympStrings))  maxSympStringsSlider->setBright();
    if(mod->getDirty(ResonanceClosestKeys))     closestKeyboard->setAlpha(1.);
    if(mod->getDirty(ResonanceOffsets))         offsetsKeyboard.setBright();
    if(mod->getDirty(ResonanceGains))           gainsKeyboard.setBright();
    if(mod->getDirty(ResonanceADSR))            ADSRSlider->setBright();
    if(mod->getDirty(ResonanceHeld))            addKeyboard->setAlpha(1.);
}

void ResonanceModificationEditor::update(void)
{
    if (processor.updateState->currentModResonanceId < 0) return;
    
    selectCB.setSelectedId(processor.updateState->currentModResonanceId, dontSendNotification);
    
    ResonanceModification::Ptr _mod = processor.gallery->getResonanceModification(processor.updateState->currentModResonanceId);
    ResonancePreparation* mod = dynamic_cast<ResonancePreparation*>(_mod->getPrep().get());
    if (mod != nullptr)
    {
        greyOutAllComponents();
        highlightModedComponents();
        
        selectCB.setSelectedId(processor.updateState->currentModResonanceId, dontSendNotification);
        
        defGainSlider->setValue(mod->defaultGain.value, dontSendNotification);
        startTimeSlider->setMinValue(mod->rMinStartTimeMS.value, dontSendNotification);
        startTimeSlider->setMaxValue(mod->rMaxStartTimeMS.value, dontSendNotification);
        blendGainSlider->setValue(mod->rBlendronicGain.value, dontSendNotification);
        maxSympStringsSlider->setValue(mod->rMaxSympStrings.value, dontSendNotification);
        ADSRSlider->setValue(mod->getADSRvals(), dontSendNotification);
        fundamentalKeyboard->setKeysInKeymap({mod->getFundamentalKey()});
        closestKeyboard->setKeysInKeymap(mod->getResonanceKeys());
        offsetsKeyboard.setKeys(mod->getResonanceKeys());
        offsetsKeyboard.setValues(mod->getOffsets());
        gainsKeyboard.setKeys(mod->getResonanceKeys());
        gainsKeyboard.setValues(mod->getGains());
        addKeyboard->setKeysInKeymap(mod->getHeldKeys());
        alternateMod.setToggleState(_mod->altMod, dontSendNotification);
        
        //updateComponentVisibility();
    }
     
}

void ResonanceModificationEditor::fillSelectCB(int last, int current)
{
    selectCB.clear(dontSendNotification);
    
    
    for (auto prep : processor.gallery->getResonanceModifications())
    {
        int Id = prep->getId();;
        String name = prep->_getName();
        
        if (name != String())  selectCB.addItem(name, Id);
        else selectCB.addItem("ResonanceMod"+String(Id), Id);
        
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

void ResonanceModificationEditor::bkMessageReceived (const String& message)
{
    if (message == "resonance/update")
    {
        update();
    }
}

int ResonanceModificationEditor::addPreparation(void)
{
    processor.gallery->add(PreparationTypeResonanceMod);
    
    return processor.gallery->getResonanceModifications().getLast()->getId();
}

int ResonanceModificationEditor::duplicatePreparation(void)
{
    processor.gallery->duplicate(PreparationTypeResonanceMod, processor.updateState->currentModResonanceId);
    
    return processor.gallery->getResonanceModifications().getLast()->getId();
}

void ResonanceModificationEditor::deleteCurrent(void)
{
    int directId = selectCB.getSelectedId();
    int index = selectCB.getSelectedItemIndex();
    
    if ((index == 0) && (selectCB.getItemId(index+1) == -1)) return;
    
    processor.gallery->remove(PreparationTypeResonance, directId);
    
    fillSelectCB(0, 0);
    
    selectCB.setSelectedItemIndex(0, dontSendNotification);
    int newId = selectCB.getSelectedId();
    
    setCurrentId(newId);
}

void ResonanceModificationEditor::setCurrentId(int Id)
{
    processor.updateState->currentResonanceId = Id;
    
    processor.updateState->idDidChange = true;
    
    update();
    
    fillSelectCB(lastId, Id);
    
    lastId = Id;
}

void ResonanceModificationEditor::actionButtonCallback(int action, ResonanceModificationEditor* vc)
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
        processor.saveGalleryToHistory("New Resonance Modification");
    }
    else if (action == 2)
    {
        int Id = vc->duplicatePreparation();
        vc->setCurrentId(Id);
        processor.saveGalleryToHistory("Duplicate Resonance Modification");
    }
    else if (action == 3)
    {
        vc->deleteCurrent();
        processor.saveGalleryToHistory("Delete Resonance Modification");
    }
    else if (action == 5)
    {
        processor.clear(PreparationTypeResonanceMod, processor.updateState->currentModResonanceId);
        vc->update();
        vc->updateModification();
        processor.saveGalleryToHistory("Clear Resonance Modification");
    }
    else if (action == 6)
    {
        AlertWindow prompt("", "", AlertWindow::AlertIconType::QuestionIcon);
        
        int Id = processor.updateState->currentModResonanceId;
        ResonanceModification::Ptr prep = processor.gallery->getResonanceModification(Id);
        
        prompt.addTextEditor("name", prep->_getName());
        
        prompt.addButton("Ok", 1, KeyPress(KeyPress::returnKey));
        prompt.addButton("Cancel", 2, KeyPress(KeyPress::escapeKey));
        
        int result = prompt.runModalLoop();
        
        String name = prompt.getTextEditorContents("name");
        
        if (result == 1)
        {
            prep->_setName(name);
            vc->fillSelectCB(Id, Id);
            processor.saveGalleryToHistory("Rename Resonance Modification");
        }
        
        vc->update();
    }
    else if (action == 7)
    {
        AlertWindow prompt("", "", AlertWindow::AlertIconType::QuestionIcon);
        
        int Id = processor.updateState->currentModResonanceId;
        ResonanceModification::Ptr prep = processor.gallery->getResonanceModification(Id);
        
        prompt.addTextEditor("name", prep->_getName());
        
        prompt.addButton("Ok", 1, KeyPress(KeyPress::returnKey));
        prompt.addButton("Cancel", 2, KeyPress(KeyPress::escapeKey));
        
        int result = prompt.runModalLoop();
        
        String name = prompt.getTextEditorContents("name");
        
        DBG("name: " + String(name));
        
        if (result == 1)
        {
            processor.exportPreparation(PreparationTypeResonanceMod, Id, name);
        }
    }
    else if (action >= 100)
    {
        int which = action - 100;
        processor.importPreparation(PreparationTypeResonanceMod, processor.updateState->currentModResonanceId, which);
        vc->update();
        processor.saveGalleryToHistory("Import Resonance Modification");
    }
    
}

void ResonanceModificationEditor::bkComboBoxDidChange (ComboBox* box)
{
    String name = box->getName();
    int Id = box->getSelectedId();
    
    if (name == "Resonance")
    {
        setCurrentId(Id);
    }
    
    processor.updateState->editsMade = true;
}

void ResonanceModificationEditor::BKEditableComboBoxChanged(String name, BKEditableComboBox* cb)
{
    ResonanceModification::Ptr mod = processor.gallery->getResonanceModification(processor.updateState->currentModResonanceId);
    
    mod->_setName(name);
    
    updateModification();
    
    processor.updateState->editsMade = true;
}


void ResonanceModificationEditor::BKSingleSliderValueChanged(BKSingleSlider* slider, String name, double val)
{
    
    ResonanceModification::Ptr _mod = processor.gallery->getResonanceModification(processor.updateState->currentModResonanceId);
    ResonanceModification* mod = dynamic_cast<ResonanceModification*>(_mod.get());
    if (name == defGainSlider->getName())
    {
        DBG("modding resonant note volume to " + String(val));
        mod->getPrepPtr()->setDefGain(val);
        
        mod->setDirty(ResonanceGain);
        defGainSlider->setBright();
    }
    else if (name == blendGainSlider->getName())
    {
        DBG("modding blendronic gain to " + String(val));
        mod->getPrepPtr()->setBlendGain(val);
        
        mod->setDirty(ResonanceBlendronicGain);
        blendGainSlider->setBright();
    }
    else if (name == maxSympStringsSlider->getName())
    {
        DBG("modding max symp strings to " + String(val));
        mod->getPrepPtr()->setMaxSympStrings(val);
        
        mod->setDirty(ResonanceMaxSympStrings);
        maxSympStringsSlider->setBright();
    }
    processor.updateState->editsMade = true;
    
    updateModification();
}

void ResonanceModificationEditor::BKRangeSliderValueChanged(String name, double minval, double maxval)
{
    ResonanceModification::Ptr mod = processor.gallery->getResonanceModification(processor.updateState->currentModResonanceId);
    
    if(name == startTimeSlider->getName()) {
        //DBG("modding new AdaptiveTempo 1 time diff min/max " + String(minval) + " " + String(maxval));
        mod->getPrepPtr()->setMinStartTime(minval);
        mod->getPrepPtr()->setMaxStartTime(maxval);
        
        mod->setDirty(ResonanceMinStartTime);
        mod->setDirty(ResonanceMaxStartTime);
        startTimeSlider->setBright();
    }
    
    updateModification();
}

void ResonanceModificationEditor::handleKeymapNoteToggled(BKKeymapKeyboardState* source, int midiNoteNumber) {
    
    ResonanceModification::Ptr mod = processor.gallery->getResonanceModification(processor.updateState->currentModResonanceId);
    DBG("ResonanceModificationEditor::handleKeymapNoteToggled");
    
    if (source == &resonanceKeyboardState)
    {
        //closestKeyboard->setKeysInKeymap(prep->getKeys());
        //DBG("resonanceKeyboardState " + String(midiNoteNumber));
        mod->getPrepPtr()->toggleResonanceKey(midiNoteNumber);
        closestKeyboard->setKeysInKeymap(mod->getPrepPtr()->getResonanceKeys());
        mod->setDirty(ResonanceClosestKeys);
        closestKeyboard->setAlpha(1.);
        
        offsetsKeyboard.setKeys(mod->getPrepPtr()->getResonanceKeys());
        offsetsKeyboard.setValues(mod->getPrepPtr()->getOffsets());
        mod->setDirty(ResonanceOffsets);
        offsetsKeyboard.setBright();
        
        gainsKeyboard.setKeys(mod->getPrepPtr()->getResonanceKeys());
        gainsKeyboard.setValues(mod->getPrepPtr()->getGains());
        mod->setDirty(ResonanceGains);
        gainsKeyboard.setBright();
         

    }
    else if (source == &fundamentalKeyboardState)
    {
        mod->getPrepPtr()->setFundamentalKey(midiNoteNumber);
        fundamentalKeyboard->setKeysInKeymap({midiNoteNumber});
        mod->setDirty(ResonanceFundamental);
        fundamentalKeyboard->setAlpha(1.);
        
        //DBG("fundamental key toggled " + String(midiNoteNumber));
    }
    else if (source == &addKeyboardState )
    {
        addKeyboard->setAlpha(1.);
        mod->setDirty(ResonanceHeld);
        //ResonanceProcessor::Ptr proc = processor.currentPiano->getResonanceProcessor(processor.updateState->currentResonanceId);
        if (mod->getPrepPtr()->rActiveHeldKeys.arrayContains(midiNoteNumber))
        {
            // clear this held string's partial
            mod->getPrepPtr()->rActiveHeldKeys.arrayRemoveAllInstancesOf(midiNoteNumber);
            mod->getPrepPtr()->removeSympStrings(midiNoteNumber, 0);
        } else
        {
            mod->getPrepPtr()->addSympStrings(midiNoteNumber, 127);
            mod->getPrepPtr()->addHeldKey(midiNoteNumber);
            mod->getPrepPtr()->sympStrings.remove(midiNoteNumber);
        }
           
        addKeyboard->setKeysInKeymap(mod->getPrepPtr()->rActiveHeldKeys.value);
    }
    update();

}

void ResonanceModificationEditor::keyboardSliderChanged(String name, Array<float> values)
{
    ResonanceModification::Ptr mod = processor.gallery->getResonanceModification(processor.updateState->currentModResonanceId);

    if(name == gainsKeyboard.getName())
    {
        DBG("modding gain vals");
        mod->getPrepPtr()->setGains(values);
        
        mod->setDirty(ResonanceGains);
        gainsKeyboard.setBright();
    }
    else if(name == offsetsKeyboard.getName())
    {
        DBG("modding offset vals");
        mod->getPrepPtr()->setOffsets(values);
        
        mod->setDirty(ResonanceOffsets);
        offsetsKeyboard.setBright();
    }
    
    updateModification();

}
    
    // also need for other two keyboard sliders

void ResonanceModificationEditor::BKStackedSliderValueChanged(String name, Array<float> val)
{

}

void ResonanceModificationEditor::BKADSRSliderValueChanged(String name, int attack, int decay, float sustain, int release)
{
    
    ResonanceModification::Ptr mod = processor.gallery->getResonanceModification(processor.updateState->currentModResonanceId);
    
    Array<float> newvals = {(float)attack, (float)decay, sustain, (float)release};
    
    mod->getPrepPtr()->setADSRvals(newvals);
    mod->setDirty(ResonanceADSR);
    
    ADSRSlider->setBright();
    
    updateModification();
}

void ResonanceModificationEditor::BKADSRButtonStateChanged(String name, bool mod, bool state)
{
    setSubWindowInFront(!state);
}

void ResonanceModificationEditor::updateModification(void)
{
    processor.updateState->modificationDidChange = true;
    
    processor.updateState->editsMade = true;
}

void ResonanceModificationEditor::buttonClicked (Button* b)
{
    if (b == &hideOrShow)
    {
        processor.updateState->setCurrentDisplay(DisplayNil);
        
    }
    else if (b == &actionButton)
    {
        bool single = processor.gallery->getResonanceModifications().size() == 2;
        getModOptionMenu(PreparationTypeResonanceMod, single).showMenuAsync (PopupMenu::Options().withTargetComponent (&actionButton), ModalCallbackFunction::forComponent (actionButtonCallback, this) );
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
    else if (b == &alternateMod)
    {
        ResonanceModification::Ptr mod = processor.gallery->getResonanceModification(processor.updateState->currentModResonanceId);
        mod->altMod = alternateMod.getToggleState();
    }
}

void ResonanceModificationEditor::timerCallback()
{
    
}
