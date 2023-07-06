#include "BKListBoxComponent.h"
#include "../../PluginEditor.h"
BKListBoxComponent::BKListBoxComponent(BKAudioProcessor &p)
    : p(p),
 itemData(p.gallery->getPianoIteratorOrder()),
    listBoxModel(listBox, itemData)
{
   // listBoxModel.addChangeListener(&listBox);
    listBox.addChangeListener(this);
    wrapperType = p.wrapperType;
   // itemData = p.gallery->getPianoIteratorOrder();
    addBtn.setButtonText("Add Item...");
    addBtn.onClick = [this]()
    {
        getPianoOptionMenu().showMenuAsync (PopupMenu::Options().withTargetComponent (&addBtn), ModalCallbackFunction::forComponent (pianoOptionMenuCallback, this) );
    };
    iteratorOnOff.setButtonText("Iterator On/Off");
    iteratorOnOff.setTooltip("Turning on Piano Iterator will disable Piano Mapper Objects");
    iteratorOnOff.onClick = [this]()
    {
        this->p.gallery->iteratorIsEnabled = iteratorOnOff.getToggleState();
        if(this->p.gallery->iteratorIsEnabled)
        {
            for (auto piano : this->p.gallery->getPianos())
            {
                for (auto item : piano->getItems())
                {
                   if(item->getType() == PreparationTypePianoMap)
                   {
                       item->isDisabled = true;
                       item->setInterceptsMouseClicks(true, false);
                       item->setComponentEffect(new MonochromeEffect());
                   }
                }
            }
        }
        else
        {
            for (auto piano : this->p.gallery->getPianos())
            {
                for (auto item : piano->getItems())
                {
                   if(item->getType() == PreparationTypePianoMap)
                   {
                       item->isDisabled = false;
                       item->setInterceptsMouseClicks(true, true);
                       item->setComponentEffect(nullptr);
                       
                   }
                }
            }
        }
    };
    addAndMakeVisible(addBtn);
    listBox.setMultipleSelectionEnabled(true);
    listBox.setModel(&listBoxModel);
    upKeyboardLabel.setLookAndFeel(&laf);
    upKeyboardLabel.setText("Forward Key: ",dontSendNotification);
    downKeyboardLabel.setLookAndFeel(&laf);
    downKeyboardLabel.setText("Backward Key: ",dontSendNotification);
    upKeyboard = std::make_unique<BKKeymapKeyboardComponent> (upKeyboardState, BKKeymapKeyboardComponent::horizontalKeyboard);
    upKeyboard->setAvailableRange(21, 108);
    upKeyboard->setAllowDrag(true);
    upKeyboard->setOctaveForMiddleC(4);
    upKeyboardState.addListener(this);
    
    downKeyboard = std::make_unique<BKKeymapKeyboardComponent> (downKeyboardState, BKKeymapKeyboardComponent::horizontalKeyboard);
    downKeyboard->setAvailableRange(21, 108);
    downKeyboard->setAllowDrag(true);
    downKeyboard->setOctaveForMiddleC(4);
    downKeyboardState.addListener(this);
    
    listBox.setRowHeight(40);
    
    
    midiInputSelectButton.setName("MidiInput");
    midiInputSelectButton.setButtonText("Select MIDI Inputs");
    midiInputSelectButton.setTooltip("Select from available MIDI input devices");
    midiInputSelectButton.addListener(this);
    
    if(wrapperType == juce::AudioPluginInstance::wrapperType_Standalone)
        addAndMakeVisible(midiInputSelectButton);
    
    
   
    
    //addAndMakeVisible(midiInputSelectButton);
    addAndMakeVisible(listBox);
    addAndMakeVisible(upKeyboardLabel);
    addAndMakeVisible(downKeyboardLabel);
    addAndMakeVisible(upKeyboard.get());
    addAndMakeVisible(downKeyboard.get());
    addAndMakeVisible(iteratorOnOff);
    p.updateState->iteratorViewActive = true;
    setSize (600, 400);
}

void BKListBoxComponent::paint (Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
}

void BKListBoxComponent::bkButtonClicked(Button *b)
{
    if (b == &midiInputSelectButton)
    {
        getMidiInputSelectMenu().showMenuAsync(PopupMenu::Options().withTargetComponent(&midiInputSelectButton), ModalCallbackFunction::forComponent(midiInputSelectCallback, this));
    }
}

void BKListBoxComponent::resized()
{
    auto area = getLocalBounds().reduced(20);
    auto row = area.removeFromTop(24);
    addBtn.setBounds(row.removeFromRight(100));
    if (wrapperType == juce::AudioPluginInstance::wrapperType_Standalone)
    {
        midiInputSelectButton.setBounds(row.removeFromLeft(100));
        midiInputSelectButton.setVisible(true);
    }
    iteratorOnOff.setBounds(row.removeFromLeft(100));
    
    area.removeFromTop(6);
    auto keyRow = area.removeFromBottom(120);
    auto half = area.getWidth() / 2;
    auto labelRow = keyRow.removeFromTop(40);
    
    downKeyboardLabel.setBounds(labelRow.removeFromLeft(half).removeFromLeft(half/2));
    upKeyboardLabel.setBounds(labelRow.removeFromLeft(half/2));
    
    auto keyboardRow = keyRow;
    downKeyboard->setBounds(keyboardRow.removeFromLeft(half).reduced(2,0));
    float keyWidth = downKeyboard->getWidth() / round((108 - 22) * 7. / 12 + 1); //num white keys
    downKeyboard->setKeyWidth(2*keyWidth);
    downKeyboard->setBlackNoteLengthProportion(0.6);
    
    upKeyboard->setBounds(keyboardRow.reduced(2, 0));
    keyWidth = upKeyboard->getWidth() / round((108 - 22) * 7. / 12 + 1); //num white keys
    upKeyboard->setKeyWidth(2*keyWidth);
    upKeyboard->setBlackNoteLengthProportion(0.6);
    listBox.setBounds(area);
    
    
}

PopupMenu BKListBoxComponent::getPianoOptionMenu()
{
    BKPopupMenu menu;
    
    Piano::PtrArr pianos = p.gallery->getPianos();
    
    for (int i = 0; i < pianos.size(); i++)
    {
        String name = pianos[i]->getName();
        
        
        if (name != String())
        {
            menu.addItem(pianos[i]->getId(),pianos[i]->getName());
            menu.addSeparator();
        }
    }
    return std::move(menu);
}

void BKListBoxComponent::pianoOptionMenuCallback(int res, BKListBoxComponent* bkl)
{
    if (bkl == nullptr)
    {
        PopupMenu::dismissAllActiveMenus();
        return;
    }
    
    BKAudioProcessor& processor = bkl->p;
    
    bkl->itemData.modelData.push_back( processor.gallery->getPianos()[res-1]);
    bkl->listBox.updateContent();
//    processor.gallery->setPianoIteratorOrder(bkl->itemData.modelData);
}

bool BKListBoxComponent::keyPressed(const KeyPress &k)
{
    int code = k.getKeyCode();
    if (code == 68) //D
    {
        if(k.getModifiers().isCommandDown())
        {
//            std::vector<int> rows;
//            for (int i = 0; i < modelData.getNumItems(); i++)
//            {
//                if(listBox.isRowSelected(i))
//                {
//                    rows.push_back(i);
//                }
//                    //dynamic_cast<DraggableListBoxItem*>(getComponentForRowNumber(i))->itemDroppedSelected( rowToDrop, before);
//            }
////            if (rowNum == 0)
////            {
////                rowNum = 1;
////            }
//            
//            int newIndex = modelData.moveToIndex(rows, rowNum);
//            listBox.deselectAllRows();
//          
//
//                listBox.selectRangeOfRows(newIndex, newIndex + rows.size()-1);
            itemData.duplicate(listBox.getSelectedRow());
            listBox.updateContent();
            listBox.selectRow(listBox.getSelectedRow() + 1);
            listBox.updateContent();
        }
    }
    return true;
}

void BKListBoxComponent::handleKeymapNoteToggled (BKKeymapKeyboardState* source, int midiNoteNumber)
{
    if (source == &upKeyboardState)
    {
        p.gallery->getIteratorUpKeymap()->toggleNote(midiNoteNumber);
        upKeyboard->setKeysInKeymap(p.gallery->getIteratorUpKeymap()->keys());
    }
    else if (source == &downKeyboardState)
    {
        p.gallery->getIteratorDownKeymap()->toggleNote(midiNoteNumber);
        downKeyboard->setKeysInKeymap(p.gallery->getIteratorDownKeymap()->keys());
    }
    
    p.updateState->editsMade = true;
    
    
}
void BKListBoxComponent::changeListenerCallback(ChangeBroadcaster *source) 
{
    p.setCurrentPiano(itemData.modelData[listBox.getSelectedRow()]->getId());
    p.gallery->currentPianoIndex = listBox.getSelectedRow();
    p.updateState->currentIteratorPiano = listBox.getSelectedRow();
    
    p.getBKEditor()->getMainViewController().fillPianoCB();

    //update();
    
    p.saveGalleryToHistory("Change Piano");
}
