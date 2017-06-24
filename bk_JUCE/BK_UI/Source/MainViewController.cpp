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
header(p),
construction(p, &theGraph),
overtop(p, &theGraph),
timerCallbackCount(0)
{
    addKeyListener(this);
    
    gen = processor.gallery->getGeneralSettings();
    
    levelMeterComponentL = new BKLevelMeterComponent;
    addAndMakeVisible(levelMeterComponentL);
    
    mainSlider = new Slider();
    addAndMakeVisible (mainSlider);
    mainSlider->setRange (-90, 12.0, 0.1);
    mainSlider->getLookAndFeel().setColour(Slider::thumbColourId, Colours::goldenrod);
    mainSlider->getLookAndFeel().setColour(Slider::trackColourId, Colours::goldenrod.withMultipliedAlpha(0.75));
    mainSlider->getLookAndFeel().setColour(Slider::backgroundColourId, Colours::goldenrod.withMultipliedAlpha(0.25));
    mainSlider->setSkewFactor (2.5, false);
    mainSlider->setPopupMenuEnabled (true);
    mainSlider->setValue (0);
    mainSlider->setSliderStyle (Slider::LinearBarVertical);
    //mainSlider->setSliderStyle (Slider::LinearVertical);
    mainSlider->setTextBoxStyle (Slider::NoTextBox, false, 0, 0);
    mainSlider->setPopupDisplayEnabled (true, this);
    mainSlider->setDoubleClickReturnValue (true, 0.0); // double-clicking this slider will set it to 50.0
    mainSlider->setTextValueSuffix (" dB");
    mainSlider->addListener(this);
    
    addAndMakeVisible (keyboardComponent =
                       new BKKeymapKeyboardComponent (keyboardState, BKKeymapKeyboardComponent::horizontalKeyboard));
    
    keyboard =  ((BKKeymapKeyboardComponent*)keyboardComponent);
    
    keyboard->setScrollButtonsVisible(false);
    keyboard->setAvailableRange(21, 108);
    keyboard->setOctaveForMiddleC(4);
    keyboard->setFundamental(-1);
    keyboard->setAllowDrag(false);
    keyboard->doKeysToggle(false);
    keyboard->addMouseListener(this, true);
    keyboardState.addListener(this);
    
    //keyboardComponent.setAvailableRange(21, 108);
    //keyboardComponent.setScrollButtonsVisible(false);
    //keyboardComponent.setOctaveForMiddleC(4);
    //keyboardComponent.addMyListener (this);
    //addAndMakeVisible (keyboardComponent);
    /*
     absoluteKeyboard.setName("Key-by-Key Offsets");
     absoluteKeyboard.addMyListener(this);
     addAndMakeVisible(absoluteKeyboard);
    
     */
    
    preparationPanel = new PreparationPanel(processor);
    addAndMakeVisible(preparationPanel);
    
    addAndMakeVisible(header);
    addAndMakeVisible(construction);
    addChildComponent(overtop);
    
    /*
    File file ("~/bk_icons/icon.png");
    FileInputStream inputStream(file);
    PNGImageFormat tempimg;
    Image bimg = tempimg.decodeImage(inputStream);
    backgroundImageComponent.setImage(bimg);
    backgroundImageComponent.setImagePlacement(RectanglePlacement(juce::RectanglePlacement::stretchToFit));
    backgroundImageComponent.setAlpha(0.2);
    addAndMakeVisible(backgroundImageComponent);
     */
    
    startTimerHz (50);
    
}

MainViewController::~MainViewController()
{
    
}


void MainViewController::paint (Graphics& g)
{
    g.fillAll(Colours::black);
}

void MainViewController::resized()
{
   
    int headerHeight = 30;
    int sidebarWidth = 20;
    int footerHeight = 40;
    
    float paddingScalarX = (float)(getTopLevelComponent()->getWidth() - gMainComponentMinWidth) / (gMainComponentWidth - gMainComponentMinWidth);
    float paddingScalarY = (float)(getTopLevelComponent()->getHeight() - gMainComponentMinHeight) / (gMainComponentHeight - gMainComponentMinHeight);
    
    Rectangle<int> area (getLocalBounds());
    header.setBounds(area.removeFromTop(headerHeight));
    
    Rectangle<int> overtopSlice = area;
    overtopSlice.reduce(70 * paddingScalarX, 90 * paddingScalarY);
    overtop.setBounds(overtopSlice);
    
    Rectangle<int> footerSlice = area.removeFromBottom(footerHeight + footerHeight * paddingScalarY + gYSpacing);
    //float keyWidth = footerSlice.getWidth() / round((108 - 21) * 7./12 + 1);
    footerSlice.reduce(gXSpacing, gYSpacing);
    //keyboardComponent.setKeyWidth(keyWidth);
    //keyboardComponent.setBlackNoteLengthProportion(0.65);
    //keyboardComponent.setBounds(footerSlice);
    float keyWidth = footerSlice.getWidth() / round((108 - 21) * 7./12 + 1); //num white keys
    keyboard->setKeyWidth(keyWidth);
    keyboard->setBlackNoteLengthProportion(0.65);
    keyboardComponent->setBounds(footerSlice);
    
    Rectangle<int> levelMeterSlice = area.removeFromLeft(sidebarWidth + gXSpacing);
    levelMeterSlice.removeFromLeft(gXSpacing);
    levelMeterSlice.removeFromTop(2 * gYSpacing * paddingScalarY);
    levelMeterComponentL->setBounds(levelMeterSlice);
    
    Rectangle<int> gainSliderSlice = area.removeFromRight(sidebarWidth + gXSpacing);
    gainSliderSlice.removeFromRight(gXSpacing);
    //gainSliderSlice.reduce(0, 1);
    mainSlider->setBounds(gainSliderSlice);
    
    area.reduce(gXSpacing, 0);
    construction.setBounds(area);
    
    /*
    backgroundImageComponent.setBounds(getBounds());
    backgroundImageComponent.toBack();
     */
}



void MainViewController::bkButtonClicked (Button* b)
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

void MainViewController::handleKeymapNoteToggled (BKKeymapKeyboardState* source, int midiNoteNumber)
{
    DBG("MainViewController::handleKeymapNoteToggled " + String(midiNoteNumber));
}

void MainViewController::handleKeymapNoteOn (BKKeymapKeyboardState* source, int midiNoteNumber)
{
    DBG("MainViewController::handleKeymapNoteOn " + String(midiNoteNumber));
    processor.noteOnUI(midiNoteNumber);
}


void MainViewController::handleKeymapNoteOff (BKKeymapKeyboardState* source, int midiNoteNumber)
{
    DBG("MainViewController::handleKeymapNoteOff " + String(midiNoteNumber));
    processor.noteOffUI(midiNoteNumber);
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
        construction.addItem(PreparationTypeDirectMod, 1);
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
    
    Array<bool> noteOns = processor.getNoteOns();
    keyboardState.setKeymap(noteOns);
    keyboard->repaint();
    
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
        
        preparationPanel->refill(PreparationTypeDirect);
        preparationPanel->refill(PreparationTypeDirectMod);
        
        overtop.dvc2.updateFields();
    }
    
    if (processor.updateState->nostalgicPreparationDidChange)
    {
        processor.updateState->nostalgicPreparationDidChange = false;
        
        preparationPanel->refill(PreparationTypeNostalgic);
        preparationPanel->refill(PreparationTypeNostalgicMod);
        
        overtop.nvc2.updateFields();
    }
    
    if (processor.updateState->synchronicPreparationDidChange)
    {
        processor.updateState->synchronicPreparationDidChange = false;
        
        preparationPanel->refill(PreparationTypeSynchronic);
        preparationPanel->refill(PreparationTypeSynchronicMod);
        
        
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
    
    levelMeterComponentL->updateLevel(processor.getLevelL());
    //levelMeterComponentR->updateLevel(processor.getLevelL());
    
    
}


