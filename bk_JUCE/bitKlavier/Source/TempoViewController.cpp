/*
  ==============================================================================

    TempoViewController.cpp
    Created: 17 Jun 2017 5:29:25pm
    Author:  Daniel Trueman

  ==============================================================================
*/

#include "TempoViewController.h"


TempoViewController::TempoViewController(BKAudioProcessor& p, BKItemGraph* theGraph):
BKViewController(p, theGraph, 1)
{
    
    setLookAndFeel(&buttonsAndMenusLAF);
    
    iconImageComponent.setImage(ImageCache::getFromMemory(BinaryData::tempo_icon_png, BinaryData::tempo_icon_pngSize));
    iconImageComponent.setImagePlacement(RectanglePlacement(juce::RectanglePlacement::stretchToFit));
    iconImageComponent.setAlpha(0.095);
    addAndMakeVisible(iconImageComponent);
    
    selectCB.setName("Tempo");
    selectCB.addSeparator();
    selectCB.setSelectedItemIndex(0);
    selectCB.setTooltip("Select from available saved preparation settings");
    addAndMakeVisible(selectCB);
    
    modeCB.setName("Mode");
    modeCB.BKSetJustificationType(juce::Justification::centredRight);
    modeCB.setTooltip("Indicates whether in Constant or Adaptive mode");
    fillModeCB();
    addAndMakeVisible(modeCB);
    
    tempoSlider = std::make_unique<BKSingleSlider>("Tempo", 40, 208, 100, 0.01);
    tempoSlider->setToolTipString("Indicates current beats-per-minute (BPM)");
    addAndMakeVisible(*tempoSlider);
    
    subSlider = std::make_unique<BKSingleSlider>("Subdivisions", 0.01, 32.0, 1.0, 0.01);
    subSlider->setToolTipString("Number of pulses per beat");
    addAndMakeVisible(*subSlider);
    
    ATHistorySlider = std::make_unique<BKSingleSlider>("History", 1, 10, 4, 1);
    ATHistorySlider->setJustifyRight(false);
    ATHistorySlider->setToolTipString("Indicates how many notes Tempo is using to determine and generate an average pulse ");
    addAndMakeVisible(*ATHistorySlider);
    
    ATSubdivisionsSlider = std::make_unique<BKSingleSlider>("Subdivisions", 0., 12, 1, 0.01);
    ATSubdivisionsSlider->setJustifyRight(false);
    ATSubdivisionsSlider->setToolTipString("Multiplies tempo by interpreting rhythmic value of played notes; values less than 1 will result in tempos slower than what is played, values greater than 1 will result in tempos faster than what is played");
    addAndMakeVisible(*ATSubdivisionsSlider);
    
    ATMinMaxSlider = std::make_unique<BKRangeSlider>("Min/Max (ms)", 1, 2000, 100, 500, 10);
    ATMinMaxSlider->setJustifyRight(false);
    ATMinMaxSlider->setIsMinAlwaysLessThanMax(true);
    ATMinMaxSlider->setToolTipString("Time within which Tempo will consider notes to be part of a constant pulse; any notes played futher apart than Max, or closer together than Min, will be ignored");
    addAndMakeVisible(*ATMinMaxSlider);
    
    atModeButton.setName("ATMode");
    atModeButton.setButtonText("Select tempo measures");
    addAndMakeVisible(atModeButton);
    atModeLabel.setText("Mode", dontSendNotification);
    atModeButton.setTooltip("Indicates which aspects of performance Tempo is analyzing, using information from connected Keymap");
//    addAndMakeVisible(atModeLabel);
    
    attachKeymap.setText("Attach a Keymap to use Adaptive!", dontSendNotification);
    addAndMakeVisible(attachKeymap);
    
    addAndMakeVisible(A1AdaptedTempo);
    addAndMakeVisible(A1AdaptedPeriodMultiplier);
    A1AdaptedPeriodMultiplier.setJustificationType(juce::Justification::centredRight);
    
    A1reset.setButtonText("reset");
    //addAndMakeVisible(A1reset);
    
    addAndMakeVisible(actionButton);
    actionButton.setButtonText("Action");
    actionButton.setTooltip("Create, duplicate, rename, delete, or reset current settings");
    actionButton.addListener(this);
    
#if JUCE_IOS
    ATMinMaxSlider->addWantsBigOneListener(this);
    ATHistorySlider->addWantsBigOneListener(this);
    ATSubdivisionsSlider->addWantsBigOneListener(this);
    tempoSlider->addWantsBigOneListener(this);
    subSlider->addWantsBigOneListener(this);
#endif
    
    iterationsSlider = std::make_unique<BKSingleSlider>("Iterations", 1, 32, 4, 1);
    addAndMakeVisible(*iterationsSlider);

    emaAlphaSlider = std::make_unique<BKSingleSlider>("Alpha", 0, 1, 0.5, 0.01);
    addAndMakeVisible(*emaAlphaSlider);
    
    exponentialToggle.setButtonText("Exponential MA");
    exponentialToggle.setClickingTogglesState(true);
    exponentialToggle.addListener(this);
    
    addAndMakeVisible(exponentialToggle);
    
    weightsToggle.setButtonText("Use weights");
    weightsToggle.setClickingTogglesState(true);
    weightsToggle.addListener(this);
    
    addAndMakeVisible(weightsToggle);
    
    TempoPreparation::Ptr active = processor.gallery->getActiveTempoPreparation(processor.updateState->currentTempoId);
    
    weightsText.setText(floatArrayToString(active->getAdaptiveTempoWeights()));
    weightsText.setMultiLine(true);
    weightsText.addListener(this);
    addAndMakeVisible(weightsText);

    updateComponentVisibility();
}


void TempoViewController::resized()
{

    Rectangle<int> area (getLocalBounds());
    
    iconImageComponent.setBounds(area);
    area.reduce(10 * processor.paddingScalarX + 4, 10 * processor.paddingScalarY + 4);
    
    Rectangle<int> leftColumn = area.removeFromLeft(area.getWidth() * 0.5);
    Rectangle<int> comboBoxSlice = leftColumn.removeFromTop(gComponentComboBoxHeight);
    comboBoxSlice.removeFromRight(gXSpacing + 2.*gPaddingConst * processor.paddingScalarX);
    comboBoxSlice.removeFromLeft(gXSpacing);
    hideOrShow.setBounds(comboBoxSlice.removeFromLeft(gComponentComboBoxHeight));
    comboBoxSlice.removeFromLeft(gXSpacing);
    selectCB.setBounds(comboBoxSlice.removeFromLeft(comboBoxSlice.getWidth() * 0.75));
    
    actionButton.setBounds(selectCB.getRight()+gXSpacing,
                           selectCB.getY(),
                           selectCB.getWidth() * 0.75,
                           selectCB.getHeight());
    
    comboBoxSlice.removeFromLeft(gXSpacing);
    A1reset.setBounds(comboBoxSlice.removeFromLeft(90));
    
    /* *** above here should be generic (mostly) to all prep layouts *** */
    /* ***         below here will be specific to each prep          *** */
    
    // ********* left column
    
    leftColumn.removeFromBottom(gYSpacing);
    int extraY = (leftColumn.getHeight() -
                  (gComponentComboBoxHeight +
                   gComponentSingleSliderHeight * 2 +
                   gComponentRangeSliderHeight +
                   gComponentTextFieldHeight +
                   gYSpacing * 6)) / 6.;
    
    //DBG("extraY = " + String(extraY));
    
    leftColumn.removeFromTop(extraY + gYSpacing);
    Rectangle<int> atModeButtonSlice = leftColumn.removeFromTop(gComponentComboBoxHeight);
    attachKeymap.setBounds(leftColumn);
    atModeButtonSlice.removeFromRight(gXSpacing + 2.*gPaddingConst * processor.paddingScalarX);
    //A1ModeCBSlice.removeFromLeft(2 * gXSpacing + hideOrShow.getWidth());
    atModeButtonSlice.removeFromLeft(gXSpacing);
    atModeButton.setBounds(atModeButtonSlice.removeFromLeft(selectCB.getWidth() + gXSpacing + hideOrShow.getWidth()));
    atModeLabel.setBounds(atModeButtonSlice);
    
    leftColumn.removeFromTop(extraY + gYSpacing);
    ATHistorySlider->setBounds(leftColumn.removeFromTop(gComponentSingleSliderHeight));
    
    leftColumn.removeFromTop(extraY + gYSpacing);
    ATSubdivisionsSlider->setBounds(leftColumn.removeFromTop(gComponentSingleSliderHeight));
    
    leftColumn.removeFromTop(extraY + gYSpacing);
    ATMinMaxSlider->setBounds(leftColumn.removeFromTop(gComponentRangeSliderHeight));
    
    leftColumn.removeFromTop(extraY + gYSpacing);
    Rectangle<int> adaptedLabelSlice = leftColumn.removeFromTop(gComponentTextFieldHeight);
    A1AdaptedTempo.setBounds(adaptedLabelSlice.removeFromLeft(leftColumn.getWidth() / 2.));
    A1AdaptedPeriodMultiplier.setBounds(adaptedLabelSlice);
    
    
    
    // ********* right column
    
    Rectangle<int> modeSlice = area.removeFromTop(gComponentComboBoxHeight);
    modeSlice.removeFromRight(gXSpacing);
    modeCB.setBounds(modeSlice.removeFromRight(modeSlice.getWidth() / 2.));
    
    area.removeFromTop(atModeButton.getY() - selectCB.getBottom());
    Rectangle<int> tempoSliderSlice = area.removeFromTop(gComponentSingleSliderHeight);
    tempoSliderSlice.removeFromLeft(gXSpacing + 2.*gPaddingConst * processor.paddingScalarX - gComponentSingleSliderXOffset);
    tempoSliderSlice.removeFromRight(gXSpacing - gComponentSingleSliderXOffset);
    tempoSlider->setBounds(tempoSliderSlice);
    
    area.removeFromTop(extraY * 0.5 + gYSpacing);
    Rectangle<int> iterationsSliderSlice = area.removeFromTop(gComponentSingleSliderHeight);
    iterationsSliderSlice.removeFromLeft(gXSpacing + 2.*gPaddingConst * processor.paddingScalarX - gComponentSingleSliderXOffset);
    iterationsSliderSlice.removeFromRight(gXSpacing - gComponentSingleSliderXOffset);
    iterationsSlider->setBounds(iterationsSliderSlice);
    
    area.removeFromTop(extraY * 0.5 + gYSpacing);
    Rectangle<int> expToggleSlice = area.removeFromTop(gComponentSingleSliderHeight);
    expToggleSlice.removeFromLeft(gXSpacing + 2.*gPaddingConst * processor.paddingScalarX - gComponentSingleSliderXOffset);
    expToggleSlice.removeFromRight(gXSpacing - gComponentSingleSliderXOffset);
    exponentialToggle.setBounds(expToggleSlice);
    
    area.removeFromTop(extraY * 0.5 + gYSpacing);
    Rectangle<int> alphaSliderSlice = area.removeFromTop(gComponentSingleSliderHeight);
    alphaSliderSlice.removeFromLeft(gXSpacing + 2.*gPaddingConst * processor.paddingScalarX - gComponentSingleSliderXOffset);
    alphaSliderSlice.removeFromRight(gXSpacing - gComponentSingleSliderXOffset);
    emaAlphaSlider->setBounds(alphaSliderSlice);
    
    area.removeFromTop(extraY * 0.5 + gYSpacing);
    Rectangle<int> weightsToggleSlice = area.removeFromTop(gComponentSingleSliderHeight);
    weightsToggleSlice.removeFromLeft(gXSpacing + 2.*gPaddingConst * processor.paddingScalarX - gComponentSingleSliderXOffset);
    weightsToggleSlice.removeFromRight(gXSpacing - gComponentSingleSliderXOffset);
    weightsToggle.setBounds(weightsToggleSlice);
    
    area.removeFromTop(extraY * 0.5 + gYSpacing);
    Rectangle<int> weightsTextSlice = area.removeFromTop(gComponentSingleSliderHeight*3);
    weightsTextSlice.removeFromLeft(gXSpacing + 2.*gPaddingConst * processor.paddingScalarX - gComponentSingleSliderXOffset);
    weightsTextSlice.removeFromRight(gXSpacing - gComponentSingleSliderXOffset);
    weightsText.setBounds(weightsTextSlice);
    
    
    subSlider->setBounds(tempoSlider->getX(), tempoSlider->getBottom() + gYSpacing,
                         tempoSlider->getWidth(), tempoSlider->getHeight());
    
}


void TempoViewController::paint (Graphics& g)
{
    g.fillAll(Colours::black);
}
void TempoViewController::fillModeCB(void)
{
    
    modeCB.clear(dontSendNotification);
    
    for (int i = 0; i < cTempoModeTypes.size(); i++)
    {
        String name = cTempoModeTypes[i];
        modeCB.addItem(name, i+1);
    }
    
    modeCB.setSelectedItemIndex(0, dontSendNotification);
}

void TempoViewController::updateComponentVisibility()
{
    if(modeCB.getText() == "Adaptive Tempo")
    {
        TempoProcessor::Ptr mProcessor = processor.currentPiano->getTempoProcessor(processor.updateState->currentTempoId);
        
        bool keymapAttached = false;
        if (mProcessor->getKeymaps().size() > 0) keymapAttached = true;
        
        if (keymapAttached)
        {
            // keymap needs to be attached for adaptive tempo to work
            ATHistorySlider->setVisible(true);
            ATSubdivisionsSlider->setVisible(true);
            ATMinMaxSlider->setVisible(true);
            
            atModeLabel.setVisible(true);
            atModeButton.setVisible(true);
            
            A1AdaptedTempo.setVisible(true);
            A1AdaptedPeriodMultiplier.setVisible(true);
            
            A1reset.setVisible(true);
            
            subSlider->setVisible(false);
            
            attachKeymap.setVisible(false);
            
            exponentialToggle.setVisible(true);
            weightsToggle.setVisible(true);
            emaAlphaSlider->setVisible(true);
            
            iterationsSlider->setVisible(true);
        }
        else
        {
            // show message that keymap needs to be attached
            attachKeymap.setVisible(true);
        }

    }
    else
    {
        ATHistorySlider->setVisible(false);
        ATSubdivisionsSlider->setVisible(false);;
        ATMinMaxSlider->setVisible(false);
        
        atModeLabel.setVisible(false);
        atModeButton.setVisible(false);
        
        A1AdaptedTempo.setVisible(false);
        A1AdaptedPeriodMultiplier.setVisible(false);
        
        attachKeymap.setVisible(false);
        A1reset.setVisible(false);
        
        subSlider->setVisible(true);
        
        exponentialToggle.setVisible(false);
        weightsToggle.setVisible(false);
        emaAlphaSlider->setVisible(false);
        
        iterationsSlider->setVisible(false);
    }
    
}

#if JUCE_IOS
void TempoViewController::iWantTheBigOne(TextEditor* tf, String name)
{
    hideOrShow.setAlwaysOnTop(false);
    rightArrow.setAlwaysOnTop(false);
    leftArrow.setAlwaysOnTop(false);
    bigOne.display(tf, name, getBounds());
}
#endif



// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ TempoPreparationEditor ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~//

TempoPreparationEditor::TempoPreparationEditor(BKAudioProcessor& p, BKItemGraph* theGraph):
TempoViewController(p, theGraph)
{
    selectCB.addMyListener(this);
    selectCB.addListener(this);
    fillSelectCB(-1,-1);
    
    modeCB.addListener(this);
    
    tempoSlider->addMyListener(this);
    subSlider->addMyListener(this);
    atModeButton.addListener(this);
    A1reset.addListener(this);
    ATHistorySlider->addMyListener(this);
    ATSubdivisionsSlider->addMyListener(this);
    ATMinMaxSlider->addMyListener(this);
    
    emaAlphaSlider->addMyListener(this);
    
    iterationsSlider->addMyListener(this);
    
    startTimer(50);
    
    update();
}

void TempoPreparationEditor::timerCallback()
{
    if (processor.updateState->currentDisplay == DisplayTempo)
    {
        TempoProcessor::Ptr mProcessor = processor.currentPiano->getTempoProcessor(processor.updateState->currentTempoId);
        TempoPreparation::Ptr active = processor.gallery->getActiveTempoPreparation(processor.updateState->currentTempoId);
        
        if (mProcessor != nullptr)
        {
            if(active->getHostTempo()) tempoSlider->setValue(active->getTempo(), dontSendNotification);
            
            if(mProcessor->getPeriodMultiplier() != lastPeriodMultiplier)
            {
                lastPeriodMultiplier = mProcessor->getPeriodMultiplier();
                
                A1AdaptedTempo.setText("Tempo = " + String(mProcessor->getAdaptedTempo()), dontSendNotification);
                A1AdaptedPeriodMultiplier.setText("Period Multiplier = " + String(mProcessor->getPeriodMultiplier()), dontSendNotification);
            }
            
            if(mProcessor->getAtDelta() < active->getAdaptiveTempoMax())
                ATMinMaxSlider->setDisplayValue(mProcessor->getAtDelta());
            else
                ATMinMaxSlider->setDisplayValue(0);
        }
        
    }
    
}

void TempoPreparationEditor::fillSelectCB(int last, int current)
{
    selectCB.clear(dontSendNotification);
    
    for (auto prep : processor.gallery->getAllTempo())
    {
        int Id = prep->getId();
        
        if (Id == -1) continue;
        
        String name = prep->getName();
        
        if (name != String())  selectCB.addItem(name, Id);
        else                        selectCB.addItem("Tempo"+String(Id), Id);
        
        selectCB.setItemEnabled(Id, true);
        if (processor.currentPiano->isActive(PreparationTypeTempo, Id))
            selectCB.setItemEnabled(Id, false);
    }
    
    if (last != 0)      selectCB.setItemEnabled(last, true);
    if (current != 0)   selectCB.setItemEnabled(current, false);
    
    int selectedId = processor.updateState->currentTempoId;
    
    selectCB.setSelectedId(selectedId, NotificationType::dontSendNotification);
    
    selectCB.setItemEnabled(selectedId, false);
    
    lastId = selectedId;
}

int TempoPreparationEditor::addPreparation(void)
{
    processor.gallery->add(PreparationTypeTempo);
    
    return processor.gallery->getAllTempo().getLast()->getId();
}

int TempoPreparationEditor::duplicatePreparation(void)
{
    processor.gallery->duplicate(PreparationTypeTempo, processor.updateState->currentTempoId);
    
    return processor.gallery->getAllTempo().getLast()->getId();
}

void TempoPreparationEditor::deleteCurrent(void)
{
    int TempoId = selectCB.getSelectedId();
    int index = selectCB.getSelectedItemIndex();
    
    if ((index == 0) && (selectCB.getItemId(index+1) == -1)) return;
    
    processor.gallery->remove(PreparationTypeTempo, TempoId);
    
    fillSelectCB(0, 0);
    
    int newId = 0;
    
    selectCB.setSelectedId(newId, dontSendNotification);
    
    processor.updateState->currentTempoId = -1;
    setCurrentId(-1);
}

void TempoPreparationEditor::setCurrentId(int Id)
{
    processor.updateState->currentTempoId = Id;
    
    processor.updateState->idDidChange = true;
    
    update();
    
    fillSelectCB(lastId, Id);
    
    lastId = Id;
}

PopupMenu TempoPreparationEditor::getATModeMenu(void)
{
    PopupMenu menu;
    menu.setLookAndFeel(&buttonsAndMenusLAF);
    
    TempoPreparation::Ptr prep = processor.gallery->getActiveTempoPreparation(processor.updateState->currentTempoId);
    
    int id = 1;
    for (int mode = TimeBetweenOnsets; mode < AdaptiveTempoModeNil; mode++)
    {
        if (prep->getAdaptiveTempoMode().getUnchecked(mode))
            menu.addItem(PopupMenu::Item(cAdaptiveTempoModeTypes[mode]).setID(id++).setTicked(true));
        else menu.addItem(PopupMenu::Item(cAdaptiveTempoModeTypes[mode]).setID(id++));
    }
    return menu;
}

void TempoPreparationEditor::actionButtonCallback(int action, TempoPreparationEditor* vc)
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
        processor.reset(PreparationTypeTempo, processor.updateState->currentTempoId);
        vc->update();
    }
    else if (action == 5)
    {
        processor.clear(PreparationTypeTempo, processor.updateState->currentTempoId);
        vc->update();
    }
    else if (action == 6)
    {
        AlertWindow prompt("", "", AlertWindow::AlertIconType::QuestionIcon);
        
        int Id = processor.updateState->currentTempoId;
        Tempo::Ptr prep = processor.gallery->getTempo(Id);
        
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
        
        int Id = processor.updateState->currentTempoId;
        Tempo::Ptr prep = processor.gallery->getTempo(Id);
        
        prompt.addTextEditor("name", prep->getName());
        
        prompt.addButton("Ok", 1, KeyPress(KeyPress::returnKey));
        prompt.addButton("Cancel", 2, KeyPress(KeyPress::escapeKey));
        
        int result = prompt.runModalLoop();
        
        String name = prompt.getTextEditorContents("name");
        
        DBG("name: " + String(name));
        
        if (result == 1)
        {
            processor.exportPreparation(PreparationTypeTempo, Id, name);
        }
    }
    else if (action >= 100)
    {
        int which = action - 100;
        processor.importPreparation(PreparationTypeTempo, processor.updateState->currentTempoId, which);
        vc->update();
    }
}

void TempoPreparationEditor::atModeCallback(int result, TempoPreparationEditor* vc)
{
    if (vc == nullptr)
    {
        PopupMenu::dismissAllActiveMenus();
        return;
    }
    
    BKAudioProcessor& processor = vc->processor;
    TempoPreparation::Ptr prep = processor.gallery->getStaticTempoPreparation(processor.updateState->currentTempoId);
    TempoPreparation::Ptr active = processor.gallery->getActiveTempoPreparation(processor.updateState->currentTempoId);
    
    if (result <= 0) return;
    
    Array<bool> modes(active->getAdaptiveTempoMode());
    modes.set(result-1, !modes.getUnchecked(result-1));
    
    prep->setAdaptiveTempoMode(modes);
    active->setAdaptiveTempoMode(modes);
    
    vc->getATModeMenu().showMenuAsync(PopupMenu::Options().withTargetComponent(vc->atModeButton), ModalCallbackFunction::forComponent(atModeCallback, vc));
}


void TempoPreparationEditor::bkComboBoxDidChange (ComboBox* box)
{
    String name = box->getName();
    int index = box->getSelectedItemIndex();
    int Id = box->getSelectedId();
    
    TempoPreparation::Ptr prep = processor.gallery->getStaticTempoPreparation(processor.updateState->currentTempoId);
    TempoPreparation::Ptr active = processor.gallery->getActiveTempoPreparation(processor.updateState->currentTempoId);
    
    if (name == selectCB.getName())
    {
        setCurrentId(Id);
    }
    else if (name == modeCB.getName())
    {
        prep->setTempoSystem(TempoType(index));
        active->setTempoSystem(TempoType(index));
        updateComponentVisibility();
    }
}


void TempoPreparationEditor::BKEditableComboBoxChanged(String name, BKEditableComboBox* cb)
{
    Tempo::Ptr tempo = processor.gallery->getTempo(processor.updateState->currentTempoId);
    tempo->setName(name);
}

void TempoPreparationEditor::BKRangeSliderValueChanged(String name, double minval, double maxval)
{
    TempoPreparation::Ptr prep = processor.gallery->getStaticTempoPreparation(processor.updateState->currentTempoId);
    TempoPreparation::Ptr active = processor.gallery->getActiveTempoPreparation(processor.updateState->currentTempoId);
    
    if(name == ATMinMaxSlider->getName()) {
        DBG("got new AdaptiveTempo 1 time diff min/max " + String(minval) + " " + String(maxval));
        prep->setAdaptiveTempoMin(minval);
        prep->setAdaptiveTempoMax(maxval);
        active->setAdaptiveTempoMin(minval);
        active->setAdaptiveTempoMax(maxval);
    }
}

void TempoPreparationEditor::update(void)
{
    if (processor.updateState->currentTempoId < 0) return;
    
    TempoPreparation::Ptr prep = processor.gallery->getActiveTempoPreparation(processor.updateState->currentTempoId);
    
    if (prep != nullptr)
    {
        selectCB.setSelectedId(processor.updateState->currentTempoId, dontSendNotification);
        modeCB.setSelectedItemIndex((int)prep->getTempoSystem(), dontSendNotification);
        tempoSlider->setValue(prep->getTempo(), dontSendNotification);
        subSlider->setValue(prep->getSubdivisions(), dontSendNotification);
        DBG("tempoSlider set to " + String(prep->getTempo()));
        DBG("subSlider set to " + String(prep->getSubdivisions()));
        
        ATHistorySlider->setValue(prep->getAdaptiveTempoHistorySize(), dontSendNotification);
        ATSubdivisionsSlider->setValue(prep->getAdaptiveTempoSubdivisions(), dontSendNotification);
        ATMinMaxSlider->setMinValue(prep->getAdaptiveTempoMin(), dontSendNotification);
        ATMinMaxSlider->setMaxValue(prep->getAdaptiveTempoMax(), dontSendNotification);
        
        weightsText.setText(floatArrayToString(prep->getAdaptiveTempoWeights()));
        
        iterationsSlider->setValue(prep->getAdaptiveTempoIterations(), dontSendNotification);
        emaAlphaSlider->setValue(prep->getAdaptiveTempoAlpha(), dontSendNotification);
        exponentialToggle.setToggleState(prep->getUseExponential(), dontSendNotification);
        weightsToggle.setToggleState(prep->getUseWeights(), dontSendNotification);
        
        updateComponentVisibility();
    }
}


void TempoPreparationEditor::BKSingleSliderValueChanged(BKSingleSlider* slider, String name, double val)
{
    TempoPreparation::Ptr prep = processor.gallery->getStaticTempoPreparation(processor.updateState->currentTempoId);
    TempoPreparation::Ptr active = processor.gallery->getActiveTempoPreparation(processor.updateState->currentTempoId);;
    
    if(slider == tempoSlider.get()) {
        DBG("got tempo " + String(val));
        prep->setTempo(val);
        active->setTempo(val);
    }
    if(slider == subSlider.get()) {
        DBG("got sub " + String(val));
        prep->setSubdivisions(val);
        active->setSubdivisions(val);
    }
    else if(slider == ATHistorySlider.get()) {
        DBG("got A1History " + String(val));
        prep->setAdaptiveTempoHistory(val);
        active->setAdaptiveTempoHistory(val);
    }
    else if(slider == ATSubdivisionsSlider.get()) {
        DBG("got A1Subdivisions " + String(val));
        prep->setAdaptiveTempoSubdivisions(val);
        active->setAdaptiveTempoSubdivisions(val);
    }
    else if (slider == emaAlphaSlider.get())
    {
        prep->setAdaptiveTempoAlpha(val);
        active->setAdaptiveTempoAlpha(val);
    }
    else if (slider == iterationsSlider.get())
    {
        prep->setAdaptiveTempoIterations(val);
        active->setAdaptiveTempoIterations(val);
    }
    
}

void TempoPreparationEditor::buttonClicked (Button* b)
{
    TempoPreparation::Ptr prep = processor.gallery->getStaticTempoPreparation(processor.updateState->currentTempoId);
    TempoPreparation::Ptr active = processor.gallery->getActiveTempoPreparation(processor.updateState->currentTempoId);
    
    if (b == &A1reset)
    {
        DBG("resetting A1 tempo multiplier");
        
        TempoProcessor::Ptr tProcessor = processor.currentPiano->getTempoProcessor(processor.updateState->currentTempoId);
        tProcessor->reset();
    }
    else if (b == &hideOrShow)
    {
        processor.updateState->setCurrentDisplay(DisplayNil);
    }
    else if (b == &actionButton)
    {
        bool single = processor.gallery->getAllTempo().size() == 2;
        getPrepOptionMenu(PreparationTypeTempo, single).showMenuAsync (PopupMenu::Options().withTargetComponent (&actionButton), ModalCallbackFunction::forComponent (actionButtonCallback, this) );
    }
    else if (b == &atModeButton)
    {
        getATModeMenu().showMenuAsync(PopupMenu::Options().withTargetComponent(atModeButton), ModalCallbackFunction::forComponent(atModeCallback, this));
    }
    else if (b == &exponentialToggle)
    {
        prep->setUseExponential(exponentialToggle.getToggleState());
        active->setUseExponential(exponentialToggle.getToggleState());
    }
    else if (b == &weightsToggle)
    {
        prep->setUseWeights(weightsToggle.getToggleState());
        active->setUseWeights(weightsToggle.getToggleState());
    }
    
}

void TempoPreparationEditor::textEditorReturnKeyPressed(TextEditor& textEditor)
{
    TempoPreparation::Ptr prep = processor.gallery->getStaticTempoPreparation(processor.updateState->currentTempoId);
    TempoPreparation::Ptr active = processor.gallery->getActiveTempoPreparation(processor.updateState->currentTempoId);
    
    if(textEditor.getName() == weightsText.getName())
    {
        prep  ->setAdaptiveTempoWeights(stringToFloatArray(weightsText.getText()));
        active->setAdaptiveTempoWeights(stringToFloatArray(weightsText.getText()));
    }
}

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ TempoModificationEditor ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~//

TempoModificationEditor::TempoModificationEditor(BKAudioProcessor& p, BKItemGraph* theGraph):
TempoViewController(p, theGraph)
{
    
    A1reset.setVisible(false);
    
    fillSelectCB(-1,-1);
    
    selectCB.addMyListener(this);
    modeCB.addListener(this);
    tempoSlider->addMyListener(this);
    subSlider->addMyListener(this);
    ATHistorySlider->addMyListener(this);
    ATSubdivisionsSlider->addMyListener(this);
    ATMinMaxSlider->addMyListener(this);
    
    emaAlphaSlider->addMyListener(this);
    
    iterationsSlider->addMyListener(this);
    
    atModeButton.addListener(this);

    update();
}

void TempoModificationEditor::greyOutAllComponents()
{
    A1reset.setVisible(false);
    
    atModeLabel.setAlpha(gModAlpha);
    modeCB.setAlpha(gModAlpha);
    atModeButton.setAlpha(gModAlpha);
    tempoSlider->setDim(gModAlpha);
    subSlider->setDim(gModAlpha);
    ATHistorySlider->setDim(gModAlpha);
    ATSubdivisionsSlider->setDim(gModAlpha);
    ATMinMaxSlider->setDim(gModAlpha);
}

void TempoModificationEditor::highlightModedComponents()
{
    TempoModification::Ptr mod = processor.gallery->getTempoModification(processor.updateState->currentModTempoId);
    
    if(mod->getDirty(TempoBPM))           tempoSlider->setBright();
    if(mod->getDirty(TempoSubdivisions))  subSlider->setBright();
    if(mod->getDirty(TempoSystem))        modeCB.setAlpha(1);
    if(mod->getDirty(ATHistory))         ATHistorySlider->setBright();
    if(mod->getDirty(ATSubdivisions))    ATSubdivisionsSlider->setBright();
    if(mod->getDirty(ATMin))             ATMinMaxSlider->setBright();
    if(mod->getDirty(ATMax))             ATMinMaxSlider->setBright();
    if(mod->getDirty(ATMode))            { atModeButton.setAlpha(1.);  atModeLabel.setAlpha(1.); }

}

void TempoModificationEditor::fillSelectCB(int last, int current)
{
    selectCB.clear(dontSendNotification);
    
    for (auto prep : processor.gallery->getTempoModifications())
    {
        int Id = prep->getId();;
        String name = prep->getName();
        
        if (name != String())  selectCB.addItem(name, Id);
        else                        selectCB.addItem("TempoMod"+String(Id), Id);
        
        selectCB.setItemEnabled(Id, true);
        if (processor.currentPiano->isActive(PreparationTypeTempo, Id))
            selectCB.setItemEnabled(Id, false);
    }
    
    if (last != 0)      selectCB.setItemEnabled(last, true);
    if (current != 0)   selectCB.setItemEnabled(current, false);
    
    int selectedId = processor.updateState->currentTempoId;
    
    selectCB.setSelectedId(selectedId, NotificationType::dontSendNotification);
    
    selectCB.setItemEnabled(selectedId, false);
    
    lastId = selectedId;
}


void TempoModificationEditor::update(void)
{
    selectCB.setSelectedId(processor.updateState->currentModTempoId, dontSendNotification);
    
    TempoModification::Ptr mod = processor.gallery->getTempoModification(processor.updateState->currentModTempoId);
    
    if (mod != nullptr)
    {
        
        greyOutAllComponents();
        highlightModedComponents();
        
        modeCB.setSelectedItemIndex(mod->getTempoSystem(), dontSendNotification);

        tempoSlider->setValue(mod->getTempo(), dontSendNotification);
        
        subSlider->setValue(mod->getSubdivisions(), dontSendNotification);
        
        ATHistorySlider->setValue(mod->getAdaptiveTempoHistorySize(), dontSendNotification);
        
        ATSubdivisionsSlider->setValue(mod->getAdaptiveTempoSubdivisions(), dontSendNotification);
        
        ATMinMaxSlider->setMinValue(mod->getAdaptiveTempoMin(), dontSendNotification);
        
        ATMinMaxSlider->setMaxValue(mod->getAdaptiveTempoMax(), dontSendNotification);
        
        weightsText.setText(floatArrayToString(mod->getAdaptiveTempoWeights()));
        
        iterationsSlider->setValue(mod->getAdaptiveTempoIterations(), dontSendNotification);
        emaAlphaSlider->setValue(mod->getAdaptiveTempoAlpha(), dontSendNotification);
        exponentialToggle.setToggleState(mod->getUseExponential(), dontSendNotification);
        weightsToggle.setToggleState(mod->getUseWeights(), dontSendNotification);
        
        updateComponentVisibility();
    }
    
}


int TempoModificationEditor::addPreparation(void)
{
    processor.gallery->add(PreparationTypeTempoMod);
    
    return processor.gallery->getTempoModifications().getLast()->getId();
}

int TempoModificationEditor::duplicatePreparation(void)
{
    processor.gallery->duplicate(PreparationTypeTempoMod, processor.updateState->currentModTempoId);
    
    return processor.gallery->getTempoModifications().getLast()->getId();
}

void TempoModificationEditor::deleteCurrent(void)
{
    int oldId = selectCB.getSelectedId();
    int index = selectCB.getSelectedItemIndex();
    
    if ((index == 0) && (selectCB.getItemId(index+1) == -1)) return;
    
    processor.gallery->remove(PreparationTypeTempoMod, oldId);
    
    fillSelectCB(0, 0);
    
    int newId = 0;
    
    selectCB.setSelectedId(newId, dontSendNotification);
    
    processor.updateState->currentModTempoId = -1;
    setCurrentId(-1);
}

void TempoModificationEditor::setCurrentId(int Id)
{
    processor.updateState->currentModTempoId = Id;
    
    processor.updateState->idDidChange = true;
    
    update();
    
    fillSelectCB(lastId, Id);
    
    lastId = Id;
}

PopupMenu TempoModificationEditor::getATModeMenu(void)
{
    PopupMenu menu;
    menu.setLookAndFeel(&buttonsAndMenusLAF);
    
    TempoModification::Ptr mod = processor.gallery->getTempoModification(processor.updateState->currentModTempoId);
    
    int id = 1;
    for (int mode = TimeBetweenOnsets; mode < AdaptiveTempoModeNil; mode++)
    {
        if (mod->getAdaptiveTempoMode().getUnchecked(mode))
            menu.addItem(PopupMenu::Item(cAdaptiveTempoModeTypes[mode]).setID(id++).setTicked(true));
        else menu.addItem(PopupMenu::Item(cAdaptiveTempoModeTypes[mode]).setID(id++));
    }
    return menu;
}

void TempoModificationEditor::actionButtonCallback(int action, TempoModificationEditor* vc)
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
        processor.clear(PreparationTypeTempoMod, processor.updateState->currentModTempoId);
        vc->update();
        vc->updateModification();
    }
    else if (action == 6)
    {
        AlertWindow prompt("", "", AlertWindow::AlertIconType::QuestionIcon);
        
        int Id = processor.updateState->currentModTempoId;
        TempoModification::Ptr prep = processor.gallery->getTempoModification(Id);
        
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
        
        int Id = processor.updateState->currentModTempoId;
        TempoModification::Ptr prep = processor.gallery->getTempoModification(Id);
        
        prompt.addTextEditor("name", prep->getName());
        
        prompt.addButton("Ok", 1, KeyPress(KeyPress::returnKey));
        prompt.addButton("Cancel", 2, KeyPress(KeyPress::escapeKey));
        
        int result = prompt.runModalLoop();
        
        String name = prompt.getTextEditorContents("name");
        
        DBG("name: " + String(name));
        
        if (result == 1)
        {
            processor.exportPreparation(PreparationTypeTempoMod, Id, name);
        }
    }
    else if (action >= 100)
    {
        int which = action - 100;
        processor.importPreparation(PreparationTypeTempoMod, processor.updateState->currentModTempoId, which);
        vc->update();
    }
}

void TempoModificationEditor::atModeCallback(int result, TempoModificationEditor* vc)
{
    if (vc == nullptr)
    {
        PopupMenu::dismissAllActiveMenus();
        return;
    }
    
    BKAudioProcessor& processor = vc->processor;
    TempoModification::Ptr mod = processor.gallery->getTempoModification(processor.updateState->currentModTempoId);
    
    if (result <= 0) return;
    
    Array<bool> modes(mod->getAdaptiveTempoMode());
    modes.set(result-1, !modes.getUnchecked(result-1));
    
    mod->setAdaptiveTempoMode(modes);
    mod->setDirty(ATMode);
    
    vc->atModeButton.setAlpha(1.);
    vc->atModeLabel.setAlpha(1.);
    
    vc->getATModeMenu().showMenuAsync(PopupMenu::Options().withTargetComponent(vc->atModeButton), ModalCallbackFunction::forComponent(atModeCallback, vc));
}



void TempoModificationEditor::bkComboBoxDidChange (ComboBox* box)
{
    String name = box->getName();
    int index = box->getSelectedItemIndex();
    int Id = box->getSelectedId();
    
    TempoModification::Ptr mod = processor.gallery->getTempoModification(processor.updateState->currentModTempoId);
    
    if (name == selectCB.getName())
    {
        setCurrentId(Id);
    }
    else if (name == modeCB.getName())
    {
        mod->setTempoSystem((TempoType) index);
        mod->setDirty(TempoSystem);
        
        modeCB.setAlpha(1.);
        updateComponentVisibility();
    }
    
    if (name != selectCB.getName()) updateModification();
    
}


void TempoModificationEditor::BKEditableComboBoxChanged(String name, BKEditableComboBox* cb)
{
    processor.gallery->getTempo(processor.updateState->currentModTempoId)->setName(name);
    
    updateModification();
}

void TempoModificationEditor::BKRangeSliderValueChanged(String name, double minval, double maxval)
{
    TempoModification::Ptr mod = processor.gallery->getTempoModification(processor.updateState->currentModTempoId);

    if(name == ATMinMaxSlider->getName())
    {
        mod->setAdaptiveTempoMin(minval);
        mod->setDirty(ATMin);
        
        mod->setAdaptiveTempoMax(maxval);
        mod->setDirty(ATMax);
        
        ATMinMaxSlider->setBright();
    }
    
    updateModification();
}



void TempoModificationEditor::BKSingleSliderValueChanged(BKSingleSlider* slider, String name, double val)
{
    TempoModification::Ptr mod = processor.gallery->getTempoModification(processor.updateState->currentModTempoId);
    
    if(slider == tempoSlider.get())
    {
        mod->setTempo(val);
        mod->setDirty(TempoBPM);
        tempoSlider->setBright();
    }
    else if(slider == subSlider.get())
    {
        mod->setSubdivisions(val);
        mod->setDirty(TempoSubdivisions);
        subSlider->setBright();
    }
    else if(slider == ATHistorySlider.get())
    {
        mod->setAdaptiveTempoHistory(val);
        mod->setDirty(ATHistory);
        ATHistorySlider->setBright();
    }
    else if(slider == ATSubdivisionsSlider.get())
    {
        mod->setAdaptiveTempoSubdivisions(val);
        mod->setDirty(ATSubdivisions);
        ATSubdivisionsSlider->setBright();
    }
    else if (slider == emaAlphaSlider.get())
    {
        mod->setAdaptiveTempoAlpha(val);
        mod->setDirty(ATAlpha);
        emaAlphaSlider->setBright();
    }
    else if (slider == iterationsSlider.get())
    {
        mod->setAdaptiveTempoIterations(val);
        mod->setDirty(ATIterations);
        iterationsSlider->setBright();
    }
    
    updateModification();
    
}

void TempoModificationEditor::updateModification(void)
{
    processor.updateState->modificationDidChange = true;
}

void TempoModificationEditor::buttonClicked (Button* b)
{
    TempoModification::Ptr mod = processor.gallery->getTempoModification(processor.updateState->currentModTempoId);
    
    if (b == &A1reset)
    {
        
    }
    else if (b == &hideOrShow)
    {
        processor.updateState->setCurrentDisplay(DisplayNil);
    }
    else if (b == &actionButton)
    {
        bool single = processor.gallery->getTempoModifications().size() == 2;
        getModOptionMenu(PreparationTypeTempoMod, single).showMenuAsync (PopupMenu::Options().withTargetComponent (&actionButton), ModalCallbackFunction::forComponent (actionButtonCallback, this) );
    }
    else if (b == &atModeButton)
    {
        getATModeMenu().showMenuAsync(PopupMenu::Options().withTargetComponent(atModeButton), ModalCallbackFunction::forComponent(atModeCallback, this));
    }
    else if (b == &exponentialToggle)
    {
        mod->setUseExponential(exponentialToggle.getToggleState());
        mod->setDirty(ATExponential);
//        exponentialToggle.setBright();
    }
    else if (b == &weightsToggle)
    {
        mod->setUseWeights(weightsToggle.getToggleState());
        mod->setDirty(ATWeighted);
//        weightsToggle.setBright();
    }
    
}

void TempoModificationEditor::textEditorReturnKeyPressed(TextEditor& textEditor)
{
    if(textEditor.getName() == weightsText.getName())
    {
        
        
    }
}

