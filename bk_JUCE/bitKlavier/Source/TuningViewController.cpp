/*
  ==============================================================================

    TuningViewController.cpp
    Created: 10 Jun 2017 11:24:22am
    Author:  Daniel Trueman

  ==============================================================================
*/

#include "TuningViewController.h"

TuningViewController::TuningViewController(BKAudioProcessor& p, BKItemGraph* theGraph):
BKViewController(p,theGraph),
showSprings(false)
#if JUCE_IOS
, absoluteKeyboard(true)
#endif
{
    setLookAndFeel(&buttonsAndMenusLAF);

    
    for (int i = 0; i < 128; i++)
    {
        Slider* s = new Slider("t" + String(i));
        
        s->setSliderStyle(Slider::SliderStyle::LinearBar);
        s->setRange(0.0, 1.0);
        //s->setSkewFactor(100., false);
        addChildComponent(s);
        tetherSliders.add(s);
        
        Label* l = new Label(s->getName(), Utilities::getNoteString(i));
        l->setColour(juce::Label::ColourIds::textColourId, Colours::black);
        addChildComponent(l);
        tetherLabels.add(l);
    }
    
    for (int i = 0; i < 12; i++)
    {
        Slider* s = new Slider(intervalNames[i+1]);
        
        s->setSliderStyle(Slider::SliderStyle::LinearBar);
        s->setRange(0.0, 1.0);
        //s->setSkewFactor(100., false);
        addChildComponent(s);
        springSliders.add(s);
        
        Label* l = new Label(s->getName(), s->getName());
        l->setColour(juce::Label::ColourIds::textColourId, Colours::black);
        addChildComponent(l);
        springLabels.add(l);
        
        ToggleButton* t = new ToggleButton("");
        //buttonsAndMenusLAF.setToggleBoxTextToRightBool(false);
        t->setColour(ToggleButton::textColourId, Colours::black);
        t->setColour(ToggleButton::tickColourId, Colours::black);
        t->setColour(ToggleButton::tickDisabledColourId, Colours::grey);
        t->setTooltip("When selected, all " + notesInAnOctave[i] + " springs will have same weight." );
        //addChildComponent(t);
        toggles.add(t);
        
        l = new Label(notesInAnOctave[i], notesInAnOctave[i]);
        l->setColour(juce::Label::ColourIds::textColourId, Colours::black);
        //addChildComponent(l);
        toggleLabels.add(l);
        
    }
    
    
    springTuningToggle.setColour(ToggleButton::textColourId, Colours::antiquewhite);
    springTuningToggle.setColour(ToggleButton::tickColourId, Colours::antiquewhite);
    springTuningToggle.setColour(ToggleButton::tickDisabledColourId, Colours::antiquewhite);
    springTuningToggle.setTooltip("Turns on/off Spring Tuning.");
    springTuningToggle.setAlwaysOnTop(true);
    springTuningToggle.setButtonText("");
    springTuningToggle.changeWidthToFitText();
    addChildComponent(springTuningToggle);
    
    springTuningLabel.setText("Springs", dontSendNotification);
    springTuningLabel.setColour(Label::ColourIds::textColourId, Colours::antiquewhite);
    addChildComponent(springTuningLabel);
    
    rateSlider.setSliderStyle(Slider::SliderStyle::LinearBar);
    rateSlider.setRange(5.0, 400.0);
    addChildComponent(rateSlider);
    
    dragSlider.setSliderStyle(Slider::SliderStyle::LinearBar);
    dragSlider.setRange(0.0, 1.0);
    addChildComponent(dragSlider);
    
    tetherStiffnessLabel.setText("stf", dontSendNotification);
    tetherStiffnessLabel.setColour(juce::Label::ColourIds::textColourId, Colours::black);
    addChildComponent(tetherStiffnessLabel);
    
    tetherStiffnessSlider.setSliderStyle(Slider::SliderStyle::LinearBar);
    tetherStiffnessSlider.setRange(0., 1.0);
    addChildComponent(tetherStiffnessSlider);
    
    intervalStiffnessLabel.setText("stf", dontSendNotification);
    intervalStiffnessLabel.setColour(juce::Label::ColourIds::textColourId, Colours::black);
    addChildComponent(intervalStiffnessLabel);
    
    intervalStiffnessSlider.setSliderStyle(Slider::SliderStyle::LinearBar);
    intervalStiffnessSlider.setRange(0., 1.0);
    addChildComponent(intervalStiffnessSlider);
    
    iconImageComponent.setImage(ImageCache::getFromMemory(BinaryData::tuning_icon_png, BinaryData::tuning_icon_pngSize));
    iconImageComponent.setImagePlacement(RectanglePlacement(juce::RectanglePlacement::stretchToFit));
    iconImageComponent.setAlpha(0.095);
    addAndMakeVisible(iconImageComponent);
    
    selectCB.setName("Tuning");
    selectCB.addSeparator();
    selectCB.addListener(this);
    selectCB.setSelectedItemIndex(0);
    selectCB.setTooltip("Select from available saved preparation settings");
    addAndMakeVisible(selectCB);
    
    scaleCB.setName("Scale");
    scaleCB.setTooltip("Select from a range of preset temperaments; open the Adaptive Tuning settings");
    addAndMakeVisible(scaleCB);
    
    springScaleCB.setName("SpringTuning Scale");
    springScaleCB.setTooltip("Select a temperament for the spring intervals.");
    addChildComponent(springScaleCB);
    
    scaleLabel.setText("Scale", dontSendNotification);
    //addAndMakeVisible(scaleLabel);
    
    fundamentalCB.setName("Fundamental");
    fundamentalCB.setTooltip("Select root around which your temperament will be generated");
    addAndMakeVisible(fundamentalCB);
    
    fundamentalLabel.setText("Fundamental", dontSendNotification);
    //addAndMakeVisible(fundamentalLabel);
    
    A1IntervalScaleCB.setName("A1IntervalScale");
    //A1IntervalScaleCB.BKSetJustificationType(juce::Justification::centredRight);
    A1IntervalScaleCB.addListener(this);
    A1IntervalScaleCB.setTooltip("scale the sets how successive intervals are tuned");
    addAndMakeVisible(A1IntervalScaleCB);
    
    A1IntervalScaleLabel.setText("Adaptive:", dontSendNotification);
    //A1IntervalScaleLabel.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(A1IntervalScaleLabel);
    
    A1Inversional.setButtonText ("invert");
    A1Inversional.setToggleState (true, dontSendNotification);
    //buttonsAndMenusLAF.setToggleBoxTextToRightBool(false);
    A1Inversional.setColour(ToggleButton::textColourId, Colours::antiquewhite);
    A1Inversional.setColour(ToggleButton::tickColourId, Colours::antiquewhite);
    A1Inversional.setColour(ToggleButton::tickDisabledColourId, Colours::antiquewhite);
    A1Inversional.setTooltip("when selected, intervals will be tuned the same whether they ascend or descend; e.g. C-D will always be the same interval as C-Bb");
    addAndMakeVisible(A1Inversional);
    
    
    
    A1AnchorScaleCB.setName("A1AnchorScale");
    A1AnchorScaleCB.setTooltip("determines where the moving fundamental will be tuned to");
    addAndMakeVisible(A1AnchorScaleCB);
    
    A1AnchorScaleLabel.setText("Anchor:", dontSendNotification);
    //A1AnchorScaleLabel.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(A1AnchorScaleLabel);
    
    A1FundamentalCB.setName("A1Fundamental");
    A1FundamentalCB.setTooltip("sets fundamental for the anchor scale");
    addAndMakeVisible(A1FundamentalCB);
    
    //A1FundamentalLabel.setText("Adaptive 1 Anchor Fundamental", dontSendNotification);
    //addAndMakeVisible(A1FundamentalLabel);
    
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
    
    showSpringsButton.setButtonText("Graph");
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
    
    updateComponentVisibility();
    
#if JUCE_IOS
    offsetSlider->addWantsBigOneListener(this);
    A1ClusterMax->addWantsBigOneListener(this);
    A1ClusterThresh->addWantsBigOneListener(this);
    
    absoluteKeyboard.addWantsBigOneListener(this);
    customKeyboard.addWantsBigOneListener(this);
    
    nToneSemitoneWidthSlider->addWantsBigOneListener(this);
#endif
    
}

void TuningViewController::resized()
{
    Rectangle<int> area (getLocalBounds());
    
    iconImageComponent.setBounds(area);
    area.reduce(10 * processor.paddingScalarX+3, 10 * processor.paddingScalarY+3);
    
    float keyboardHeight = 100 + 50 * processor.paddingScalarY;
    Rectangle<int> absoluteKeymapRow = area.removeFromBottom(keyboardHeight);
    absoluteKeymapRow.reduce(gXSpacing, 0);
    
    absoluteKeyboard.setBounds(absoluteKeymapRow);
    
    Rectangle<int> leftColumn = area.removeFromLeft(area.getWidth() * 0.5);
    Rectangle<int> comboBoxSlice = leftColumn.removeFromTop(gComponentComboBoxHeight);
    comboBoxSlice.removeFromRight(gXSpacing + 2.*gPaddingConst * processor.paddingScalarX);
    hideOrShow.setBounds(comboBoxSlice.removeFromLeft(gComponentComboBoxHeight));
    comboBoxSlice.removeFromLeft(gXSpacing);
    selectCB.setBounds(comboBoxSlice.removeFromLeft(comboBoxSlice.getWidth() / 2.));
    
    actionButton.setBounds(selectCB.getRight()+gXSpacing,
                           selectCB.getY(),
                           selectCB.getWidth() * 0.5,
                           selectCB.getHeight());
    
    comboBoxSlice.removeFromLeft(gXSpacing);
    
    A1reset.setBounds(actionButton.getRight()+gXSpacing,
                      actionButton.getY(),
                      actionButton.getWidth(),
                      actionButton.getHeight());
    
    
    /* *** above here should be generic (mostly) to all prep layouts *** */
    /* ***         below here will be specific to each prep          *** */
    
    // ********* right column
    
    Rectangle<int> modeSlice = area.removeFromTop(gComponentComboBoxHeight);
    modeSlice.removeFromLeft(gXSpacing + 2.*gPaddingConst * processor.paddingScalarX);
    modeSlice.removeFromRight(gXSpacing);
    scaleCB.setBounds(modeSlice.removeFromLeft(modeSlice.getWidth() / 2.));
    
    modeSlice.removeFromLeft(gXSpacing + gPaddingConst * processor.paddingScalarX);
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
    
    //DBG("extraY = " + String(extraY));
    
    leftColumn.removeFromTop(extraY + gYSpacing);
    Rectangle<int> A1IntervalScaleCBSlice = leftColumn.removeFromTop(gComponentComboBoxHeight);
    A1IntervalScaleCBSlice.removeFromRight(gXSpacing + 2.*gPaddingConst * processor.paddingScalarX);
    A1IntervalScaleCBSlice.removeFromLeft(gXSpacing);
    int tempwidth = A1IntervalScaleCBSlice.getWidth() / 3.;
    A1Inversional.setBounds(A1IntervalScaleCBSlice.removeFromRight(tempwidth));
    A1IntervalScaleCB.setBounds(A1IntervalScaleCBSlice.removeFromRight(tempwidth));
    A1IntervalScaleLabel.setBounds(A1IntervalScaleCBSlice);
    
    leftColumn.removeFromTop(extraY + gYSpacing);
    Rectangle<int> A1ClusterMaxSlice = leftColumn.removeFromTop(gComponentSingleSliderHeight);
    A1ClusterMaxSlice.removeFromRight(gXSpacing + 2.*gPaddingConst * processor.paddingScalarX - gComponentSingleSliderXOffset);
    //A1ClusterMaxSlice.removeFromLeft(gXSpacing);
    A1ClusterMax->setBounds(A1ClusterMaxSlice);
    nToneSemitoneWidthSlider->setBounds(A1ClusterMaxSlice);
    
    leftColumn.removeFromTop(gYSpacing);
    Rectangle<int> A1ClusterThreshSlice = leftColumn.removeFromTop(gComponentSingleSliderHeight);
    A1ClusterThreshSlice.removeFromRight(gXSpacing + 2.*gPaddingConst * processor.paddingScalarX - gComponentSingleSliderXOffset);
    //A1ClusterThreshSlice.removeFromLeft(gXSpacing);
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
    
    const int x_offset = 10;
    const int y_offset = TOP+75;
    const int w = 125;
    const int h = 25;
    const int yspacing = 3;
    const int xspacing = 3;
    
    
    for (int i = 0; i < 12; i++)
    {
        springLabels[i]->setBounds(x_offset, y_offset + (h + yspacing) * (i + 1), 30, h);
        springSliders[i]->setBounds(springLabels[i]->getRight() + xspacing, springLabels[i]->getY(), w, h);
        
        toggleLabels[i]->setBounds(springSliders[i]->getRight() + xspacing, springSliders[i]->getY(), 30, h);
        toggles[i]->setBounds(toggleLabels[i]->getRight() + xspacing, toggleLabels[i]->getY(), 30, h);
    }
    
    intervalStiffnessLabel.setBounds(springLabels[0]->getX(), y_offset, 30, h);
    intervalStiffnessSlider.setBounds(springSliders[0]->getX(), y_offset, w, h);
    
    tetherStiffnessLabel.setBounds(toggles[0]->getRight() + xspacing, y_offset, 30, h);
    tetherStiffnessSlider.setBounds(tetherStiffnessLabel.getRight() + xspacing, tetherStiffnessLabel.getY(), w, h);
    
    int width = getWidth() * 0.24, height = h;
    rateSlider.setBounds(getWidth() * 0.75, TOP + 15, width, height);
    dragSlider.setBounds(rateSlider.getX(), rateSlider.getBottom() + gYSpacing, width, height);
    springScaleCB.setBounds(dragSlider.getX(), dragSlider.getBottom() + gYSpacing, width, height);
    //springScaleCB.setBounds(rateSlider.getX(), rateSlider.getBottom() + gYSpacing, width, height);
    
    int labelWidth = 60;
    
    springTuningLabel.setBounds(scaleCB.getX(), scaleCB.getBottom() + gYSpacing, labelWidth, scaleCB.getHeight());
    
    springTuningToggle.setBounds(springTuningLabel.getRight() + gXSpacing, springTuningLabel.getY() - 6, 35, 35);
    
    
    showSpringsButton.setBounds(springTuningToggle.getRight()+gXSpacing,
                                springTuningLabel.getY(),
                                springTuningLabel.getWidth(),
                                scaleCB.getHeight());
    
    
    updateComponentVisibility();
}

void TuningViewController::paint (Graphics& g)
{
    if (processor.updateState->currentDisplay == DisplayTuning)
    {
        g.fillAll(Colours::black);
        
        TuningProcessor::Ptr tuning = processor.currentPiano->getTuningProcessor(processor.updateState->currentTuningId);
        Tuning::Ptr currenttuning = processor.gallery->getTuning(processor.updateState->currentTuningId);
        bool springsOn = currenttuning->getSpringsActive();
        
        if (!showSprings) return;
        
        Rectangle<int> b = getBounds();
        b.removeFromTop(TOP);
        
        g.setColour(Colours::antiquewhite);
        g.setOpacity(1.0);
        g.fillRect(b);

        g.setColour (Colours::black);
        g.setFont (40.0f);
        g.drawFittedText ("Spring Tuning", b, Justification::centredTop, 1);
        
        g.setFont(20.0f);
        g.drawFittedText("Intervals", 10, TOP+20, 150, 40, Justification::centredTop, 1);
        g.drawFittedText("Tethers", 170, TOP+20, 150, 40, Justification::centredTop, 1);

        
        float midi,scalex,posx,radians,cx,cy;
        float centerx = b.getWidth() * 0.7f,
        centery = b.getHeight() * 0.75f,
        radius = jmin(b.getHeight() * 0.32, b.getWidth() * 0.32);
        float dimc = jmin(b.getHeight() * 0.06, b.getWidth() * 0.06);
        int x_offset = 0.075 * b.getWidth();
        
        float midiScale;
        
        Particle::PtrArr particles = tuning->getTuning()->getTetherParticles();
        /*
        for (int i = 0; i < 12; i++)
        {
            // DRAW REST PARTICLE
            
            midi = Utilities::ftom(Utilities::centsToFreq(particles[i]->getRestX()));
            scalex = ((midi - 60.0f) / 12.0f);
            posx = scalex * (getWidth() - 2*x_offset);
            
            radians = scalex * Utilities::twopi - Utilities::pi * 0.5;
            
            cx = centerx + cosf(radians) * radius - dimc * 0.5f;
            cy = centery + sinf(radians) * radius - dimc * 0.5f;
            
            g.setColour (Colours::dimgrey);
            g.setOpacity(0.25);
            g.fillEllipse(cx, cy, dimc, dimc);
            
        }
        */
        for (auto s : tuning->getTuning()->getSprings())
        {
            if (s->getEnabled())
            {
                /*
                 if(springsOn) {
                 midi = Utilities::clip(0, Utilities::ftom(Utilities::centsToFreq(p->getX() - (1200.0 * p->getOctave()))), 128);
                 midi += ((p->getOctave() - 5) * 12.0);
                 }
                 else {
                 midi = .01 * currenttuning->getCurrentScaleCents()[p->getNote() % 12];
                 midi += p->getNote();
                 }
                 */
                
                Particle* a = s->getA();
                if(springsOn) midi = Utilities::ftom(Utilities::centsToFreq(a->getX() - (1200.0 * a->getOctave())));
                else {
                    //midi = .01 * currenttuning->getCurrentScaleCents()[a->getNote() % 12];
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
                    //midi = .01 * currenttuning->getCurrentScaleCents()[b->getNote() % 12];
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
                
                //g.setColour(Colours::lightgrey);
                g.setColour(colour);
                g.drawLine(cxa, cya, cxb, cyb,  (strength > 0.0) ? strength * 5.0 + 1.0 : 0.0);
                
                int h = 10, w = 35;
                
                int midX = (cxa + cxb) / 2.0; //+ xoff;
                int midY = (cya + cyb) / 2.0; //+ yoff;
                
                //g.saveState();
                
                //g.addTransform(AffineTransform::identity.rotated(radians, midX, midY));
                
                g.setColour(Colours::black);
                //g.setColour(colour);
                g.setFont(12.0f);
                if(springsOn) g.drawText(String((int)round(s->getLength())), midX-dimc*0.25, midY, w, h, Justification::topLeft);
                else g.drawText(String((int)round(100.*(midi - midiSave))), midX-dimc*0.25, midY, w, h, Justification::topLeft);
                //g.restoreState();
            
            }
            
        }
        
        for (auto p : tuning->getTuning()->getParticles())
        {
            if (p->getEnabled())
            {
                // DRAW PARTICLE IN MOTION
                if(springsOn) {
                    midi = Utilities::clip(0, Utilities::ftom(Utilities::centsToFreq(p->getX() - (1200.0 * p->getOctave()))), 128);
                    midi += ((p->getOctave() - 5) * 12.0);
                }
                else {
                    //midi = .01 * currenttuning->getCurrentScaleCents()[p->getNote() % 12];
                    midi = tuning->getOffset(p->getNote(), false);
                    //DBG("midiOffset = " + String(midi) + " for note: " + String(p->getNote() % 12));
                    midi += p->getNote();
                }
                
                //DBG("midi = " + String(midi));
                midiScale = midi / 60.;
                
                int cents = roundToInt(((midi - (float)p->getNote())) * 100.0);
                
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
    
}

void TuningViewController::fillTuningCB(void)
{
    scaleCB.clear(dontSendNotification);
    springScaleCB.clear(dontSendNotification);
    A1IntervalScaleCB.clear(dontSendNotification);
    A1AnchorScaleCB.clear(dontSendNotification);
    
    int count =0;
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
        else
        {
            springScaleCB.addItem(name, ++count);
            
        }
                
        if(name == "Custom") {
            customIndex = i;
        }

    }
}

void TuningViewController::fillFundamentalCB(void)
{
    fundamentalCB.clear(dontSendNotification);
    A1FundamentalCB.clear(dontSendNotification);
    nToneRootCB.clear(dontSendNotification);
    nToneRootOctaveCB.clear(dontSendNotification);
    
    for (int i = 0; i < cFundamentalNames.size(); i++)
    {
        String name = cFundamentalNames[i];
        fundamentalCB.addItem(name, i+1);
        A1FundamentalCB.addItem(name, i+1);
        nToneRootCB.addItem(name, i+1);
    }
    
    for (int i = 0; i < 9; i++)
    {
        nToneRootOctaveCB.addItem(String(i), i+1);
    }
}

void TuningViewController::updateComponentVisibility()
{
    for (auto s : tetherSliders)    s->setVisible(false);
    for (auto s : springSliders)    s->setVisible(false);
    for (auto l : springLabels)     l->setVisible(false);
    for (auto t : toggles)          t->setVisible(false);
    for (auto l : toggleLabels)     l->setVisible(false);
    
    rateSlider.setVisible(false);
    dragSlider.setVisible(false);
    springScaleCB.setVisible(false);
    
    tetherStiffnessLabel.setVisible(false);
    tetherStiffnessSlider.setVisible(false);
    
    intervalStiffnessLabel.setVisible(false);
    intervalStiffnessSlider.setVisible(false);
    
    absoluteKeyboard.setVisible(true);
    customKeyboard.setVisible(true);
    lastInterval.setVisible(true);
    offsetSlider->setVisible(true);
    springTuningToggle.setVisible(true);
    springTuningLabel.setVisible(true);
    showSpringsButton.setVisible(true);
    
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
        currentFundamental.setVisible(true);
        nToneRootCB.setVisible(false);
        nToneRootOctaveCB.setVisible(false);
        nToneSemitoneWidthSlider->setVisible(false);

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
        currentFundamental.setVisible(true);
        nToneRootCB.setVisible(false);
        nToneRootOctaveCB.setVisible(false);
        nToneSemitoneWidthSlider->setVisible(false);
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
        currentFundamental.setVisible(false);
        nToneRootCB.setVisible(true);
        nToneRootOctaveCB.setVisible(true);
        nToneSemitoneWidthSlider->setVisible(true);
    }
    
    if (showSprings)
    {
        lastInterval.setVisible(false);
        
        Tuning::Ptr tuning = processor.gallery->getTuning(processor.updateState->currentTuningId);
        Spring::PtrArr springs = tuning->getSprings();
        Spring::PtrArr tetherSprings = tuning->getTetherSprings();
        
        nToneRootCB.setVisible(false);
        nToneRootOctaveCB.setVisible(false);
        nToneSemitoneWidthSlider->setVisible(false);
        
        absoluteKeyboard.setVisible(false);
        customKeyboard.setVisible(false);
        offsetSlider->setVisible(false);
        
        rateSlider.setVisible(true);
        rateSlider.toFront(true);
        rateSlider.setValue(tuning->getSpringRate(), dontSendNotification);
        
        dragSlider.setVisible(true);
        dragSlider.toFront(true);
        dragSlider.setValue(tuning->getSpringTuning()->getDrag(), dontSendNotification);
        
        tetherStiffnessLabel.setVisible(true);
        tetherStiffnessSlider.setVisible(true);
        tetherStiffnessSlider.toFront(true);
        tetherStiffnessSlider.setValue(tuning->getSpringTuning()->getTetherStiffness(), dontSendNotification);
        
        intervalStiffnessLabel.setVisible(true);
        intervalStiffnessSlider.setVisible(true);
        intervalStiffnessSlider.toFront(true);
        intervalStiffnessSlider.setValue(tuning->getSpringTuning()->getIntervalStiffness(), dontSendNotification);
        
        springScaleCB.setVisible(true);
        springScaleCB.toFront(true);
        
        for (int i = 0; i < 12; i++)
        {
            springSliders[i]->setVisible(true);
            springSliders[i]->toFront(true);
            springSliders[i]->setValue(springs[i]->getStrength(), dontSendNotification);
            
            springLabels[i]->setVisible(true);
            
            toggles[i]->setVisible(true);
            toggles[i]->toFront(true);
            toggles[i]->setToggleState(tuning->getTetherLock(i), dontSendNotification);
            
            toggleLabels[i]->setVisible(true);
            
        }
        
        for (int i = 0; i < 128; i++)
        {
            tetherSliders[i]->toFront(true);
            tetherSliders[i]->setValue(tetherSprings[i]->getStrength(), dontSendNotification);
        }
        
    }
}

#if JUCE_IOS
void TuningViewController::iWantTheBigOne(TextEditor* tf, String name)
{
    hideOrShow.setAlwaysOnTop(false);
    bigOne.display(tf, name, getBounds());
}
#endif

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ TuningPreparationEditor ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ //

TuningPreparationEditor::TuningPreparationEditor(BKAudioProcessor& p, BKItemGraph* theGraph):
TuningViewController(p, theGraph)
{
    fillSelectCB(-1,-1);
    
    for (int i = 0; i < 12; i++)
    {
        springSliders[i]->addListener(this);
        toggles[i]->addListener(this);
    }
    
    for (int i = 0; i < 128; i++)
    {
        tetherSliders[i]->addListener(this);
    }
    
    springTuningToggle.addListener(this);
    
    rateSlider.addListener(this);
    
    dragSlider.addListener(this);
    
    intervalStiffnessSlider.addListener(this);
    
    tetherStiffnessSlider.addListener(this);
    
    selectCB.addMyListener(this);
    
    selectCB.addListener(this);

    scaleCB.addListener(this);
    
    springScaleCB.addListener(this);
    
    fundamentalCB.addListener(this);

    A1IntervalScaleCB.addListener(this);
    
    A1Inversional.addListener(this);
    
    A1AnchorScaleCB.addListener(this);
    
    A1FundamentalCB.addListener(this);
    
    A1ClusterThresh->addMyListener(this);
    
    A1ClusterMax->addMyListener(this);
    
    A1reset.addListener(this);
    
    showSpringsButton.addListener(this);
    
    absoluteKeyboard.addMyListener(this);
    
    customKeyboard.addMyListener(this);
    
    offsetSlider->addMyListener(this);
    
    nToneRootCB.addListener(this);
    
    nToneRootOctaveCB.addListener(this);
    
    nToneSemitoneWidthSlider->addMyListener(this);
    
    startTimerHz(30);
    
    update();
}

void TuningPreparationEditor::timerCallback()
{
    if (processor.updateState->currentDisplay == DisplayTuning)
    {
        TuningProcessor::Ptr tProcessor = processor.currentPiano->getTuningProcessor(processor.updateState->currentTuningId);
        TuningPreparation::Ptr active = processor.gallery->getActiveTuningPreparation(processor.updateState->currentTuningId);
        
        if (tProcessor != nullptr)
        {
            if (tProcessor->getLastNoteTuning() != lastNoteTuningSave)
            {
                lastNoteTuningSave = tProcessor->getLastNoteTuning();
                lastNote.setText("note: " + String(lastNoteTuningSave, 3), dontSendNotification);
                lastInterval.setText("interval: "  + String(tProcessor->getLastIntervalTuning(), 3), dontSendNotification);
                
                currentFundamental.setText("current fundamental: " + String(ftom(tProcessor->getAdaptiveFundamentalFreq()), 3), dontSendNotification);
            }
            
            if(active->getScale() == AdaptiveTuning || active->getScale() == AdaptiveAnchoredTuning )
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
            const int x_offset = 10;
            const int y_offset = tetherStiffnessSlider.getBottom()+3;
            const int w = 125;
            const int h = 25;
            const int yspacing = 3;
            const int xspacing = 3;
            
            Tuning::Ptr tuning = tProcessor->getTuning();
            Spring::PtrArr tetherSprings =  tuning->getTetherSprings();
            Array<bool> locked = tuning->getSpringTuning()->getTethersLocked();
            
            // Place locked tethers first
            for (int i = 0; i < 12; i++)
            {
                if (locked[i])
                {
                    tetherLabels[i]->setBounds(toggles[0]->getRight() + xspacing, y_offset + (h + yspacing) * i, 30, h);
                    tetherLabels[i]->setText(Utilities::getNoteString(i, false), dontSendNotification);
                    tetherLabels[i]->setVisible(true);
                    
                    tetherSliders[i]->setBounds(tetherLabels[i]->getRight() + xspacing, tetherLabels[i]->getY(), w, h);
                    tetherSliders[i]->setValue(tetherSprings[i]->getStrength(), dontSendNotification);
                    tetherSliders[i]->setVisible(true);
                }
                
            }
            
            int count = 0;
            for (int i = 0; i < 128; i++)
            {
                int pc = i % 12;
                
                if (!locked[pc])
                {
                    if (tetherSprings[i]->getEnabled())
                    {
                        tetherLabels[i]->setBounds(toggles[0]->getRight() + xspacing, y_offset + (h + yspacing) * (count + 1), 30, h);
                        tetherLabels[i]->setText(Utilities::getNoteString(i), dontSendNotification);
                        tetherLabels[i]->setVisible(true);
                        
                        tetherSliders[i]->setBounds(tetherLabels[i]->getRight() + xspacing, tetherLabels[i]->getY(), w, h);
                        tetherSliders[i]->setValue(tetherSprings[i]->getStrength(), dontSendNotification);
                        tetherSliders[i]->setVisible(true);
                        
                        // Clever little counter
                        count++;
                        // check if count is index of locked tether.
                        // increment until it is not an index of a locked tether.
                        for (int i = count; i < 12; i++)
                        {
                            if (locked[count])
                            {
                                count++;
                            }
                            else break;
                        }
                        
                    }
                    else
                    {
                        tetherSliders[i]->setVisible(false);
                        tetherLabels[i]->setVisible(false);
                    }
                }
               
            }
            
            repaint();
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
    
    if (box == &selectCB)
    {
        setCurrentId(Id);
    }
    else if (box == &scaleCB)
    {
        prep->setScale((TuningSystem) index);
        active->setScale((TuningSystem) index);
        
        Tuning::Ptr currentTuning = processor.gallery->getTuning(processor.updateState->currentTuningId);
        DBG("current tuning from processor = " + String(processor.updateState->currentTuningId));
        customKeyboard.setValues(currentTuning->getCurrentScaleCents());
        
        if(currentTuning->getSpringsActive())
            currentTuning->getSpringTuning()->setTetherTuning(currentTuning->getCurrentScaleCents());
        else
            currentTuning->getSpringTuning()->setTetherTuning(EqualTemperament); //use ET as background when not in Spring Tuning
        
        updateComponentVisibility();
        
    }
    else if (box == &fundamentalCB)
    {
        prep->setFundamental((PitchClass) index);
        active->setFundamental((PitchClass) index);
        
        customKeyboard.setFundamental(index);
        
        updateComponentVisibility();
        
    }
    else if (box == &A1IntervalScaleCB)
    {
        prep->setAdaptiveIntervalScale((TuningSystem) index);
        active->setAdaptiveIntervalScale((TuningSystem) index);
        
        updateComponentVisibility();
    }
    else if (box == &A1AnchorScaleCB)
    {
        prep->setAdaptiveAnchorScale((TuningSystem) index);
        active->setAdaptiveAnchorScale((TuningSystem) index);
        
        updateComponentVisibility();
    }
    else if (box == &A1FundamentalCB)
    {
        prep->setAdaptiveAnchorFundamental((PitchClass) index);
        active->setAdaptiveAnchorFundamental((PitchClass) index);
        
        updateComponentVisibility();
        
    }
    else if (box == &nToneRootCB)
    {
        prep->setNToneRootPC(index);
        active->setNToneRootPC(index);
        
        updateComponentVisibility();
    }
    else if (box == &nToneRootOctaveCB)
    {
        prep->setNToneRootOctave(index);
        active->setNToneRootOctave(index);
        
        updateComponentVisibility();
    }
    else if (box == &springScaleCB)
    {
        Tuning::Ptr tuning = processor.gallery->getTuning(processor.updateState->currentTuningId);
     
        TuningSystem springScaleId = (TuningSystem) index;
        
        tuning->getSpringTuning()->setScaleId(springScaleId);
        
        Array<float> scale = tuning->getScaleCents(springScaleId);
        
        tuning->getSpringTuning()->setIntervalTuning(scale);
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
        scaleCB.setSelectedItemIndex(prep->getScale(), dontSendNotification);
        springScaleCB.setSelectedItemIndex(tuning->getCurrentSpringScaleId(), dontSendNotification);
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
        
        nToneRootCB.setSelectedItemIndex(prep->getNToneRootPC(), dontSendNotification);
        nToneRootOctaveCB.setSelectedItemIndex(prep->getNToneRootOctave(), dontSendNotification);
        nToneSemitoneWidthSlider->setValue(prep->getNToneSemitoneWidth(), dontSendNotification);
        
        //NEED TO CHANGE THE FOLLOWING AFTER SPRING TUNING IS FULLY INTEGRATED
        tetherStiffnessSlider.setValue(tuning->getSpringTuning()->getTetherStiffness());
        intervalStiffnessSlider.setValue(tuning->getSpringTuning()->getIntervalStiffness());
    }
    
    updateComponentVisibility();
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
        
        prep->setScale((TuningSystem)customIndex);
        active->setScale((TuningSystem)customIndex);
        
        //DBG("keyboardSliderChanged values.size() = " + String(values.size()));
        prep->setCustomScaleCents(values);
        active->setCustomScaleCents(values);
        
        tuning->getSpringTuning()->setIntervalTuning(values);
        
    }
    processor.gallery->setGalleryDirty(true);
}

void TuningPreparationEditor::sliderValueChanged (Slider* slider)
{
    double value = slider->getValue();
    
    String name = slider->getName();
    
    Tuning::Ptr tuning = processor.gallery->getTuning(processor.updateState->currentTuningId);
    
    Spring::PtrArr tetherSprings = tuning->getTetherSprings();
    Spring::PtrArr springs = tuning->getSprings();
    
    if (slider == &rateSlider)
    {
        tuning->setSpringRate(value);
    }
    
    else if (slider == &dragSlider)
    {
        tuning->getSpringTuning()->setDrag(value);
    }
    
    else if (slider == &tetherStiffnessSlider)
    {
        tuning->getSpringTuning()->setTetherStiffness(value);
    }
    else if (slider == &intervalStiffnessSlider)
    {
        tuning->getSpringTuning()->setIntervalStiffness(value);
    }
    else
    {
        for (int i = 0; i < 128; i++)
        {
            if (slider == tetherSliders[i])
            {
                tuning->setTetherWeight(i, value);
                break;
            }
            else if (slider == springSliders[i])
            {
                tuning->setSpringWeight((i+1), value);
                break;
            }
        }
    }
    
}

void TuningPreparationEditor::BKSingleSliderValueChanged(String name, double val)
{
    TuningPreparation::Ptr prep = processor.gallery->getStaticTuningPreparation(processor.updateState->currentTuningId);
    TuningPreparation::Ptr active = processor.gallery->getActiveTuningPreparation(processor.updateState->currentTuningId);
    
    if(name == offsetSlider->getName()) {
        //DBG("got offset " + String(val));
        prep->setFundamentalOffset(val * 0.01);
        active->setFundamentalOffset(val * 0.01);
    }
    else if(name == A1ClusterThresh->getName()) {
        //DBG("got A1ClusterThresh " + String(val));
        prep->setAdaptiveClusterThresh(val);
        active->setAdaptiveClusterThresh(val);
    }
    else if(name == A1ClusterMax->getName()) {
        //DBG("got A1ClusterMax " + String(val));
        prep->setAdaptiveHistory(val);
        active->setAdaptiveHistory(val);
    }
    else if(name == nToneSemitoneWidthSlider->getName()) {
        //DBG("got nToneSemiToneSliderWidth " + String(val));
        prep->setNToneSemitoneWidth(val);
        active->setNToneSemitoneWidth(val);
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
    else if (b == &showSpringsButton)
    {
        showSprings = !showSprings;
        
        updateComponentVisibility();
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
    else if (b == &springTuningToggle)
    {
        bool state = b->getToggleState();
        
        Tuning::Ptr tuning = processor.gallery->getTuning(processor.updateState->currentTuningId);
        tuning->setSpringsActive(state);
    }
    else
    {
        for (int i = 0; i < 12; i++)
        {
            if (b == toggles[i])
            {
                Tuning::Ptr tuning = processor.gallery->getTuning(processor.updateState->currentTuningId);
                tuning->setTetherLock(i, b->getToggleState());
                break;
            }
        }
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
    if(mod->getParam(TuningNToneRootCB) != "")          nToneRootCB.setAlpha(1);
    if(mod->getParam(TuningNToneRootOctaveCB) != "")    nToneRootOctaveCB.setAlpha(1);
    if(mod->getParam(TuningNToneSemitoneWidth) != "")   nToneSemitoneWidthSlider->setBright();
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
        
        val = mod->getParam(TuningNToneRootCB);
        nToneRootCB.setSelectedItemIndex(val.getIntValue(), dontSendNotification);
        //                       fundamentalCB.setSelectedItemIndex(prep->getFundamental(), dontSendNotification);
        
        val = mod->getParam(TuningNToneRootOctaveCB);
        nToneRootOctaveCB.setSelectedItemIndex(val.getIntValue(), dontSendNotification);
        //                       fundamentalCB.setSelectedItemIndex(prep->getFundamental(), dontSendNotification);
        
        val = mod->getParam(TuningNToneSemitoneWidth);
        nToneSemitoneWidthSlider->setValue(val.getFloatValue(), dontSendNotification);
        //                       offsetSlider->setValue(prep->getFundamentalOffset() * 100., dontSendNotification);
        
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
    else if (action == 6)
    {
        AlertWindow prompt("", "", AlertWindow::AlertIconType::QuestionIcon);
        
        int Id = processor.updateState->currentModTuningId;
        TuningModPreparation::Ptr prep = processor.gallery->getTuningModPreparation(Id);
        
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
    else if (name == nToneRootCB.getName())
    {
        mod->setParam(TuningNToneRootCB, String(index));
        nToneRootCB.setAlpha(1.);
    }
    else if (name == nToneRootOctaveCB.getName())
    {
        mod->setParam(TuningNToneRootOctaveCB, String(index));
        nToneRootOctaveCB.setAlpha(1.);
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
    if(name == nToneSemitoneWidthSlider->getName())
    {
        mod->setParam(TuningNToneSemitoneWidth, String(val));
        nToneSemitoneWidthSlider->setBright();
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






