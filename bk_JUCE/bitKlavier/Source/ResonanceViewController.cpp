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
    BKViewController(p, theGraph, 2)
#if JUCE_IOS
//absoluteKeyboard(false, true)
#else
//absoluteKeyboard(false, false)
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

    maxSympStringsSlider =std::make_unique<BKSingleSlider>("max sympathetic strings", "maxsymp", 0, 20, 20, 0.01, "-inf");
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
    
    NUM_KEYS = 49;
    absoluteKeyboard = std::make_unique<BKKeymapKeyboardComponent> (keyboardState, BKKeymapKeyboardComponent::horizontalKeyboard);
    absoluteKeyboard->setName("absolute");
//    absoluteKeyboard.setAlpha(1);
    absoluteKeyboard->setAvailableRange(24, 24+NUM_KEYS);
    absoluteKeyboard->setOctaveForMiddleC(4);
    addAndMakeVisible(*absoluteKeyboard);
    
    fundamentalKeyboard = std::make_unique<BKKeymapKeyboardComponent> (fundamentalKeyboardState, BKKeymapKeyboardComponent::horizontalKeyboard);
    fundamentalKeyboard->setName("fundamental");
//    absoluteKeyboard.setAlpha(1);
    fundamentalKeyboard->setAvailableRange(24, 24+NUM_KEYS);
    fundamentalKeyboard->setOctaveForMiddleC(4);
    addAndMakeVisible(*fundamentalKeyboard);
    
//    ringingKeyboard = std::make_unique<BKKeymapKeyboardComponent> (keyboardState, BKKeymapKeyboardComponent::horizontalKeyboard);
//    ringingKeyboard->setName("ringing");
////    absoluteKeyboard.setAlpha(1);
////    absoluteKeyboard->setAvailableRange(24, 24+NUM_KEYS);
//    ringingKeyboard->setOctaveForMiddleC(4);
//    addAndMakeVisible(*ringingKeyboard);
    
    float sliderMin  = -1.;
   float sliderMax  = 1.;
   float  sliderIncrement = 0.01;
   float sliderDefault = 0.;
   int numDefaultSliders = 12;
    bool allowSubSliders = false;
    String subSliderName = "add subslider";
    int sliderWidth = 20;
   int  sliderHeight = 60;
   int displaySliderWidth = 80;
   float clickedHeight = 0.;
    Slider::SliderStyle subsliderStyle = Slider::LinearBarVertical;
    
    // the bigInvisibleSlider sits on top of the individual sliders
    // it's used to set the values of the slider that the mouse is nearest
//    bigInvisibleSlider = std::make_unique<BKSubSlider>(Slider::LinearBarVertical,
//                                         sliderMin,
//                                         sliderMax,
//                                         sliderDefault,
//                                         sliderIncrement,
//                                         20,
//                                         sliderHeight);
    
    for (int i = 0; i < NUM_KEYS; i++)
    {
//        ToggleButton* t = new ToggleButton();
//        t->setToggleState(false, dontSendNotification);
//        t->setLookAndFeel(&buttonsAndMenusLAF);
//        t->setTooltip("Toggle whether this key is your fundamental");
//        t->addListener(this);
//        t->setRadioGroupId(1);
//        t->setClickingTogglesState(true);
//
//        addChildComponent(t);
//        addAndMakeVisible(t);
//        fundamentalButtons.add(t);
        
        BKSubSlider* refSlider = new BKSubSlider(Slider::LinearBarVertical,
                                                                 sliderMin,
                                                                 sliderMax,
                                                                 sliderDefault,
                                                                 sliderIncrement,
                                                                 20,
                                                                sliderHeight);
//        refSlider->setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxLeft, true, 0,0);
//        refSlider->addMouseListener(this, true);
        refSlider->setName("offset" + String(i));
//        refSlider->setInterceptsMouseClicks(false, false);
//        refSlider->setLookAndFeel(&displaySliderLookAndFeel);
        addAndMakeVisible(*refSlider);
        
        offsetsArray.add(refSlider);
        
        BKSubSlider* refgainSlider = new BKSubSlider(Slider::LinearBarVertical,
                                                                 sliderMin,
                                                                 sliderMax,
                                                                 sliderDefault,
                                                                 sliderIncrement,
                                                                 20,
                                                                sliderHeight);
        refgainSlider->setName("gain" + String(i));
        addAndMakeVisible(*refgainSlider);
        gainsArray.add(refgainSlider);
        
        isActive.add(false);
//        gainsArray.add(std::make_unique<BKSingleSlider>("gain" + String(i), 10, -100, 24, 1, 0.01, "-inf"));
        
    }
    
    
//    Array<float> newActiveVals;
//    Array<bool> newactives;
//
//    for (int i = 0; i < NUM_KEYS; i++){
//        newActiveVals.add(i);
//        newactives.add(true);
//    }
//
//    offsets.setName("offsets");
//    offsets.setToOnlyActive(newActiveVals,newactives, dontSendNotification);
//    addAndMakeVisible(offsets);
//    offsets.hideRotateButton();
//    offsets.setSkewFromMidpoint(false);
//
//    gains.setName("gains");
//    gains.setToOnlyActive(newActiveVals,newactives, dontSendNotification);
//    addAndMakeVisible(gains);
//    gains.hideRotateButton();


//    lastNote.setText("note: ", dontSendNotification);
//    lastNote.setTooltip("last note played as MIDI value");
//    addAndMakeVisible(lastNote);
    
    closestKeyLabel.setText("Closest Key: ", dontSendNotification);
    addAndMakeVisible(closestKeyLabel);
    
    fundamentalLabel.setText("Fundamental: ", dontSendNotification);
    addAndMakeVisible(fundamentalLabel);
    
    gainsLabel.setText("Gains: ", dontSendNotification);
    addAndMakeVisible(gainsLabel);
    
    offsetsLabel.setText("Offsets: ", dontSendNotification);
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

    int x0 = leftArrow.getRight() + gXSpacing;
//    int y0 = hideOrShow.getBottom() + gYSpacing;
//    int right = rightArrow.getX() - gXSpacing;
//    int width = right - x0;
//    int height = getHeight() - y0;

    Rectangle<int> area(getLocalBounds());
    area.removeFromLeft(leftArrow.getWidth());
    area.removeFromRight(rightArrow.getWidth());
    area.removeFromTop(gComponentComboBoxHeight);

    if (tab == 0){
        iconImageComponent.setBounds(area);
//        area.reduce(10 * processor.paddingScalarX + 4, 10 * processor.paddingScalarY + 4);
        area.reduce(x0 + 10 * processor.paddingScalarX + 4, 10 * processor.paddingScalarY + 4);
        
//        overtonesSlider->setVisible(true);
        
//        lastNote.setVisible(true);

#if JUCE_IOS
//        area.removeFromTop(gComponentComboBoxHeight);
        area.reduce(0.f, area.getHeight() * 0.2f);
#endif
        float keyboardHeight = 80 * processor.paddingScalarY;
        
        Rectangle<int> fundamentalKeyboardRow = area.removeFromBottom(keyboardHeight);
        
        fundamentalLabel.setBounds(fundamentalKeyboardRow.removeFromLeft(fundamentalKeyboardRow.getWidth()*0.1));
        fundamentalLabel.setVisible(true);
        
        float keyWidth = fundamentalKeyboardRow.getWidth() / round((NUM_KEYS) * 7. / 12 + 1); //num white keys, gives error
        
        fundamentalKeyboard->setKeyWidth(18.5);
//        absoluteKeyboard->setKeyWidth(absoluteKeyboardRow.getWidth()/NUM_KEYS); //gives error
        fundamentalKeyboard->setBlackNoteLengthProportion(0.6);
        fundamentalKeyboard->setBounds(fundamentalKeyboardRow);
        fundamentalKeyboard->setVisible(true);
        
        area.removeFromBottom(processor.paddingScalarX * 10);
        
        Rectangle<int> absoluteKeyboardRow = area.removeFromBottom(keyboardHeight);
        
        closestKeyLabel.setBounds(absoluteKeyboardRow.removeFromLeft(absoluteKeyboardRow.getWidth()*0.1));
        closestKeyLabel.setVisible(true);
        
        DBG("calculated key width: "+ String(keyWidth));
        DBG("width of row: " + String(absoluteKeyboardRow.getWidth()));
        DBG("key width: " + String(absoluteKeyboard->getKeyWidth()));
        absoluteKeyboard->setKeyWidth(18.5);
//        absoluteKeyboard->setKeyWidth(absoluteKeyboardRow.getWidth()/NUM_KEYS); //gives error
        absoluteKeyboard->setBlackNoteLengthProportion(0.6);
        absoluteKeyboard->setBounds(absoluteKeyboardRow);
        absoluteKeyboard->setVisible(true);
    
        area.removeFromBottom(processor.paddingScalarY * 10);

        float multiHeight = 100 + processor.paddingScalarY;
        
        Rectangle<int> offsetsRow = area.removeFromBottom(multiHeight);
        offsetsLabel.setBounds(offsetsRow.removeFromLeft(offsetsRow.getWidth()*0.1));
        offsetsLabel.setVisible(true);
        
        float sliderWidth = offsetsRow.getWidth()/NUM_KEYS;
        for (int i = 0; i < NUM_KEYS; i++) {
            offsetsArray[i]->setBounds(offsetsRow.removeFromLeft(sliderWidth));
            if (isActive[i])
                offsetsArray[i]->setVisible(true);
        }
   
        area.removeFromBottom(10 * processor.paddingScalarY);
        
        Rectangle<int> gainsRow = area.removeFromBottom(multiHeight);
        
        gainsLabel.setBounds(gainsRow.removeFromLeft(gainsRow.getWidth()*0.1));
        gainsLabel.setVisible(true);
        
        for (int i = 0; i < NUM_KEYS; i++) {
            gainsArray[i]->setBounds(gainsRow.removeFromLeft(sliderWidth));
            if (isActive[i])
                gainsArray[i]->setVisible(true);
        }
        area.removeFromBottom(10 * processor.paddingScalarY);

        
        float sliderHeight = 50 + processor.paddingScalarY;
//        Rectangle<int> sliderRow = area.removeFromBottom(sliderHeight);
        
//        Rectangle<int> volumeSlider = sliderRow.removeFromLeft(sliderRow.getWidth()/3);
//        defGainSlider->setBounds(volumeSlider);
//        defGainSlider->setVisible(true);
//
//        Rectangle<int> startSlider = sliderRow.removeFromLeft(sliderRow.getWidth()/2);
//        startTimeSlider->setBounds(startSlider);
//        startTimeSlider->setVisible(true);
//
//        maxSympStringsSlider->setBounds(sliderRow);
//        maxSympStringsSlider->setVisible(true);
    
//
//        Rectangle<float> editAllBounds = absoluteKeyboard.getEditAllBounds();
//        editAllBounds.translate(absoluteKeyboard.getX(), absoluteKeyboard.getY());
//        lastNote.setBounds(editAllBounds.getRight() + gXSpacing, editAllBounds.getY(),editAllBounds.getWidth() * 2, editAllBounds.getHeight());
//        lastInterval.setBounds(lastNote.getRight() + gXSpacing, lastNote.getY(),lastNote.getWidth(), lastNote.getHeight());
        
    }
    else if (tab == 1){
        
#if JUCE_IOS
//        area.removeFromTop(gComponentComboBoxHeight);
        area.reduce(0.f, area.getHeight() * 0.2f);
#endif
        
//        defGainSlider->setVisible(true);
//        startTimeSlider->setVisible(true);
        
        ADSRSlider->setVisible(true);
        ADSRLabel.setVisible(true);

        //area = (getLocalBounds());
        area.reduce(20 * processor.paddingScalarX + 4, 20 * processor.paddingScalarY + 4);
        
        //single parameter sliders

        
        int columnHeight = area.getHeight();
        
        area.removeFromTop(columnHeight/5);
 
        int columnWidth = area.getWidth()/2;
//        defGainSlider->setBounds(area.removeFromTop(columnHeight / 5));
//        startTimeSlider->setBounds(area.removeFromTop(columnHeight / 5));
        
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

    comboBoxSlice.removeFromLeft(gXSpacing);

    Rectangle<int> modeSlice = area.removeFromTop(gComponentComboBoxHeight);
    modeSlice.removeFromRight(gXSpacing);
    lengthModeSelectCB.setBounds(modeSlice.removeFromRight(modeSlice.getWidth() / 2.));

    actionButton.toFront(false);

//    ResonancePreparation::Ptr prep = processor.gallery->getResonancePreparation(processor.updateState->currentResonanceId);

//    overtonesSlider->setVisible(true);
//    defGainSlider->setVisible(true);
//    startTimeSlider->setVisible(true);
//    exciteThreshSlider->setVisible(true);
//    attackThreshSlider->setVisible(true);
//    ADSRSlider->setVisible(true);
//    ADSRLabel.setVisible(true);
//
//    //area = (getLocalBounds());
//    area.reduce(10 * processor.paddingScalarX + 4, 10 * processor.paddingScalarY + 4);
//
//    //single parameter sliders
//
//    int columnHeight = area.getHeight();
//    defGainSlider->setBounds(area.removeFromTop(columnHeight / 5));
//    startTimeSlider->setBounds(area.removeFromTop(columnHeight / 5));
//    lengthSlider->setBounds(area.removeFromTop(columnHeight / 5));
//    exciteThreshSlider->setBounds(area.removeFromTop(columnHeight / 5));
//    attackThreshSlider->setBounds(area.removeFromTop(columnHeight / 5));
//

    //overtone slider

//    leftColumn.removeFromTop(gYSpacing + processor.paddingScalarY * 30);
//    //area.removeFromLeft(leftArrow.getWidth());
//    //area.removeFromRight(rightArrow.getWidth());
//    overtonesSlider->setBounds(leftColumn.removeFromTop(gComponentStackedSliderHeight + processor.paddingScalarY * 30));
    
    leftArrow.setBounds(0, getHeight() * 0.4, 50, 50);
    rightArrow.setBounds(getRight() - 50, getHeight() * 0.4, 50, 50);
    
//    offsets.setVisible();
    
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
    
    absoluteKeyboard->setVisible(false);
    fundamentalKeyboard->setVisible(false);
    
    for (int i = 0; i < NUM_KEYS; i++) {
        offsetsArray[i]->setVisible(false);
        gainsArray[i]->setVisible(false);
    }

    closestKeyLabel.setVisible(false);
    fundamentalLabel.setVisible(false);

}



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
    keyboardState.addListener(this);
    fundamentalKeyboardState.addListener(this);

    
    
    startTimer(30);
}

void ResonancePreparationEditor::update()
{
    if (processor.updateState->currentResonanceId < 0) return;
    ADSRSlider->setIsButtonOnly(true);

    ResonancePreparation::Ptr prep = processor.gallery->getResonancePreparation(processor.updateState->currentResonanceId);

    if (prep != nullptr)
    {
        selectCB.setSelectedId(processor.updateState->currentResonanceId, dontSendNotification);

        //overtonesSlider->setValue(prep->getDistancesFloat(), dontSendNotification);
        defGainSlider->setValue(prep->getDefGain(), dontSendNotification);
        //startTimeSlider->setValue(prep->getStartTime(), dontSendNotification);
        //lengthSlider->setValue(prep->getLength(), dontSendNotification);
        //exciteThreshSlider->setValue(prep->getExciteThresh(), dontSendNotification);
        //attackThreshSlider->setValue(prep->getAttackThresh(), dontSendNotification);

        Array<float> ADSRarray = prep->getADSRvals();
        ADSRSlider->setAttackValue(ADSRarray[0], dontSendNotification);
        ADSRSlider->setDecayValue(ADSRarray[1], dontSendNotification);
        ADSRSlider->setSustainValue(ADSRarray[2], dontSendNotification);
        ADSRSlider->setReleaseValue(ADSRarray[3], dontSendNotification);
        
        for (int i = 0; i < gainsArray.size(); i++){
            if (prep->isActive(i + 24)) {
                gainsArray[i]->setVisible(true);
                offsetsArray[i]->setVisible(true);
            }
            else {
                gainsArray[i]->setVisible(false);
                offsetsArray[i]->setVisible(false);
            }
        }
        
        
    }
}

void ResonancePreparationEditor::fillSelectCB(int last, int current)
{
}

void ResonancePreparationEditor::timerCallback()
{
//    Resonance::Ptr rs = processor.gallery->getResonance(processor.updateState->currentId);
//
//    DBG("Ringing: " + rs->getRinging());
}

void ResonancePreparationEditor::actionButtonCallback(int action, ResonancePreparationEditor*)
{
    //so far nothing needed here, there will inevitably need to be a tower of if statements later
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
//    else
//    {
//        for (int i=0; i<fundamentalButtons.size(); i++)
//        {
//            if (b == fundamentalButtons[i])
//            {
//                ResonancePreparation::Ptr prep = processor.gallery->getResonancePreparation(processor.updateState->currentResonanceId);
//                prep->setFundamental(i);
////                DBG("Keymap toggle change: " + (String)cKeymapTargetTypes[i] + " " + String((int)b->getToggleState()));
////                fundamentalButtons[i]->setToggleState(true, dontSendNotification);
//                DBG("new fundamental:" + String(prep->getFundamental()));
//                DBG("button pressed:" + String(i));
//                DBG("number of buttons:" + String(fundamentalButtons.size()));
//                DBG("clicked toggle state:" + String((int)b->getToggleState()));
//            }
//        }
        processor.updateState->editsMade = true;
    }
//    else if (b == fundamentalButtons[1])
//    {
//        DBG("fundamental button pressed");
//        fundamentalButtons[1]->setToggleState(false, dontSendNotification);
//
//        ResonancePreparation::Ptr prep = processor.gallery->getResonancePreparation(processor.updateState->currentResonanceId);
//        prep->setFundamental(1);
//    }
    //can leave blank for now since there are no buttons, will need to fill in later
//}

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
    }
    else {
        for (int i = 0; i < offsetsArray.size(); i++){
            if (name == offsetsArray[i]->getName()) {
                prep->setOffset(i, val);
                DBG("set" + String(i) + "offset to " + String(val));
            }
        }
        for (int i = 0; i < gainsArray.size(); i++){
            if (name == gainsArray[i]->getName()) {
                prep->setGain(i, val);
                DBG("set" + String(i) + "offset to " + String(val));
            }
        }
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
    if (source == &keyboardState)
    {

        prep->toggleNote(midiNoteNumber);
        absoluteKeyboard->setKeysInKeymap(prep->getKeys());
       
        if (prep->isActive(midiNoteNumber)) {
            prep->removeActive(midiNoteNumber);
            isActive.set(midiNoteNumber - 24, false);
            DBG("removing active" + String(midiNoteNumber));
        }
        else {
            prep->addActive(midiNoteNumber, 1.0, 0);
            isActive.set(midiNoteNumber - 24, true);
            DBG("adding active"+ String(midiNoteNumber));
        }
    }
    else if (source == &fundamentalKeyboardState)
    {

        prep->toggleFundamental(midiNoteNumber);
        fundamentalKeyboard->setKeysInKeymap({midiNoteNumber});
        DBG("fundamental key toggled");
    }
    update();

}

void ResonancePreparationEditor::keyboardSliderChanged(String name, Array<float> values)
{
    
//    processor.gallery->setGalleryDirty(true);
//
//    processor.updateState->editsMade = true;
}

