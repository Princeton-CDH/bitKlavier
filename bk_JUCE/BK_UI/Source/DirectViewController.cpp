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
    comboBoxSlice.removeFromLeft(gXSpacing);
    clearModsButton.setBounds(comboBoxSlice.removeFromLeft(90));
    
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
    
    fillSelectCB(-1,-1);
    
    transpositionSlider->addMyListener(this);
    
    gainSlider->addMyListener(this);
    
    resonanceGainSlider->addMyListener(this);
    
    hammerGainSlider->addMyListener(this);
    
    hideOrShow.addListener(this);
}

void DirectPreparationEditor::update(void)
{
    if (processor.updateState->currentDirectId < 0) return;
    
    DirectPreparation::Ptr prep = processor.gallery->getActiveDirectPreparation(processor.updateState->currentDirectId);

    if (prep != nullptr)
    {
        selectCB.setSelectedItemIndex(processor.updateState->currentDirectId, dontSendNotification);
        
        transpositionSlider->setValue(prep->getTransposition(), dontSendNotification);
        resonanceGainSlider->setValue(prep->getResonanceGain(), dontSendNotification);
        hammerGainSlider->setValue(prep->getHammerGain(), dontSendNotification);
        gainSlider->setValue(prep->getGain(), dontSendNotification);
    }
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
    int Id = box->getSelectedId();

    if (name == "Direct")
    {
        if (Id == -1)
        {
            processor.gallery->addDirect();
            
            Id = processor.gallery->getAllDirect().getLast()->getId();
        }
        
        processor.updateState->currentDirectId = Id;
        
        processor.updateState->idDidChange = true;
        
        update();
        
        fillSelectCB(lastId, Id);
    
        lastId = Id;
    }
}

void DirectPreparationEditor::BKEditableComboBoxChanged(String name, BKEditableComboBox* cb)
{
    Direct::Ptr direct = processor.gallery->getDirect(processor.updateState->currentDirectId);
    direct->editted = true;
    
    direct->setName(name);
}


void DirectPreparationEditor::BKSingleSliderValueChanged(String name, double val)
{
    Direct::Ptr direct = processor.gallery->getDirect(processor.updateState->currentDirectId);
    direct->editted = true;
    
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
    Direct::Ptr direct = processor.gallery->getDirect(processor.updateState->currentDirectId);
    direct->editted = true;
    
    DirectPreparation::Ptr prep = processor.gallery->getStaticDirectPreparation(processor.updateState->currentDirectId);
    DirectPreparation::Ptr active = processor.gallery->getActiveDirectPreparation(processor.updateState->currentDirectId);
    
    prep->setTransposition(val);
    active->setTransposition(val);
}

void DirectPreparationEditor::fillSelectCB(int last, int current)
{
    selectCB.clear(dontSendNotification);
    
    for (auto prep : processor.gallery->getAllDirect())
    {
        int Id = prep->getId();;
        String name = prep->getName();
        
        if (name != String::empty)  selectCB.addItem(name, Id);
        else                        selectCB.addItem("Direct"+String(Id), Id);
        
        selectCB.setItemEnabled(Id, true);
        if (processor.currentPiano->isActive(PreparationTypeDirect, Id))
            selectCB.setItemEnabled(Id, false);
    }
    
    if (last != 0)      selectCB.setItemEnabled(last, true);
    if (current != 0)   selectCB.setItemEnabled(current, false);
    
    int selectedId = processor.updateState->currentDirectId;
    
    selectCB.setSelectedId(selectedId, NotificationType::dontSendNotification);
    
    selectCB.setItemEnabled(selectedId, false);
    
    selectCB.addSeparator();
    selectCB.addItem("New Direct...", -1);
    
    lastId = selectedId;
}

void DirectPreparationEditor::buttonClicked (Button* b)
{
    if (b == &hideOrShow)
    {
        DirectModPreparation::Ptr mod = processor.gallery->getDirectModPreparation(processor.updateState->currentModDirectId);
        mod->editted = true;
        
        processor.updateState->setCurrentDisplay(DisplayNil);
    }
}

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ DirectModificationEditor ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~//
DirectModificationEditor::DirectModificationEditor(BKAudioProcessor& p, BKItemGraph* theGraph):
DirectViewController(p, theGraph)
{
    
    selectCB.addListener(this);
    selectCB.addMyListener(this);
    
    fillSelectCB(-1,-1);
    
    transpositionSlider->addMyListener(this);
    
    gainSlider->addMyListener(this);
    
    resonanceGainSlider->addMyListener(this);
    
    hammerGainSlider->addMyListener(this);
    
    hideOrShow.addListener(this);
    
    clearModsButton.setButtonText("clear mods");
    addAndMakeVisible(clearModsButton);
    clearModsButton.addListener(this);
}

void DirectModificationEditor::greyOutAllComponents()
{
    hammerGainSlider->setDim(gModAlpha);
    resonanceGainSlider->setDim(gModAlpha);
    transpositionSlider->setDim(gModAlpha);
    gainSlider->setDim(gModAlpha);
}

void DirectModificationEditor::highlightModedComponents()
{
    DirectModPreparation::Ptr mod = processor.gallery->getDirectModPreparation(processor.updateState->currentModDirectId);

    if(mod->getParam(DirectTransposition) != "")    transpositionSlider->setBright();
    if(mod->getParam(DirectGain) != "")             gainSlider->setBright();
    if(mod->getParam(DirectResGain) != "")          resonanceGainSlider->setBright();
    if(mod->getParam(DirectHammerGain) != "")       hammerGainSlider->setBright();
}

void DirectModificationEditor::update(void)
{
    if (processor.updateState->currentModDirectId < 0) return;
    
    selectCB.setSelectedItemIndex(processor.updateState->currentModDirectId, dontSendNotification);
    
    DirectModPreparation::Ptr mod = processor.gallery->getDirectModPreparation(processor.updateState->currentModDirectId);
    
    if (mod != nullptr)
    {
        greyOutAllComponents();
        highlightModedComponents();
        
        String val = mod->getParam(DirectTransposition);
        transpositionSlider->setValue(stringToFloatArray(val), dontSendNotification);
        
        val = mod->getParam(DirectResGain);
        resonanceGainSlider->setValue(val.getFloatValue(), dontSendNotification);
        
        val = mod->getParam(DirectHammerGain);
        hammerGainSlider->setValue(val.getFloatValue(), dontSendNotification);
        
        val = mod->getParam(DirectGain);
        gainSlider->setValue(val.getFloatValue(), dontSendNotification);
    }
    
    
}

void DirectModificationEditor::fillSelectCB(int last, int current)
{
    selectCB.clear(dontSendNotification);
    
    for (auto prep : processor.gallery->getDirectModPreparations())
    {
        int Id = prep->getId();;
        String name = prep->getName();
        
        if (name != String::empty)  selectCB.addItem(name, Id);
        else                        selectCB.addItem("DirectMod"+String(Id), Id);
        
        selectCB.setItemEnabled(Id, true);
        if (processor.currentPiano->isActive(PreparationTypeDirect, Id))
            selectCB.setItemEnabled(Id, false);
    }
    
    if (last != 0)      selectCB.setItemEnabled(last, true);
    if (current != 0)   selectCB.setItemEnabled(current, false);
    
    int selectedId = processor.updateState->currentDirectId;
    
    selectCB.setSelectedId(selectedId, NotificationType::dontSendNotification);
    
    selectCB.setItemEnabled(selectedId, false);
    
    selectCB.addSeparator();
    selectCB.addItem("New Direct Mod...", -1);
    
    lastId = selectedId;
    
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
    int Id = box->getSelectedId();
    
    if (name == "Direct")
    {
        if (Id == -1)
        {
            processor.gallery->addDirectMod();
            
            Id = processor.gallery->getDirectModPreparations().getLast()->getId();
        }
        
        processor.updateState->currentModDirectId = Id;
  
        processor.updateState->idDidChange = true;
        
        update();
        
        fillSelectCB(lastId, Id);
        
        lastId = Id;
    }
}

void DirectModificationEditor::BKEditableComboBoxChanged(String name, BKEditableComboBox* cb)
{
    DirectModPreparation::Ptr mod = processor.gallery->getDirectModPreparation(processor.updateState->currentModDirectId);
    
    mod->setName(name);
    
    updateModification();
}


void DirectModificationEditor::BKSingleSliderValueChanged(String name, double val)
{
    DirectModPreparation::Ptr mod = processor.gallery->getDirectModPreparation(processor.updateState->currentModDirectId);
    
    if(name == "resonance gain")
    {
        mod->setParam(DirectResGain, String(val));
        resonanceGainSlider->setBright();
    }
    else if(name == "hammer gain")
    {
        mod->setParam(DirectHammerGain, String(val));
        hammerGainSlider->setBright();
    }
    else if(name == "gain")
    {
        mod->setParam(DirectGain, String(val));
        gainSlider->setBright();
    }
    
    updateModification();
}

void DirectModificationEditor::BKStackedSliderValueChanged(String name, Array<float> val)
{
    DirectModPreparation::Ptr mod = processor.gallery->getDirectModPreparation(processor.updateState->currentModDirectId);
    
    mod->setParam(DirectTransposition, floatArrayToString(val));
    transpositionSlider->setBright();
    
    updateModification();
}

void DirectModificationEditor::updateModification(void)
{
    DirectModPreparation::Ptr mod = processor.gallery->getDirectModPreparation(processor.updateState->currentModDirectId);
    mod->editted = true;
    
    processor.updateState->modificationDidChange = true;
}

void DirectModificationEditor::buttonClicked (Button* b)
{
    if (b == &hideOrShow)
    {
        DirectModPreparation::Ptr mod = processor.gallery->getDirectModPreparation(processor.updateState->currentModDirectId);
        mod->editted = true;
        
        processor.updateState->setCurrentDisplay(DisplayNil);
    }
    else if (b == &clearModsButton)
    {
        DirectModPreparation::Ptr mod = processor.gallery->getDirectModPreparation(processor.updateState->currentModDirectId);
        mod->clearAll();
        update();
    }
}


