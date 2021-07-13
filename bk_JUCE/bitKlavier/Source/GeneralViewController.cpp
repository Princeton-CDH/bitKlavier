/*
  ==============================================================================

    GeneralViewController.cpp
    Created: 30 Nov 2016 5:00:39pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "GeneralViewController.h"

CommentViewController::CommentViewController(BKAudioProcessor& p, BKItemGraph* theGraph):
BKViewController(p, theGraph, 1)
{
    setLookAndFeel(&buttonsAndMenusLAF);
    
    removeChildComponent(&hideOrShow);
    
    addAndMakeVisible(comment);
    comment.setName("comment");
    comment.setMultiLine(true);
    comment.setWantsKeyboardFocus(true);
    
    comment.setReturnKeyStartsNewLine(true);
    comment.addListener(this);
    comment.setText("Text here...");
#if JUCE_IOS
    comment.setScrollbarsShown(true);
#endif
    //comment.setColour(TextEditor::ColourIds::high, )
    
    comment.setColour(TextEditor::ColourIds::backgroundColourId, Colours::burlywood.withMultipliedBrightness(0.45f));
    comment.setColour(TextEditor::ColourIds::textColourId, Colours::antiquewhite);
    
    comment.setOpaque(false);
    
    addAndMakeVisible(ok);
    ok.setButtonText("Ok");
    ok.addListener(this);
    ok.setColour(TextEditor::ColourIds::backgroundColourId, Colours::black.withAlpha(0.5f));
    
    addAndMakeVisible(cancel);
    cancel.setButtonText("Cancel");
    cancel.addListener(this);
    cancel.setColour(TextEditor::ColourIds::backgroundColourId, Colours::red.withAlpha(0.2f));
}

CommentViewController::~CommentViewController()
{
    setLookAndFeel(nullptr);
}

void CommentViewController::update(void)
{
    comment.setText(processor.updateState->comment, dontSendNotification);
    comment.grabKeyboardFocus();
}

void CommentViewController::paint (Graphics& g)
{
    g.fillAll(Colours::transparentBlack);
}

void CommentViewController::resized(void)
{
    float heightUnit = (getHeight() - hideOrShow.getHeight()) * 0.1;
    
#if JUCE_IOS
    cancel.setBounds    (0,                 hideOrShow.getBottom(), getWidth()*0.5,         2*heightUnit);
    ok.setBounds        (getWidth()*0.5,    cancel.getY(),          getWidth()*0.5,     2*heightUnit);
    comment.setBounds   (0,                 cancel.getBottom(),     getWidth(),     8*heightUnit);
#else
    comment.setBounds   (0,                 hideOrShow.getBottom(), getWidth(),         9*heightUnit);
    cancel.setBounds    (comment.getX(),    comment.getBottom(),    getWidth()*0.5,     1*heightUnit);
    ok.setBounds        (cancel.getRight(), cancel.getY(),          getWidth()*0.5,     1*heightUnit);
#endif
}

void CommentViewController::bkTextFieldDidChange (TextEditor& tf)
{

}

void CommentViewController::bkButtonClicked (Button* b)
{
    if (b == &ok)
    {
        if (processor.updateState->comment != comment.getText())
            processor.updateState->editsMade = true;
        processor.updateState->comment = comment.getText();
        processor.updateState->commentDidChange = true;
        processor.updateState->setCurrentDisplay(DisplayNil);
    }
    else if (b == &cancel)
    {
        processor.updateState->comment = "";
        processor.updateState->setCurrentDisplay(DisplayNil);
    }
}

AboutViewController::AboutViewController(BKAudioProcessor& p, BKItemGraph* theGraph):
BKViewController(p, theGraph, 1)
{
    setLookAndFeel(&buttonsAndMenusLAF);
    
    addAndMakeVisible(about);
    about.setEnabled(false);
    about.setJustification(Justification::centredTop);
    about.setMultiLine(true);
    String astring = "Welcome to bitKlavier v";
    astring.append(JucePlugin_VersionString, 8);
    astring += "! www.bitKlavier.com\n--\nbitKlavier was created by\nDan Trueman\n Mike Mulshine\nMatt Wang";
    about.setText(astring);

    image = ImageCache::getFromMemory(BinaryData::icon_png, BinaryData::icon_pngSize);
    
    placement = RectanglePlacement::centred;
}

AboutViewController::~AboutViewController()
{
    setLookAndFeel(nullptr);
}
    
void AboutViewController::paint (Graphics& g)
{
    g.fillAll(Colours::black);
    
    g.setOpacity (1.0f);
    
    g.drawImage (image, imageRect, placement);
}

void AboutViewController::resized(void)
{
    hideOrShow.setBounds(10,10,gComponentComboBoxHeight,gComponentComboBoxHeight);
    
    float imageZ = getHeight() * 0.5;
    float imageX = getWidth() * 0.5 - imageZ * 0.5;
    float imageY = 50;
    
    imageRect.setBounds(imageX, imageY, imageZ, imageZ);
    
    about.setBounds(10, imageRect.getBottom() + 20, getWidth() - 20, getBottom() - (imageRect.getBottom() + 20));
    
    repaint();
}

void AboutViewController::bkButtonClicked (Button* b)
{
    if (b == &hideOrShow)
    {
        processor.updateState->setCurrentDisplay(DisplayNil);
    }
}


//==============================================================================
GeneralViewController::GeneralViewController(BKAudioProcessor& p, BKItemGraph* theGraph):
BKViewController(p, theGraph, 2),
processor(p)
{
    
    setLookAndFeel(&buttonsAndMenusLAF);
    
    // Settings UI components setup
    A4tuningReferenceFrequencySlider = std::make_unique<BKSingleSlider>("A4 reference frequency", "A4 reference frequency", 415., 450., 440., 0.1);
    A4tuningReferenceFrequencySlider->setJustifyRight(false);
    A4tuningReferenceFrequencySlider->addMyListener(this);
    addAndMakeVisible(*A4tuningReferenceFrequencySlider);
    
    tempoMultiplierSlider = std::make_unique<BKSingleSlider>("tempo multiplier", "tempo multiplier", 0.25, 4., 1., 0.01);
    tempoMultiplierSlider->setSkewFactorFromMidPoint(1.);
    tempoMultiplierSlider->setJustifyRight(false);
    tempoMultiplierSlider->addMyListener(this);
    addAndMakeVisible(*tempoMultiplierSlider);
    
    GeneralSettings::Ptr gen = processor.gallery->getGeneralSettings();
    
    invertSustainB.addListener(this);
    invertSustainB.setButtonText("invert sustain");
    invertSustainB.setToggleState(gen->getInvertSustain(), dontSendNotification);
    processor.setSustainInversion(gen->getInvertSustain());
    addAndMakeVisible(invertSustainB);
    
    noteOnSetsNoteOffVelocityB.addListener(this);
    noteOnSetsNoteOffVelocityB.setButtonText("noteOn velocity sets noteOff velocity");
    noteOnSetsNoteOffVelocityB.setToggleState(gen->getNoteOnSetsNoteOffVelocity(), dontSendNotification);
    addAndMakeVisible(noteOnSetsNoteOffVelocityB);

#if JUCE_IOS
    tempoMultiplierSlider->addWantsBigOneListener(this);
    A4tuningReferenceFrequencySlider->addWantsBigOneListener(this);
#endif
    
    // Equalizer UI components setup
    // Filter Selection Buttons
    lowCutButton.setName("Low Cut");
    lowCutButton.setButtonText("Low Cut");
    lowCutButton.setTooltip("Select the Low Cut filter to alter its parameters.");
    lowCutButton.addListener(this);
    addAndMakeVisible(lowCutButton);
    
    peak1Button.setName("Peak 1");
    peak1Button.setButtonText("Peak 1");
    peak1Button.setTooltip("Select the first Peak filter to alter its parameters.");
    peak1Button.addListener(this);
    addAndMakeVisible(peak1Button);
    
    peak2Button.setName("Peak 2");
    peak2Button.setButtonText("Peak 2");
    peak2Button.setTooltip("Select the second Peak filter to alter its parameters.");
    peak2Button.addListener(this);
    addAndMakeVisible(peak2Button);
    
    peak3Button.setName("Peak 3");
    peak3Button.setButtonText("Peak 3");
    peak3Button.setTooltip("Select the third Peak filter to alter its parameters.");
    peak3Button.addListener(this);
    addAndMakeVisible(peak3Button);
    
    highCutButton.setName("High Cut");
    highCutButton.setButtonText("High Cut");
    highCutButton.setTooltip("Select the High Cut filter to alter its parameters.");
    highCutButton.addListener(this);
    addAndMakeVisible(highCutButton);
    
    // grab defaults from here
    BKEqualizer* eq = processor.getBKEqualizer();
    
    // Slider setup -- that's a lot of sliders!
    lowCutFreqSlider = std::make_unique<BKSingleSlider>("Frequency", "Low Cut Frequency", 20.f, 20000.f, eq->getLowCutFreq(), 1.f);
    lowCutFreqSlider->setToolTipString("Adjust the frequency of the low cut");
    lowCutFreqSlider->setSkewFactorFromMidPoint(1000.f);
    lowCutFreqSlider->addMyListener(this);
    addAndMakeVisible(*lowCutFreqSlider);
    
    lowCutSlopeSlider = std::make_unique<BKSingleSlider>("Slope (db/Oct)", "Low Cut Slope", 12, 48, eq->getLowCutSlope(), 12);
    lowCutSlopeSlider->setToolTipString("Adjust the slope of the low cut");
    lowCutSlopeSlider->addMyListener(this);
    addAndMakeVisible(*lowCutSlopeSlider);
    
    highCutFreqSlider = std::make_unique<BKSingleSlider>("Frequency", "High Cut Frequency", 20.f, 20000.f, eq->getHighCutFreq(), 1.f);
    highCutFreqSlider->setToolTipString("Adjust the frequency of the high cut");
    highCutFreqSlider->setSkewFactorFromMidPoint(1000.f);
    highCutFreqSlider->addMyListener(this);
    addAndMakeVisible(*highCutFreqSlider);
    
    highCutSlopeSlider = std::make_unique<BKSingleSlider>("Slope (db/Oct)", "High Cut Slope", 12, 48, eq->getHighCutSlope(), 12);
    highCutSlopeSlider->setToolTipString("Adjust the slope of the high cut");
    highCutSlopeSlider->addMyListener(this);
    addAndMakeVisible(*highCutSlopeSlider);
    
    peak1FreqSlider = std::make_unique<BKSingleSlider>("Frequency", "Peak 1 Frequency", 20.f, 20000.f, eq->getPeak1Freq(), 1.5);
    peak1FreqSlider->setToolTipString("Adjust the frequency of the first peak filter");
    peak1FreqSlider->setSkewFactorFromMidPoint(1000.f);
    peak1FreqSlider->addMyListener(this);
    addAndMakeVisible(*peak1FreqSlider);
    
    peak1GainSlider = std::make_unique<BKSingleSlider>("Gain (db)", "Peak 1 Gain", -24.f, 24.f, eq->getPeak1Gain(), 0.5f);
    peak1GainSlider->setToolTipString("Adjust the cut or boost size of the first peak filter");
    peak1GainSlider->addMyListener(this);
    addAndMakeVisible(*peak1GainSlider);
    
    peak1QualitySlider = std::make_unique<BKSingleSlider>("Quality", "Peak 1 Quality", 0.1f, 10.f, eq->getPeak1Quality(), 0.05f);
    peak1QualitySlider->setToolTipString("Adjust the quality of the first peak filter's cut or boost");
    peak1QualitySlider->addMyListener(this);
    addAndMakeVisible(*peak1QualitySlider);
    
    peak2FreqSlider = std::make_unique<BKSingleSlider>("Frequency", "Peak 2 Frequency", 20.f, 20000.f, eq->getPeak2Freq(), 1.5);
    peak2FreqSlider->setToolTipString("Adjust the frequency of the second peak filter");
    peak2FreqSlider->setSkewFactorFromMidPoint(1000.f);
    peak2FreqSlider->addMyListener(this);
    addAndMakeVisible(*peak2FreqSlider);
    
    peak2GainSlider = std::make_unique<BKSingleSlider>("Gain (db)", "Peak 2 Gain", -24.f, 24.f, eq->getPeak2Gain(), 0.5f);
    peak2GainSlider->setToolTipString("Adjust the cut or boost size of the second peak filter");
    peak2GainSlider->addMyListener(this);
    addAndMakeVisible(*peak2GainSlider);
    
    peak2QualitySlider = std::make_unique<BKSingleSlider>("Quality", "Peak 2 Quality", 0.1f, 10.f, eq->getPeak2Quality(), 0.05f);
    peak2QualitySlider->setToolTipString("Adjust the quality of the second peak filter's cut or boost");
    peak2QualitySlider->addMyListener(this);
    addAndMakeVisible(*peak2QualitySlider);
    
    peak3FreqSlider = std::make_unique<BKSingleSlider>("Frequency", "Peak 3 Frequency", 20.f, 20000.f, eq->getPeak3Freq(), 1.5);
    peak3FreqSlider->setToolTipString("Adjust the frequency of the third peak filter");
    peak3FreqSlider->setSkewFactorFromMidPoint(1000.f);
    peak3FreqSlider->addMyListener(this);
    addAndMakeVisible(*peak3FreqSlider);
    
    peak3GainSlider = std::make_unique<BKSingleSlider>("Gain (db)", "Peak 3 Gain", -24.f, 24.f, eq->getPeak3Gain(), 0.5f);
    peak3GainSlider->setToolTipString("Adjust the cut or boost size of the third peak filter");
    peak3GainSlider->addMyListener(this);
    addAndMakeVisible(*peak3GainSlider);
    
    peak3QualitySlider = std::make_unique<BKSingleSlider>("Quality", "Peak 3 Quality", 0.1f, 10.f, eq->getPeak3Quality(), 0.05f);
    peak3QualitySlider->setToolTipString("Adjust the quality of the third peak filter's cut or boost");
    peak3QualitySlider->addMyListener(this);
    addAndMakeVisible(*peak3QualitySlider);
    
    
    update();
    
}

#if JUCE_IOS
void GeneralViewController::iWantTheBigOne(TextEditor* tf, String name)
{
    hideOrShow.setAlwaysOnTop(false);
    bigOne.display(tf, name, getBounds());
}
#endif

GeneralViewController::~GeneralViewController()
{
    setLookAndFeel(nullptr);
}

void GeneralViewController::paint (Graphics& g)
{
    g.fillAll(Colours::black);
}

void GeneralViewController::displayTab(int tab) {
    currentTab = tab;
    invisible();
    displayShared();
    
    if (tab == Tabs::settings) {
        A4tuningReferenceFrequencySlider->setVisible(true);
        tempoMultiplierSlider->setVisible(true);
        invertSustainB.setVisible(true);
        noteOnSetsNoteOffVelocityB.setVisible(true);
    }
    else if (tab == Tabs::equalizer) {
        lowCutButton.setVisible(true);
        highCutButton.setVisible(true);
        peak1Button.setVisible(true);
        peak2Button.setVisible(true);
        peak3Button.setVisible(true);
        displayFilter(currentFilter);
    }
    
}

void GeneralViewController::displayShared() {
    leftArrow.setVisible(true);
    rightArrow.setVisible(true);
}

void GeneralViewController::invisible() {
    A4tuningReferenceFrequencySlider->setVisible(false);
    tempoMultiplierSlider->setVisible(false);
    invertSustainB.setVisible(false);
    noteOnSetsNoteOffVelocityB.setVisible(false);
    lowCutButton.setVisible(false);
    highCutButton.setVisible(false);
    peak1Button.setVisible(false);
    peak2Button.setVisible(false);
    peak3Button.setVisible(false);
    invisibleFilters();
}

void GeneralViewController::resized()
{
    // Position shared UI components
    leftArrow.setBounds(0, getHeight() * 0.4, arrowSpace, arrowSpace);
    rightArrow.setBounds(getRight() - arrowSpace, getHeight() * 0.4, arrowSpace, arrowSpace);
    
    // Position settings UI components
    Rectangle<int> area (getLocalBounds());
    area.reduce(10 * processor.paddingScalarX + 4, 10 * processor.paddingScalarY + 4);
    
    Rectangle<int> leftColumn = area.removeFromLeft(area.getWidth() * 0.5);
    Rectangle<int> comboBoxSlice = leftColumn.removeFromTop(gComponentComboBoxHeight);
    comboBoxSlice.removeFromRight(4 + 2.*gPaddingConst * processor.paddingScalarX);
    comboBoxSlice.removeFromLeft(gXSpacing);
    hideOrShow.setBounds(comboBoxSlice.removeFromLeft(gComponentComboBoxHeight));
    
    Rectangle<int> settingsArea (getLocalBounds());
    settingsArea.removeFromLeft(arrowSpace);
    settingsArea.removeFromRight(arrowSpace);
    settingsArea.reduce(100, 100);
    int numSettings = 4;
    
    A4tuningReferenceFrequencySlider->setBounds(settingsArea.getX(), settingsArea.getY(),
                                                settingsArea.getWidth(),
                                                settingsArea.getHeight() / numSettings);
    tempoMultiplierSlider->setBounds(settingsArea.getX(), A4tuningReferenceFrequencySlider->getBottom(),
                                     settingsArea.getWidth(),
                                     settingsArea.getHeight() / numSettings);
    invertSustainB.setBounds(settingsArea.getX(), tempoMultiplierSlider->getBottom(),
                             settingsArea.getWidth(),
                             settingsArea.getHeight() / numSettings);
    noteOnSetsNoteOffVelocityB.setBounds(settingsArea.getX(), invertSustainB.getBottom(),
                               settingsArea.getWidth(),
                               settingsArea.getHeight() / numSettings);
    
    // Position equalizer UI components
    // Buttons get 1/8 space at top, graph gets 4/8 space in middle, sliders get 3/8 space at bottom
    // Do buttons first
    Rectangle<int> equalizerArea(getLocalBounds());
    equalizerArea.removeFromLeft(arrowSpace + 20);
    equalizerArea.removeFromRight(arrowSpace + 20);
    equalizerArea.removeFromTop(20);
    equalizerArea.removeFromBottom(10);
    Rectangle<int> equalizerButtonsArea(equalizerArea);
    equalizerButtonsArea.removeFromBottom(equalizerArea.getHeight() * 7 / 8);
    
    float spacing = equalizerButtonsArea.getWidth() / 5;
    int padding = spacing - 2; // change the 2 to change padding between buttons
    lowCutButton.setBounds(equalizerButtonsArea.removeFromLeft(spacing).removeFromRight(padding));
    peak1Button.setBounds(equalizerButtonsArea.removeFromLeft(spacing).removeFromRight(padding));
    peak2Button.setBounds(equalizerButtonsArea.removeFromLeft(spacing).removeFromRight(padding));
    peak3Button.setBounds(equalizerButtonsArea.removeFromLeft(spacing).removeFromRight(padding));
    highCutButton.setBounds(equalizerButtonsArea.removeFromLeft(spacing).removeFromRight(padding));
    
    // Graph
    Rectangle<int> equalizerGraphArea(equalizerArea);
    equalizerGraphArea.removeFromTop(equalizerArea.getHeight() * 1 / 8);
    equalizerGraphArea.removeFromBottom(equalizerArea.getHeight() * 3 / 8);
    
    // Sliders
    Rectangle<int> equalizerSlidersArea(equalizerArea);
    equalizerSlidersArea.removeFromTop(equalizerArea.getHeight() * 5 / 8);
    equalizerSlidersArea.reduce(100, 0); // sliders don't need to be so wide
    Rectangle<int> equalizerSlidersAreaCopy(equalizerSlidersArea);
    
    int spacing3 = equalizerSlidersArea.getHeight() / 3;
    Rectangle<int> firstOfThree(equalizerSlidersArea.removeFromTop(spacing3));
    Rectangle<int> secondOfThree(equalizerSlidersArea.removeFromTop(spacing3));
    Rectangle<int> thirdOfThree(equalizerSlidersArea.removeFromTop(spacing3));
    
    peak1FreqSlider->setBounds(firstOfThree);
    peak2FreqSlider->setBounds(firstOfThree);
    peak3FreqSlider->setBounds(firstOfThree);
    
    lowCutSlopeSlider->setBounds(secondOfThree);
    peak1GainSlider->setBounds(secondOfThree);
    peak2GainSlider->setBounds(secondOfThree);
    peak3GainSlider->setBounds(secondOfThree);
    
    peak1QualitySlider->setBounds(thirdOfThree);
    peak2QualitySlider->setBounds(thirdOfThree);
    peak3QualitySlider->setBounds(thirdOfThree);
    
    int spacing2 = equalizerSlidersAreaCopy.getHeight() / 2;
    Rectangle<int> firstOfTwo(equalizerSlidersAreaCopy.removeFromTop(spacing2));
    Rectangle<int> secondOfTwo(equalizerSlidersAreaCopy.removeFromTop(spacing2));
    
    lowCutFreqSlider->setBounds(firstOfTwo);
    highCutFreqSlider->setBounds(firstOfTwo);
    
    lowCutSlopeSlider->setBounds(secondOfTwo);
    highCutSlopeSlider->setBounds(secondOfTwo);
    
    
    // Update view
    displayShared();
    displayTab(currentTab);
    repaint();
}

void GeneralViewController::displayFilter(int filter) {
    invisibleFilters();
    
    currentFilter = filter;
    
    if (filter == Filters::lowCut) {
        lowCutFreqSlider->setVisible(true);
        lowCutSlopeSlider->setVisible(true);
    }
    else if (filter == Filters::peak1) {
        peak1FreqSlider->setVisible(true);
        peak1GainSlider->setVisible(true);
        peak1QualitySlider->setVisible(true);
    }
    else if (filter == Filters::peak2) {
        peak2FreqSlider->setVisible(true);
        peak2GainSlider->setVisible(true);
        peak2QualitySlider->setVisible(true);
    }
    else if (filter == Filters::peak3) {
        peak3FreqSlider->setVisible(true);
        peak3GainSlider->setVisible(true);
        peak3QualitySlider->setVisible(true);
    }
    else if (filter == Filters::highCut) {
        highCutFreqSlider->setVisible(true);
        highCutSlopeSlider->setVisible(true);
    }
}

void GeneralViewController::invisibleFilters() {
    lowCutFreqSlider->setVisible(false);
    lowCutSlopeSlider->setVisible(false);
    peak1FreqSlider->setVisible(false);
    peak1GainSlider->setVisible(false);
    peak1QualitySlider->setVisible(false);
    peak2FreqSlider->setVisible(false);
    peak2GainSlider->setVisible(false);
    peak2QualitySlider->setVisible(false);
    peak3FreqSlider->setVisible(false);
    peak3GainSlider->setVisible(false);
    peak3QualitySlider->setVisible(false);
    highCutFreqSlider->setVisible(false);
    highCutSlopeSlider->setVisible(false);
}


void GeneralViewController::bkTextFieldDidChange(TextEditor& tf)
{

}

void GeneralViewController::update(void)
{
    GeneralSettings::Ptr gen = processor.gallery->getGeneralSettings();
    
    A4tuningReferenceFrequencySlider->setValue(gen->getTuningFundamental(), dontSendNotification);
    tempoMultiplierSlider->setValue(gen->getTempoMultiplier(), dontSendNotification);
    invertSustainB.setToggleState(gen->getInvertSustain(), dontSendNotification);
}

void GeneralViewController::BKSingleSliderValueChanged(BKSingleSlider* slider, String name, double val)
{
    GeneralSettings::Ptr gen = processor.gallery->getGeneralSettings();
    BKEqualizer* eq = processor.getBKEqualizer();

    if(name == A4tuningReferenceFrequencySlider->getName())
    {
        DBG("general tuning fundamental " + String(val));
        gen->setTuningFundamental(val);
    }
    else if(name == tempoMultiplierSlider->getName())
    {
        DBG("general tempo multiplier " + String(val));
        gen->setTempoMultiplier(val);
    }
    else if (slider == lowCutFreqSlider.get()) {
        float lowCutFreq = lowCutFreqSlider->getValue();
        eq->setLowCutFreq(lowCutFreq);
        DBG("Low Cut Freq: " + String(lowCutFreq));
    }
    else if (slider == lowCutSlopeSlider.get()) {
        int lowCutSlope = lowCutSlopeSlider->getValue();
        eq->setLowCutSlope(lowCutSlope);
        DBG("Low Cut Slope: " + String(lowCutSlope));
    }
    else if (slider == peak1FreqSlider.get()) {
        float peak1Freq = peak1FreqSlider->getValue();
        eq->setPeak1Freq(peak1Freq);
        DBG("Peak 1 Frequency: " + String(peak1Freq));
    }
    else if (slider == peak1GainSlider.get()) {
        float peak1Gain = peak1GainSlider->getValue();
        eq->setPeak1Gain(peak1Gain);
        DBG("Peak 1 Gain: " + String(peak1Gain));
    }
    else if (slider == peak1QualitySlider.get()) {
        float peak1Quality = peak1QualitySlider->getValue();
        eq->setPeak1Quality(peak1Quality);
        DBG("Peak 1 Quality: " + String(peak1Quality));
    }
    else if (slider == peak2FreqSlider.get()) {
        float peak2Freq = peak2FreqSlider->getValue();
        eq->setPeak2Freq(peak2Freq);
        DBG("Peak 2 Frequency: " + String(peak2Freq));
    }
    else if (slider == peak2GainSlider.get()) {
        float peak2Gain = peak2GainSlider->getValue();
        eq->setPeak2Gain(peak2Gain);
        DBG("Peak 2 Gain: " + String(peak2Gain));
    }
    else if (slider == peak2QualitySlider.get()) {
        float peak2Quality = peak2QualitySlider->getValue();
        eq->setPeak2Quality(peak2Quality);
        DBG("Peak 2 Quality: " + String(peak2Quality));
    }
    else if (slider == peak3FreqSlider.get()) {
        float peak3Freq = peak3FreqSlider->getValue();
        eq->setPeak3Freq(peak3Freq);
        DBG("Peak 3 Frequency: " + String(peak3Freq));
    }
    else if (slider == peak3GainSlider.get()) {
        float peak3Gain = peak3GainSlider->getValue();
        eq->setPeak3Gain(peak3Gain);
        DBG("Peak 3 Gain: " + String(peak3Gain));
    }
    else if (slider == peak3QualitySlider.get()) {
        float peak3Quality = peak3QualitySlider->getValue();
        eq->setPeak3Quality(peak3Quality);
        DBG("Peak 3 Quality: " + String(peak3Quality));
    }
    else if (slider == highCutFreqSlider.get()) {
        float highCutFreq = highCutFreqSlider->getValue();
        eq->setHighCutFreq(highCutFreq);
        DBG("High Cut Freq: " + String(highCutFreq));
    }
    else if (slider == highCutSlopeSlider.get()) {
        int highCutSlope = highCutSlopeSlider->getValue();
        eq->setHighCutSlope(highCutSlope);
        DBG("High Cut Slope: " + String(highCutSlope));
    }
}

void GeneralViewController::bkButtonClicked (Button* b)
{
    GeneralSettings::Ptr gen = processor.gallery->getGeneralSettings();
    
    if (b == &hideOrShow)
    {
        processor.updateState->setCurrentDisplay(DisplayNil);
        
    }
    else if (b == &invertSustainB)
    {
        //bool inversion =  (bool) b->getToggleStateValue().toString().getIntValue();
        bool inversion =  b->getToggleState();
        DBG("invert sustain button = " + String((int)inversion));
        processor.setSustainInversion(inversion);
        gen->setInvertSustain(inversion);
    }
    else if (b == &noteOnSetsNoteOffVelocityB)
    {
        bool newstate =  b->getToggleState();
        DBG("invert noteOnSetsNoteOffVelocity = " + String((int)newstate));
        gen->setNoteOnSetsNoteOffVelocity(newstate);
    }
    else if (b == &lowCutButton) {
        displayFilter(Filters::lowCut);
    }
    else if (b == &peak1Button) {
        displayFilter(Filters::peak1);
    }
    else if (b == &peak2Button) {
        displayFilter(Filters::peak2);
    }
    else if (b == &peak3Button) {
        displayFilter(Filters::peak3);
    }
    else if (b == &highCutButton) {
        displayFilter(Filters::highCut);
    }
    else if (b == &leftArrow) {
        arrowPressed(LeftArrow);
        displayTab(currentTab);
    }
    else if (b == &rightArrow) {
        arrowPressed(RightArrow);
        displayTab(currentTab);
    }
}



//==============================================================================
ModdableViewController::ModdableViewController(BKAudioProcessor& p, BKItemGraph* theGraph):
BKViewController(p, theGraph, 1)
{
    
    setLookAndFeel(&buttonsAndMenusLAF);
    
    timeSlider = std::make_unique<BKSingleSlider>("mod time (ms)", "mod time (ms)", 0, 2000, 0, 1);
    timeSlider->setToolTipString("how long it will take to ramp to this mod value");
    timeSlider->setJustifyRight(false);
    timeSlider->addMyListener(this);
    addAndMakeVisible(*timeSlider);
    
    incSlider = std::make_unique<BKSingleSlider>("mod increment", "mod increment", -2.0f, 2.0f, 0.0, 0.001);
    incSlider->setToolTipString("how much to increment the mod value on each time the mod is triggered");
    incSlider->setJustifyRight(false);
    incSlider->addMyListener(this);
    addAndMakeVisible(*incSlider);
    
    maxIncSlider = std::make_unique<BKSingleSlider>("max times to increment", "max times to increment", 0, 10, 0, 1, "no max");
    maxIncSlider->setToolTipString("how many times maximum to increment the mod value");
    maxIncSlider->setJustifyRight(false);
    maxIncSlider->addMyListener(this);
    addAndMakeVisible(*maxIncSlider);

#if JUCE_IOS
    timeSlider->addWantsBigOneListener(this);
    incSlider->addWantsBigOneListener(this);
    maxIncSlider->addWantsBigOneListener(this);
#endif
    
    startTimerHz(10);
}

#if JUCE_IOS
void ModdableViewController::iWantTheBigOne(TextEditor* tf, String name)
{
    hideOrShow.setAlwaysOnTop(false);
    bigOne.display(tf, name, getBounds());
}
#endif

ModdableViewController::~ModdableViewController()
{
    stopTimer();
    setLookAndFeel(nullptr);
}

void ModdableViewController::paint (Graphics& g)
{
    g.fillAll(Colours::black.withAlpha(0.6f));
    
    Rectangle<int> area (getLocalBounds());
    area.reduce(area.getWidth() * 0.2f, area.getHeight() * 0.2f);
    
    g.setColour(Colours::black);
    g.fillRect(area);
    
    g.setColour(Colours::antiquewhite.withAlpha(0.6f));
    g.drawRect(area, 1);
}

void ModdableViewController::resized()
{
    Rectangle<int> area (getLocalBounds());
    area.reduce(area.getWidth() * 0.2f, area.getHeight() * 0.2f);
    
    Rectangle<int> hideOrShowBounds = area.reduced(10 * processor.paddingScalarX + 4, 10 * processor.paddingScalarY + 4).removeFromLeft(area.getWidth() * 0.5);
    hideOrShowBounds = hideOrShowBounds.removeFromTop(gComponentComboBoxHeight);
    hideOrShowBounds.removeFromRight(4 + 2.0f * gPaddingConst * processor .paddingScalarX);
    hideOrShowBounds.removeFromLeft(gXSpacing);
    hideOrShow.setBounds(hideOrShowBounds.removeFromLeft(gComponentComboBoxHeight));
    
    area.reduce(area.getWidth() * 0.2f, area.getHeight() * 0.2f);
    
    int h = area.getHeight();
    
    timeSlider->setBounds(area.removeFromTop(h * 0.3f));
    incSlider->setBounds(area.removeFromTop(h * 0.3f));
    maxIncSlider->setBounds(area);
}


void ModdableViewController::bkTextFieldDidChange(TextEditor& tf)
{
    
}

void ModdableViewController::update(void)
{
    ModdableBase* mod = getCurrentModdable();
    if (mod == nullptr) return;
    
    timeSlider->setValue(mod->getTime(), dontSendNotification);
    incSlider->setValue(mod->getInc(), dontSendNotification);
    maxIncSlider->setValue(mod->getMaxNumberOfInc(), dontSendNotification);
}

void ModdableViewController::BKSingleSliderValueChanged(BKSingleSlider* slider, String name, double val)
{
    ModdableBase* mod = getCurrentModdable();
    if (mod == nullptr) return;
    
    if (name == timeSlider->getName())
    {
        mod->setTime(int(val));
    }
    if (name == incSlider->getName())
    {
        mod->setInc(val);
    }
    if (name == maxIncSlider->getName())
    {
        mod->setMaxNumberOfInc(val);
    }
}

void ModdableViewController::bkButtonClicked (Button* b)
{
    if (b == &hideOrShow)
    {
        processor.updateState->setCurrentDisplay(processor.updateState->previousDisplay);
    }
}

ModdableBase* ModdableViewController::getCurrentModdable()
{
    // Might be a good idea to standardize the naming/organization of preparations parameters
    // and sliders, etc so this could be streamlined. Or at least use constants for component names
    if (processor.updateState->previousDisplay == DisplayDirectMod)
    {
        DirectModification::Ptr mod = processor.gallery->getDirectModification(processor.updateState->currentModDirectId);
        
        if (processor.updateState->currentModdableIdentifier == cDirectGain)
            return &mod->dGain;
        else if (processor.updateState->currentModdableIdentifier == cDirectResonanceGain)
            return &mod->dResonanceGain;
        else if (processor.updateState->currentModdableIdentifier == cDirectHammerGain)
            return &mod->dHammerGain;
        else if (processor.updateState->currentModdableIdentifier == cDirectBlendronicGain)
            return &mod->dBlendronicGain;
    }
    else if (processor.updateState->previousDisplay == DisplayNostalgicMod)
    {
        NostalgicModification::Ptr mod = processor.gallery->getNostalgicModification(processor.updateState->currentModNostalgicId);
        
        if (processor.updateState->currentModdableIdentifier == cNostalgicGain)
            return &mod->nGain;
        else if (processor.updateState->currentModdableIdentifier == cNostalgicBlendronicGain)
            return &mod->nBlendronicGain;
        else if (processor.updateState->currentModdableIdentifier == cNostalgicLengthMultiplier)
            return &mod->nLengthMultiplier;
        else if (processor.updateState->currentModdableIdentifier == cNostalgicBeatsToSkip)
            return &mod->nBeatsToSkip;
        else if (processor.updateState->currentModdableIdentifier == cNostalgicClusterMin)
            return &mod->clusterMin;
        else if (processor.updateState->currentModdableIdentifier == cNostalgicClusterThreshold)
            return &mod->clusterThreshold;
    }
    else if (processor.updateState->previousDisplay == DisplaySynchronicMod)
    {
        SynchronicModification::Ptr mod = processor.gallery->getSynchronicModification(processor.updateState->currentModSynchronicId);
        
        if (processor.updateState->currentModdableIdentifier == cSynchronicGain)
            return &mod->sGain;
        else if (processor.updateState->currentModdableIdentifier == cSynchronicBlendronicGain)
            return &mod->sBlendronicGain;
        else if (processor.updateState->currentModdableIdentifier == cSynchronicNumBeats)
            return &mod->sNumBeats;
        else if (processor.updateState->currentModdableIdentifier == cSynchronicClusterThresh)
            return &mod->sClusterThresh;
        else if (processor.updateState->currentModdableIdentifier == cSynchronicClusterCap)
            return &mod->sClusterCap;
        else if (processor.updateState->currentModdableIdentifier == cSynchronicNumClusters)
            return &mod->numClusters;
    }
    else if (processor.updateState->previousDisplay == DisplayBlendronicMod)
    {
        BlendronicModification::Ptr mod = processor.gallery->getBlendronicModification(processor.updateState->currentModBlendronicId);
        
        if (processor.updateState->currentModdableIdentifier == cBlendronicOutGain)
            return &mod->outGain;
    }
    else if (processor.updateState->previousDisplay == DisplayTuningMod)
    {
        TuningModification::Ptr mod = processor.gallery->getTuningModification(processor.updateState->currentModTuningId);
        
        if (processor.updateState->currentModdableIdentifier == cSpringTuningRate)
            return &mod->getSpringTuning()->rate;
        else if (processor.updateState->currentModdableIdentifier == cSpringTuningDrag)
            return &mod->getSpringTuning()->drag;
        else if (processor.updateState->currentModdableIdentifier == cSpringTuningStiffness)
            return &mod->getSpringTuning()->stiffness;
        else if (processor.updateState->currentModdableIdentifier == cSpringTuningIntervalStiffness)
            return &mod->getSpringTuning()->intervalStiffness;
        else if (processor.updateState->currentModdableIdentifier == cSpringTuningTetherWeight)
            return &mod->getSpringTuning()->tetherWeightGlobal;
        else if (processor.updateState->currentModdableIdentifier == cSpringTuningTetherWeightSecondary)
            return &mod->getSpringTuning()->tetherWeightSecondaryGlobal;
        else if (processor.updateState->currentModdableIdentifier == cTuningAdaptiveClusterThresh)
            return &mod->tAdaptiveClusterThresh;
        else if (processor.updateState->currentModdableIdentifier == cTuningAdaptiveHistory)
            return &mod->tAdaptiveHistory;
        else if (processor.updateState->currentModdableIdentifier == cTuningToneSemitoneWidth)
            return &mod->nToneSemitoneWidth;
        else if (processor.updateState->currentModdableIdentifier == cTuningFundamentalOffset)
            return &mod->tFundamentalOffset;
    }
    else if (processor.updateState->previousDisplay == DisplayTempoMod)
    {
        TempoModification::Ptr mod = processor.gallery->getTempoModification(processor.updateState->currentModTempoId);

        if (processor.updateState->currentModdableIdentifier == cTempoTempo)
            return &mod->sTempo;
        else if (processor.updateState->currentModdableIdentifier == cTempoSubdivisions)
            return &mod->subdivisions;
        else if (processor.updateState->currentModdableIdentifier == cTempoAT1History)
            return &mod->at1History;
        else if (processor.updateState->currentModdableIdentifier == cTempoAT1Subdivisions)
            return &mod->at1Subdivisions;
    }
    
    return nullptr;
}


void ModdableViewController::timerCallback()
{
//    ModdableBase* mod = getCurrentModdable();
}
