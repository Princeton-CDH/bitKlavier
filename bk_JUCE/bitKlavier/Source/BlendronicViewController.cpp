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
    
    selectCB.setName("Blendronic");
    selectCB.setTooltip("Select from available saved preparation settings");
    selectCB.addSeparator();
    selectCB.addListener(this);
    selectCB.setSelectedItemIndex(0);
    addAndMakeVisible(selectCB);
    
    lengthModeSelectCB.setName("Length Mode");
    lengthModeSelectCB.setTooltip("Indicates how Blendronic calculates length of reverse wave");
    lengthModeSelectCB.addSeparator();
    lengthModeSelectCB.BKSetJustificationType(juce::Justification::centredRight);
    lengthModeSelectCB.setSelectedItemIndex(0);
    fillModeSelectCB();
    addAndMakeVisible(lengthModeSelectCB);
    
    transpositionSlider = std::make_unique<BKStackedSlider>("transpositions", -12, 12, -12, 12, 0, 0.01);
    transpositionSlider->setTooltip("Determines pitch (in semitones) of Blendronic notes; control-click to add another voice, double-click to edit all");
    addAndMakeVisible(*transpositionSlider);
    
    lengthMultiplierSlider = std::make_unique<BKSingleSlider>("note length multiplier", 0, 10, 1, 0.01);
    lengthMultiplierSlider->setToolTipString("Changes length of Blendronic wave as a factor of note duration");
    lengthMultiplierSlider->setSkewFactorFromMidPoint(1.);
    addAndMakeVisible(*lengthMultiplierSlider);
    
    holdTimeMinMaxSlider = std::make_unique<BKRangeSlider>("hold time (ms)", 0., 12000., 0.0, 12000., 1);
    holdTimeMinMaxSlider->setToolTipString("Sets Min and Max time (ms) held to trigger swell; Min can be greater than Max");
    holdTimeMinMaxSlider->setJustifyRight(true);
    addAndMakeVisible(*holdTimeMinMaxSlider);
    
    velocityMinMaxSlider = std::make_unique<BKRangeSlider>("velocity min/max (0-127)", 0, 127, 0, 127, 1);
    velocityMinMaxSlider->setToolTipString("Sets Min and Max velocity (0-127) to trigger swell; Min can be greater than Max");
    velocityMinMaxSlider->setJustifyRight(true);
    addAndMakeVisible(*velocityMinMaxSlider);
    
    clusterMinSlider = std::make_unique<BKSingleSlider>("cluster min", 1, 10, 1, 1);
    clusterMinSlider->setToolTipString("Sets Min cluster size needed to trigger swell");
    clusterMinSlider->setJustifyRight(true);
    addAndMakeVisible(*clusterMinSlider);
    
    //clusterThresholdSlider
    clusterThresholdSlider = std::make_unique<BKSingleSlider>("cluster thresh", 0, 1000, 150, 1);
    clusterThresholdSlider->setToolTipString("time between note releases (ms) to be included in cluster");
    clusterThresholdSlider->setJustifyRight(true);
    addAndMakeVisible(*clusterThresholdSlider);
    
    beatsToSkipSlider = std::make_unique<BKSingleSlider>("beats to skip", 0, 10, 0, 1);
    beatsToSkipSlider->setToolTipString("Indicates how long Blendronic wave lasts with respect to linked Synchronic sequence");
    addAndMakeVisible(*beatsToSkipSlider);
    beatsToSkipSlider->setVisible(false);
    
    gainSlider = std::make_unique<BKSingleSlider>("gain", 0, 10, 1, 0.01);
    gainSlider->setToolTipString("Volume multiplier for Blendronic notes");
    gainSlider->setSkewFactorFromMidPoint(1.);
    gainSlider->setJustifyRight(false);
    addAndMakeVisible(*gainSlider);
    
    addAndMakeVisible(actionButton);
    actionButton.setButtonText("Action");
    actionButton.setTooltip("Create, duplicate, rename, delete, or reset current settings");
    actionButton.addListener(this);
    
    reverseADSRSlider = std::make_unique<BKADSRSlider>("reverseEnvelope");
    reverseADSRSlider->setButtonText("edit reverse envelope");
    reverseADSRSlider->setToolTip("ADSR settings for Blendronic wave");
    reverseADSRSlider->setButtonMode(false);
    addAndMakeVisible(*reverseADSRSlider);
    
    reverseADSRLabel.setText("Reverse ADSR", dontSendNotification);
    reverseADSRLabel.setJustificationType(Justification::centred);
    addAndMakeVisible(&reverseADSRLabel, ALL);
    
    undertowADSRSlider = std::make_unique<BKADSRSlider>("undertowEnvelope");
    undertowADSRSlider->setButtonText("edit undertow envelope");
    undertowADSRSlider->setToolTip("ADSR settings for Undertow");
    undertowADSRSlider->setButtonMode(false);
    addAndMakeVisible(*undertowADSRSlider);
    
    undertowADSRLabel.setText("Undertow ADSR", dontSendNotification);
    undertowADSRLabel.setJustificationType(Justification::centred);
    addAndMakeVisible(&undertowADSRLabel, ALL);
    
    keyOnResetToggle.setTooltip("interrupts currently sounding blendronic notes of the same pitch");
    addAndMakeVisible(keyOnResetToggle);
    
    keyOnResetLabel.setText("key-on reset:", dontSendNotification);
    keyOnResetLabel.setJustificationType(Justification::centredRight);
    keyOnResetLabel.setTooltip("interrupts currently sounding blendronic notes of the same pitch");
    addAndMakeVisible(keyOnResetLabel);
    
    
#if JUCE_IOS
    holdTimeMinMaxSlider->addWantsBigOneListener(this);
    velocityMinMaxSlider->addWantsBigOneListener(this);
    clusterMinSlider->addWantsBigOneListener(this);
    clusterThresholdSlider->addWantsBigOneListener(this);
    beatsToSkipSlider->addWantsBigOneListener(this);
    gainSlider->addWantsBigOneListener(this);
    lengthMultiplierSlider->addWantsBigOneListener(this);
    transpositionSlider->addWantsBigOneListener(this);
    nDisplaySlider.addWantsBigOneListener(this);
#endif
    
    showADSR = false;
    showReverseADSR = false;
    showUndertowADSR = false;
    
    currentTab = 0;
    displayTab(currentTab);
    
}

void BlendronicViewController::invisible(void)
{
    gainSlider->setVisible(false);
    lengthMultiplierSlider->setVisible(false);
    beatsToSkipSlider->setVisible(false);
    transpositionSlider->setVisible(false);
    
    
    holdTimeMinMaxSlider->setVisible(false);
    velocityMinMaxSlider->setVisible(false);
    clusterMinSlider->setVisible(false);
    clusterThresholdSlider->setVisible(false);
    
    reverseADSRSlider->setVisible(false);
    undertowADSRSlider->setVisible(false);
    reverseADSRLabel.setVisible(false);
    undertowADSRLabel.setVisible(false);
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
    
    comboBoxSlice.removeFromLeft(gXSpacing);
    
    Rectangle<int> modeSlice = area.removeFromTop(gComponentComboBoxHeight);
    modeSlice.removeFromRight(gXSpacing);
    //modeSlice.reduce(4 + 2.*gPaddingConst * processor.paddingScalarX, 0);
    //lengthModeSelectCB.setBounds(modeSlice.removeFromLeft(modeSlice.getWidth() / 2.));
    lengthModeSelectCB.setBounds(modeSlice.removeFromRight(modeSlice.getWidth() / 2.));
    
    float dim = lengthModeSelectCB.getHeight();
    keyOnResetToggle.setBounds(lengthModeSelectCB.getX() - (dim + gXSpacing), lengthModeSelectCB.getY(), dim, dim);
    keyOnResetToggle.changeWidthToFitText();
    keyOnResetLabel.setBounds(keyOnResetToggle.getX() - 200, keyOnResetToggle.getY(), 200, dim);
    
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
    
    int col1x = x0;
    int col2x = x0 + width * 0.5f;
    
    if (tab == 0)
    {
        BlendronomerPreparation::Ptr prep = processor.gallery->getStaticBlendronomerPreparation(processor.updateState->currentBlendronicId);
        
//        gainSlider->setVisible(true);
//        transpositionSlider->setVisible(true);
//        nDisplaySlider.setVisible(true);
//
//        if(prep->getMode() == NoteLengthSync)
//        {
//            lengthMultiplierSlider->setVisible(true);
//            beatsToSkipSlider->setVisible(false);
//        }
//        else
//        {
//            lengthMultiplierSlider->setVisible(false);
//            beatsToSkipSlider->setVisible(true);
//        }
        
        Rectangle<int> area (getLocalBounds());
        area.reduce(10 * processor.paddingScalarX + 4, 10 * processor.paddingScalarY + 4);
        
        Rectangle<int> nDisplayRow = area.removeFromBottom(100 + 80 * processor.paddingScalarY);
        nDisplayRow.reduce(0, 4);
        nDisplayRow.removeFromLeft(gXSpacing + gPaddingConst * processor.paddingScalarX * 0.5);
        nDisplayRow.removeFromRight(gXSpacing + gPaddingConst * processor.paddingScalarX * 0.5);
        
        area.removeFromBottom(gYSpacing + processor.paddingScalarY * 30);
        area.removeFromLeft(leftArrow.getWidth());
        area.removeFromRight(rightArrow.getWidth());
        transpositionSlider->setBounds(area.removeFromBottom(gComponentStackedSliderHeight + processor.paddingScalarY * 30));
        
        area.removeFromBottom(gYSpacing + processor.paddingScalarY * 30);
        
        Rectangle<int> leftColumn (area.removeFromLeft(area.getWidth()* 0.5));
        
        leftColumn.removeFromRight(processor.paddingScalarX * 20);
        leftColumn.removeFromLeft(processor.paddingScalarX * 20);
        
        area.removeFromLeft(processor.paddingScalarX * 20); //area is now right column
        area.removeFromRight(processor.paddingScalarX * 20);
        
        gainSlider->setBounds(leftColumn.removeFromBottom(gComponentSingleSliderHeight + processor.paddingScalarY * 30));
        lengthMultiplierSlider->setBounds(area.removeFromBottom(gComponentSingleSliderHeight + processor.paddingScalarY * 30));
        beatsToSkipSlider->setBounds(lengthMultiplierSlider->getBounds());
        
    }
    else if (tab == 1)
    {
        // SET VISIBILITY
        holdTimeMinMaxSlider->setVisible(true);
        velocityMinMaxSlider->setVisible(true);
        clusterMinSlider->setVisible(true);
        clusterThresholdSlider->setVisible(true);
        
        Rectangle<int> area (getBounds());
        area.removeFromTop(selectCB.getHeight() + 100 * processor.paddingScalarY + 4 + gYSpacing);
        area.removeFromRight(rightArrow.getWidth());
        area.removeFromLeft(leftArrow.getWidth());
        
        area.removeFromLeft(processor.paddingScalarX * 100);
        area.removeFromRight(processor.paddingScalarX * 100);
        
        int columnHeight = area.getHeight();
        
        holdTimeMinMaxSlider->setBounds(area.removeFromTop(columnHeight / 4));
        velocityMinMaxSlider->setBounds(area.removeFromTop(columnHeight / 4));
        clusterMinSlider->setBounds(area.removeFromTop(columnHeight / 4));
        clusterThresholdSlider->setBounds(area.removeFromTop(columnHeight / 4));
        
    }
    else if (tab == 2)
    {
        reverseADSRSlider->setVisible(true);
        undertowADSRSlider->setVisible(true);
        reverseADSRLabel.setVisible(true);
        undertowADSRLabel.setVisible(true);
        
        Rectangle<int> area (getBounds());
        Rectangle<int> areaSave (getBounds());
        area.removeFromTop(selectCB.getHeight() + 100 * processor.paddingScalarY + 4 + gYSpacing);
        area.removeFromRight(rightArrow.getWidth());
        area.removeFromLeft(leftArrow.getWidth());
        
        area.removeFromLeft(processor.paddingScalarX * 20);
        area.removeFromRight(processor.paddingScalarX * 20);
        
        int columnHeight = area.getHeight();
        
        reverseADSRSlider->setBounds(area.removeFromTop(columnHeight * 0.5));
        undertowADSRSlider->setBounds(area.removeFromTop(columnHeight * 0.5));
        
        reverseADSRLabel.setBounds(areaSave.removeFromTop(columnHeight * 0.5));
        undertowADSRLabel.setBounds(areaSave.removeFromTop(columnHeight * 0.5));
        
    }
}

void BlendronicViewController::paint (Graphics& g)
{
    g.fillAll(Colours::black);
}

void BlendronicViewController::setShowADSR(String name, bool newval)
{
    showADSR = newval;
    
    if(showADSR)
    {
        beatsToSkipSlider->setVisible(false);
        gainSlider->setVisible(false);
        lengthMultiplierSlider->setVisible(false);
        transpositionSlider->setVisible(false);
        lengthModeSelectCB.setVisible(false);
        
        if(name == reverseADSRSlider->getName()) {
            reverseADSRSlider->setButtonText("close  reverse envelope");
            undertowADSRSlider->setVisible(false);
            
            showReverseADSR = true;
            showUndertowADSR = false;
        }
        else if(name == undertowADSRSlider->getName()) {
            undertowADSRSlider->setButtonText("close undertow envelope");
            reverseADSRSlider->setVisible(false);
            
            showReverseADSR = false;
            showUndertowADSR = true;
        }
    }
    else
    {
        if (currentTab == 0)
        {
            if (lengthModeSelectCB.getSelectedId() == 0)
            {
                lengthMultiplierSlider->setVisible(true);
            }
            else
            {
                beatsToSkipSlider->setVisible(true);
            }
        }
        
        gainSlider->setVisible(true);
        transpositionSlider->setVisible(true);
        lengthModeSelectCB.setVisible(true);
        
        reverseADSRSlider->setVisible(true);
        undertowADSRSlider->setVisible(true);
        
        if(name == reverseADSRSlider->getName()) reverseADSRSlider->setButtonText("edit reverse envelope");
        else if(name == undertowADSRSlider->getName()) undertowADSRSlider->setButtonText("edit undertow envelope");
    }
    
    resized();
}

void BlendronicViewController::resized()
{
    displayShared();
    displayTab(currentTab);
    
#if 0
    Rectangle<int> area (getLocalBounds());
    
    iconImageComponent.setBounds(area);
    area.reduce(10 * processor.paddingScalarX + 4, 10 * processor.paddingScalarY + 4);
    
    Rectangle<int> areaSave = area;
    
    Rectangle<int> nDisplayRow = area.removeFromBottom(100 + 80 * processor.paddingScalarY);
    nDisplayRow.reduce(0, 4);
    nDisplayRow.removeFromLeft(gXSpacing + gPaddingConst * processor.paddingScalarX * 0.5);
    nDisplayRow.removeFromRight(gXSpacing + gPaddingConst * processor.paddingScalarX * 0.5);
    nDisplaySlider.setBounds(nDisplayRow);
    
    Rectangle<int> leftColumn = area.removeFromLeft(area.getWidth() * 0.5);
    Rectangle<int> comboBoxSlice = leftColumn.removeFromTop(gComponentComboBoxHeight);
    comboBoxSlice.removeFromRight(4 + 2.*gPaddingConst * processor.paddingScalarX);
    comboBoxSlice.removeFromLeft(gXSpacing);
    hideOrShow.setBounds(comboBoxSlice.removeFromLeft(gComponentComboBoxHeight));
    comboBoxSlice.removeFromLeft(gXSpacing);
    selectCB.setBounds(comboBoxSlice.removeFromLeft(comboBoxSlice.getWidth() / 2.));
    comboBoxSlice.removeFromLeft(gXSpacing);
    
    actionButton.setBounds(selectCB.getRight()+gXSpacing,
                           selectCB.getY(),
                           selectCB.getWidth() * 0.5,
                           selectCB.getHeight());
    
    /* *** above here should be generic to all prep layouts *** */
    /* ***    below here will be specific to each prep      *** */
    if(!showADSR)
    {
        Rectangle<int> modeSlice = area.removeFromTop(gComponentComboBoxHeight);
        modeSlice.removeFromRight(gXSpacing);
        //modeSlice.reduce(4 + 2.*gPaddingConst * processor.paddingScalarX, 0);
        //lengthModeSelectCB.setBounds(modeSlice.removeFromLeft(modeSlice.getWidth() / 2.));
        lengthModeSelectCB.setBounds(modeSlice.removeFromRight(modeSlice.getWidth() / 2.));
        
        float dim = lengthModeSelectCB.getHeight();
        keyOnResetToggle.setBounds(lengthModeSelectCB.getX() - (dim + gXSpacing), lengthModeSelectCB.getY(), dim, dim);
        keyOnResetToggle.changeWidthToFitText();
        
        keyOnResetLabel.setBounds(keyOnResetToggle.getX() - 100, keyOnResetToggle.getY(), 100, dim);
        
        DBG("kt: " + rectangleToString(keyOnResetToggle.getBounds()) );
        DBG("kl: " + rectangleToString(keyOnResetLabel.getBounds()) );
        
        Rectangle<int> sliderSlice = area;
        sliderSlice.removeFromLeft(gXSpacing + 2.*gPaddingConst * processor.paddingScalarX);
        //sliderSlice.removeFromRight(gXSpacing - gComponentSingleSliderXOffset);
        /*
         sliderSlice.reduce(4 + 2.*gPaddingConst * processor.paddingScalarX,
         4 + 2.*gPaddingConst * processor.paddingScalarY);
         */
        
        int nextCenter = sliderSlice.getY() + sliderSlice.getHeight() * 0.15f;
        lengthMultiplierSlider->setBounds(sliderSlice.getX(),
                                          nextCenter - gComponentSingleSliderHeight/2 + 8,
                                          sliderSlice.getWidth(),
                                          gComponentSingleSliderHeight);
        beatsToSkipSlider->setBounds(lengthMultiplierSlider->getBounds());
        
        nextCenter = sliderSlice.getY() + sliderSlice.getHeight() * 0.4f;
        holdTimeMinMaxSlider->setBounds(sliderSlice.getX(),
                                        nextCenter - gComponentSingleSliderHeight/2 + 4,
                                        sliderSlice.getWidth(),
                                        gComponentSingleSliderHeight);
        
        nextCenter = sliderSlice.getY() + sliderSlice.getHeight() * 0.65f;
        clusterMinSlider->setBounds(sliderSlice.getX(),
                                    nextCenter - gComponentSingleSliderHeight/2 + 4,
                                    sliderSlice.getWidth(),
                                    gComponentSingleSliderHeight);
        
        
        
        nextCenter = sliderSlice.getY() + sliderSlice.getHeight() * 0.9f;
        velocityMinMaxSlider->setBounds(sliderSlice.getX(),
                                        nextCenter - gComponentSingleSliderHeight/2 + 4,
                                        sliderSlice.getWidth(),
                                        gComponentSingleSliderHeight);
        
        
        //leftColumn.reduce(4, 0);
        leftColumn.removeFromRight(gXSpacing + 2.*gPaddingConst * processor.paddingScalarX);
        leftColumn.removeFromLeft(gXSpacing);
        transpositionSlider->setBounds(leftColumn.getX(),
                                       lengthMultiplierSlider->getY(),
                                       leftColumn.getWidth(),
                                       gComponentStackedSliderHeight + processor.paddingScalarY * 30);
        
        reverseADSRSlider->setBounds(leftColumn.getX(),
                                     transpositionSlider->getBottom() + gComponentComboBoxHeight * 0.5,
                                     leftColumn.getWidth() * 0.5 - gYSpacing,
                                     gComponentComboBoxHeight);
        
        undertowADSRSlider->setBounds(leftColumn.getX() + leftColumn.getWidth() * 0.5 + gYSpacing,
                                      transpositionSlider->getBottom() + gComponentComboBoxHeight * 0.5,
                                      leftColumn.getWidth() * 0.5 - gYSpacing,
                                      gComponentComboBoxHeight);
        
        gainSlider->setBounds(leftColumn.getX(),
                              reverseADSRSlider->getBottom() + gComponentComboBoxHeight * 0.5,
                              leftColumn.getWidth(),
                              gComponentStackedSliderHeight + processor.paddingScalarY * 30);
    }
    else
    {
        areaSave.removeFromTop(gYSpacing * 2 + gYSpacing + 8.*gPaddingConst * processor.paddingScalarY);
        Rectangle<int> adsrSliderSlice = areaSave.removeFromTop(gComponentComboBoxHeight * 2 + gComponentSingleSliderHeight * 2 + gYSpacing * 3);
        if(showReverseADSR) reverseADSRSlider->setBounds(adsrSliderSlice);
        else undertowADSRSlider->setBounds(adsrSliderSlice);
        
        selectCB.toFront(false);
    }
#endif
}

void BlendronicViewController::fillModeSelectCB(void)
{
    
//    lengthModeSelectCB.clear(dontSendNotification);
//    for (int i = 0; i < cBlendronicSyncModes.size(); i++)
//    {
//        String name = cBlendronicSyncModes[i];
//        if (name != String())  lengthModeSelectCB.addItem(name, i+1);
//        else                        lengthModeSelectCB.addItem(String(i+1), i+1);
//    }
//
//    lengthModeSelectCB.setSelectedItemIndex(0, NotificationType::dontSendNotification);
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
    lengthModeSelectCB.addListener(this);
    transpositionSlider->addMyListener(this);
    lengthMultiplierSlider->addMyListener(this);
    holdTimeMinMaxSlider->addMyListener(this);
    velocityMinMaxSlider->addMyListener(this);
    clusterMinSlider->addMyListener(this);
    clusterThresholdSlider->addMyListener(this);
    beatsToSkipSlider->addMyListener(this);
    
    keyOnResetToggle.addListener(this);
    
    gainSlider->addMyListener(this);
    
    reverseADSRSlider->addMyListener(this);
    undertowADSRSlider->addMyListener(this);
    
    startTimer(20);
}

void BlendronicPreparationEditor::BKEditableComboBoxChanged(String name, BKEditableComboBox* cb)
{
    Blendronomer::Ptr blendronic = processor.gallery->getBlendronomer(processor.updateState->currentBlendronicId);
    
    blendronic->setName(name);
}

void BlendronicPreparationEditor::update(void)
{
    if (processor.updateState->currentBlendronicId < 0) return;
    setShowADSR(reverseADSRSlider->getName(), false);
    reverseADSRSlider->setIsButtonOnly(true);
    setShowADSR(undertowADSRSlider->getName(), false);
    undertowADSRSlider->setIsButtonOnly(true);
    setSubWindowInFront(false);
    
    BlendronomerPreparation::Ptr prep = processor.gallery->getActiveBlendronomerPreparation(processor.updateState->currentBlendronicId);
    
    if (prep != nullptr)
    {
//        nDisplaySlider.setWaveDistance(prep->getWavedistance(), dontSendNotification);
//        nDisplaySlider.setUndertow(prep->getUndertow(), dontSendNotification);
//
//        selectCB.setSelectedId(processor.updateState->currentBlendronicId, dontSendNotification);
//        lengthModeSelectCB.setSelectedItemIndex(prep->getMode(), dontSendNotification);
//
//        //transpositionSlider->setValue(prep->getTransposition(), dontSendNotification);
//        transpositionSlider->setTo(prep->getTransposition(), dontSendNotification);
//        lengthMultiplierSlider->setValue(prep->getLengthMultiplier(), dontSendNotification);
//
//        holdTimeMinMaxSlider->setMinValue(prep->getHoldMin(), dontSendNotification);
//        holdTimeMinMaxSlider->setMaxValue(prep->getHoldMax(), dontSendNotification);
//
//        velocityMinMaxSlider->setMinValue(prep->getVelocityMin(), dontSendNotification);
//        velocityMinMaxSlider->setMaxValue(prep->getVelocityMax(), dontSendNotification);
//
//        clusterMinSlider->setValue(prep->getClusterMin(), dontSendNotification);
//        clusterThresholdSlider->setValue(prep->getClusterThreshold(), dontSendNotification);
//
//        keyOnResetToggle.setToggleState(prep->getKeyOnReset(), dontSendNotification);
//
//        beatsToSkipSlider->setValue(prep->getBeatsToSkip(), dontSendNotification);
//        gainSlider->setValue(prep->getGain(), dontSendNotification);
//
//        if (currentTab == 0)
//        {
//            if(prep->getMode() == NoteLengthSync)
//            {
//
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
//        reverseADSRSlider->setAttackValue(prep->getReverseAttack(), dontSendNotification);
//        reverseADSRSlider->setDecayValue(prep->getReverseDecay(), dontSendNotification);
//        reverseADSRSlider->setSustainValue(prep->getReverseSustain(), dontSendNotification);
//        reverseADSRSlider->setReleaseValue(prep->getReverseRelease(), dontSendNotification);
//
//        undertowADSRSlider->setAttackValue(prep->getUndertowAttack(), dontSendNotification);
//        undertowADSRSlider->setDecayValue(prep->getUndertowDecay(), dontSendNotification);
//        undertowADSRSlider->setSustainValue(prep->getUndertowSustain(), dontSendNotification);
//        undertowADSRSlider->setReleaseValue(prep->getUndertowRelease(), dontSendNotification);
        
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
    processor.gallery->add(PreparationTypeBlendronomer);
    
    return processor.gallery->getAllBlendronomer().getLast()->getId();
}

int BlendronicPreparationEditor::duplicatePreparation(void)
{
    processor.gallery->duplicate(PreparationTypeBlendronomer, processor.updateState->currentBlendronicId);
    
    return processor.gallery->getAllBlendronomer().getLast()->getId();
}

void BlendronicPreparationEditor::deleteCurrent(void)
{
    int BlendronicId = selectCB.getSelectedId();
    int index = selectCB.getSelectedItemIndex();
    
    if ((index == 0) && (selectCB.getItemId(index+1) == -1)) return;
    
    processor.gallery->remove(PreparationTypeBlendronomer, BlendronicId);
    
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
        processor.reset(PreparationTypeBlendronomer, processor.updateState->currentBlendronicId);
        vc->update();
    }
    else if (action == 5)
    {
        processor.clear(PreparationTypeBlendronomer, processor.updateState->currentBlendronicId);
        vc->update();
    }
    else if (action == 6)
    {
        AlertWindow prompt("", "", AlertWindow::AlertIconType::QuestionIcon);
        
        int Id = processor.updateState->currentBlendronicId;
        Blendronomer::Ptr prep = processor.gallery->getBlendronomer(Id);
        
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
        Blendronomer::Ptr prep = processor.gallery->getBlendronomer(Id);
        
        prompt.addTextEditor("name", prep->getName());
        
        prompt.addButton("Ok", 1, KeyPress(KeyPress::returnKey));
        prompt.addButton("Cancel", 2, KeyPress(KeyPress::escapeKey));
        
        int result = prompt.runModalLoop();
        
        String name = prompt.getTextEditorContents("name");
        
        DBG("name: " + String(name));
        
        if (result == 1)
        {
            processor.exportPreparation(PreparationTypeBlendronomer, Id, name);
        }
    }
    else if (action >= 100)
    {
        int which = action - 100;
        processor.importPreparation(PreparationTypeBlendronomer, processor.updateState->currentBlendronicId, which);
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
    else if (name == "Length Mode")
    {
        BlendronomerPreparation::Ptr prep = processor.gallery->getStaticBlendronomerPreparation(processor.updateState->currentBlendronicId);
        BlendronomerPreparation::Ptr active = processor.gallery->getActiveBlendronomerPreparation(processor.updateState->currentBlendronicId);
        
//        prep    ->setMode((BlendronicSyncMode) index);
//        active  ->setMode((BlendronicSyncMode) index);
//
//        if(currentTab == 0)
//        {
//            if(prep->getMode() == NoteLengthSync)
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
        
    }
}

void BlendronicPreparationEditor::BKSingleSliderValueChanged(BKSingleSlider* slider, String name, double val)
{
    BlendronomerPreparation::Ptr prep = processor.gallery->getStaticBlendronomerPreparation(processor.updateState->currentBlendronicId);
    BlendronomerPreparation::Ptr active = processor.gallery->getActiveBlendronomerPreparation(processor.updateState->currentBlendronicId);
    
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
//    else if(name == "gain")
//    {
//        //DBG("gain " + String(val));
//        prep->setGain(val);
//        active->setGain(val);
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

void BlendronicPreparationEditor::BKADSRSliderValueChanged(String name, int attack, int decay, float sustain, int release)
{
    DBG("received ADSR slider " + name);
    
    BlendronomerPreparation::Ptr prep = processor.gallery->getStaticBlendronomerPreparation(processor.updateState->currentBlendronicId);
    BlendronomerPreparation::Ptr active = processor.gallery->getActiveBlendronomerPreparation(processor.updateState->currentBlendronicId);
    
//    if(name == reverseADSRSlider->getName())
//    {
//        prep->setReverseAttack(attack);
//        prep->setReverseDecay(decay);
//        prep->setReverseSustain(sustain);
//        prep->setReverseRelease(release);
//        active->setReverseAttack(attack);
//        active->setReverseDecay(decay);
//        active->setReverseSustain(sustain);
//        active->setReverseRelease(release);
//    }
//    else if(name == undertowADSRSlider->getName())
//    {
//        prep->setUndertowAttack(attack);
//        prep->setUndertowDecay(decay);
//        prep->setUndertowSustain(sustain);
//        prep->setUndertowRelease(release);
//        active->setUndertowAttack(attack);
//        active->setUndertowDecay(decay);
//        active->setUndertowSustain(sustain);
//        active->setUndertowRelease(release);
//    }
    
}

void BlendronicPreparationEditor::closeSubWindow()
{
    reverseADSRSlider->setIsButtonOnly(true);
    undertowADSRSlider->setIsButtonOnly(true);
    setShowADSR(reverseADSRSlider->getName(), false);
    setShowADSR(undertowADSRSlider->getName(), false);
    setSubWindowInFront(false);
}


void BlendronicPreparationEditor::BKADSRButtonStateChanged(String name, bool mod, bool state)
{
    //DBG("BKADSRButtonStateChanged " + name);
    setShowADSR(name, !state);
    setSubWindowInFront(!state);
}

void BlendronicPreparationEditor::BKStackedSliderValueChanged(String name, Array<float> val)
{
    BlendronomerPreparation::Ptr prep = processor.gallery->getStaticBlendronomerPreparation(processor.updateState->currentBlendronicId);
    BlendronomerPreparation::Ptr active = processor.gallery->getActiveBlendronomerPreparation(processor.updateState->currentBlendronicId);
    
//    prep->setTransposition(val);
//    active->setTransposition(val);
}

void BlendronicPreparationEditor::fillSelectCB(int last, int current)
{
    selectCB.clear(dontSendNotification);
    
    for (auto prep : processor.gallery->getAllBlendronomer())
    {
        int Id = prep->getId();
        
        if (Id == -1) continue;
        
        String name = prep->getName();
        
        if (name != String())  selectCB.addItem(name, Id);
        else                        selectCB.addItem("Blendronic"+String(Id), Id);
        
        selectCB.setItemEnabled(Id, true);
        if (processor.currentPiano->isActive(PreparationTypeBlendronomer, Id))
            selectCB.setItemEnabled(Id, false);
    }
    
    if (last != 0)      selectCB.setItemEnabled(last, true);
    if (current != 0)   selectCB.setItemEnabled(current, false);
    
    int selectedId = processor.updateState->currentBlendronicId;
    
    selectCB.setSelectedId(selectedId, NotificationType::dontSendNotification);
    
    selectCB.setItemEnabled(selectedId, false);
    
    lastId = selectedId;
    
}

void BlendronicPreparationEditor::timerCallback()
{
    if (processor.updateState->currentDisplay == DisplayBlendronomer)
    {
        BlendronomerProcessor::Ptr bProcessor = processor.currentPiano->getBlendronomerProcessor(processor.updateState->currentBlendronicId);
        
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
            
            
            
            BlendronomerPreparation::Ptr active = processor.gallery->getActiveBlendronomerPreparation(processor.updateState->currentBlendronicId);
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


void BlendronicPreparationEditor::buttonClicked (Button* b)
{
    BlendronomerPreparation::Ptr prep = processor.gallery->getStaticBlendronomerPreparation(processor.updateState->currentBlendronicId);
    BlendronomerPreparation::Ptr active = processor.gallery->getActiveBlendronomerPreparation(processor.updateState->currentBlendronicId);
    
    if (b == &hideOrShow)
    {
        processor.updateState->setCurrentDisplay(DisplayNil);
        
        reverseADSRSlider->setIsButtonOnly(true);
        undertowADSRSlider->setIsButtonOnly(true);
        setShowADSR(reverseADSRSlider->getName(), false);
        setShowADSR(undertowADSRSlider->getName(), false);
        setSubWindowInFront(false);
    }
    else if (b == &actionButton)
    {
        getPrepOptionMenu(PreparationTypeBlendronomer).showMenuAsync (PopupMenu::Options().withTargetComponent (&actionButton), ModalCallbackFunction::forComponent (actionButtonCallback, this) );
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
    BlendronomerPreparation::Ptr prep = processor.gallery->getStaticBlendronomerPreparation(processor.updateState->currentBlendronicId);
    BlendronomerPreparation::Ptr active = processor.gallery->getActiveBlendronomerPreparation(processor.updateState->currentBlendronicId);
    
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
    lengthModeSelectCB.addListener(this);
    transpositionSlider->addMyListener(this);
    lengthMultiplierSlider->addMyListener(this);
    holdTimeMinMaxSlider->addMyListener(this);
    velocityMinMaxSlider->addMyListener(this);
    clusterMinSlider->addMyListener(this);
    clusterThresholdSlider->addMyListener(this);
    beatsToSkipSlider->addMyListener(this);
    
    keyOnResetToggle.addListener(this);
    
    gainSlider->addMyListener(this);
    
    reverseADSRSlider->addMyListener(this);
    undertowADSRSlider->addMyListener(this);
    
    //startTimer(20);
}

void BlendronicModificationEditor::greyOutAllComponents()
{
    lengthModeSelectCB.setAlpha(gModAlpha);
    transpositionSlider->setDim(gModAlpha);
    lengthMultiplierSlider->setDim(gModAlpha);
    beatsToSkipSlider->setDim(gModAlpha);
    gainSlider->setDim(gModAlpha);
    reverseADSRSlider->setDim(gModAlpha);
    undertowADSRSlider->setDim(gModAlpha);
    holdTimeMinMaxSlider->setDim(gModAlpha);
    velocityMinMaxSlider->setDim(gModAlpha);
    clusterMinSlider->setDim(gModAlpha);
    clusterThresholdSlider->setDim(gModAlpha);
    
    keyOnResetToggle.setAlpha(gModAlpha);
    keyOnResetLabel.setAlpha(gModAlpha);
    
    reverseADSRLabel.setAlpha(gModAlpha);
    undertowADSRLabel.setAlpha(gModAlpha);
}

void BlendronicModificationEditor::highlightModedComponents()
{
    BlendronomerModification::Ptr mod = processor.gallery->getBlendronomerModification(processor.updateState->currentModBlendronicId);
    
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
    
    
    BlendronomerModification::Ptr mod = processor.gallery->getBlendronomerModification(processor.updateState->currentModBlendronicId);
    
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
//        BlendronomerSyncMode mode = (BlendronicSyncMode) mod->getMode();
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
    
    for (auto prep : processor.gallery->getBlendronomerModifications())
    {
        int Id = prep->getId();;
        String name = prep->getName();
        
        if (name != String())  selectCB.addItem(name, Id);
        else                        selectCB.addItem("BlendronicMod"+String(Id), Id);
        
        selectCB.setItemEnabled(Id, true);
        if (processor.currentPiano->isActive(PreparationTypeBlendronomer, Id))
            selectCB.setItemEnabled(Id, false);
    }
    
    if (last != 0)      selectCB.setItemEnabled(last, true);
    if (current != 0)   selectCB.setItemEnabled(current, false);
    
    int selectedId = processor.updateState->currentBlendronicId;
    
    selectCB.setSelectedId(selectedId, NotificationType::dontSendNotification);
    
    selectCB.setItemEnabled(selectedId, false);
    
    lastId = selectedId;
}

void BlendronicModificationEditor::timerCallback()
{
    if (processor.updateState->currentDisplay == DisplayBlendronomerMod)
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

void BlendronicModificationEditor::BKEditableComboBoxChanged(String name, BKEditableComboBox* cb)
{
    BlendronomerModification::Ptr mod = processor.gallery->getBlendronomerModification(processor.updateState->currentModBlendronicId);
    
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
    processor.gallery->add(PreparationTypeBlendronomerMod);
    
    return processor.gallery->getBlendronomerModifications().getLast()->getId();
}

int BlendronicModificationEditor::duplicatePreparation(void)
{
    processor.gallery->duplicate(PreparationTypeBlendronomerMod, processor.updateState->currentModBlendronicId);
    
    return processor.gallery->getBlendronomerModifications().getLast()->getId();
}

void BlendronicModificationEditor::deleteCurrent(void)
{
    int oldId = selectCB.getSelectedId();
    int index = selectCB.getSelectedItemIndex();
    
    if ((index == 0) && (selectCB.getItemId(index+1) == -1)) return;
    
    processor.gallery->remove(PreparationTypeBlendronomerMod, oldId);
    
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
        processor.clear(PreparationTypeBlendronomerMod, processor.updateState->currentModBlendronicId);
        vc->update();
        vc->updateModification();
    }
    else if (action == 6)
    {
        AlertWindow prompt("", "", AlertWindow::AlertIconType::QuestionIcon);
        
        int Id = processor.updateState->currentModBlendronicId;
        BlendronomerModification::Ptr prep = processor.gallery->getBlendronomerModification(Id);
        
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
        BlendronomerModification::Ptr prep = processor.gallery->getBlendronomerModification(Id);
        
        prompt.addTextEditor("name", prep->getName());
        
        prompt.addButton("Ok", 1, KeyPress(KeyPress::returnKey));
        prompt.addButton("Cancel", 2, KeyPress(KeyPress::escapeKey));
        
        int result = prompt.runModalLoop();
        
        String name = prompt.getTextEditorContents("name");
        
        DBG("name: " + String(name));
        
        if (result == 1)
        {
            processor.exportPreparation(PreparationTypeBlendronomerMod, Id, name);
        }
    }
    else if (action >= 100)
    {
        int which = action - 100;
        processor.importPreparation(PreparationTypeBlendronomerMod, processor.updateState->currentModBlendronicId, which);
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
    else if (name == "Length Mode")
    {
        BlendronomerModification::Ptr mod = processor.gallery->getBlendronomerModification(processor.updateState->currentModBlendronicId);
        
//        BlendronomerSyncMode mode = (BlendronicSyncMode) index;
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
    BlendronomerModification::Ptr mod = processor.gallery->getBlendronomerModification(processor.updateState->currentModBlendronicId);
    
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

void BlendronicModificationEditor::BKStackedSliderValueChanged(String name, Array<float> val)
{
    BlendronomerModification::Ptr mod = processor.gallery->getBlendronomerModification(processor.updateState->currentModBlendronicId);
    
//    mod->setTransposition(val);
//    mod->setDirty(BlendronicTransposition);
//
//    transpositionSlider->setBright();
//
    updateModification();
}

void BlendronicModificationEditor::updateModification(void)
{
    processor.updateState->modificationDidChange = true;
}

void BlendronicModificationEditor::buttonClicked (Button* b)
{
    BlendronomerModification::Ptr mod = processor.gallery->getBlendronomerModification(processor.updateState->currentModBlendronicId);
    
    if (b == &hideOrShow)
    {
        processor.updateState->setCurrentDisplay(DisplayNil);
    }
    else if (b == &actionButton)
    {
        getModOptionMenu(PreparationTypeBlendronomerMod).showMenuAsync (PopupMenu::Options().withTargetComponent (&actionButton), ModalCallbackFunction::forComponent (actionButtonCallback, this) );
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

void BlendronicModificationEditor::BKADSRSliderValueChanged(String name, int attack, int decay, float sustain, int release)
{
    BlendronomerModification::Ptr mod = processor.gallery->getBlendronomerModification(processor.updateState->currentModBlendronicId);
    
    Array<float> newvals = {(float)attack, (float)decay, sustain, (float)release};
//    if(name == reverseADSRSlider->getName())
//    {
//        mod->setReverseADSRvals(newvals);
//        mod->setDirty(BlendronicReverseADSR);
//
//        reverseADSRSlider->setBright();
//    }
//    else if(name == undertowADSRSlider->getName())
//    {
//        mod->setUndertowADSRvals(newvals);
//        mod->setDirty(BlendronicUndertowADSR);
//
//        undertowADSRSlider->setBright();
//    }
    
    
    updateModification();
}

void BlendronicModificationEditor::BKADSRButtonStateChanged(String name, bool mod, bool state)
{
    setShowADSR(name, !state);
    setSubWindowInFront(!state);
}

void BlendronicModificationEditor::BKRangeSliderValueChanged(String name, double minval, double maxval)
{
    BlendronomerModification::Ptr mod = processor.gallery->getBlendronomerModification(processor.updateState->currentModBlendronicId);
    
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
