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
BKViewController(p, theGraph, 3)
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
    
    keyboard = std::make_unique<BKKeymapKeyboardComponent> (keyboardState, BKKeymapKeyboardComponent::horizontalKeyboard);
    
    // Keyboard
    addAndMakeVisible(*keyboard);
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

    harKeyboardLabel.setText("Key to harmonize", dontSendNotification);
    harKeyboardLabel.setBorderSize(BorderSize<int>(1, 0, 1, 5));
    harKeyboardLabel.setJustificationType(Justification::bottomLeft);
    harKeyboardLabel.setTooltip("Select a key to create a harmonization for, or press 'edit full mapping' for an editable text representation of the full keyboard mapping. The text representation reflect the input transposition but not the output transposition.");
    addAndMakeVisible(&harKeyboardLabel, ALL);
    
    harKeyboard = std::make_unique<BKKeymapKeyboardComponent>(harKeyboardState, BKKeymapKeyboardComponent::horizontalKeyboard);
    // Harmonizer Keyboard
    addAndMakeVisible(*harKeyboard);
    harKeyboard->setScrollButtonsVisible(false);

#if JUCE_IOS

    harOctaveSlider.setRange(0, 6, 1);
    harOctaveSlider.addListener(this);
    harOctaveSlider.setLookAndFeel(&laf);
    harOctaveSlider.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    harOctaveSlider.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    harOctaveSlider.setValue(3);

    addAndMakeVisible(harOctaveSlider);
#endif

    harKeyboard->setAvailableRange(minKey, maxKey);

    harKeyboard->setAllowDrag(true);
    harKeyboard->setOctaveForMiddleC(4);
    harKeyboardState.addListener(this);

    addAndMakeVisible(harAllKeymapTF);
    harAllKeymapTF.addListener(this);
    harAllKeymapTF.setName("HarmonizerAllKeymapMidi");
    harAllKeymapTF.setMultiLine(true);
    harKeyboardAllValsTextFieldOpen.setName("HKSLIDERTXTEDITALLBUTTON");
    harKeyboardAllValsTextFieldOpen.addListener(this);
    harKeyboardAllValsTextFieldOpen.setButtonText("edit full mapping");
    addAndMakeVisible(harKeyboardAllValsTextFieldOpen);
    harAllKeymapTF.setVisible(false);
    harAllKeymapTF.toBack();

    //Harmonizer Array stuff

    addAndMakeVisible(harArrayKeymapTF);
    harArrayKeymapTF.addListener(this);
    harArrayKeymapTF.setName("HarmonizerArrayKeymapMidi");
//    harArrayKeymapTF.setTooltip("Select or deselect all keys by individually clicking or click-dragging, or press 'edit key mapping' to type or copy/paste MIDI notes to be selected in Keymap");
    harArrayKeymapTF.setMultiLine(true);
    
        
    
    harArrayKeyboardLabel.setText("Key harmonization", dontSendNotification);
    harArrayKeyboardLabel.setBorderSize(BorderSize<int>(1, 0, 1, 5));
    harArrayKeyboardLabel.setJustificationType(Justification::bottomLeft);
    harArrayKeyboardLabel.setTooltip("Select the harmonization for the key to harmonize, or press 'edit key mapping' for an editable text representation of the harmonization. The key highlighting and text representation here reflect the input transposition but not the output transposition.");
    addAndMakeVisible(&harArrayKeyboardLabel, ALL);
    
    harArrayKeyboard = std::make_unique<BKKeymapKeyboardComponent>(harArrayKeyboardState, BKKeymapKeyboardComponent::horizontalKeyboard);
    // Harmonizer Array Keyboard
    addAndMakeVisible(*harArrayKeyboard);
    harArrayKeyboard->setScrollButtonsVisible(false);

#if JUCE_IOS

    harArrayOctaveSlider.setRange(0, 6, 1);
    harArrayOctaveSlider.addListener(this);
    harArrayOctaveSlider.setLookAndFeel(&laf);
    harArrayOctaveSlider.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    harArrayOctaveSlider.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    harArrayOctaveSlider.setValue(3);

    addAndMakeVisible(harArrayOctaveSlider);
#endif

    harArrayKeyboard->setAvailableRange(minKey, maxKey);

    harArrayKeyboard->setAllowDrag(true);
    harArrayKeyboard->setOctaveForMiddleC(4);
    harArrayKeyboardState.addListener(this);

    harArrayKeyboardValsTextFieldOpen.setName("HAKSLIDERTXTEDITALLBUTTON");
    harArrayKeyboardValsTextFieldOpen.addListener(this);
    harArrayKeyboardValsTextFieldOpen.setButtonText("edit key mapping");
    addAndMakeVisible(harArrayKeyboardValsTextFieldOpen);
    harArrayKeymapTF.setVisible(false);
    harArrayKeymapTF.toBack();

    /*
    enableHarmonizerToggle.setButtonText("Play harmonizer notes");
    enableHarmonizerToggle.setLookAndFeel(&buttonsAndMenusLAF2); // text to left
    enableHarmonizerToggle.setTooltip("Indicates whether notes outlined by the harmonizer will play");
    enableHarmonizerToggle.addListener(this);
    addAndMakeVisible(&enableHarmonizerToggle, ALL);
    */

    harmonizerMenuButton.setName("HarmonizerCommands");
    harmonizerMenuButton.setButtonText("Harmonizer Commands");
    harmonizerMenuButton.setTooltip("Select ways to add to or clear the harmonizer keyboard");
    harmonizerMenuButton.addListener(this);
    addAndMakeVisible(harmonizerMenuButton);

    harPreTranspositionSlider = std::make_unique<BKSingleSlider>("Transpose input", "Transpose input", -12, 12, 0, 1);
    harPreTranspositionSlider->setToolTipString("Transpose all input into this Keymap before harmonization");
    harPreTranspositionSlider->addMyListener(this);
    addAndMakeVisible(*harPreTranspositionSlider);
    
    harPostTranspositionSlider = std::make_unique<BKSingleSlider>("Transpose output", "Transpose output", -12, 12, 0, 1);
    harPostTranspositionSlider->setToolTipString("Transpose all output of this Keymap after harmonization");
    harPostTranspositionSlider->addMyListener(this);
    addAndMakeVisible(*harPostTranspositionSlider);
    
    addAndMakeVisible(actionButton);
    actionButton.setButtonText("Action");
    actionButton.setTooltip("Create, duplicate, rename, delete, or reset current settings");
    actionButton.addListener(this);
    
    if (processor.wrapperType == juce::AudioPluginInstance::wrapperType_Standalone)
        midiEditToggle.setButtonText ("midi edit (E)");
    else midiEditToggle.setButtonText ("midi edit");
    buttonsAndMenusLAF.setToggleBoxTextToRightBool(true);
    midiEditToggle.setToggleState (false, dontSendNotification);
    midiEditToggle.setTooltip("Toggle whether MIDI input will edit this Keymap");
    midiEditToggle.addListener(this);
    addAndMakeVisible(&midiEditToggle, ALL);

    if (processor.wrapperType == juce::AudioPluginInstance::wrapperType_Standalone)
        harMidiEditToggle.setButtonText("midi edit (W)");
    else midiEditToggle.setButtonText ("midi edit");
    buttonsAndMenusLAF.setToggleBoxTextToRightBool(true);
    harMidiEditToggle.setToggleState(false, dontSendNotification);
    harMidiEditToggle.setTooltip("Toggle whether MIDI input will edit this Keymap");
    harMidiEditToggle.addListener(this);
    addAndMakeVisible(&harMidiEditToggle, ALL);

    if (processor.wrapperType == juce::AudioPluginInstance::wrapperType_Standalone)
        harArrayMidiEditToggle.setButtonText("midi edit (Q)");
    else midiEditToggle.setButtonText ("midi edit");
    buttonsAndMenusLAF.setToggleBoxTextToRightBool(true);
    harArrayMidiEditToggle.setToggleState(false, dontSendNotification);
    harArrayMidiEditToggle.setTooltip("Toggle whether MIDI input will edit this Keymap");
    harArrayMidiEditToggle.addListener(this);
    addAndMakeVisible(&harArrayMidiEditToggle, ALL);
    
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

    invertOnOffToggle.setButtonText ("Invert Note On/Off");
    invertOnOffToggle.setToggleState (false, dontSendNotification);
    invertOnOffToggle.setLookAndFeel(&buttonsAndMenusLAF); // text to left
    invertOnOffToggle.setTooltip("Toggle whether to invert Note-On and Note-Off messages for this Keymap");
    invertOnOffToggle.addListener(this);
    addAndMakeVisible(&invertOnOffToggle, ALL);

    endKeystrokesToggle.setButtonText("Trigger All Notes Off");
    endKeystrokesToggle.setToggleState(false, dontSendNotification);
    endKeystrokesToggle.setLookAndFeel(&buttonsAndMenusLAF); // text to right?
    endKeystrokesToggle.setTooltip("Toggle whether to end all notes whenever a key in this Keymap is played. The Keymap does not need to be connected to anything for this feature. Inverting note on/offs will cause this to activate on note offs.");
    endKeystrokesToggle.addListener(this);
    addAndMakeVisible(&endKeystrokesToggle, ALL);

    ignoreSustainToggle.setButtonText("Ignore Sustain Pedal");
    ignoreSustainToggle.setToggleState(false, dontSendNotification);
    ignoreSustainToggle.setLookAndFeel(&buttonsAndMenusLAF);
    ignoreSustainToggle.setTooltip("Toggle whether to ignore the sustain pedal for this keymap");
    ignoreSustainToggle.addListener(this);
    addAndMakeVisible(&ignoreSustainToggle, ALL);
    
    sustainPedalKeysToggle.setButtonText("Use as Sustain Pedal");
    sustainPedalKeysToggle.setToggleState(false, dontSendNotification);
    sustainPedalKeysToggle.setLookAndFeel(&buttonsAndMenusLAF);
    sustainPedalKeysToggle.setTooltip("Toggle whether the keys of this Keymap should function as a sustain pedals");
    sustainPedalKeysToggle.addListener(this);
    addAndMakeVisible(&sustainPedalKeysToggle, ALL);

    currentTab = 0;
    //displayTab(currentTab);

    fillSelectCB(-1,-1);
    
    startTimer(100); // was 20

    update();
}

KeymapViewController::~KeymapViewController()
{
    setLookAndFeel(nullptr);
    //transpositionLaf 
    //need to set some null pointer stuff - transposition?
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
    displayShared();
    displayTab(currentTab);
    repaint();
}

void KeymapViewController::displayTab(int tab)
{
    currentTab = tab;
    invisible();
    displayShared();

    int x0 = leftArrow.getRight() + gXSpacing;
//    int y0 = hideOrShow.getBottom() + gYSpacing;
//    int right = rightArrow.getX() - gXSpacing;
//    int width = right - x0;
//    int height = getHeight() - y0;

    Rectangle<int> area(getLocalBounds());

    if (tab == 0)
    {
        iconImageComponent.setBounds(area);
        area.reduce(10 * processor.paddingScalarX + 4, 10 * processor.paddingScalarY + 4);
        
        Rectangle<int> leftColumn = area.removeFromLeft(area.getWidth() * 0.5);
        
        // height of the box for the prep with the most targets (Synchronic)
        int maxTargetHeight = (TargetTypeSynchronicRotate - TargetTypeSynchronic + 1) *
        (gComponentToggleBoxHeight + gYSpacing) + 5 * gYSpacing;
        
#if JUCE_IOS
        leftColumn.removeFromTop((leftColumn.getHeight() - maxTargetHeight) * processor.paddingScalarY * 0.25);
        area.removeFromTop((area.getHeight() - maxTargetHeight) * processor.paddingScalarY * 0.25);
#else
        leftColumn.removeFromTop((leftColumn.getHeight() - maxTargetHeight) * processor.paddingScalarY * 0.5);
        area.removeFromTop((area.getHeight() - maxTargetHeight) * processor.paddingScalarY * 0.5);
#endif
        
        leftColumn.removeFromLeft(leftArrow.getWidth());
        area.removeFromRight(rightArrow.getWidth());
        
        Rectangle<int> secondColumn = leftColumn.removeFromRight(leftColumn.getWidth() * 0.5);
        Rectangle<int> thirdColumn = area.removeFromLeft(area.getWidth() * 0.5);
        //area is now fourth column
        
        
        // Synchronic Targets
        //        leftColumn.removeFromTop(10 * gYSpacing * processor.paddingScalarY);
        Rectangle<int> synchronicBox1 = leftColumn.removeFromTop((TargetTypeSynchronicClear - TargetTypeSynchronic + 1) * (gComponentToggleBoxHeight + gYSpacing) + 5 * gYSpacing);
        
        Rectangle<int> synchronicGroup1(synchronicBox1);
        
        synchronicBox1.removeFromTop(4 * gYSpacing);
        synchronicBox1.removeFromLeft(gXSpacing);
        for (int i = TargetTypeSynchronic; i <= TargetTypeSynchronicClear; i++)
        {
            targetControlTBs[i]->setBounds(synchronicBox1.removeFromTop(gComponentToggleBoxHeight));
            targetControlTBs[i]->setLookAndFeel(&buttonsAndMenusLAF2);
            targetControlTBs[i]->setVisible(true);
            synchronicBox1.removeFromTop(gYSpacing);
        }
        
        //        secondColumn.removeFromTop(10 * gYSpacing * processor.paddingScalarY);
        Rectangle<int> synchronicBox2 = secondColumn.removeFromTop((TargetTypeSynchronicRotate - TargetTypeSynchronicPausePlay + 1) * (gComponentToggleBoxHeight + gYSpacing) + 5 * gYSpacing);
        Rectangle<int> synchronicGroup2 = synchronicBox2;
        //synchronicTBGroup.setBounds(synchronicBox2);
        synchronicBox2.removeFromTop(4 * gYSpacing);
        synchronicBox2.removeFromLeft(gXSpacing);
        for (int i = TargetTypeSynchronicPausePlay; i <= TargetTypeSynchronicRotate; i++)
        {
            targetControlTBs[i]->setBounds(synchronicBox2.removeFromTop(gComponentToggleBoxHeight));
            targetControlTBs[i]->setVisible(true);
            synchronicBox2.removeFromTop(gYSpacing);
        }
        
        
        synchronicTBGroup.setBounds(synchronicGroup1.getX(),
                                    synchronicGroup1.getY(),
                                    synchronicGroup2.getWidth() +
                                    synchronicGroup1.getWidth(),
                                    synchronicGroup1.getHeight());
        synchronicTBGroup.setVisible(true);
        
        
        
        // Nostalgic Targets
        //        thirdColumn.removeFromTop(10 * gYSpacing * processor.paddingScalarY);
        Rectangle<int> nostalgicBox = thirdColumn.removeFromTop((TargetTypeNostalgic - TargetTypeNostalgic + 1) * (gComponentToggleBoxHeight + gYSpacing) + 5 * gYSpacing);
        Rectangle<int> nostalgicGroup1 = nostalgicBox;
        //blendronicTBGroup.setBounds(nostalgicBox);
        nostalgicBox.removeFromTop(4 * gYSpacing);
        nostalgicBox.removeFromLeft(gXSpacing);
        for (int i = TargetTypeNostalgic; i <= TargetTypeNostalgic; i++)
        {
            targetControlTBs[i]->setBounds(nostalgicBox.removeFromTop(gComponentToggleBoxHeight));
            targetControlTBs[i]->setLookAndFeel(&buttonsAndMenusLAF2);
            targetControlTBs[i]->setVisible(true);
            nostalgicBox.removeFromTop(gYSpacing);
        }
        
        //        area.removeFromTop(10 * gYSpacing * processor.paddingScalarY);
        Rectangle<int> nostalgicBox2 = area.removeFromTop((TargetTypeNostalgicClear - TargetTypeNostalgicClear + 1) * (gComponentToggleBoxHeight + gYSpacing) + 5 * gYSpacing);
        Rectangle<int> nostalgicGroup2 = nostalgicBox2;
        //blendronicTBGroup.setBounds(nostalgicBox2);
        nostalgicBox2.removeFromTop(4 * gYSpacing);
        nostalgicBox2.removeFromLeft(gXSpacing);
        for (int i = TargetTypeNostalgicClear; i <= TargetTypeNostalgicClear; i++)
        {
            targetControlTBs[i]->setBounds(nostalgicBox2.removeFromTop(gComponentToggleBoxHeight));
            //targetControlTBs[i]->setLookAndFeel(&buttonsAndMenusLAF2);
            targetControlTBs[i]->setVisible(true);
            nostalgicBox2.removeFromTop(gYSpacing);
        }
        
        nostalgicTBGroup.setBounds(nostalgicGroup1.getX(),
                                   nostalgicGroup1.getY(),
                                   nostalgicGroup2.getWidth() +
                                   nostalgicGroup1.getWidth(),
                                   nostalgicGroup1.getHeight());
        nostalgicTBGroup.setVisible(true);
        
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
        for (int i = TargetTypeBlendronicPatternSync; i <= TargetTypeBlendronicClear; i++)
        {
            targetControlTBs[i]->setBounds(blendronicBox.removeFromTop(gComponentToggleBoxHeight));
            targetControlTBs[i]->setLookAndFeel(&buttonsAndMenusLAF2);
            targetControlTBs[i]->setVisible(true);
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
        for (int i = TargetTypeBlendronicPausePlay; i <= TargetTypeBlendronicOpenCloseOutput; i++)
        {
            targetControlTBs[i]->setBounds(blendronicBox2.removeFromTop(gComponentToggleBoxHeight));
            //targetControlTBs[i]->setLookAndFeel(&buttonsAndMenusLAF2);
            targetControlTBs[i]->setVisible(true);
            blendronicBox2.removeFromTop(gYSpacing);
        }
        
        blendronicTBGroup.setBounds(blendronicGroup1.getX(),
                                    blendronicGroup1.getY(),
                                    blendronicGroup2.getWidth() +
                                    blendronicGroup1.getWidth(),
                                    blendronicGroup1.getHeight());
        blendronicTBGroup.setVisible(true);
    }
	else if (tab == 1)
    {
        area.reduce(x0 + 10 * processor.paddingScalarX + 4, 10 * processor.paddingScalarY + 4);
        area.removeFromTop(gComponentComboBoxHeight);
        
        float harKeyboardHeight = 80 * processor.paddingScalarY;
        
        area.removeFromBottom(50 + 30 * processor.paddingScalarY + gYSpacing * 6);
        area.removeFromBottom(gComponentComboBoxHeight);
        area.removeFromBottom(gComponentComboBoxHeight * processor.paddingScalarY + gYSpacing);
        
        Rectangle<int> harKeyboardSlice = area.removeFromBottom(harKeyboardHeight);
        area.removeFromBottom(gYSpacing);
        Rectangle<int> sliderSlice = area.removeFromBottom(gComponentComboBoxHeight);
        area.removeFromBottom(gYSpacing);
        Rectangle<int> labelSlice = area.removeFromBottom(gComponentComboBoxHeight * processor.paddingScalarY);
        
        float keyWidth = harKeyboardSlice.getWidth() / round((maxKey - minKey) * 7. / 12 + 1); //num white keys
        
        harKeyboard->setKeyWidth(keyWidth);
        harKeyboard->setBlackNoteLengthProportion(0.6);
        
#if JUCE_IOS
        float sliderHeight = 15;
        Rectangle<int> sliderArea = harKeyboardSlice.removeFromTop(sliderHeight);
        
        harOctaveSlider.setBounds(sliderArea);
        harOctaveSlider.setVisible(true);
#endif
        
        harKeyboard->setBounds(harKeyboardSlice);
        harKeyboard->setVisible(true);
        
#if JUCE_IOS
        
#else
        harAllKeymapTF.setBounds(harKeyboardSlice);
        harAllKeymapTF.setVisible(true);
#endif
        harKeyboardAllValsTextFieldOpen.setBounds(sliderSlice.removeFromLeft(getWidth() * 0.15f));
        harKeyboardAllValsTextFieldOpen.setVisible(true);
        
        harMidiEditToggle.setBounds(sliderSlice.removeFromLeft(harKeyboardAllValsTextFieldOpen.getWidth()));
        harMidiEditToggle.setVisible(true);
        
        harPreTranspositionSlider->setBounds(sliderSlice.withTop(sliderSlice.getY() - sliderSlice.getHeight() * 0.5f * processor.paddingScalarY));
        harPreTranspositionSlider->setVisible(true);
        
        harKeyboardLabel.setBounds(labelSlice.removeFromLeft(harKeyboardAllValsTextFieldOpen.getWidth()*2));
        harKeyboardLabel.setVisible(true);
        
        //harmonizer array keyboard
        
        Rectangle<int> textButtonSlice = area.removeFromBottom(gComponentComboBoxHeight - gYSpacing * 2).expanded(0, gYSpacing);
        harmonizerMenuButton.setBounds(textButtonSlice.removeFromRight(keysCB.getWidth() * 2));
        harmonizerMenuButton.setVisible(true);
        
        //        area = area.removeFromTop(harKeyboardHeight + gComponentComboBoxHeight + gComponentComboBoxHeight * processor.paddingScalarY + gYSpacing + gYSpacing);
        
        area.removeFromBottom(gYSpacing);
        harKeyboardSlice = area.removeFromBottom(harKeyboardHeight);
        area.removeFromBottom(gYSpacing);
        sliderSlice = area.removeFromBottom(gComponentComboBoxHeight);
        area.removeFromBottom(gYSpacing);
        labelSlice = area.removeFromBottom(gComponentComboBoxHeight * processor.paddingScalarY);
        
        harArrayKeyboard->setKeyWidth(keyWidth);
        harArrayKeyboard->setBlackNoteLengthProportion(0.6);
        
#if JUCE_IOS
        sliderArea = harKeyboardSlice.removeFromTop(sliderHeight);
        
        harArrayOctaveSlider.setBounds(sliderArea);
        harArrayOctaveSlider.setVisible(true);
#endif
        
        harArrayKeyboard->setBounds(harKeyboardSlice);
        harArrayKeyboard->setVisible(true);
        
#if JUCE_IOS
        harArrayKeymapTF.setTopLeftPosition(hideOrShow.getX(), hideOrShow.getBottom() + gYSpacing);
        harArrayKeymapTF.setSize(harKeyboardSlice.getWidth() * 0.5, getBottom() - hideOrShow.getBottom() - 2 * gYSpacing);
        
#else
        harArrayKeymapTF.setBounds(harKeyboardSlice);
        harArrayKeymapTF.setVisible(true);
        
        
#endif
        
        harArrayKeyboardValsTextFieldOpen.setBounds(sliderSlice.removeFromLeft(getWidth() * 0.15f));
        harArrayKeyboardValsTextFieldOpen.setVisible(true);
        
        harArrayMidiEditToggle.setBounds(sliderSlice.removeFromLeft(harArrayKeyboardValsTextFieldOpen.getWidth()));
        harArrayMidiEditToggle.setVisible(true);
        
        harPostTranspositionSlider->setBounds(sliderSlice.withTop(sliderSlice.getY() - sliderSlice.getHeight() * 0.5f * processor.paddingScalarY));
        harPostTranspositionSlider->setVisible(true);
        
        harArrayKeyboardLabel.setBounds(labelSlice.removeFromLeft(harArrayKeyboardValsTextFieldOpen.getWidth()*2));
        harArrayKeyboardLabel.setVisible(true);
        
        //        Keymap::Ptr thisKeymap = processor.gallery->getKeymap(processor.updateState->currentKeymapId);
        //        int tempHarKey = thisKeymap->getHarKey();
        //
        //        harKeyboard->setKeysInKeymap(Array<int>({ tempHarKey }));
        //
        //        Array<int> harmonizationArray = (thisKeymap->getHarmonizationForKey(tempHarKey));
        //        harArrayKeyboard->setKeysInKeymap(harmonizationArray);
        update(); //update() does all this ^
    }
    else if (tab == 2)
    {
        area.reduce(x0 + 10 * processor.paddingScalarX + 4, 10 * processor.paddingScalarY + 4);
        area.removeFromTop(gComponentComboBoxHeight);
        
        area.reduce(area.getWidth() * 0.2f, area.getHeight() * 0.2f * processor.paddingScalarY);
        
        endKeystrokesToggle.setBounds(area.removeFromTop(gComponentComboBoxHeight + 2 * gYSpacing));
        endKeystrokesToggle.setVisible(true);

        invertOnOffToggle.setBounds(area.removeFromTop(gComponentComboBoxHeight + 2 * gYSpacing));
        invertOnOffToggle.setVisible(true);

        ignoreSustainToggle.setBounds(area.removeFromTop(gComponentComboBoxHeight + 2 * gYSpacing));
        ignoreSustainToggle.setVisible(true);
        
        sustainPedalKeysToggle.setBounds(area.removeFromTop(gComponentComboBoxHeight + 2 * gYSpacing));
        sustainPedalKeysToggle.setVisible(true);
    }
}

void KeymapViewController::displayShared()
{
    Rectangle<int> area(getLocalBounds());

    iconImageComponent.setBounds(area);
    area.reduce(10 * processor.paddingScalarX + 4, 10 * processor.paddingScalarY + 4);
    
    
    Rectangle<int> topArea (area);
    //area.removeFromTop(selectCB.getHeight() + 50 * processor.paddingScalarY + 4 + gYSpacing);
    Rectangle<int> leftColumn = topArea.removeFromLeft(topArea.getWidth() * 0.5);
    Rectangle<int> comboBoxSlice = leftColumn.removeFromTop(gComponentComboBoxHeight);
    
    comboBoxSlice.removeFromRight(4 + 2. * gPaddingConst * processor.paddingScalarX);
    comboBoxSlice.removeFromLeft(gXSpacing);
    hideOrShow.setBounds(comboBoxSlice.removeFromLeft(gComponentComboBoxHeight));
    comboBoxSlice.removeFromLeft(gXSpacing);
    selectCB.setBounds(comboBoxSlice.removeFromLeft(comboBoxSlice.getWidth() / 2));
    selectCB.setVisible(true);
    
    actionButton.setBounds(selectCB.getRight() + gXSpacing,
                           selectCB.getY(),
                           selectCB.getWidth() * 0.5,
                           selectCB.getHeight());
    actionButton.setVisible(true);
    
    Rectangle<int> midiInputSlice = topArea.removeFromTop(gComponentComboBoxHeight);
    midiInputSlice.removeFromRight(gXSpacing);
    if (wrapperType == juce::AudioPluginInstance::wrapperType_Standalone)
    {
        midiInputSelectButton.setBounds(midiInputSlice.removeFromRight(selectCB.getWidth()));
        midiInputSelectButton.setVisible(true);
    }
    
    

    // float keyboardHeight = 100; // + 36 * processor.paddingScalarY;
    float keyboardHeight = 50 + 50 * processor.paddingScalarY;
    Rectangle<int> keyboardRow = area.removeFromBottom(keyboardHeight);
    float keyWidth = keyboardRow.getWidth() / round((maxKey - minKey) * 7. / 12 + 1); //num white keys
    keyboard->setKeyWidth(keyWidth);
    keyboard->setBlackNoteLengthProportion(0.6);
    keyboardRow.reduce(gXSpacing, 0);

#if JUCE_IOS
    float sliderHeight = 15;
    Rectangle<int> sliderArea = keyboardRow.removeFromTop(sliderHeight);

    octaveSlider.setBounds(sliderArea);
#endif

    keyboard->setBounds(keyboardRow);
    keyboard->setVisible(true);

#if JUCE_IOS
    keymapTF.setTopLeftPosition(hideOrShow.getX(), hideOrShow.getBottom() + gYSpacing);
    keymapTF.setSize(keyboardRow.getWidth() * 0.5, getBottom() - hideOrShow.getBottom() - 2 * gYSpacing);

#else
    keymapTF.setBounds(keyboardRow);
    keymapTF.setVisible(true);
#endif

    area.removeFromBottom(gYSpacing);
    Rectangle<int> textButtonSlab = area.removeFromBottom(gComponentComboBoxHeight);
    textButtonSlab.removeFromLeft(gXSpacing);
    keyboardValsTextFieldOpen.setBounds(textButtonSlab.removeFromLeft(getWidth() * 0.15));
    keyboardValsTextFieldOpen.setVisible(true);

    keysCB.setBounds(textButtonSlab.removeFromLeft(keyboardValsTextFieldOpen.getWidth()));
    keysCB.setVisible(true);
    keysButton.setBounds(textButtonSlab.removeFromLeft(keysCB.getWidth()));
    keysButton.setVisible(true);
    midiEditToggle.setBounds(textButtonSlab.removeFromLeft(keysCB.getWidth()));
    midiEditToggle.setVisible(true);
    textButtonSlab.removeFromRight(gXSpacing);
    clearButton.setBounds(textButtonSlab.removeFromRight(keysCB.getWidth()));
    clearButton.setVisible(true);

    midiEditToggle.toFront(false);

    leftArrow.setBounds(0, getHeight() * 0.4, 50, 50);
    rightArrow.setBounds(getRight() - 50, getHeight() * 0.4, 50, 50);

}

void KeymapViewController::invisible()
{
    keymapSelectL.setVisible(false);
    selectCB.setVisible(false);
    invertOnOffToggle.setVisible(false);
    midiEditToggle.setVisible(false);
    harMidiEditToggle.setVisible(false);
    harArrayMidiEditToggle.setVisible(false);
    keymapL.setVisible(false);
    keymapTF.setVisible(false);
    
    keyboard->setVisible(false);
    keyboardValsTextFieldOpen.setVisible(false);

    harKeyboardLabel.setVisible(false);
    harKeyboard->setVisible(false);
    harKeyboardAllValsTextFieldOpen.setVisible(false);
    harAllKeymapTF.setVisible(false);

    harArrayKeyboardLabel.setVisible(false);
    harArrayKeymapTF.setVisible(false);
    harArrayKeyboard->setVisible(false);
    harArrayKeyboardValsTextFieldOpen.setVisible(false);
    
#if JUCE_IOS
    harOctaveSlider.setVisible(false);
    harArrayOctaveSlider.setVisible(false);
#endif

    //enableHarmonizerToggle.setVisible(false);

    midiInputSelectButton.setVisible(false);
    keysButton.setVisible(false);
    clearButton.setVisible(false);
    keysCB.setVisible(false);

    for (ToggleButton* button : targetControlTBs)
    {
        button->setVisible(false);
    }
    directTBGroup.setVisible(false);
    synchronicTBGroup.setVisible(false);
    nostalgicTBGroup.setVisible(false);
    blendronicTBGroup.setVisible(false);
    tuningTBGroup.setVisible(false);
    tempoTBGroup.setVisible(false);

    actionButton.setVisible(false);

    endKeystrokesToggle.setVisible(false);
    ignoreSustainToggle.setVisible(false);
    sustainPedalKeysToggle.setVisible(false);

    harmonizerMenuButton.setVisible(false);

    harPreTranspositionSlider->setVisible(false);
    harPostTranspositionSlider->setVisible(false);
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
    
    selectCB.setSelectedItemIndex(0, dontSendNotification);
    int newId = selectCB.getSelectedId();
    
    setCurrentId(newId);
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
        processor.saveGalleryToHistory("New Keymap Preparation");
    }
    else if (action == 2)
    {
        int Id = vc->duplicateKeymap();
        vc->setCurrentId(Id);
        processor.saveGalleryToHistory("Duplicate Keymap Preparation");
    }
    else if (action == 3)
    {
        vc->deleteCurrent();
        processor.saveGalleryToHistory("Delete Keymap Preparation");
    }
    else if (action == 5)
    {
        processor.clear(PreparationTypeKeymap, processor.updateState->currentKeymapId);
        vc->update();
        processor.saveGalleryToHistory("Clear Keymap Preparation");
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
            processor.saveGalleryToHistory("Rename Keymap Preparation");
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
        processor.saveGalleryToHistory("Import Keymap Preparation");
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
    
    processor.updateState->editsMade = true;
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

// Passing in harmonization for the current key in case we have some options that
// depend on that and display more detail
PopupMenu KeymapViewController::getHarmonizerMenu(Array<int> keyHarmonization)
{
    BKPopupMenu menu;

    menu.addItem(1, "Clear");
    menu.addItem(2, "Default");
    menu.addItem(3, "Copy Mapping to All");
    menu.addItem(4, "Copy Pattern to All");
    menu.addItem(5, "Copy Mapping to Octaves");
    menu.addItem(6, "Copy Pattern to Octaves");
    menu.addItem(7, "Mirror");

    return std::move(menu);
}

void KeymapViewController::harmonizerMenuCallback(int result, KeymapViewController* vc)
{
    BKAudioProcessor& processor = vc->processor;

    Keymap::Ptr keymap = processor.gallery->getKeymap(processor.updateState->currentKeymapId);
    int harKey = keymap->getHarKey();
    
    if (result <= 0) return;
    else if (result == 1)
    {
        keymap->clearHarmonizations();
    }
    else if (result == 2)
    {
        keymap->defaultHarmonizations();
    }
    else if (result == 3)
    {
        keymap->copyKeyMappingToAll(harKey);
    }
    else if (result == 4)
    {
        keymap->copyKeyPatternToAll(harKey);
    }
    else if (result == 5)
    {
        keymap->copyKeyMappingToOctaves(harKey);
    }
    else if (result == 6)
    {
        keymap->copyKeyPatternToOctaves(harKey);
    }
    else if (result == 7)
    {
        keymap->mirrorKey(harKey);
    }

    vc->update();
    
    processor.updateState->editsMade = true;
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
    
    processor.updateState->editsMade = true;
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
    
    vc->keyboard->setKeysInKeymap(keymap->keys());
    
    processor.updateState->editsMade = true;
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
        menu.addItem(PopupMenu::Item(cMidiInputDefaultDisplay).setID(id).setTicked(true));
    }
    else menu.addItem(PopupMenu::Item(cMidiInputDefaultDisplay).setID(id));
    
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
        keymap->setHarMidiEdit(false);
        keymap->setHarArrayMidiEdit(false);
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
    else if (b->getName() == harKeyboardAllValsTextFieldOpen.getName())
    {
#if JUCE_IOS
        hasBigOne = true;
        iWantTheBigOne(&harAllKeymapTF, "harAllKeymap");
#else
        harAllKeymapTF.setVisible(true);
        harAllKeymapTF.toFront(true);

        focusLostByEscapeKey = false;
#endif
    }
    else if (b->getName() == harArrayKeyboardValsTextFieldOpen.getName())
    {
#if JUCE_IOS
        hasBigOne = true;
        iWantTheBigOne(&harArrayKeymapTF, "harArrayKeymap");
#else
        harArrayKeymapTF.setVisible(true);
        harArrayKeymapTF.toFront(true);

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
    else if (b == &harmonizerMenuButton)
    {
        Keymap::Ptr keymap = processor.gallery->getKeymap(processor.updateState->currentKeymapId);
        getHarmonizerMenu(keymap->getHarmonizationForKey(keymap->getHarKey(), false, false)).showMenuAsync(PopupMenu::Options().withTargetComponent(&harmonizerMenuButton), ModalCallbackFunction::forComponent(harmonizerMenuCallback, this));
        update();
    }
    else if (b == &invertOnOffToggle)
    {
        Keymap::Ptr keymap = processor.gallery->getKeymap(processor.updateState->currentKeymapId);
        keymap->setInverted(invertOnOffToggle.getToggleState());
        processor.updateState->editsMade = true;
    }
    /*
    else if (b == &enableHarmonizerToggle)
    {
        Keymap::Ptr keymap = processor.gallery->getKeymap(processor.updateState->currentKeymapId);
        keymap->setHarmonizerEnabled(enableHarmonizerToggle.getToggleState());
    }
    */
    else if (b == &midiEditToggle)
    {
        harMidiEditToggle.setToggleState(false, dontSendNotification);
        harArrayMidiEditToggle.setToggleState(false, dontSendNotification);

        Keymap::Ptr keymap = processor.gallery->getKeymap(processor.updateState->currentKeymapId);
        keymap->setMidiEdit(midiEditToggle.getToggleState());
        keymap->setHarMidiEdit(false);
        keymap->setHarArrayMidiEdit(false);
    }
    else if (b == &harMidiEditToggle)
    {
        midiEditToggle.setToggleState(false, dontSendNotification);
        harArrayMidiEditToggle.setToggleState(false, dontSendNotification);

        Keymap::Ptr keymap = processor.gallery->getKeymap(processor.updateState->currentKeymapId);
        keymap->setHarMidiEdit(harMidiEditToggle.getToggleState());
        keymap->setMidiEdit(false);
        keymap->setHarArrayMidiEdit(false);
    }
    else if (b == &harArrayMidiEditToggle)
    {
        midiEditToggle.setToggleState(false, dontSendNotification);
        harMidiEditToggle.setToggleState(false, dontSendNotification);

        Keymap::Ptr keymap = processor.gallery->getKeymap(processor.updateState->currentKeymapId);
        keymap->setHarArrayMidiEdit(harArrayMidiEditToggle.getToggleState());
        keymap->setHarMidiEdit(false);
        keymap->setMidiEdit(false);
    }
    else if (b == &keysButton)
    {
        getKeysMenu().showMenuAsync(PopupMenu::Options().withTargetComponent(&keysButton), ModalCallbackFunction::forComponent(keysMenuCallback, this));
    }
    else if (b == &clearButton)
    {
        Keymap::Ptr keymap = processor.gallery->getKeymap(processor.updateState->currentKeymapId);
        keymap->setKeys(KeySetAll, false);
        keyboard->setKeysInKeymap(keymap->keys());
    }
    else if (b == &endKeystrokesToggle)
    {
        Keymap::Ptr keymap = processor.gallery->getKeymap(processor.updateState->currentKeymapId);
        keymap->setAllNotesOff(endKeystrokesToggle.getToggleState());
        processor.updateState->editsMade = true;
    }
    else if (b == &ignoreSustainToggle)
    {
        Keymap::Ptr keymap = processor.gallery->getKeymap(processor.updateState->currentKeymapId);
        keymap->setIgnoreSustain(ignoreSustainToggle.getToggleState());
        processor.updateState->editsMade = true;
    }
    else if (b == &sustainPedalKeysToggle)
    {
        Keymap::Ptr keymap = processor.gallery->getKeymap(processor.updateState->currentKeymapId);
        keymap->setSustainPedalKeys(sustainPedalKeysToggle.getToggleState());
        processor.updateState->editsMade = true;
    }
    else if (b == &rightArrow)
    {
        arrowPressed(RightArrow);

        DBG("currentTab: " + String(currentTab));

        displayTab(currentTab);
    }
    else if (b == &leftArrow)
    {
        arrowPressed(LeftArrow);

        DBG("currentTab: " + String(currentTab));

        displayTab(currentTab);
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
        processor.updateState->editsMade = true;
    }
}


void KeymapViewController::BKEditableComboBoxChanged(String name, BKEditableComboBox* cb)
{
    Keymap::Ptr thisKeymap = processor.gallery->getKeymap(processor.updateState->currentKeymapId);
    
    thisKeymap->setName(name);
    
    processor.updateState->editsMade = true;
}

void KeymapViewController::bkTextFieldDidChange(TextEditor& tf)
{
    String name = tf.getName();

    if (name == "KeymapMidi" || name == "HarmonizerArrayKeymapMidi" || name == "HarmonizerAllKeymapMidi")
    {
        update();
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
    Keymap::Ptr keymap = processor.gallery->getKeymap(processor.updateState->currentKeymapId);
    
    String text = tf.getText();
    String name = tf.getName();
    BKTextEditor* textEditor;

    if (name == "KeymapMidi")
    {
        textEditor = &keymapTF;
        Array<int> keys = keymapStringToIntArray(text);
        
        processor.gallery->setKeymap(processor.updateState->currentKeymapId, keys);
        keyboard->setKeysInKeymap(keys);
    }
    else if (name == "HarmonizerAllKeymapMidi")
    {
        textEditor = &harAllKeymapTF;
        Array<int> keys = keymapStringToIntArray(text);
        textEditor->setText(processor.gallery->getKeymap(processor.updateState->currentKeymapId)->getHarmonizerTextForDisplay());
        
        processor.gallery->setKeymapHarmonizersFromString(processor.updateState->currentKeymapId, text);
    }
    else // if (name == "HarmonizerArrayKeymapMidi")
    {
        textEditor = &harArrayKeymapTF;
        Array<int> keys = keymapStringToIntArray(text);
        textEditor->setText(intArrayToString(keys));
        
        processor.gallery->setKeymapHarmonization(processor.updateState->currentKeymapId, keymap->getHarKey() + keymap->getHarPreTranspose(), keys);
        harArrayKeyboard->setKeysInKeymap(keys);
    }
    
    textEditor->setVisible(false);
    textEditor->toBack();
    
    update();

    processor.updateState->editsMade = true;
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
    textEditor.setVisible(false);
    textEditor.toBack();
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
    if(textEditor.getName() == keymapTF.getName() || textEditor.getName() == harArrayKeymapTF.getName() || textEditor.getName() == harAllKeymapTF.getName())
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
        //harKey = km->getHarKey();

        selectCB.setSelectedId(processor.updateState->currentKeymapId, dontSendNotification);
        invertOnOffToggle.setToggleState(km->isInverted(), dontSendNotification);
        midiEditToggle.setToggleState(km->getMidiEdit(), dontSendNotification);
        harMidiEditToggle.setToggleState(km->getHarMidiEdit(), dontSendNotification);
        harArrayMidiEditToggle.setToggleState(km->getHarArrayMidiEdit(), dontSendNotification);
        endKeystrokesToggle.setToggleState(km->getAllNotesOff(), dontSendNotification);
        ignoreSustainToggle.setToggleState(km->getIgnoreSustain(), dontSendNotification);
        sustainPedalKeysToggle.setToggleState(km->getSustainPedalKeys(), dontSendNotification);
        keymapTF.setText( intArrayToString(km->keys()));
        harAllKeymapTF.setText(km->getHarmonizerTextForDisplay());
        harArrayKeymapTF.setText(intArrayToString(km->getHarmonizationForKey(true, false)));
        harPreTranspositionSlider->setValue(km->getHarPreTranspose(), dontSendNotification);
        harPostTranspositionSlider->setValue(km->getHarPostTranspose(), dontSendNotification);
        keyboard->setKeysInKeymap(km->keys());
        harKeyboard->setKeysInKeymap(Array<int>(km->getHarKey()));
        harArrayKeyboard->setKeysInKeymap(km->getHarmonizationForKey(true, false));
        
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
    if (source == &keyboardState)
    {
        thisKeymap->toggleNote(midiNoteNumber);

        update();

        keyboard->setKeysInKeymap(thisKeymap->keys());
    }
    else if (source == &harKeyboardState)
    {
        //harKey = midiNoteNumber;
        thisKeymap->setHarKey(midiNoteNumber);
        update();
    }
    else if (source == &harArrayKeyboardState)
    {
        thisKeymap->toggleHarmonizerList(midiNoteNumber);
        update();
    }
    
    processor.updateState->editsMade = true;
    
    // We call configure here to reconfigure any mods, reset, and pianomaps
    // for newly selected/deselected keys
    processor.currentPiano->configure();
}



void KeymapViewController::sliderValueChanged (Slider* slider)
{
#if JUCE_IOS
    if (slider == &octaveSlider)
    {
        int octave = (int) octaveSlider.getValue();
        
        if (octave == 0)    keyboard->setAvailableRange(21, 45);
        else                keyboard->setAvailableRange(12+octave*12, 36+octave*12);
    }
    else if (slider == &harOctaveSlider)
    {
        int octave = (int) harOctaveSlider.getValue();
        
        if (octave == 0)    harKeyboard->setAvailableRange(21, 45);
        else                harKeyboard->setAvailableRange(12+octave*12, 36+octave*12);
    }
    else if (slider == &harArrayOctaveSlider)
    {
        int octave = (int) harArrayOctaveSlider.getValue();
        
        if (octave == 0)    harArrayKeyboard->setAvailableRange(21, 45);
        else                harArrayKeyboard->setAvailableRange(12+octave*12, 36+octave*12);
    }
#endif
    processor.updateState->editsMade = true;
}

void KeymapViewController::BKSingleSliderValueChanged(BKSingleSlider* slider, String name, double val)
{
    if (slider == harPreTranspositionSlider.get())
    {
        int transposition = (int) harPreTranspositionSlider->getValue();
        Keymap::Ptr km = processor.gallery->getKeymap(processor.updateState->currentKeymapId);
        km->setHarPreTranspose(transposition);
        DBG("harPreTranspositionSlider = " + String(transposition));
        
        update();
    }
    else if (slider == harPostTranspositionSlider.get())
    {
        int transposition = (int) harPostTranspositionSlider->getValue();
        Keymap::Ptr km = processor.gallery->getKeymap(processor.updateState->currentKeymapId);
        km->setHarPostTranspose(transposition);
        DBG("harPostTranspositionSlider = " + String(transposition));
        
        update();
    }

    processor.updateState->editsMade = true;
}



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

    midiEditToggle.setToggleState(km->getMidiEdit(), dontSendNotification);
    harMidiEditToggle.setToggleState(km->getHarMidiEdit(), dontSendNotification);
    harArrayMidiEditToggle.setToggleState(km->getHarArrayMidiEdit(), dontSendNotification);
    
    if (km->getMidiEdit())
    {
        keyboard->setKeysInKeymap(km->keys());
    }
    else if (km->getHarMidiEdit())
    {
        harKeyboard->setKeysInKeymap(Array<int>(km->getHarKey()));
        harArrayKeyboard->setKeysInKeymap(km->getHarmonizationForKey(true, false));
    }
    else if (km->getHarArrayMidiEdit())
    {
        harArrayKeyboard->setKeysInKeymap(km->getHarmonizationForKey(true, false));
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
