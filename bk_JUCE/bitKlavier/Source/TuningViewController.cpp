/*
  ==============================================================================

    TuningViewController.cpp
    Created: 10 Jun 2017 11:24:22am
    Author:  Daniel Trueman

  ==============================================================================
*/

#include "TuningViewController.h"

TuningViewController::TuningViewController(BKAudioProcessor& p, BKItemGraph* theGraph):
BKViewController(p, theGraph, 2),
showSprings(false)
#if JUCE_IOS
, absoluteKeyboard(true)
#endif
{
    setLookAndFeel(&buttonsAndMenusLAF);

    //setRepaintsOnMouseActivity(false);
    
    for (int i = 0; i < 128; i++)
    {
        Slider* s = new Slider("t" + String(i));
        
        s->setSliderStyle(Slider::SliderStyle::LinearBar);
        s->setRange(0.0, 1.0);
        addChildComponent(s);
        tetherSliders.add(s);
        
        Label* l = new Label(s->getName(), Utilities::getNoteString(i));
        l->setColour(juce::Label::ColourIds::textColourId, Colours::white);
        addChildComponent(l);
        tetherLabels.add(l);
    }
    
    for (int i = 0; i < 12; i++)
    {
        Slider* s = new Slider(intervalNames[i+1]);
        
        s->setSliderStyle(Slider::SliderStyle::LinearBar);
        s->setRange(0.0, 1.0);
        addChildComponent(s);
        springSliders.add(s);
        
        Label* l = new Label(s->getName(), s->getName());
        l->setColour(juce::Label::ColourIds::textColourId, Colours::white);
        addChildComponent(l);
        springLabels.add(l);
    
    }
    
    adaptiveSystemsCB.setName("Adaptive");
    adaptiveSystemsCB.setTooltip("Select an adaptive tuning system.");
    adaptiveSystemsCB.setEnabled(true);
    adaptiveSystemsCB.toFront(true);
    addAndMakeVisible(adaptiveSystemsCB);
    
    adaptiveSystemsCB.addItem("None", 1);
    adaptiveSystemsCB.addItem("Adaptive 1", 2);
    adaptiveSystemsCB.addItem("Adaptive Anchored 1", 3);
    adaptiveSystemsCB.addItem("Spring", 4);

    rateSlider = new BKSingleSlider("rate", 5., 400., 100., 1);
    rateSlider->setJustifyRight(false);
    rateSlider->displaySliderVisible(false);
    rateSlider->setToolTipString("rate that spring model runs at (Hz)");
    addChildComponent(rateSlider);
    
    dragSlider = new BKSingleSlider("drag", 0., 1., 0.5, 0.0001);
    dragSlider->setJustifyRight(false);
    dragSlider->displaySliderVisible(false);
    dragSlider->setToolTipString("frictional component in spring model");
    addChildComponent(dragSlider);
    
    tetherStiffnessSlider = new BKSingleSlider("anchor stiff", 0., 1., 0.5, 0.0001);
    tetherStiffnessSlider->setJustifyRight(false);
    tetherStiffnessSlider->displaySliderVisible(false);
    tetherStiffnessSlider->setToolTipString("overall stiffness of anchor sliders");
    addChildComponent(tetherStiffnessSlider);
    
    intervalStiffnessSlider = new BKSingleSlider("interval stiff", 0., 1., 0.5, 0.0001);
    intervalStiffnessSlider->setJustifyRight(false);
    intervalStiffnessSlider->displaySliderVisible(false);
    intervalStiffnessSlider->setToolTipString("overall stiffness of interval sliders");
    addChildComponent(intervalStiffnessSlider);
    
    iconImageComponent.setImage(ImageCache::getFromMemory(BinaryData::tuning_icon_png, BinaryData::tuning_icon_pngSize));
    iconImageComponent.setImagePlacement(RectanglePlacement(juce::RectanglePlacement::stretchToFit));
    iconImageComponent.setAlpha(0.095);
    //addAndMakeVisible(iconImageComponent); //this seems to be the source of the UI lagginess, alas...
    
    selectCB.setName("Tuning");
    selectCB.addSeparator();
    selectCB.addListener(this);
    selectCB.setSelectedItemIndex(0);
    selectCB.setTooltip("Select from available saved preparation settings");
    addAndMakeVisible(selectCB);
    
    scaleCB.setName("Scale");
    scaleCB.setTooltip("Select from a range of preset temperaments");
    addAndMakeVisible(scaleCB);
    
    springScaleCB.setName("SpringTuning Scale");
    springScaleCB.setTooltip("Select a temperament for the spring intervals.");
    addChildComponent(springScaleCB);
    
    springScaleFundamentalCB.setName("SpringTuning Fundamental");
    springScaleFundamentalCB.setTooltip("Set the fundamental for the interval scale");
    addChildComponent(springScaleFundamentalCB);
    
    scaleLabel.setText("Scale", dontSendNotification);
    
    fundamentalCB.setName("Fundamental");
    fundamentalCB.setTooltip("Select root around which your temperament will be generated");
    addAndMakeVisible(fundamentalCB);
    
    fundamentalLabel.setText("Fundamental", dontSendNotification);
    
    A1IntervalScaleCB.setName("A1IntervalScale");
    A1IntervalScaleCB.addListener(this);
    A1IntervalScaleCB.setTooltip("scale the sets how successive intervals are tuned");
    addAndMakeVisible(A1IntervalScaleCB);
    
    A1IntervalScaleLabel.setText("Adaptive:", dontSendNotification);
    
    A1Inversional.setButtonText ("invert");
    A1Inversional.setToggleState (true, dontSendNotification);
    A1Inversional.setColour(ToggleButton::textColourId, Colours::antiquewhite);
    A1Inversional.setColour(ToggleButton::tickColourId, Colours::antiquewhite);
    A1Inversional.setColour(ToggleButton::tickDisabledColourId, Colours::antiquewhite);
    A1Inversional.setTooltip("when selected, intervals will be tuned the same whether they ascend or descend; e.g. C-D will always be the same interval as C-Bb");
    addAndMakeVisible(A1Inversional);
    
    A1AnchorScaleCB.setName("A1AnchorScale");
    A1AnchorScaleCB.setTooltip("determines where the moving fundamental will be tuned to");
    addAndMakeVisible(A1AnchorScaleCB);
    
    A1AnchorScaleLabel.setText("Anchor:", dontSendNotification);
    addAndMakeVisible(A1AnchorScaleLabel);
    
    A1FundamentalCB.setName("A1Fundamental");
    A1FundamentalCB.setTooltip("sets fundamental for the anchor scale");
    addAndMakeVisible(A1FundamentalCB);
    
    A1ClusterThresh = new BKSingleSlider("Threshold", 1, 1000, 0, 1);
    A1ClusterThresh->setJustifyRight(false);
    A1ClusterThresh->setToolTipString("if this time (ms) is exceeded, the fundamental will reset");
    addAndMakeVisible(A1ClusterThresh);
    
    A1ClusterMax = new BKSingleSlider("Maximum", 1, 8, 1, 1);
    A1ClusterMax->setJustifyRight(false);
    A1ClusterMax->setToolTipString("after these many notes are played, the fundamental will reset");
    addAndMakeVisible(A1ClusterMax);
    
    A1reset.setButtonText("reset");
    addAndMakeVisible(A1reset);
    
    showSpringsButton.setButtonText("Spiral");
    showSpringsButton.setClickingTogglesState(true);
    showSpringsButton.setTooltip("show spiral view of tuning relationships");
    showSpringsButton.setColour(TextButton::buttonOnColourId, Colours::red.withMultipliedAlpha(0.5));
    addAndMakeVisible(showSpringsButton);
    
    nToneRootCB.setName("nToneRoot");
    nToneRootCB.setTooltip("set root note, when semitone width is not 100");
    addAndMakeVisible(nToneRootCB);
    
    nToneRootOctaveCB.setName("nToneRootOctave");
    nToneRootOctaveCB.setTooltip("set octave for root note, when semitone width is not 100");
    addAndMakeVisible(nToneRootOctaveCB);
    
    nToneSemitoneWidthSlider = new BKSingleSlider("semitone width and root", 1, 200, 100, 0.001);
    nToneSemitoneWidthSlider->setJustifyRight(false);
    nToneSemitoneWidthSlider->displaySliderVisible(false);
    nToneSemitoneWidthSlider->setToolTipString("Adjusts half step distance. For example, 50 cents is a quarter-tone keyboard, and -100 cents is an inverted keyboard");
    addAndMakeVisible(nToneSemitoneWidthSlider);
    
    fillTuningCB();
    fillFundamentalCB();
    
    // Absolute Tuning Keyboard
    absoluteOffsets.ensureStorageAllocated(128);
    for(int i=0; i<128; i++) absoluteOffsets.add(0.);
    absoluteKeyboard.setName("absolute");
    absoluteKeyboard.setAlpha(1);
    addAndMakeVisible(absoluteKeyboard);
    
    //Custom Tuning Keyboard
    customOffsets.ensureStorageAllocated(12);
    for(int i=0; i<12; i++) customOffsets.add(0.);

    customKeyboard.setName("offset from ET");
    customKeyboard.setDimensionRatio(2.0);
    customKeyboard.setFundamental(0);
    addAndMakeVisible(customKeyboard);
    
    offsetSlider = new BKSingleSlider("offset: ", -100, 100, 0, 0.1);
    offsetSlider->displaySliderVisible(false);
    offsetSlider->setToolTipString("Raise or lower the entire temperament in cents");
    addAndMakeVisible(offsetSlider);
    
    lastNote.setText("note: ", dontSendNotification);
    lastNote.setTooltip("last note played as MIDI value");
    lastInterval.setText("interval: ", dontSendNotification);
    lastInterval.setJustificationType(juce::Justification::centredRight);
    lastInterval.setTooltip("Distance between last two notes played");
    addAndMakeVisible(lastNote);
    addAndMakeVisible(lastInterval);
    
    currentFundamental.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(currentFundamental);
    
    actionButton.setButtonText("Action");
    actionButton.setTooltip("Create, duplicate, rename, delete, or reset current settings");
    actionButton.addListener(this);
    addAndMakeVisible(actionButton);
    
#if JUCE_IOS
    offsetSlider->addWantsBigOneListener(this);
    A1ClusterMax->addWantsBigOneListener(this);
    A1ClusterThresh->addWantsBigOneListener(this);
    
    absoluteKeyboard.addWantsBigOneListener(this);
    customKeyboard.addWantsBigOneListener(this);
    
    nToneSemitoneWidthSlider->addWantsBigOneListener(this);
#endif
    
    currentTab = 0;
    displayTab(currentTab);
    
    updateComponentVisibility();
    
    startTimerHz(30);
}

void TuningViewController::invisible(void)
{
    currentFundamental.setVisible(false);
    absoluteKeyboard.setVisible(false);
    customKeyboard.setVisible(false);
    nToneRootCB.setVisible(false);
    nToneRootOctaveCB.setVisible(false);
    nToneSemitoneWidthSlider->setVisible(false);
    
    //adaptiveSystemsCB.setVisible(false);
    offsetSlider->setVisible(false);
    lastNote.setVisible(false);
    lastInterval.setVisible(false);
    A1Inversional.setVisible(false);
    A1IntervalScaleCB.setVisible(false);
    A1reset.setVisible(false);
    A1ClusterMax->setVisible(false);
    nToneSemitoneWidthSlider->setVisible(false);
    A1ClusterThresh->setVisible(false);
    nToneRootCB.setVisible(false);
    nToneRootOctaveCB.setVisible(false);
    A1AnchorScaleLabel.setVisible(false);
    A1AnchorScaleCB.setVisible(false);
    A1FundamentalCB.setVisible(false);
    
    for (auto s : tetherSliders)    s->setVisible(false);
    for (auto s : springSliders)    s->setVisible(false);
    for (auto l : springLabels)     l->setVisible(false);
    
    rateSlider->setVisible(false);
    dragSlider->setVisible(false);
    springScaleCB.setVisible(false);
    springScaleFundamentalCB.setVisible(false);
    
    tetherStiffnessSlider->setVisible(false);
    intervalStiffnessSlider->setVisible(false);
}

void TuningViewController::displayShared(void)
{
    Rectangle<int> area (getBounds());
    
    //iconImageComponent.setBounds(area);
    
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
    
    leftArrow.setBounds (0, getHeight() * 0.4, 50, 50);
    rightArrow.setBounds (getRight() - 50, getHeight() * 0.4, 50, 50);
    
}

void TuningViewController::displayTab(int tab)
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
        //iconImageComponent.setVisible(true);
        //iconImageComponent.toBack();
        
        adaptiveSystemsCB.setVisible(true);
        scaleCB.setVisible(true);
        fundamentalCB.setVisible(true);
        customKeyboard.setVisible(true);
        absoluteKeyboard.setVisible(true);
        offsetSlider->setVisible(true);
        currentFundamental.setVisible(true);
        lastNote.setVisible(true);
        lastInterval.setVisible(true);
        A1Inversional.setVisible(true);
        A1IntervalScaleCB.setVisible(true);
        A1reset.setVisible(true);
        A1ClusterMax->setVisible(true);
        nToneSemitoneWidthSlider->setVisible(true);
        A1ClusterThresh->setVisible(true);
        nToneRootCB.setVisible(true);
        nToneRootOctaveCB.setVisible(true);
        A1AnchorScaleLabel.setVisible(true);
        A1AnchorScaleCB.setVisible(true);
        A1FundamentalCB.setVisible(true);
        
        Rectangle<int> area (getBounds());
        area.reduce(10 * processor.paddingScalarX + 4, 10 * processor.paddingScalarY + 4);
        
        float keyboardHeight = 100 + 50 * processor.paddingScalarY;
        Rectangle<int> absoluteKeymapRow = area.removeFromBottom(keyboardHeight);
        absoluteKeymapRow.reduce(gXSpacing, 0);
        
        absoluteKeyboard.setBounds(absoluteKeymapRow);
        
        Rectangle<float> editAllBounds = absoluteKeyboard.getEditAllBounds();
        editAllBounds.translate(absoluteKeyboard.getX(), absoluteKeyboard.getY());
        lastNote.setBounds(editAllBounds.getRight() + gXSpacing, editAllBounds.getY(),editAllBounds.getWidth() * 2, editAllBounds.getHeight());
        lastInterval.setBounds(lastNote.getRight() + gXSpacing, lastNote.getY(),lastNote.getWidth(), lastNote.getHeight());
        
        Rectangle<int> leftColumn = area.removeFromLeft(area.getWidth() * 0.5);
        leftColumn.removeFromLeft(leftArrow.getWidth());
        
        Rectangle<int> modeSlice = area.removeFromTop(gComponentComboBoxHeight);
        modeSlice.removeFromLeft(gXSpacing);
        modeSlice.removeFromRight(gXSpacing);
        //adaptiveSystemsCB.setBounds(modeSlice.removeFromLeft(showSpringsButton.getWidth()));
        adaptiveSystemsCB.setBounds(modeSlice.removeFromLeft(modeSlice.getWidth() / 3.));
        modeSlice.removeFromLeft(gXSpacing);
        //scaleCB.setBounds(modeSlice.removeFromLeft(modeSlice.getWidth() / 2.));
        scaleCB.setBounds(modeSlice.removeFromLeft(2. * modeSlice.getWidth() / 3. - 2.*gXSpacing));
        
        modeSlice.removeFromLeft(gXSpacing);
        fundamentalCB.setBounds(modeSlice);
        
        int customKeyboardHeight = 80 + 70. * processor.paddingScalarY;
        int extraY = (area.getHeight() - (customKeyboardHeight + gComponentSingleSliderHeight + gYSpacing * 3)) * 0.25;
        
        area.removeFromTop(extraY);
        area.removeFromRight(rightArrow.getWidth());
        Rectangle<int> customKeyboardSlice = area.removeFromTop(customKeyboardHeight);
        customKeyboardSlice.removeFromLeft(gXSpacing + 2.*gPaddingConst * processor.paddingScalarX);
        customKeyboardSlice.removeFromRight(gXSpacing);
        //DBG("customKeyboardSlice width = " + String(customKeyboardSlice.getWidth()));
        if(customKeyboardSlice.getWidth() > 0) customKeyboard.setBounds(customKeyboardSlice); //shouldn't need this check, not sure why it's happening
        
        area.removeFromTop(extraY);
        Rectangle<int> offsetSliderSlice = area.removeFromTop(gComponentSingleSliderHeight);
        offsetSliderSlice.removeFromLeft(gXSpacing + 2.*gPaddingConst * processor.paddingScalarX - gComponentSingleSliderXOffset);
        offsetSliderSlice.removeFromRight(gXSpacing - gComponentSingleSliderXOffset);
        offsetSlider->setBounds(offsetSliderSlice);
        
        area.removeFromTop(extraY);
        Rectangle<int> currentFundamentalSlice = area.removeFromTop(gComponentTextFieldHeight);
        currentFundamental.setBounds(currentFundamentalSlice);
        
        extraY = (leftColumn.getHeight() -
                  (gComponentComboBoxHeight * 2 +
                   gComponentSingleSliderHeight * 2 +
                   gYSpacing * 5)) * 0.25;
        
        leftColumn.removeFromTop(extraY + gYSpacing);
        Rectangle<int> A1IntervalScaleCBSlice = leftColumn.removeFromTop(gComponentComboBoxHeight);
        A1IntervalScaleCBSlice.removeFromRight(gXSpacing + 2.*gPaddingConst * processor.paddingScalarX);
        A1IntervalScaleCBSlice.removeFromLeft(gXSpacing);
        int tempwidth = A1IntervalScaleCBSlice.getWidth() / 3.;
        A1Inversional.setBounds(A1IntervalScaleCBSlice.removeFromRight(tempwidth));
        A1IntervalScaleCB.setBounds(A1IntervalScaleCBSlice.removeFromRight(tempwidth));
        A1IntervalScaleCBSlice.removeFromRight(gXSpacing);
        A1reset.setBounds(A1IntervalScaleCBSlice);
        
        leftColumn.removeFromTop(extraY + gYSpacing);
        Rectangle<int> A1ClusterMaxSlice = leftColumn.removeFromTop(gComponentSingleSliderHeight);
        A1ClusterMaxSlice.removeFromRight(gXSpacing + 2.*gPaddingConst * processor.paddingScalarX - gComponentSingleSliderXOffset);
        A1ClusterMax->setBounds(A1ClusterMaxSlice);
        nToneSemitoneWidthSlider->setBounds(A1ClusterMaxSlice);
        
        leftColumn.removeFromTop(gYSpacing);
        Rectangle<int> A1ClusterThreshSlice = leftColumn.removeFromTop(gComponentSingleSliderHeight);
        A1ClusterThreshSlice.removeFromRight(gXSpacing + 2.*gPaddingConst * processor.paddingScalarX - gComponentSingleSliderXOffset);
        A1ClusterThresh->setBounds(A1ClusterThreshSlice);
        
        Rectangle<int> nToneRootCBSlice = A1ClusterThreshSlice.removeFromLeft(tempwidth);
        nToneRootCBSlice = nToneRootCBSlice.removeFromTop(gComponentComboBoxHeight);
        nToneRootCBSlice.removeFromLeft(gXSpacing * 2);
        nToneRootCB.setBounds(nToneRootCBSlice);
        
        Rectangle<int> nToneRootOctaveCBSlice = A1ClusterThreshSlice.removeFromLeft(tempwidth);
        nToneRootOctaveCBSlice = nToneRootOctaveCBSlice.removeFromTop(gComponentComboBoxHeight);
        nToneRootOctaveCBSlice.removeFromLeft(gXSpacing * 2);
        nToneRootOctaveCB.setBounds(nToneRootOctaveCBSlice);
        
        leftColumn.removeFromTop(extraY + gYSpacing);
        Rectangle<int> A1AnchorScaleCBSlice = leftColumn.removeFromTop(gComponentComboBoxHeight);
        A1AnchorScaleCBSlice.removeFromRight(gXSpacing + 2.*gPaddingConst * processor.paddingScalarX);
        A1AnchorScaleCBSlice.removeFromLeft(gXSpacing);
        tempwidth = A1AnchorScaleCBSlice.getWidth() / 3.;
        A1AnchorScaleLabel.setBounds(A1AnchorScaleCBSlice.removeFromLeft(tempwidth));
        A1AnchorScaleCB.setBounds(A1AnchorScaleCBSlice.removeFromLeft(tempwidth));
        A1AnchorScaleCBSlice.removeFromLeft(gXSpacing);
        A1FundamentalCB.setBounds(A1AnchorScaleCBSlice);
        
        updateComponentVisibility();

    }
    else if (tab == 1)
    {
        // SET VISIBILITY
        if(showSprings)
        {
            for (auto s : tetherSliders)    s->setVisible(true);
            for (auto s : springSliders)    s->setVisible(true);
            for (auto l : springLabels)     l->setVisible(true);
            
            rateSlider->setVisible(true);
            dragSlider->setVisible(true);
            springScaleCB.setVisible(true);
            springScaleFundamentalCB.setVisible(true);
            
            tetherStiffnessSlider->setVisible(true);
            intervalStiffnessSlider->setVisible(true);
        }

        iconImageComponent.setVisible(false);

        Rectangle<int> area (getBounds());
        //area.reduce(10 * processor.paddingScalarX + 4, 10 * processor.paddingScalarY + 4);
        area.removeFromTop(selectCB.getHeight() + 50 * processor.paddingScalarY + 4 + gYSpacing);
        area.removeFromRight(rightArrow.getWidth());
        area.removeFromLeft(leftArrow.getWidth());
        
        Rectangle<int> leftColumn (area.removeFromLeft(area.getWidth()* 0.5));
        leftColumn.removeFromLeft(leftArrow.getWidth());
        
        leftColumn.removeFromRight(processor.paddingScalarX * 20);
        leftColumn.removeFromLeft(processor.paddingScalarX * 20);
        
        area.removeFromLeft(processor.paddingScalarX * 20); //area is now right column
        area.removeFromRight(processor.paddingScalarX * 20);
        
        int columnHeight = leftColumn.getHeight();
        
        Rectangle<float> editAllBounds = absoluteKeyboard.getEditAllBounds();
        editAllBounds.translate(absoluteKeyboard.getX(), absoluteKeyboard.getY());
        lastNote.setBounds(editAllBounds.getRight() + gXSpacing, editAllBounds.getY(),editAllBounds.getWidth() * 2, editAllBounds.getHeight());
        lastInterval.setBounds(lastNote.getRight() + gXSpacing, lastNote.getY(),lastNote.getWidth(), lastNote.getHeight());
        
        rateSlider->setBounds(selectCB.getX()-gComponentSingleSliderXOffset, selectCB.getBottom() + gYSpacing, selectCB.getWidth()+gComponentSingleSliderXOffset, gComponentSingleSliderHeight);
        dragSlider->setBounds(actionButton.getX()-gComponentSingleSliderXOffset, rateSlider->getY(), showSpringsButton.getWidth() + actionButton.getWidth(), gComponentSingleSliderHeight);
        
        springScaleCB.setBounds(scaleCB.getX(), rateSlider->getY(), scaleCB.getWidth(), gComponentComboBoxHeight);
        springScaleFundamentalCB.setBounds(fundamentalCB.getX(), springScaleCB.getY(), fundamentalCB.getWidth(), gComponentComboBoxHeight);
        
        intervalStiffnessSlider->setBounds(selectCB.getX() - gComponentSingleSliderXOffset,
                                           rateSlider->getBottom() + gYSpacing,
                                           rateSlider->getWidth(),
                                           gComponentSingleSliderHeight);
        
        tetherStiffnessSlider->setBounds(//fundamentalCB.getX() - gComponentSingleSliderXOffset,
                                         fundamentalCB.getRight() - intervalStiffnessSlider->getWidth(),
                                         intervalStiffnessSlider->getY(),
                                         //fundamentalCB.getWidth() + 2.*gComponentSingleSliderXOffset,
                                         intervalStiffnessSlider->getWidth(),
                                         gComponentSingleSliderHeight);
        
        float sliderHeight = (absoluteKeyboard.getBottom() - (rateSlider->getBottom() + gYSpacing)) / 13.;
        
        for (int i = 0; i < 12; i++)
        {
            springLabels[i]->setBounds(selectCB.getX(),
                                       intervalStiffnessSlider->getBottom() + (sliderHeight) * (11 - i),
                                       hideOrShow.getWidth(),
                                       sliderHeight);
            springSliders[i]->setBounds(springLabels[i]->getRight() + 2,
                                        springLabels[i]->getY(),
                                        intervalStiffnessSlider->getWidth() * 0.7,
                                        sliderHeight);
        }
    
    }
}


void TuningViewController::resized()
{
    
    displayShared();
    displayTab(currentTab);
    
#if 0
    Rectangle<int> area (getLocalBounds());
    
    iconImageComponent.setBounds(area);
    area.reduce(10 * processor.paddingScalarX+3, 10 * processor.paddingScalarY+3);
    
    float keyboardHeight = 100 + 50 * processor.paddingScalarY;
    Rectangle<int> absoluteKeymapRow = area.removeFromBottom(keyboardHeight);
    absoluteKeymapRow.reduce(gXSpacing, 0);
    
    absoluteKeyboard.setBounds(absoluteKeymapRow);
    
    Rectangle<int> leftColumn = area.removeFromLeft(area.getWidth() * 0.5);
    Rectangle<int> comboBoxSlice = leftColumn.removeFromTop(gComponentComboBoxHeight);
    comboBoxSlice.removeFromRight(gXSpacing);
    hideOrShow.setBounds(comboBoxSlice.removeFromLeft(gComponentComboBoxHeight));
    comboBoxSlice.removeFromLeft(gXSpacing);
    selectCB.setBounds(comboBoxSlice.removeFromLeft(comboBoxSlice.getWidth() / 2.));
    comboBoxSlice.removeFromLeft(gXSpacing);
    actionButton.setBounds(comboBoxSlice.removeFromLeft(comboBoxSlice.getWidth() / 2.));
    comboBoxSlice.removeFromLeft(gXSpacing);
    showSpringsButton.setBounds(comboBoxSlice);

    /* *** above here should be generic (mostly) to all prep layouts *** */
    /* ***         below here will be specific to each prep          *** */
    
    // ********* right column
    
    Rectangle<int> modeSlice = area.removeFromTop(gComponentComboBoxHeight);
    modeSlice.removeFromLeft(gXSpacing);
    modeSlice.removeFromRight(gXSpacing);
    //adaptiveSystemsCB.setBounds(modeSlice.removeFromLeft(showSpringsButton.getWidth()));
    adaptiveSystemsCB.setBounds(modeSlice.removeFromLeft(modeSlice.getWidth() / 3.));
    modeSlice.removeFromLeft(gXSpacing);
    //scaleCB.setBounds(modeSlice.removeFromLeft(modeSlice.getWidth() / 2.));
    scaleCB.setBounds(modeSlice.removeFromLeft(2. * modeSlice.getWidth() / 3. - 2.*gXSpacing));
    
    modeSlice.removeFromLeft(gXSpacing);
    fundamentalCB.setBounds(modeSlice);
    
    int customKeyboardHeight = 80 + 70. * processor.paddingScalarY;
    int extraY = (area.getHeight() - (customKeyboardHeight + gComponentSingleSliderHeight + gYSpacing * 3)) * 0.25;
    
    area.removeFromTop(extraY);
    Rectangle<int> customKeyboardSlice = area.removeFromTop(customKeyboardHeight);
    customKeyboardSlice.removeFromLeft(gXSpacing + 2.*gPaddingConst * processor.paddingScalarX);
    customKeyboardSlice.removeFromRight(gXSpacing);
    customKeyboard.setBounds(customKeyboardSlice);
    
    area.removeFromTop(extraY);
    Rectangle<int> offsetSliderSlice = area.removeFromTop(gComponentSingleSliderHeight);
    offsetSliderSlice.removeFromLeft(gXSpacing + 2.*gPaddingConst * processor.paddingScalarX - gComponentSingleSliderXOffset);
    offsetSliderSlice.removeFromRight(gXSpacing - gComponentSingleSliderXOffset);
    offsetSlider->setBounds(offsetSliderSlice);
    
    area.removeFromTop(extraY);
    Rectangle<int> currentFundamentalSlice = area.removeFromTop(gComponentTextFieldHeight);
    currentFundamental.setBounds(currentFundamentalSlice);
    
    // ********* left column
    
    extraY = (leftColumn.getHeight() -
              (gComponentComboBoxHeight * 2 +
               gComponentSingleSliderHeight * 2 +
               gYSpacing * 5)) * 0.25;
    
    leftColumn.removeFromTop(extraY + gYSpacing);
    Rectangle<int> A1IntervalScaleCBSlice = leftColumn.removeFromTop(gComponentComboBoxHeight);
    A1IntervalScaleCBSlice.removeFromRight(gXSpacing + 2.*gPaddingConst * processor.paddingScalarX);
    A1IntervalScaleCBSlice.removeFromLeft(gXSpacing);
    int tempwidth = A1IntervalScaleCBSlice.getWidth() / 3.;
    A1Inversional.setBounds(A1IntervalScaleCBSlice.removeFromRight(tempwidth));
    A1IntervalScaleCB.setBounds(A1IntervalScaleCBSlice.removeFromRight(tempwidth));
    A1IntervalScaleCBSlice.removeFromRight(gXSpacing);
    A1reset.setBounds(A1IntervalScaleCBSlice);
    
    leftColumn.removeFromTop(extraY + gYSpacing);
    Rectangle<int> A1ClusterMaxSlice = leftColumn.removeFromTop(gComponentSingleSliderHeight);
    A1ClusterMaxSlice.removeFromRight(gXSpacing + 2.*gPaddingConst * processor.paddingScalarX - gComponentSingleSliderXOffset);
    A1ClusterMax->setBounds(A1ClusterMaxSlice);
    nToneSemitoneWidthSlider->setBounds(A1ClusterMaxSlice);
    
    leftColumn.removeFromTop(gYSpacing);
    Rectangle<int> A1ClusterThreshSlice = leftColumn.removeFromTop(gComponentSingleSliderHeight);
    A1ClusterThreshSlice.removeFromRight(gXSpacing + 2.*gPaddingConst * processor.paddingScalarX - gComponentSingleSliderXOffset);
    A1ClusterThresh->setBounds(A1ClusterThreshSlice);
    
    Rectangle<int> nToneRootCBSlice = A1ClusterThreshSlice.removeFromLeft(tempwidth);
    nToneRootCBSlice = nToneRootCBSlice.removeFromTop(gComponentComboBoxHeight);
    nToneRootCBSlice.removeFromLeft(gXSpacing * 2);
    nToneRootCB.setBounds(nToneRootCBSlice);
    
    Rectangle<int> nToneRootOctaveCBSlice = A1ClusterThreshSlice.removeFromLeft(tempwidth);
    nToneRootOctaveCBSlice = nToneRootOctaveCBSlice.removeFromTop(gComponentComboBoxHeight);
    nToneRootOctaveCBSlice.removeFromLeft(gXSpacing * 2);
    nToneRootOctaveCB.setBounds(nToneRootOctaveCBSlice);
    
    leftColumn.removeFromTop(extraY + gYSpacing);
    Rectangle<int> A1AnchorScaleCBSlice = leftColumn.removeFromTop(gComponentComboBoxHeight);
    A1AnchorScaleCBSlice.removeFromRight(gXSpacing + 2.*gPaddingConst * processor.paddingScalarX);
    A1AnchorScaleCBSlice.removeFromLeft(gXSpacing);
    tempwidth = A1AnchorScaleCBSlice.getWidth() / 3.;
    A1AnchorScaleLabel.setBounds(A1AnchorScaleCBSlice.removeFromLeft(tempwidth));
    A1AnchorScaleCB.setBounds(A1AnchorScaleCBSlice.removeFromLeft(tempwidth));
    A1AnchorScaleCBSlice.removeFromLeft(gXSpacing);
    A1FundamentalCB.setBounds(A1AnchorScaleCBSlice);
    
    Rectangle<float> editAllBounds = absoluteKeyboard.getEditAllBounds();
    editAllBounds.translate(absoluteKeyboard.getX(), absoluteKeyboard.getY());
    lastNote.setBounds(editAllBounds.getRight() + gXSpacing, editAllBounds.getY(),editAllBounds.getWidth() * 2, editAllBounds.getHeight());
    lastInterval.setBounds(lastNote.getRight() + gXSpacing, lastNote.getY(),lastNote.getWidth(), lastNote.getHeight());
    
    rateSlider->setBounds(selectCB.getX()-gComponentSingleSliderXOffset, selectCB.getBottom() + gYSpacing, selectCB.getWidth()+gComponentSingleSliderXOffset, gComponentSingleSliderHeight);
    dragSlider->setBounds(actionButton.getX()-gComponentSingleSliderXOffset, rateSlider->getY(), showSpringsButton.getWidth() + actionButton.getWidth(), gComponentSingleSliderHeight);
    
    springScaleCB.setBounds(scaleCB.getX(), rateSlider->getY(), scaleCB.getWidth(), gComponentComboBoxHeight);
    springScaleFundamentalCB.setBounds(fundamentalCB.getX(), springScaleCB.getY(), fundamentalCB.getWidth(), gComponentComboBoxHeight);
    
    intervalStiffnessSlider->setBounds(selectCB.getX() - gComponentSingleSliderXOffset,
                                       rateSlider->getBottom() + gYSpacing,
                                       rateSlider->getWidth(),
                                       gComponentSingleSliderHeight);
    
    tetherStiffnessSlider->setBounds(//fundamentalCB.getX() - gComponentSingleSliderXOffset,
                                     fundamentalCB.getRight() - intervalStiffnessSlider->getWidth(),
                                     intervalStiffnessSlider->getY(),
                                     //fundamentalCB.getWidth() + 2.*gComponentSingleSliderXOffset,
                                     intervalStiffnessSlider->getWidth(),
                                     gComponentSingleSliderHeight);
    
    //dragSlider->setBounds(fundamentalCB.getX()-gComponentSingleSliderXOffset, intervalStiffnessSlider->getY(), fundamentalCB.getWidth()+gComponentSingleSliderXOffset*2., gComponentSingleSliderHeight);
    
    
    float sliderHeight = (absoluteKeyboard.getBottom() - (rateSlider->getBottom() + gYSpacing)) / 13.;
    
    for (int i = 0; i < 12; i++)
    {
        springLabels[i]->setBounds(hideOrShow.getX(),
                                   intervalStiffnessSlider->getBottom() + (sliderHeight) * (11 - i),
                                   hideOrShow.getWidth(),
                                   sliderHeight);
        springSliders[i]->setBounds(selectCB.getX(),
                                    springLabels[i]->getY(),
                                    intervalStiffnessSlider->getWidth() * 0.75,
                                    sliderHeight);
    }
    
    //updateComponentVisibility();
#endif
}

void TuningViewController::paint (Graphics& g)
{
    
    g.fillAll(Colours::black);
    
    //if (!showSprings || currentTab != 1) return;
    if (currentTab != 1) return;
    
    TuningProcessor::Ptr tuning;
    TuningPreparation::Ptr active;
    TuningModification::Ptr mod;
    
    if (processor.updateState->currentDisplay == DisplayTuningMod)
    {
        mod = processor.gallery->getTuningModification(processor.updateState->currentModTuningId);

        Array<int> targets = mod->getTargets();
        
        int tuningId;
        if (targets.size())   tuningId = targets[0];
        else                  tuningId = -1;
        
        tuning = processor.currentPiano->getTuningProcessor(tuningId);
        active = processor.gallery->getActiveTuningPreparation(tuningId);
    }
    else
    {
        tuning = processor.currentPiano->getTuningProcessor(processor.updateState->currentTuningId);
        active = processor.gallery->getActiveTuningPreparation(processor.updateState->currentTuningId);
    }
    
    bool springsOn = (active->getAdaptiveType() == AdaptiveSpring);
    
    Rectangle<int> b = getLocalBounds();
    b.removeFromTop(selectCB.getBottom());
    
    float midi,scalex,posx,radians,cx,cy;
    float centerx = b.getWidth() * 0.5f;
    float centery = b.getCentreY();
    
    float radius_scale = 0.25;
    float radius = jmin(b.getHeight() * radius_scale, b.getWidth() * radius_scale);
    float dimc_scale = 0.05;
    float dimc = jmin(b.getHeight() * dimc_scale, b.getWidth() * dimc_scale);
    int x_offset = 0.075 * b.getWidth();
    
    float midiScale;
    
    Particle::PtrArr particles = active->getTetherParticles();

    for (auto s : active->getEnabledSprings())
    {
        if (s != nullptr && s->getEnabled())
        {
            Particle* a = s->getA();
            if(springsOn) midi = Utilities::ftom(Utilities::centsToFreq(a->getX() - (1200.0 * a->getOctave())));
            else
            {
                midi = tuning->getOffset(a->getNote(), false);
                midi += a->getNote();
            }
            
            scalex = ((midi - 60.0f) / 12.0f);
            
            float midiSave = midi;
            
            midiScale = Utilities::clip(0, Utilities::ftom(Utilities::centsToFreq(a->getX() - (1200.0 * a->getOctave()))), 128);
            midiScale += ((a->getOctave() - 5) * 12.0);
            midiScale /= 60.;
            
            radians = scalex * Utilities::twopi - Utilities::pi * 0.5;
            
            float cxa = centerx + cosf(radians) * radius * midiScale;
            float cya = centery + sinf(radians) * radius * midiScale;
            
            Particle* b = s->getB();
            if(springsOn) midi = Utilities::ftom(Utilities::centsToFreq(b->getX() - (1200.0 * b->getOctave())));
            else {
                midi = tuning->getOffset(b->getNote(), false);
                midi += b->getNote();
            }
            
            scalex = ((midi - 60.0f) / 12.0f);
            
            midiScale = Utilities::clip(0, Utilities::ftom(Utilities::centsToFreq(b->getX() - (1200.0 * b->getOctave()))), 128);
            midiScale += ((b->getOctave() - 5) * 12.0);
            midiScale /= 60.;
            
            radians = scalex * Utilities::twopi - Utilities::pi * 0.5;
            
            float cxb = centerx + cosf(radians) * radius * midiScale;
            float cyb = centery + sinf(radians) * radius * midiScale;
            
            double strength = s->getStrength();
            
            float hue = fmod((midi + midiSave)/2., 12.) / 12.;
            Colour colour (hue, 0.5f, 0.5f, 0.25f);
            
            g.setColour(colour);
            g.drawLine(cxa, cya, cxb, cyb,  (strength > 0.0) ? strength * 5.0 + 1.0 : 0.0);
            
            int h = 10, w = 35;
            
            int midX = (cxa + cxb) / 2.0; //+ xoff;
            int midY = (cya + cyb) / 2.0; //+ yoff;
        
            g.setColour(Colours::ghostwhite);
            g.setFont(12.0f);
            g.setOpacity(0.7);
            if(springsOn)
            {
                g.drawText(String((int)round(s->getLength())), midX-dimc*0.25, midY, w, h, Justification::topLeft);
            }
            else
            {
                g.drawText(String((int)round(100.*(midi - midiSave))), midX-dimc*0.25, midY, w, h, Justification::topLeft);
            }
        }
    }
    
    for (auto p : active->getParticles())
    {
        if (p != nullptr && p->getEnabled())
        {
            // DRAW PARTICLE IN MOTION
            if(springsOn) {
                midi = Utilities::clip(0, Utilities::ftom(Utilities::centsToFreq(p->getX() - (1200.0 * p->getOctave()))), 128);
                midi += ((p->getOctave() - 5) * 12.0);
            }
            else {
                midi = tuning->getOffset(p->getNote(), false);
                //DBG("midiOffset = " + String(midi) + " for note: " + String(p->getNote() % 12));
                midi += p->getNote();
            }
            
            //DBG("midi = " + String(midi));
            midiScale = midi / 60.;
            
            //int cents = roundToInt(((midi - (float)p->getNote())) * 100.0);
            int cents = round(((midi - (float)p->getNote())) * 100.0);
            
            scalex = ((midi - 60.0f) / 12.0f);
            
            posx = scalex *  (b.getWidth() - tetherSliders[0]->getRight());
            
            radians = scalex * Utilities::twopi - Utilities::pi * 0.5;

            cx = centerx + cosf(radians) * radius * midiScale - dimc * 0.5f;
            cy = centery + sinf(radians) * radius * midiScale - dimc * 0.5f;
            
            float hue = fmod(midi, 12.) / 12.;
            Colour colour (hue, 0.5f, 0.5f, 0.9f);
            
            //g.setColour (Colours::black);
            g.setColour (colour);
            g.fillEllipse(cx, cy, dimc, dimc);
            
            g.setColour(Colours::white);
            g.setFont(14.0f);
            //g.drawText(String(round(cents)), cx + dimc * 0.25, cy-dimc*0.7, 40, 10, Justification::topLeft);
            g.drawText(String(round(cents)), cx-dimc*0.25, cy+dimc*0.25, dimc * 1.5, dimc * 0.5, Justification::centred);
        }
        
        //DRAW REST PARTICLE
        midi = Utilities::clip(0, Utilities::ftom(Utilities::centsToFreq(p->getRestX() - (1200.0 * p->getOctave()))), 128);
        midi += ((p->getOctave() - 5) * 12.0);
        
        if(midi > 20 && midi < 109) {
            midiScale = midi / 60.;
            scalex = ((midi - 60.0f) / 12.0f);
            //posx = scalex *  (b.getWidth() - tetherSliders[0]->getRight());
            radians = scalex * Utilities::twopi - Utilities::pi * 0.5;
            cx = centerx + cosf(radians) * radius * midiScale - dimc * 0.5f;
            cy = centery + sinf(radians) * radius * midiScale - dimc * 0.5f;
            g.setColour (Colours::dimgrey);
            g.setOpacity(0.25);
            g.fillEllipse(cx, cy, dimc, dimc);
        }
    }
}

void TuningViewController::fillTuningCB(void)
{
    scaleCB.clear(dontSendNotification);
    springScaleCB.clear(dontSendNotification);
    A1IntervalScaleCB.clear(dontSendNotification);
    A1AnchorScaleCB.clear(dontSendNotification);
    
    //create submenu of historical temperaments
    PopupMenu* additionalTuningsPopUp = scaleCB.getRootMenu();
    OwnedArray<PopupMenu> submenus;
    submenus.add(new PopupMenu());
    submenus.add(new PopupMenu());
    
    PopupMenu* additionalTuningsPopUp2 = springScaleCB.getRootMenu();
    OwnedArray<PopupMenu> submenus2;
    submenus2.add(new PopupMenu());
    submenus2.add(new PopupMenu());
    
    PopupMenu* additionalTuningsPopUp3 = A1AnchorScaleCB.getRootMenu();
    OwnedArray<PopupMenu> submenus3;
    submenus3.add(new PopupMenu());
    submenus3.add(new PopupMenu());
    
    PopupMenu* additionalTuningsPopUp4 = A1IntervalScaleCB.getRootMenu();
    OwnedArray<PopupMenu> submenus4;
    submenus4.add(new PopupMenu());
    submenus4.add(new PopupMenu());
    
    int count =0;
    for (int i = 0; i < cTuningSystemNames.size(); i++) //&& if(i<=8), for original systems; otherwise add to submenu of historical temperaments
    {
        if (i == 3 || i == 4) continue;
        
        String name = cTuningSystemNames[i];
        
        if(i<=8) //original bK scales
        {
            scaleCB.addItem(name, i+1);
            A1IntervalScaleCB.addItem(name, i+1);
            A1AnchorScaleCB.addItem(name, i+1);
            springScaleCB.addItem(name, ++count);
        }
                
        if(name == "Custom") {
            customIndex = i - 2;
        }
        
        if(i>8 && i<=35) //historical
        {
            //add to Historical Temperaments popup
            DBG("adding historical temperament: " + name);
            PopupMenu* historicalMenu = submenus.getUnchecked(0);
            historicalMenu->addItem(i+1, name);
            
            PopupMenu* historicalMenu2 = submenus2.getUnchecked(0);
            historicalMenu2->addItem(i+1, name);
            
            PopupMenu* historicalMenu3 = submenus3.getUnchecked(0);
            historicalMenu3->addItem(i+1, name);
            
            PopupMenu* historicalMenu4 = submenus4.getUnchecked(0);
            historicalMenu4->addItem(i+1, name);
        }
        else if (i>35) //various
        {
            //add to Various popup
            DBG("adding various tunings: " + name);
            PopupMenu* variousMenu = submenus.getUnchecked(1);
            variousMenu->addItem(i+1, name);
            
            PopupMenu* variousMenu2 = submenus2.getUnchecked(1);
            variousMenu2->addItem(i+1, name);
            
            PopupMenu* variousMenu3 = submenus3.getUnchecked(1);
            variousMenu3->addItem(i+1, name);
            
            PopupMenu* variousMenu4 = submenus4.getUnchecked(1);
            variousMenu4->addItem(i+1, name);
        }
    }
    
    scaleCB.addSeparator();
    additionalTuningsPopUp->addSubMenu("Historical", *submenus.getUnchecked(0));
    additionalTuningsPopUp->addSubMenu("Various", *submenus.getUnchecked(1));
    
    springScaleCB.addSeparator();
    additionalTuningsPopUp2->addSubMenu("Historical", *submenus2.getUnchecked(0));
    additionalTuningsPopUp2->addSubMenu("Various", *submenus2.getUnchecked(1));
    
    A1AnchorScaleCB.addSeparator();
    additionalTuningsPopUp3->addSubMenu("Historical", *submenus3.getUnchecked(0));
    additionalTuningsPopUp3->addSubMenu("Various", *submenus3.getUnchecked(1));
    
    A1IntervalScaleCB.addSeparator();
    additionalTuningsPopUp4->addSubMenu("Historical", *submenus4.getUnchecked(0));
    additionalTuningsPopUp4->addSubMenu("Various", *submenus4.getUnchecked(1));
    
}

void TuningViewController::fillFundamentalCB(void)
{
    fundamentalCB.clear(dontSendNotification);
    A1FundamentalCB.clear(dontSendNotification);
    nToneRootCB.clear(dontSendNotification);
    nToneRootOctaveCB.clear(dontSendNotification);
    springScaleFundamentalCB.clear(dontSendNotification);
    
    for (int i = 0; i < cFundamentalNames.size(); i++)
    {
        String name = cFundamentalNames[i];
        fundamentalCB.addItem(name, i+1);
        A1FundamentalCB.addItem(name, i+1);
        nToneRootCB.addItem(name, i+1);
        springScaleFundamentalCB.addItem(name, i+1);
    }
    
    for (int i = 0; i < 9; i++)
    {
        nToneRootOctaveCB.addItem(String(i), i+1);
    }
    
    springScaleFundamentalCB.addItem("none", (int)cFundamentalNames.size()+1);
    springScaleFundamentalCB.addItem("lowest", (int)cFundamentalNames.size()+2);
    springScaleFundamentalCB.addItem("highest", (int)cFundamentalNames.size()+3);
    springScaleFundamentalCB.addItem("last", (int)cFundamentalNames.size()+4);
    
}

void TuningViewController::updateComponentVisibility()
{
    TuningPreparation::Ptr prep = processor.gallery->getStaticTuningPreparation(processor.updateState->currentTuningId);
    TuningPreparation::Ptr active = processor.gallery->getActiveTuningPreparation(processor.updateState->currentTuningId);
    Tuning::Ptr tuning = processor.gallery->getTuning(processor.updateState->currentTuningId);
    TuningModification::Ptr mod = processor.gallery->getTuningModification(processor.updateState->currentModTuningId);
    
    TuningAdaptiveSystemType adaptiveType = active->getAdaptiveType();
    
    if (currentTab == 0)
    {
        if (adaptiveType == AdaptiveNone || AdaptiveSpring)
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
            currentFundamental.setVisible(false);
            nToneRootCB.setVisible(true);
            nToneRootOctaveCB.setVisible(true);
            nToneSemitoneWidthSlider->setVisible(true);
            
            
            Array<float> intervalWeights = active->getSpringTuning()->getSpringWeights();
            if (intervalWeights.size() < 12)
            {
                intervalWeights = tuning->getStaticSpringTuning()->getSpringWeights();
            }
            
            for (int i = 0; i < 12; i++)
            {
                springSliders[i]->setValue(intervalWeights[i], dontSendNotification);
            }
        }
        
        if (adaptiveType == AdaptiveNormal)
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
            currentFundamental.setVisible(true);
            nToneRootCB.setVisible(false);
            nToneRootOctaveCB.setVisible(false);
            nToneSemitoneWidthSlider->setVisible(false);
        }
        else if (adaptiveType == AdaptiveAnchored)
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
            currentFundamental.setVisible(true);
            nToneRootCB.setVisible(false);
            nToneRootOctaveCB.setVisible(false);
            nToneSemitoneWidthSlider->setVisible(false);
        }
    }
    
#if 0
    for (auto s : tetherSliders)    s->setVisible(false);
    for (auto s : springSliders)    s->setVisible(false);
    for (auto l : springLabels)     l->setVisible(false);
    
    rateSlider->setVisible(false);
    dragSlider->setVisible(false);
    springScaleCB.setVisible(false);
    springScaleFundamentalCB.setVisible(false);
    
    tetherStiffnessSlider->setVisible(false);
    intervalStiffnessSlider->setVisible(false);
    
    absoluteKeyboard.setVisible(true);
    customKeyboard.setVisible(true);
    lastInterval.setVisible(true);
    lastNote.setVisible(true);
    offsetSlider->setVisible(true);
    adaptiveSystemsCB.setVisible(true);
    adaptiveSystemsCB.toFront(true);
    showSpringsButton.setVisible(true);
    
    if (adaptiveType == AdaptiveNone || showSprings || AdaptiveSpring)
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
        currentFundamental.setVisible(false);
        nToneRootCB.setVisible(true);
        nToneRootOctaveCB.setVisible(true);
        nToneSemitoneWidthSlider->setVisible(true);
    }
    
    if (!showSprings)
    {
        if (adaptiveType == AdaptiveNormal)
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
            currentFundamental.setVisible(true);
            nToneRootCB.setVisible(false);
            nToneRootOctaveCB.setVisible(false);
            nToneSemitoneWidthSlider->setVisible(false);
        }
        else if (adaptiveType == AdaptiveAnchored)
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
            currentFundamental.setVisible(true);
            nToneRootCB.setVisible(false);
            nToneRootOctaveCB.setVisible(false);
            nToneSemitoneWidthSlider->setVisible(false);
        }
    }
    else
    {
        bool tuningMod = (processor.updateState->currentDisplay == DisplayTuningMod);

        lastInterval.setVisible(false);
        lastNote.setVisible(false);
        
        Array<float> springWeights;
        Array<float> tetherWeights;
        
        if (tuningMod)
        {
            springWeights = mod->getSpringTuning()->getSpringWeights();
            tetherWeights = mod->getSpringTuning()->getTetherWeights();
        }
        else
        {
            springWeights = tuning->getCurrentSpringTuning()->getSpringWeights();
            tetherWeights = tuning->getCurrentSpringTuning()->getTetherWeights();
        }
        
        nToneRootCB.setVisible(false);
        nToneRootOctaveCB.setVisible(false);
        nToneSemitoneWidthSlider->setVisible(false);
        
        absoluteKeyboard.setVisible(false);
        customKeyboard.setVisible(false);
        offsetSlider->setVisible(false);
        
        if (tuning->getCurrentSpringTuning()->getActive() || tuningMod)
        {
            double val;
            
            rateSlider->setVisible(true);
            rateSlider->toFront(false);
            
            if (tuningMod)  val = mod->getSpringRate();
            else            val = active->getSpringTuning()->getRate();
            
            rateSlider->setValue(val, dontSendNotification);
            
            dragSlider->setVisible(true);
            dragSlider->toFront(false);
            
            if (tuningMod)  val = mod->getSpringTuning()->getDrag();
            else            val = active->getSpringTuning()->getDrag();
            
            val = dt_asymwarp_inverse(1.0f - val, 100.);
            dragSlider->setValue(val, dontSendNotification);
            
            tetherStiffnessSlider->setVisible(true);
            tetherStiffnessSlider->toFront(false);
            
            if (tuningMod)  val = mod->getSpringTuning()->getTetherStiffness();
            else            val = active->getSpringTuning()->getTetherStiffness();
            
            tetherStiffnessSlider->setValue(val, dontSendNotification);
            
            intervalStiffnessSlider->setVisible(true);
            intervalStiffnessSlider->toFront(false);
            
            if (tuningMod)  val = mod->getSpringTuning()->getIntervalStiffness();
            else            val = active->getSpringTuning()->getIntervalStiffness();
            
            intervalStiffnessSlider->setValue(val, dontSendNotification);
            
            springScaleCB.setVisible(true);
            springScaleCB.toFront(false);
            
            springScaleFundamentalCB.setVisible(true);
            springScaleFundamentalCB.toFront(false);
            
            for (int i = 0; i < 12; i++)
            {
                springSliders[i]->setVisible(true);
                springSliders[i]->toFront(false);
                
                springSliders[i]->setValue(springWeights[i], dontSendNotification);
                
                springLabels[i]->setVisible(true);
            }
            
            for (int i = 0; i < 128; i++)
            {
                tetherSliders[i]->toFront(false);
                
                tetherSliders[i]->setValue(tetherWeights[i], dontSendNotification);
            }
        }
    }
#endif
}

#if JUCE_IOS
void TuningViewController::iWantTheBigOne(TextEditor* tf, String name)
{
    hideOrShow.setAlwaysOnTop(false);
    bigOne.display(tf, name, getBounds());
}
#endif

void TuningViewController::timerCallback(void)
{
    //if (processor.updateState->currentDisplay == DisplayTuning)
    {
        TuningProcessor::Ptr tProcessor;
        TuningPreparation::Ptr active;
        Tuning::Ptr tuning;
        TuningModification::Ptr mod;
        
        int tuningId = processor.updateState->currentTuningId;
        bool isMod = false;
        if (processor.updateState->currentDisplay == DisplayTuningMod)
        {
            isMod = true;
            mod = processor.gallery->getTuningModification(processor.updateState->currentModTuningId);
            
            Array<int> preps = mod->getTargets();
            
            if (preps.size())   tuningId = preps[0];
            else                tuningId = -1;
            
        }
        
        tProcessor = processor.currentPiano->getTuningProcessor(tuningId);
        active = processor.gallery->getActiveTuningPreparation(tuningId);
        tuning = processor.gallery->getTuning(tuningId);
        
        if ((tProcessor != nullptr) &&
            (active != nullptr) &&
            (tuning != nullptr))
        {
            if (tProcessor->getLastNoteTuning() != lastNoteTuningSave)
            {
                lastNoteTuningSave = tProcessor->getLastNoteTuning();
                lastNote.setText("note: " + String(lastNoteTuningSave, 3), dontSendNotification);
                lastInterval.setText("interval: "  + String(tProcessor->getLastIntervalTuning(), 3), dontSendNotification);
                
                currentFundamental.setText("current fundamental: " + String(ftom(tProcessor->getAdaptiveFundamentalFreq()), 3), dontSendNotification);
            }
            
            if(active->getAdaptiveType() == AdaptiveNormal || active->getAdaptiveType() == AdaptiveAnchored )
            {
                A1ClusterMax->setDisplayValue(tProcessor->getAdaptiveHistoryCounter() + 1);
                
                if(tProcessor->getAdaptiveClusterTimer() < active->getAdaptiveClusterThresh())
                    A1ClusterThresh->setDisplayValue(tProcessor->getAdaptiveClusterTimer());
                else
                {
                    A1ClusterThresh->setDisplayValue(0);
                    A1ClusterMax->setDisplayValue(0);
                }
            }
        }
        
        
        
        if (showSprings)
        {
            float sliderHeight = springSliders.getUnchecked(0)->getHeight();
            
            Tuning::Ptr tuning = tProcessor->getTuning();
            Spring::PtrArr tetherSprings =  active->getTetherSprings();
            //Array<bool> locked = active->getSpringTuning()->getTethersLocked();
    
            int count = 0;
            for (int i = 0; i < 128; i++)
            {
                int pc = i % 12;
            
                if (tetherSprings[i]->getEnabled())
                {
                    tetherSliders[i]->setBounds(//fundamentalCB.getX(),
                                                //tetherStiffnessSlider->getRight() - springSliders[0]->getWidth() - hideOrShow.getWidth(),
                                                tetherStiffnessSlider->getX(),
                                                tetherStiffnessSlider->getBottom() + sliderHeight * (count),
                                                //fundamentalCB.getWidth() * 0.75,
                                                //(tetherStiffnessSlider->getWidth() - gComponentSingleSliderXOffset) * 0.85,
                                                springSliders[0]->getWidth(),
                                                sliderHeight);
                    
                    if (!isMod) tetherSliders[i]->setValue(tetherSprings[i]->getStrength(), dontSendNotification);
                    if(active->getAdaptiveType() == AdaptiveSpring) tetherSliders[i]->setVisible(true);
                    
                    tetherLabels[i]->setBounds(tetherSliders[i]->getRight() + gXSpacing,
                                               tetherSliders[i]->getY(),
                                               30,
                                               sliderHeight);
                    
                    tetherLabels[i]->setText(Utilities::getNoteString(i), dontSendNotification);
                    if(active->getAdaptiveType() == AdaptiveSpring) tetherLabels[i]->setVisible(true);
                    count++;
                    
                }
                else
                {
                    tetherSliders[i]->setVisible(false);
                    tetherLabels[i]->setVisible(false);
                }
                
            }
            
            repaint();
        }
    }
}

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ TuningPreparationEditor ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ //

TuningPreparationEditor::TuningPreparationEditor(BKAudioProcessor& p, BKItemGraph* theGraph):
TuningViewController(p, theGraph)
{
    fillSelectCB(-1,-1);
    
    for (int i = 0; i < 12; i++)
    {
        springSliders[i]->addListener(this);
    }
    
    for (int i = 0; i < 128; i++)
    {
        tetherSliders[i]->addListener(this);
    }
    
    adaptiveSystemsCB.addListener(this);
    rateSlider->addMyListener(this);
    dragSlider->addMyListener(this);
    intervalStiffnessSlider->addMyListener(this);
    tetherStiffnessSlider->addMyListener(this);
    springScaleCB.addListener(this);
    springScaleFundamentalCB.addListener(this);
    showSpringsButton.addListener(this);
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
    nToneRootCB.addListener(this);
    nToneRootOctaveCB.addListener(this);
    nToneSemitoneWidthSlider->addMyListener(this);
    
    update();
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
    else if (action == 6)
    {
        AlertWindow prompt("", "", AlertWindow::AlertIconType::QuestionIcon);
        
        int Id = processor.updateState->currentTuningId;
        Tuning::Ptr prep = processor.gallery->getTuning(Id);
        
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
}


void TuningPreparationEditor::bkComboBoxDidChange (ComboBox* box)
{
    String name = box->getName();
    int index = box->getSelectedItemIndex();
    int Id = box->getSelectedId();
    
    TuningPreparation::Ptr prep = processor.gallery->getStaticTuningPreparation(processor.updateState->currentTuningId);
    TuningPreparation::Ptr active = processor.gallery->getActiveTuningPreparation(processor.updateState->currentTuningId);
    Tuning::Ptr tuning = processor.gallery->getTuning(processor.updateState->currentTuningId);
    
    if (box == &selectCB)
    {
        setCurrentId(Id);
    }
    else if (box == &scaleCB)
    {
        DBG("name of scale chosen: " + box->getItemText(index));
        
        //redoing this so we index by tuning name, rather than index, so we don't lock the menu structure down
        prep->setScaleByName(box->getItemText(index));
        active->setScaleByName(box->getItemText(index));

        DBG("current TuningSystem " + prep->getScaleName());
        customKeyboard.setValues(tuning->getCurrentScaleCents());
        
        if (active->getAdaptiveType() == AdaptiveSpring)
        {
            prep->getSpringTuning()->setTetherTuning(tuning->getCurrentScaleCents());
            active->getSpringTuning()->setTetherTuning(tuning->getCurrentScaleCents());
        }
        else
        {
            prep->getSpringTuning()->setTetherTuning(EqualTemperament); //use ET as background when not in Spring Tuning
            active->getSpringTuning()->setTetherTuning(EqualTemperament);
        }
        
        //updateComponentVisibility();
        
    }
    else if (box == &adaptiveSystemsCB)
    {
        TuningAdaptiveSystemType type = (TuningAdaptiveSystemType) index;
        
        prep->setAdaptiveType(type);
        active->setAdaptiveType(type);
        
        // SpringTuning selected
        if (type == AdaptiveSpring)
        {
            showSprings = true;
            
            prep->getSpringTuning()->setActive(true);
            active->getSpringTuning()->setActive(true);
            
            prep->getSpringTuning()->setTetherTuning(tuning->getCurrentScaleCents());
            active->getSpringTuning()->setTetherTuning(tuning->getCurrentScaleCents());
            
            //need to make sure the interval scale is also set; i'm finding that sometimes i have to manually change away from just and back to get the system to work
            TuningSystem springScaleId = prep->getSpringTuning()->getScaleId();
            Array<float> scale = tuning->getScaleCents(springScaleId);
            prep->getSpringTuning()->setIntervalTuning(scale);
            active->getSpringTuning()->setIntervalTuning(scale);
            
            displayTab(currentTab);
            
        }
        else
        {
            showSprings = false;
            
            prep->getSpringTuning()->setActive(false);
            active->getSpringTuning()->setActive(false);
            
            prep->getSpringTuning()->setTetherTuning(EqualTemperament);
            active->getSpringTuning()->setTetherTuning(EqualTemperament);
            
            displayTab(currentTab);
        }
        
        
        if (type == AdaptiveNone) // Non-adaptive
        {
            //redoing this so we index by tuning name, rather than index, so we don't lock the menu structure down
            prep->setScaleByName(scaleCB.getText());
            active->setScaleByName(scaleCB.getText());
        }
        
        customKeyboard.setValues(tuning->getCurrentScaleCents());
        
        updateComponentVisibility();
    }
    else if (box == &fundamentalCB)
    {
        prep->setFundamental((PitchClass) index);
        active->setFundamental((PitchClass) index);
        
        prep->getSpringTuning()->setTetherFundamental((PitchClass) index);
        active->getSpringTuning()->setTetherFundamental((PitchClass) index);
        
        customKeyboard.setFundamental(index);
        
        //updateComponentVisibility();
        
    }
    else if (box == &A1IntervalScaleCB)
    {
        prep->setAdaptiveIntervalScale((TuningSystem) index);
        active->setAdaptiveIntervalScale((TuningSystem) index);
        
        //updateComponentVisibility();
    }
    else if (box == &A1AnchorScaleCB)
    {
        prep->setAdaptiveAnchorScale((TuningSystem) index);
        active->setAdaptiveAnchorScale((TuningSystem) index);
        
        //updateComponentVisibility();
    }
    else if (box == &A1FundamentalCB)
    {
        prep->setAdaptiveAnchorFundamental((PitchClass) index);
        active->setAdaptiveAnchorFundamental((PitchClass) index);
        
        //updateComponentVisibility();
        
    }
    else if (box == &nToneRootCB)
    {
        prep->setNToneRootPC(index);
        active->setNToneRootPC(index);
        
        //updateComponentVisibility();
    }
    else if (box == &nToneRootOctaveCB)
    {
        prep->setNToneRootOctave(index);
        active->setNToneRootOctave(index);
        
        //updateComponentVisibility();
    }
    else if (box == &springScaleCB)
    {
        TuningSystem springScaleId = (TuningSystem) index;
        
        if (springScaleId >= AdaptiveTuning) springScaleId = (TuningSystem)((int)springScaleId + 2);
        
        prep->getSpringTuning()->setScaleId(springScaleId);
        active->getSpringTuning()->setScaleId(springScaleId);
        
        //TuningSystem springScaleId = prep->getSpringTuning()->getScaleId();
        
        Array<float> scale = tuning->getScaleCents(springScaleId);
        
        prep->getSpringTuning()->setIntervalTuning(scale);
        active->getSpringTuning()->setIntervalTuning(scale);
        
        DBG("current springTuningSystem " + String(prep->getSpringTuning()->getScaleId()));
    }
    else if (box == &springScaleFundamentalCB)
    {
        prep->getSpringTuning()->setIntervalFundamental((PitchClass)index);
        active->getSpringTuning()->setIntervalFundamental((PitchClass)index);
        
        DBG("current springtuning interval Fundamental = " + String(index));
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
    
    Tuning::Ptr tuning = processor.gallery->getTuning(processor.updateState->currentTuningId);
    TuningPreparation::Ptr prep = processor.gallery->getActiveTuningPreparation(processor.updateState->currentTuningId);
    
    if (prep != nullptr)
    {
        selectCB.setSelectedId(processor.updateState->currentTuningId, dontSendNotification);
        int scaleIndex = prep->getScale();
        scaleIndex = (scaleIndex >= AdaptiveTuning) ? scaleIndex - 2 : scaleIndex;
        scaleCB.setSelectedItemIndex(scaleIndex, dontSendNotification);

        
        int springScaleId = prep->getCurrentSpringScaleId();
        if (springScaleId >= AdaptiveTuning) springScaleId = (TuningSystem)((int)springScaleId - 2);
        springScaleCB.setSelectedItemIndex(springScaleId, dontSendNotification);
        
        DBG("springScaleFundamentalCB.setSelectedItemIndex " + String(prep->getSpringTuning()->getIntervalFundamental()));
        springScaleFundamentalCB.setSelectedItemIndex((int)prep->getSpringTuning()->getIntervalFundamental(), dontSendNotification);
        
        fundamentalCB.setSelectedItemIndex(prep->getFundamental(), dontSendNotification);
        offsetSlider->setValue(prep->getFundamentalOffset() * 100., dontSendNotification);

        DBG("offsets: " + floatArrayToString(prep->getAbsoluteOffsets()));
        absoluteKeyboard.setValues(prep->getAbsoluteOffsetsCents());
        customKeyboard.setValues(tuning->getCurrentScaleCents());
        
        A1IntervalScaleCB.setSelectedItemIndex(prep->getAdaptiveIntervalScale(), dontSendNotification);
        A1Inversional.setToggleState(prep->getAdaptiveInversional(), dontSendNotification);
        A1AnchorScaleCB.setSelectedItemIndex(prep->getAdaptiveAnchorScale(), dontSendNotification);
        A1FundamentalCB.setSelectedItemIndex(prep->getAdaptiveAnchorFundamental(), dontSendNotification);
        A1ClusterThresh->setValue(prep->getAdaptiveClusterThresh(), dontSendNotification);
        A1ClusterMax->setValue(prep->getAdaptiveHistory(), dontSendNotification);
        
        nToneRootCB.setSelectedItemIndex(prep->getNToneRootPC(), dontSendNotification);
        nToneRootOctaveCB.setSelectedItemIndex(prep->getNToneRootOctave(), dontSendNotification);
        nToneSemitoneWidthSlider->setValue(prep->getNToneSemitoneWidth(), dontSendNotification);
        
        rateSlider->setValue(prep->getSpringTuning()->getRate(), dontSendNotification);
        
        tetherStiffnessSlider->setValue(prep->getSpringTuning()->getTetherStiffness(), dontSendNotification);
        intervalStiffnessSlider->setValue(prep->getSpringTuning()->getIntervalStiffness(), dontSendNotification);

        // springs active or adaptive tuning
        adaptiveSystemsCB.setSelectedItemIndex(prep->getAdaptiveType(), dontSendNotification);

        //dragSlider->setValue(  //must remember to use dt_asym_inversion on 1 - val)
        double newval = dt_asymwarp_inverse(1.0f - prep->getSpringTuning()->getDrag(), 100.);
        dragSlider->setValue(newval, dontSendNotification);
        
        /*
        if (!prep->getSpringTuning()->getUsingFundamentalForIntervalSprings())
            springScaleFundamentalCB.setSelectedItemIndex(12, dontSendNotification);
        else
            springScaleFundamentalCB.setSelectedItemIndex(prep->getSpringTuning()->getIntervalFundamental(), dontSendNotification);
        */

    }
    
    //updateComponentVisibility();
}

void TuningPreparationEditor::keyboardSliderChanged(String name, Array<float> values)
{
    Tuning::Ptr tuning = processor.gallery->getTuning(processor.updateState->currentTuningId);
    TuningPreparation::Ptr prep = processor.gallery->getStaticTuningPreparation(processor.updateState->currentTuningId);
    TuningPreparation::Ptr active = processor.gallery->getActiveTuningPreparation(processor.updateState->currentTuningId);
 
    if(name == absoluteKeyboard.getName())
    {
        //DBG("updating absolute tuning vals");
        prep->setAbsoluteOffsetCents(values);
        active->setAbsoluteOffsetCents(values);
    }
    else if(name == customKeyboard.getName())
    {
        //DBG("updating custom tuning vals");
        scaleCB.setSelectedItemIndex(customIndex, dontSendNotification);
        
        prep->setScaleByName(scaleCB.getItemText(customIndex));
        active->setScaleByName(scaleCB.getItemText(customIndex));
        
        //DBG("keyboardSliderChanged values.size() = " + String(values.size()));
        prep->setCustomScaleCents(values);
        active->setCustomScaleCents(values);
        
        prep->getSpringTuning()->setIntervalTuning(values);
        active->getSpringTuning()->setIntervalTuning(values);
        
    }
    processor.gallery->setGalleryDirty(true);
}

void TuningPreparationEditor::sliderValueChanged (Slider* slider)
{
    
    Tuning::Ptr tuning = processor.gallery->getTuning(processor.updateState->currentTuningId);
    TuningPreparation::Ptr prep = processor.gallery->getStaticTuningPreparation(processor.updateState->currentTuningId);
    TuningPreparation::Ptr active = processor.gallery->getActiveTuningPreparation(processor.updateState->currentTuningId);
    
    double value = slider->getValue();
    
    String name = slider->getName();

    for (int i = 0; i < 128; i++)
    {
        if (slider == tetherSliders[i])
        {
            prep->getSpringTuning()->setTetherWeight(i, value);
            active->getSpringTuning()->setTetherWeight(i, value);
            break;
        }
        else if (slider == springSliders[i])
        {
            prep->getSpringTuning()->setSpringWeight(i, value);
            active->getSpringTuning()->setSpringWeight(i, value);
            break;
        }
    }
}

void TuningPreparationEditor::BKSingleSliderValueChanged(BKSingleSlider* slider, String name, double val)
{
    TuningPreparation::Ptr prep = processor.gallery->getStaticTuningPreparation(processor.updateState->currentTuningId);
    TuningPreparation::Ptr active = processor.gallery->getActiveTuningPreparation(processor.updateState->currentTuningId);
    Tuning::Ptr tuning = processor.gallery->getTuning(processor.updateState->currentTuningId);
    
    if(slider == offsetSlider) {
        //DBG("got offset " + String(val));
        prep->setFundamentalOffset(val * 0.01);
        active->setFundamentalOffset(val * 0.01);
    }
    else if(slider == A1ClusterThresh) {
        //DBG("got A1ClusterThresh " + String(val));
        prep->setAdaptiveClusterThresh(val);
        active->setAdaptiveClusterThresh(val);
    }
    else if(slider == A1ClusterMax) {
        //DBG("got A1ClusterMax " + String(val));
        prep->setAdaptiveHistory(val);
        active->setAdaptiveHistory(val);
    }
    else if(slider == nToneSemitoneWidthSlider) {
        //DBG("got nToneSemiToneSliderWidth " + String(val));
        prep->setNToneSemitoneWidth(val);
        active->setNToneSemitoneWidth(val);
    }
    else if (slider == rateSlider)
    {
        prep->getSpringTuning()->setRate(val, false);
        active->getSpringTuning()->setRate(val);
    }
    else if (slider == dragSlider)
    {
        double newval = dt_asymwarp(val, 100.);
        //DBG("warped = " + String(newval) + " inverted = " + String(dt_asymwarp_inverse(newval, 100.)));
        prep->getSpringTuning()->setDrag(1. - newval);
        active->getSpringTuning()->setDrag(1. - newval);
    }
    else if (slider == tetherStiffnessSlider)
    {
        prep->getSpringTuning()->setTetherStiffness(val);
        active->getSpringTuning()->setTetherStiffness(val);
    }
    else if (slider == intervalStiffnessSlider)
    {
        prep->getSpringTuning()->setIntervalStiffness(val);
        active->getSpringTuning()->setIntervalStiffness(val);
    }
    
    processor.gallery->setGalleryDirty(true);
}

void TuningPreparationEditor::buttonClicked (Button* b)
{
    TuningPreparation::Ptr prep = processor.gallery->getStaticTuningPreparation(processor.updateState->currentTuningId);
    TuningPreparation::Ptr active = processor.gallery->getActiveTuningPreparation(processor.updateState->currentTuningId);
    Tuning::Ptr tuning = processor.gallery->getTuning(processor.updateState->currentTuningId);
    
    if (b == &A1Inversional)
    {
        DBG("setting A1Inversional " + String((int)A1Inversional.getToggleState()));

        prep->setAdaptiveInversional(A1Inversional.getToggleState());
        active->setAdaptiveInversional(A1Inversional.getToggleState());
        
        processor.gallery->setGalleryDirty(true);
    }
    else if (b == &showSpringsButton)
    {
        showSprings = !showSprings;
        displayTab(currentTab);
        DBG("showSprings = " + String((int)showSprings));
        
        //updateComponentVisibility();
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
    
    nToneRootCB.addListener(this);
    nToneRootOctaveCB.addListener(this);
    nToneSemitoneWidthSlider->addMyListener(this);
    
    
    // ~ ~ ~ ~ ~ SPRING TUNING STUFF ~ ~ ~ ~ ~
    for (int i = 0; i < 12; i++)
    {
        springSliders[i]->addListener(this);
    }
    
    for (int i = 0; i < 128; i++)
    {
        tetherSliders[i]->addListener(this);
    }
    
    adaptiveSystemsCB.addListener(this);
    rateSlider->addMyListener(this);
    dragSlider->addMyListener(this);
    intervalStiffnessSlider->addMyListener(this);
    tetherStiffnessSlider->addMyListener(this);
    springScaleCB.addListener(this);
    springScaleFundamentalCB.addListener(this);
    showSpringsButton.addListener(this);
    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

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
    
    nToneRootCB.setAlpha(gModAlpha);
    nToneRootOctaveCB.setAlpha(gModAlpha);
    nToneSemitoneWidthSlider->setDim(gModAlpha);
    
    springScaleFundamentalCB.setAlpha(gModAlpha);
    
    for (int i = 0; i < 12; i++)
    {
        springSliders[i]->setAlpha(gModAlpha);
    }
    
    for (int i = 0; i < 128; i++)
    {
        tetherSliders[i]->setAlpha(gModAlpha);
    }
    
    adaptiveSystemsCB.setAlpha(gModAlpha);
    
    rateSlider->setDim(gModAlpha);
    
    dragSlider->setDim(gModAlpha);
    
    intervalStiffnessSlider->setDim(gModAlpha);
    
    tetherStiffnessSlider->setDim(gModAlpha);
    
    springScaleCB.setAlpha(gModAlpha);
}

void TuningModificationEditor::highlightModedComponents()
{
    TuningModification::Ptr mod = processor.gallery->getTuningModification(processor.updateState->currentModTuningId);

    if(mod->getDirty(TuningScale))                scaleCB.setAlpha(1.);
    if(mod->getDirty(TuningFundamental))          fundamentalCB.setAlpha(1);
    if(mod->getDirty(TuningA1IntervalScale))      { A1IntervalScaleCB.setAlpha(1); A1IntervalScaleLabel.setAlpha(1); }
    if(mod->getDirty(TuningA1Inversional))        A1Inversional.setAlpha(1);
    if(mod->getDirty(TuningA1AnchorScale))        { A1AnchorScaleCB.setAlpha(1); A1AnchorScaleLabel.setAlpha(1); }
    if(mod->getDirty(TuningA1AnchorFundamental))  A1FundamentalCB.setAlpha(1);
    if(mod->getDirty(TuningA1ClusterThresh))      A1ClusterThresh->setBright();;
    if(mod->getDirty(TuningA1History))            A1ClusterMax->setBright();;
    if(mod->getDirty(TuningAbsoluteOffsets))      absoluteKeyboard.setAlpha(1);
    if(mod->getDirty(TuningCustomScale))          customKeyboard.setAlpha(1);
    if(mod->getDirty(TuningOffset))               offsetSlider->setBright();
    if(mod->getDirty(TuningNToneRootCB))          nToneRootCB.setAlpha(1);
    if(mod->getDirty(TuningNToneRootOctaveCB))    nToneRootOctaveCB.setAlpha(1);
    if(mod->getDirty(TuningNToneSemitoneWidth))   nToneSemitoneWidthSlider->setBright();
    if (mod->getDirty(TuningSpringTetherStiffness))
    {
        tetherStiffnessSlider->setBright();
    }
    if (mod->getDirty(TuningSpringIntervalStiffness))
    {
        intervalStiffnessSlider->setBright();
    }
    if (mod->getDirty(TuningSpringRate))
    {
        rateSlider->setBright();
    }
    if (mod->getDirty(TuningSpringDrag))
    {
        dragSlider->setBright();
    }
    if (mod->getDirty(TuningSpringActive))
    {
        adaptiveSystemsCB.setAlpha(1);
    }
    if (mod->getDirty(TuningSpringTetherWeights))
    {
        for (int i = 0; i < 128; i++)
        {
            if (mod->getTetherWeightActive(i))  tetherSliders[i]->setAlpha(1);
            else                                tetherSliders[i]->setAlpha(gModAlpha);
        }
    }
    if (mod->getDirty(TuningSpringIntervalWeights))
    {
        for (int i = 0; i < 12; i++)
        {
            if (mod->getSpringWeightActive(i))  springSliders[i]->setAlpha(1);
            else                                springSliders[i]->setAlpha(gModAlpha);
        }
    }
    if (mod->getDirty(TuningSpringIntervalScale))
    {
        springScaleCB.setAlpha(1);
    }
    if (mod->getDirty(TuningSpringIntervalFundamental))
    {
        springScaleFundamentalCB.setAlpha(1);
    }
    if (mod->getDirty(TuningAdaptiveSystem))
    {
        adaptiveSystemsCB.setAlpha(1);
    }
    
    repaint();
}

void TuningModificationEditor::update(void)
{
    TuningModification::Ptr mod = processor.gallery->getTuningModification(processor.updateState->currentModTuningId);
    
    if (mod != nullptr)
    {
        greyOutAllComponents();
        highlightModedComponents();
        
        selectCB.setSelectedId(processor.updateState->currentModTuningId, dontSendNotification);
        
        int scaleIndex = mod->getScale();
        scaleIndex = (scaleIndex >= AdaptiveTuning) ? scaleIndex - 2 : scaleIndex;
        scaleCB.setSelectedItemIndex(scaleIndex, dontSendNotification);
        
        fundamentalCB.setSelectedItemIndex( mod->getFundamental(), dontSendNotification);
        
        offsetSlider->setValue(mod->getFundamentalOffset() * 100., dontSendNotification);
        
        absoluteKeyboard.setValues(mod->getAbsoluteOffsetsCents());
        
        customKeyboard.setValues(mod->getCustomScaleCents());
        
        scaleIndex = mod->getAdaptiveIntervalScale();
        scaleIndex = (scaleIndex >= AdaptiveTuning) ? scaleIndex - 2 : scaleIndex;
        A1IntervalScaleCB.setSelectedItemIndex(scaleIndex, dontSendNotification);
        
        A1Inversional.setToggleState((bool)mod->getAdaptiveInversional(), dontSendNotification);
        
        A1AnchorScaleCB.setSelectedItemIndex(mod->getAdaptiveAnchorScale(), dontSendNotification);
        
        A1FundamentalCB.setSelectedItemIndex( mod->getAdaptiveAnchorFundamental(), dontSendNotification);
        
        A1ClusterThresh->setValue(mod->getAdaptiveClusterThresh(), dontSendNotification);
        
        A1ClusterMax->setValue(mod->getAdaptiveHistory(), dontSendNotification);
        
        nToneRootCB.setSelectedItemIndex(mod->getNToneRootPC(), dontSendNotification);
        
        nToneRootOctaveCB.setSelectedItemIndex(mod->getNToneRootOctave(), dontSendNotification);
        
        nToneSemitoneWidthSlider->setValue(mod->getNToneSemitoneWidth(), dontSendNotification);
        
        tetherStiffnessSlider->setValue(mod->getSpringTuning()->getTetherStiffness(), dontSendNotification);
        
        intervalStiffnessSlider->setValue(mod->getSpringTuning()->getIntervalStiffness(), dontSendNotification);
        
        rateSlider->setValue(mod->getSpringTuning()->getRate(), dontSendNotification);
        
        float val = mod->getSpringTuning()->getDrag();
        double newval = dt_asymwarp_inverse(1.0f - val, 100.);
        dragSlider->setValue(newval, dontSendNotification);
    
        adaptiveSystemsCB.setSelectedItemIndex(mod->getAdaptiveType(), dontSendNotification);

        Array<float> vals;
        
        /*
        val = mod->getParam(TuningSpringTetherWeights);
        vals = stringToFloatArray(val);
        for (int i = 0; i < 128; i++)
        {
            tetherSliders[i]->setValue(vals[i], dontSendNotification);
        }
        
        val = mod->getParam(TuningSpringIntervalWeights);
        vals = stringToFloatArray(val);
        for (int i = 0; i < 12; i++) springSliders[i]->setValue(vals[i], dontSendNotification);
        */
   
        scaleIndex = mod->getSpringTuning()->getScaleId();
        scaleIndex = (scaleIndex >= AdaptiveTuning) ? scaleIndex - 2 : scaleIndex;
        springScaleCB.setSelectedItemIndex(scaleIndex, dontSendNotification);
        
        int fund = mod->getSpringTuning()->getIntervalFundamental();
        springScaleFundamentalCB.setSelectedItemIndex(fund, dontSendNotification);
    
        updateComponentVisibility();
        A1reset.setVisible(false);
    }
    
}

void TuningModificationEditor::fillSelectCB(int last, int current)
{
    selectCB.clear(dontSendNotification);
    
    for (auto prep : processor.gallery->getTuningModifications())
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
    
    return processor.gallery->getTuningModifications().getLast()->getId();
}

int TuningModificationEditor::duplicatePreparation(void)
{
    processor.gallery->duplicate(PreparationTypeTuningMod, processor.updateState->currentModTuningId);
    
    return processor.gallery->getTuningModifications().getLast()->getId();
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
    else if (action == 6)
    {
        AlertWindow prompt("", "", AlertWindow::AlertIconType::QuestionIcon);
        
        int Id = processor.updateState->currentModTuningId;
        TuningModification::Ptr prep = processor.gallery->getTuningModification(Id);
        
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
}

void TuningModificationEditor::bkComboBoxDidChange (ComboBox* box)
{
    String name = box->getName();
    int index = box->getSelectedItemIndex();
    int Id = box->getSelectedId();
    
    TuningModification::Ptr mod = processor.gallery->getTuningModification(processor.updateState->currentModTuningId);
    
    if (box == &selectCB)
    {
        setCurrentId(Id);
    }
    else if (box == &scaleCB)
    {
        DBG("name of scale chosen: " + box->getItemText(index));
        
        //redoing this so we index by tuning name, rather than index, so we don't lock the menu structure down
        mod->setScaleByName(box->getItemText(index));
        
        scaleCB.setAlpha(1.);
        
        Tuning::Ptr currentTuning = processor.gallery->getTuning(processor.updateState->currentTuningId);
        customKeyboard.setValues(currentTuning->getCurrentScaleCents());
    }
    else if (box == &adaptiveSystemsCB)
    {
        mod->setAdaptiveType((TuningAdaptiveSystemType) index);
        mod->setDirty(TuningAdaptiveSystem);
    }
    else if (box == &fundamentalCB)
    {
        mod->setFundamental((PitchClass) index);
        mod->setDirty(TuningFundamental);
        
        fundamentalCB.setAlpha(1.);
        
        customKeyboard.setFundamental(index);
    }
    else if (box == &A1IntervalScaleCB)
    {
        mod->setAdaptiveIntervalScale((TuningSystem) index);
        mod->setDirty(TuningA1IntervalScale);
        
        A1IntervalScaleCB.setAlpha(1.);
        A1IntervalScaleLabel.setAlpha(1);
    }
    else if (box == &A1AnchorScaleCB)
    {
        mod->setAdaptiveAnchorScale((TuningSystem) index);
        mod->setDirty(TuningA1AnchorScale);
        
        A1AnchorScaleCB.setAlpha(1.);
        A1AnchorScaleLabel.setAlpha(1);
    }
    else if (box == &A1FundamentalCB)
    {
        mod->setAdaptiveAnchorFundamental((PitchClass) index);
        mod->setDirty(TuningA1AnchorFundamental);
        
        A1FundamentalCB.setAlpha(1.);
    }
    else if (box == &nToneRootCB)
    {
        mod->setNToneRootPC(index);
        mod->setDirty(TuningNToneRootCB);
        mod->setDirty(TuningNToneRoot); // ?
        
        nToneRootCB.setAlpha(1.);
    }
    else if (box == &nToneRootOctaveCB)
    {
        mod->setNToneRootOctave(index);
        mod->setDirty(TuningNToneRootOctaveCB);
        
        nToneRootOctaveCB.setAlpha(1.);
    }
    else if (box == &springScaleCB)
    {
        TuningSystem springScaleId = (TuningSystem) index;
        
        if (springScaleId >= AdaptiveTuning) springScaleId = (TuningSystem)((int)springScaleId + 2);
        
        mod->getSpringTuning()->setScaleId(springScaleId);
        mod->setDirty(TuningSpringIntervalScale);
    }
    else if (box == &springScaleFundamentalCB)
    {
        int fund = index;
        
        mod->getSpringTuning()->setIntervalFundamental((PitchClass)fund);
        mod->setDirty(TuningSpringIntervalFundamental);
    }

    if (name != selectCB.getName()) updateModification();
    
    updateComponentVisibility();
    A1reset.setVisible(false);
}

void TuningModificationEditor::BKEditableComboBoxChanged(String name, BKEditableComboBox* cb)
{
    processor.gallery->getTuningModification(processor.updateState->currentModTuningId)->setName(name);
    
    updateModification();
}

void TuningModificationEditor::keyboardSliderChanged(String name, Array<float> values)
{
    TuningModification::Ptr mod = processor.gallery->getTuningModification(processor.updateState->currentModTuningId);
    
    if(name == absoluteKeyboard.getName())
    {
        mod->setAbsoluteOffsets(values);
        mod->setDirty(TuningAbsoluteOffsets);
        
        absoluteKeyboard.setAlpha(1.);
        
    }
    else if(name == customKeyboard.getName())
    {
        scaleCB.setSelectedItemIndex(customIndex, dontSendNotification);
        
        mod->setCustomScale(values);
        mod->setDirty(TuningCustomScale);
        
        mod->setScaleByName(scaleCB.getItemText(customIndex));
        mod->setDirty(TuningScale);
        
        customKeyboard.setAlpha(1.);
    }
    
    updateModification();
}

void TuningModificationEditor::BKSingleSliderValueChanged(BKSingleSlider* slider, String name, double val)
{
    TuningModification::Ptr mod = processor.gallery->getTuningModification(processor.updateState->currentModTuningId);
    
    if(slider == offsetSlider)
    {
        mod->setFundamentalOffset(val * 0.01);
        mod->setDirty(TuningOffset);
        
        offsetSlider->setBright();
    }
    else if(slider == A1ClusterThresh)
    {
        mod->setAdaptiveClusterThresh(val);
        mod->setDirty(TuningA1ClusterThresh);
        
        A1ClusterThresh->setBright();
    }
    else if(slider == A1ClusterMax)
    {
        mod->setAdaptiveHistory(val);
        mod->setDirty(TuningA1History);
        
        A1ClusterMax->setBright();
    }
    else if(slider == nToneSemitoneWidthSlider)
    {
        mod->setNToneSemitoneWidth(val);
        mod->setDirty(TuningNToneSemitoneWidth);
        
        nToneSemitoneWidthSlider->setBright();
    }
    else if (slider == rateSlider)
    {
        mod->getSpringTuning()->setRate(val);
        mod->setDirty(TuningSpringRate);
        
        rateSlider->setBright();
    }
    else if (slider == dragSlider)
    {
        double newval = dt_asymwarp(val, 100.);
        mod->getSpringTuning()->setDrag(1.-newval);
        mod->setDirty(TuningSpringDrag);
        
        dragSlider->setBright();
    }
    else if (slider == tetherStiffnessSlider)
    {
        mod->getSpringTuning()->setTetherStiffness(val);
        mod->setDirty(TuningSpringTetherStiffness);
        
        tetherStiffnessSlider->setBright();
    }
    else if (slider == intervalStiffnessSlider)
    {
        mod->getSpringTuning()->setIntervalStiffness(val);
        mod->setDirty(TuningSpringIntervalStiffness);
        
        intervalStiffnessSlider->setBright();
    }

    updateModification();
}

void TuningModificationEditor::sliderValueChanged (Slider* slider)
{
    TuningModification::Ptr mod = processor.gallery->getTuningModification(processor.updateState->currentModTuningId);
    Tuning::Ptr tuning = processor.gallery->getTuning(processor.updateState->currentTuningId);
    
    double value = slider->getValue();
    
    String name = slider->getName();
    
    Array<float> tetherWeights = mod->getSpringTuning()->getTetherWeights();
    Array<float> intervalWeights = mod->getSpringTuning()->getSpringWeights();
    
    if (tetherWeights.size() < 128)
    {
        tetherWeights = tuning->getStaticSpringTuning()->getTetherWeights();
    }
    
    if (intervalWeights.size() < 12)
    {
        intervalWeights = tuning->getStaticSpringTuning()->getSpringWeights();
    }
    
    for (int i = 0; i < 128; i++)
    {
        if (slider == tetherSliders[i])
        {
            tetherWeights.setUnchecked(i, value);
            
            mod->getSpringTuning()->setTetherWeights(tetherWeights);
            mod->setDirty(TuningSpringTetherWeights);
            
            mod->setTetherWeightActive(i, true);
            
            break;
        }
        else if (slider == springSliders[i])
        {
            intervalWeights.setUnchecked(i, value);
            String thing = floatArrayToString(intervalWeights);
            
            mod->getSpringTuning()->setSpringWeights(intervalWeights);
            mod->setDirty(TuningSpringIntervalWeights);
            
            mod->setSpringWeightActive(i, true);
            
            break;
        }
    }
    
    updateModification();
}

void TuningModificationEditor::updateModification(void)
{
    greyOutAllComponents();
    highlightModedComponents();
    
    processor.updateState->modificationDidChange = true;
}

void TuningModificationEditor::buttonClicked (Button* b)
{
    TuningModification::Ptr mod = processor.gallery->getTuningModification(processor.updateState->currentModTuningId);
    
    if (b == &A1Inversional)
    {
        mod->setAdaptiveInversional(A1Inversional.getToggleState());
        mod->setDirty(TuningA1Inversional);
        
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
    else if (b == &showSpringsButton)
    {
        showSprings = !showSprings;
        
        updateComponentVisibility();
    }
    
    updateModification();
}
