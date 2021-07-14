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
preferencesButton("Preferences"),
globalSoundSetButton("Use global samples"),
sustainPedalButton("Sustain Pedal"),
equalizerButton("Equalizer")
{
    if (processor.platform == BKIOS)    display = DisplayConstruction;
    else                                display = DisplayDefault;
    
    globalSoundSetButton.setClickingTogglesState(true);
    globalSoundSetButton.addListener(this);
    addAndMakeVisible(globalSoundSetButton);

    sustainPedalButton.setClickingTogglesState(true);
    sustainPedalButton.addListener(this);
    addAndMakeVisible(sustainPedalButton);
    sustainPedalButton.setVisible(false);
    
    addAndMakeVisible(splash);
    splash.setAlwaysOnTop(true);
    
    initial = true;
    
    gen = processor.gallery->getGeneralSettings();
    
    levelMeterComponentL = std::make_unique<BKLevelMeterComponent>();
    addAndMakeVisible(*levelMeterComponentL);
    
    mainSlider.setLookAndFeel(&laf);

    mainSlider.setRange (-100, 12.0, 0.1);
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
    sampleCB.beforeOpen = [this] { processor.collectSoundfonts(); fillSampleCB(); };
    
    instrumentCB.setLookAndFeel(&comboBoxRightJustifyLAF);
    comboBoxRightJustifyLAF.setComboBoxJustificationType(juce::Justification::centredRight);
    instrumentCB.setTooltip("Load specific instrument from selected soundfont (if available)");
    instrumentCB.beforeOpen = [this] { fillInstrumentCB(); };
    
    sampleCB.addListener(this);
    instrumentCB.addListener(this);
    
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
    
    preferencesButton.addListener (this);
    preferencesButton.setTriggeredOnMouseDown (true);
    preferencesButton.setLookAndFeel(&windowLAF);
    addAndMakeVisible (preferencesButton);
    
    // Equalizer
    equalizerButton.addListener(this);
    equalizerButton.setTriggeredOnMouseDown(true);
    equalizerButton.setLookAndFeel(&windowLAF);
    addAndMakeVisible(equalizerButton);
    
//    undoStatus.setLookAndFeel(&laf);
    addChildComponent(undoStatus);
    undoStatusCountdown = 0;
    
    // fill menus
    processor.collectGalleries();
    processor.collectPreparations();
    processor.collectPianos();
    processor.collectSoundfonts();
    
    fillSampleCB();
    fillInstrumentCB();
    
    addMouseListener(this, true);
    setWantsKeyboardFocus(true);
    addKeyListener(this);
    
    startTimerHz (MVC_REFRESH_RATE);
}

MainViewController::~MainViewController()
{
    stopTimer();
    
    PopupMenu::dismissAllActiveMenus();
    
    removeMouseListener(this);
    keyboard->removeMouseListener(this);
    keyboardState.removeListener(this);
    octaveSlider.removeListener(this);
    globalSoundSetButton.removeListener(this);
    sustainPedalButton.removeListener(this);
    preferencesButton.removeListener(this);
    equalizerButton.removeListener(this);
    mainSlider.removeListener(this);
    sampleCB.removeListener(this);
    instrumentCB.removeListener(this);
    removeKeyListener(this);
    
    setLookAndFeel(nullptr);
    sampleCB.setLookAndFeel(nullptr);
    instrumentCB.setLookAndFeel(nullptr);
    octaveSlider.setLookAndFeel(nullptr);
    mainSlider.setLookAndFeel(nullptr);
    overtop.setLookAndFeel(nullptr);
    undoStatus.setLookAndFeel(nullptr);
    globalSoundSetButton.setLookAndFeel(nullptr);
    //sustainPedalButton.setLookAndFeel(nullptr);
    preferencesButton.setLookAndFeel(nullptr);
    equalizerButton.setLookAndFeel(nullptr);
    keyboardComponent = nullptr;
    
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
        
        footerSlice.reduce(gXSpacing * 0.5f, gYSpacing);

        float unit = footerSlice.getWidth() * 0.25;
        
        preferencesButton.setBounds (footerSlice.getX(), footerSlice.getY(), 100, 20);
        
        equalizerButton.setBounds(footerSlice.getWidth() - 100, footerSlice.getY(), 100, 20);

		//original spacing to restore once tooltips/keystrokes/hotkeys get moved to a separate menu
		sampleCB.setBounds(unit, footerSlice.getY(), unit - 0.5 * gXSpacing, 20);
		instrumentCB.setBounds(2 * unit + 0.5 * gXSpacing, sampleCB.getY(), sampleCB.getWidth(), sampleCB.getHeight());
        
        globalSoundSetButton.setBounds(instrumentCB.getRight()+0.5*gXSpacing, sampleCB.getY(), 120, 20);

        sustainPedalButton.setBounds(globalSoundSetButton.getRight() + 0.4 * gXSpacing, sampleCB.getY(), 120, 20);
            
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
    area.removeFromLeft(1);
    construction.setBounds(area);
    
    undoStatus.setFont(undoStatus.getFont().withHeight(area.getHeight() * 0.05f));
    undoStatus.setBounds(area.getX() + gXSpacing, area.getY() + gXSpacing, area.getWidth() * 0.5,  undoStatus.getFont().getHeight());

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
        
        Rectangle<int> sampleCBSlice = area.removeFromBottom(area.getHeight() * 0.1);
        int width = sampleCBSlice.getWidth() * 0.25;
        sampleCBSlice.removeFromLeft(width);
        sampleCB.setBounds(sampleCBSlice.removeFromLeft(width));
        instrumentCB.setBounds(sampleCBSlice.removeFromLeft(width));
        globalSoundSetButton.setBounds(sampleCBSlice.removeFromLeft(width*0.5));
        sustainPedalButton.setBounds(sampleCBSlice.removeFromLeft(width * 0.4));
        sampleCB.toFront(false);
        instrumentCB.toFront(false);
        globalSoundSetButton.toFront(false);
        sustainPedalButton.toFront(false);
        
        octaveSlider.setVisible(false);
        keyboardComponent->setVisible(false);
    }
}

void MainViewController::mouseDown(const MouseEvent &event)
{
	// header.fillGalleryCB();
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
    fillSampleCB();
    fillInstrumentCB();
}

void MainViewController::mouseUp(const MouseEvent &event)
{
    fillSampleCB();
    fillInstrumentCB();
}

void MainViewController::bkComboBoxDidChange(ComboBox* cb)
{
    DirectPreparation::Ptr dPrep;
    SynchronicPreparation::Ptr sPrep;
    NostalgicPreparation::Ptr nPrep;
    DirectModification::Ptr dMod;
    SynchronicModification::Ptr sMod;
    NostalgicModification::Ptr nMod;
    bool directSelected = false;
    bool directModSelected = false;
    bool synchronicSelected = false;
    bool synchronicModSelected = false;
    bool nostalgicSelected = false;
    bool nostalgicModSelected = false;
    if (!globalSoundSetButton.getToggleState() && construction.getNumSelected() == 1)
    {
        BKItem* item = construction.getSelectedItems().getUnchecked(0);
        if (item->getType() == PreparationTypeDirect)
        {
            dPrep = processor.gallery->getDirectPreparation(item->getId());
            directSelected = true;
        }
        else if (item->getType() == PreparationTypeSynchronic)
        {
            sPrep = processor.gallery->getSynchronicPreparation(item->getId());
            synchronicSelected = true;
        }
        else if (item->getType() == PreparationTypeNostalgic)
        {
            nPrep = processor.gallery->getNostalgicPreparation(item->getId());
            nostalgicSelected = true;
        }
        // Modifications
        else if (item->getType() == PreparationTypeDirectMod)
        {
            dMod = processor.gallery->getDirectModification(item->getId());
            directModSelected = true;
        }
        else if (item->getType() == PreparationTypeSynchronicMod)
        {
            sMod = processor.gallery->getSynchronicModification(item->getId());
            synchronicModSelected = true;
        }
        else if (item->getType() == PreparationTypeNostalgicMod)
        {
            nMod = processor.gallery->getNostalgicModification(item->getId());
            nostalgicModSelected = true;
        }
    }
    
    if (cb == &sampleCB)
    {
        String name = cb->getText();
        
        int selectedId = cb->getSelectedId();
        
        int soundSetId;

        if (selectedId <= BKLoadSoundfont)
        {
            soundSetId = processor.loadSamples((BKSampleLoadType)(selectedId - 1), String(), 0, globalSoundSetButton.getToggleState());
        }
        else if (selectedId <= processor.soundfontNames.size() + BKLoadSoundfont)
        {
            int index = selectedId - BKLoadSoundfont - 1;
            
            soundSetId = processor.loadSamples(BKLoadSoundfont, processor.soundfontNames[index], 0, globalSoundSetButton.getToggleState());
        }
        else //if (selectedId < cb->getNumItems())
        {
            int index = selectedId - processor.soundfontNames.size() - BKLoadSoundfont - 1;
            soundSetId = processor.loadSamples(BKLoadCustom, processor.customSampleSetNames[index], 0, globalSoundSetButton.getToggleState());
        }
//        else
//        {
//            FileChooser chooser("Import sample files...",
//                                File::getSpecialLocation (File::userHomeDirectory));
//
//            if (chooser.browseForDirectory())
//            {
//                String path = chooser.getResult().getFullPathName();
//                soundSetId = processor.loadSamples(BKLoadCustom, path, 0, globalSoundSetButton.getToggleState());
//                fillSampleCB();
//            }
//            else
//            {
//                sampleCB.setSelectedId(lastSelectedSampleCBId, dontSendNotification);
//                return;
//            }
//        }
        
        String soundSetName = processor.loadedSoundSets[soundSetId].fromLastOccurrenceOf(File::getSeparatorString(), false, false);
        if (directSelected)
        {
            dPrep->dSoundSet.set(soundSetId);
            dPrep->dSoundSetName.set(soundSetName);
        }
        else if (synchronicSelected)
        {
            sPrep->sSoundSet.set(soundSetId);
            sPrep->sSoundSetName.set(soundSetName);
        }
        else if (nostalgicSelected)
        {
            nPrep->nSoundSet.set(soundSetId);
            nPrep->nSoundSetName.set(soundSetName);
        }
        else if (directModSelected)
        {
            dMod->dSoundSet.set(soundSetId);
            dMod->dSoundSetName.set(soundSetName);
            dMod->setDirty(DirectSoundSet);
        }
        else if (synchronicModSelected)
        {
            sMod->sSoundSet.set(soundSetId);
            sMod->sSoundSetName.set(soundSetName);
            sMod->setDirty(SynchronicSoundSet);
        }
        else if (nostalgicModSelected)
        {
            nMod->nSoundSet.set(soundSetId);
            nMod->nSoundSetName.set(soundSetName);
            nMod->setDirty(NostalgicSoundSet);
        }
        
        lastSelectedSampleCBId = selectedId;
    }
    else if (cb == &instrumentCB)
    {
        if (directSelected)
        {
            String sfname = processor.loadedSoundSets[dPrep->getSoundSet()].upToLastOccurrenceOf(".subsound", false, false);
            int soundSetId = processor.loadSamples(BKLoadSoundfont, sfname, cb->getSelectedItemIndex(), false);
            String soundSetName = processor.loadedSoundSets[soundSetId].fromLastOccurrenceOf(File::getSeparatorString(), false, false);
            dPrep->dSoundSet.set(soundSetId);
            dPrep->dSoundSetName.set(soundSetName);
        }
        else if (synchronicSelected)
        {
            String sfname = processor.loadedSoundSets[sPrep->getSoundSet()].upToLastOccurrenceOf(".subsound", false, false);
            int soundSetId = processor.loadSamples(BKLoadSoundfont, sfname, cb->getSelectedItemIndex(), false);
            String soundSetName = processor.loadedSoundSets[soundSetId].fromLastOccurrenceOf(File::getSeparatorString(), false, false);
            sPrep->sSoundSet.set(soundSetId);
            sPrep->sSoundSetName.set(soundSetName);
        }
        else if (nostalgicSelected)
        {
            String sfname = processor.loadedSoundSets[nPrep->getSoundSet()].upToLastOccurrenceOf(".subsound", false, false);
            int soundSetId = processor.loadSamples(BKLoadSoundfont, sfname, cb->getSelectedItemIndex(), false);
            String soundSetName = processor.loadedSoundSets[soundSetId].fromLastOccurrenceOf(File::getSeparatorString(), false, false);
            nPrep->nSoundSet.set(soundSetId);
            nPrep->nSoundSetName.set(soundSetName);
        }
        // Modifications
        else if (directModSelected)
        {
            String sfname = processor.loadedSoundSets[dMod->getSoundSet()].upToLastOccurrenceOf(".subsound", false, false);
            int soundSetId = processor.loadSamples(BKLoadSoundfont, sfname, cb->getSelectedItemIndex(), false);
            String soundSetName = processor.loadedSoundSets[soundSetId].fromLastOccurrenceOf(File::getSeparatorString(), false, false);
            dMod->dSoundSet.set(soundSetId);
            dMod->dSoundSetName.set(soundSetName);
            dMod->setDirty(DirectSoundSet);
        }
        else if (synchronicModSelected)
        {
            String sfname = processor.loadedSoundSets[sMod->getSoundSet()].upToLastOccurrenceOf(".subsound", false, false);
            int soundSetId = processor.loadSamples(BKLoadSoundfont, sfname, cb->getSelectedItemIndex(), false);
            String soundSetName = processor.loadedSoundSets[soundSetId].fromLastOccurrenceOf(File::getSeparatorString(), false, false);
            sMod->sSoundSet.set(soundSetId);
            sMod->sSoundSetName.set(soundSetName);
            sMod->setDirty(SynchronicSoundSet);
        }
        else if (nostalgicModSelected)
        {
            String sfname = processor.loadedSoundSets[nMod->getSoundSet()].upToLastOccurrenceOf(".subsound", false, false);
            int soundSetId = processor.loadSamples(BKLoadSoundfont, sfname, cb->getSelectedItemIndex(), false);
            String soundSetName = processor.loadedSoundSets[soundSetId].fromLastOccurrenceOf(File::getSeparatorString(), false, false);
            nMod->nSoundSet.set(soundSetId);
            nMod->nSoundSetName.set(soundSetName);
            nMod->setDirty(NostalgicSoundSet);
        }
        else
        {
            processor.loadSamples(BKLoadSoundfont, processor.globalSoundfont, cb->getSelectedItemIndex());
        }
    }
}

void MainViewController::bkButtonClicked (Button* b)
{
    String name = b->getName();
    if (b == &preferencesButton)
    {
        editor.showBKSettingsDialog(b);
    }
    if (b == &globalSoundSetButton)
    {
        BKItem* item = construction.getSelectedItems().getUnchecked(0);
        if (item->getType() == PreparationTypeDirect)
        {
            DirectPreparation::Ptr prep = processor.gallery->getDirectPreparation(item->getId());
            bool toggle = !prep->dUseGlobalSoundSet.value;
            prep->dUseGlobalSoundSet.set(toggle);
        }
        else if (item->getType() == PreparationTypeSynchronic)
        {
            SynchronicPreparation::Ptr prep = processor.gallery->getSynchronicPreparation(item->getId());
            bool toggle = !prep->sUseGlobalSoundSet.value;
            prep->sUseGlobalSoundSet.set(toggle);
        }
        else if (item->getType() == PreparationTypeNostalgic)
        {
            NostalgicPreparation::Ptr prep = processor.gallery->getNostalgicPreparation(item->getId());
            bool toggle = !prep->nUseGlobalSoundSet.value;
            prep->nUseGlobalSoundSet.set(toggle);
        }
        // Modifications
        else if (item->getType() == PreparationTypeDirectMod)
        {
            DirectModification::Ptr mod = processor.gallery->getDirectModification(item->getId());
            bool toggle = !mod->dUseGlobalSoundSet.value;
            mod->dUseGlobalSoundSet.set(toggle);
            mod->setDirty(DirectUseGlobalSoundSet);
        }
        else if (item->getType() == PreparationTypeSynchronicMod)
        {
            SynchronicModification::Ptr mod = processor.gallery->getSynchronicModification(item->getId());
            bool toggle = !mod->sUseGlobalSoundSet.value;
            mod->sUseGlobalSoundSet.set(toggle);
            mod->setDirty(SynchronicUseGlobalSoundSet);
        }
        else if (item->getType() == PreparationTypeNostalgicMod)
        {
            NostalgicModification::Ptr mod = processor.gallery->getNostalgicModification(item->getId());
            bool toggle = !mod->nUseGlobalSoundSet.value;
            mod->nUseGlobalSoundSet.set(toggle);
            mod->setDirty(NostalgicUseGlobalSoundSet);
        }
    }
    if (b == &sustainPedalButton)
    {
        processor.setSustainFromMenu(sustainPedalButton.getToggleState());
    }
    if (b == &equalizerButton) {
        editor.showGenSettings(1);
    }
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

	if (processor.areHotkeysEnabled())
	{
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
        else if (code == KeyPress::spaceKey)
        {
            processor.clearBitKlavier();
        }
        else if (code == 65) // A all
        {
            if (e.getModifiers().isCommandDown())   construction.selectAll();
            fillSampleCB();
            fillInstrumentCB();
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
        else if (code == 69) // E
        {
            if (processor.updateState->currentDisplay == DisplayKeymap)
            {
                Keymap::Ptr keymap = processor.gallery->getKeymap(processor.updateState->currentKeymapId);
                keymap->toggleMidiEdit();
                keymap->setHarMidiEdit(false);
                keymap->setHarArrayMidiEdit(false);
            }
        }
        else if (code == 73) // I
        {
            construction.removeConnectionsBetween();
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
        else if (code == 79) // O
        {
            construction.removeConnectionsTo();
        }
        else if (code == 80) // P piano
        {
            construction.addItem(PreparationTypePianoMap);
        }
        else if (code == 81) // Q comment
        {
            if (processor.updateState->currentDisplay == DisplayKeymap)
            {
                Keymap::Ptr keymap = processor.gallery->getKeymap(processor.updateState->currentKeymapId);
                keymap->setMidiEdit(false);
                keymap->setHarMidiEdit(false);
                keymap->toggleHarArrayMidiEdit();
            }
            else construction.addItem(PreparationTypeComment);
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
        
        else if (code == 85) // U
        {
            construction.connectAllSelected();
        }
        else if (code == 86) // V
        {
            if (e.getModifiers().isCommandDown())   construction.paste();
        }
        else if (code == 87) // W
        {
            if (processor.updateState->currentDisplay == DisplayKeymap)
            {
                Keymap::Ptr keymap = processor.gallery->getKeymap(processor.updateState->currentKeymapId);
                keymap->setMidiEdit(false);
                keymap->toggleHarMidiEdit();
                keymap->setHarArrayMidiEdit(false);
            }
        }
        else if (code == 88) // X
        {
            if (e.getModifiers().isCommandDown())   construction.cut();
        }
        else if (code == 90) // Z
        {
            if (processor.updateState->currentDisplay == DisplayNil)
            {
                if (e.getModifiers().isCommandDown())
                {
                    if (e.getModifiers().isShiftDown())
                        performRedo();
                    else
                        performUndo();
                }
            }
            else if (processor.updateState->currentDisplay == DisplayKeymap)
            {
                Keymap::Ptr keymap = processor.gallery->getKeymap(processor.updateState->currentKeymapId);
                keymap->toggleMidiEdit();
                keymap->setHarMidiEdit(false);
                keymap->setHarArrayMidiEdit(false);
            }
        }
	}

	if (code == 72) //CTRL + H toggles hotkeys on or off
	{
		if (e.getModifiers().isCommandDown()) processor.setHotkeysEnabled(!processor.areHotkeysEnabled());
	}

    return true;
}

void MainViewController::fillSampleCB()
{
    int idx = -1;
    if (construction.getNumSelected() == 1)
    {
        BKItem* item = construction.getSelectedItems().getUnchecked(0);
        if (item->getType() == PreparationTypeDirect)
        {
            DirectPreparation::Ptr prep = processor.gallery->getDirectPreparation(item->getId());
            if (prep != nullptr) idx = prep->getSoundSet();
        }
        else if (item->getType() == PreparationTypeSynchronic)
        {
            SynchronicPreparation::Ptr prep = processor.gallery->getSynchronicPreparation(item->getId());
            if (prep != nullptr) idx = prep->getSoundSet();
        }
        else if (item->getType() == PreparationTypeNostalgic)
        {
            NostalgicPreparation::Ptr prep = processor.gallery->getNostalgicPreparation(item->getId());
            if (prep != nullptr) idx = prep->getSoundSet();
        }
        // Modifications
        else if (item->getType() == PreparationTypeDirectMod)
        {
            DirectModification::Ptr mod = processor.gallery->getDirectModification(item->getId());
            if (mod != nullptr) idx = mod->getSoundSet();
        }
        else if (item->getType() == PreparationTypeSynchronicMod)
        {
            SynchronicModification::Ptr mod = processor.gallery->getSynchronicModification(item->getId());
            if (mod != nullptr) idx = mod->getSoundSet();
        }
        else if (item->getType() == PreparationTypeNostalgicMod)
        {
            NostalgicModification::Ptr mod = processor.gallery->getNostalgicModification(item->getId());
            if (mod != nullptr) idx = mod->getSoundSet();
        }
    }
    if (idx < 0) idx = processor.globalSoundSetId;
    
    String name;

    name = processor.loadedSoundSets[idx].fromLastOccurrenceOf(File::getSeparatorString(), false, true).upToFirstOccurrenceOf(".sf", false, true);
    
    sampleCB.clear(dontSendNotification);

    sampleCB.addItem("Piano (litest)", 1);
    sampleCB.addItem("Piano (lite)", 2);
    sampleCB.addItem("Piano (medium)", 3);
    sampleCB.addItem("Piano (heavy)", 4);
    
    sampleCB.addSeparator();
    
    for (int i = 0; i < sampleCB.getNumItems(); i++)
    {
        if (sampleCB.getItemText(i) == name)
        {
            sampleCB.setSelectedItemIndex(i, dontSendNotification);
        }
    }
    
    int id = 5;
    for (auto sf : processor.soundfontNames)
    {
		String sfname = sf.fromLastOccurrenceOf(File::getSeparatorString(), false, true)
        .upToFirstOccurrenceOf(".sf", false, true);

        sampleCB.addItem(sfname, id);
        
        if (sfname == name)
            sampleCB.setSelectedId(id, dontSendNotification);
        id++;
    }
    
    if (!processor.customSampleSetNames.isEmpty()) sampleCB.addSeparator();
    
    for (auto custom : processor.customSampleSetNames)
    {
        String customName = custom.fromLastOccurrenceOf(File::getSeparatorString(), false, true);
        
        sampleCB.addItem(customName, id);
        
        if (customName == name)
            sampleCB.setSelectedId(id, dontSendNotification);
        id++;
    }
//
//    sampleCB.addSeparator();
//
//    sampleCB.addItem("Import samples...", id);
}

void MainViewController::fillInstrumentCB()
{
    int idx = -1;
    if (construction.getNumSelected() == 1)
    {
        BKItem* item = construction.getSelectedItems().getUnchecked(0);
        if (item->getType() == PreparationTypeDirect)
        {
            DirectPreparation::Ptr prep = processor.gallery->getDirectPreparation(item->getId());
            if (prep != nullptr) idx = prep->getSoundSet();
        }
        else if (item->getType() == PreparationTypeSynchronic)
        {
            SynchronicPreparation::Ptr prep = processor.gallery->getSynchronicPreparation(item->getId());
            if (prep != nullptr) idx = prep->getSoundSet();
        }
        else if (item->getType() == PreparationTypeNostalgic)
        {
            NostalgicPreparation::Ptr prep = processor.gallery->getNostalgicPreparation(item->getId());
            if (prep != nullptr) idx = prep->getSoundSet();
        }
        // Modifications
        else if (item->getType() == PreparationTypeDirectMod)
        {
            DirectModification::Ptr mod = processor.gallery->getDirectModification(item->getId());
            if (mod != nullptr) idx = mod->getSoundSet();
        }
        else if (item->getType() == PreparationTypeSynchronicMod)
        {
            SynchronicModification::Ptr mod = processor.gallery->getSynchronicModification(item->getId());
            if (mod != nullptr) idx = mod->getSoundSet();
        }
        else if (item->getType() == PreparationTypeNostalgicMod)
        {
            NostalgicModification::Ptr mod = processor.gallery->getNostalgicModification(item->getId());
            if (mod != nullptr) idx = mod->getSoundSet();
        }
    }
    if (idx < 0) idx = processor.globalSoundSetId;
    
    int subsound = processor.loadedSoundSets[idx].getTrailingIntValue();
    
    instrumentCB.clear(dontSendNotification);
    
    // If theres only one instrument, dont bother showing name
    if (sampleCB.getSelectedItemIndex() < BKLoadSoundfont)
    {
        instrumentCB.setEnabled(false);
    }
    else
    {
        instrumentCB.setEnabled(true);
        
        int i = 1;
        if (processor.instrumentNames.contains(idx))
        {
            for (auto inst : processor.instrumentNames.getReference(idx))
            {
                if (inst == "") inst = "Instrument " + String(i);
                instrumentCB.addItem(inst, i++);
            }
            
            instrumentCB.setSelectedItemIndex(subsound, dontSendNotification);
        }
        else instrumentCB.setEnabled(false);
    }
    
}

void MainViewController::performUndo()
{
    String action = processor.undoGallery();
    if (action == String()) return;
    undoStatus.setText(action, dontSendNotification);
    undoStatusCountdown = MVC_REFRESH_RATE * 2;
}

void MainViewController::performRedo()
{
    String action = processor.redoGallery();
    if (action == String()) return;
    undoStatus.setText(action, dontSendNotification);
    undoStatusCountdown = MVC_REFRESH_RATE * 2;
}


void MainViewController::timerCallback()
{
    BKUpdateState::Ptr state = processor.updateState;
    
    if (undoStatusCountdown > 0)
    {
        undoStatus.setVisible(true);
        undoStatus.setAlpha(undoStatusCountdown / (float) MVC_REFRESH_RATE);
        undoStatusCountdown--;
    }
    else undoStatus.setVisible(false);
    
    fillInstrumentCB();
    
    // display active noteOns on main keyboard
    keyboardState.setKeymap(processor.getSourcedNotesOn());

    bool soundItemSelected = false;
    sampleCB.setAlpha(1.);
    instrumentCB.setAlpha(1.);
    globalSoundSetButton.setAlpha(1.);
    
    bool front = true;
#if JUCE_IOS
    front = display == DisplayConstruction &&
    processor.updateState->currentDisplay == DisplayNil;
#endif
    if (construction.getNumSelected() == 1 && front)
    {
        // set main keyboard to display active keys in selected keymap (if there is a selected keymap)
        BKItem* item = construction.getSelectedItems().getUnchecked(0);
        if (item->getType() == PreparationTypeKeymap && processor.updateState->currentDisplay != DisplayKeymap)
        {
            keyboardState.setKeymap(processor.gallery->getKeymap(item->getId())->getKeymap());
        }
        
        if (item->getType() == PreparationTypeDirect)
        {
            soundItemSelected = true;
            globalSoundSetButton.setVisible(true);
            DirectPreparation::Ptr prep = processor.gallery->getDirectPreparation(item->getId());
            if (prep != nullptr)
                globalSoundSetButton.setToggleState(prep->dUseGlobalSoundSet.value, dontSendNotification);
        }
        else if (item->getType() == PreparationTypeSynchronic)
        {
            soundItemSelected = true;
            globalSoundSetButton.setVisible(true);
            SynchronicPreparation::Ptr prep = processor.gallery->getSynchronicPreparation(item->getId());
            if (prep != nullptr)
                globalSoundSetButton.setToggleState(prep->sUseGlobalSoundSet.value, dontSendNotification);
        }
        else if (item->getType() == PreparationTypeNostalgic)
        {
            soundItemSelected = true;
            globalSoundSetButton.setVisible(true);
            NostalgicPreparation::Ptr prep = processor.gallery->getNostalgicPreparation(item->getId());
            if (prep != nullptr)
                globalSoundSetButton.setToggleState(prep->nUseGlobalSoundSet.value, dontSendNotification);
        }
        // Modifications
        else if (item->getType() == PreparationTypeDirectMod)
        {
            soundItemSelected = true;
            globalSoundSetButton.setVisible(true);
            DirectModification::Ptr mod = processor.gallery->getDirectModification(item->getId());
            if (mod != nullptr)
            {
                globalSoundSetButton.setToggleState(mod->dUseGlobalSoundSet.value, dontSendNotification);
                globalSoundSetButton.setAlpha(mod->getDirty(DirectUseGlobalSoundSet) ? 1. : gModAlpha);
                sampleCB.setAlpha(mod->getDirty(DirectSoundSet) ? 1. : gModAlpha);
                instrumentCB.setAlpha(mod->getDirty(DirectSoundSet) ? 1. : gModAlpha);
            }
        }
        else if (item->getType() == PreparationTypeSynchronicMod)
        {
            soundItemSelected = true;
            globalSoundSetButton.setVisible(true);
            SynchronicModification::Ptr mod = processor.gallery->getSynchronicModification(item->getId());
            if (mod != nullptr)
            {
                globalSoundSetButton.setToggleState(mod->sUseGlobalSoundSet.value, dontSendNotification);
                globalSoundSetButton.setAlpha(mod->getDirty(SynchronicUseGlobalSoundSet) ? 1. : gModAlpha);
                sampleCB.setAlpha(mod->getDirty(SynchronicSoundSet) ? 1. : gModAlpha);
                instrumentCB.setAlpha(mod->getDirty(SynchronicSoundSet) ? 1. : gModAlpha);
            }
        }
        else if (item->getType() == PreparationTypeNostalgicMod)
        {
            soundItemSelected = true;
            globalSoundSetButton.setVisible(true);
            NostalgicModification::Ptr mod = processor.gallery->getNostalgicModification(item->getId());
            if (mod != nullptr)
            {
                globalSoundSetButton.setToggleState(mod->nUseGlobalSoundSet.value, dontSendNotification);
                globalSoundSetButton.setAlpha(mod->getDirty(NostalgicUseGlobalSoundSet) ? 1. : gModAlpha);
                sampleCB.setAlpha(mod->getDirty(NostalgicSoundSet) ? 1. : gModAlpha);
                instrumentCB.setAlpha(mod->getDirty(NostalgicSoundSet) ? 1. : gModAlpha);
            }
        }
    }
    
    if (!soundItemSelected)
    {
        globalSoundSetButton.setVisible(false);
        globalSoundSetButton.setToggleState(true, dontSendNotification);
    }
    
#if JUCE_IOS
    if (display == DisplayConstruction &&
        processor.updateState->currentDisplay == DisplayNil)
    {
        sampleCB.setVisible(true);
        instrumentCB.setVisible(true);
    }
    else
    {
        sampleCB.setVisible(false);
        instrumentCB.setVisible(false);
    }
#endif
    
    keyboard->repaint();
    
    if (processor.loader.getNumJobs() > 0)
    {
        splash.setVisible(true);
        
        splash.setProgress(processor.progress / processor.loader.getNumJobs());
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
        
        processor.updateState->previousDisplay = overtop.getCurrentDisplay();
        BKPreparationDisplay prevDisplay = processor.updateState->previousDisplay;
        
        overtop.setCurrentDisplay(processor.updateState->currentDisplay);
        
        header.update();

        if (processor.updateState->currentDisplay != DisplayNil)
        {
            processor.updateState->editsMade = false;
        }
        else
        {
            if (processor.updateState->editsMade)
            {
                if (prevDisplay != DisplayNil)
                    processor.saveGalleryToHistory(cDisplayNames[prevDisplay] + " Edits");
            }
        }   
    }
    
    levelMeterComponentL->updateLevel(processor.getLevelL());
    //levelMeterComponentR->updateLevel(processor.getLevelL());
    
    
}
