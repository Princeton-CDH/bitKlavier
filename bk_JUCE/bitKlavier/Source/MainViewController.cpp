/*
  ==============================================================================

    MainViewController.cpp
    Created: 27 Mar 2017 2:04:26pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "MainViewController.h"
#include "BKConstructionSite.h"

//==============================================================================
MainViewController::MainViewController (BKAudioProcessor& p):
processor (p),
theGraph(p),
header(p, &construction),
construction(p, &theGraph),
overtop(p, &theGraph),
timerCallbackCount(0)
{
    if (processor.platform == BKIOS)
        display = DisplayConstruction;
    else
        display = DisplayDefault;
    
    initial = true;
    addMouseListener(this, true);
    
    setWantsKeyboardFocus(true);

    addKeyListener(this);
    
    gen = processor.gallery->getGeneralSettings();
    
    levelMeterComponentL = new BKLevelMeterComponent;
    addAndMakeVisible(levelMeterComponentL);
    
    mainSlider = new Slider();
    mainSlider->setLookAndFeel(&laf);
    
    mainSlider->setRange (-90, 12.0, 0.1);
    mainSlider->setSkewFactor (2.5, false);
    mainSlider->setPopupMenuEnabled (true);
    mainSlider->setValue (0);
    mainSlider->setSliderStyle (Slider::LinearBarVertical);
    mainSlider->setTextBoxStyle (Slider::NoTextBox, false, 0, 0);
    mainSlider->setPopupDisplayEnabled (true, true, this);
    mainSlider->setDoubleClickReturnValue (true, 0.0); // double-clicking this slider will set it to 50.0
    mainSlider->setTextValueSuffix (" dB");
    mainSlider->addListener(this);

    
    addAndMakeVisible (mainSlider);
    

    addAndMakeVisible (keyboardComponent =
                       new BKKeymapKeyboardComponent (keyboardState, BKKeymapKeyboardComponent::horizontalKeyboard));
    
#if JUCE_IOS
    keyStart = 60;  keyEnd = 72;
#else
    keyStart = 21;  keyEnd = 108;
#endif

    
    keyboard =  ((BKKeymapKeyboardComponent*) keyboardComponent);
    keyboard->setScrollButtonsVisible(false);
    keyboard->setAvailableRange(keyStart, keyEnd);
    keyboard->setOctaveForMiddleC(4);
    keyboard->setFundamental(-1);
    keyboard->setAllowDrag(true);
    keyboard->doKeysToggle(false);
    keyboard->addMouseListener(this, true);
    keyboardState.addListener(this);
    
    octaveSlider.setRange(0, 6, 1);
    octaveSlider.addListener(this);
    octaveSlider.setLookAndFeel(&laf);
    octaveSlider.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    octaveSlider.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    octaveSlider.setValue(3);

    addAndMakeVisible(octaveSlider);
    
    preparationPanel = new PreparationPanel(processor);
    addAndMakeVisible(preparationPanel);
    
    addAndMakeVisible(header);
    addAndMakeVisible(construction);
    addChildComponent(overtop);

    
    juce::Point<int> myshadowOffset(2, 2);
    DropShadow myshadow(Colours::darkgrey, 5, myshadowOffset);
    overtopShadow = new DropShadower(myshadow);
    overtopShadow->setOwner(&overtop);
    
    startTimerHz (50);
}

MainViewController::~MainViewController()
{
    setLookAndFeel(nullptr);
    octaveSlider.setLookAndFeel(nullptr);
    mainSlider->setLookAndFeel(nullptr);
    removeKeyListener(this);
}

/*
void MainViewController::setSliderLookAndFeel(BKButtonAndMenuLAF *laf)
{
    octaveSlider.setLookAndFeel(laf);
    mainSlider->setLookAndFeel(laf);
}
 */


void MainViewController::paint (Graphics& g)
{
    g.fillAll(Colours::black);
    
    g.setColour(Colours::antiquewhite);
    
    Rectangle<float> bounds = construction.getBounds().toFloat();
    bounds.expand(2.0f,2.0f);
    g.drawRoundedRectangle(bounds, 2.0f, 0.5f);
    
    bounds = mainSlider->getBounds().toFloat();
    g.drawRoundedRectangle(bounds, 2.0, 0.5f);
    
    bounds = levelMeterComponentL->getBounds().toFloat();
    bounds.expand(1, 1);
    bounds.translate(0, -5);
    g.drawRoundedRectangle(bounds, 2.0, 0.5f);
}

void MainViewController::setDisplay(DisplayType type)
{
    display = type;
    
    resized();
    repaint();
}

void MainViewController::resized()
{
    int headerHeight,sidebarWidth,footerHeight;
    
#if JUCE_IOS
    headerHeight = processor.screenHeight * 0.125;
    headerHeight = (headerHeight > 60) ? 60 : headerHeight;
    
    sidebarWidth = processor.screenWidth * 0.075;
    sidebarWidth = (sidebarWidth > 45) ? 45 : sidebarWidth;
#else
    headerHeight = 40;
    sidebarWidth = 30;
#endif
    footerHeight = 50;
    
    
    Rectangle<int> area (getLocalBounds());
    header.setBounds(area.removeFromTop(headerHeight));
    
    Rectangle<int> overtopSlice;
    
    if (processor.screenWidth > 640 || processor.screenHeight > 480)
    {
        //overtopSlice = area;
        //overtopSlice.removeFromTop(60 * processor.paddingScalarY);
        //overtopSlice.removeFromBottom(60 * processor.paddingScalarY);
        //overtopSlice.reduce(70 * processor.paddingScalarX, 0);
        overtopSlice = construction.getBounds();
    }
    else
    {
        overtopSlice = getLocalBounds();
    }
    
    overtop.setBounds(overtopSlice);
    
    if (display == DisplayDefault)
    {
        Rectangle<int> footerSlice = area.removeFromBottom(footerHeight + footerHeight * processor.paddingScalarY + gYSpacing);
        footerSlice.reduce(gXSpacing, gYSpacing);
        float keyWidth = footerSlice.getWidth() / round((keyEnd - keyStart) * 7./12 + 1); //num white keys
        keyboard->setKeyWidth(keyWidth);
        keyboard->setBlackNoteLengthProportion(0.65);
        keyboardComponent->setBounds(footerSlice);
        keyboardComponent->setVisible(true);
    }
    
    
    
    Rectangle<int> gainSliderSlice = area.removeFromRight(sidebarWidth+gXSpacing);
    
    gainSliderSlice.removeFromLeft(gXSpacing);
    mainSlider->setBounds(gainSliderSlice.getX(), gainSliderSlice.getY(),
                          header.getRight() - gainSliderSlice.getX() - gXSpacing,
                          area.getHeight());
    
    Rectangle<int> levelMeterSlice = area.removeFromLeft(sidebarWidth+gXSpacing);
    levelMeterSlice.removeFromRight(gXSpacing);
    levelMeterSlice.reduce(1, 1);
    levelMeterComponentL->setBounds(header.getX()+gXSpacing, levelMeterSlice.getY()+5,
                          mainSlider->getWidth(),
                          levelMeterSlice.getHeight());
    
    area.reduce(2, 2);
    construction.setBounds(area);
    
    

    if (display == DisplayKeyboard)
    {
        int octaveSliderHeight = 20;
        octaveSlider.setTopLeftPosition(area.getX(), area.getY());
        octaveSlider.setSize(area.getWidth(), octaveSliderHeight);
        
        //area.reduce(0, octaveSliderHeight*0.5);
        
        float keyWidth = area.getWidth() / round((keyEnd - keyStart) * 7./12 + 1); //num white keys
        keyboard->setKeyWidth(keyWidth);
        keyboard->setBlackNoteLengthProportion(0.6);
        area.removeFromTop(octaveSliderHeight + gYSpacing);
        keyboardComponent->setBounds(area);
        
        octaveSlider.setVisible(true);
        keyboardComponent->setVisible(true);
                               
        construction.setVisible(false);
    }
    else if (display == DisplayConstruction)
    {
        construction.setVisible(true);
        
        octaveSlider.setVisible(false);
        keyboardComponent->setVisible(false);
    }
    
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
    else
    {
#if JUCE_IOS
        if (event.eventComponent == levelMeterComponentL)
        {
            setDisplay((display == DisplayKeyboard) ? DisplayConstruction : DisplayKeyboard);
        }
#endif
    }
}



void MainViewController::bkButtonClicked (Button* b)
{
    String name = b->getName();

}

void MainViewController::sliderValueChanged (Slider* slider)
{
    
    if (slider == mainSlider)
    {
        gen->setGlobalGain(Decibels::decibelsToGain(mainSlider->getValue()));
        overtop.gvc.update();
    }
    else if (display == DisplayKeyboard && slider == &octaveSlider)
    {
        int octave = (int) octaveSlider.getValue();
        
        if (octave == 0)    keyboard->setAvailableRange(21, 45);
        else                keyboard->setAvailableRange(12+octave*12, 36+octave*12);
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
    else if (code == 90) // Z
    {
#if TRY_UNDO
        if (e.getModifiers().isCommandDown())
        {
            if (e.getModifiers().isShiftDown())
            {
                construction.redo();
            }
            else
            {
                construction.undo();
            }
        }
#endif
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
    
    if (state->galleriesUpdated)
    {
        state->galleriesUpdated = false;
        
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


