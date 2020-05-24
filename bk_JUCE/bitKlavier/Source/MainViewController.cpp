/*
  ==============================================================================

    MainViewController.cpp
    Created: 27 Mar 2017 2:04:26pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginEditor.h"
#include "MainViewController.h"
#include "BKConstructionSite.h"

//==============================================================================
MainViewController::MainViewController (BKAudioProcessor& p, BKAudioProcessorEditor& e):
processor (p),
editor (e),
theGraph(p),
header(p, &construction),
construction(p, &theGraph),
overtop(p, &theGraph),
splash(p),
timerCallbackCount(0),
tooltipsButton("Show tooltips")
//preferencesButton ("Preferences")
{
    if (processor.platform == BKIOS)    display = DisplayConstruction;
    else                                display = DisplayDefault;

    tooltipsButton.setClickingTogglesState(true);
    tooltipsButton.getToggleStateValue().referTo(editor.getTooltipsEnabled());

    addAndMakeVisible(tooltipsButton);
    
    addAndMakeVisible(splash);
    splash.setAlwaysOnTop(true);
    
    initial = true;
    
    gen = processor.gallery->getGeneralSettings();
    
    levelMeterComponentL = std::make_unique<BKLevelMeterComponent>();
    addAndMakeVisible(*levelMeterComponentL);
    
    mainSlider.setLookAndFeel(&laf);

    mainSlider.setRange (-90, 12.0, 0.1);
    mainSlider.setSkewFactor (2.5, false);
    mainSlider.setValue (0);
    mainSlider.setSliderStyle (Slider::LinearBarVertical);
    mainSlider.setTextBoxStyle (Slider::NoTextBox, false, 0, 0);
    mainSlider.setDoubleClickReturnValue (true, 0.0); // double-clicking this slider will set it to 50.0
    mainSlider.setTextValueSuffix (" dB");
    
    mainSlider.addListener(this);
    
#if !JUCE_IOS
    mainSlider.setPopupMenuEnabled (true);
    mainSlider.setPopupDisplayEnabled (true, true, this);
    mainSlider.setTooltip("Controls dB output of bitKlavier audio");
#endif
    
    addAndMakeVisible (mainSlider);
    
    addAndMakeVisible (keyboardComponent =
                       new BKKeymapKeyboardComponent (keyboardState, BKKeymapKeyboardComponent::horizontalKeyboard));
    
#if JUCE_IOS
    keyStart = 60;  keyEnd = 72;
#else
    keyStart = 21;  keyEnd = 108;
#endif

    //~~~~~~~~~~~MENUS~~~~~~~~
    sampleCB.setLookAndFeel(&laf);
    sampleCB.setTooltip("Choose and load sample set from your soundfonts folder");
    
    instrumentCB.setLookAndFeel(&laf);
    instrumentCB.setTooltip("Load specific instrument from selected soundfont (if available)");
    
    sampleCB.addListener(this);
    instrumentCB.addListener(this);
    
    instrumentCB.setLookAndFeel(&comboBoxRightJustifyLAF);
    comboBoxRightJustifyLAF.setComboBoxJustificationType(juce::Justification::centredRight);
    
    addAndMakeVisible(sampleCB);
    addAndMakeVisible(instrumentCB);
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~`
    
    keyboard =  std::unique_ptr<BKKeymapKeyboardComponent>((BKKeymapKeyboardComponent*)keyboardComponent);
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
    
    preparationPanel = std::make_unique<PreparationPanel>(processor);
    addAndMakeVisible(*preparationPanel);
    
    addAndMakeVisible(header);
    
    addAndMakeVisible(construction);

    
    addChildComponent(overtop);
    
    juce::Point<int> myshadowOffset(2, 2);
    DropShadow myshadow(Colours::darkgrey, 5, myshadowOffset);
    overtopShadow = std::make_unique<DropShadower>(myshadow);
    overtopShadow->setOwner(&overtop);
    
    //preferencesButton.addListener (this);
    //preferencesButton.setTriggeredOnMouseDown (true);
    //preferencesButton.setLookAndFeel(&windowLAF);
    //addAndMakeVisible (preferencesButton);
    
    if (editor.areTooltipsEnabled() && tipwindow == nullptr)
    {
        tipwindow = std::make_unique<TooltipWindow>();
    }
    else if (!editor.areTooltipsEnabled() && tipwindow != nullptr)
    {
        tipwindow = nullptr;
    }
    
    // fill menus
    processor.collectGalleries();
    processor.collectPreparations();
    processor.collectPianos();
    processor.collectSoundfonts();
    
    header.fillGalleryCB();
    
    fillSampleCB();
    fillInstrumentCB();
    
    addMouseListener(this, true);
    // setWantsKeyboardFocus(true);
    addKeyListener(this);
    
    startTimerHz (10);
}

MainViewController::~MainViewController()
{
    removeMouseListener(this);
    keyboard->removeMouseListener(this);
    
    setLookAndFeel(nullptr);
    sampleCB.setLookAndFeel(nullptr);
    instrumentCB.setLookAndFeel(nullptr);
    octaveSlider.setLookAndFeel(nullptr);
    mainSlider.setLookAndFeel(nullptr);
    overtop.setLookAndFeel(nullptr);
    tooltipsButton.setLookAndFeel(nullptr);
    //preferencesButton.setLookAndFeel(nullptr);
    keyboardComponent = nullptr;
    removeKeyListener(this);
}

/*
void MainViewController::setSliderLookAndFeel(BKButtonAndMenuLAF *laf)
{
    octaveSlider.setLookAndFeel(laf);
    mainSlider.setLookAndFeel(laf);
}
 */


void MainViewController::paint (Graphics& g)
{
    g.fillAll(Colours::black);
    
    g.setColour(Colours::antiquewhite);
    
    Rectangle<float> bounds = construction.getBounds().toFloat();
    bounds.expand(2.0f,2.0f);
    g.drawRoundedRectangle(bounds, 2.0f, 0.5f);
    
    bounds = mainSlider.getBounds().toFloat();
    g.drawRoundedRectangle(bounds, 2.0, 0.5f);
    
    bounds = levelMeterComponentL->getBounds().toFloat();
    bounds.expand(1, 1);
    bounds.translate(0, -5);
    g.drawRoundedRectangle(bounds, 2.0, 0.5f);
}

void MainViewController::toggleDisplay(void)
{
    display = (display == DisplayKeyboard) ? DisplayConstruction : DisplayKeyboard;
    
    resized();
    repaint();
}

void MainViewController::resized()
{
    int headerHeight,sidebarWidth,footerHeight;

    splash.setBounds(getLocalBounds());
    
#if JUCE_IOS
    headerHeight = processor.screenHeight * 0.125;
    headerHeight = (headerHeight > 60) ? 60 : headerHeight;
    
    sidebarWidth = processor.screenWidth * 0.075;
    sidebarWidth = (sidebarWidth > 45) ? 45 : sidebarWidth;
#else
    headerHeight = 40;
    sidebarWidth = 30;
#endif
    footerHeight = 65;
    
    
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

        float unit = footerSlice.getWidth() * 0.25;
        
        //preferencesButton.setBounds (footerSlice.getX(), footerSlice.getY(), 100, 20);
        tooltipsButton.setBounds(footerSlice.getX(), footerSlice.getY(), 120, 20);
        
        sampleCB.setBounds(unit, footerSlice.getY(), unit-0.5*gXSpacing, 20);
        instrumentCB.setBounds(2*unit+0.5*gXSpacing, sampleCB.getY(), sampleCB.getWidth(), sampleCB.getHeight());
        
        float keyWidth = footerSlice.getWidth() / round((keyEnd - keyStart) * 7./12 + 1); //num white keys
        keyboard->setKeyWidth(keyWidth);
        keyboard->setBlackNoteLengthProportion(0.65);
        keyboardComponent->setBounds(footerSlice.getX(), sampleCB.getBottom() + gYSpacing, footerSlice.getWidth(), footerSlice.getHeight() - sampleCB.getHeight());
        keyboardComponent->setVisible(true);
    }
    
    Rectangle<int> gainSliderSlice = area.removeFromRight(sidebarWidth+gXSpacing);
    
    gainSliderSlice.removeFromLeft(gXSpacing);
    mainSlider.setBounds(gainSliderSlice.getX(), gainSliderSlice.getY(),
                          header.getRight() - gainSliderSlice.getX() - gXSpacing,
                          area.getHeight());
    
    Rectangle<int> levelMeterSlice = area.removeFromLeft(sidebarWidth+gXSpacing);
    levelMeterSlice.removeFromRight(gXSpacing);
    levelMeterSlice.reduce(1, 1);
    levelMeterComponentL->setBounds(header.getX()+gXSpacing, levelMeterSlice.getY()+5,
                          mainSlider.getWidth(),
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
        if (event.eventComponent == levelMeterComponentL.get())
        //if (event.originalComponent == &testmeterComponent)
        {
            toggleDisplay();
        }
#endif
    }
}

void MainViewController::bkComboBoxDidChange(ComboBox* cb)
{
    if (cb == &sampleCB)
    {
        String name = cb->getText();
        
        int selectedId = cb->getSelectedId();
        
        if (selectedId <= 4)
        {
            processor.loadSamples((BKSampleLoadType)(selectedId - 1));
        }
        else
        {
            int index = selectedId - BKLoadSoundfont - 1;
            
            processor.loadSamples(BKLoadSoundfont, processor.soundfontNames[index], 0);
        }
    }
    else if (cb == &instrumentCB)
    {
        processor.currentInstrument = cb->getSelectedItemIndex();
        processor.loadSamples(BKLoadSoundfont, processor.currentSoundfont, processor.currentInstrument);
    }
}

void MainViewController::bkButtonClicked (Button* b)
{
    String name = b->getName();
    //if (b == &preferencesButton)
    //{
    //    editor.showBKSettingsDialog(b);
    //}
}

void MainViewController::sliderValueChanged (Slider* slider)
{
    double value = slider->getValue();
    if (slider == &mainSlider)
    {
        processor.gallery->getGeneralSettings()->setGlobalGain(Decibels::decibelsToGain(value));
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
        BKPreparationDisplay currentDisplay = overtop.getCurrentDisplay();
        if(currentDisplay == DisplayDirect)
        {
            if(overtop.dvc.getSubWindowInFront()) overtop.dvc.closeSubWindow();
            else processor.updateState->setCurrentDisplay(DisplayNil);
        }
        else if(currentDisplay == DisplayNostalgic)
        {
            if(overtop.nvc.getSubWindowInFront()) overtop.nvc.closeSubWindow();
            else processor.updateState->setCurrentDisplay(DisplayNil);
        }
        else if(currentDisplay == DisplaySynchronic)
        {
            if(overtop.svc.getSubWindowInFront()) overtop.svc.closeSubWindow();
            else processor.updateState->setCurrentDisplay(DisplayNil);
        }
        else if (currentDisplay == DisplayKeymap)
        {
            Keymap::Ptr keymap = processor.gallery->getKeymap(processor.updateState->currentKeymapId);
            keymap->setMidiEdit(false);
            processor.updateState->setCurrentDisplay(DisplayNil);
        }
        else processor.updateState->setCurrentDisplay(DisplayNil);
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
        if (processor.updateState->currentDisplay == DisplayNil)
        {
            if (e.getModifiers().isCommandDown())   construction.align(1);
            else                                    construction.move(1, e.getModifiers().isShiftDown());
        }
        else
        {
            overtop.arrowPressed(RightArrow);
        }
    }
    else if (code == KeyPress::downKey)
    {
        
            if (e.getModifiers().isCommandDown())   construction.align(2);
            else                                    construction.move(2, e.getModifiers().isShiftDown());
        
        
    }
    else if (code == KeyPress::leftKey)
    {
        if (processor.updateState->currentDisplay == DisplayNil)
        {
            
            if (e.getModifiers().isCommandDown())   construction.align(3);
            else                                    construction.move(3, e.getModifiers().isShiftDown());
        }
        else
        {
            overtop.arrowPressed(LeftArrow);
        }
    }
    else if (code == KeyPress::tabKey)
    {
        
    }
    else if (code == 65) // A all
    {
        if (e.getModifiers().isCommandDown())   construction.selectAll();
    }
    else if (code == 66) // B blendronic
    {
        construction.addItem(PreparationTypeBlendronic);
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
    else if (code == 81) // Q comment
    {
        construction.addItem(PreparationTypeComment);
    }
    else if (code == 82) // R reset
    {
        construction.addItem(PreparationTypeReset);
    }
    else if (code == 83) // S synchronic
    {
        if (e.getModifiers().isCommandDown())
        {
            if (e.getModifiers().isShiftDown() || processor.defaultLoaded) 
                processor.saveCurrentGalleryAs();
            else processor.saveCurrentGallery();
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

    }
    else if (code == 69) // E
    {
        if (e.getModifiers().isCommandDown())
        {
            if (processor.updateState->currentDisplay == DisplayKeymap)
            {
                Keymap::Ptr keymap = processor.gallery->getKeymap(processor.updateState->currentKeymapId);
                ToggleButton* midiEditToggle = overtop.kvc.getMidiEditToggle();
                midiEditToggle->setToggleState(!midiEditToggle->getToggleState(), dontSendNotification);
                keymap->setMidiEdit(midiEditToggle->getToggleState());
            }
        }
    }
    else if (code == 85) // U
    {
        construction.connectAllSelected();
    }
    else if (code == 73) // I
    {
        construction.removeConnectionsBetween();
    }
    else if (code == 79) // O
    {
        construction.removeConnectionsTo();
    }
    
    return true;
}

void MainViewController::fillSampleCB()
{
    sampleCB.clear(dontSendNotification);

    sampleCB.addItem("Piano (litest)", 1);
    sampleCB.addItem("Piano (lite)", 2);
    sampleCB.addItem("Piano (medium)", 3);
    sampleCB.addItem("Piano (heavy)", 4);
    
    sampleCB.addSeparator();
    
    if (processor.currentSampleType <= BKLoadHeavy)
    {
        sampleCB.setSelectedItemIndex(processor.currentSampleType, dontSendNotification);
    }
    
    int id = 5;
    for (auto sf : processor.soundfontNames)
    {
		//windows paths
		String name;
#if JUCE_WINDOWS
		name = sf.fromLastOccurrenceOf("\\", false, true).upToFirstOccurrenceOf(".sf", false, true);
#else
        name = sf.fromLastOccurrenceOf("/", false, true).upToFirstOccurrenceOf(".sf", false, true);
#endif
        sampleCB.addItem(name, id);
        
        if ((processor.currentSampleType == BKLoadSoundfont) && (name == processor.getCurrentSoundfontName()))
        {
            sampleCB.setSelectedId(id, dontSendNotification);
        }
        
        id++;
    }
    
    
}

void MainViewController::fillInstrumentCB()
{
    instrumentCB.clear(dontSendNotification);
    
    // If theres only one instrument, dont bother showing name
    if (processor.currentSampleType < BKLoadSoundfont)
    {
        instrumentCB.setEnabled(false);
    }
    else
    {
        instrumentCB.setEnabled(true);
        
        int i = 1;
        for (auto inst : processor.instrumentNames)
        {
            if (inst == "") inst = "Instrument " + String(i);
            instrumentCB.addItem(inst, i++);
        }
        
        instrumentCB.setSelectedItemIndex(processor.currentInstrument, dontSendNotification);
    }
    
}


void MainViewController::timerCallback()
{
    BKUpdateState::Ptr state = processor.updateState;
    
    if (editor.areTooltipsEnabled() && tipwindow == nullptr)
    {
        tipwindow = std::make_unique<TooltipWindow>();
    }
    else if (!editor.areTooltipsEnabled() && tipwindow != nullptr)
    {
        tipwindow = nullptr;
    }
    
    // update menu contents periodically
    if (++timerCallbackCount >= 10)
    {
        timerCallbackCount = 0;
        processor.collectGalleries();
        processor.collectPreparations();
        processor.collectPianos();
        processor.collectSoundfonts();
        
        header.fillGalleryCB();
        
        fillSampleCB();
        fillInstrumentCB();
    }
    
    // display active noteOns on main keyboard
    Array<bool> noteOns = processor.getNoteOns();
    keyboardState.setKeymap(noteOns);
    
    // set main keyboard to display active keys in selected keymap (if there is a selected keymap)
    if (construction.getNumSelected() == 1)
    {
        BKItem::Ptr item = construction.getSelectedItems().getUnchecked(0);
        if (item->getType() == PreparationTypeKeymap)
        {
            keyboardState.setKeymap(processor.gallery->getKeymap(item->getId())->getKeymap());
        }
    }
    
    keyboard->repaint();
    
    if (state->pianoSamplesAreLoading)
    {
        splash.setVisible(true);
        
        splash.setProgress(processor.progress);
    }
    else
    {
        splash.setVisible(false);
    }
    
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
    float globalGain = processor.gallery->getGeneralSettings()->getGlobalGain();
    float genGain = Decibels::gainToDecibels(globalGain);
    if(genGain != mainSlider.getValue())
        mainSlider.setValue(Decibels::gainToDecibels(globalGain), dontSendNotification);
    
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

    if (state->blendronicPreparationDidChange)
    {
        state->blendronicPreparationDidChange = false;

        overtop.bvc.update();
        overtop.bvcm.update();
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
    
    if (state->commentDidChange)
    {
        state->commentDidChange = false;
        construction.getCurrentItem()->setCommentText(state->comment);
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
        
        header.update();
    }
    
    levelMeterComponentL->updateLevel(processor.getLevelL());
    //levelMeterComponentR->updateLevel(processor.getLevelL());
    
    
}
