/*
  ==============================================================================

    KeymapViewController.cpp
    Created: 9 Dec 2016 11:57:38am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "KeymapViewController.h"


#define SELECT_ID  7
#define DESELECT_ID 8
#define TOGGLE_ID 9

#define ID(IN) (IN*12)

//==============================================================================
KeymapViewController::KeymapViewController(BKAudioProcessor& p, BKItemGraph* theGraph):
BKViewController(p, theGraph, 1)
{
    setLookAndFeel(&buttonsAndMenusLAF);
    // buttonsAndMenusLAF.drawGroupComponentOutline();
    
    wrapperType = p.wrapperType;
    
    iconImageComponent.setImage(ImageCache::getFromMemory(BinaryData::keymap_icon_png, BinaryData::keymap_icon_pngSize));
    iconImageComponent.setImagePlacement(RectanglePlacement(juce::RectanglePlacement::stretchToFit));
    iconImageComponent.setAlpha(0.095);
    // addAndMakeVisible(iconImageComponent);
    
    selectCB.setName("Keymap");
    selectCB.addSeparator();
    selectCB.addListener(this);
    selectCB.addMyListener(this);
    selectCB.setSelectedItemIndex(0);
    selectCB.setTooltip("Select from available saved preparation settings");
    addAndMakeVisible(selectCB);
    
    midiInputSelectButton.setName("MidiInput");
    midiInputSelectButton.setButtonText("Select MIDI Inputs");
    midiInputSelectButton.setTooltip("Select from available MIDI input devices");
    midiInputSelectButton.addListener(this);
    
    if(wrapperType == juce::AudioPluginInstance::wrapperType_Standalone)
        addAndMakeVisible(midiInputSelectButton);
        
    targetsButton.setName("TargetsButton"); // remove?
    targetsButton.setButtonText("Targets");
    targetsButton.setTooltip("Select which parts of connected preparations to send key information");
    targetsButton.addListener(this);
    // addAndMakeVisible(targetsButton);
    
    clearButton.setName("ClearButton");
    clearButton.setButtonText("Clear");
    clearButton.setTooltip("Deselect all keys");
    clearButton.addListener(this);
    addAndMakeVisible(clearButton);
    
    keysButton.setName("KeysButton");
    keysButton.setButtonText("Keys");
    keysButton.setTooltip("A collection of preset notes to be selected or deselected");
    keysButton.addListener(this);
    addAndMakeVisible(keysButton);
    
    keysCB.setName("keysCB");
    keysCB.addListener(this);
    keysCB.setTooltip("Choose between 'select' or 'deselect' for batch operations (under Keys)");
    keysCB.addItem("Select", SELECT_ID);
    keysCB.addItem("Deselect", DESELECT_ID);
    keysCB.setSelectedId(SELECT_ID);
    
    addAndMakeVisible(keysCB);
    
    addAndMakeVisible(keymapTF);
    keymapTF.addListener(this);
    keymapTF.setName("KeymapMidi");
    keymapTF.setTooltip("Select or deselect all keys by individually clicking or click-dragging, or press 'edit all' to type or copy/paste MIDI notes to be selected in Keymap");
    keymapTF.setMultiLine(true);
    
    keyboardComponent = std::make_unique<BKKeymapKeyboardComponent> (keyboardState, BKKeymapKeyboardComponent::horizontalKeyboard);
    
    // Keyboard
    addAndMakeVisible(*keyboardComponent);
    
    keyboard = (BKKeymapKeyboardComponent*)keyboardComponent.get();
    keyboard->setScrollButtonsVisible(false);
    
#if JUCE_IOS
    minKey = 48; // 21
    maxKey = 72; // 108
    
    octaveSlider.setRange(0, 6, 1);
    octaveSlider.addListener(this);
    octaveSlider.setLookAndFeel(&laf);
    octaveSlider.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    octaveSlider.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    octaveSlider.setValue(3);
    
    addAndMakeVisible(octaveSlider);

#else
    minKey = 21; // 21
    maxKey = 108; // 108
#endif
    
    keyboard->setAvailableRange(minKey, maxKey);
    
    keyboard->setAllowDrag(true);
    keyboard->setOctaveForMiddleC(4);
    keyboardState.addListener(this);
    
    keyboardValsTextFieldOpen.setName("KSLIDERTXTEDITALLBUTTON");
    keyboardValsTextFieldOpen.addListener(this);
    keyboardValsTextFieldOpen.setButtonText("edit all");
    addAndMakeVisible(keyboardValsTextFieldOpen);
    keymapTF.setVisible(false);
    keymapTF.toBack();
    
    addAndMakeVisible(actionButton);
    actionButton.setButtonText("Action");
    actionButton.setTooltip("Create, duplicate, rename, delete, or reset current settings");
    actionButton.addListener(this);
    
    invertOnOffToggle.setButtonText ("invert note on/off");
//    buttonsAndMenusLAF.setToggleBoxTextToRightBool(false);
    invertOnOffToggle.setToggleState (false, dontSendNotification);
    invertOnOffToggle.setLookAndFeel(&buttonsAndMenusLAF2); // text to left
    invertOnOffToggle.setTooltip("Indicates whether to invert Note-On and Note-Off messages for this Keymap");
    invertOnOffToggle.addListener(this);
    addAndMakeVisible(&invertOnOffToggle, ALL);
    
    midiEditToggle.setButtonText ("midi edit");
    buttonsAndMenusLAF.setToggleBoxTextToRightBool(true);
    midiEditToggle.setToggleState (false, dontSendNotification);
    if (processor.wrapperType == juce::AudioPluginInstance::wrapperType_Standalone)
        midiEditToggle.setTooltip("Indicates whether MIDI input will edit this Keymap" + gMidiEditShortcut);
    else
        midiEditToggle.setTooltip("Indicates whether MIDI input will edit this Keymap");
    midiEditToggle.addListener(this);
    addAndMakeVisible(&midiEditToggle, ALL);
    
    // Target Control TBs
    targetControlTBs = OwnedArray<ToggleButton>();
    buttonsAndMenusLAF2.setToggleBoxTextToRightBool(false);
    //for (int i=TargetTypeDirect; i<=TargetTypeTuning; i++)
    for (int i=TargetTypeDirect; i<=TargetTypeBlendronicOpenCloseOutput; i++)
    {
        targetControlTBs.add(new ToggleButton()); // insert at the end of the array
        targetControlTBs.getLast()->setName(cKeymapTargetTypes[i]);
        targetControlTBs.getLast()->setButtonText(cKeymapTargetTypes[i]);
        targetControlTBs.getLast()->addListener(this);
        addAndMakeVisible(targetControlTBs.getLast(), ALL);
    }

    // border for Direct Target Toggles
    directTBGroup.setName("directGroup");
    directTBGroup.setText("Direct Targets");
    directTBGroup.setTextLabelPosition(Justification::centred);
    //directTBGroup.setTextLabelPosition(Justification::right);
    //addAndMakeVisible(directTBGroup);
    
    // border for Synchronic Target Toggles
    synchronicTBGroup.setName("synchronicGroup");
    synchronicTBGroup.setText("Synchronic Targets");
    synchronicTBGroup.setTextLabelPosition(Justification::centred);
    addAndMakeVisible(synchronicTBGroup);
    
    // border for Nostalgic Target Toggles
    nostalgicTBGroup.setName("nostalgicGroup");
    nostalgicTBGroup.setText("Nostalgic Targets");
    nostalgicTBGroup.setTextLabelPosition(Justification::centred);
    addAndMakeVisible(nostalgicTBGroup);
    
    // border for Blendronic Target Toggles
    blendronicTBGroup.setName("blendronicGroup");
    blendronicTBGroup.setText("Blendronic Targets");
    //blendronicTBGroup.setTextLabelPosition(Justification::right);
    blendronicTBGroup.setTextLabelPosition(Justification::centred);
    addAndMakeVisible(blendronicTBGroup);
    
    // border for Tuning Target Toggles
    tuningTBGroup.setName("tuningGroup");
    tuningTBGroup.setText("Tuning Targets");
    tuningTBGroup.setTextLabelPosition(Justification::centred);
    //addAndMakeVisible(tuningTBGroup);
    
    // border for Tempo Target Toggles
    tempoTBGroup.setName("tempoGroup");
    tempoTBGroup.setText("Tempo Targets");
    tempoTBGroup.setTextLabelPosition(Justification::centred);
    //addAndMakeVisible(tempoTBGroup);
    
    fillSelectCB(-1,-1);
    
    startTimer(100); // was 20

    update();
}

KeymapViewController::~KeymapViewController()
{
    keyboard = nullptr;
    setLookAndFeel(nullptr);
}

void KeymapViewController::reset(void)
{
    fillSelectCB(-1,-1);
    update();
}

void KeymapViewController::paint (Graphics& g)
{
    g.fillAll(Colours::black);
}

void KeymapViewController::resized()
{
    Rectangle<int> area (getLocalBounds());

    iconImageComponent.setBounds(area);
    area.reduce(10 * processor.paddingScalarX + 4, 10 * processor.paddingScalarY + 4);

    // float keyboardHeight = 100; // + 36 * processor.paddingScalarY;
    float keyboardHeight = 50 + 50 * processor.paddingScalarY;
    Rectangle<int> keyboardRow = area.removeFromBottom(keyboardHeight);
    float keyWidth = keyboardRow.getWidth() / round((maxKey - minKey) * 7./12 + 1); //num white keys
    keyboard->setKeyWidth(keyWidth);
    keyboard->setBlackNoteLengthProportion(0.6);
    keyboardRow.reduce(gXSpacing, 0);
    
#if JUCE_IOS
    float sliderHeight = 15;
    Rectangle<int> sliderArea = keyboardRow.removeFromTop(sliderHeight);
    
    octaveSlider.setBounds(sliderArea);
#endif
    
    keyboard->setBounds(keyboardRow);
    
#if JUCE_IOS
    keymapTF.setTopLeftPosition(hideOrShow.getX(), hideOrShow.getBottom() + gYSpacing);
    keymapTF.setSize(keyboardRow.getWidth() * 0.5, getBottom() - hideOrShow.getBottom() - 2 * gYSpacing);

#else
    keymapTF.setBounds(keyboardRow);
#endif
    
    area.removeFromBottom(gYSpacing);
    Rectangle<int> textButtonSlab = area.removeFromBottom(gComponentComboBoxHeight);
    textButtonSlab.removeFromLeft(gXSpacing);
    keyboardValsTextFieldOpen.setBounds(textButtonSlab.removeFromLeft(getWidth() * 0.15));
    
    keysCB.setBounds(textButtonSlab.removeFromLeft(keyboardValsTextFieldOpen.getWidth()));
    keysButton.setBounds(textButtonSlab.removeFromLeft(keysCB.getWidth()));
    midiEditToggle.setBounds(textButtonSlab.removeFromLeft(keysCB.getWidth()));
    textButtonSlab.removeFromRight(gXSpacing);
    clearButton.setBounds(textButtonSlab.removeFromRight(keysCB.getWidth()));

    midiEditToggle.toFront(false);
    
    Rectangle<int> leftColumn = area.removeFromLeft(area.getWidth() * 0.5);
    Rectangle<int> comboBoxSlice = leftColumn.removeFromTop(gComponentComboBoxHeight);
    
    comboBoxSlice.removeFromRight(4 + 2.*gPaddingConst * processor.paddingScalarX);
    comboBoxSlice.removeFromLeft(gXSpacing);
    hideOrShow.setBounds(comboBoxSlice.removeFromLeft(gComponentComboBoxHeight));
    comboBoxSlice.removeFromLeft(gXSpacing);
    selectCB.setBounds(comboBoxSlice.removeFromLeft(comboBoxSlice.getWidth() / 2.));
    
    actionButton.setBounds(selectCB.getRight()+gXSpacing,
                           selectCB.getY(),
                           selectCB.getWidth() * 0.5,
                           selectCB.getHeight());
    
    Rectangle<int> targetsSlice = area.removeFromTop(gComponentComboBoxHeight);
    targetsSlice.removeFromRight(gXSpacing);
    if(wrapperType == juce::AudioPluginInstance::wrapperType_Standalone)
    {
        midiInputSelectButton.setBounds(targetsSlice.removeFromRight(selectCB.getWidth()));
        targetsSlice.removeFromRight(gXSpacing);
    }
    
    invertOnOffToggle.setBounds(targetsSlice.removeFromRight(selectCB.getWidth()));
    invertOnOffToggle.toFront(false);
    
    // height of the box for the prep with the most targets (Synchronic)
    int maxTargetHeight =   (TargetTypeSynchronicRotate - TargetTypeSynchronic + 1) *
                            (gComponentToggleBoxHeight + gYSpacing) + 5 * gYSpacing;
    
#if JUCE_IOS
    leftColumn.removeFromTop((leftColumn.getHeight() - maxTargetHeight) * processor.paddingScalarY * 0.25);
    area.removeFromTop((area.getHeight() - maxTargetHeight) * processor.paddingScalarY * 0.25);
#else
    leftColumn.removeFromTop((leftColumn.getHeight() - maxTargetHeight) * processor.paddingScalarY * 0.5);
    area.removeFromTop((area.getHeight() - maxTargetHeight) * processor.paddingScalarY * 0.5);
#endif

    
    Rectangle<int> secondColumn = leftColumn.removeFromRight(leftColumn.getWidth() * 0.5);
    Rectangle<int> thirdColumn = area.removeFromLeft(area.getWidth() * 0.5);
    //area is now fourth column
    
  
    // Synchronic Targets
    leftColumn.removeFromTop(10 * gYSpacing * processor.paddingScalarY);
    Rectangle<int> synchronicBox1 = leftColumn.removeFromTop((TargetTypeSynchronicClear - TargetTypeSynchronic + 1) * (gComponentToggleBoxHeight + gYSpacing) + 5 * gYSpacing);
    
    Rectangle<int> synchronicGroup1(synchronicBox1);
    
    synchronicBox1.removeFromTop(4 * gYSpacing);
    synchronicBox1.removeFromLeft(gXSpacing);
    for (int i=TargetTypeSynchronic; i<=TargetTypeSynchronicClear; i++)
    {
        targetControlTBs[i]->setBounds(synchronicBox1.removeFromTop(gComponentToggleBoxHeight));
        targetControlTBs[i]->setLookAndFeel(&buttonsAndMenusLAF2);
        synchronicBox1.removeFromTop(gYSpacing);
    }
    
    secondColumn.removeFromTop(10 * gYSpacing * processor.paddingScalarY);
    Rectangle<int> synchronicBox2 = secondColumn.removeFromTop((TargetTypeSynchronicRotate - TargetTypeSynchronicPausePlay + 1) * (gComponentToggleBoxHeight + gYSpacing) + 5 * gYSpacing);
    Rectangle<int> synchronicGroup2 = synchronicBox2;
    //synchronicTBGroup.setBounds(synchronicBox2);
    synchronicBox2.removeFromTop(4 * gYSpacing);
    synchronicBox2.removeFromLeft(gXSpacing);
    for (int i=TargetTypeSynchronicPausePlay; i<=TargetTypeSynchronicRotate; i++)
    {
        targetControlTBs[i]->setBounds(synchronicBox2.removeFromTop(gComponentToggleBoxHeight));
        synchronicBox2.removeFromTop(gYSpacing);
    }
    
    
    synchronicTBGroup.setBounds(synchronicGroup1.getX(),
                                synchronicGroup1.getY(),
                                synchronicGroup2.getWidth() +
                                synchronicGroup1.getWidth(),
                                synchronicGroup1.getHeight());
     
                                
    
     
    // Nostalgic Targets
    thirdColumn.removeFromTop(10 * gYSpacing * processor.paddingScalarY);
    Rectangle<int> nostalgicBox = thirdColumn.removeFromTop((TargetTypeNostalgic - TargetTypeNostalgic + 1) * (gComponentToggleBoxHeight + gYSpacing) + 5 * gYSpacing);
    Rectangle<int> nostalgicGroup1 = nostalgicBox;
    //blendronicTBGroup.setBounds(nostalgicBox);
    nostalgicBox.removeFromTop(4 * gYSpacing);
    nostalgicBox.removeFromLeft(gXSpacing);
    for (int i=TargetTypeNostalgic; i<=TargetTypeNostalgic; i++)
    {
        targetControlTBs[i]->setBounds(nostalgicBox.removeFromTop(gComponentToggleBoxHeight));
        targetControlTBs[i]->setLookAndFeel(&buttonsAndMenusLAF2);
        nostalgicBox.removeFromTop(gYSpacing);
    }
    
    area.removeFromTop(10 * gYSpacing * processor.paddingScalarY);
    Rectangle<int> nostalgicBox2 = area.removeFromTop((TargetTypeNostalgicClear - TargetTypeNostalgicClear + 1) * (gComponentToggleBoxHeight + gYSpacing) + 5 * gYSpacing);
    Rectangle<int> nostalgicGroup2 = nostalgicBox2;
    //blendronicTBGroup.setBounds(nostalgicBox2);
    nostalgicBox2.removeFromTop(4 * gYSpacing);
    nostalgicBox2.removeFromLeft(gXSpacing);
    for (int i=TargetTypeNostalgicClear; i<=TargetTypeNostalgicClear; i++)
    {
        targetControlTBs[i]->setBounds(nostalgicBox2.removeFromTop(gComponentToggleBoxHeight));
        //targetControlTBs[i]->setLookAndFeel(&buttonsAndMenusLAF2);
        nostalgicBox2.removeFromTop(gYSpacing);
    }
    
    nostalgicTBGroup.setBounds(nostalgicGroup1.getX(),
    nostalgicGroup1.getY(),
    nostalgicGroup2.getWidth() +
    nostalgicGroup1.getWidth(),
    nostalgicGroup1.getHeight());
    
    // Blendronic Targets
    // thirdColumn.removeFromTop(10 * gYSpacing * processor.paddingScalarY);
#if JUCE_IOS
    thirdColumn.removeFromTop(gYSpacing * 1.5);
#endif
    thirdColumn.removeFromTop(gYSpacing * 2);
    Rectangle<int> blendronicBox = thirdColumn.removeFromTop((TargetTypeBlendronicClear - TargetTypeBlendronicPatternSync + 1) * (gComponentToggleBoxHeight + gYSpacing) + 5 * gYSpacing);
    Rectangle<int> blendronicGroup1 = blendronicBox;
    //blendronicTBGroup.setBounds(blendronicBox);
    blendronicBox.removeFromTop(4 * gYSpacing);
    blendronicBox.removeFromLeft(gXSpacing);
    for (int i=TargetTypeBlendronicPatternSync; i<=TargetTypeBlendronicClear; i++)
    {
        targetControlTBs[i]->setBounds(blendronicBox.removeFromTop(gComponentToggleBoxHeight));
        targetControlTBs[i]->setLookAndFeel(&buttonsAndMenusLAF2);
        blendronicBox.removeFromTop(gYSpacing);
    }
    
    //area.removeFromTop(10 * gYSpacing * processor.paddingScalarY);
#if JUCE_IOS
    area.removeFromTop(gYSpacing * 1.5);
#endif
    area.removeFromTop(gYSpacing * 2);
    Rectangle<int> blendronicBox2 = area.removeFromTop((TargetTypeBlendronicOpenCloseOutput - TargetTypeBlendronicPausePlay + 1) * (gComponentToggleBoxHeight + gYSpacing) + 5 * gYSpacing);
    Rectangle<int> blendronicGroup2 = blendronicBox2;
    //blendronicTBGroup.setBounds(blendronicBox2);
    blendronicBox2.removeFromTop(4 * gYSpacing);
    blendronicBox2.removeFromLeft(gXSpacing);
    for (int i=TargetTypeBlendronicPausePlay; i<=TargetTypeBlendronicOpenCloseOutput; i++)
    {
        targetControlTBs[i]->setBounds(blendronicBox2.removeFromTop(gComponentToggleBoxHeight));
        //targetControlTBs[i]->setLookAndFeel(&buttonsAndMenusLAF2);
        blendronicBox2.removeFromTop(gYSpacing);
    }
    
    blendronicTBGroup.setBounds(blendronicGroup1.getX(),
                                blendronicGroup1.getY(),
                                blendronicGroup2.getWidth() +
                                blendronicGroup1.getWidth(),
                                blendronicGroup1.getHeight());
}

int KeymapViewController::addKeymap(void)
{
    processor.gallery->add(PreparationTypeKeymap);
    
    return processor.gallery->getKeymaps().getLast()->getId();
}

int KeymapViewController::duplicateKeymap(void)
{
    processor.gallery->duplicate(PreparationTypeKeymap, processor.updateState->currentKeymapId);
    
    return processor.gallery->getKeymaps().getLast()->getId();
}

void KeymapViewController::deleteCurrent(void)
{
    int KeymapId = selectCB.getSelectedId();
    int index = selectCB.getSelectedItemIndex();
    
    if ((index == 0) && (selectCB.getItemId(index+1) == -1)) return;
    
    processor.gallery->remove(PreparationTypeKeymap, KeymapId);
    
    fillSelectCB(0, 0);
    
    int newId = 0;
    
    selectCB.setSelectedId(newId, dontSendNotification);
    
    processor.updateState->currentKeymapId = -1;
    setCurrentId(-1);
}

void KeymapViewController::setCurrentId(int Id)
{
    processor.updateState->currentKeymapId = Id;
    
    processor.updateState->idDidChange = true;
    
    update();
    
    fillSelectCB(lastId, Id);
    
    lastId = Id;
}

void KeymapViewController::actionButtonCallback(int action, KeymapViewController* vc)
{
    if (vc == nullptr)
    {
        PopupMenu::dismissAllActiveMenus();
        return;
    }
    
    BKAudioProcessor& processor = vc->processor;
    
    if (action == 1)
    {
        int Id = vc->addKeymap();
        vc->setCurrentId(Id);
    }
    else if (action == 2)
    {
        int Id = vc->duplicateKeymap();
        vc->setCurrentId(Id);
    }
    else if (action == 3)
    {
        vc->deleteCurrent();
    }
    else if (action == 5)
    {
        processor.clear(PreparationTypeKeymap, processor.updateState->currentKeymapId);
        vc->update();
    }
    else if (action == 6)
    {
        AlertWindow prompt("", "", AlertWindow::AlertIconType::QuestionIcon, vc);
        
        int Id = processor.updateState->currentKeymapId;
        Keymap::Ptr prep = processor.gallery->getKeymap(Id);
        
        prompt.addTextEditor("name", prep->getName());
        
        prompt.addButton("Ok", 1, KeyPress(KeyPress::returnKey));
        prompt.addButton("Cancel", 2, KeyPress(KeyPress::escapeKey));
        
        int result = prompt.runModalLoop();
        
        String name = prompt.getTextEditorContents("name");
        
        if (result == 1)
        {
            prep->setName(name);
            vc->fillSelectCB(Id, Id);
        }
        
        vc->update();
    }
    else if (action == 7)
    {
        AlertWindow prompt("", "", AlertWindow::AlertIconType::QuestionIcon);
        
        int Id = processor.updateState->currentKeymapId;
        Keymap::Ptr keymap = processor.gallery->getKeymap(Id);
        
        prompt.addTextEditor("name", keymap->getName());
        
        prompt.addButton("Ok", 1, KeyPress(KeyPress::returnKey));
        prompt.addButton("Cancel", 2, KeyPress(KeyPress::escapeKey));
        
        int result = prompt.runModalLoop();
        
        String name = prompt.getTextEditorContents("name");
        
        DBG("name: " + String(name));
        
        if (result == 1)
        {
            processor.exportPreparation(PreparationTypeKeymap, Id, name);
        }
    }
    else if (action >= 100)
    {
        int which = action - 100;
        processor.importPreparation(PreparationTypeKeymap, processor.updateState->currentKeymapId, which);
        vc->update();
    }
}


void KeymapViewController::bkComboBoxDidChange        (ComboBox* box)
{
    Keymap::Ptr keymap = processor.gallery->getKeymap(processor.updateState->currentKeymapId);
    
    String name = box->getName();
    int Id = box->getSelectedId();
    
    if (name == "Keymap")
    {
        if (Id == -1)
        {
            processor.gallery->add(PreparationTypeKeymap);
            
            Id = processor.gallery->getKeymaps().getLast()->getId();
        }
        
        processor.updateState->currentKeymapId = Id;
        
        processor.updateState->idDidChange = true;
        
        update();
        
        fillSelectCB(lastId, Id);
        
        lastId = Id;
    }
    else if (box == &keysCB)
    {
        if (Id == SELECT_ID)        selectType = true;
        else if (Id == DESELECT_ID) selectType = false;
    }
}

String pcs[12] = {"C","C#/Db","D","D#/Eb","E","F","F#/Gb","G","G#/Ab","A","A#/Bb","B",};

PopupMenu KeymapViewController::getPitchClassMenu(int offset)
{
    int Id;
    
    BKPopupMenu menu;
    
    for (int i = 0; i < 12; i++)
    {
        Id = offset + i;
        DBG("ID: " + String(Id));
        menu.addItem(Id, pcs[i]);
    }
    
    return std::move(menu);
}

PopupMenu KeymapViewController::getKeysMenu(void)
{
    BKPopupMenu menu;
    
    menu.addItem(ID(KeySetAll), "All");
    menu.addSubMenu("All...",  getPitchClassMenu((KeySet) ID(KeySetAllPC)));
    menu.addItem(ID(KeySetBlack), "Black");
    menu.addItem(ID(KeySetWhite), "White");
    menu.addItem(ID(KeySetOctatonicOne), "Octatonic 1");
    menu.addItem(ID(KeySetOctatonicTwo), "Octatonic 2");
    menu.addItem(ID(KeySetOctatonicThree), "Octatonic 3");
    
    menu.addSubMenu("Major Triad",  getPitchClassMenu((KeySet) ID(KeySetMajorTriad)));
    menu.addSubMenu("Minor Triad",  getPitchClassMenu((KeySet) ID(KeySetMinorTriad)));
    menu.addSubMenu("Major Seven",  getPitchClassMenu((KeySet) ID(KeySetMajorSeven)));
    menu.addSubMenu("Dom Seven",    getPitchClassMenu((KeySet) ID(KeySetDomSeven)));
    menu.addSubMenu("Minor Seven",  getPitchClassMenu((KeySet) ID(KeySetMinorSeven)));
    
    menu.addSubMenu("Major", getPitchClassMenu((KeySet) ID(KeySetMajor)));
    menu.addSubMenu("Natural Minor", getPitchClassMenu((KeySet) ID(KeySetNaturalMinor)));
    menu.addSubMenu("Harmonic Minor", getPitchClassMenu((KeySet) ID(KeySetHarmonicMinor)));
    
    return std::move(menu);
}

void KeymapViewController::midiInputSelectCallback(int result, KeymapViewController* vc)
{
    if (result <= 0) return;
    
    BKAudioProcessor& processor = vc->processor;
    
    Keymap::Ptr keymap = processor.gallery->getKeymap(processor.updateState->currentKeymapId);
    
    if (result == 1)
    {
        keymap->setDefaultSelected(!keymap->isDefaultSelected());
    }
    else if (result == 2)
    {
        keymap->setOnscreenSelected(!keymap->isOnscreenSelected());
    }
    else
    {
        MidiDeviceInfo device = processor.getMidiInputDevices()[result-3];
        if (keymap->getMidiInputIdentifiers().contains(device.identifier))
        {
            keymap->removeMidiInputSource(device);
        }
        else keymap->addMidiInputSource(device);
    }

    vc->getMidiInputSelectMenu().showMenuAsync(PopupMenu::Options().withTargetComponent(vc->midiInputSelectButton), ModalCallbackFunction::forComponent(midiInputSelectCallback, vc));
}

void KeymapViewController::targetsMenuCallback(int result, KeymapViewController* vc)
{
    if (vc == nullptr)
    {
        PopupMenu::dismissAllActiveMenus();
        return;
    }
    
    if (result <= 0) return;
    
    BKAudioProcessor& processor = vc->processor;
    
    Keymap::Ptr keymap = processor.gallery->getKeymap(processor.updateState->currentKeymapId);
    keymap->toggleTarget((KeymapTargetType) (result - 1));
    
    vc->getTargetsMenu().showMenuAsync(PopupMenu::Options().withTargetComponent(vc->targetsButton), ModalCallbackFunction::forComponent(targetsMenuCallback, vc));
    
    DBG("KeymapViewController::targetsMenuCallback " + String(result));
}

void KeymapViewController::keysMenuCallback(int result, KeymapViewController* vc)
{
    if (vc == nullptr)
    {
        PopupMenu::dismissAllActiveMenus();
        return;
    }
    
    BKAudioProcessor& processor = vc->processor;
    
    // get old keys to send to update
    Keymap::Ptr keymap = processor.gallery->getKeymap(processor.updateState->currentKeymapId);

    {
        int set = result/ 12;
        int pc = result % 12;
        
        DBG("set: " + String(set) + " pc: " + String(pc));
        
        keymap->setKeys((KeySet)set, vc->selectType, (PitchClass)pc);
    }
    
    BKKeymapKeyboardComponent* keyboard =  (BKKeymapKeyboardComponent*)(vc->keyboardComponent.get());
    
    keyboard->setKeysInKeymap(keymap->keys());
}

PopupMenu KeymapViewController::getTargetsMenu()
{
    updateKeymapTargets();
    
    BKPopupMenu menu;
    
    Keymap::Ptr keymap = processor.gallery->getKeymap(processor.updateState->currentKeymapId);
    Array<KeymapTargetState> targetStates = keymap->getTargetStates();
    for (int type = 0; type < TargetTypeNil; ++type)
    {
        if (targetStates[type] == TargetStateEnabled)
        {
            menu.addItem(PopupMenu::Item(cKeymapTargetTypes[type]).setID(type+1).setTicked(true));
        }
        else if (targetStates[type] == TargetStateDisabled)
        {
            menu.addItem(PopupMenu::Item(cKeymapTargetTypes[type]).setID(type+1));
        }
    }
    if (menu.getNumItems() == 0) menu.addItem(-1, "No targets available", false);
    return std::move(menu);
}

void KeymapViewController::fillSelectCB(int last, int current)
{
    selectCB.clear(dontSendNotification);
    
    for (auto prep : processor.gallery->getKeymaps())
    {
        int Id = prep->getId();
    
        if (Id == -1) continue;
        
        String name = prep->getName();
        
        if (name != String())  selectCB.addItem(name, Id);
        
        selectCB.setItemEnabled(Id, true);
        if (processor.currentPiano->isActive(PreparationTypeKeymap, Id))
            selectCB.setItemEnabled(Id, false);
    }
    
    if (last != 0)      selectCB.setItemEnabled(last, true);
    if (current != 0)   selectCB.setItemEnabled(current, false);
    
    int selectedId = processor.updateState->currentKeymapId;
    
    selectCB.setSelectedId(selectedId, NotificationType::dontSendNotification);
    
    selectCB.setItemEnabled(selectedId, false);

    lastId = selectedId;
}

PopupMenu KeymapViewController::getMidiInputSelectMenu()
{
    BKPopupMenu menu;
    
    Keymap::Ptr keymap = processor.gallery->getKeymap(processor.updateState->currentKeymapId);
    
    int id = 1;
    if (keymap->isDefaultSelected())
    {
        menu.addItem(PopupMenu::Item("Default MIDI Input").setID(id).setTicked(true));
    }
    else menu.addItem(PopupMenu::Item("Default MIDI Input").setID(id));
    
    id++;
    if (keymap->isOnscreenSelected())
    {
        menu.addItem(PopupMenu::Item("Onscreen Keyboard").setID(id).setTicked(true));
    }
    else menu.addItem(PopupMenu::Item("Onscreen Keyboard").setID(id));
    
    for (auto device : processor.getMidiInputDevices())
    {
        if (keymap->getMidiInputIdentifiers().contains(device.identifier))
            menu.addItem(PopupMenu::Item(device.name).setID(++id).setTicked(true));
        else menu.addItem(PopupMenu::Item(device.name).setID(++id));
    }
    return std::move(menu);
}

void KeymapViewController::bkButtonClicked (Button* b)
{
    if (b == &hideOrShow)
    {
        Keymap::Ptr keymap = processor.gallery->getKeymap(processor.updateState->currentKeymapId);
        keymap->setMidiEdit(false);
        processor.updateState->setCurrentDisplay(DisplayNil);
        
    }
    else if(b->getName() == keyboardValsTextFieldOpen.getName())
    {
#if JUCE_IOS
        hasBigOne = true;
        iWantTheBigOne(&keymapTF, "keymap");
#else
        keymapTF.setVisible(true);
        keymapTF.toFront(true);
        
        focusLostByEscapeKey = false;
#endif
    }
    else if (b == &actionButton)
    {
        bool single = processor.gallery->getKeymaps().size() == 2;
        getModOptionMenu(PreparationTypeKeymap, single).showMenuAsync (PopupMenu::Options().withTargetComponent (&actionButton), ModalCallbackFunction::forComponent (actionButtonCallback, this) );
    }
    else if (b == &midiInputSelectButton)
    {
        getMidiInputSelectMenu().showMenuAsync(PopupMenu::Options().withTargetComponent(&midiInputSelectButton), ModalCallbackFunction::forComponent(midiInputSelectCallback, this));
    }
    else if (b == &targetsButton)
    {
        getTargetsMenu().showMenuAsync(PopupMenu::Options().withTargetComponent(&targetsButton), ModalCallbackFunction::forComponent(targetsMenuCallback, this));
    }
    else if (b == &invertOnOffToggle)
    {
        Keymap::Ptr keymap = processor.gallery->getKeymap(processor.updateState->currentKeymapId);
        keymap->setInverted(invertOnOffToggle.getToggleState());
    }
    else if (b == &midiEditToggle)
    {
        Keymap::Ptr keymap = processor.gallery->getKeymap(processor.updateState->currentKeymapId);
        keymap->setMidiEdit(midiEditToggle.getToggleState());
    }
    else if (b == &keysButton)
    {
        getKeysMenu().showMenuAsync(PopupMenu::Options().withTargetComponent(&keysButton), ModalCallbackFunction::forComponent(keysMenuCallback, this));
    }
    else if (b == &clearButton)
    {
        Keymap::Ptr keymap = processor.gallery->getKeymap(processor.updateState->currentKeymapId);
        keymap->setKeys(KeySetAll, false);
        
        BKKeymapKeyboardComponent* keyboard =  (BKKeymapKeyboardComponent*)(keyboardComponent.get());

        keyboard->setKeysInKeymap(keymap->keys());
    }
    else
    {
        Keymap::Ptr keymap = processor.gallery->getKeymap(processor.updateState->currentKeymapId);
        
        for (int i=0; i<targetControlTBs.size(); i++)
        {
            if (b == targetControlTBs[i])
            {
                //state ? TargetStateDisabled : TargetStateEnabled
                keymap->setTarget((KeymapTargetType) i,  (KeymapTargetState) b->getToggleState());
                // keymap->setTarget((KeymapTargetType) i,  b->getToggleState() ? TargetStateDisabled : TargetStateEnabled);
                DBG("Keymap toggle change: " + (String)cKeymapTargetTypes[i] + " " + String((int)b->getToggleState()));
            }
        }
    }
}


void KeymapViewController::BKEditableComboBoxChanged(String name, BKEditableComboBox* cb)
{
    Keymap::Ptr thisKeymap = processor.gallery->getKeymap(processor.updateState->currentKeymapId);
    
    thisKeymap->setName(name);
}



void KeymapViewController::bkTextFieldDidChange(TextEditor& tf)
{
    String name = tf.getName();
    
    if (name == "KeymapMidi")
    {
        keymapUpdated(tf);
    }
    else
    {
        DBG("Unregistered text field entered input.");
    }
}

#if JUCE_IOS
void KeymapViewController::iWantTheBigOne(TextEditor* tf, String name)
{
    hideOrShow.setAlwaysOnTop(false);
    bigOne.display(tf, name, getBounds());
}
#endif

void KeymapViewController::keymapUpdated(TextEditor& tf)
{
    String text = tf.getText();
    Array<int> keys = keymapStringToIntArray(text);
    
    keymapTF.setText(intArrayToString(keys));
    
    // get old keys to send to update
    Array<int> oldKeys = processor.gallery->getKeymap(processor.updateState->currentKeymapId)->keys();
    
    processor.gallery->setKeymap(processor.updateState->currentKeymapId, keys);
    
    BKKeymapKeyboardComponent* keyboard =  (BKKeymapKeyboardComponent*)keyboardComponent.get();
    
    keyboard->setKeysInKeymap(keys);
    
    keymapTF.setVisible(false);
    keymapTF.toBack();

}

void KeymapViewController::textEditorFocusLost(TextEditor& tf)
{
#if !JUCE_IOS
    if(!focusLostByEscapeKey)
    {
        keymapUpdated(tf);
    }
#endif
    
}

void KeymapViewController::textEditorEscapeKeyPressed (TextEditor& textEditor)
{
    focusLostByEscapeKey = true;
    keymapTF.setVisible(false);
    keymapTF.toBack();
    unfocusAllComponents();
}

void KeymapViewController::textEditorTextChanged(TextEditor& tf)
{
    if (hasBigOne)
    {
        hasBigOne = false;
        bkTextFieldDidChange(tf);
    }
}

void KeymapViewController::textEditorReturnKeyPressed(TextEditor& textEditor)
{
    if(textEditor.getName() == keymapTF.getName())
    {
        keymapUpdated(textEditor);
    }
}
       

void KeymapViewController::update(void)
{
    if (processor.updateState->currentKeymapId < 0) return;
    
    Keymap::Ptr km = processor.gallery->getKeymap(processor.updateState->currentKeymapId);
    
    if (km != nullptr)
    {
        selectCB.setSelectedId(processor.updateState->currentKeymapId, dontSendNotification);
        invertOnOffToggle.setToggleState(km->isInverted(), dontSendNotification);
        midiEditToggle.setToggleState(km->getMidiEdit(), dontSendNotification);
        keymapTF.setText( intArrayToString(km->keys()));
        BKKeymapKeyboardComponent* keyboard =  (BKKeymapKeyboardComponent*)keyboardComponent.get();
        keyboard->setKeysInKeymap(km->keys());
        
        for (int i=TargetTypeDirect; i<=TargetTypeBlendronicOpenCloseOutput; i++)
        {
            targetControlTBs[i]->setToggleState(km->getTargetState((KeymapTargetType) i) == TargetStateEnabled, dontSendNotification);
        }
    }
    
    hideUnconnectedTargets();
    
    km->print();
    
}

void KeymapViewController::bkMessageReceived (const String& message)
{
}


void KeymapViewController::handleKeymapNoteOn (BKKeymapKeyboardState* source, int midiNoteNumber)
{
    
}

void KeymapViewController::handleKeymapNoteOff (BKKeymapKeyboardState* source, int midiNoteNumber)
{
    
}

void KeymapViewController::handleKeymapNoteToggled (BKKeymapKeyboardState* source, int midiNoteNumber)
{
    Keymap::Ptr thisKeymap = processor.gallery->getKeymap(processor.updateState->currentKeymapId);
    
    Array<int> oldKeys = thisKeymap->keys();
    
    thisKeymap->toggleNote(midiNoteNumber);
    
    update();
    
    BKKeymapKeyboardComponent* keyboard =  (BKKeymapKeyboardComponent*)keyboardComponent.get();
    
    keyboard->setKeysInKeymap(thisKeymap->keys());
    
    processor.currentPiano->configure();
}


#if JUCE_IOS
void KeymapViewController::sliderValueChanged     (Slider* slider)
{
    if (slider == &octaveSlider)
    {
        int octave = (int) octaveSlider.getValue();
        
        if (octave == 0)    keyboard->setAvailableRange(21, 45);
        else                keyboard->setAvailableRange(12+octave*12, 36+octave*12);
    }
}
#endif

void KeymapViewController::updateKeymapTargets()
{
    Keymap::Ptr km = processor.gallery->getKeymap(processor.updateState->currentKeymapId);
    BKItem::Ptr kmItem = theGraph->get(PreparationTypeKeymap, km->getId());
    
    if(kmItem != nullptr) {
        for (int type = 0; type < BKPreparationTypeNil; ++type)
        {
            if (kmItem->getConnectionsOfType((BKPreparationType) type).size() == 0)
            {
                km->removeTargetsOfType((BKPreparationType) type);
            }
        }
    }
}

void KeymapViewController::hideUnconnectedTargets()
{
    Keymap::Ptr km = processor.gallery->getKeymap(processor.updateState->currentKeymapId);
    BKItem::Ptr kmItem = theGraph->get(PreparationTypeKeymap, km->getId());
    if(kmItem != nullptr) {
        if (kmItem->getConnectionsOfType(PreparationTypeSynchronic).size() == 0)
        {
            for (int i=TargetTypeSynchronic; i<=TargetTypeSynchronicRotate; i++)
            {
                targetControlTBs[i]->setAlpha(gDim);
                targetControlTBs[i]->setEnabled(false);
            }
            
            synchronicTBGroup.setAlpha(gDim);
        }
        else
        {
            for (int i=TargetTypeSynchronic; i<=TargetTypeSynchronicRotate; i++)
            {
                targetControlTBs[i]->setAlpha(gBright);
                targetControlTBs[i]->setEnabled(true);
            }
            
            synchronicTBGroup.setAlpha(gMedium);
        }
       
        if (kmItem->getConnectionsOfType(PreparationTypeBlendronic).size() == 0)
        {
            for (int i=TargetTypeBlendronicPatternSync; i<=TargetTypeBlendronicOpenCloseOutput; i++)
            {
                targetControlTBs[i]->setAlpha(gDim);
                targetControlTBs[i]->setEnabled(false);
            }
            
            blendronicTBGroup.setAlpha(gDim);
        }
        else
        {
            for (int i=TargetTypeBlendronicPatternSync; i<=TargetTypeBlendronicOpenCloseOutput; i++)
            {
                targetControlTBs[i]->setAlpha(gBright);
                targetControlTBs[i]->setEnabled(true);
            }
            
            blendronicTBGroup.setAlpha(gMedium);
        }
        
        if (kmItem->getConnectionsOfType(PreparationTypeNostalgic).size() == 0)
        {
            for (int i=TargetTypeNostalgic; i<=TargetTypeNostalgicClear; i++)
            {
                targetControlTBs[i]->setAlpha(gDim);
                targetControlTBs[i]->setEnabled(false);
            }
            
            nostalgicTBGroup.setAlpha(gDim);
        }
        else
        {
            for (int i=TargetTypeNostalgic; i<=TargetTypeNostalgicClear; i++)
            {
                targetControlTBs[i]->setAlpha(gBright);
                targetControlTBs[i]->setEnabled(true);
            }
            
            nostalgicTBGroup.setAlpha(gMedium);
        }
    }
}

void KeymapViewController::timerCallback(){
    
    Keymap::Ptr km = processor.gallery->getKeymap(processor.updateState->currentKeymapId);
    
    if (km->getMidiEdit())
    {
        BKKeymapKeyboardComponent* keyboard =  (BKKeymapKeyboardComponent*)(keyboardComponent.get());
        keyboard->setKeysInKeymap(km->keys());
    }

    //updateKeymapTargets(); // needed?
    /*
    BKItem::Ptr kmItem = theGraph->get(PreparationTypeKeymap, km->getId());
    if(kmItem != nullptr) {
        if (kmItem->getConnectionsOfType(PreparationTypeSynchronic).size() == 0)
        {
            for (int i=TargetTypeSynchronic; i<=TargetTypeSynchronicPausePlay; i++)
            {
                targetControlTBs[i]->setAlpha(gDim);
                targetControlTBs[i]->setEnabled(false);
            }
            
            synchronicTBGroup.setAlpha(gDim);
        }
        else
        {
            for (int i=TargetTypeSynchronic; i<=TargetTypeSynchronicPausePlay; i++)
            {
                targetControlTBs[i]->setAlpha(gBright);
                targetControlTBs[i]->setEnabled(true);
            }
            
            synchronicTBGroup.setAlpha(gBright);
        }
        
        if (kmItem->getConnectionsOfType(PreparationTypeBlendronic).size() == 0)
        {
            for (int i=TargetTypeBlendronicSync; i<=TargetTypeBlendronicClose; i++)
            {
                targetControlTBs[i]->setAlpha(gDim);
                targetControlTBs[i]->setEnabled(false);
            }
            
            blendronicTBGroup.setAlpha(gDim);
        }
        else
        {
            for (int i=TargetTypeBlendronicSync; i<=TargetTypeBlendronicClose; i++)
            {
                targetControlTBs[i]->setAlpha(gBright);
                targetControlTBs[i]->setEnabled(true);
            }
            
            blendronicTBGroup.setAlpha(gBright);
        }
    }
*/
}


