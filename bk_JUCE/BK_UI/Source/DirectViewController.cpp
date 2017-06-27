/*
  ==============================================================================

    DirectViewController.cpp
    Created: 7 Jun 2017 1:42:55pm
    Author:  Daniel Trueman

  ==============================================================================
*/

#include "DirectViewController.h"

DirectViewController::DirectViewController(BKAudioProcessor& p, BKItemGraph* theGraph):
BKViewController(p, theGraph)
{
    setLookAndFeel(&buttonsAndMenusLAF);
    
    iconImageComponent.setImage(ImageCache::getFromMemory(BinaryData::direct_icon_png, BinaryData::direct_icon_pngSize));
    iconImageComponent.setImagePlacement(RectanglePlacement(juce::RectanglePlacement::stretchToFit));
    iconImageComponent.setAlpha(0.095);
    addAndMakeVisible(iconImageComponent);
    
    selectCB.setName("Direct");
    selectCB.addSeparator();
    selectCB.setSelectedItemIndex(0);
    addAndMakeVisible(selectCB);
    
    transpositionSlider = new BKStackedSlider("transpositions", -12, 12, -12, 12, 0, 0.01);
    addAndMakeVisible(transpositionSlider);
    
    gainSlider = new BKSingleSlider("gain", 0, 10, 1, 0.01);
    gainSlider->setSkewFactorFromMidPoint(1.);
    gainSlider->setJustifyRight(false);
    addAndMakeVisible(gainSlider);
    
    resonanceGainSlider = new BKSingleSlider("resonance gain", 0, 10, 1, 0.01);
    resonanceGainSlider->setSkewFactorFromMidPoint(1.);
    resonanceGainSlider->setJustifyRight(false);
    addAndMakeVisible(resonanceGainSlider);
    
    hammerGainSlider = new BKSingleSlider("hammer gain", 0, 10, 1, 0.01);
    hammerGainSlider->setSkewFactorFromMidPoint(1.);
    hammerGainSlider->setJustifyRight(false);
    addAndMakeVisible(hammerGainSlider);
    
    addAndMakeVisible(hideOrShow);
    hideOrShow.setName("hideOrShow");
    hideOrShow.setButtonText(" X ");
    
}

void DirectViewController::paint (Graphics& g)
{
    g.fillAll(Colours::black);
}

void DirectViewController::resized()
{
    Rectangle<int> area (getLocalBounds());
    
    float paddingScalarX = (float)(getTopLevelComponent()->getWidth() - gMainComponentMinWidth) / (gMainComponentWidth - gMainComponentMinWidth);
    float paddingScalarY = (float)(getTopLevelComponent()->getHeight() - gMainComponentMinHeight) / (gMainComponentHeight - gMainComponentMinHeight);
    
    iconImageComponent.setBounds(area);
    area.reduce(10 * paddingScalarX + 4, 10 * paddingScalarY + 4);
    
    Rectangle<int> leftColumn = area.removeFromLeft(area.getWidth() * 0.5);
    Rectangle<int> comboBoxSlice = leftColumn.removeFromTop(gComponentComboBoxHeight);
    comboBoxSlice.removeFromRight(4 + 2.*gPaddingConst * paddingScalarX);
    comboBoxSlice.removeFromLeft(gXSpacing);
    hideOrShow.setBounds(comboBoxSlice.removeFromLeft(gComponentComboBoxHeight));
    comboBoxSlice.removeFromLeft(gXSpacing);
    selectCB.setBounds(comboBoxSlice.removeFromLeft(comboBoxSlice.getWidth() / 2.));
    
    /* *** above here should be generic to all prep layouts *** */
    /* ***    below here will be specific to each prep      *** */
    
    Rectangle<int> sliderSlice = leftColumn;
    sliderSlice.removeFromRight(gXSpacing + 2.*gPaddingConst * paddingScalarX);
    //sliderSlice.removeFromLeft(gXSpacing);
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
    area.removeFromLeft(gXSpacing + 2.*gPaddingConst * paddingScalarX);
    area.removeFromRight(gXSpacing);
    
    transpositionSlider->setBounds(area.getX(),
                                   resonanceGainSlider->getY(),
                                   area.getWidth(),
                                   gComponentStackedSliderHeight + paddingScalarY * 30);
    
}


// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ DirectPreparationEditor ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~//
DirectPreparationEditor::DirectPreparationEditor(BKAudioProcessor& p, BKItemGraph* theGraph):
DirectViewController(p, theGraph)
{
    selectCB.addMyListener(this);
    selectCB.addListener(this);
    
    fillSelectCB();
    
    transpositionSlider->addMyListener(this);
    
    gainSlider->addMyListener(this);
    
    resonanceGainSlider->addMyListener(this);
    
    hammerGainSlider->addMyListener(this);
    
    hideOrShow.addListener(this);
}

void DirectPreparationEditor::update(void)
{
    
    if (processor.updateState->currentDirectId < 0) return;
    
    fillSelectCB();
    
    DirectPreparation::Ptr prep = processor.gallery->getActiveDirectPreparation(processor.updateState->currentDirectId);

    selectCB.setSelectedItemIndex(processor.updateState->currentDirectId, dontSendNotification);
    
    transpositionSlider->setValue(prep->getTransposition(), dontSendNotification);
    resonanceGainSlider->setValue(prep->getResonanceGain(), dontSendNotification);
    hammerGainSlider->setValue(prep->getHammerGain(), dontSendNotification);
    gainSlider->setValue(prep->getGain(), dontSendNotification);
    
}

void DirectPreparationEditor::bkMessageReceived (const String& message)
{
    if (message == "direct/update")
    {
        
        update();
    }
}

void DirectPreparationEditor::bkComboBoxDidChange (ComboBox* box)
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
        
        update();
    }
}

void DirectPreparationEditor::BKEditableComboBoxChanged(String name, BKEditableComboBox* cb)
{
    processor.gallery->getDirect(processor.updateState->currentDirectId)->setName(name);
}


void DirectPreparationEditor::BKSingleSliderValueChanged(String name, double val)
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

void DirectPreparationEditor::BKStackedSliderValueChanged(String name, Array<float> val)
{
    DirectPreparation::Ptr prep = processor.gallery->getStaticDirectPreparation(processor.updateState->currentDirectId);
    DirectPreparation::Ptr active = processor.gallery->getActiveDirectPreparation(processor.updateState->currentDirectId);
    
    prep->setTransposition(val);
    active->setTransposition(val);
}

void DirectPreparationEditor::fillSelectCB(void)
{
    // Direct menu
    Direct::PtrArr newpreps = processor.gallery->getAllDirect();
    
    selectCB.clear(dontSendNotification);
    for (int i = 0; i < newpreps.size(); i++)
    {
        String name = newpreps[i]->getName();
        if (name != String::empty)  selectCB.addItem(name, i+1);
        else                        selectCB.addItem(String(i+1), i+1);
        
        selectCB.setItemEnabled(i+1, true);
        Array<int> active = processor.updateState->active.getUnchecked(PreparationTypeDirect);
        if (active.contains(i) && i != processor.updateState->currentDirectId)
        {
            selectCB.setItemEnabled(i+1, false);
        }
    }
    
    selectCB.addItem("New direct...", newpreps.size()+1);
    
    selectCB.setSelectedItemIndex(processor.updateState->currentDirectId, NotificationType::dontSendNotification);
    
}

void DirectPreparationEditor::buttonClicked (Button* b)
{
    if (b == &hideOrShow)
    {
        processor.updateState->setCurrentDisplay(DisplayNil);
    }
}

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ DirectModificationEditor ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~//
DirectModificationEditor::DirectModificationEditor(BKAudioProcessor& p, BKItemGraph* theGraph):
DirectViewController(p, theGraph)
{
    
    selectCB.addListener(this);
    selectCB.addMyListener(this);
    
    fillSelectCB();
    
    transpositionSlider->addMyListener(this);
    
    gainSlider->addMyListener(this);
    
    resonanceGainSlider->addMyListener(this);
    
    hammerGainSlider->addMyListener(this);
    
    hideOrShow.addListener(this);
}

void DirectModificationEditor::update(void)
{
    
    if (processor.updateState->currentModDirectId < 0) return;
    
    fillSelectCB();
    
    selectCB.setSelectedItemIndex(processor.updateState->currentModDirectId, dontSendNotification);
    
    DirectModPreparation::Ptr mod = processor.gallery->getDirectModPreparation(processor.updateState->currentModDirectId);
    
    
    String val = mod->getParam(DirectTransposition);
    transpositionSlider->setValue(stringToFloatArray(val), dontSendNotification);
    
    val = mod->getParam(DirectResGain);
    resonanceGainSlider->setValue(val.getFloatValue(), dontSendNotification);
    
    val = mod->getParam(DirectHammerGain);
    hammerGainSlider->setValue(val.getFloatValue(), dontSendNotification);
    
    val = mod->getParam(DirectGain);
    gainSlider->setValue(val.getFloatValue(), dontSendNotification);
}

void DirectModificationEditor::fillSelectCB(void)
{
    // Direct menu
    StringArray mods = processor.gallery->getAllDirectModNames();
    
    selectCB.clear(dontSendNotification);
    for (int i = 0; i < mods.size(); i++)
    {
        String name = mods[i];
        if (name != String::empty)  selectCB.addItem(name, i+1);
        else                        selectCB.addItem(String(i+1), i+1);
    }
    
    selectCB.addItem("New direct modification...", mods.size()+1);
    
    selectCB.setSelectedItemIndex(processor.updateState->currentModDirectId, NotificationType::dontSendNotification);

    
}

void DirectModificationEditor::bkMessageReceived (const String& message)
{
    if (message == "direct/update")
    {
        update();
    }
}

void DirectModificationEditor::bkComboBoxDidChange (ComboBox* box)
{
    String name = box->getName();
    
    if (name == "Direct")
    {
        processor.updateState->currentModDirectId = box->getSelectedItemIndex();
        
        processor.updateState->idDidChange = true;
        
        
        if (processor.updateState->currentModDirectId == selectCB.getNumItems()-1)
        {
            processor.gallery->addDirectMod();
            
            fillSelectCB();
        }
        
        update();
        
        return;
    }
}

void DirectModificationEditor::BKEditableComboBoxChanged(String name, BKEditableComboBox* cb)
{
    processor.gallery->getDirectModPreparation(processor.updateState->currentModDirectId)->setName(name);
    
    updateModification();
}


void DirectModificationEditor::BKSingleSliderValueChanged(String name, double val)
{
    DirectModPreparation::Ptr mod = processor.gallery->getDirectModPreparation(processor.updateState->currentModDirectId);
    
    if(name == "resonance gain")
    {
        mod->setParam(DirectResGain, String(val));
    }
    else if(name == "hammer gain")
    {
        mod->setParam(DirectHammerGain, String(val));
    }
    else if(name == "gain")
    {
        mod->setParam(DirectGain, String(val));
    }
    
    updateModification();
}

void DirectModificationEditor::BKStackedSliderValueChanged(String name, Array<float> val)
{
    DirectModPreparation::Ptr mod = processor.gallery->getDirectModPreparation(processor.updateState->currentModDirectId);
    
    mod->setParam(DirectTransposition, floatArrayToString(val));
    
    updateModification();
}

void DirectModificationEditor::updateModification(void)
{
    processor.updateState->modificationDidChange = true;
}

void DirectModificationEditor::buttonClicked (Button* b)
{
    if (b == &hideOrShow)
    {
        processor.updateState->setCurrentDisplay(DisplayNil);
    }
}


