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
    
    AT1HistorySlider = std::make_unique<BKSingleSlider>("History", 1, 10, 4, 1);
    AT1HistorySlider->setJustifyRight(false);
    AT1HistorySlider->setToolTipString("Indicates how many notes Tempo is using to determine and generate an average pulse ");
    addAndMakeVisible(*AT1HistorySlider);
    
    AT1SubdivisionsSlider = std::make_unique<BKSingleSlider>("Subdivisions", 0., 12, 1, 0.01);
    AT1SubdivisionsSlider->setJustifyRight(false);
    AT1SubdivisionsSlider->setToolTipString("Multiplies tempo by interpreting rhythmic value of played notes; values less than 1 will result in tempos slower than what is played, values greater than 1 will result in tempos faster than what is played");
    addAndMakeVisible(*AT1SubdivisionsSlider);
    
    AT1MinMaxSlider = std::make_unique<BKRangeSlider>("Min/Max (ms)", 1, 2000, 100, 500, 10);
    AT1MinMaxSlider->setJustifyRight(false);
    AT1MinMaxSlider->setIsMinAlwaysLessThanMax(true);
    AT1MinMaxSlider->setToolTipString("Time within which Tempo will consider notes to be part of a constant pulse; any notes played futher apart than Max, or closer together than Min, will be ignored");
    addAndMakeVisible(*AT1MinMaxSlider);
    
    A1ModeCB.setName("AT1Mode");
    addAndMakeVisible(A1ModeCB);
    fillA1ModeCB();
    A1ModeLabel.setText("Mode", dontSendNotification);
    A1ModeCB.setTooltip("Indicates which aspect of performance Tempo is analyzing, using information from connected Keymap");
    addAndMakeVisible(A1ModeLabel);
    
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
    AT1MinMaxSlider->addWantsBigOneListener(this);
    AT1HistorySlider->addWantsBigOneListener(this);
    AT1SubdivisionsSlider->addWantsBigOneListener(this);
    tempoSlider->addWantsBigOneListener(this);
    subSlider->addWantsBigOneListener(this);
#endif

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
    Rectangle<int> A1ModeCBSlice = leftColumn.removeFromTop(gComponentComboBoxHeight);
    attachKeymap.setBounds(leftColumn);
    A1ModeCBSlice.removeFromRight(gXSpacing + 2.*gPaddingConst * processor.paddingScalarX);
    //A1ModeCBSlice.removeFromLeft(2 * gXSpacing + hideOrShow.getWidth());
    A1ModeCBSlice.removeFromLeft(gXSpacing);
    A1ModeCB.setBounds(A1ModeCBSlice.removeFromLeft(selectCB.getWidth() + gXSpacing + hideOrShow.getWidth()));
    A1ModeLabel.setBounds(A1ModeCBSlice);
    
    leftColumn.removeFromTop(extraY + gYSpacing);
    AT1HistorySlider->setBounds(leftColumn.removeFromTop(gComponentSingleSliderHeight));
    
    leftColumn.removeFromTop(extraY + gYSpacing);
    AT1SubdivisionsSlider->setBounds(leftColumn.removeFromTop(gComponentSingleSliderHeight));
    
    leftColumn.removeFromTop(extraY + gYSpacing);
    AT1MinMaxSlider->setBounds(leftColumn.removeFromTop(gComponentRangeSliderHeight));
    
    leftColumn.removeFromTop(extraY + gYSpacing);
    Rectangle<int> adaptedLabelSlice = leftColumn.removeFromTop(gComponentTextFieldHeight);
    A1AdaptedTempo.setBounds(adaptedLabelSlice.removeFromLeft(leftColumn.getWidth() / 2.));
    A1AdaptedPeriodMultiplier.setBounds(adaptedLabelSlice);
    
    
    
    // ********* right column
    
    Rectangle<int> modeSlice = area.removeFromTop(gComponentComboBoxHeight);
    modeSlice.removeFromRight(gXSpacing);
    modeCB.setBounds(modeSlice.removeFromRight(modeSlice.getWidth() / 2.));
    
    area.removeFromTop(A1ModeCB.getY() - selectCB.getBottom());
    Rectangle<int> tempoSliderSlice = area.removeFromTop(gComponentSingleSliderHeight);
    tempoSliderSlice.removeFromLeft(gXSpacing + 2.*gPaddingConst * processor.paddingScalarX - gComponentSingleSliderXOffset);
    tempoSliderSlice.removeFromRight(gXSpacing - gComponentSingleSliderXOffset);
    tempoSlider->setBounds(tempoSliderSlice);
    
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


void TempoViewController::fillA1ModeCB(void)
{
    
    A1ModeCB.clear(dontSendNotification);
    
    for (int i = 0; i < cAdaptiveTempoModeTypes.size(); i++)
    {
        String name = cAdaptiveTempoModeTypes[i];
        A1ModeCB.addItem(name, i+1);
    }
    
    A1ModeCB.setSelectedItemIndex(0, dontSendNotification);
}

void TempoViewController::updateComponentVisibility()
{
    if(modeCB.getText() == "Adaptive Tempo 1")
    {
        TempoProcessor::Ptr mProcessor = processor.currentPiano->getTempoProcessor(processor.updateState->currentTempoId);
        
        bool keymapAttached = false;
        if (mProcessor->getKeymaps().size() > 0) keymapAttached = true;
        
        if (keymapAttached)
        {
            // keymap needs to be attached for adaptive tempo to work
            AT1HistorySlider->setVisible(true);
            AT1SubdivisionsSlider->setVisible(true);
            AT1MinMaxSlider->setVisible(true);
            
            A1ModeLabel.setVisible(true);
            A1ModeCB.setVisible(true);
            
            A1AdaptedTempo.setVisible(true);
            A1AdaptedPeriodMultiplier.setVisible(true);
            
            A1reset.setVisible(true);
            
            subSlider->setVisible(false);
            
            attachKeymap.setVisible(false);
        }
        else
        {
            // show message that keymap needs to be attached
            attachKeymap.setVisible(true);
        }

    }
    else
    {
        AT1HistorySlider->setVisible(false);
        AT1SubdivisionsSlider->setVisible(false);;
        AT1MinMaxSlider->setVisible(false);
        
        A1ModeLabel.setVisible(false);
        A1ModeCB.setVisible(false);
        
        A1AdaptedTempo.setVisible(false);
        A1AdaptedPeriodMultiplier.setVisible(false);
        
        attachKeymap.setVisible(false);
        A1reset.setVisible(false);
        
        subSlider->setVisible(true);
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
    A1ModeCB.addListener(this);
    A1reset.addListener(this);
    AT1HistorySlider->addMyListener(this);
    AT1SubdivisionsSlider->addMyListener(this);
    AT1MinMaxSlider->addMyListener(this);
    
    startTimer(50);
    
    update();
}

void TempoPreparationEditor::timerCallback()
{
    if (processor.updateState->currentDisplay == DisplayTempo)
    {
        TempoProcessor::Ptr mProcessor = processor.currentPiano->getTempoProcessor(processor.updateState->currentTempoId);
        TempoPreparation::Ptr prep = processor.gallery->getTempoPreparation(processor.updateState->currentTempoId);
        
        if (mProcessor != nullptr)
        {
            if(prep->getHostTempo()) tempoSlider->setValue(prep->getTempo(), dontSendNotification);
            
            if(mProcessor->getPeriodMultiplier() != lastPeriodMultiplier)
            {
                lastPeriodMultiplier = mProcessor->getPeriodMultiplier();
                
                A1AdaptedTempo.setText("Tempo = " + String(mProcessor->getAdaptedTempo()), dontSendNotification);
                A1AdaptedPeriodMultiplier.setText("Period Multiplier = " + String(mProcessor->getPeriodMultiplier()), dontSendNotification);
            }
            
            if(mProcessor->getAtDelta() < prep->getAdaptiveTempo1Max())
                AT1MinMaxSlider->setDisplayValue(mProcessor->getAtDelta());
            else
                AT1MinMaxSlider->setDisplayValue(0);
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
    
    selectCB.setSelectedItemIndex(0, dontSendNotification);
    int newId = selectCB.getSelectedId();

    setCurrentId(newId);
}

void TempoPreparationEditor::setCurrentId(int Id)
{
    processor.updateState->currentTempoId = Id;
    
    processor.updateState->idDidChange = true;
    
    update();
    
    fillSelectCB(lastId, Id);
    
    lastId = Id;
}

void TempoPreparationEditor::actionButtonCallback(int action, TempoPreparationEditor* vc)
{
    if (vc == nullptr)
    {
        PopupMenu::dismissAllActiveMenus();
        return;
    }
    
    BKAudioProcessor& processor = vc->processor;
    if (action == 1)
    {
        int Id = vc->addPreparation();
        vc->setCurrentId(Id);
        processor.saveGalleryToHistory("New Tempo Preparation");
    }
    else if (action == 2)
    {
        int Id = vc->duplicatePreparation();
        vc->setCurrentId(Id);
        processor.saveGalleryToHistory("Duplicate Tempo Preparation");
    }
    else if (action == 3)
    {
        vc->deleteCurrent();
        processor.saveGalleryToHistory("Delete Tempo Preparation");
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
        processor.saveGalleryToHistory("Clear Tempo Preparation");
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
            processor.saveGalleryToHistory("Rename Tempo Preparation");
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
        processor.saveGalleryToHistory("Import Tempo Preparation");
    }
}


void TempoPreparationEditor::bkComboBoxDidChange (ComboBox* box)
{
    String name = box->getName();
    int index = box->getSelectedItemIndex();
    int Id = box->getSelectedId();
    
    TempoPreparation::Ptr prep = processor.gallery->getTempoPreparation(processor.updateState->currentTempoId);
    
    if (name == selectCB.getName())
    {
        setCurrentId(Id);
    }
    else if (name == modeCB.getName())
    {
        prep->setTempoSystem(TempoType(index));
        updateComponentVisibility();
    }
    else if (name == A1ModeCB.getName())
    {
        DBG("A1ModeCB = " + String(index));
        prep->setAdaptiveTempo1Mode((AdaptiveTempo1Mode) index);
    }
    
    processor.updateState->editsMade = true;
}


void TempoPreparationEditor::BKEditableComboBoxChanged(String name, BKEditableComboBox* cb)
{
    Tempo::Ptr tempo = processor.gallery->getTempo(processor.updateState->currentTempoId);
    tempo->setName(name);
    
    processor.updateState->editsMade = true;
}

void TempoPreparationEditor::BKRangeSliderValueChanged(String name, double minval, double maxval)
{
    TempoPreparation::Ptr prep = processor.gallery->getTempoPreparation(processor.updateState->currentTempoId);
    
    if(name == AT1MinMaxSlider->getName()) {
        DBG("got new AdaptiveTempo 1 time diff min/max " + String(minval) + " " + String(maxval));
        prep->setAdaptiveTempo1Min(minval);
        prep->setAdaptiveTempo1Max(maxval);
    }
    
    processor.updateState->editsMade = true;
}

void TempoPreparationEditor::update(void)
{
    if (processor.updateState->currentTempoId < 0) return;
    
    TempoPreparation::Ptr prep = processor.gallery->getTempoPreparation(processor.updateState->currentTempoId);
    
    if (prep != nullptr)
    {
        selectCB.setSelectedId(processor.updateState->currentTempoId, dontSendNotification);
        modeCB.setSelectedItemIndex((int)prep->getTempoSystem(), dontSendNotification);
        tempoSlider->setValue(prep->getTempo(), dontSendNotification);
        subSlider->setValue(prep->getSubdivisions(), dontSendNotification);
        DBG("tempoSlider set to " + String(prep->getTempo()));
        DBG("subSlider set to " + String(prep->getSubdivisions()));
        
        A1ModeCB.setSelectedItemIndex(prep->getAdaptiveTempo1Mode(), dontSendNotification);
        AT1HistorySlider->setValue(prep->getAdaptiveTempo1History(), dontSendNotification);
        AT1SubdivisionsSlider->setValue(prep->getAdaptiveTempo1Subdivisions(), dontSendNotification);
        AT1MinMaxSlider->setMinValue(prep->getAdaptiveTempo1Min(), dontSendNotification);
        AT1MinMaxSlider->setMaxValue(prep->getAdaptiveTempo1Max(), dontSendNotification);
        
        updateComponentVisibility();
    }
}


void TempoPreparationEditor::BKSingleSliderValueChanged(BKSingleSlider* slider, String name, double val)
{
    TempoPreparation::Ptr prep = processor.gallery->getTempoPreparation(processor.updateState->currentTempoId);
    
    if(slider == tempoSlider.get()) {
        DBG("got tempo " + String(val));
        prep->setTempo(val);
    }
    if(slider == subSlider.get()) {
        DBG("got sub " + String(val));
        prep->setSubdivisions(val);
    }
    else if(slider == AT1HistorySlider.get()) {
        DBG("got A1History " + String(val));
        prep->setAdaptiveTempo1History(val);
    }
    else if(slider == AT1SubdivisionsSlider.get()) {
        DBG("got A1Subdivisions " + String(val));
        prep->setAdaptiveTempo1Subdivisions(val);
    }
    
    processor.updateState->editsMade = true;
}

void TempoPreparationEditor::buttonClicked (Button* b)
{
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
    AT1HistorySlider->addMyListener(this);
    AT1SubdivisionsSlider->addMyListener(this);
    AT1MinMaxSlider->addMyListener(this);
    A1ModeCB.addListener(this);

    update();
}

void TempoModificationEditor::greyOutAllComponents()
{
    A1reset.setVisible(false);
    
    A1ModeLabel.setAlpha(gModAlpha);
    modeCB.setAlpha(gModAlpha);
    A1ModeCB.setAlpha(gModAlpha);
    tempoSlider->setDim(gModAlpha);
    subSlider->setDim(gModAlpha);
    AT1HistorySlider->setDim(gModAlpha);
    AT1SubdivisionsSlider->setDim(gModAlpha);
    AT1MinMaxSlider->setDim(gModAlpha);
}

void TempoModificationEditor::highlightModedComponents()
{
    TempoModification::Ptr mod = processor.gallery->getTempoModification(processor.updateState->currentModTempoId);
    
    if(mod->getDirty(TempoBPM))           tempoSlider->setBright();
    if(mod->getDirty(TempoSubdivisions))  subSlider->setBright();
    if(mod->getDirty(TempoSystem))        modeCB.setAlpha(1);
    if(mod->getDirty(AT1History))         AT1HistorySlider->setBright();
    if(mod->getDirty(AT1Subdivisions))    AT1SubdivisionsSlider->setBright();
    if(mod->getDirty(AT1Min))             AT1MinMaxSlider->setBright();
    if(mod->getDirty(AT1Max))             AT1MinMaxSlider->setBright();
    if(mod->getDirty(AT1Mode))            { A1ModeCB.setAlpha(1.);  A1ModeLabel.setAlpha(1.); }

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
        
        A1ModeCB.setSelectedItemIndex(mod->getAdaptiveTempo1Mode(), dontSendNotification);
        
        AT1HistorySlider->setValue(mod->getAdaptiveTempo1History(), dontSendNotification);
        
        AT1SubdivisionsSlider->setValue(mod->getAdaptiveTempo1Subdivisions(), dontSendNotification);
        
        AT1MinMaxSlider->setMinValue(mod->getAdaptiveTempo1Min(), dontSendNotification);
        
        AT1MinMaxSlider->setMaxValue(mod->getAdaptiveTempo1Max(), dontSendNotification);
        
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
    
    selectCB.setSelectedItemIndex(0, dontSendNotification);
    int newId = selectCB.getSelectedId();

    setCurrentId(newId);
}

void TempoModificationEditor::setCurrentId(int Id)
{
    processor.updateState->currentModTempoId = Id;
    
    processor.updateState->idDidChange = true;
    
    update();
    
    fillSelectCB(lastId, Id);
    
    lastId = Id;
}

void TempoModificationEditor::actionButtonCallback(int action, TempoModificationEditor* vc)
{
    if (vc == nullptr)
    {
        PopupMenu::dismissAllActiveMenus();
        return;
    }
    
    BKAudioProcessor& processor = vc->processor;
    if (action == 1)
    {
        int Id = vc->addPreparation();
        vc->setCurrentId(Id);
        processor.saveGalleryToHistory("New Tempo Modification");
    }
    else if (action == 2)
    {
        int Id = vc->duplicatePreparation();
        vc->setCurrentId(Id);
        processor.saveGalleryToHistory("Duplicate Tempo Modification");
    }
    else if (action == 3)
    {
        vc->deleteCurrent();
        processor.saveGalleryToHistory("Delete Tempo Modification");
    }
    else if (action == 5)
    {
        processor.clear(PreparationTypeTempoMod, processor.updateState->currentModTempoId);
        vc->update();
        vc->updateModification();
        processor.saveGalleryToHistory("Clear Tempo Modification");
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
            processor.saveGalleryToHistory("Rename Tempo Modification");
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
        processor.saveGalleryToHistory("Import Tempo Modification");
    }
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
    else if (name == A1ModeCB.getName())
    {
        mod->setAdaptiveTempo1Mode((AdaptiveTempo1Mode) index);
        mod->setDirty(AT1Mode);
        
        A1ModeCB.setAlpha(1.);
        A1ModeLabel.setAlpha(1.);
    }
    
    if (name != selectCB.getName()) updateModification();
    
    processor.updateState->editsMade = true;
}


void TempoModificationEditor::BKEditableComboBoxChanged(String name, BKEditableComboBox* cb)
{
    processor.gallery->getTempo(processor.updateState->currentModTempoId)->setName(name);
    
    updateModification();
}

void TempoModificationEditor::BKRangeSliderValueChanged(String name, double minval, double maxval)
{
    TempoModification::Ptr mod = processor.gallery->getTempoModification(processor.updateState->currentModTempoId);

    if(name == AT1MinMaxSlider->getName())
    {
        mod->setAdaptiveTempo1Min(minval);
        mod->setDirty(AT1Min);
        
        mod->setAdaptiveTempo1Max(maxval);
        mod->setDirty(AT1Max);
        
        AT1MinMaxSlider->setBright();
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
    else if(slider == AT1HistorySlider.get())
    {
        mod->setAdaptiveTempo1History(val);
        mod->setDirty(AT1History);
        AT1HistorySlider->setBright();
    }
    else if(slider == AT1SubdivisionsSlider.get())
    {
        mod->setAdaptiveTempo1Subdivisions(val);
        mod->setDirty(AT1Subdivisions);
        AT1SubdivisionsSlider->setBright();
    }
    
    updateModification();
    
}

void TempoModificationEditor::updateModification(void)
{
    processor.updateState->modificationDidChange = true;
    processor.updateState->editsMade = true;
}

void TempoModificationEditor::buttonClicked (Button* b)
{
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
    
}

