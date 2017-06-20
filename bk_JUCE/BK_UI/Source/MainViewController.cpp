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
    addKeyListener(this);
    
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

void MainViewController::resized()
{    
    header.setBounds(0, 0, getParentComponent()->getRight(), 30);

    construction.setBounds(gXSpacing,
                           header.getBottom(),
                           getParentComponent()->getWidth() - 2*gXSpacing,
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

bool MainViewController::keyPressed (const KeyPress& e, Component*)
{
    DBG(e.getKeyCode());
    
    int code = e.getKeyCode();
    
    if (code == KeyPress::escapeKey)
    {
        processor.updateState->setCurrentDisplay(DisplayNil);
    }
    else if (code == KeyPress::deleteKey)
    {
        construction.remove();
    }
    else if (code == KeyPress::backspaceKey)
    {
        construction.remove();
    }
    else if (code == KeyPress::upKey)
    {
        if (e.getModifiers().isCommandDown())   construction.align(0);
        else                                    construction.move(0, e.getModifiers().isShiftDown());
    }
    else if (code == KeyPress::rightKey)
    {
        if (e.getModifiers().isCommandDown())   construction.align(1);
        else                                    construction.move(1, e.getModifiers().isShiftDown());
    }
    else if (code == KeyPress::downKey)
    {
        if (e.getModifiers().isCommandDown())   construction.align(2);
        else                                    construction.move(2, e.getModifiers().isShiftDown());
    }
    else if (code == KeyPress::leftKey)
    {
        if (e.getModifiers().isCommandDown())   construction.align(3);
        else                                    construction.move(3, e.getModifiers().isShiftDown());
    }
    else if (code == KeyPress::tabKey)
    {
        
    }
    else if (code == 67) // C modification
    {
        construction.addItem(PreparationTypeMod, 1);
    }
    else if (code == 68) // D direct
    {
        construction.addItem(PreparationTypeDirect, 1);
    }
    else if (code == 75) // K keymap
    {
        construction.addItem(PreparationTypeKeymap, 1);
    }
    else if (code == 77) // M tempo
    {
        construction.addItem(PreparationTypeTempo, 1);
    }
    else if (code == 78) // N nostalgic
    {
        construction.addItem(PreparationTypeNostalgic, 1);
    }
    else if (code == 80) // P piano
    {
        construction.addItem(PreparationTypePianoMap, -1);
    }
    else if (code == 82) // R reset
    {
        construction.addItem(PreparationTypeReset, -1);
    }
    else if (code == 83) // S synchronic
    {
        construction.addItem(PreparationTypeSynchronic, 1);
    }
    else if (code == 84) // T tuning
    {
        construction.addItem(PreparationTypeTuning, 1);
    }
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
    
    if (processor.updateState->idDidChange)
    {
        processor.updateState->idDidChange = false;
        
        construction.idDidChange();
    }
    
    if (processor.updateState->directPreparationDidChange)
    {
        processor.updateState->directPreparationDidChange = false;
        
        overtop.dvc.updateFields();
        overtop.dvcm.updateFields();
    }
    
    if (processor.updateState->nostalgicPreparationDidChange)
    {
        processor.updateState->nostalgicPreparationDidChange = false;
        
        overtop.nvc.updateFields();
        overtop.nvcm.updateFields();
    }
    
    if (processor.updateState->synchronicPreparationDidChange)
    {
        processor.updateState->synchronicPreparationDidChange = false;
        
        overtop.svc.updateFields();
        overtop.svcm.updateFields();
    }
    
    if (processor.updateState->tuningPreparationDidChange)
    {
        processor.updateState->tuningPreparationDidChange = false;
        
        overtop.tvc.updateFields();
        overtop.tvcm.updateFields();
    }
    
    if (processor.updateState->tempoPreparationDidChange)
    {
        processor.updateState->tempoPreparationDidChange = false;
        
        overtop.ovc.updateFields();
        overtop.ovcm.updateFields();
    }
    
    if (processor.updateState->pianoDidChangeForGraph)
    {
        processor.updateState->pianoDidChangeForGraph = false;
        
        construction.redraw();
    }
    
    
    if (processor.updateState->keymapDidChange)
    {
        processor.updateState->keymapDidChange = false;
    
        overtop.kvc.reset();
    }
    
    if (processor.updateState->displayDidChange)
    {
        processor.updateState->displayDidChange = false;
        
        overtop.setCurrentDisplay(processor.updateState->currentDisplay);
    }
    
    /*
    levelMeterComponentL->updateLevel(processor.getLevelL());
    levelMeterComponentR->updateLevel(processor.getLevelL());
     */
    
}


