/*
  ==============================================================================

    BKGraph.cpp
    Created: 6 Apr 2017 12:24:29pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include "BKGraph.h"

#include "PreparationMap.h"

#include "Piano.h"

#include "BKConstructionSite.h"

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ BKItem ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
BKItem::BKItem(BKPreparationType type, int Id, BKAudioProcessor& p):
ItemMapper(type, Id),
BKDraggableComponent(true,false,true),
processor(p)
{
    fullChild.setAlwaysOnTop(true);
    addAndMakeVisible(fullChild);
    
    if (type == PreparationTypeTuning)
    {
        setImage(ImageCache::getFromMemory(BinaryData::tuning_icon_png, BinaryData::tuning_icon_pngSize));
    }
    else if (type == PreparationTypeTempo)
    {
        setImage(ImageCache::getFromMemory(BinaryData::tempo_icon_png, BinaryData::tempo_icon_pngSize));
    }
    else if (type == PreparationTypeSynchronic)
    {
        setImage(ImageCache::getFromMemory(BinaryData::synchronic_icon_png, BinaryData::synchronic_icon_pngSize));
    }
    else if (type == PreparationTypeNostalgic)
    {
        setImage(ImageCache::getFromMemory(BinaryData::nostalgic_icon_png, BinaryData::nostalgic_icon_pngSize));
    }
    else if (type == PreparationTypeDirect)
    {
        setImage(ImageCache::getFromMemory(BinaryData::direct_icon_png, BinaryData::direct_icon_pngSize));
    }
    else if (type == PreparationTypeKeymap)
    {
        setImage(ImageCache::getFromMemory(BinaryData::keymap_icon_png, BinaryData::keymap_icon_pngSize));
    }
    else if (type == PreparationTypePianoMap || type == PreparationTypeGenericMod || (type >= PreparationTypeDirectMod && type <= PreparationTypeTempoMod) || type == PreparationTypeReset)
    {
        setItemType(type, false);
    }
}

BKItem::BKItem(ItemMapper::Ptr mapper, BKAudioProcessor& p):
BKItem(mapper->getType(), mapper->getId(), p)
{
    
}

BKItem::~BKItem()
{

}

void BKItem::setImage(Image newImage)
{
    image = newImage;
    
    placement = RectanglePlacement::centred;
    
    int val =
    (type == PreparationTypeGenericMod && type == PreparationTypeReset) ?  80 :
    (type == PreparationTypePianoMap) ? 120 :
    (type == PreparationTypeKeymap) ? 80 :
    (type == PreparationTypeTempo || type == PreparationTypeTuning) ? 60 :
    65;
    
    
    while (!(image.getWidth() < val || image.getHeight() < val))
        image = image.rescaled(image.getWidth() * 0.8, image.getHeight() * 0.8);
    
    if (type != PreparationTypePianoMap)    setSize(image.getWidth(), image.getHeight());
    else                                    setSize(image.getWidth(), image.getHeight() + 25);
}

void BKItem::setItemType(BKPreparationType newType, bool create)
{
    
    if (type != PreparationTypeGenericMod) setActive(false);

    if (create)
    {
        Id = processor.gallery->getNewId(newType);
        processor.gallery->addTypeWithId(newType, Id);
    }
    
    type = newType;
    
    if (type == PreparationTypeGenericMod)
    {
        setImage(ImageCache::getFromMemory(BinaryData::mod_unassigned_icon_png, BinaryData::mod_unassigned_icon_pngSize));
    }
    else if (type == PreparationTypeReset)
    {
        setImage(image = ImageCache::getFromMemory(BinaryData::reset_icon_png, BinaryData::reset_icon_pngSize));
    }
    else if (type == PreparationTypeDirectMod)
    {
        setImage(ImageCache::getFromMemory(BinaryData::mod_direct_icon_png, BinaryData::mod_direct_icon_pngSize));
    }
    else if (type == PreparationTypeSynchronicMod)
    {
        setImage(ImageCache::getFromMemory(BinaryData::mod_synchronic_icon_png, BinaryData::mod_synchronic_icon_pngSize));
    }
    else if (type == PreparationTypeNostalgicMod)
    {
        setImage(ImageCache::getFromMemory(BinaryData::mod_nostalgic_icon_png, BinaryData::mod_nostalgic_icon_pngSize));
    }
    else if (type == PreparationTypeTuningMod)
    {
        setImage(ImageCache::getFromMemory(BinaryData::mod_tuning_icon_png, BinaryData::mod_tuning_icon_pngSize));
    }
    else if (type == PreparationTypeTempoMod)
    {
        setImage(ImageCache::getFromMemory(BinaryData::mod_tempo_icon_png, BinaryData::mod_tempo_icon_pngSize));
    }
    else if (type == PreparationTypePianoMap)
    {
        ItemMapper::setType(PreparationTypePianoMap);
        
        setPianoTarget(processor.currentPiano->getId());
        
        setImage(ImageCache::getFromMemory(BinaryData::piano_icon_png, BinaryData::piano_icon_pngSize));
        
        addAndMakeVisible(menu);
        
        menu.setName(cPreparationTypes[type]);
        menu.addListener(this);
        
        Piano::PtrArr pianos = processor.gallery->getPianos();
        for (int i = 0; i < pianos.size(); i++)
        {
            menu.addItem(pianos[i]->getName(), i+1);
            menu.addSeparator();
        }
        
        menu.setSelectedItemIndex(processor.gallery->getIndexFromId(PreparationTypePiano, processor.currentPiano->getId()),
                                  NotificationType::dontSendNotification);
    }
    
    if (type != PreparationTypeGenericMod)
    {
        setActive(true);
        
        saveBounds(getBounds());
        processor.currentPiano->add(this);
    }
    
    repaint();
}

void BKItem::paint(Graphics& g)
{
    
    g.setOpacity (1.0f);
    g.drawImage (image, getLocalBounds().toFloat(), placement);
    
    if (isSelected)
    {
        g.setColour(Colours::white);
        g.drawRect(getLocalBounds(),2);
    }
    else
    {
        g.setColour(Colours::transparentWhite);
        g.drawRect(getLocalBounds(),0);
    }
    
    
}

void BKItem::resized(void)
{
    if (type == PreparationTypePianoMap)
    {
        menu.setBounds(0, image.getHeight(), getWidth(), 25);
    }
    else
    {
        label.setBounds(0,0,getWidth(),getHeight());
    }
    
    fullChild.setBounds(0,0,getWidth(),getHeight());
}



void BKItem::copy(BKItem::Ptr itemToCopy)
{
    name = itemToCopy->getItemName();
    position = itemToCopy->getPosition();
    type = itemToCopy->getType();
    Id = itemToCopy->getId();
    currentId = itemToCopy->getSelectedPianoId();
    
    // COPY CONNECTIONS TOO, OR HAVE REDUNDANT 2D ARRAY OF CONNEX
}

void BKItem::bkComboBoxDidChange    (ComboBox* cb)
{
    String name = cb->getName();
    int pianoId = processor.gallery->getIdFromIndex(PreparationTypePiano, cb->getSelectedItemIndex());
    
    if (name == "PianoMap")
    {
        if (pianoId != currentId)
        {
            currentId = pianoId;
            
            // FIX THIS
            
            ((BKConstructionSite*)getParentComponent())->pianoMapDidChange(this);
            
            DBG("New piano selected: "+String(currentId));
        }
    }
}

void BKItem::itemIsBeingDragged(const MouseEvent& e, Point<int> startPosition)
{
    ((BKConstructionSite*)getParentComponent())->itemIsBeingDragged(this, startPosition);
}

void BKItem::mouseDown(const MouseEvent& e)
{
    
    ((BKConstructionSite*)getParentComponent())->setCurrentItem(this);
    
    if (isDraggable)
    {
        prepareDrag(e);
    }
    
    if (e.getNumberOfClicks() >= 2)
    {
        
        if (type == PreparationTypeDirect)
        {
            processor.updateState->currentDirectId = Id;
            processor.updateState->directPreparationDidChange = true;
            processor.updateState->setCurrentDisplay(DisplayDirect);
        }
        else if (type == PreparationTypeSynchronic)
        {
            processor.updateState->currentSynchronicId = Id;
            processor.updateState->synchronicPreparationDidChange = true;
            processor.updateState->setCurrentDisplay(DisplaySynchronic);
        }
        else if (type == PreparationTypeNostalgic)
        {
            processor.updateState->currentNostalgicId = Id;
            processor.updateState->nostalgicPreparationDidChange = true;
            processor.updateState->setCurrentDisplay(DisplayNostalgic);
        }
        else if (type == PreparationTypeTuning)
        {
            processor.updateState->currentTuningId = Id;
            processor.updateState->tuningPreparationDidChange = true;
            processor.updateState->setCurrentDisplay(DisplayTuning);
        }
        else if (type == PreparationTypeTempo)
        {
            processor.updateState->currentTempoId = Id;
            processor.updateState->tempoPreparationDidChange = true;
            processor.updateState->setCurrentDisplay(DisplayTempo);
        }
        else if (type == PreparationTypeKeymap)
        {
            processor.updateState->currentKeymapId = Id;
            processor.updateState->keymapDidChange = true;
            processor.updateState->setCurrentDisplay(DisplayKeymap);
        }
        else if (type == PreparationTypeDirectMod)
        {
            processor.updateState->currentModDirectId = Id;
            processor.updateState->directDidChange = true;
            processor.updateState->setCurrentDisplay(DisplayDirectMod);
        }
        else if (type == PreparationTypeNostalgicMod)
        {
            processor.updateState->currentModNostalgicId = Id;
            processor.updateState->nostalgicPreparationDidChange = true;
            processor.updateState->setCurrentDisplay(DisplayNostalgicMod);
        }
        else if (type == PreparationTypeSynchronicMod)
        {
            processor.updateState->currentModSynchronicId = Id;
            processor.updateState->synchronicPreparationDidChange = true;
            processor.updateState->setCurrentDisplay(DisplaySynchronicMod);
        }
        else if (type == PreparationTypeTuningMod)
        {
            processor.updateState->currentModTuningId = Id;
            processor.updateState->tuningPreparationDidChange = true;
            processor.updateState->setCurrentDisplay(DisplayTuningMod);
        }
        else if (type == PreparationTypeTempoMod)
        {
            processor.updateState->currentModTempoId = Id;
            processor.updateState->tempoPreparationDidChange = true;
            processor.updateState->setCurrentDisplay(DisplayTempoMod);
        }
        else if (type == PreparationTypePianoMap)
        {
            menu.showPopup();
        }
    }

}

void BKItem::keyPressedWhileSelected(const KeyPress& e)
{
    
}



// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ BKGraph ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

void BKItemGraph::updateClipboard(void)
{
    clipboard.clear();
    
    for (auto item : getSelectedItems())
    {
        BKItem* toAdd = new BKItem(item->getType(), item->getId(), processor);
        
        toAdd->copy(item);
        
        clipboard.add(toAdd);
    }
}

void BKItemGraph::addAndRegisterItem(BKItem* thisItem)
{
    addItem(thisItem);
    registerItem(thisItem);
}

void BKItemGraph::addItem(BKItem* thisItem)
{
    items.add(thisItem);
}

void BKItemGraph::registerItem(BKItem* thisItem)
{
    thisItem->saveBounds(thisItem->getBounds());
    
    thisItem->setActive(true);
    
    processor.currentPiano->add(thisItem);
}

void BKItemGraph::removeItem(BKItem* thisItem)
{
    items.removeObject(thisItem);
}

void BKItemGraph::unregisterItem(BKItem* thisItem)
{
    for (auto item : thisItem->getConnections()) item->removeConnection(thisItem);
    
    thisItem->setActive(false);
    
    processor.currentPiano->remove(thisItem);
}

void BKItemGraph::removeAndUnregisterItem(BKItem* thisItem)
{
    unregisterItem(thisItem);
    removeItem(thisItem);
}

bool BKItemGraph::contains(BKItem* thisItem)
{
    bool alreadyThere = false;
    for (auto item : items)
    {
        if (item->getType() == BKPreparationTypeNil) continue;
        
        if ((item->getType() == thisItem->getType()) && (item->getId() == thisItem->getId()))
        {
            alreadyThere = true;
            break;
        }
    }
    
    return alreadyThere;
}


bool BKItemGraph::contains(BKPreparationType type, int Id)
{
    bool alreadyThere = false;
    for (auto item : items)
    {
        if ((item->getType() == type) && (item->getId() == Id))
        {
            alreadyThere = true;
            break;
        }
    }
    
    return alreadyThere;
}

BKItem::Ptr BKItemGraph::get(BKPreparationType type, int Id)
{
    BKItem::Ptr thisItem = nullptr;

    for (auto item : items)
    {
        if (item->getType() == type && item->getId() == Id)
        {
            thisItem = item;
            break;
        }
    }

    return thisItem;
}

void BKItemGraph::removeUI(BKItem* itemToRemove)
{
    for (auto item : itemToRemove->getConnections())
    {
        disconnectUI(itemToRemove, getItem(item));
    }

    items.removeObject(itemToRemove);
    
}

void BKItemGraph::clear(void)
{
    for (auto itemToRemove : items)
    {
        removeAndUnregisterItem(itemToRemove);
    }
}

void BKItemGraph::route(bool connect, bool reconfigure, BKItem* item1, BKItem* item2)
{
    BKPreparationType item1Type = item1->getType();
    int item1Id = item1->getId();
    
    BKPreparationType item2Type = item2->getType();
    int item2Id = item2->getId();
    
    // DEAL WITH CONNECTION STUFF OVER HERE... or not? maybe move all connection/graph stuff to piano

    
    if (connect)
    {
        if (item1->isConnectedTo(item2Type, item2Id) && item2->isConnectedTo(item1Type, item1Id)) return;
        
        if (item1Type == PreparationTypeTuning && item2Type <= PreparationTypeNostalgic)
        {
            if (item2Type == PreparationTypeDirect)
            {
                disconnectTuningFromDirect(item2);
            }
            else if (item2Type == PreparationTypeNostalgic)
            {
                disconnectTuningFromNostalgic(item2);
            }
            if (item2Type == PreparationTypeSynchronic)
            {
                disconnectTuningFromSynchronic(item2);
            }
        }
        else if (item1Type <= PreparationTypeNostalgic && item2Type == PreparationTypeTuning)
        {
            if (item1Type == PreparationTypeDirect)
            {
                disconnectTuningFromDirect(item1);
            }
            else if (item1Type == PreparationTypeNostalgic)
            {
                disconnectTuningFromNostalgic(item1);
            }
            if (item1Type == PreparationTypeSynchronic)
            {
                disconnectTuningFromSynchronic(item1);
            }

        }
        else if (item1Type == PreparationTypeSynchronic && item2Type == PreparationTypeTempo)
        {
            disconnectTempoFromSynchronic(item1);
        }
        else if (item1Type == PreparationTypeTempo && item2Type == PreparationTypeSynchronic)
        {
            disconnectTempoFromSynchronic(item2);
        }
        else if (item1Type == PreparationTypeSynchronic && item2Type == PreparationTypeNostalgic)
        {
            disconnectSynchronicFromNostalgic(item2);
        }
        else if (item1Type == PreparationTypeNostalgic && item2Type == PreparationTypeSynchronic)
        {
            disconnectSynchronicFromNostalgic(item1);
        }
        // MODS RESETS AND PMAPS CAN ONLY HAVE ONE KEYMAP!
        else if (item2Type == PreparationTypeKeymap  &&
                (item1Type == PreparationTypePianoMap || item1Type == PreparationTypeGenericMod || item1Type == PreparationTypeReset))
        {
            if (item1->getConnectionsOfType(PreparationTypeKeymap).size()) return;
        }
        else if (item1Type == PreparationTypeKeymap &&
                (item2Type == PreparationTypePianoMap || item2Type == PreparationTypeGenericMod || item2Type == PreparationTypeReset))
        {
            if (item2->getConnectionsOfType(PreparationTypeKeymap).size()) return;
        }
        
        // MODS RESETS AND PMAPS CAN ONLY HAVE ONE KEYMAP!
        
        item1->addConnection(item2);
        item2->addConnection(item1);
        
    }
    else // !connect
    {
        if (!item1->isConnectedTo(item2Type, item1Type) && !item2->isConnectedTo(item1Type, item1Id)) return;
        
        item1->removeConnection(item2Type, item2Id);
        item2->removeConnection(item1Type, item1Id);
    }
}



void BKItemGraph::reconnect(BKItem* item1, BKItem* item2)
{
    route(false, true, item1, item2);
    route(true, true, item1, item2);
    
    print();
}

BKItem::Ptr BKItemGraph::getItem(ItemMapper::Ptr mapper)
{
    for (auto item : items)
    {
        if (item == mapper) return item;
    }
    return nullptr;
}

BKItem::Ptr BKItemGraph::createItem(ItemMapper::Ptr mapper)
{
    return new BKItem(mapper, processor);
}

void BKItemGraph::update(BKPreparationType type, int Id)
{
    // Only applies to Keymaps and Modification types, so as to make sure that when ModPreparations are changed, so are the Modifications.
    // Also applies to PianoMaps.

    if (type == PreparationTypeKeymap || type == PreparationTypePianoMap || (type >= PreparationTypeDirectMod && type <= PreparationTypeTempoMod)) // All the mods
    {
        for (auto item : items)
        {
            if (item->getType() == type && item->getId() == Id)
            {
                for (auto connection : item->getConnections())
                {
                    reconnect(item, getItem(connection));
                }
            }
        }
    }
    

}



void BKItemGraph::updateMod(BKPreparationType modType, int modId)
{
    for (auto item : items)
    {
        if (item->getType() == modType && item->getId() == modId)
        {
            for (auto connection : item->getConnections())
            {
                reconnect(item,  getItem(connection));
            }
        }
    }
}

void BKItemGraph::disconnectUI(BKItem* item1, BKItem* item2)
{
    
}

void BKItemGraph::connect(BKItem* item1, BKItem* item2)
{
    BKPreparationType item1Type = item1->getType();
    BKPreparationType item2Type = item2->getType();
    int item1Id = item1->getId();
    int item2Id = item2->getId();
    
    if (item1Type == PreparationTypeGenericMod)
    {
        if (item2Type >= PreparationTypeDirect && item2Type <= PreparationTypeTempo)
        {
            item1->setItemType(getModType(item2Type), true);
        }
        else return;
    }
    if (item2Type == PreparationTypeGenericMod)
    {
        if (item1Type >= PreparationTypeDirect && item1Type <= PreparationTypeTempo)
        {
            item2->setItemType(getModType(item1Type), true);
        }
        else return;
    }
    
    item1->addConnection(item2);
    item2->addConnection(item1);

    processor.currentPiano->configure();
}

void BKItemGraph::connectWithoutCreatingNew(BKItem* item1, BKItem* item2)
{
    item1->addConnection(item2);
    item2->addConnection(item1);
}

void BKItemGraph::disconnect(BKItem* item1, BKItem* item2)
{
    BKPreparationType item1Type = item1->getType();
    BKPreparationType item2Type = item2->getType();
    int item1Id = item1->getId();
    int item2Id = item2->getId();
    
    item1->removeConnection(item2);
    item2->removeConnection(item1);
    
    if (item1Type == PreparationTypeGenericMod)
    {
        if (!item1->isConnectedToAnyPreparation())
        {
            item1->setItemType(PreparationTypeGenericMod, false);
        }
    }
    else if (item2Type == PreparationTypeGenericMod)
    {
        if (!item2->isConnectedToAnyPreparation())
        {
            item2->setItemType(PreparationTypeGenericMod, false);
        }
    }
}

void BKItemGraph::disconnectTuningFromSynchronic(BKItem* synchronicItem)
{
    BKPreparationType thisItemType = synchronicItem->getType();
    
    if (thisItemType != PreparationTypeSynchronic) return;
    
    for (auto otherItem : synchronicItem->getConnections())
    {
        BKPreparationType otherItemType = otherItem->getType();
        
        if (otherItemType == PreparationTypeTuning)
        {
            // reset tempo of synchronic in model
            
            synchronicItem->removeConnection(otherItem);
            otherItem->removeConnection(synchronicItem);
        }
    }
    
}

void BKItemGraph::disconnectTuningFromNostalgic(BKItem* nostalgicItem)
{
    BKPreparationType thisItemType = nostalgicItem->getType();
    
    if (thisItemType != PreparationTypeNostalgic) return;
    
    for (auto otherItem : nostalgicItem->getConnections())
    {
        BKPreparationType otherItemType = otherItem->getType();
        
        if (otherItemType == PreparationTypeTuning)
        {
            // reset tempo of synchronic in model
            
            nostalgicItem->removeConnection(otherItem);
            otherItem->removeConnection(nostalgicItem);
        }
    }
    
}

void BKItemGraph::disconnectTuningFromDirect(BKItem* directItem)
{
    BKPreparationType thisItemType = directItem->getType();
    
    if (thisItemType != PreparationTypeDirect) return;
    
    for (auto otherItem : directItem->getConnections())
    {
        BKPreparationType otherItemType = otherItem->getType();
        
        if (otherItemType == PreparationTypeTuning)
        {
            // reset tempo of synchronic in model
            
            directItem->removeConnection(otherItem);
            otherItem->removeConnection(directItem);
        }
    }
    
}

void BKItemGraph::disconnectTempoFromSynchronic(BKItem* synchronicItem)
{
    BKPreparationType thisItemType = synchronicItem->getType();
    
    if (thisItemType != PreparationTypeSynchronic) return;

    for (auto otherItem : synchronicItem->getConnections())
    {
        BKPreparationType otherItemType = otherItem->getType();
        
        if (otherItemType == PreparationTypeTempo)
        {
            // reset tempo of synchronic in model
            
            synchronicItem->removeConnection(otherItem);
            otherItem->removeConnection(synchronicItem);
        }
    }
}

void BKItemGraph::disconnectSynchronicFromNostalgic(BKItem* nostalgicItem)
{
    BKPreparationType thisItemType = nostalgicItem->getType();
    
    if (thisItemType != PreparationTypeNostalgic) return;
    
    for (auto otherItem : nostalgicItem->getConnections())
    {
        BKPreparationType otherItemType = otherItem->getType();
        
        if (otherItemType == PreparationTypeSynchronic)
        {
            // reset tempo of synchronic in model
            
            nostalgicItem->removeConnection(otherItem);
            otherItem->removeConnection(nostalgicItem);
        }
    }
}

Array<Line<int>> BKItemGraph::getLines(void)
{
    Array<Line<int>> lines;
    
    for (auto thisItem : items)
    {
        for (auto otherItem : thisItem->getConnections())
        {
            Rectangle<int> otherBounds = otherItem->retrieveBounds();

            lines.add(Line<int>(thisItem->getX() + thisItem->getWidth()/2.0f,
                                thisItem->getY() + thisItem->getHeight()/2.0f,
                                otherBounds.getX() + otherBounds.getWidth()/2.0f,
                                otherBounds.getY() + otherBounds.getHeight()/2.0f));
        }
    }
    
    return lines;
}

void BKItemGraph::reconstruct(void)
{
    Piano::Ptr thisPiano = processor.currentPiano;
    
    // Create items based on the ItemMappers in current Piano and add them to BKItemGraph
    for (auto item : thisPiano->getItems())
    {
        BKItem* newItem = new BKItem(item, processor);
        
        addItem(newItem);
        
        newItem->setTopLeftPosition(newItem->retrieveXY());
    }
}


