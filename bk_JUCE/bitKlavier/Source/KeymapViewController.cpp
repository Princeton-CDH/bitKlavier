/*
  ==============================================================================

    KeymapViewController.cpp
    Created: 9 Dec 2016 11:57:38am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "KeymapViewController.h"

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
    
    addAndMakeVisible(keymapTF);
    keymapTF.addListener(this);
    keymapTF.setName("KeymapMidi");
    keymapTF.setMultiLine(true);
    
    // Keyboard
    addAndMakeVisible (keyboardComponent = new BKKeymapKeyboardComponent (keyboardState,
                                                                 BKKeymapKeyboardComponent::horizontalKeyboard));
    
    keyboard = (BKKeymapKeyboardComponent*)keyboardComponent.get();
    keyboard->setScrollButtonsVisible(false);
    keyboard->setAvailableRange(21, 108);
    keyboard->setAllowDrag(true);
    keyboard->setOctaveForMiddleC(4);
    keyboardState.addListener(this);
    
    keyboardValsTextFieldOpen.setName("KSLIDERTXTEDITALLBUTTON");
    keyboardValsTextFieldOpen.addListener(this);
    keyboardValsTextFieldOpen.setButtonText("edit all");
    addAndMakeVisible(keyboardValsTextFieldOpen);
    keymapTF.setVisible(false);
    keymapTF.toBack();
    
    addAndMakeVisible(hideOrShow);
    hideOrShow.setName("hideOrShow");
    hideOrShow.addListener(this);
    hideOrShow.setButtonText(" X ");
    
    addAndMakeVisible(actionButton);
    actionButton.setButtonText("Action");
    actionButton.addListener(this);
    
    fillSelectCB(-1,-1);

    update();
}

KeymapViewController::~KeymapViewController()
{
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
    
    float paddingScalarX = (float)(getTopLevelComponent()->getWidth() - gMainComponentMinWidth) / (gMainComponentWidth - gMainComponentMinWidth);
    float paddingScalarY = (float)(getTopLevelComponent()->getHeight() - gMainComponentMinHeight) / (gMainComponentHeight - gMainComponentMinHeight);
    
    iconImageComponent.setBounds(area);
    area.reduce(10 * paddingScalarX + 4, 10 * paddingScalarY + 4);
    
    float keyboardHeight = 86; // + 36 * paddingScalarY;
    Rectangle<int> keyboardRow = area.removeFromBottom(keyboardHeight);
    float keyWidth = keyboardRow.getWidth() / round((108 - 21) * 7./12 + 1); //num white keys
    keyboard->setKeyWidth(keyWidth);
    keyboard->setBlackNoteLengthProportion(0.65);
    keyboardRow.reduce(gXSpacing, 0);
    keyboard->setBounds(keyboardRow);
    keymapTF.setBounds(keyboardRow);
    
    area.removeFromBottom(gYSpacing);
    Rectangle<int> textButtonSlab = area.removeFromBottom(gComponentComboBoxHeight);
    textButtonSlab.removeFromLeft(gXSpacing);
    keyboardValsTextFieldOpen.setBounds(textButtonSlab.removeFromLeft(75));
    
    Rectangle<int> leftColumn = area.removeFromLeft(area.getWidth() * 0.5);
    Rectangle<int> comboBoxSlice = leftColumn.removeFromTop(gComponentComboBoxHeight);
    comboBoxSlice.removeFromRight(4 + 2.*gPaddingConst * paddingScalarX);
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
        keymapTF.setVisible(true);
        keymapTF.toFront(true);
        focusLostByEscapeKey = false;
    }
    else if (b == &actionButton)
    {
        getModOptionMenu().showMenuAsync (PopupMenu::Options().withTargetComponent (&actionButton), ModalCallbackFunction::forComponent (actionButtonCallback, this) );
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
    if(!focusLostByEscapeKey)
    {
        keymapUpdated(tf);
    }
    
}

void KeymapViewController::textEditorEscapeKeyPressed (TextEditor& textEditor)
{
    focusLostByEscapeKey = true;
    keymapTF.setVisible(false);
    keymapTF.toBack();
    unfocusAllComponents();
}

void KeymapViewController::update(void)
{
    if (processor.updateState->currentKeymapId < 0) return;
    
    Keymap::Ptr km = processor.gallery->getKeymap(processor.updateState->currentKeymapId);
    
    if (km != nullptr)
    {
        selectCB.setSelectedId(processor.updateState->currentKeymapId, dontSendNotification);
        
        keymapTF.setText( intArrayToString(km->keys()));
        
        keymapNameTF.setText(km->getName());
        
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



