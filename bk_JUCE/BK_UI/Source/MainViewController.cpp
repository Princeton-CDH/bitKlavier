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
construction(p, &viewPort, &theGraph),
overtop(p, &theGraph),
timerCallbackCount(0)
{
    initial = true;
    addMouseListener(this, true);
    
    setWantsKeyboardFocus(true);

    addKeyListener(this);
    
    gen = processor.gallery->getGeneralSettings();
    
    levelMeterComponentL = new BKLevelMeterComponent;
    addAndMakeVisible(levelMeterComponentL);
    
    mainSlider = new Slider();
    addAndMakeVisible (mainSlider);
    mainSlider->setRange (-90, 12.0, 0.1);
    mainSlider->getLookAndFeel().setColour(Slider::thumbColourId, Colours::goldenrod);
    mainSlider->getLookAndFeel().setColour(Slider::trackColourId, Colours::goldenrod.withMultipliedAlpha(0.25));
    mainSlider->getLookAndFeel().setColour(Slider::backgroundColourId, Colours::goldenrod.withMultipliedAlpha(0.1));
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
    keyboard->setAllowDrag(true);
    keyboard->doKeysToggle(false);
    keyboard->addMouseListener(this, true);
    keyboardState.addListener(this);
    
    preparationPanel = new PreparationPanel(processor);
    addAndMakeVisible(preparationPanel);
    
    addAndMakeVisible(header);
    addChildComponent(overtop);

    viewPort.setViewedComponent(&construction);
    viewPort.setViewPosition(0, 0);
    viewPort.setScrollBarsShown(false, false, true, true);
    
    addAndMakeVisible(viewPort);
    
    
    Point<int> myshadowOffset(2, 2);
    DropShadow myshadow(Colours::darkgrey, 5, myshadowOffset);
    overtopShadow = new DropShadower(myshadow);
    overtopShadow->setOwner(&overtop);
    
    startTimerHz (50);
    
}

MainViewController::~MainViewController()
{
    removeKeyListener(this);
}


void MainViewController::paint (Graphics& g)
{
    g.fillAll(Colours::black);
    
    g.setColour(Colours::white);
    
    Rectangle<int> bounds = viewPort.getBounds().translated(-1, -1);
    bounds.expand(2,2);
    g.drawRect(bounds, 1);
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
    //overtopSlice.reduce(70 * paddingScalarX, 90 * paddingScalarY);
    overtopSlice.removeFromTop(60 * paddingScalarY);
    overtopSlice.removeFromBottom(120 * paddingScalarY);
    overtopSlice.reduce(70 * paddingScalarX, 0);
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
    
    area.reduce(gXSpacing, 3);
    viewPort.setBounds(area);
    viewPort.toBack();
    
    if (initial)
    {
        initial = false;
        initialWidth = viewPort.getWidth(); initialHeight = viewPort.getHeight();
        construction.setSize(initialWidth, initialHeight);
    }
    
    if (!construction.itemOutsideBounds(viewPort.getBounds()))
    {
        if (viewPort.getWidth() > initialWidth) construction.setSize(viewPort.getWidth(), construction.getHeight());
        
        if (viewPort.getHeight() > initialHeight) construction.setSize(construction.getWidth(), viewPort.getHeight());
    }
    
    
    
    
    /*
    backgroundImageComponent.setBounds(getBounds());
    backgroundImageComponent.toBack();
     */
}

void MainViewController::mouseDown(const MouseEvent &event)
{
    if(event.eventComponent == &construction)
    {
        
        if (overtop.getCurrentDisplay() != DisplayNil)
        {
            processor.updateState->setCurrentDisplay(DisplayNil);
        }
    }
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
        overtop.gvc.update();
    }
}


void MainViewController::handleNoteOn(BKKeymapKeyboardState* source, int midiNoteNumber, float velocity) 
{
    processor.noteOnUI(midiNoteNumber);
}

void MainViewController::handleNoteOff(BKKeymapKeyboardState* source, int midiNoteNumber, float velocity)
{
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
        construction.deleteSelected();
    }
    else if (code == KeyPress::backspaceKey)
    {
        construction.deleteSelected();
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
    else if (code == 65) // A all
    {
        if (e.getModifiers().isCommandDown())   construction.selectAll();
    }
    else if (code == 67) // C modification
    {
        if (e.getModifiers().isCommandDown())   construction.copy();
        else                                    construction.addItem(PreparationTypeGenericMod);
    }
    else if (code == 68) // D direct
    {
        construction.addItem(PreparationTypeDirect);
    }
    else if (code == 75) // K keymap
    {
        construction.addItem(PreparationTypeKeymap);
    }
    else if (code == 77) // M tempo
    {
        construction.addItem(PreparationTypeTempo);
    }
    else if (code == 78) // N nostalgic
    {
        construction.addItem(PreparationTypeNostalgic);
    }
    else if (code == 80) // P piano
    {
        construction.addItem(PreparationTypePianoMap);
    }
    else if (code == 82) // R reset
    {
        construction.addItem(PreparationTypeReset);
    }
    else if (code == 83) // S synchronic
    {
        if (e.getModifiers().isCommandDown())
        {
            if (e.getModifiers().isShiftDown()) processor.saveGalleryAs();
            else                                processor.saveGallery();
        }
        else                                    construction.addItem(PreparationTypeSynchronic);
    }
    else if (code == 84) // T tuning
    {
        construction.addItem(PreparationTypeTuning);
    }
    else if (code == 86) // V
    {
        if (e.getModifiers().isCommandDown())   construction.paste();
    }
    else if (code == 88) // X
    {
        if (e.getModifiers().isCommandDown())   construction.cut();
    }
    return true;
}



void MainViewController::timerCallback()
{
    BKUpdateState::Ptr state = processor.updateState;
    
    if (++timerCallbackCount >= 100)
    {
        timerCallbackCount = 0;
        processor.collectGalleries();
        header.fillGalleryCB();
    }
    
    
    
    Array<bool> noteOns = processor.getNoteOns();
    keyboardState.setKeymap(noteOns);
    keyboard->repaint();
    
    if (state->galleryDidChange)
    {
        state->galleryDidChange = false;
        
        header.switchGallery();
    }
    
    if (state->generalSettingsDidChange)
    {
        state->generalSettingsDidChange = false;
        overtop.gvc.update();
    }
    
    //check to see if General Settings globalGain has changed, update slider accordingly
    float genGain = Decibels::gainToDecibels(gen->getGlobalGain());
    if(genGain != mainSlider->getValue())
        mainSlider->setValue(Decibels::gainToDecibels(gen->getGlobalGain()), dontSendNotification);
    
    if (state->modificationDidChange)
    {
        state->modificationDidChange = false;
        
        processor.currentPiano->configure();
    }
    
    if (state->idDidChange)
    {
        state->idDidChange = false;
        
        construction.idDidChange();
    }
    
    if (state->directPreparationDidChange)
    {
        state->directPreparationDidChange = false;
        
        overtop.dvc.update();
        overtop.dvcm.update();
    }
    
    if (state->nostalgicPreparationDidChange)
    {
        state->nostalgicPreparationDidChange = false;
        
        overtop.nvc.update();
        overtop.nvcm.update();
    }
    
    if (state->synchronicPreparationDidChange)
    {
        state->synchronicPreparationDidChange = false;
        
        overtop.svc.update();
        overtop.svcm.update();
    }
    
    if (state->tuningPreparationDidChange)
    {
        state->tuningPreparationDidChange = false;
        
        overtop.tvc.update();
        overtop.tvcm.update();
    }
    
    if (state->tempoPreparationDidChange)
    {
        state->tempoPreparationDidChange = false;
        
        overtop.ovc.update();
        overtop.ovcm.update();
    }
    
    if (state->pianoDidChangeForGraph)
    {
        state->pianoDidChangeForGraph = false;
        
        construction.redraw();
        
        header.fillPianoCB();
    }
    
    
    if (state->keymapDidChange)
    {
        state->keymapDidChange = false;
    
        overtop.kvc.reset();
    }
    
    if (state->displayDidChange)
    {
        state->displayDidChange = false;
        
        overtop.setCurrentDisplay(processor.updateState->currentDisplay);
    }
    
    levelMeterComponentL->updateLevel(processor.getLevelL());
    //levelMeterComponentR->updateLevel(processor.getLevelL());
    
    
}


