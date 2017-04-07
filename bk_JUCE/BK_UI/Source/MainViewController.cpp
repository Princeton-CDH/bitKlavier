/*
  ==============================================================================

    MainViewController.cpp
    Created: 27 Mar 2017 2:04:26pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "MainViewController.h"
#include "juce_Decibels.h"

//==============================================================================
MainViewController::MainViewController (BKAudioProcessor& p):
processor (p),
construction(p),
galvc(p),
kvc(p),
tvc(p),
gvc(p),
svc(p),
nvc(p),
dvc(p),
ovc(p),
timerCallbackCount(0)
{
    gen = processor.gallery->getGeneralSettings();
    
    for (int i = 0; i < cDisplayNames.size(); i++)
    {
        buttons.add(new TextButton());
        
        buttons[i]->setName(cDisplayNames[i]);
        buttons[i]->setButtonText(cDisplayNames[i]);
        
        buttons[i]->changeWidthToFitText();
        buttons[i]->addListener(this);
        addAndMakeVisible(buttons[i]);
        
    }
    
    
    preparationPanel = new PreparationPanel(processor);
    addAndMakeVisible(preparationPanel);
    
    addAndMakeVisible(construction);
    
    addAndMakeVisible(galvc);
    
    addAndMakeVisible(gvc);
    addAndMakeVisible(svc);
    addAndMakeVisible(nvc);
    addAndMakeVisible(dvc);
    addAndMakeVisible(kvc);
    addAndMakeVisible(tvc);
    addAndMakeVisible(ovc);
    
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
    
    setCurrentDisplay(DisplayDirect);
    
    
    startTimerHz (50);
    
}

MainViewController::~MainViewController()
{
    
}


void MainViewController::paint (Graphics& g)
{
    g.fillAll(Colours::dimgrey);
}

#define SOME_PADDING 350
void MainViewController::resized()
{
    galvc.setBounds(0, 0, gVCWidth+2*gXSpacing, getHeight()*.38);
    
    // Place buttons.
    float buttonWidth = ((getRight()-galvc.getRight() - 7 * gXSpacing - SOME_PADDING)/7.0f);
    float buttonHeight = 30;
    buttons[0]->setBounds(galvc.getRight()+gXSpacing, gYSpacing, buttonWidth, buttonHeight);
    for (int i = 1; i < cDisplayNames.size(); i++)
    {
        buttons[i]->setBounds(buttons[i-1]->getRight()+gXSpacing, gYSpacing, buttonWidth, buttonHeight);
    }
    
    /*
     float kvcH = cKeymapParameterTypes.size() * (gComponentTextFieldHeight + gYSpacing) + 1.5 * gYSpacing;
     float gvcH = cGeneralParameterTypes.size() * (gComponentTextFieldHeight + gYSpacing) + 1.5 * gYSpacing;
     float svcH = cSynchronicParameterTypes.size() * (gComponentTextFieldHeight + gYSpacing) + 1.5 * gYSpacing;
     float nvcH = cNostalgicParameterTypes.size()  * (gComponentTextFieldHeight + gYSpacing) + 1.5 *  gYSpacing;
     float dvcH = cDirectParameterTypes.size()  * (gComponentTextFieldHeight + gYSpacing) + 1.5 * gYSpacing;
     float tvcH = cTuningParameterTypes.size()  * (gComponentTextFieldHeight + gYSpacing) + gYSpacing;
     float ovcH = cTempoParameterTypes.size()  * (gComponentTextFieldHeight + gYSpacing) + gYSpacing;
     */
    
    kvc.setBounds(galvc.getRight() + gXSpacing,
                  buttons[0]->getBottom() + gYSpacing,
                  getRight() - galvc.getRight() - 2*gXSpacing - SOME_PADDING,
                  getHeight()*.38);
    
    gvc.setBounds(kvc.getBounds());
    tvc.setBounds(kvc.getBounds());
    dvc.setBounds(kvc.getBounds());
    svc.setBounds(kvc.getBounds());
    nvc.setBounds(kvc.getBounds());
    ovc.setBounds(kvc.getBounds());
    
    int panelWidth = 250;
    preparationPanel->setBounds(getScreenBounds().getRight() - SOME_PADDING, getScreenBounds().getY(), panelWidth, getScreenBounds().getHeight());
    
    construction.setBounds(gXSpacing, kvc.getBottom() + gYSpacing, getScreenBounds().getWidth() - SOME_PADDING - 2*gXSpacing, getScreenBounds().getBottom() - kvc.getBottom()-2*gYSpacing);
    
    /*
    float mainSliderHeight = 320;
    float mainSliderWidth = 70;
    float mainSliderTextBoxWidth = 50;
    float mainSliderTextBoxHeight = 20;
    mainSlider->setTextBoxStyle (Slider::TextBoxBelow, false, mainSliderTextBoxWidth, mainSliderTextBoxHeight);
    
    float levelMeterHeight = mainSliderHeight - mainSliderTextBoxHeight;
    float levelMeterWidth = 20;
    
    
    //levelMeterComponent->setBounds(galvc.getRight() + gXSpacing, getBottom()-1.5*levelMeterHeight, levelMeterWidth, levelMeterHeight);
    levelMeterComponentL->setBounds(kvc.getRight() + gXSpacing,
                                    kvc.getBottom() - levelMeterHeight - mainSliderTextBoxHeight,
                                    levelMeterWidth,
                                    levelMeterHeight);
    
    mainSlider->setBounds (levelMeterComponentL->getRight() - mainSliderWidth * 0.5 + gXSpacing,
                           levelMeterComponentL->getBottom() - mainSliderHeight + mainSliderTextBoxHeight,
                           mainSliderWidth,
                           mainSliderHeight);
    
    
    levelMeterComponentR->setBounds(mainSlider->getRight() - mainSliderWidth * 0.5 + gXSpacing,
                                    kvc.getBottom() - levelMeterHeight - mainSliderTextBoxHeight,
                                    levelMeterWidth,
                                    levelMeterHeight);
     */
    
    
    
    
    
    
    
}


void MainViewController::bkButtonClicked            (Button* b)
{
    String name = b->getName();
    if (name == cDisplayNames[DisplayKeymap])
    {
        setCurrentDisplay(DisplayKeymap);
    }
    else if (name == cDisplayNames[DisplayDirect])
    {
        setCurrentDisplay(DisplayDirect);
    }
    else if (name == cDisplayNames[DisplaySynchronic])
    {
        setCurrentDisplay(DisplaySynchronic);
    }
    else if (name == cDisplayNames[DisplayNostalgic])
    {
        setCurrentDisplay(DisplayNostalgic);
    }
    else if (name == cDisplayNames[DisplayTempo])
    {
        setCurrentDisplay(DisplayTempo);
    }
    else if (name == cDisplayNames[DisplayTuning])
    {
        setCurrentDisplay(DisplayTuning);
    }
    else if (name == cDisplayNames[DisplayGeneral])
    {
        setCurrentDisplay(DisplayGeneral);
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
    }
    
    if (processor.updateState->nostalgicPreparationDidChange)
    {
        processor.updateState->nostalgicPreparationDidChange = false;
        
        preparationPanel->refill(PreparationTypeNostalgic);
        preparationPanel->refill(PreparationTypeNostalgicMod);
        
        nvc.updateFields();
        nvc.updateModFields();
    }
    
    if (processor.updateState->synchronicPreparationDidChange)
    {
        processor.updateState->synchronicPreparationDidChange = false;
        
        preparationPanel->refill(PreparationTypeSynchronic);
        preparationPanel->refill(PreparationTypeSynchronicMod);
        
        svc.updateFields();
        svc.updateModFields();
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
    
    
    if (processor.updateState->keymapDidChange)
    {
        processor.updateState->keymapDidChange = false;
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
    kvc.setVisible(false);
    gvc.setVisible(false);
    tvc.setVisible(false);
    dvc.setVisible(false);
    svc.setVisible(false);
    nvc.setVisible(false);
    ovc.setVisible(false);
    
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
        svc.setVisible(true);
    }
    else if (type == DisplayNostalgic)
    {
        nvc.setVisible(true);
    }
    else if (type == DisplayDirect)
    {
        dvc.setVisible(true);
    }
    else if (type == DisplayGeneral)
    {
        gvc.setVisible(true);
    }
        
}
