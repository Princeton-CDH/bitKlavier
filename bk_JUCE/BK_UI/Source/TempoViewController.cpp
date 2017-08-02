/*
  ==============================================================================

    TempoViewController.cpp
    Created: 17 Jun 2017 5:29:25pm
    Author:  Daniel Trueman

  ==============================================================================
*/

#include "TempoViewController.h"


TempoViewController::TempoViewController(BKAudioProcessor& p, BKItemGraph* theGraph):
BKViewController(p, theGraph)
{
    
    setLookAndFeel(&buttonsAndMenusLAF);
    
    iconImageComponent.setImage(ImageCache::getFromMemory(BinaryData::tempo_icon_png, BinaryData::tempo_icon_pngSize));
    iconImageComponent.setImagePlacement(RectanglePlacement(juce::RectanglePlacement::stretchToFit));
    iconImageComponent.setAlpha(0.095);
    addAndMakeVisible(iconImageComponent);
    
    selectCB.setName("Tempo");
    selectCB.addSeparator();
    selectCB.setSelectedItemIndex(0);
    addAndMakeVisible(selectCB);
    
    modeCB.setName("Mode");
    modeCB.BKSetJustificationType(juce::Justification::centredRight);
    fillModeCB();
    addAndMakeVisible(modeCB);
    
    tempoSlider = new BKSingleSlider("Tempo", 40, 208, 100, 0.01);
    addAndMakeVisible(tempoSlider);
    
    AT1HistorySlider = new BKSingleSlider("History", 1, 10, 4, 1);
    AT1HistorySlider->setJustifyRight(false);
    addAndMakeVisible(AT1HistorySlider);
    
    AT1SubdivisionsSlider = new BKSingleSlider("Subdivisions", 0., 12, 1, 0.01);
    AT1SubdivisionsSlider->setJustifyRight(false);
    addAndMakeVisible(AT1SubdivisionsSlider);
    
    AT1MinMaxSlider = new BKRangeSlider("Min/Max (ms)", 1, 2000, 100, 500, 10);
    AT1MinMaxSlider->setJustifyRight(false);
    AT1MinMaxSlider->setIsMinAlwaysLessThanMax(true);
    addAndMakeVisible(AT1MinMaxSlider);
    
    A1ModeCB.setName("Adaptive Mode");
    addAndMakeVisible(A1ModeCB);
    fillA1ModeCB();
    A1ModeLabel.setText("Adaptive Mode", dontSendNotification);
    addAndMakeVisible(A1ModeLabel);
    
    addAndMakeVisible(A1AdaptedTempo);
    addAndMakeVisible(A1AdaptedPeriodMultiplier);
    A1AdaptedPeriodMultiplier.setJustificationType(juce::Justification::centredRight);
    
    A1reset.setButtonText("reset");
    addAndMakeVisible(A1reset);
    
    addAndMakeVisible(hideOrShow);
    hideOrShow.setName("hideOrShow");
    hideOrShow.setButtonText(" X ");
    
    
    updateComponentVisibility();
}


void TempoViewController::resized()
{
    Rectangle<int> area (getLocalBounds());
    
    float paddingScalarX = (float)(getTopLevelComponent()->getWidth() - gMainComponentMinWidth) / (gMainComponentWidth - gMainComponentMinWidth);
    float paddingScalarY = (float)(getTopLevelComponent()->getHeight() - gMainComponentMinHeight) / (gMainComponentHeight - gMainComponentMinHeight);
    
    iconImageComponent.setBounds(area);
    area.reduce(10 * paddingScalarX + 4, 10 * paddingScalarY + 4);
    
    Rectangle<int> leftColumn = area.removeFromLeft(area.getWidth() * 0.5);
    Rectangle<int> comboBoxSlice = leftColumn.removeFromTop(gComponentComboBoxHeight);
    comboBoxSlice.removeFromRight(gXSpacing + 2.*gPaddingConst * paddingScalarX);
    comboBoxSlice.removeFromLeft(gXSpacing);
    hideOrShow.setBounds(comboBoxSlice.removeFromLeft(gComponentComboBoxHeight));
    comboBoxSlice.removeFromLeft(gXSpacing);
    selectCB.setBounds(comboBoxSlice.removeFromLeft(comboBoxSlice.getWidth() / 2.));
    comboBoxSlice.removeFromLeft(gXSpacing);
    A1reset.setBounds(comboBoxSlice.removeFromLeft(90));
    clearModsButton.setBounds(A1reset.getBounds());
    
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
    A1ModeCBSlice.removeFromRight(gXSpacing + 2.*gPaddingConst * paddingScalarX);
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
    tempoSliderSlice.removeFromLeft(gXSpacing + 2.*gPaddingConst * paddingScalarX - gComponentSingleSliderXOffset);
    tempoSliderSlice.removeFromRight(gXSpacing - gComponentSingleSliderXOffset);
    tempoSlider->setBounds(tempoSliderSlice);
    
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
    
    DBG("updating component visibility");
    if(modeCB.getText() == "Adaptive Tempo 1")
    {
        AT1HistorySlider->setVisible(true);
        AT1SubdivisionsSlider->setVisible(true);;
        AT1MinMaxSlider->setVisible(true);
        
        A1ModeLabel.setVisible(true);
        A1ModeCB.setVisible(true);
        
        A1AdaptedTempo.setVisible(true);
        A1AdaptedPeriodMultiplier.setVisible(true);
        
        A1reset.setVisible(true);
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
        
        A1reset.setVisible(false);
    }
    
}


// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ TempoPreparationEditor ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~//

TempoPreparationEditor::TempoPreparationEditor(BKAudioProcessor& p, BKItemGraph* theGraph):
TempoViewController(p, theGraph)
{
    selectCB.addMyListener(this);
    selectCB.addListener(this);
    fillSelectCB(-1,-1);
    
    modeCB.addListener(this);
    
    tempoSlider->addMyListener(this);
    A1ModeCB.addListener(this);
    A1reset.addListener(this);
    AT1HistorySlider->addMyListener(this);
    AT1SubdivisionsSlider->addMyListener(this);
    AT1MinMaxSlider->addMyListener(this);
    hideOrShow.addListener(this);
    
    startTimer(50);
    
    update();
}

void TempoPreparationEditor::timerCallback()
{
    TempoProcessor::Ptr tProcessor = processor.gallery->getTempoProcessor(processor.updateState->currentTempoId);

    if(tProcessor->getPeriodMultiplier() != lastPeriodMultiplier)
    {
        lastPeriodMultiplier = tProcessor->getPeriodMultiplier();
        
        A1AdaptedTempo.setText("Adapted Tempo = " + String(tProcessor->getAdaptedTempo()), dontSendNotification);
        A1AdaptedPeriodMultiplier.setText("Adapted Period Multiplier = " + String(tProcessor->getPeriodMultiplier()), dontSendNotification);
    }
    
}


void TempoPreparationEditor::fillSelectCB(int last, int current)
{
    selectCB.clear(dontSendNotification);
    
    for (auto prep : processor.gallery->getAllTempo())
    {
        int Id = prep->getId();;
        String name = prep->getName();
        
        if (name != String::empty)  selectCB.addItem(name, Id);
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
    
    selectCB.addSeparator();
    selectCB.addItem("New Tempo...", -1);
    
    lastId = selectedId;
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
        if (Id == -1)
        {
            processor.gallery->add(PreparationTypeTempo);
            
            Id = processor.gallery->getAllTempo().getLast()->getId();
        }
        
        processor.updateState->currentTempoId = Id;
        
        processor.updateState->idDidChange = true;
        
        update();
        
        fillSelectCB(lastId, Id);
        
        lastId = Id;
    }
    else if (name == modeCB.getName())
    {
        prep->setTempoSystem(TempoType(index));
        active->setTempoSystem(TempoType(index));
        updateComponentVisibility();
    }
    else if (name == A1ModeCB.getName())
    {
        prep->setAdaptiveTempo1Mode((AdaptiveTempo1Mode) index);
        active->setAdaptiveTempo1Mode((AdaptiveTempo1Mode) index);
    }
    
    if (name != selectCB.getName())
    {
        Tempo::Ptr tempo = processor.gallery->getTempo(processor.updateState->currentTempoId);
        tempo->editted = true;
    }
}


void TempoPreparationEditor::BKEditableComboBoxChanged(String name, BKEditableComboBox* cb)
{
    Tempo::Ptr tempo = processor.gallery->getTempo(processor.updateState->currentTempoId);
    tempo->editted = true;
        DBG("************** tempo->edited = true");
    
    tempo->setName(name);
}

void TempoPreparationEditor::BKRangeSliderValueChanged(String name, double minval, double maxval)
{
    Tempo::Ptr tempo = processor.gallery->getTempo(processor.updateState->currentTempoId);
    tempo->editted = true;
    
    TempoPreparation::Ptr prep = processor.gallery->getStaticTempoPreparation(processor.updateState->currentTempoId);
    TempoPreparation::Ptr active = processor.gallery->getActiveTempoPreparation(processor.updateState->currentTempoId);
    
    if(name == AT1MinMaxSlider->getName()) {
        DBG("got new AdaptiveTempo 1 time diff min/max " + String(minval) + " " + String(maxval));
        prep->setAdaptiveTempo1Min(minval);
        prep->setAdaptiveTempo1Max(maxval);
        active->setAdaptiveTempo1Min(minval);
        active->setAdaptiveTempo1Max(maxval);
    }
}

void TempoPreparationEditor::update(void)
{
    if (processor.updateState->currentTempoId < 0) return;
    
    TempoPreparation::Ptr prep = processor.gallery->getStaticTempoPreparation(processor.updateState->currentTempoId);
    
    if (prep != nullptr)
    {
        selectCB.setSelectedId(processor.updateState->currentTempoId, dontSendNotification);
        modeCB.setSelectedItemIndex((int)prep->getTempoSystem(), dontSendNotification);
        tempoSlider->setValue(prep->getTempo(), dontSendNotification);
        DBG("tempoSlider set to " + String(prep->getTempo()));
        
        A1ModeCB.setSelectedItemIndex(prep->getAdaptiveTempo1Mode(), dontSendNotification);
        AT1HistorySlider->setValue(prep->getAdaptiveTempo1History(), dontSendNotification);
        AT1SubdivisionsSlider->setValue(prep->getAdaptiveTempo1Subdivisions(), dontSendNotification);
        AT1MinMaxSlider->setMinValue(prep->getAdaptiveTempo1Min(), dontSendNotification);
        AT1MinMaxSlider->setMaxValue(prep->getAdaptiveTempo1Max(), dontSendNotification);
        
        updateComponentVisibility();
    }
}


void TempoPreparationEditor::BKSingleSliderValueChanged(String name, double val)
{
    Tempo::Ptr tempo = processor.gallery->getTempo(processor.updateState->currentTempoId);
    tempo->editted = true;
    
    TempoPreparation::Ptr prep = processor.gallery->getStaticTempoPreparation(processor.updateState->currentTempoId);
    TempoPreparation::Ptr active = processor.gallery->getActiveTempoPreparation(processor.updateState->currentTempoId);;
    
    if(name == tempoSlider->getName()) {
        DBG("got tempo " + String(val));
        prep->setTempo(val);
        active->setTempo(val);
    }
    else if(name == AT1HistorySlider->getName()) {
        DBG("got A1History " + String(val));
        prep->setAdaptiveTempo1History(val);
        active->setAdaptiveTempo1History(val);
    }
    else if(name == AT1SubdivisionsSlider->getName()) {
        DBG("got A1Subdivisions " + String(val));
        prep->setAdaptiveTempo1Subdivisions(val);
        active->setAdaptiveTempo1Subdivisions(val);
    }
    
}

void TempoPreparationEditor::buttonClicked (Button* b)
{
    Tempo::Ptr tempo = processor.gallery->getTempo(processor.updateState->currentTempoId);
    tempo->editted = true;
    
    if (b == &A1reset)
    {
        DBG("resetting A1 tempo multiplier");
        
        TempoProcessor::Ptr tProcessor = processor.gallery->getTempoProcessor(processor.updateState->currentTempoId);
        tProcessor->reset();
    }
    else if (b == &hideOrShow)
    {
        processor.updateState->setCurrentDisplay(DisplayNil);
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
    AT1HistorySlider->addMyListener(this);
    AT1SubdivisionsSlider->addMyListener(this);
    AT1MinMaxSlider->addMyListener(this);
    A1ModeCB.addListener(this);
    hideOrShow.addListener(this);
    
    clearModsButton.setButtonText("clear mods");
    addAndMakeVisible(clearModsButton);
    clearModsButton.addListener(this);
    
    update();
}

void TempoModificationEditor::greyOutAllComponents()
{
    A1reset.setVisible(false);
    
    A1ModeLabel.setAlpha(gModAlpha);
    modeCB.setAlpha(gModAlpha);
    A1ModeCB.setAlpha(gModAlpha);
    tempoSlider->setDim(gModAlpha);
    AT1HistorySlider->setDim(gModAlpha);
    AT1SubdivisionsSlider->setDim(gModAlpha);
    AT1MinMaxSlider->setDim(gModAlpha);
}

void TempoModificationEditor::highlightModedComponents()
{
    TempoModPreparation::Ptr mod = processor.gallery->getTempoModPreparation(processor.updateState->currentModTempoId);
    
    if(mod->getParam(TempoBPM) != "")           tempoSlider->setBright();
    if(mod->getParam(TempoSystem) != "")        modeCB.setAlpha(1);
    if(mod->getParam(AT1History) != "")         AT1HistorySlider->setBright();
    if(mod->getParam(AT1Subdivisions) != "")    AT1SubdivisionsSlider->setBright();
    if(mod->getParam(AT1Min) != "")             AT1MinMaxSlider->setBright();
    if(mod->getParam(AT1Max) != "")             AT1MinMaxSlider->setBright();
    if(mod->getParam(AT1Mode) != "")            { A1ModeCB.setAlpha(1.);  A1ModeLabel.setAlpha(1.); }

}

void TempoModificationEditor::fillSelectCB(int last, int current)
{
    selectCB.clear(dontSendNotification);
    
    for (auto prep : processor.gallery->getTempoModPreparations())
    {
        int Id = prep->getId();;
        String name = prep->getName();
        
        if (name != String::empty)  selectCB.addItem(name, Id);
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
    
    selectCB.addSeparator();
    selectCB.addItem("New Tempo Mod...", -1);
    
    lastId = selectedId;
}


void TempoModificationEditor::update(void)
{
    selectCB.setSelectedId(processor.updateState->currentModTempoId, dontSendNotification);
    
    TempoModPreparation::Ptr mod = processor.gallery->getTempoModPreparation(processor.updateState->currentModTempoId);
    
    if (mod != nullptr)
    {
        
        greyOutAllComponents();
        highlightModedComponents();
        
        String val = mod->getParam(TempoSystem);
        modeCB.setSelectedItemIndex(val.getIntValue(), dontSendNotification);
        //                       modeCB.setSelectedItemIndex((int)prep->getTempoSystem(), dontSendNotification);
        
        val = mod->getParam(TempoBPM);
        tempoSlider->setValue(val.getFloatValue(), dontSendNotification);
        //                       tempoSlider->setValue(prep->getTempo(), dontSendNotification);
        
        val = mod->getParam(AT1Mode);
        A1ModeCB.setSelectedItemIndex(val.getIntValue(), dontSendNotification);
        //                       A1ModeCB.setSelectedItemIndex(prep->getAdaptiveTempo1Mode(), dontSendNotification);
        
        val = mod->getParam(AT1History);
        AT1HistorySlider->setValue(val.getIntValue(), dontSendNotification);
        //                       AT1HistorySlider->setValue(prep->getAdaptiveTempo1History(), dontSendNotification);
        
        val = mod->getParam(AT1Subdivisions);
        AT1SubdivisionsSlider->setValue(val.getFloatValue(), dontSendNotification);
        //                       AT1SubdivisionsSlider->setValue(prep->getAdaptiveTempo1Subdivisions(), dontSendNotification);
        
        val = mod->getParam(AT1Min);
        AT1MinMaxSlider->setMinValue(val.getDoubleValue(), dontSendNotification);
        //                       AT1MinMaxSlider->setMinValue(prep->getAdaptiveTempo1Min(), dontSendNotification);
        
        val = mod->getParam(AT1Max);
        AT1MinMaxSlider->setMaxValue(val.getDoubleValue(), dontSendNotification);
        //                       AT1MinMaxSlider->setMaxValue(prep->getAdaptiveTempo1Max(), dontSendNotification);
        
        updateComponentVisibility();
    }
    
}


void TempoModificationEditor::bkComboBoxDidChange (ComboBox* box)
{
    String name = box->getName();
    int index = box->getSelectedItemIndex();
    int Id = box->getSelectedId();
    
    TempoModPreparation::Ptr mod = processor.gallery->getTempoModPreparation(processor.updateState->currentModTempoId);
    
    if (name == selectCB.getName())
    {
        if (Id == -1)
        {
            processor.gallery->add(PreparationTypeTempoMod);
            
            Id = processor.gallery->getTempoModPreparations().getLast()->getId();
        }
        
        processor.updateState->currentModTempoId = Id;
        
        processor.updateState->idDidChange = true;
        
        update();
        
        fillSelectCB(lastId, Id);
        
        lastId = Id;
    }
    else if (name == modeCB.getName())
    {
        mod->setParam(TempoSystem, String(index));
        modeCB.setAlpha(1.);
        updateComponentVisibility();
    }
    else if (name == A1ModeCB.getName())
    {
        mod->setParam(AT1Mode, String(index));
        A1ModeCB.setAlpha(1.);
        A1ModeLabel.setAlpha(1.);
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
    TempoModPreparation::Ptr mod = processor.gallery->getTempoModPreparation(processor.updateState->currentModTempoId);

    if(name == AT1MinMaxSlider->getName())
    {
        mod->setParam(AT1Min, String(minval));
        mod->setParam(AT1Max, String(maxval));
        AT1MinMaxSlider->setBright();
    }
    
    updateModification();
}



void TempoModificationEditor::BKSingleSliderValueChanged(String name, double val)
{
    TempoModPreparation::Ptr mod = processor.gallery->getTempoModPreparation(processor.updateState->currentModTempoId);
    
    if(name == tempoSlider->getName())
    {
        mod->setParam(TempoBPM, String(val));
        tempoSlider->setBright();
    }
    else if(name == AT1HistorySlider->getName())
    {
        mod->setParam(AT1History, String(val));
        AT1HistorySlider->setBright();
    }
    else if(name == AT1SubdivisionsSlider->getName())
    {
        mod->setParam(AT1Subdivisions, String(val));
        AT1SubdivisionsSlider->setBright();
    }
    
    updateModification();
    
}

void TempoModificationEditor::updateModification(void)
{
    TempoModPreparation::Ptr mod = processor.gallery->getTempoModPreparation(processor.updateState->currentModTempoId);
    mod->editted = true;
    
    processor.updateState->modificationDidChange = true;
}

void TempoModificationEditor::buttonClicked (Button* b)
{
    if (b == &A1reset)
    {
        
    }
    else if (b == &hideOrShow)
    {
        TempoModPreparation::Ptr mod = processor.gallery->getTempoModPreparation(processor.updateState->currentModTempoId);
        mod->editted = true;
        
        processor.updateState->setCurrentDisplay(DisplayNil);
        
    }
    else if (b == &clearModsButton)
    {
        TempoModPreparation::Ptr mod = processor.gallery->getTempoModPreparation(processor.updateState->currentModTempoId);
        mod->clearAll();
        update();
    }
    
}

