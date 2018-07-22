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
    selectCB.setTooltip("Select from available saved preparation settings");
    addAndMakeVisible(selectCB);
    
    transpositionSlider = new BKStackedSlider("transpositions", -12, 12, -12, 12, 0, 0.01);
    transpositionSlider->setTooltip("Determines pitch (in semitones) of Direct notes; control-click to add another voice, double-click to edit all");
    addAndMakeVisible(transpositionSlider);
    
    gainSlider = new BKSingleSlider("gain", 0, 10, 1, 0.01);
    gainSlider->setSkewFactorFromMidPoint(1.);
    gainSlider->setJustifyRight(false);
    gainSlider->setToolTipString("Adjusts overall volume of keyboard");
    addAndMakeVisible(gainSlider);
    
    resonanceGainSlider = new BKSingleSlider("resonance gain", 0, 10, 0.2, 0.01);
    resonanceGainSlider->setSkewFactorFromMidPoint(1.);
    resonanceGainSlider->setJustifyRight(false);
    resonanceGainSlider->setToolTipString("Adjusts overall resonance/reverb based on keyOff velocity; change to keyOn velocity in Gallery>settings");
    addAndMakeVisible(resonanceGainSlider);
    
    hammerGainSlider = new BKSingleSlider("hammer gain", 0, 10, 1, 0.01);
    hammerGainSlider->setSkewFactorFromMidPoint(1.);
    hammerGainSlider->setJustifyRight(false);
    hammerGainSlider->setToolTipString("Adjusts mechanical noise sample based on keyOff velocity; change to keyOn velocity in Gallery>settings");
    addAndMakeVisible(hammerGainSlider);
    
    ADSRSlider = new BKADSRSlider("ADSR");
    ADSRSlider->setButtonText("edit envelope");
    ADSRSlider->setToolTip("adjust Attack, Decay, Sustain, and Release envelope parameters");
    addAndMakeVisible(ADSRSlider);
    setShowADSR(false);
    
    
#if JUCE_IOS
    transpositionSlider->addWantsBigOneListener(this);
    gainSlider->addWantsBigOneListener(this);
    resonanceGainSlider->addWantsBigOneListener(this);
    hammerGainSlider->addWantsBigOneListener(this);
#endif
    
    addAndMakeVisible(actionButton);
    actionButton.setButtonText("Action");
    actionButton.setTooltip("Create, duplicate, rename, delete, or reset current settings");
    actionButton.addListener(this);

}

void DirectViewController::paint (Graphics& g)
{
    g.fillAll(Colours::black);
}

void DirectViewController::setShowADSR(bool newval)
{
    showADSR = newval;
    
    if(showADSR)
    {
        resonanceGainSlider->setVisible(false);
        hammerGainSlider->setVisible(false);
        gainSlider->setVisible(false);
        transpositionSlider->setVisible(false);
        
        ADSRSlider->setButtonText("close envelope");

        
    }
    else
    {
        resonanceGainSlider->setVisible(true);
        hammerGainSlider->setVisible(true);
        gainSlider->setVisible(true);
        transpositionSlider->setVisible(true);

        ADSRSlider->setButtonText("edit envelope");
    }
    
    resized();
    
}

void DirectViewController::resized()
{
    Rectangle<int> area (getLocalBounds());

    iconImageComponent.setBounds(area);
    area.reduce(10 * processor.paddingScalarX + 4, 10 * processor.paddingScalarY + 4);
    
    Rectangle<int> areaSave = area;

    Rectangle<int> leftColumn = area.removeFromLeft(area.getWidth() * 0.5);
    Rectangle<int> comboBoxSlice = leftColumn.removeFromTop(gComponentComboBoxHeight);
    comboBoxSlice.removeFromRight(4 + 2.*gPaddingConst * processor.paddingScalarX);
    comboBoxSlice.removeFromLeft(gXSpacing);
    hideOrShow.setBounds(comboBoxSlice.removeFromLeft(gComponentComboBoxHeight));
    comboBoxSlice.removeFromLeft(gXSpacing);
    selectCB.setBounds(comboBoxSlice.removeFromLeft(comboBoxSlice.getWidth() / 2.));

    comboBoxSlice.removeFromLeft(gXSpacing);
    
#if JUCE_IOS
    actionButton.setBounds(selectCB.getRight()+gXSpacing,
                           selectCB.getY(),
                           selectCB.getWidth() * 0.5,
                           selectCB.getHeight());
#else
    actionButton.setBounds(selectCB.getRight()+gXSpacing,
                           selectCB.getY(),
                           selectCB.getWidth() * 0.5,
                           selectCB.getHeight());
#endif
    
    /* *** above here should be generic to all prep layouts *** */
    /* ***    below here will be specific to each prep      *** */
    
    if(!showADSR)
    {
        Rectangle<int> sliderSlice = leftColumn;
        sliderSlice.removeFromRight(gXSpacing + 2.*gPaddingConst * processor.paddingScalarX);
        //sliderSlice.removeFromLeft(gXSpacing);
        /*
         sliderSlice.reduce(4 + 2.*gPaddingConst * processor.paddingScalarX,
         4 + 2.*gPaddingConst * processor.paddingScalarY);
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
        area.removeFromLeft(gXSpacing + 2.*gPaddingConst * processor.paddingScalarX);
        area.removeFromRight(gXSpacing);
        
        
        transpositionSlider->setBounds(area.getX(),
                                       resonanceGainSlider->getY(),
                                       area.getWidth(),
                                       gComponentStackedSliderHeight + processor.paddingScalarY * 30);
        
        
        //area.removeFromTop(gComponentComboBoxHeight);
        //transpositionSlider->setBounds(area.removeFromTop(gComponentStackedSliderHeight + processor.paddingScalarY * 40));
        
        //area.removeFromTop(gYSpacing + 6.*gPaddingConst * processor.paddingScalarY);
        //ADSRSlider->setBounds(area.removeFromTop(5*gComponentSingleSliderHeight));
        
        //area.removeFromBottom(gYSpacing + 6.*gPaddingConst * processor.paddingScalarY);
        //ADSRSlider->setBounds(area.removeFromBottom(gComponentComboBoxHeight));
        
        ADSRSlider->setBounds(area.getX(),
                              gainSlider->getY() + gComponentComboBoxHeight * 0.5,
                              area.getWidth(),
                              gComponentComboBoxHeight);
    }
    else
    {
        areaSave.removeFromTop(gYSpacing * 2 + 8.*gPaddingConst * processor.paddingScalarY);
        Rectangle<int> adsrSliderSlice = areaSave.removeFromTop(gComponentComboBoxHeight * 2 + gComponentSingleSliderHeight * 2 + gYSpacing * 3);
        ADSRSlider->setBounds(adsrSliderSlice);
        
        //areaSave.removeFromTop(gComponentComboBoxHeight * 2 + gYSpacing + 8.*gPaddingConst * processor.paddingScalarY);
        //ADSRSlider->setBounds(areaSave);
        
        selectCB.toFront(false);
    }
}

#if JUCE_IOS
void DirectViewController::iWantTheBigOne(TextEditor* tf, String name)
{
    hideOrShow.setAlwaysOnTop(false);
    bigOne.display(tf, name, getBounds());
}
#endif

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
    
    ADSRSlider->addMyListener(this);
    
    
}

void DirectPreparationEditor::update(void)
{
    if (processor.updateState->currentDirectId < 0) return;
    setShowADSR(false);
    setSubWindowInFront(false);
    ADSRSlider->setIsButtonOnly(true);
    
    DirectPreparation::Ptr prep = processor.gallery->getActiveDirectPreparation(processor.updateState->currentDirectId);

    if (prep != nullptr)
    {
        selectCB.setSelectedId(processor.updateState->currentDirectId, dontSendNotification);
        
        transpositionSlider->setValue(prep->getTransposition(), dontSendNotification);
        resonanceGainSlider->setValue(prep->getResonanceGain(), dontSendNotification);
        hammerGainSlider->setValue(prep->getHammerGain(), dontSendNotification);
        gainSlider->setValue(prep->getGain(), dontSendNotification);
        ADSRSlider->setAttackValue(prep->getAttack(), dontSendNotification);
        ADSRSlider->setDecayValue(prep->getDecay(), dontSendNotification);
        ADSRSlider->setSustainValue(prep->getSustain(), dontSendNotification);
        ADSRSlider->setReleaseValue(prep->getRelease(), dontSendNotification);
    }
}

void DirectPreparationEditor::bkMessageReceived (const String& message)
{
    if (message == "direct/update")
    {
        update();
    }
}

int DirectPreparationEditor::addPreparation(void)
{
    processor.gallery->add(PreparationTypeDirect);
    
    return processor.gallery->getAllDirect().getLast()->getId();
}

int DirectPreparationEditor::duplicatePreparation(void)
{
    processor.gallery->duplicate(PreparationTypeDirect, processor.updateState->currentDirectId);
    
    return processor.gallery->getAllDirect().getLast()->getId();
}

void DirectPreparationEditor::deleteCurrent(void)
{
    int directId = selectCB.getSelectedId();
    int index = selectCB.getSelectedItemIndex();
    
    if ((index == 0) && (selectCB.getItemId(index+1) == -1)) return;
    
    processor.gallery->remove(PreparationTypeDirect, directId);
    
    fillSelectCB(0, 0);
    
    int newId = 0;
    
    selectCB.setSelectedId(newId, dontSendNotification);
    
    processor.updateState->currentDirectId = -1;
}

void DirectPreparationEditor::setCurrentId(int Id)
{
    processor.updateState->currentDirectId = Id;
    
    processor.updateState->idDidChange = true;
    
    update();
    
    fillSelectCB(lastId, Id);
    
    lastId = Id;
}

void DirectPreparationEditor::actionButtonCallback(int action, DirectPreparationEditor* vc)
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
        processor.reset(PreparationTypeDirect, processor.updateState->currentDirectId);
        vc->update();
    }
    else if (action == 5)
    {
        processor.clear(PreparationTypeDirect, processor.updateState->currentDirectId);
        vc->update();
    }
    else if (action == 6)
    {
        AlertWindow prompt("", "", AlertWindow::AlertIconType::QuestionIcon);
        
        int Id = processor.updateState->currentDirectId;
        Direct::Ptr prep = processor.gallery->getDirect(Id);
        
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

void DirectPreparationEditor::bkComboBoxDidChange (ComboBox* box)
{
    String name = box->getName();
    int Id = box->getSelectedId();

    if (name == "Direct")
    {
        setCurrentId(Id);
    }
}

void DirectPreparationEditor::BKEditableComboBoxChanged(String name, BKEditableComboBox* cb)
{
    Direct::Ptr direct = processor.gallery->getDirect(processor.updateState->currentDirectId);
    direct->setName(name);
    
    fillSelectCB(0, processor.updateState->currentDirectId);
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

void DirectPreparationEditor::BKADSRSliderValueChanged(String name, int attack, int decay, float sustain, int release)
{
    DBG("BKADSRSliderValueChanged received");
    
    DirectPreparation::Ptr prep = processor.gallery->getStaticDirectPreparation(processor.updateState->currentDirectId);
    DirectPreparation::Ptr active = processor.gallery->getActiveDirectPreparation(processor.updateState->currentDirectId);
    
    prep->setAttack(attack);
    active->setAttack(attack);
    prep->setDecay(decay);
    active->setDecay(decay);
    prep->setSustain(sustain);
    active->setSustain(sustain);
    prep->setRelease(release);
    active->setRelease(release);
}

void DirectPreparationEditor::BKADSRButtonStateChanged(String name, bool mod, bool state)
{
    setShowADSR(!state);
    setSubWindowInFront(!state);
}

void DirectPreparationEditor::closeSubWindow()
{
    ADSRSlider->setIsButtonOnly(true);
    setShowADSR(false);
    setSubWindowInFront(false);
}


void DirectPreparationEditor::fillSelectCB(int last, int current)
{
    selectCB.clear(dontSendNotification);
    
    for (auto prep : processor.gallery->getAllDirect())
    {
        int Id = prep->getId();
        
        if (Id == -1) continue;
        
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
    
    
    lastId = selectedId;
}

void DirectPreparationEditor::buttonClicked (Button* b)
{
    if (b == &hideOrShow)
    {
        processor.updateState->setCurrentDisplay(DisplayNil);
        
        ADSRSlider->setIsButtonOnly(true);
        setShowADSR(false);
        setSubWindowInFront(false);
    }
    else if (b == &actionButton)
    {
        getPrepOptionMenu().showMenuAsync (PopupMenu::Options().withTargetComponent (&actionButton), ModalCallbackFunction::forComponent (actionButtonCallback, this) );
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
    
    ADSRSlider->addMyListener(this);
}

void DirectModificationEditor::greyOutAllComponents()
{
    hammerGainSlider->setDim(gModAlpha);
    resonanceGainSlider->setDim(gModAlpha);
    transpositionSlider->setDim(gModAlpha);
    gainSlider->setDim(gModAlpha);
    ADSRSlider->setDim(gModAlpha);
}

void DirectModificationEditor::highlightModedComponents()
{
    DirectModPreparation::Ptr mod = processor.gallery->getDirectModPreparation(processor.updateState->currentModDirectId);

    if(mod->getParam(DirectTransposition) != "")    transpositionSlider->setBright();
    if(mod->getParam(DirectGain) != "")             gainSlider->setBright();
    if(mod->getParam(DirectResGain) != "")          resonanceGainSlider->setBright();
    if(mod->getParam(DirectHammerGain) != "")       hammerGainSlider->setBright();
    if(mod->getParam(DirectADSR) != "")             ADSRSlider->setBright();
}

void DirectModificationEditor::update(void)
{
    if (processor.updateState->currentModDirectId < 0) return;
    
    selectCB.setSelectedId(processor.updateState->currentModDirectId, dontSendNotification);
    
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
        
        val = mod->getParam(DirectADSR);
        ADSRSlider->setValue(stringToFloatArray(val), dontSendNotification);
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
    
    lastId = selectedId;
    
}

void DirectModificationEditor::bkMessageReceived (const String& message)
{
    if (message == "direct/update")
    {
        update();
    }
}

int DirectModificationEditor::addPreparation(void)
{
    processor.gallery->add(PreparationTypeDirectMod);
    
    return processor.gallery->getDirectModPreparations().getLast()->getId();
}

int DirectModificationEditor::duplicatePreparation(void)
{
    processor.gallery->duplicate(PreparationTypeDirectMod, processor.updateState->currentModDirectId);
    
    return processor.gallery->getDirectModPreparations().getLast()->getId();
}

void DirectModificationEditor::deleteCurrent(void)
{
    int directId = selectCB.getSelectedId();
    int index = selectCB.getSelectedItemIndex();
    
    if ((index == 0) && (selectCB.getItemId(index+1) == -1)) return;
    
    processor.gallery->remove(PreparationTypeDirect, directId);
    
    fillSelectCB(0, 0);
    
    int newId = 0;
    
    selectCB.setSelectedId(newId, dontSendNotification);
    
    processor.updateState->currentDirectId = -1;
}

void DirectModificationEditor::setCurrentId(int Id)
{
    processor.updateState->currentDirectId = Id;
    
    processor.updateState->idDidChange = true;
    
    update();
    
    fillSelectCB(lastId, Id);
    
    lastId = Id;
}

void DirectModificationEditor::actionButtonCallback(int action, DirectModificationEditor* vc)
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
        processor.clear(PreparationTypeDirectMod, processor.updateState->currentModDirectId);
        vc->update();
        vc->updateModification();
    }
    else if (action == 6)
    {
        AlertWindow prompt("", "", AlertWindow::AlertIconType::QuestionIcon);
        
        int Id = processor.updateState->currentModDirectId;
        DirectModPreparation::Ptr prep = processor.gallery->getDirectModPreparation(Id);
        
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

void DirectModificationEditor::bkComboBoxDidChange (ComboBox* box)
{
    String name = box->getName();
    int Id = box->getSelectedId();
    
    if (name == "Direct")
    {
        setCurrentId(Id);
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

void DirectModificationEditor::BKADSRSliderValueChanged(String name, int attack, int decay, float sustain, int release)
{
    DirectModPreparation::Ptr mod = processor.gallery->getDirectModPreparation(processor.updateState->currentModDirectId);
    
    Array<float> newvals = {(float)attack, (float)decay, sustain, (float)release};
    mod->setParam(DirectADSR, floatArrayToString(newvals));
    ADSRSlider->setBright();
    
    updateModification();
}

void DirectModificationEditor::BKADSRButtonStateChanged(String name, bool mod, bool state)
{
    setShowADSR(!state);
    setSubWindowInFront(!state);
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
    else if (b == &actionButton)
    {
        getModOptionMenu().showMenuAsync (PopupMenu::Options().withTargetComponent (&actionButton), ModalCallbackFunction::forComponent (actionButtonCallback, this) );
    }
}

