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
construction(p, &theGraph, &viewPort),
galvc(p),
kvc(p, &theGraph),
tvc(p, &theGraph),
gvc(p),
//svc(p, &theGraph),
//nvc(p, &theGraph),
dvc(p, &theGraph),
ovc(p, &theGraph),
svc2(p, &theGraph),
nvc2(p, &theGraph),
dvc2(p, &theGraph),
timerCallbackCount(0)
{
    
    addKeyListener(this);
    
    
    gen = processor.gallery->getGeneralSettings();
    
    preparationPanel = new PreparationPanel(processor);
    addAndMakeVisible(preparationPanel);
    
    
    viewPort.setViewedComponent(&construction);
    viewPort.setViewPosition(0, 0);
    addAndMakeVisible(viewPort);
    
    addAndMakeVisible(galvc);
    
    overtop.addAndMakeVisible(hideOrShow);
    hideOrShow.setName("hideOrShow");
    hideOrShow.addListener(this);
    hideOrShow.setButtonText(" X ");
    

    addChildComponent(overtop);
    overtop.addChildComponent(gvc);
    //overtop.addChildComponent(dvc);
    overtop.addChildComponent(kvc);
    overtop.addChildComponent(tvc);
    overtop.addChildComponent(ovc);
    overtop.addChildComponent(svc2);
    overtop.addChildComponent(nvc2);
    overtop.addChildComponent(dvc2);
    
    /*
    addAndMakeVisible (levelMeterComponentL = new BKLevelMeterComponent());
    addAndMakeVisible (levelMeterComponentR = new BKLevelMeterComponent());
     */

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
    
    setCurrentDisplay(DisplayNil);
    
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
    galvc.setBounds(0, 0, getParentComponent()->getRight() - SOME_PADDING, 30);
    
    int panelWidth = 200;

    preparationPanel->setBounds(getParentComponent()->getRight() - SOME_PADDING, getParentComponent()->getY(), panelWidth, getParentComponent()->getHeight());
    
    viewPort.setBounds(gXSpacing,
                           galvc.getBottom(),
                           getParentComponent()->getWidth() - SOME_PADDING - 2*gXSpacing,
                           getParentComponent()->getBottom() - galvc.getBottom()-2*gYSpacing);
    
    construction.setSize(viewPort.getWidth(), viewPort.getHeight());
    
    overtop.setBounds(viewPort.getBounds());
    
    hideOrShow.setBounds(gXSpacing,gYSpacing, 20,20);
    

    float X = gXSpacing; float Y = hideOrShow.getBottom()+gYSpacing;
    float width = overtop.getWidth() - 10;
    float height = overtop.getHeight() - 35;
    
    kvc.setBounds(X, Y, width, height);
    
    gvc.setBounds(X, Y, width, height);
    
    tvc.setBounds(X, Y, width, height);
    
    ovc.setBounds(X, Y, width, height);
    
    svc2.setBounds(X, Y, width, height);
    
    nvc2.setBounds(X, Y, width, height);
    
    dvc2.setBounds(X, Y, width, height);
    

    
}


bool MainViewController::keyPressed (const KeyPress& e, Component*)
{
    if (e.isKeyCode(27)) // Escape
    {
        setCurrentDisplay(DisplayNil);
    }

}

void MainViewController::bkButtonClicked            (Button* b)
{
    String name = b->getName();
    
    if (name == "hideOrShow")
    {
        setCurrentDisplay(DisplayNil);
    }
}

void MainViewController::sliderValueChanged (Slider* slider)
{
    
    if(slider == mainSlider)
    {
        gen->setGlobalGain(Decibels::decibelsToGain(mainSlider->getValue()));
        gvc.updateFields();
    }
}


void MainViewController::timerCallback()
{
    
    if (++timerCallbackCount >= 100)
    {
        timerCallbackCount = 0;
        processor.collectGalleries();
        galvc.fillGalleryCB();
    }
    
    galvc.update();
    
    if (processor.updateState->generalSettingsDidChange)
    {
        processor.updateState->generalSettingsDidChange = false;
        gvc.updateFields();
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
        
        dvc.updateFields();
        dvc.updateModFields();
        
        dvc2.updateFields();
    }
    
    if (processor.updateState->nostalgicPreparationDidChange)
    {
        processor.updateState->nostalgicPreparationDidChange = false;
        
        preparationPanel->refill(PreparationTypeNostalgic);
        preparationPanel->refill(PreparationTypeNostalgicMod);
        
        //nvc.updateFields();
        //nvc.updateModFields();
        
        nvc2.updateFields();
    }
    
    if (processor.updateState->synchronicPreparationDidChange)
    {
        processor.updateState->synchronicPreparationDidChange = false;
        
        preparationPanel->refill(PreparationTypeSynchronic);
        preparationPanel->refill(PreparationTypeSynchronicMod);
        
        //svc.updateFields();
        //svc.updateModFields();
        
        svc2.updateFields();
    }
    
    if (processor.updateState->tuningPreparationDidChange)
    {
        processor.updateState->tuningPreparationDidChange = false;
        
        preparationPanel->refill(PreparationTypeTuning);
        preparationPanel->refill(PreparationTypeTuningMod);
        
        tvc.updateFields();
        tvc.updateModFields();
    }
    
    if (processor.updateState->tempoPreparationDidChange)
    {
        processor.updateState->tempoPreparationDidChange = false;
        
        preparationPanel->refill(PreparationTypeTempo);
        preparationPanel->refill(PreparationTypeTempoMod);
        
        ovc.updateFields();
        ovc.updateModFields();
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
        
        kvc.reset();
    }
    
    if (processor.updateState->directDidChange)
    {
        processor.updateState->directDidChange = false;
        
        preparationPanel->refill(PreparationTypeDirect);
        preparationPanel->refill(PreparationTypeDirectMod);
        
        dvc.reset();
    }
    
    if (processor.updateState->displayDidChange)
    {
        processor.updateState->displayDidChange = false;
        
        setCurrentDisplay(processor.updateState->currentPreparationDisplay);
    }
    
    /*
    levelMeterComponentL->updateLevel(processor.getLevelL());
    levelMeterComponentR->updateLevel(processor.getLevelL());
     */
    
}


void MainViewController::setCurrentDisplay(BKPreparationDisplay type)
{
    if (type == DisplayNil) {
        overtop.setVisible(false);
        processor.updateState->currentPreparationDisplay = DisplayNil;
        return;
    }
    else
        overtop.setVisible(true);
    
    kvc.setVisible(false);
    gvc.setVisible(false);
    tvc.setVisible(false);
    dvc.setVisible(false);
    ovc.setVisible(false);
    
    //svc.setVisible(false);
    //nvc.setVisible(false);
    
    svc2.setVisible(false);
    nvc2.setVisible(false);
    dvc2.setVisible(false);
    
    
    if (type == DisplayKeymap)
    {
        kvc.setVisible(true);
    }
    else if (type == DisplayTuning)
    {
        tvc.setVisible(true);
    }
    else if (type == DisplayTempo)
    {
        ovc.setVisible(true);
    }
    else if (type == DisplaySynchronic)
    {
        svc2.setVisible(true);
    }
    else if (type == DisplayNostalgic)
    {
        nvc2.setVisible(true);
    }
    else if (type == DisplayDirect)
    {
        dvc2.setVisible(true);
    }
    else if (type == DisplayGeneral)
    {
        gvc.setVisible(true);
    }
        
}
