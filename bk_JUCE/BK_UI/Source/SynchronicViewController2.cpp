/*
  ==============================================================================

    SynchronicViewController2.cpp
    Created: 21 Apr 2017 11:17:47pm
    Author:  Daniel Trueman

  ==============================================================================
*/

#include "SynchronicViewController2.h"
#include "BKUtilities.h"
#include "Keymap.h"
#include "Preparation.h"

SynchronicViewController2::SynchronicViewController2(BKAudioProcessor& p, BKItemGraph* theGraph):
processor(p),
theGraph(theGraph)
{
    SynchronicPreparation::Ptr prep = processor.gallery->getStaticSynchronicPreparation(processor.updateState->currentSynchronicId);
    
    // MultSliders
    paramSliders = OwnedArray<BKMultiSlider>();
    
    for (int i = 0; i < cSynchronicParameterTypes.size(); i++)
    {
        if (cSynchronicDataTypes[i] == BKFloatArr || cSynchronicDataTypes[i] == BKArrFloatArr)
        {
            paramSliders.insert(0, new BKMultiSlider(HorizontalMultiBarSlider));
            addAndMakeVisible(paramSliders[0]);
            paramSliders[0]->addMyListener(this);
            paramSliders[0]->setName(cSynchronicParameterTypes[i]);
            paramSliders[0]->setMinMaxDefaultInc(cSynchronicDefaultRangeValuesAndInc[i]);
        }
    }
    
    selectCB.setName("Synchronic");
    selectCB.addSeparator();
    selectCB.addListener(this);
    selectCB.setSelectedItemIndex(0);
    selectCB.addMyListener(this);
    fillSelectCB();
    addAndMakeVisible(selectCB);
    
    modeSelectCB.setName("Mode");
    modeSelectCB.addSeparator();
    modeSelectCB.addListener(this);
    modeSelectCB.setSelectedItemIndex(0);
    fillModeSelectCB();
    addAndMakeVisible(modeSelectCB);
    
    //offsetParamStartToggle = new BKSingleSlider("skip first", 0, 1, 0, 1);
    offsetParamStartToggle.addListener(this);
    offsetParamStartToggle.setButtonText ("skip first");
    offsetParamStartToggle.setToggleState (true, dontSendNotification);
    addAndMakeVisible(offsetParamStartToggle);
    
    howManySlider = new BKSingleSlider("how many", 1, 100, 1, 1);
    howManySlider->addMyListener(this);
    addAndMakeVisible(howManySlider);
    
    clusterThreshSlider = new BKSingleSlider("cluster threshold", 20, 2000, 200, 10);
    clusterThreshSlider->addMyListener(this);
    addAndMakeVisible(clusterThreshSlider);

    clusterMinMaxSlider = new BKRangeSlider("cluster min/max", 1, 10, 3, 4, 1);
    clusterMinMaxSlider->addMyListener(this);
    addAndMakeVisible(clusterMinMaxSlider);
    
    gainSlider = new BKSingleSlider("gain", 0, 10, 1, 0.01);
    gainSlider->setSkewFactorFromMidPoint(1.);
    gainSlider->addMyListener(this);
    addAndMakeVisible(gainSlider);
    
    startTimer(20);

}

SynchronicViewController2::~SynchronicViewController2()
{
    
}

/*
 "beat length multipliers",
 "sustain length multipliers",
 "accents",
 "transpositions"*/

void SynchronicViewController2::timerCallback()
{
    SynchronicProcessor::Ptr sProcessor = processor.gallery->getSynchronicProcessor(processor.updateState->currentSynchronicId);

    for (int i = 0; i < paramSliders.size(); i++)
    {
        if(paramSliders[i]->getName() == "beat length multipliers")
        {
            paramSliders[i]->setCurrentSlider(sProcessor->getBeatMultiplierCounter());
        }
        else if(paramSliders[i]->getName() == "sustain length multipliers")
        {
            paramSliders[i]->setCurrentSlider(sProcessor->getLengthMultiplierCounter());
        }
        else if(paramSliders[i]->getName() == "accents")
        {
            paramSliders[i]->setCurrentSlider(sProcessor->getAccentMultiplierCounter());
        }
        else if(paramSliders[i]->getName() == "transpositions")
        {
            paramSliders[i]->setCurrentSlider(sProcessor->getTranspCounter());
        }
    }
}


void SynchronicViewController2::multiSliderValueChanged(String name, int whichSlider, Array<float> values)
{
  
    //DBG("multiSliderValueChanged called");
    
    SynchronicPreparation::Ptr prep = processor.gallery->getStaticSynchronicPreparation(processor.updateState->currentSynchronicId);
    SynchronicPreparation::Ptr active = processor.gallery->getActiveSynchronicPreparation(processor.updateState->currentSynchronicId);
    
    if (name == cSynchronicParameterTypes[SynchronicAccentMultipliers])
    {
        prep    ->setAccentMultiplier(whichSlider, values[0]);
        active  ->setAccentMultiplier(whichSlider, values[0]);
    }
    else if (name == cSynchronicParameterTypes[SynchronicBeatMultipliers])
    {
        prep    ->setBeatMultiplier(whichSlider, values[0]);
        active  ->setBeatMultiplier(whichSlider, values[0]);
    }
    else if (name == cSynchronicParameterTypes[SynchronicLengthMultipliers])
    {
        prep    ->setLengthMultiplier(whichSlider, values[0]);
        active  ->setLengthMultiplier(whichSlider, values[0]);
    }
    else if (name == cSynchronicParameterTypes[SynchronicTranspOffsets])
    {
        prep    ->setSingleTransposition(whichSlider, values);
        active  ->setSingleTransposition(whichSlider, values);
    }
    
    //processor.updateState->synchronicPreparationDidChange = true;

}

void SynchronicViewController2::multiSliderAllValuesChanged(String name, Array<Array<float>> values)
{

    //DBG("multiSliderALLValueChanged called");
    
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

void SynchronicViewController2::BKSingleSliderValueChanged(String name, double val)
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

void SynchronicViewController2::BKRangeSliderValueChanged(String name, double minval, double maxval)
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

void SynchronicViewController2::paint (Graphics& g)
{
    g.fillAll(Colours::lightgrey);
}

void SynchronicViewController2::resized()
{
    Rectangle<int> area (getLocalBounds());
    Rectangle<int> oneColumn = area.removeFromLeft(area.getWidth() * 0.5);
    
    Rectangle<int> modeSlice = area.removeFromTop(24);
    modeSlice.reduce(4, 2);
    modeSelectCB.setBounds(modeSlice.removeFromLeft(modeSlice.getWidth() / 2));
    offsetParamStartToggle.setBounds(modeSlice);
    
    int tempHeight = area.getHeight() / paramSliders.size();
    
    for(int i = 0; i < paramSliders.size(); i++)
    {
        paramSliders[i]->setBounds(area.removeFromBottom(tempHeight));
    }

    Rectangle<int> comboBoxSlice = oneColumn.removeFromTop(24);
    comboBoxSlice.reduce(4, 2);
    selectCB.setBounds(comboBoxSlice);
    
    int oneColumnRowHeight = oneColumn.getHeight() / 4.;
    howManySlider->setBounds(oneColumn.removeFromTop(oneColumnRowHeight));
    clusterThreshSlider->setBounds(oneColumn.removeFromTop(oneColumnRowHeight));
    clusterMinMaxSlider->setBounds(oneColumn.removeFromTop(oneColumnRowHeight));
    gainSlider->setBounds(oneColumn.removeFromTop(oneColumnRowHeight));

}

void SynchronicViewController2::updateFields(NotificationType notify)
{
    
    //DBG("**** updating synchronic sliders **** prep # " + String(processor.updateState->currentSynchronicId) + " " + String(notify));
    
    SynchronicPreparation::Ptr prep   = processor.gallery->getActiveSynchronicPreparation(processor.updateState->currentSynchronicId);
    
    selectCB.setSelectedItemIndex(processor.updateState->currentSynchronicId, notify);
    modeSelectCB.setSelectedItemIndex(prep->getMode(), notify);
    offsetParamStartToggle.setToggleState(prep->getOffsetParamToggle(), notify);
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
        else if(paramSliders[i]->getName() == cSynchronicParameterTypes[SynchronicBeatMultipliers])
        {
            paramSliders[i]->setTo(prep->getBeatMultipliers(), notify);
        }
        else if(paramSliders[i]->getName() == cSynchronicParameterTypes[SynchronicLengthMultipliers])
        {
            paramSliders[i]->setTo(prep->getLengthMultipliers(), notify);
        }
        else if(paramSliders[i]->getName() == cSynchronicParameterTypes[SynchronicTranspOffsets])
        {
            paramSliders[i]->setTo(prep->getTransposition(), notify);
        }
    }
    
}

void SynchronicViewController2::updateFields()
{
    updateFields(dontSendNotification);
}

void SynchronicViewController2::fillSelectCB(void)
{
    // Direct menu
    Synchronic::PtrArr newpreps = processor.gallery->getAllSynchronic();
    
    selectCB.clear(dontSendNotification);
    for (int i = 0; i < newpreps.size(); i++)
    {
        String name = newpreps[i]->getName();
        if (name != String::empty)  selectCB.addItem(name, i+1);
        else                        selectCB.addItem(String(i+1), i+1);
    }
    
    selectCB.addItem("New synchronic...", newpreps.size()+1);
    
    selectCB.setSelectedItemIndex(processor.updateState->currentSynchronicId, NotificationType::dontSendNotification);
    
}

//SynchronicSyncMode
void SynchronicViewController2::fillModeSelectCB(void)
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



void SynchronicViewController2::bkComboBoxDidChange (ComboBox* box)
{
    String name = box->getName();
    
    if (name == "Synchronic")
    {
        processor.updateState->currentSynchronicId = box->getSelectedItemIndex();
        
        if (processor.updateState->currentSynchronicId == selectCB.getNumItems()-1)
        {
            processor.gallery->addSynchronic();
            
            fillSelectCB();
        }
        
        //updateFields(sendNotification);
        updateFields();
        
    }
    else if (name == "Mode")
    {
        SynchronicPreparation::Ptr prep = processor.gallery->getStaticSynchronicPreparation(processor.updateState->currentSynchronicId);
        SynchronicPreparation::Ptr active = processor.gallery->getActiveSynchronicPreparation(processor.updateState->currentSynchronicId);
        
        prep    ->setMode((SynchronicSyncMode) box->getSelectedItemIndex());
        active  ->setMode((SynchronicSyncMode) box->getSelectedItemIndex());
        
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

void SynchronicViewController2::bkTextFieldDidChange(TextEditor& tf)
{
    String text = tf.getText();
    String name = tf.getName();
    
    //SynchronicPreparation::Ptr prep = processor.gallery->getStaticSynchronicPreparation(processor.updateState->currentSynchronicId);
    //SynchronicPreparation::Ptr active = processor.gallery->getActiveSynchronicPreparation(processor.updateState->currentSynchronicId);
    
    if (name == "Name")
    {
        processor.gallery->getSynchronic(processor.updateState->currentSynchronicId)->setName(text);
        
        int selected = selectCB.getSelectedId();
        if (selected != selectCB.getNumItems()) selectCB.changeItemText(selected, text);
        selectCB.setSelectedId(selected, dontSendNotification );
    }
}

void SynchronicViewController2::BKEditableComboBoxChanged(String name, BKEditableComboBox* cb)
{
    processor.gallery->getSynchronic(processor.updateState->currentSynchronicId)->setName(name);
    
    int selected = selectCB.getSelectedId();
    if (selected != selectCB.getNumItems()) selectCB.changeItemText(selected, name);
    selectCB.setSelectedId(selected, dontSendNotification );
}

void SynchronicViewController2::bkMessageReceived (const String& message)
{
    if (message == "synchronic/update")
    {
        updateFields();
    }
}

void SynchronicViewController2::buttonClicked (Button* b)
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
}


