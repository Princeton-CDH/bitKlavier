/*
  ==============================================================================

    NostalgicViewController2.cpp
    Created: 24 May 2017 7:13:23pm
    Author:  Daniel Trueman

  ==============================================================================
*/

#include "NostalgicViewController2.h"

NostalgicViewController2::NostalgicViewController2(BKAudioProcessor& p, BKItemGraph* theGraph):
processor(p),
theGraph(theGraph)
{
    
    setLookAndFeel(&buttonsAndMenusLAF);
    
    iconImageComponent.setImage(ImageCache::getFromMemory(BinaryData::nostalgic_icon_png, BinaryData::nostalgic_icon_pngSize));
    iconImageComponent.setImagePlacement(RectanglePlacement(juce::RectanglePlacement::stretchToFit));
    iconImageComponent.setAlpha(0.095);
    addAndMakeVisible(iconImageComponent);
    
    selectCB.setName("Nostalgic");
    selectCB.addSeparator();
    selectCB.addListener(this);
    selectCB.setSelectedItemIndex(0);
    selectCB.BKSetJustificationType(juce::Justification::centredRight);
    selectCB.addMyListener(this);
    fillSelectCB();
    addAndMakeVisible(selectCB);
    
    lengthModeSelectCB.setName("Length Mode");
    lengthModeSelectCB.addSeparator();
    lengthModeSelectCB.addListener(this);
    lengthModeSelectCB.setSelectedItemIndex(0);
    fillModeSelectCB();
    addAndMakeVisible(lengthModeSelectCB);
    
    transpositionSlider = new BKStackedSlider("transpositions", -12, 12, -12, 12, 0, 0.01);
    transpositionSlider->addMyListener(this);
    addAndMakeVisible(transpositionSlider);
    
    lengthMultiplierSlider = new BKSingleSlider("note length multiplier", 0, 10, 1, 0.01);
    lengthMultiplierSlider->setSkewFactorFromMidPoint(1.);
    lengthMultiplierSlider->addMyListener(this);
    addAndMakeVisible(lengthMultiplierSlider);
    
    beatsToSkipSlider = new BKSingleSlider("beats to skip", 0, 10, 0, 1);
    beatsToSkipSlider->addMyListener(this);
    addAndMakeVisible(beatsToSkipSlider);
    beatsToSkipSlider->setVisible(false);
    
    gainSlider = new BKSingleSlider("gain", 0, 10, 1, 0.01);
    gainSlider->setSkewFactorFromMidPoint(1.);
    gainSlider->addMyListener(this);
    addAndMakeVisible(gainSlider);
    
    addAndMakeVisible(hideOrShow);
    hideOrShow.setName("hideOrShow");
    hideOrShow.addListener(this);
    hideOrShow.setButtonText(" X ");
    
    nDisplaySlider.addMyListener(this);
    addAndMakeVisible(nDisplaySlider);
    
    startTimer(20);
}

void NostalgicViewController2::paint (Graphics& g)
{
    g.fillAll(Colours::black);
}

void NostalgicViewController2::resized()
{
    Rectangle<int> area (getLocalBounds());
    
    float paddingScalarX = (float)(getTopLevelComponent()->getWidth() - gMainComponentMinWidth) / (gMainComponentWidth - gMainComponentMinWidth);
    float paddingScalarY = (float)(getTopLevelComponent()->getHeight() - gMainComponentMinHeight) / (gMainComponentHeight - gMainComponentMinHeight);
    
    iconImageComponent.setBounds(area);
    area.reduce(10 * paddingScalarX + 4, 10 * paddingScalarY + 4);
    
    Rectangle<int> nDisplayRow = area.removeFromBottom(100 + 80 * paddingScalarY);
    nDisplayRow.reduce(4, 4);
    nDisplaySlider.setBounds(nDisplayRow);
    
    Rectangle<int> leftColumn = area.removeFromLeft(area.getWidth() * 0.5);
    Rectangle<int> comboBoxSlice = leftColumn.removeFromTop(gComponentComboBoxHeight);
    comboBoxSlice.removeFromRight(4 + 2.*gPaddingConst * paddingScalarX);
    hideOrShow.setBounds(comboBoxSlice.removeFromLeft(gComponentComboBoxHeight));
    selectCB.setBounds(comboBoxSlice.removeFromRight(comboBoxSlice.getWidth() / 2.));
    
    /* *** above here should be generic to all prep layouts *** */
    /* ***    below here will be specific to each prep      *** */
    
    Rectangle<int> modeSlice = area.removeFromTop(gComponentComboBoxHeight);
    modeSlice.reduce(4 + 2.*gPaddingConst * paddingScalarX, 0);
    lengthModeSelectCB.setBounds(modeSlice.removeFromLeft(modeSlice.getWidth() / 2.));
    
    Rectangle<int> sliderSlice = area;
    sliderSlice.reduce(4 + 2.*gPaddingConst * paddingScalarX,
                       4 + 2.*gPaddingConst * paddingScalarY);
    
    int nextCenter = sliderSlice.getY() + sliderSlice.getHeight() / 4.;
    lengthMultiplierSlider->setBounds(sliderSlice.getX(),
                                      nextCenter - gComponentSingleSliderHeight/2 + 8,
                                      sliderSlice.getWidth(),
                                      gComponentSingleSliderHeight);
    beatsToSkipSlider->setBounds(lengthMultiplierSlider->getBounds());
    
    nextCenter = sliderSlice.getY() + 3. * sliderSlice.getHeight() / 4.;
    gainSlider->setBounds(sliderSlice.getX(),
                          nextCenter - gComponentSingleSliderHeight/2 + 4,
                          sliderSlice.getWidth(),
                          gComponentSingleSliderHeight);

    nextCenter = leftColumn.getY() + leftColumn.getHeight() / 2.;
    leftColumn.reduce(4, 0);
    
    transpositionSlider->setBounds(leftColumn.getX(),
                                   lengthMultiplierSlider->getY(),
                                   leftColumn.getWidth(),
                                   gComponentStackedSliderHeight + paddingScalarY * 30);
}

void NostalgicViewController2::BKWaveDistanceUndertowSliderValueChanged(String name, double wavedist, double undertow)
{
    
    NostalgicPreparation::Ptr prep = processor.gallery->getStaticNostalgicPreparation(processor.updateState->currentNostalgicId);
    NostalgicPreparation::Ptr active = processor.gallery->getActiveNostalgicPreparation(processor.updateState->currentNostalgicId);
    
    prep    ->setWaveDistance(wavedist);
    active  ->setWaveDistance(wavedist);
    prep    ->setUndertow(undertow);
    active  ->setUndertow(undertow);
    
}

void NostalgicViewController2::BKEditableComboBoxChanged(String name, BKEditableComboBox* cb)
{
    processor.gallery->getNostalgic(processor.updateState->currentNostalgicId)->setName(name);
    
    //int selected = selectCB.getSelectedId();
    //if (selected != selectCB.getNumItems()) selectCB.changeItemText(selected, name);
    //selectCB.setSelectedId(selected, dontSendNotification );
}

void NostalgicViewController2::updateFields(void)
{
    fillSelectCB();
    
    NostalgicPreparation::Ptr prep = processor.gallery->getActiveNostalgicPreparation(processor.updateState->currentNostalgicId);
    
    nDisplaySlider.setWaveDistance(prep->getWavedistance(), dontSendNotification);
    nDisplaySlider.setUndertow(prep->getUndertow(), dontSendNotification);
    
    selectCB.setSelectedItemIndex(processor.updateState->currentNostalgicId, dontSendNotification);
    lengthModeSelectCB.setSelectedItemIndex(prep->getMode(), dontSendNotification);
    
    transpositionSlider->setValue(prep->getTransposition(), dontSendNotification);
    lengthMultiplierSlider->setValue(prep->getLengthMultiplier(), dontSendNotification);
    beatsToSkipSlider->setValue(prep->getBeatsToSkip(), dontSendNotification);
    gainSlider->setValue(prep->getGain(), dontSendNotification);
    
    if(prep->getMode() == NoteLengthSync)
    {
        lengthMultiplierSlider->setVisible(true);
        beatsToSkipSlider->setVisible(false);
    }
    else
    {
        lengthMultiplierSlider->setVisible(false);
        beatsToSkipSlider->setVisible(true);
    }

}

void NostalgicViewController2::bkMessageReceived (const String& message)
{
    if (message == "nostalgic/update")
    {
        
        updateFields();
    }
}

void NostalgicViewController2::bkComboBoxDidChange (ComboBox* box)
{
    String name = box->getName();
    
    if (name == "Nostalgic")
    {
        processor.updateState->currentNostalgicId = box->getSelectedItemIndex();
        
        processor.updateState->idDidChange = true;
        
        if (processor.updateState->currentNostalgicId == selectCB.getNumItems()-1)
        {
            processor.gallery->addNostalgic();
            
            fillSelectCB();
        }
        
        //updateFields(sendNotification);
        updateFields();
        
    }
    else if (name == "Length Mode")
    {
        NostalgicPreparation::Ptr prep = processor.gallery->getStaticNostalgicPreparation(processor.updateState->currentNostalgicId);
        NostalgicPreparation::Ptr active = processor.gallery->getActiveNostalgicPreparation(processor.updateState->currentNostalgicId);
        
        prep    ->setMode((NostalgicSyncMode) box->getSelectedItemIndex());
        active  ->setMode((NostalgicSyncMode) box->getSelectedItemIndex());
        
        if(prep->getMode() == NoteLengthSync)
        {
            lengthMultiplierSlider->setVisible(true);
            beatsToSkipSlider->setVisible(false);
        }
        else
        {
            lengthMultiplierSlider->setVisible(false);
            beatsToSkipSlider->setVisible(true);
        }

    }
}

void NostalgicViewController2::BKSingleSliderValueChanged(String name, double val)
{
    NostalgicPreparation::Ptr prep = processor.gallery->getStaticNostalgicPreparation(processor.updateState->currentNostalgicId);
    NostalgicPreparation::Ptr active = processor.gallery->getActiveNostalgicPreparation(processor.updateState->currentNostalgicId);
    
    if(name == "note length multiplier")
    {
        //DBG("note length multiplier " + String(val));
        prep->setLengthMultiplier(val);
        active->setLengthMultiplier(val);
    }
    else if(name == "beats to skip")
    {
        //DBG("beats to skip " + String(val));
        prep->setBeatsToSkip(val);
        active->setBeatsToSkip(val);
    }
    else if(name == "gain")
    {
        //DBG("gain " + String(val));
        prep->setGain(val);
        active->setGain(val);
    }
}

void NostalgicViewController2::BKStackedSliderValueChanged(String name, Array<float> val)
{
    NostalgicPreparation::Ptr prep = processor.gallery->getStaticNostalgicPreparation(processor.updateState->currentNostalgicId);
    NostalgicPreparation::Ptr active = processor.gallery->getActiveNostalgicPreparation(processor.updateState->currentNostalgicId);

    prep->setTransposition(val);
    active->setTransposition(val);
}

void NostalgicViewController2::bkButtonClicked (Button* b)
{
    if (b == &hideOrShow)
    {
        processor.updateState->setCurrentDisplay(DisplayNil);
    }
}

void NostalgicViewController2::fillSelectCB(void)
{
    // Direct menu
    Nostalgic::PtrArr newpreps = processor.gallery->getAllNostalgic();
    
    selectCB.clear(dontSendNotification);
    for (int i = 0; i < newpreps.size(); i++)
    {
        String name = newpreps[i]->getName();
        if (name != String::empty)  selectCB.addItem(name, i+1);
        else                        selectCB.addItem(String(i+1), i+1);
    }
    
    selectCB.addItem("New nostalgic...", newpreps.size()+1);
    
    selectCB.setSelectedItemIndex(processor.updateState->currentNostalgicId, NotificationType::dontSendNotification);
    
}

void NostalgicViewController2::fillModeSelectCB(void)
{
    
    lengthModeSelectCB.clear(dontSendNotification);
    for (int i = 0; i < cNostalgicSyncModes.size(); i++)
    {
        String name = cNostalgicSyncModes[i];
        if (name != String::empty)  lengthModeSelectCB.addItem(name, i+1);
        else                        lengthModeSelectCB.addItem(String(i+1), i+1);
    }
    
    lengthModeSelectCB.setSelectedItemIndex(0, NotificationType::dontSendNotification);
}

void NostalgicViewController2::timerCallback()
{
    NostalgicProcessor::Ptr nProcessor = processor.gallery->getNostalgicProcessor(processor.updateState->currentNostalgicId);
    
    Array<int> currentPlayPositions = nProcessor->getPlayPositions();
    Array<int> currentUndertowPositions = nProcessor->getUndertowPositions();
    currentPlayPositions.addArray(currentUndertowPositions);
    
    nDisplaySlider.updateSliderPositions(currentPlayPositions);
    
}
