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
    BKViewController(p, theGraph, 1)
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

    overtonesSlider = std::make_unique<BKStackedSlider>("overtones", -50, 50, -50, 50, 0, 1);
    overtonesSlider->setTooltip("Determines overtone pitches that resonance with a fundamental; control-click to add another voice, double-click to edit all");
    addAndMakeVisible(*overtonesSlider);

    defGainSlider = std::make_unique<BKSingleSlider>("resonant note volume (dB)", cResonanceDefGain, -100, 24, 0, 0.01, "-inf");
    defGainSlider->setJustifyRight(false);
    defGainSlider->setToolTipString("Adjusts overall volume of resonant notes");
    addAndMakeVisible(*defGainSlider);

    startTimeSlider = std::make_unique<BKSingleSlider>("start time (ms)", cResonanceStartTime, -4000, 4000, 2000, 1);
    startTimeSlider->setJustifyRight(false);
    startTimeSlider->setToolTipString("Adjusts start time into sample of resonant notes");
    addAndMakeVisible(*startTimeSlider);

    lengthSlider = std::make_unique<BKSingleSlider>("note length (ms)", cResonanceLength, -4000, 4000, 2000, 1);
    lengthSlider->setJustifyRight(false);
    lengthSlider->setToolTipString("Adjusts length of resonant notes");
    addAndMakeVisible(*lengthSlider);

    exciteThreshSlider = std::make_unique<BKSingleSlider>("excitement threshold (dB)", cResonanceExciteThresh, -100, 24, 1, 0.01, "-inf");
    exciteThreshSlider->setJustifyRight(false);
    exciteThreshSlider->setToolTipString("Adjusts the gain threshold which strings must be below to resonate");
    addAndMakeVisible(*exciteThreshSlider);

    attackThreshSlider = std::make_unique<BKSingleSlider>("attack threshold (ms)", cResonanceAttackThresh, -100, 24, 0, 0.01);
    attackThreshSlider->setJustifyRight(false);
    attackThreshSlider->setToolTipString("Adjusts the velocity threshold which attacking strings must be above to resonate");
    addAndMakeVisible(*attackThreshSlider);


    ADSRSlider = std::make_unique<BKADSRSlider>("Resonance envelope");
    ADSRSlider->setButtonText("edit resonance envelope");
    ADSRSlider->setToolTip("ADSR settings for resonant notes");
    ADSRSlider->setButtonMode(false);
    addAndMakeVisible(*ADSRSlider);

    ADSRLabel.setText("ADSR", dontSendNotification);
    ADSRLabel.setJustificationType(Justification::centred);
    addAndMakeVisible(&ADSRLabel, ALL);

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

    /*actionButton.setBounds(selectCB.getRight() + gXSpacing,
        selectCB.getY(),
        selectCB.getWidth() * 0.5,
        selectCB.getHeight());

    alternateMod.setBounds(actionButton.getRight() + gXSpacing,
        actionButton.getY(),
        selectCB.getWidth(),
        actionButton.getHeight());*/

    comboBoxSlice.removeFromLeft(gXSpacing);

    Rectangle<int> modeSlice = area.removeFromTop(gComponentComboBoxHeight);
    modeSlice.removeFromRight(gXSpacing);
    //modeSlice.reduce(4 + 2.*gPaddingConst * processor.paddingScalarX, 0);
    //lengthModeSelectCB.setBounds(modeSlice.removeFromLeft(modeSlice.getWidth() / 2.));
    lengthModeSelectCB.setBounds(modeSlice.removeFromRight(modeSlice.getWidth() / 2.));

    actionButton.toFront(false);

    ResonancePreparation::Ptr prep = processor.gallery->getResonancePreparation(processor.updateState->currentResonanceId);

    overtonesSlider->setVisible(true);
    defGainSlider->setVisible(true);
    startTimeSlider->setVisible(true);
    lengthSlider->setVisible(true);
    exciteThreshSlider->setVisible(true);
    attackThreshSlider->setVisible(true);
    ADSRSlider->setVisible(true);
    ADSRLabel.setVisible(true);

    //area = (getLocalBounds());
    area.reduce(10 * processor.paddingScalarX + 4, 10 * processor.paddingScalarY + 4);
    
    //single parameter sliders

    int columnHeight = area.getHeight();
    defGainSlider->setBounds(area.removeFromTop(columnHeight / 5));
    startTimeSlider->setBounds(area.removeFromTop(columnHeight / 5));
    lengthSlider->setBounds(area.removeFromTop(columnHeight / 5));
    exciteThreshSlider->setBounds(area.removeFromTop(columnHeight / 5));
    attackThreshSlider->setBounds(area.removeFromTop(columnHeight / 5));

    //ADSR stuff

    //area.removeFromLeft(processor.paddingScalarX * 20);
    //area.removeFromRight(processor.paddingScalarX * 20);
    area.removeFromTop(20 * processor.paddingScalarY);

    columnHeight = leftColumn.getHeight();

    ADSRLabel.setBounds(leftColumn.removeFromTop(columnHeight * 0.15));
    ADSRSlider->setBounds(leftColumn.removeFromTop(columnHeight * 0.35));

    //overtone slider

    leftColumn.removeFromTop(gYSpacing + processor.paddingScalarY * 30);
    //area.removeFromLeft(leftArrow.getWidth());
    //area.removeFromRight(rightArrow.getWidth());
    overtonesSlider->setBounds(leftColumn.removeFromTop(gComponentStackedSliderHeight + processor.paddingScalarY * 30));
}

void ResonanceViewController::invisible(void)
{
    overtonesSlider->setVisible(false);
    defGainSlider->setVisible(false);
    startTimeSlider->setVisible(false);
    lengthSlider->setVisible(false);
    exciteThreshSlider->setVisible(false);
    attackThreshSlider->setVisible(false);
    ADSRSlider->setVisible(false);
    ADSRLabel.setVisible(false);
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

    overtonesSlider->addMyListener(this);
    defGainSlider->addMyListener(this);
    startTimeSlider->addMyListener(this);
    lengthSlider->addMyListener(this);
    exciteThreshSlider->addMyListener(this);
    attackThreshSlider->addMyListener(this);

    ADSRSlider->addMyListener(this);

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

        overtonesSlider->setValue(prep->getDistancesFloat(), dontSendNotification);
        defGainSlider->setValue(prep->getDefGain(), dontSendNotification);
        startTimeSlider->setValue(prep->getStartTime(), dontSendNotification);
        lengthSlider->setValue(prep->getLength(), dontSendNotification);
        exciteThreshSlider->setValue(prep->getExciteThresh(), dontSendNotification);
        attackThreshSlider->setValue(prep->getAttackThresh(), dontSendNotification);

        Array<float> ADSRarray = prep->getADSRvals();
        ADSRSlider->setAttackValue(ADSRarray[0], dontSendNotification);
        ADSRSlider->setDecayValue(ADSRarray[1], dontSendNotification);
        ADSRSlider->setSustainValue(ADSRarray[2], dontSendNotification);
        ADSRSlider->setReleaseValue(ADSRarray[3], dontSendNotification);
    }
}

void ResonancePreparationEditor::fillSelectCB(int last, int current)
{
}

void ResonancePreparationEditor::timerCallback()
{
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
    //can leave blank for now since there are no buttons, will need to fill in later
}

void ResonancePreparationEditor::BKEditableComboBoxChanged(String name, BKEditableComboBox* cb)
{
    //will need to fill this in eventually, should be fine empty for now
}

void ResonancePreparationEditor::BKSingleSliderValueChanged(BKSingleSlider* slider, String name, double val)
{
    ResonancePreparation::Ptr prep = processor.gallery->getResonancePreparation(processor.updateState->currentResonanceId);

    if (name == "resonant note volume (dB)")
    {
        DBG("set resonant note volume to " + String(val));
        prep->setDefGain(val);
    }
    else if (name == "start time (ms)")
    {
        DBG("set start time to " + String(val));
        prep->setStartTime(val);
    }
    else if (name == "note length (ms)")
    {
        DBG("set length to " + String(val));
        prep->setLength(val);
    }
    else if (name == "excitement threshold (dB)")
    {
        DBG("set excitement threshold to " + String(val));
        prep->setExciteThresh(val);
    }
    else if (name == "attack threshold (dB)")
    {
        DBG("set attack threshold to " + String(val));
        prep->setAttackThresh(val);
    }

    processor.updateState->editsMade = true;
}

void ResonancePreparationEditor::BKStackedSliderValueChanged(String name, Array<float> val)
{
    DBG("received overtone slider " + name);

    ResonancePreparation::Ptr prep = processor.gallery->getResonancePreparation(processor.updateState->currentResonanceId);

    prep->setDistances(val);

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
    //no range sliders at the moment
}

