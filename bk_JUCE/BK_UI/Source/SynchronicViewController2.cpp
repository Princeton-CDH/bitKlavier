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
    
    addAndMakeVisible(selectCB);
    selectCB.setName("Synchronic");
    selectCB.addSeparator();
    selectCB.addListener(this);
    //selectCB.setEditableText(true);
    selectCB.setSelectedItemIndex(0);
    fillSelectCB();
    
    /*
    // Labels
    synchronicL = OwnedArray<BKLabel>();
    synchronicL.ensureStorageAllocated(cSynchronicParameterTypes.size());
    
    for (int i = 0; i < cSynchronicParameterTypes.size(); i++)
    {
        synchronicL.set(i, new BKLabel());
        addAndMakeVisible(synchronicL[i]);
        synchronicL[i]->setName(cSynchronicParameterTypes[i]);
        synchronicL[i]->setText(cSynchronicParameterTypes[i], NotificationType::dontSendNotification);
    }
    
    // Text Fields
    synchronicTF = OwnedArray<BKTextField>();
    synchronicTF.ensureStorageAllocated(cSynchronicParameterTypes.size());
    
    for (int i = 0; i < cSynchronicParameterTypes.size(); i++)
    {
        synchronicTF.set(i, new BKTextField());
        addAndMakeVisible(synchronicTF[i]);
        synchronicTF[i]->addListener(this);
        synchronicTF[i]->setName(cSynchronicParameterTypes[i]);
    }
    
    modSynchronicTF = OwnedArray<BKTextField>();
    modSynchronicTF.ensureStorageAllocated(cSynchronicParameterTypes.size());
    
    for (int i = 0; i < cSynchronicParameterTypes.size(); i++)
    {
        modSynchronicTF.set(i, new BKTextField());
        addAndMakeVisible(modSynchronicTF[i]);
        modSynchronicTF[i]->addListener(this);
        modSynchronicTF[i]->setName("M"+cSynchronicParameterTypes[i]);
    }
    
    
    sliderTest = new BKMultiSlider(HorizontalMultiBarSlider); //or HorizontalMultiSlider, VerticalMultiSlider, HorizontalMultiBarSlider, VerticalMultiBarSlider
    addAndMakeVisible(sliderTest);
    sliderTest->addMyListener(this);
    
    
    updateModFields();
    updateFields();
     */
    

}

SynchronicViewController2::~SynchronicViewController2()
{
    
}


//sliderTest->addMyListener(&msliderListener);
void SynchronicViewController2::multiSliderValueChanged(String name, int whichSlider, Array<float> values)
{
    //DBG("received slider value " + name + " " + String(whichSlider) + " " + String(value));
    
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
    DBG("received all slider vals " + name);
    
    SynchronicPreparation::Ptr prep = processor.gallery->getStaticSynchronicPreparation(processor.updateState->currentSynchronicId);
    SynchronicPreparation::Ptr active = processor.gallery->getActiveSynchronicPreparation(processor.updateState->currentSynchronicId);
    
    Array<float> newvals;
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
    else if (name == cSynchronicParameterTypes[SynchronicTranspOffsets])
    {
        prep    ->setTransposition(values);
        active  ->setTransposition(values);
    }
    
    //processor.updateState->synchronicPreparationDidChange = true;
}

void SynchronicViewController2::paint (Graphics& g)
{
    g.setColour(Colours::goldenrod);
    g.drawRect(getLocalBounds(), 1);
}

void SynchronicViewController2::resized()
{
    Rectangle<int> area (getLocalBounds());
    Rectangle<int> rightColumn = area.removeFromRight(area.getWidth() * 0.5);
    
    int tempHeight = area.getHeight() / paramSliders.size();
    
    for(int i = 0; i < paramSliders.size(); i++)
    {
        paramSliders[i]->setBounds(area.removeFromBottom(tempHeight));
    }
    
    selectCB.setBounds(rightColumn.removeFromTop(20));
}

void SynchronicViewController2::updateFields()
{
    
    DBG("**** updating sliders ****");
    
    SynchronicPreparation::Ptr prep   = processor.gallery->getActiveSynchronicPreparation(processor.updateState->currentSynchronicId);
    
    for(int i = 0; i < paramSliders.size(); i++)
    {
        if(paramSliders[i]->getName() == cSynchronicParameterTypes[SynchronicAccentMultipliers])
        {
            paramSliders[i]->setTo(prep->getAccentMultipliers());
        }
        else if(paramSliders[i]->getName() == cSynchronicParameterTypes[SynchronicBeatMultipliers])
        {
            paramSliders[i]->setTo(prep->getBeatMultipliers());
        }
        else if(paramSliders[i]->getName() == cSynchronicParameterTypes[SynchronicLengthMultipliers])
        {
            paramSliders[i]->setTo(prep->getLengthMultipliers());
        }
        else if(paramSliders[i]->getName() == cSynchronicParameterTypes[SynchronicTranspOffsets])
        {
            paramSliders[i]->setTo(prep->getTransposition());
        }
    }
    
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
        
        updateFields();
    }
}

void SynchronicViewController2::bkMessageReceived (const String& message)
{
    if (message == "synchronic/update")
    {
        //updateFields();
        updateFields();
    }
}


