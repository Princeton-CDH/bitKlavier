/*
  ==============================================================================

    SynchronicViewController.cpp
    Created: 21 Apr 2017 11:17:47pm
    Author:  Daniel Trueman

  ==============================================================================
*/

#include "SynchronicViewController.h"
#include "BKUtilities.h"
#include "Keymap.h"

#include "Synchronic.h"

SynchronicViewController::SynchronicViewController(BKAudioProcessor& p, BKItemGraph* theGraph):
BKViewController(p, theGraph)
{
    SynchronicPreparation::Ptr prep = processor.gallery->getStaticSynchronicPreparation(processor.updateState->currentSynchronicId);
    
    setLookAndFeel(&buttonsAndMenusLAF);
    
    iconImageComponent.setImage(ImageCache::getFromMemory(BinaryData::synchronic_icon_png, BinaryData::synchronic_icon_pngSize));
    iconImageComponent.setImagePlacement(RectanglePlacement(juce::RectanglePlacement::stretchToFit));
    iconImageComponent.setAlpha(0.095);
    addAndMakeVisible(iconImageComponent);
    
    // MultSliders
    paramSliders = OwnedArray<BKMultiSlider>();
    
    int idx = 0;
    for (int i = 0; i < cSynchronicParameterTypes.size(); i++)
    {
        if (cSynchronicDataTypes[i] == BKFloatArr || cSynchronicDataTypes[i] == BKArrFloatArr)
        {
            paramSliders.insert(idx, new BKMultiSlider(HorizontalMultiBarSlider));
            addAndMakeVisible(paramSliders[idx]);
            paramSliders[idx]->setName(cSynchronicParameterTypes[idx+SynchronicTranspOffsets]);
            paramSliders[idx]->setMinMaxDefaultInc(cSynchronicDefaultRangeValuesAndInc[i]);
            
            if(paramSliders[idx]->getName() == "transpositions")
            {
                paramSliders[idx]->setAllowSubSlider(true);
                paramSliders[idx]->setSubSliderName("add transposition");
            }
            
            /* // need this?
            if(paramSliders[0]->getName() == "sustain length multipliers")
            {
                paramSliders[0]->setSkewFromMidpoint(false);
            }
            */
            idx++;
        }
        
    }
    
    selectCB.setName("Synchronic");
    selectCB.addSeparator();
    selectCB.addListener(this);
    selectCB.setSelectedId(1, dontSendNotification);
    addAndMakeVisible(selectCB);
    
    modeSelectCB.setName("Mode");
    modeSelectCB.addSeparator();
    modeSelectCB.BKSetJustificationType(juce::Justification::centredRight);
    modeSelectCB.setSelectedItemIndex(0);
    fillModeSelectCB();
    addAndMakeVisible(modeSelectCB);
    
    //offsetParamStartToggle = new BKSingleSlider("skip first", 0, 1, 0, 1);
    offsetParamStartToggle.setButtonText ("skip first");
    buttonsAndMenusLAF.setToggleBoxTextToRightBool(false);
    offsetParamStartToggle.setToggleState (true, dontSendNotification);
    addAndMakeVisible(offsetParamStartToggle);
    
    howManySlider = new BKSingleSlider("how many", 1, 100, 20, 1);
    howManySlider->setJustifyRight(false);
    addAndMakeVisible(howManySlider);
    
    clusterThreshSlider = new BKSingleSlider("cluster threshold", 20, 2000, 200, 10);
    clusterThreshSlider->setJustifyRight(false);
    addAndMakeVisible(clusterThreshSlider);
    
    clusterMinMaxSlider = new BKRangeSlider("cluster min/max", 1, 12, 3, 4, 1);
    clusterMinMaxSlider->setJustifyRight(false);
    addAndMakeVisible(clusterMinMaxSlider);
    
    gainSlider = new BKSingleSlider("gain", 0, 10, 1, 0.01);
    gainSlider->setJustifyRight(false);
    gainSlider->setSkewFactorFromMidPoint(1.);
    addAndMakeVisible(gainSlider);
    
    releaseVelocitySetsSynchronicToggle.addListener(this);
    releaseVelocitySetsSynchronicToggle.setLookAndFeel(&buttonsAndMenusLAF2); //need different one so toggle text can be on other side
    releaseVelocitySetsSynchronicToggle.setButtonText ("noteOff loudness");
    buttonsAndMenusLAF2.setToggleBoxTextToRightBool(false);
    releaseVelocitySetsSynchronicToggle.setToggleState (false, dontSendNotification);
    //addAndMakeVisible(releaseVelocitySetsSynchronicToggle); //possibly for future version, but it seems even keyboards that do noteOff velocity suck at it.
    
    addAndMakeVisible(actionButton);
    actionButton.setButtonText("Action");
    actionButton.addListener(this);
}

void SynchronicViewController::paint (Graphics& g)
{
    g.fillAll(Colours::black);
}

void SynchronicViewController::resized()
{
    Rectangle<int> area (getLocalBounds());

    iconImageComponent.setBounds(area);
    area.reduce(10 * processor.paddingScalarX + 4, 10 * processor.paddingScalarY + 4);
    
    Rectangle<int> leftColumn = area.removeFromLeft(area.getWidth() * 0.5);
    Rectangle<int> comboBoxSlice = leftColumn.removeFromTop(gComponentComboBoxHeight);
    comboBoxSlice.removeFromRight(4 + 2.*gPaddingConst * processor.paddingScalarX);
    comboBoxSlice.removeFromLeft(gXSpacing);
    hideOrShow.setBounds(comboBoxSlice.removeFromLeft(gComponentComboBoxHeight));
    comboBoxSlice.removeFromLeft(gXSpacing);
    selectCB.setBounds(comboBoxSlice.removeFromLeft(comboBoxSlice.getWidth() / 2.));
    
    actionButton.setBounds(selectCB.getRight()+gXSpacing,
                           selectCB.getY(),
                           selectCB.getWidth() * 0.5,
                           selectCB.getHeight());
    
    comboBoxSlice.removeFromLeft(gXSpacing);
    
    /* *** above here should be generic to all prep layouts *** */
    /* ***    below here will be specific to each prep      *** */
    
    Rectangle<int> modeSlice = area.removeFromTop(gComponentComboBoxHeight);
    modeSlice.removeFromRight(gXSpacing);
    modeSelectCB.setBounds(modeSlice.removeFromRight(modeSlice.getWidth() / 2.));
    offsetParamStartToggle.setBounds(modeSlice);
    
    int tempHeight = (area.getHeight() - paramSliders.size() * (gYSpacing + gPaddingConst * processor.paddingScalarY)) / paramSliders.size();
    area.removeFromLeft(4 + 2.*gPaddingConst * processor.paddingScalarX);
    area.removeFromRight(gXSpacing);
    for(int i = 0; i < paramSliders.size(); i++)
    {
        area.removeFromTop(gYSpacing + gPaddingConst * processor.paddingScalarY);
        paramSliders[i]->setBounds(area.removeFromTop(tempHeight));
    }
    
    //leftColumn.reduce(4 + 2.*gPaddingConst * paddingScalarX, 0);
    leftColumn.removeFromRight(gXSpacing + 2.*gPaddingConst * processor.paddingScalarX - gComponentSingleSliderXOffset);
    //leftColumn.removeFromLeft(gXSpacing);
    
    int nextCenter = paramSliders[0]->getY() + paramSliders[0]->getHeight() / 2 + gPaddingConst * (1. - processor.paddingScalarY) ;
    howManySlider->setBounds(leftColumn.getX(),
                             nextCenter - gComponentSingleSliderHeight/2.,
                             leftColumn.getWidth(),
                             gComponentSingleSliderHeight);
    
    nextCenter = paramSliders[1]->getY() + paramSliders[1]->getHeight() / 2 + gPaddingConst * (1. - processor.paddingScalarY);
    clusterThreshSlider->setBounds(leftColumn.getX(),
                                   nextCenter - gComponentSingleSliderHeight/2.,
                                   leftColumn.getWidth(),
                                   gComponentSingleSliderHeight);
    
    nextCenter = paramSliders[2]->getY() + paramSliders[2]->getHeight() / 2 + gPaddingConst * (1. - processor.paddingScalarY);
    clusterMinMaxSlider->setBounds(leftColumn.getX(),
                                   nextCenter - gComponentRangeSliderHeight/2.,
                                   leftColumn.getWidth(),
                                   gComponentRangeSliderHeight);
    
    nextCenter = paramSliders[3]->getY() + paramSliders[3]->getHeight() / 2 + gPaddingConst * (1. - processor.paddingScalarY);
    gainSlider->setBounds(leftColumn.getX(),
                          nextCenter - gComponentSingleSliderHeight/2.,
                          leftColumn.getWidth(),
                          gComponentSingleSliderHeight);
    
    Rectangle<int> releaseToggleSlice = gainSlider->getBounds().removeFromTop(gComponentTextFieldHeight);
    releaseToggleSlice.removeFromRight(gYSpacing);
    releaseVelocitySetsSynchronicToggle.setBounds(releaseToggleSlice.removeFromRight(releaseToggleSlice.getWidth() * 0.5));
    
    repaint();
    
}

//SynchronicSyncMode
void SynchronicViewController::fillModeSelectCB(void)
{
    
    modeSelectCB.clear(dontSendNotification);
    for (int i = 0; i < cSynchronicSyncModes.size(); i++)
    {
        String name = cSynchronicSyncModes[i];
        if (name != String::empty)  modeSelectCB.addItem(name, i+1);
        else                        modeSelectCB.addItem(String(i+1), i+1);
    }
    
    modeSelectCB.setSelectedItemIndex(0, NotificationType::dontSendNotification);
}


// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ SynchronicPreparationEditor ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ //

SynchronicPreparationEditor::SynchronicPreparationEditor(BKAudioProcessor& p, BKItemGraph* theGraph):
SynchronicViewController(p, theGraph)
{
    fillSelectCB(-1,-1);
    
    SynchronicPreparation::Ptr prep = processor.gallery->getStaticSynchronicPreparation(processor.updateState->currentSynchronicId);
    
    selectCB.addListener(this);
    selectCB.addMyListener(this);
    modeSelectCB.addListener(this);
    offsetParamStartToggle.addListener(this);
    howManySlider->addMyListener(this);
    clusterThreshSlider->addMyListener(this);
    clusterMinMaxSlider->addMyListener(this);

    gainSlider->addMyListener(this);
    
    startTimer(30);
    
}

void SynchronicPreparationEditor::timerCallback()
{
    if (processor.updateState->currentDisplay == DisplaySynchronic)
    {
        SynchronicProcessor::Ptr sProcessor = processor.currentPiano->getSynchronicProcessor(processor.updateState->currentSynchronicId);

        int counter = 0, size = 0;
        
        if (sProcessor != nullptr)
        {
            for (int i = 0; i < paramSliders.size(); i++)
            {
                if(paramSliders[i]->getName() == "beat length multipliers")
                {
                    size = paramSliders[i]->getNumVisible();
                    counter = sProcessor->getBeatMultiplierCounter();
                    paramSliders[i]->setCurrentSlider((counter >= size || counter < 0) ? 0 : counter);
                }
                else if(paramSliders[i]->getName() == "sustain length multipliers")
                {
                    size = paramSliders[i]->getNumVisible();
                    counter = sProcessor->getLengthMultiplierCounter();
                    paramSliders[i]->setCurrentSlider((counter >= size || counter < 0) ? 0 : counter);
                }
                else if(paramSliders[i]->getName() == "accents")
                {
                    size = paramSliders[i]->getNumVisible();
                    counter = sProcessor->getAccentMultiplierCounter();
                    paramSliders[i]->setCurrentSlider((counter >= size || counter < 0) ? 0 : counter);
                }
                else if(paramSliders[i]->getName() == "transpositions")
                {
                    size = paramSliders[i]->getNumVisible();
                    counter = sProcessor->getTranspCounter();
                    paramSliders[i]->setCurrentSlider((counter >= size || counter < 0) ? 0 : counter);
                }
            }
        }
        
    }
}


void SynchronicPreparationEditor::multiSliderDidChange(String name, int whichSlider, Array<float> values)
{
    SynchronicPreparation::Ptr prep = processor.gallery->getStaticSynchronicPreparation(processor.updateState->currentSynchronicId);
    SynchronicPreparation::Ptr active = processor.gallery->getActiveSynchronicPreparation(processor.updateState->currentSynchronicId);
    
    if (name == "accents")
    {
        prep    ->setAccentMultiplier(whichSlider, values[0]);
        active  ->setAccentMultiplier(whichSlider, values[0]);
    }
    else if (name == "beat length multipliers")
    {
        prep    ->setBeatMultiplier(whichSlider, values[0]);
        active  ->setBeatMultiplier(whichSlider, values[0]);
    }
    else if (name == "sustain length multipliers")
    {
        prep    ->setLengthMultiplier(whichSlider, values[0]);
        active  ->setLengthMultiplier(whichSlider, values[0]);
    }
    else if (name == "transpositions")
    {
        prep    ->setSingleTransposition(whichSlider, values);
        active  ->setSingleTransposition(whichSlider, values);
    }
    
    //processor.updateState->synchronicPreparationDidChange = true;

}

void SynchronicPreparationEditor::multiSlidersDidChange(String name, Array<Array<float>> values)
{
    SynchronicPreparation::Ptr prep = processor.gallery->getStaticSynchronicPreparation(processor.updateState->currentSynchronicId);
    SynchronicPreparation::Ptr active = processor.gallery->getActiveSynchronicPreparation(processor.updateState->currentSynchronicId);
    
    //only transposition allows multiple simultaneous vals, so trim down to 1D array
    Array<float> newvals = Array<float>();
    for(int i=0; i<values.size(); i++) newvals.add(values[i][0]);
    
    if (name == cSynchronicParameterTypes[SynchronicAccentMultipliers])
    {
        prep    ->setAccentMultipliers(newvals);
        active  ->setAccentMultipliers(newvals);
    }
    else if (name == cSynchronicParameterTypes[SynchronicBeatMultipliers])
    {
        prep    ->setBeatMultipliers(newvals);
        active  ->setBeatMultipliers(newvals);
    }
    else if (name == cSynchronicParameterTypes[SynchronicLengthMultipliers])
    {
        prep    ->setLengthMultipliers(newvals);
        active  ->setLengthMultipliers(newvals);
    }
    //pass original 2D array for transpositions
    else if (name == cSynchronicParameterTypes[SynchronicTranspOffsets])
    {
        prep    ->setTransposition(values);
        active  ->setTransposition(values);
    }
    
    //processor.updateState->synchronicPreparationDidChange = true;
}

void SynchronicPreparationEditor::BKSingleSliderValueChanged(String name, double val)
{
    SynchronicPreparation::Ptr prep = processor.gallery->getStaticSynchronicPreparation(processor.updateState->currentSynchronicId);
    SynchronicPreparation::Ptr active = processor.gallery->getActiveSynchronicPreparation(processor.updateState->currentSynchronicId);
    
    if(name == "how many") {
        DBG("got new how many " + String(val));
        prep->setNumBeats(val);
        active->setNumBeats(val);
    }
    else if(name == "cluster threshold")
    {
        DBG("got new cluster threshold " + String(val));
        prep->setClusterThresh(val);
        active->setClusterThresh(val);
    }
    else if(name == "gain")
    {
        DBG("gain " + String(val));
        prep->setGain(val);
        active->setGain(val);
    }
}

void SynchronicPreparationEditor::BKRangeSliderValueChanged(String name, double minval, double maxval)
{
    SynchronicPreparation::Ptr prep = processor.gallery->getStaticSynchronicPreparation(processor.updateState->currentSynchronicId);
    SynchronicPreparation::Ptr active = processor.gallery->getActiveSynchronicPreparation(processor.updateState->currentSynchronicId);
    
    if(name == "cluster min/max") {
        DBG("got new cluster min/max " + String(minval) + " " + String(maxval));
        prep->setClusterMin(minval);
        prep->setClusterMax(maxval);
        active->setClusterMin(minval);
        active->setClusterMax(maxval);
    }
}

void SynchronicPreparationEditor::update(NotificationType notify)
{
    if (processor.updateState->currentSynchronicId < 0) return;
    
    SynchronicPreparation::Ptr prep   = processor.gallery->getActiveSynchronicPreparation(processor.updateState->currentSynchronicId);
    
    if (prep != nullptr)
    {
        
        selectCB.setSelectedId(processor.updateState->currentSynchronicId, notify);
        modeSelectCB.setSelectedItemIndex(prep->getMode(), notify);
        offsetParamStartToggle.setToggleState(prep->getOffsetParamToggle(), notify);
        releaseVelocitySetsSynchronicToggle.setToggleState(prep->getReleaseVelocitySetsSynchronic(), notify);
        howManySlider->setValue(prep->getNumBeats(), notify);
        clusterThreshSlider->setValue(prep->getClusterThreshMS(), notify);
        clusterMinMaxSlider->setMinValue(prep->getClusterMin(), notify);
        clusterMinMaxSlider->setMaxValue(prep->getClusterMax(), notify);
        gainSlider->setValue(prep->getGain(), notify);
        
        for(int i = 0; i < paramSliders.size(); i++)
        {
            if(paramSliders[i]->getName() == cSynchronicParameterTypes[SynchronicAccentMultipliers])
            {
                paramSliders[i]->setTo(prep->getAccentMultipliers(), notify);
            }
            
            if(paramSliders[i]->getName() == cSynchronicParameterTypes[SynchronicBeatMultipliers])
            {
                paramSliders[i]->setTo(prep->getBeatMultipliers(), notify);
            }
            
            if(paramSliders[i]->getName() == cSynchronicParameterTypes[SynchronicLengthMultipliers])
            {
                paramSliders[i]->setTo(prep->getLengthMultipliers(), notify);
            }
            
            if(paramSliders[i]->getName() == cSynchronicParameterTypes[SynchronicTranspOffsets])
            {
                paramSliders[i]->setTo(prep->getTransposition(), notify);
            }
        }
        
    }
    
}

void SynchronicPreparationEditor::update()
{
    update(dontSendNotification);
}

void SynchronicPreparationEditor::fillSelectCB(int last, int current)
{
    selectCB.clear(dontSendNotification);
    
    for (auto prep : processor.gallery->getAllSynchronic())
    {
        int Id = prep->getId();
        
        if (Id == -1) continue;
        
        String name = prep->getName();
        
        if (name != String::empty)  selectCB.addItem(name, Id);
        else                        selectCB.addItem("Synchronic"+String(Id), Id);
        
        selectCB.setItemEnabled(Id, true);
        if (processor.currentPiano->isActive(PreparationTypeSynchronic, Id))
            selectCB.setItemEnabled(Id, false);
    }
    
    if (last != 0)      selectCB.setItemEnabled(last, true);
    if (current != 0)   selectCB.setItemEnabled(current, false);
    
    int selectedId = processor.updateState->currentSynchronicId;
    
    selectCB.setSelectedId(selectedId, NotificationType::dontSendNotification);
    
    selectCB.setItemEnabled(selectedId, false);

    lastId = selectedId;
}


int SynchronicPreparationEditor::addPreparation(void)
{
    processor.gallery->add(PreparationTypeSynchronic);
    
    return processor.gallery->getAllSynchronic().getLast()->getId();
}

int SynchronicPreparationEditor::duplicatePreparation(void)
{
    processor.gallery->duplicate(PreparationTypeSynchronic, processor.updateState->currentSynchronicId);
    
    return processor.gallery->getAllSynchronic().getLast()->getId();
}

void SynchronicPreparationEditor::deleteCurrent(void)
{
    int SynchronicId = selectCB.getSelectedId();
    int index = selectCB.getSelectedItemIndex();
    
    if ((index == 0) && (selectCB.getItemId(index+1) == -1)) return;
    
    processor.gallery->remove(PreparationTypeSynchronic, SynchronicId);
    
    fillSelectCB(0, 0);
    
    int newId = 0;
    
    selectCB.setSelectedId(newId, dontSendNotification);
    
    processor.updateState->currentSynchronicId = -1;
}

void SynchronicPreparationEditor::setCurrentId(int Id)
{
    processor.updateState->currentSynchronicId = Id;
    
    processor.updateState->idDidChange = true;
    
    update();
    
    fillSelectCB(lastId, Id);
    
    lastId = Id;
}

void SynchronicPreparationEditor::actionButtonCallback(int action, SynchronicPreparationEditor* vc)
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
        processor.reset(PreparationTypeSynchronic, processor.updateState->currentSynchronicId);
        vc->update();
    }
    else if (action == 5)
    {
        processor.clear(PreparationTypeSynchronic, processor.updateState->currentSynchronicId);
        vc->update();
    }
}


void SynchronicPreparationEditor::bkComboBoxDidChange (ComboBox* box)
{
    String name = box->getName();
    int index = box->getSelectedItemIndex();
    int Id = box->getSelectedId();
    
    if (name == selectCB.getName())
    {
        setCurrentId(Id);
    }
    else if (name == "Mode")
    {
        SynchronicPreparation::Ptr prep = processor.gallery->getStaticSynchronicPreparation(processor.updateState->currentSynchronicId);
        SynchronicPreparation::Ptr active = processor.gallery->getActiveSynchronicPreparation(processor.updateState->currentSynchronicId);
        
        prep    ->setMode((SynchronicSyncMode) index);
        active  ->setMode((SynchronicSyncMode) index);
        
        int toggleVal;
        if(offsetParamStartToggle.getToggleState()) toggleVal = 1;
        else toggleVal = 0;
        
        if(prep->getMode() == FirstNoteOnSync || prep->getMode() == AnyNoteOnSync)
        {
            prep->setBeatsToSkip(toggleVal - 1);
            active->setBeatsToSkip(toggleVal - 1);
        }
        else
        {
            prep->setBeatsToSkip(toggleVal);
            active->setBeatsToSkip(toggleVal);
        }
    }
}

void SynchronicPreparationEditor::bkTextFieldDidChange(TextEditor& tf)
{
    String text = tf.getText();
    String name = tf.getName();
    
    //SynchronicPreparation::Ptr prep = processor.gallery->getStaticSynchronicPreparation(processor.updateState->currentSynchronicId);
    //SynchronicPreparation::Ptr active = processor.gallery->getActiveSynchronicPreparation(processor.updateState->currentSynchronicId);
    
    if (name == "Name")
    {
        Synchronic::Ptr synchronic = processor.gallery->getSynchronic(processor.updateState->currentSynchronicId);
        
        synchronic->setName(text);
        
        int selected = selectCB.getSelectedId();
        if (selected != selectCB.getNumItems()) selectCB.changeItemText(selected, text);
        selectCB.setSelectedId(selected, dontSendNotification );
    }
}

void SynchronicPreparationEditor::BKEditableComboBoxChanged(String name, BKEditableComboBox* cb)
{
    Synchronic::Ptr synchronic = processor.gallery->getSynchronic(processor.updateState->currentSynchronicId);
    
    synchronic->setName(name);
}

void SynchronicPreparationEditor::bkMessageReceived (const String& message)
{
    if (message == "synchronic/update")
    {
        update();
    }
}

void SynchronicPreparationEditor::buttonClicked (Button* b)
{
    if (b == &offsetParamStartToggle)
    {
        SynchronicPreparation::Ptr prep = processor.gallery->getStaticSynchronicPreparation(processor.updateState->currentSynchronicId);
        SynchronicPreparation::Ptr active = processor.gallery->getActiveSynchronicPreparation(processor.updateState->currentSynchronicId);
        
        int toggleVal;
        if(offsetParamStartToggle.getToggleState()) toggleVal = 1;
        else toggleVal = 0;
        
        if(prep->getMode() == FirstNoteOnSync || prep->getMode() == AnyNoteOnSync)
        {
            prep->setBeatsToSkip(toggleVal - 1);
            active->setBeatsToSkip(toggleVal - 1);
        }
        else
        {
            prep->setBeatsToSkip(toggleVal);
            active->setBeatsToSkip(toggleVal);
        }

    }
    else if (b == &releaseVelocitySetsSynchronicToggle)
    {
        SynchronicPreparation::Ptr prep = processor.gallery->getStaticSynchronicPreparation(processor.updateState->currentSynchronicId);
        SynchronicPreparation::Ptr active = processor.gallery->getActiveSynchronicPreparation(processor.updateState->currentSynchronicId);
        
        prep->setReleaseVelocitySetsSynchronic(releaseVelocitySetsSynchronicToggle.getToggleState());
        active->setReleaseVelocitySetsSynchronic(releaseVelocitySetsSynchronicToggle.getToggleState());
    }
    else if (b == &hideOrShow)
    {
        processor.updateState->setCurrentDisplay(DisplayNil);
    }
    else if (b == &actionButton)
    {
        getPrepOptionMenu().showMenuAsync (PopupMenu::Options().withTargetComponent (&actionButton), ModalCallbackFunction::forComponent (actionButtonCallback, this) );
    }
}



// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ SynchronicModificationEditor ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ //
SynchronicModificationEditor::SynchronicModificationEditor(BKAudioProcessor& p, BKItemGraph* theGraph):
SynchronicViewController(p, theGraph)
{
    greyOutAllComponents();
    
    fillSelectCB(-1,-1);
    
    selectCB.addListener(this);
    selectCB.addMyListener(this);
    modeSelectCB.addListener(this);
    offsetParamStartToggle.addListener(this);
    howManySlider->addMyListener(this);
    clusterThreshSlider->addMyListener(this);
    clusterMinMaxSlider->addMyListener(this);
    gainSlider->addMyListener(this);
    
    //startTimer(20);
    
}

void SynchronicModificationEditor::greyOutAllComponents()
{
    modeSelectCB.setAlpha(gModAlpha);
    offsetParamStartToggle.setAlpha(gModAlpha);
    
    //howManySlider->setAlpha(gModAlpha);
    howManySlider->setDim(gModAlpha);
    //clusterThreshSlider->setAlpha(gModAlpha);
    clusterThreshSlider->setDim(gModAlpha);
    //clusterMinMaxSlider->setAlpha(gModAlpha);
    clusterThreshSlider->setDim(gModAlpha);
    //gainSlider->setAlpha(gModAlpha);
    gainSlider->setDim(gModAlpha);
    
    //clusterMinMaxSlider->setAlpha(gModAlpha);
    clusterMinMaxSlider->setDim(gModAlpha);
    
    for(int i = 0; i < paramSliders.size(); i++)
    {
        paramSliders[i]->setAlpha(gModAlpha);
    }
}

void SynchronicModificationEditor::highlightModedComponents()
{
    SynchronicModPreparation::Ptr mod = processor.gallery->getSynchronicModPreparation(processor.updateState->currentModSynchronicId);
    
    if(mod->getParam(SynchronicMode) != "")             modeSelectCB.setAlpha(1.);
    if(mod->getParam(SynchronicNumPulses) != "")        howManySlider->setBright(); //howManySlider->setAlpha(1);
    if(mod->getParam(SynchronicClusterThresh) != "")    clusterThreshSlider->setBright(); //clusterThreshSlider->setAlpha(1);
    if(mod->getParam(SynchronicClusterMin) != "")       clusterMinMaxSlider->setBright(); //clusterMinMaxSlider->setAlpha(1);
    if(mod->getParam(SynchronicClusterMax) != "")       clusterMinMaxSlider->setBright(); //clusterMinMaxSlider->setAlpha(1);
    if(mod->getParam(SynchronicBeatsToSkip) != "")      offsetParamStartToggle.setAlpha(1.);
    if(mod->getParam(SynchronicGain) != "")             gainSlider->setBright(); //gainSlider->setAlpha(1);

    if(mod->getParam(SynchronicBeatMultipliers) != "")
    {
        for(int i = 0; i < paramSliders.size(); i++)
        {
            if(paramSliders[i]->getName() == cSynchronicParameterTypes[SynchronicBeatMultipliers])
                paramSliders[i]->setAlpha(1.);
        }
    }
    if(mod->getParam(SynchronicLengthMultipliers) != "")
    {
        for(int i = 0; i < paramSliders.size(); i++)
        {
            if(paramSliders[i]->getName() == cSynchronicParameterTypes[SynchronicLengthMultipliers])
                paramSliders[i]->setAlpha(1.);
        }
    }
    if(mod->getParam(SynchronicAccentMultipliers) != "")
    {
        for(int i = 0; i < paramSliders.size(); i++)
        {
            if(paramSliders[i]->getName() == cSynchronicParameterTypes[SynchronicAccentMultipliers])
                paramSliders[i]->setAlpha(1.);
        }
    }
    if(mod->getParam(SynchronicTranspOffsets) != "")
    {
        for(int i = 0; i < paramSliders.size(); i++)
        {
            if(paramSliders[i]->getName() == cSynchronicParameterTypes[SynchronicTranspOffsets])
                paramSliders[i]->setAlpha(1.);
        }
    }
}

void SynchronicModificationEditor::timerCallback()
{

}

void SynchronicModificationEditor::update(NotificationType notify)
{
    if (processor.updateState->currentModSynchronicId < 0) return;
    
    greyOutAllComponents();
    highlightModedComponents();
    
    selectCB.setSelectedId(processor.updateState->currentModSynchronicId, notify);
    
    SynchronicModPreparation::Ptr mod = processor.gallery->getSynchronicModPreparation(processor.updateState->currentModSynchronicId);
    
    if (mod != nullptr)
    {
        String val = mod->getParam(SynchronicMode);
        modeSelectCB.setSelectedItemIndex(val.getIntValue(), notify);
        
        //FIXIT offsetParamStartToggle.setToggleState(prep->getOffsetParamToggle(), notify);
        //SynchronicBeatsToSkip determines whether to set this toggle
        val = mod->getParam(SynchronicBeatsToSkip);
        offsetParamStartToggle.setToggleState(val.getIntValue() + 1, notify);
        
        val = mod->getParam(SynchronicNumPulses);
        howManySlider->setValue(val.getIntValue(), notify);
        
        val = mod->getParam(SynchronicClusterThresh);
        clusterThreshSlider->setValue(val.getFloatValue(), notify);
        
        val = mod->getParam(SynchronicClusterMin);
        clusterMinMaxSlider->setMinValue(val.getIntValue(), notify);
        
        val = mod->getParam(SynchronicClusterMax);
        clusterMinMaxSlider->setMaxValue(val.getIntValue(), notify);
        
        val = mod->getParam(SynchronicGain);
        gainSlider->setValue(val.getFloatValue(), notify);
        
        for (int i = 0; i < paramSliders.size(); i++)
        {
            if(paramSliders[i]->getName() == cSynchronicParameterTypes[SynchronicAccentMultipliers])
            {
                val = mod->getParam(SynchronicAccentMultipliers);
                paramSliders[i]->setTo(stringToFloatArray(val), notify);
            }
            else if(paramSliders[i]->getName() == cSynchronicParameterTypes[SynchronicBeatMultipliers])
            {
                val = mod->getParam(SynchronicBeatMultipliers);
                paramSliders[i]->setTo(stringToFloatArray(val), notify);
            }
            else if(paramSliders[i]->getName() == cSynchronicParameterTypes[SynchronicLengthMultipliers])
            {
                val = mod->getParam(SynchronicLengthMultipliers);
                paramSliders[i]->setTo(stringToFloatArray(val), notify);
            }
            else if(paramSliders[i]->getName() == cSynchronicParameterTypes[SynchronicTranspOffsets])
            {
                val = mod->getParam(SynchronicTranspOffsets);
                paramSliders[i]->setTo(stringToArrayFloatArray(val), notify);
            }
        }
    }

}

void SynchronicModificationEditor::update()
{
    update(dontSendNotification);
}


void SynchronicModificationEditor::fillSelectCB(int last, int current)
{
    selectCB.clear(dontSendNotification);
    
    for (auto prep : processor.gallery->getSynchronicModPreparations())
    {
        int Id = prep->getId();;
        String name = prep->getName();
        
        if (name != String::empty)  selectCB.addItem(name, Id);
        else                        selectCB.addItem("SynchronicMod"+String(Id), Id);
        
        selectCB.setItemEnabled(Id, true);
        if (processor.currentPiano->isActive(PreparationTypeSynchronic, Id))
            selectCB.setItemEnabled(Id, false);
    }
    
    if (last != 0)      selectCB.setItemEnabled(last, true);
    if (current != 0)   selectCB.setItemEnabled(current, false);
    
    int selectedId = processor.updateState->currentSynchronicId;
    
    selectCB.setSelectedId(selectedId, NotificationType::dontSendNotification);
    
    selectCB.setItemEnabled(selectedId, false);
    
    lastId = selectedId;
}

void SynchronicModificationEditor::multiSliderDidChange(String name, int whichSlider, Array<float> values)
{
    SynchronicModPreparation::Ptr mod = processor.gallery->getSynchronicModPreparation(processor.updateState->currentModSynchronicId);
    
    if (name == cSynchronicParameterTypes[SynchronicAccentMultipliers])
    {
        Array<float> accents = stringToFloatArray(mod->getParam(SynchronicAccentMultipliers));
        accents.set(whichSlider, values[0]);
        mod->setParam(SynchronicAccentMultipliers, floatArrayToString(accents));
    }
    else if (name == cSynchronicParameterTypes[SynchronicBeatMultipliers])
    {
        Array<float> beats = stringToFloatArray(mod->getParam(SynchronicBeatMultipliers));
        beats.set(whichSlider, values[0]);
        mod->setParam(SynchronicBeatMultipliers, floatArrayToString(beats));
    }
    else if (name == cSynchronicParameterTypes[SynchronicLengthMultipliers])
    {
        Array<float> lens = stringToFloatArray(mod->getParam(SynchronicLengthMultipliers));
        lens.set(whichSlider, values[0]);
        mod->setParam(SynchronicLengthMultipliers, floatArrayToString(lens));
    }
    else if (name == cSynchronicParameterTypes[SynchronicTranspOffsets])
    {
        //paramSliders
        Array<Array<float>> transps = stringToArrayFloatArray(mod->getParam(SynchronicTranspOffsets));
        transps.set(whichSlider, values);
        mod->setParam(SynchronicTranspOffsets, arrayFloatArrayToString(transps));
    }
    
    for(int i = 0; i < paramSliders.size(); i++)
    {
        if(paramSliders[i]->getName() == name) paramSliders[i]->setAlpha(1.);
    }
    
    updateModification();
    
}

void SynchronicModificationEditor::multiSlidersDidChange(String name, Array<Array<float>> values)
{
    SynchronicModPreparation::Ptr mod = processor.gallery->getSynchronicModPreparation(processor.updateState->currentModSynchronicId);
    
    //only transposition allows multiple simultaneous vals, so trim down to 1D array
    Array<float> newvals = Array<float>();
    for(int i=0; i<values.size(); i++) newvals.add(values[i][0]);
    
    if (name == cSynchronicParameterTypes[SynchronicAccentMultipliers])
    {
        mod->setParam(SynchronicAccentMultipliers, floatArrayToString(newvals));
    }
    else if (name == cSynchronicParameterTypes[SynchronicBeatMultipliers])
    {
        mod->setParam(SynchronicBeatMultipliers, floatArrayToString(newvals));
    }
    else if (name == cSynchronicParameterTypes[SynchronicLengthMultipliers])
    {
        mod->setParam(SynchronicLengthMultipliers, floatArrayToString(newvals));
    }
    //pass original 2D array for transpositions
    else if (name == cSynchronicParameterTypes[SynchronicTranspOffsets])
    {
        DBG("set mod: " + arrayFloatArrayToString(values));
        mod->setParam(SynchronicTranspOffsets, arrayFloatArrayToString(values));
    }
    
    for(int i = 0; i < paramSliders.size(); i++)
    {
        if(paramSliders[i]->getName() == name) paramSliders[i]->setAlpha(1.);
    }
    
    updateModification();
}

void SynchronicModificationEditor::BKSingleSliderValueChanged(String name, double val)
{
    SynchronicModPreparation::Ptr mod = processor.gallery->getSynchronicModPreparation(processor.updateState->currentModSynchronicId);
    
    GeneralSettings::Ptr genmod = processor.gallery->getGeneralSettings();
    
    if(name == "how many")
    {
        mod->setParam(SynchronicNumPulses, String(val));
        //howManySlider->setAlpha(1.);
        howManySlider->setBright();
    }
    else if(name == "cluster threshold")
    {
        mod->setParam(SynchronicClusterThresh, String(val));
        //clusterThreshSlider->setAlpha(1.);
        clusterThreshSlider->setBright();
    }
    else if(name == "gain")
    {
        mod->setParam(SynchronicGain, String(val));
        //gainSlider->setAlpha(1.);
        gainSlider->setBright();
    }
    
    updateModification();
}

void SynchronicModificationEditor::BKRangeSliderValueChanged(String name, double minval, double maxval)
{
    SynchronicModPreparation::Ptr mod = processor.gallery->getSynchronicModPreparation(processor.updateState->currentModSynchronicId);
    
    if(name == "cluster min/max")
    {
        mod->setParam(SynchronicClusterMin, String(minval));
        mod->setParam(SynchronicClusterMax, String(maxval));
        //clusterMinMaxSlider->setAlpha(1.);
        clusterMinMaxSlider->setBright();
    }
    
    updateModification();
}


int SynchronicModificationEditor::addPreparation(void)
{
    processor.gallery->add(PreparationTypeSynchronicMod);
    
    return processor.gallery->getSynchronicModPreparations().getLast()->getId();
}

int SynchronicModificationEditor::duplicatePreparation(void)
{
    processor.gallery->duplicate(PreparationTypeSynchronicMod, processor.updateState->currentModSynchronicId);
    
    return processor.gallery->getSynchronicModPreparations().getLast()->getId();
}

void SynchronicModificationEditor::deleteCurrent(void)
{
    int oldId = selectCB.getSelectedId();
    int index = selectCB.getSelectedItemIndex();
    
    if ((index == 0) && (selectCB.getItemId(index+1) == -1)) return;
    
    processor.gallery->remove(PreparationTypeSynchronicMod, oldId);
    
    fillSelectCB(0, 0);
    
    int newId = 0;
    
    selectCB.setSelectedId(newId, dontSendNotification);
    
    processor.updateState->currentModSynchronicId = -1;
}

void SynchronicModificationEditor::setCurrentId(int Id)
{
    processor.updateState->currentModSynchronicId = Id;
    
    processor.updateState->idDidChange = true;
    
    update();
    
    fillSelectCB(lastId, Id);
    
    lastId = Id;
}

void SynchronicModificationEditor::actionButtonCallback(int action, SynchronicModificationEditor* vc)
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
        processor.clear(PreparationTypeSynchronicMod, processor.updateState->currentModSynchronicId);
        vc->update();
        vc->updateModification();
    }
}


void SynchronicModificationEditor::bkComboBoxDidChange (ComboBox* box)
{
    String name = box->getName();
    int index = box->getSelectedItemIndex();
    int Id = box->getSelectedId();
    
    if (name == selectCB.getName())
    {
        setCurrentId(Id);
    }
    else if (name == "Mode")
    {
        SynchronicModPreparation::Ptr mod = processor.gallery->getSynchronicModPreparation(processor.updateState->currentModSynchronicId);
        
        mod->setParam(SynchronicMode, String(index));
        
        int toggleVal;
        if(offsetParamStartToggle.getToggleState()) toggleVal = 1;
        else toggleVal = 0;
        
        if(((SynchronicSyncMode)mod->getParam(SynchronicMode).getIntValue()) == FirstNoteOnSync ||
           ((SynchronicSyncMode)mod->getParam(SynchronicMode).getIntValue())  == AnyNoteOnSync)
        {
            mod->setParam(SynchronicBeatsToSkip, String(toggleVal - 1));
        }
        else
        {
            mod->setParam(SynchronicBeatsToSkip, String(toggleVal));
        }
        
        updateModification();
        modeSelectCB.setAlpha(1.);
    }
    
    
}

void SynchronicModificationEditor::bkTextFieldDidChange(TextEditor& tf)
{
    String text = tf.getText();
    String name = tf.getName();
    
    if (name == "Name")
    {
        processor.gallery->getSynchronicModPreparation(processor.updateState->currentModSynchronicId)->setName(text);
        
        int selected = selectCB.getSelectedId();
        if (selected != selectCB.getNumItems()) selectCB.changeItemText(selected, text);
        selectCB.setSelectedId(selected, dontSendNotification );
    }
    
    updateModification();
}

void SynchronicModificationEditor::BKEditableComboBoxChanged(String name, BKEditableComboBox* cb)
{
    SynchronicModPreparation::Ptr mod = processor.gallery->getSynchronicModPreparation(processor.updateState->currentModSynchronicId);
    
    mod->setName(name);
    
    updateModification();
}

void SynchronicModificationEditor::bkMessageReceived (const String& message)
{
    if (message == "synchronic/update")
    {
        update();
    }
}

void SynchronicModificationEditor::buttonClicked (Button* b)
{
    SynchronicModPreparation::Ptr mod = processor.gallery->getSynchronicModPreparation(processor.updateState->currentModSynchronicId);
    
    if (b == &offsetParamStartToggle)
    {
        int toggleVal;
        if(offsetParamStartToggle.getToggleState()) toggleVal = 1;
        else toggleVal = 0;
        
        if(((SynchronicSyncMode)mod->getParam(SynchronicMode).getIntValue()) == FirstNoteOnSync ||
           ((SynchronicSyncMode)mod->getParam(SynchronicMode).getIntValue())  == AnyNoteOnSync)
        {
            mod->setParam(SynchronicBeatsToSkip, String(toggleVal - 1));
        }
        else
        {
            mod->setParam(SynchronicBeatsToSkip, String(toggleVal));
        }
        
        offsetParamStartToggle.setAlpha(1.);
        
    }
    else if (b == &releaseVelocitySetsSynchronicToggle)
    {
        SynchronicPreparation::Ptr prep = processor.gallery->getStaticSynchronicPreparation(processor.updateState->currentSynchronicId);
        SynchronicPreparation::Ptr active = processor.gallery->getActiveSynchronicPreparation(processor.updateState->currentSynchronicId);
        
        // this for MODS
        // unimplemented for now
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

void SynchronicModificationEditor::updateModification(void)
{
    processor.updateState->modificationDidChange = true;
}



