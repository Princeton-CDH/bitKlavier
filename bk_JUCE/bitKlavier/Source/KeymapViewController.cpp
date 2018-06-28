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
BKViewController(p, theGraph)
{
    setLookAndFeel(&buttonsAndMenusLAF);
    
    iconImageComponent.setImage(ImageCache::getFromMemory(BinaryData::keymap_icon_png, BinaryData::keymap_icon_pngSize));
    iconImageComponent.setImagePlacement(RectanglePlacement(juce::RectanglePlacement::stretchToFit));
    iconImageComponent.setAlpha(0.095);
    addAndMakeVisible(iconImageComponent);
    
    selectCB.setName("Keymap");
    selectCB.addSeparator();
    selectCB.addListener(this);
    selectCB.addMyListener(this);
    selectCB.setSelectedItemIndex(0);
    addAndMakeVisible(selectCB);
    
    clearButton.setName("ClearButton");
    clearButton.setButtonText("Clear");
    clearButton.addListener(this);
    addAndMakeVisible(clearButton);
    
    keysButton.setName("KeysButton");
    keysButton.setButtonText("Keys");
    keysButton.addListener(this);
    addAndMakeVisible(keysButton);
    
    keysCB.setName("keysCB");
    keysCB.addListener(this);
    
    keysCB.addItem("Select", SELECT_ID);
    keysCB.addItem("Deselect", DESELECT_ID);
    
    keysCB.setSelectedId(SELECT_ID);
    
    addAndMakeVisible(keysCB);
    
    addAndMakeVisible(keymapTF);
    keymapTF.addListener(this);
    keymapTF.setName("KeymapMidi");
    keymapTF.setMultiLine(true);
    
    // Keyboard
    addAndMakeVisible (keyboardComponent = new BKKeymapKeyboardComponent (keyboardState,
                                                                 BKKeymapKeyboardComponent::horizontalKeyboard));
    
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
    actionButton.addListener(this);
    
    fillSelectCB(-1,-1);
    
    

    update();
}

KeymapViewController::~KeymapViewController()
{
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
    
    
    
    float keyboardHeight = 100; // + 36 * processor.paddingScalarY;
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
    
    keysCB.setBounds(keyboardValsTextFieldOpen.getRight() + gXSpacing, keyboardValsTextFieldOpen.getY(), keyboardValsTextFieldOpen.getWidth(), keyboardValsTextFieldOpen.getHeight());
    keysButton.setBounds(keysCB.getRight()+gXSpacing, keysCB.getY(), keysCB.getWidth(), keysCB.getHeight());
    
    clearButton.setBounds(keyboard->getRight() - keyboardValsTextFieldOpen.getWidth(), keyboardValsTextFieldOpen.getY(), keyboardValsTextFieldOpen.getWidth(),keyboardValsTextFieldOpen.getHeight());
    
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
}


void KeymapViewController::bkComboBoxDidChange        (ComboBox* box)
{
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
    
    PopupMenu menu;
    menu.setLookAndFeel(&buttonsAndMenusLAF);
    
    for (int i = 0; i < 12; i++)
    {
        Id = offset + i;
        DBG("ID: " + String(Id));
        menu.addItem(Id, pcs[i]);
    }
    
    return menu;
}

PopupMenu KeymapViewController::getKeysMenu(void)
{
    PopupMenu menu;
    menu.setLookAndFeel(&buttonsAndMenusLAF);
    
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
    
    return menu;
}

void KeymapViewController::keysMenuCallback(int result, KeymapViewController* vc)
{
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

void KeymapViewController::fillSelectCB(int last, int current)
{
    selectCB.clear(dontSendNotification);
    
    for (auto prep : processor.gallery->getKeymaps())
    {
        int Id = prep->getId();
    
        if (Id == -1) continue;
        
        String name = prep->getName();
        
        if (name != String::empty)  selectCB.addItem(name, Id);
        else                        selectCB.addItem("Keymap"+String(Id), Id);
        
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

void KeymapViewController::bkButtonClicked (Button* b)
{
    if (b == &hideOrShow)
    {
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
        getModOptionMenu().showMenuAsync (PopupMenu::Options().withTargetComponent (&actionButton), ModalCallbackFunction::forComponent (actionButtonCallback, this) );
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
        
        keymapTF.setText( intArrayToString(km->keys()));
        
        BKKeymapKeyboardComponent* keyboard =  (BKKeymapKeyboardComponent*)keyboardComponent.get();
        
        keyboard->setKeysInKeymap(km->keys());
    }
    
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



