/*
  ==============================================================================

    DirectViewController2.cpp
    Created: 7 Jun 2017 1:42:55pm
    Author:  Daniel Trueman

  ==============================================================================
*/

#include "DirectViewController2.h"

DirectViewController2::DirectViewController2(BKAudioProcessor& p, BKItemGraph* theGraph):
processor(p),
theGraph(theGraph)
{
    setLookAndFeel(&buttonsAndMenusLAF);
    
    iconImageComponent.setImage(ImageCache::getFromMemory(BinaryData::direct_icon_png, BinaryData::direct_icon_pngSize));
    iconImageComponent.setImagePlacement(RectanglePlacement(juce::RectanglePlacement::stretchToFit));
    iconImageComponent.setAlpha(0.095);
    addAndMakeVisible(iconImageComponent);
    
    selectCB.setName("Direct");
    selectCB.addSeparator();
    selectCB.addListener(this);
    selectCB.setSelectedItemIndex(0);
    selectCB.BKSetJustificationType(juce::Justification::centredRight);
    selectCB.addMyListener(this);
    fillSelectCB();
    addAndMakeVisible(selectCB);
    
    transpositionSlider = new BKStackedSlider("transpositions", -12, 12, -12, 12, 0, 0.01);
    transpositionSlider->addMyListener(this);
    addAndMakeVisible(transpositionSlider);

    gainSlider = new BKSingleSlider("gain", 0, 10, 1, 0.01);
    gainSlider->setSkewFactorFromMidPoint(1.);
    gainSlider->addMyListener(this);
    addAndMakeVisible(gainSlider);
    
    resonanceGainSlider = new BKSingleSlider("resonance gain", 0, 10, 1, 0.01);
    resonanceGainSlider->setSkewFactorFromMidPoint(1.);
    resonanceGainSlider->addMyListener(this);
    addAndMakeVisible(resonanceGainSlider);
    
    hammerGainSlider = new BKSingleSlider("hammer gain", 0, 10, 1, 0.01);
    hammerGainSlider->setSkewFactorFromMidPoint(1.);
    hammerGainSlider->addMyListener(this);
    addAndMakeVisible(hammerGainSlider);
    
    addAndMakeVisible(hideOrShow);
    hideOrShow.setName("hideOrShow");
    hideOrShow.addListener(this);
    hideOrShow.setButtonText(" X ");
    
}

void DirectViewController2::paint (Graphics& g)
{
    g.fillAll(Colours::black);
}

void DirectViewController2::resized()
{
    Rectangle<int> area (getLocalBounds());
    
    float paddingScalarX = (float)(getTopLevelComponent()->getWidth() - gMainComponentMinWidth) / (gMainComponentWidth - gMainComponentMinWidth);
    float paddingScalarY = (float)(getTopLevelComponent()->getHeight() - gMainComponentMinHeight) / (gMainComponentHeight - gMainComponentMinHeight);
    
    iconImageComponent.setBounds(area);
    area.reduce(10 * paddingScalarX + 4, 10 * paddingScalarY + 4);
    
    Rectangle<int> leftColumn = area.removeFromLeft(area.getWidth() * 0.5);
    Rectangle<int> comboBoxSlice = leftColumn.removeFromTop(gComponentComboBoxHeight);
    comboBoxSlice.removeFromRight(4 + 2.*gPaddingConst * paddingScalarX);
    hideOrShow.setBounds(comboBoxSlice.removeFromLeft(gComponentComboBoxHeight));
    selectCB.setBounds(comboBoxSlice.removeFromRight(comboBoxSlice.getWidth() / 2.));
    
    /* *** above here should be generic to all prep layouts *** */
    /* ***    below here will be specific to each prep      *** */
    
    Rectangle<int> modeSlice = area.removeFromTop(gComponentComboBoxHeight);
    modeSlice.reduce(4 + 2.*gPaddingConst * paddingScalarX, 0);
    
    Rectangle<int> sliderSlice = area;
    sliderSlice.removeFromLeft(gXSpacing + 2.*gPaddingConst * paddingScalarX);
    sliderSlice.removeFromRight(gXSpacing);
    /*
    sliderSlice.reduce(4 + 2.*gPaddingConst * paddingScalarX,
                       4 + 2.*gPaddingConst * paddingScalarY);
     */
    
    int nextCenter = sliderSlice.getY() + sliderSlice.getHeight() / 5.;
    resonanceGainSlider->setBounds(sliderSlice.getX(),
                                      nextCenter - gComponentSingleSliderHeight/2 + 8,
                                      sliderSlice.getWidth(),
                                      gComponentSingleSliderHeight);
    
    nextCenter = sliderSlice.getY() + sliderSlice.getHeight() / 2.;
    hammerGainSlider->setBounds(sliderSlice.getX(),
                                   nextCenter - gComponentSingleSliderHeight/2 + 8,
                                   sliderSlice.getWidth(),
                                   gComponentSingleSliderHeight);
    
    nextCenter = sliderSlice.getY() + 4. * sliderSlice.getHeight() / 5.;
    gainSlider->setBounds(sliderSlice.getX(),
                          nextCenter - gComponentSingleSliderHeight/2 + 4,
                          sliderSlice.getWidth(),
                          gComponentSingleSliderHeight);
    
    //leftColumn.reduce(4, 0);
    leftColumn.removeFromRight(gXSpacing + 2.*gPaddingConst * paddingScalarX);
    leftColumn.removeFromLeft(gXSpacing);
    transpositionSlider->setBounds(leftColumn.getX(),
                                   resonanceGainSlider->getY(),
                                   leftColumn.getWidth(),
                                   gComponentStackedSliderHeight + paddingScalarY * 30);
    
    /*
    Rectangle<int> area (getLocalBounds());
    
    Rectangle<int> displayRow = area.removeFromBottom(area.getHeight() * 0.5);
    
    Rectangle<int> leftColumn = area.removeFromLeft(area.getWidth() * 0.5);
    selectCB.setBounds(leftColumn.removeFromTop(20));
    transpositionSlider->setBounds(leftColumn.removeFromTop(40));
    
    resonanceGainSlider->setBounds(area.removeFromTop(40));
    hammerGainSlider->setBounds(area.removeFromTop(40));
    gainSlider->setBounds(area.removeFromBottom(40));
     */
    
}

void DirectViewController2::updateFields(void)
{
    fillSelectCB();
    
    DirectPreparation::Ptr prep = processor.gallery->getActiveDirectPreparation(processor.updateState->currentDirectId);

    selectCB.setSelectedItemIndex(processor.updateState->currentDirectId, dontSendNotification);
    
    transpositionSlider->setValue(prep->getTransposition(), dontSendNotification);
    resonanceGainSlider->setValue(prep->getResonanceGain(), dontSendNotification);
    hammerGainSlider->setValue(prep->getHammerGain(), dontSendNotification);
    gainSlider->setValue(prep->getGain(), dontSendNotification);
    
}

void DirectViewController2::bkMessageReceived (const String& message)
{
    if (message == "direct/update")
    {
        
        updateFields();
    }
}

void DirectViewController2::bkComboBoxDidChange (ComboBox* box)
{
    String name = box->getName();
    
    if (name == "Direct")
    {
        processor.updateState->currentDirectId = box->getSelectedItemIndex();
        
        processor.updateState->idDidChange = true;
        
        if (processor.updateState->currentDirectId == selectCB.getNumItems()-1)
        {
            processor.gallery->addDirect();
            
            fillSelectCB();
        }
        
        //updateFields(sendNotification);
        updateFields();
    }
}

void DirectViewController2::BKEditableComboBoxChanged(String name, BKEditableComboBox* cb)
{
    processor.gallery->getDirect(processor.updateState->currentDirectId)->setName(name);
    
    //int selected = selectCB.getSelectedId();
    //if (selected != selectCB.getNumItems()) selectCB.changeItemText(selected, name);
    //selectCB.setSelectedId(selected, dontSendNotification );
}


void DirectViewController2::BKSingleSliderValueChanged(String name, double val)
{
    DirectPreparation::Ptr prep = processor.gallery->getStaticDirectPreparation(processor.updateState->currentDirectId);
    DirectPreparation::Ptr active = processor.gallery->getActiveDirectPreparation(processor.updateState->currentDirectId);
    
    if(name == "resonance gain")
    {
        //DBG("note length multiplier " + String(val));
        prep->setResonanceGain(val);
        active->setResonanceGain(val);
    }
    else if(name == "hammer gain")
    {
        //DBG("beats to skip " + String(val));
        prep->setHammerGain(val);
        active->setHammerGain(val);
    }
    else if(name == "gain")
    {
        //DBG("gain " + String(val));
        prep->setGain(val);
        active->setGain(val);
    }
}

void DirectViewController2::BKStackedSliderValueChanged(String name, Array<float> val)
{
    DirectPreparation::Ptr prep = processor.gallery->getStaticDirectPreparation(processor.updateState->currentDirectId);
    DirectPreparation::Ptr active = processor.gallery->getActiveDirectPreparation(processor.updateState->currentDirectId);
    
    prep->setTransposition(val);
    active->setTransposition(val);
}

void DirectViewController2::bkButtonClicked (Button* b)
{
    if (b == &hideOrShow)
    {
        processor.updateState->setCurrentDisplay(DisplayNil);
    }
}

void DirectViewController2::fillSelectCB(void)
{
    // Direct menu
    Direct::PtrArr newpreps = processor.gallery->getAllDirect();
    
    selectCB.clear(dontSendNotification);
    for (int i = 0; i < newpreps.size(); i++)
    {
        String name = newpreps[i]->getName();
        if (name != String::empty)  selectCB.addItem(name, i+1);
        else                        selectCB.addItem(String(i+1), i+1);
    }
    
    selectCB.addItem("New direct...", newpreps.size()+1);
    
    selectCB.setSelectedItemIndex(processor.updateState->currentDirectId, NotificationType::dontSendNotification);
    
}
