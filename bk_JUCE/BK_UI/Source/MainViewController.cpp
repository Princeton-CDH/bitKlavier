/*
  ==============================================================================

    MainViewController.cpp
    Created: 27 Mar 2017 2:04:26pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "MainViewController.h"

//==============================================================================
MainViewController::MainViewController (BKAudioProcessor& p):
processor (p),
theGraph(p),
construction(p, &theGraph),
overtop(p, &theGraph),
header(p),
timerCallbackCount(0)
{
    
    gen = processor.gallery->getGeneralSettings();
    
    preparationPanel = new PreparationPanel(processor);
    addAndMakeVisible(preparationPanel);
    
    addAndMakeVisible(construction);
    
    addAndMakeVisible(header);
    
    addChildComponent(overtop);

    mainSlider = new Slider();
    addAndMakeVisible (mainSlider);
    mainSlider->setRange (-90, 12.0, 0.1);
    mainSlider->setSkewFactor (2.5, false);
    mainSlider->setPopupMenuEnabled (true);
    mainSlider->setValue (0);
    mainSlider->setSliderStyle (Slider::LinearVertical);
    mainSlider->setDoubleClickReturnValue (true, 0.0); // double-clicking this slider will set it to 50.0
    mainSlider->setTextValueSuffix (" dB");
    mainSlider->addListener(this);
    
    startTimerHz (50);
    
}

MainViewController::~MainViewController()
{
    
}


void MainViewController::paint (Graphics& g)
{
    g.fillAll(Colours::dimgrey);
}

#define SOME_PADDING 200
void MainViewController::resized()
{    
    header.setBounds(0, 0, getParentComponent()->getRight() - SOME_PADDING, 30);
    
    int panelWidth = 200;

    preparationPanel->setBounds(getParentComponent()->getRight() - SOME_PADDING, getParentComponent()->getY(), panelWidth, getParentComponent()->getHeight());
    
    construction.setBounds(gXSpacing,
                           header.getBottom(),
                           getParentComponent()->getWidth() - SOME_PADDING - 2*gXSpacing,
                           getParentComponent()->getBottom() - header.getBottom()-2*gYSpacing);
    
    
    overtop.setBounds(construction.getBounds());
}



void MainViewController::bkButtonClicked            (Button* b)
{
    String name = b->getName();

}

void MainViewController::sliderValueChanged (Slider* slider)
{
    
    if(slider == mainSlider)
    {
        gen->setGlobalGain(Decibels::decibelsToGain(mainSlider->getValue()));
        overtop.gvc.updateFields();
    }
}

void MainViewController::deletePressed(void)
{
    construction.remove();
}

void MainViewController::arrowPressed(int which, bool fine)
{
    construction.move(which, fine);
}

void MainViewController::align(int which)
{
    construction.align(which);
}

void MainViewController::tabPressed(void)
{
    
}

void MainViewController::escapePressed(void)
{
    processor.updateState->setCurrentDisplay(DisplayNil);
}

void MainViewController::timerCallback()
{
    
    if (++timerCallbackCount >= 100)
    {
        timerCallbackCount = 0;
        processor.collectGalleries();
        header.fillGalleryCB();
    }
    
    header.update();
    
    if (processor.updateState->generalSettingsDidChange)
    {
        processor.updateState->generalSettingsDidChange = false;
        overtop.gvc.updateFields();
    }
    
    //check to see if General Settings globalGain has changed, update slider accordingly
    float genGain = Decibels::gainToDecibels(gen->getGlobalGain());
    if(genGain != mainSlider->getValue())
        mainSlider->setValue(Decibels::gainToDecibels(gen->getGlobalGain()), dontSendNotification);
    
    if (processor.updateState->directPreparationDidChange)
    {
        processor.updateState->directPreparationDidChange = false;
        
        preparationPanel->refill(PreparationTypeDirect);
        preparationPanel->refill(PreparationTypeDirectMod);
        
        overtop.dvc.updateFields();
        overtop.dvc.updateModFields();
        
        overtop.dvc2.updateFields();
    }
    
    if (processor.updateState->nostalgicPreparationDidChange)
    {
        processor.updateState->nostalgicPreparationDidChange = false;
        
        preparationPanel->refill(PreparationTypeNostalgic);
        preparationPanel->refill(PreparationTypeNostalgicMod);
        
        //nvc.updateFields();
        //nvc.updateModFields();
        
        overtop.nvc2.updateFields();
    }
    
    if (processor.updateState->synchronicPreparationDidChange)
    {
        processor.updateState->synchronicPreparationDidChange = false;
        
        preparationPanel->refill(PreparationTypeSynchronic);
        preparationPanel->refill(PreparationTypeSynchronicMod);
        
        //overtop.svc.updateFields();
        //overtop.svc.updateModFields();
        
        overtop.svc2.updateFields();
    }
    
    if (processor.updateState->tuningPreparationDidChange)
    {
        processor.updateState->tuningPreparationDidChange = false;
        
        preparationPanel->refill(PreparationTypeTuning);
        preparationPanel->refill(PreparationTypeTuningMod);
        
        //overtop.tvc.updateFields();
        //overtop.tvc.updateModFields();
        
        overtop.tvc2.updateFields();
    }
    
    if (processor.updateState->tempoPreparationDidChange)
    {
        processor.updateState->tempoPreparationDidChange = false;
        
        preparationPanel->refill(PreparationTypeTempo);
        preparationPanel->refill(PreparationTypeTempoMod);
        
        overtop.ovc2.updateFields();
        //overtop.ovc2.updateModFields();
    }
    
    if (processor.updateState->pianoDidChangeForGraph)
    {
        processor.updateState->pianoDidChangeForGraph = false;
        
        construction.redraw();
    }
    
    
    if (processor.updateState->keymapDidChange)
    {
        processor.updateState->keymapDidChange = false;
        
        preparationPanel->refill(PreparationTypeKeymap);
        
        overtop.kvc.reset();
    }
    
    if (processor.updateState->directDidChange)
    {
        processor.updateState->directDidChange = false;
        
        preparationPanel->refill(PreparationTypeDirect);
        preparationPanel->refill(PreparationTypeDirectMod);
        
        overtop.dvc.reset();
    }
    
    if (processor.updateState->displayDidChange)
    {
        processor.updateState->displayDidChange = false;
        
        overtop.setCurrentDisplay(processor.updateState->currentPreparationDisplay);
    }
    
    /*
    levelMeterComponentL->updateLevel(processor.getLevelL());
    levelMeterComponentR->updateLevel(processor.getLevelL());
     */
    
}


