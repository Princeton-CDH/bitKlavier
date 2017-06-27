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
BKDraggableComponent(true,false,true),
processor(p),
type(type),
Id(Id),
mapper(new ModificationMapper(BKPreparationTypeNil, -1))
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
    else if (type == PreparationTypeMod)
    {
        setImage(ImageCache::getFromMemory(BinaryData::mod_unassigned_icon_png, BinaryData::mod_unassigned_icon_pngSize));
    }
    else if (type == PreparationTypePianoMap)
    {
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
    
        menu.setSelectedId(0, NotificationType::dontSendNotification);
    }
    else if (type == PreparationTypeReset)
    {
        setImage(image = ImageCache::getFromMemory(BinaryData::reset_icon_png, BinaryData::reset_icon_pngSize));
        mapper->setType(PreparationTypeReset);
    }
    
    if (type == PreparationTypeMod || type == PreparationTypePianoMap || type == PreparationTypeReset)
        processor.currentPiano->addMapper(mapper);
    
}

BKItem::~BKItem()
{
    if (type == PreparationTypeMod || type == PreparationTypePianoMap || type == PreparationTypeReset)
        processor.currentPiano->removeMapper(mapper);
}

void BKItem::setImage(Image newImage)
{
    image = newImage;
    
    placement = RectanglePlacement::centred;
    
    int val =
    (type == PreparationTypeMod && type == PreparationTypeReset) ?  90 :
    (type == PreparationTypePianoMap) ? 75 :
    (type == PreparationTypeKeymap) ? 90 :
    (type == PreparationTypeTempo || type == PreparationTypeTuning) ? 55 :
    65;
    
    
    while (!(image.getWidth() < val || image.getHeight() < val))
        image = image.rescaled(image.getWidth() * 0.75, image.getHeight() * 0.75);
    
    if (type != PreparationTypePianoMap)    setSize(image.getWidth(), image.getHeight());
    else                                    setSize(image.getWidth(), image.getHeight() + 25);
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

bool BKItem::compare(BKItem* otherItem)
{
    if (type == otherItem->getType())
    {
        if (type == PreparationTypeMod)
        {
            if ((mapper->getType() == otherItem->mapper->getType()))
            {
                if (mapper->getType() == PreparationTypeReset)
                {
                    for (int i = 0; i < 5; i++)
                    {
                        if (mapper->resets[i] != otherItem->mapper->resets[i]) return false;
                    }
                    
                    if (mapper->getKeymaps() == otherItem->mapper->getKeymaps()) return true;
                }
                else if ((mapper->getTargets() == otherItem->mapper->getTargets()) &&
                         (mapper->getKeymaps() == otherItem->mapper->getKeymaps()))
                {
                    return true;
                }
            }
        }
        else if (type == PreparationTypePianoMap)
        {
        
        }
        else
        {
            if (Id == otherItem->getId()) return true;
        }
    }
}


void BKItem::copy(BKItem::Ptr itemToCopy)
{
    name = itemToCopy->getName();
    position = itemToCopy->getPosition();
    type = itemToCopy->getType();
    Id = itemToCopy->getId();
    currentId = itemToCopy->getSelectedId();
    mapper = itemToCopy->getMapper();
}

void BKItem::bkComboBoxDidChange    (ComboBox* cb)
{
    String name = cb->getName();
    int Id = cb->getSelectedId();
    if (name == "PianoMap")
    {
        if (Id != currentId)
        {
            currentId = Id;
            
            ((BKConstructionSite*)getParentComponent())->pianoMapDidChange(this);
            
            mapper->addTarget(currentId);
            
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
        else if (type == PreparationTypeMod)
        {
            BKPreparationType modType = mapper->getType();
            int modId = mapper->getId();
            
            if (modType == PreparationTypeDirect)
            {
                processor.updateState->currentModDirectId = modId;
                processor.updateState->directDidChange = true;
                processor.updateState->setCurrentDisplay(DisplayDirectMod);
            }
            else if (modType == PreparationTypeNostalgic)
            {
                processor.updateState->currentModNostalgicId = modId;
                processor.updateState->nostalgicPreparationDidChange = true;
                processor.updateState->setCurrentDisplay(DisplayNostalgicMod);
            }
            else if (modType == PreparationTypeSynchronic)
            {
                processor.updateState->currentModSynchronicId = modId;
                processor.updateState->synchronicPreparationDidChange = true;
                processor.updateState->setCurrentDisplay(DisplaySynchronicMod);
            }
            else if (modType == PreparationTypeTuning)
            {
                processor.updateState->currentModTuningId = modId;
                processor.updateState->tuningPreparationDidChange = true;
                processor.updateState->setCurrentDisplay(DisplayTuningMod);
            }
            else if (modType == PreparationTypeTempo)
            {
                processor.updateState->currentModTempoId = modId;
                processor.updateState->tempoPreparationDidChange = true;
                processor.updateState->setCurrentDisplay(DisplayTuningMod);
            }
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

inline void BKItem::addConnection(BKItem* item)
{
    connections.add(item);
}

bool BKItem::isConnectedWith(BKItem* item)
{
    return connections.contains(item);
}

void BKItem::removeConnection(BKItem* toDisconnect)
{
    int index = 0;
    for (auto item : connections)
    {
        if (toDisconnect == item)
        {
            connections.remove(index);
            break;
        }
        
        index++;
        
    }
    
}


// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ BKGraph ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

void BKItemGraph::updateLast(void)
{
    last.clear();
    
    for (auto item : items)
    {
        BKItem* toAdd = new BKItem(item->getType(), item->getId(), processor);
        
        toAdd->copy(item);
        
        last.add(toAdd);
    }
    
}

void BKItemGraph::updateClipboard(void)
{
    clipboard.clear();
    
    for (auto item : getSelectedItems())
    {
        BKItem* toAdd = new BKItem(item->getType(), item->getId(), processor);
        
        toAdd->copy(item);
        
        clipboard.add(toAdd);
    }
    
    /*
    for (int i = items.size(); --i >= 0; )
    {
        for (auto connection : items[i]->getConnections())
        {
            for (auto item : clipboard)
            {
                if (item->compare(connection))
                {
                    clipboard[i]->addConnection(item);
                }
            }
                
        }

    }
     */
}



void BKItemGraph::add(BKItem* itemToAdd)
{
    items.add(itemToAdd);
    processor.currentPiano->configuration->addItem(itemToAdd->getType(), itemToAdd->getId());
    
    
}

BKItem* BKItemGraph::get(BKPreparationType type, int Id)
{
    for (auto item : items)
    {
        if ((item->getType() == type) && (item->getId() == Id))
        {
            return item;
        }
    }
    return nullptr;
}

bool BKItemGraph::contains(BKItem* thisItem)
{
    bool alreadyThere = false;
    for (auto item : items)
    {
        if ((item->getType() == thisItem->getType()) && (item->getId() == thisItem->getId()))
        {
            alreadyThere = true;
            break;
        }
    }
    
    return alreadyThere;
}

BKItem* BKItemGraph::itemWithTypeAndId(BKPreparationType type, int Id)
{
   // if (type >= PreparationTypeKeymap) return nullptr;
    BKItem* thisItem = new BKItem(type, Id, processor);
    
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
        disconnectUI(itemToRemove, item);
    }
    
    items.removeObject(itemToRemove);
    
}

void BKItemGraph::remove(BKItem* itemToRemove)
{
    for (auto item : itemToRemove->getConnections())
    {
        disconnect(itemToRemove, item);
    }
    
    items.removeObject(itemToRemove);
    
    
    
}


void BKItemGraph::clear(void)
{
    for (auto itemToRemove : items)
    {
        for (auto item : itemToRemove->getConnections())
        {
            disconnect(itemToRemove, item);
        }
        
        items.removeObject(itemToRemove);
    }
    
    
}

void BKItemGraph::linkSynchronicWithTempo(Synchronic::Ptr synchronic, Tempo::Ptr thisTempo)
{
    synchronic->setTempo(thisTempo);
}

void BKItemGraph::linkNostalgicWithSynchronic(Nostalgic::Ptr nostalgic, Synchronic::Ptr synchronic)
{
    nostalgic->setSynchronic(synchronic);
}

void BKItemGraph::linkPreparationWithTuning(BKPreparationType thisType, int thisId, Tuning::Ptr thisTuning)
{
    if (thisType == PreparationTypeDirect)
    {
        Direct::Ptr thisDirect = processor.gallery->getDirect(thisId);
        
        thisDirect->setTuning(thisTuning);
    }
    else if (thisType == PreparationTypeSynchronic)
    {
        Synchronic::Ptr thisSynchronic = processor.gallery->getSynchronic(thisId);
        
        thisSynchronic->setTuning(thisTuning);
    }
    else if (thisType == PreparationTypeNostalgic)
    {
        Nostalgic::Ptr thisNostalgic = processor.gallery->getNostalgic(thisId);
        
        thisNostalgic->setTuning(thisTuning);
    }
}


void BKItemGraph::removePreparationFromKeymap(BKPreparationType thisType, int thisId, Keymap::Ptr thisKeymap)
{
    PreparationMap::Ptr thisPreparationMap = processor.currentPiano->getPreparationMapWithKeymap(thisKeymap);
    
    if (thisPreparationMap == nullptr) return;
    
    if (thisType == PreparationTypeDirect)
    {
        Direct::Ptr thisDirect = processor.gallery->getDirect(thisId);
        
        thisPreparationMap->removeDirect(thisDirect);
    }
    else if (thisType == PreparationTypeSynchronic)
    {
        Synchronic::Ptr thisSynchronic = processor.gallery->getSynchronic(thisId);
        
        thisPreparationMap->removeSynchronic(thisSynchronic);
    }
    else if (thisType == PreparationTypeNostalgic)
    {
        Nostalgic::Ptr thisNostalgic = processor.gallery->getNostalgic(thisId);
        
        thisPreparationMap->removeNostalgic(thisNostalgic);
    }
    else if (thisType == PreparationTypeTempo)
    {
        Tempo::Ptr thisTempo = processor.gallery->getTempo(thisId);
        
        thisPreparationMap->removeTempo(thisTempo);
    }
    else if (thisType == PreparationTypeTuning)
    {
        Tuning::Ptr thisTuning = processor.gallery->getTuning(thisId);
        
        thisPreparationMap->removeTuning(thisTuning);
    }
    
}

void BKItemGraph::addPreparationToKeymap(BKPreparationType thisType, int thisId, Keymap::Ptr thisKeymap)
{
    PreparationMap::Ptr thisPreparationMap = processor.currentPiano->getPreparationMapWithKeymap(thisKeymap);
    
    if (thisPreparationMap == nullptr)
    {
        int whichPMap = processor.currentPiano->addPreparationMap(thisKeymap);
        
        thisPreparationMap = processor.currentPiano->prepMaps[whichPMap];
    }
    
    if (thisType == PreparationTypeDirect)
    {
        Direct::Ptr thisDirect = processor.gallery->getDirect(thisId);
        
        thisPreparationMap->addDirect(thisDirect);
    }
    else if (thisType == PreparationTypeSynchronic)
    {
        Synchronic::Ptr thisSynchronic = processor.gallery->getSynchronic(thisId);
        
        thisPreparationMap->addSynchronic(thisSynchronic);
    }
    else if (thisType == PreparationTypeNostalgic)
    {
        Nostalgic::Ptr thisNostalgic = processor.gallery->getNostalgic(thisId);
        
        thisPreparationMap->addNostalgic(thisNostalgic);
    }
    else if (thisType == PreparationTypeTempo)
    {
        Tempo::Ptr thisTempo = processor.gallery->getTempo(thisId);
        
        thisPreparationMap->addTempo(thisTempo);
    }
    else if (thisType == PreparationTypeTuning)
    {
        Tuning::Ptr thisTuning = processor.gallery->getTuning(thisId);
        
        thisPreparationMap->addTuning(thisTuning);
    }

}

void BKItemGraph::route(bool connect, BKItem* item1, BKItem* item2)
{
    BKPreparationType item1Type = item1->getType();
    int item1Id = item1->getId();
    
    BKPreparationType item2Type = item2->getType();
    int item2Id = item2->getId();

    
    if (connect)
    {
        if (item1->isConnectedWith(item2) && item2->isConnectedWith(item1)) return;
        
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
        else if (item1Type == PreparationTypePianoMap || item1Type == PreparationTypeMod || item1Type == PreparationTypeReset)
        {
            if (item1->getMapper()->getKeymaps().size()) return;
        }
        else if (item2Type == PreparationTypePianoMap || item2Type == PreparationTypeMod || item2Type == PreparationTypeReset)
        {
            if (item2->getMapper()->getKeymaps().size()) return;
        }
        
        // MODS RESETS AND PMAPS CAN ONLY HAVE ONE KEYMAP!
        
        item1->addConnection(item2);
        item2->addConnection(item1);
        
    }
    else // !connect
    {
        if (!item1->isConnectedWith(item2) && !item2->isConnectedWith(item1)) return;
        
        item1->removeConnection(item2);
        item2->removeConnection(item1);
    }
    
    
    // CONFIGURATIONS
    if (item1Type == PreparationTypeKeymap && item2Type == PreparationTypePianoMap)
    {
        Keymap::Ptr thisKeymap = processor.gallery->getKeymap(item1Id);
        int pianoId = item2->getSelectedId();
        
        ModificationMapper::Ptr thisMapper = item2->getMapper();
        
        if (connect)
        {
            thisMapper->addKeymap(item1Id);
            processor.currentPiano->configurePianoMap(thisKeymap, pianoId);
        }
        else
        {
            processor.currentPiano->deconfigurePianoMap(thisKeymap, pianoId);
            thisMapper->clearKeymaps();
        }
        
    }
    else if (item1Type == PreparationTypePianoMap && item2Type == PreparationTypeKeymap)
    {
        Keymap::Ptr thisKeymap = processor.gallery->getKeymap(item2Id);
        
        ModificationMapper::Ptr thisMapper = item1->getMapper();
        
        if (connect)
        {
            thisMapper->addKeymap(item2Id);
            processor.currentPiano->configurePianoMap(thisKeymap, thisMapper->getTargets().getFirst());
        }
        else
        {
            processor.currentPiano->deconfigurePianoMap(thisKeymap, thisMapper->getTargets().getFirst());
            thisMapper->clearKeymaps();
        }
    }

    else if (item1Type == PreparationTypeKeymap && item2Type <= PreparationTypeTempo)
    {
        if (connect)    addPreparationToKeymap(item2Type, item2Id, processor.gallery->getKeymap(item1Id));
        else
        {
            Keymap::Ptr thisKeymap = processor.gallery->getKeymap(item1Id);
            
            processor.currentPiano->removePreparationMapWithKeymap(thisKeymap);
        }
    }
    else if (item1Type <= PreparationTypeTempo && item2Type == PreparationTypeKeymap)
    {
        if (connect)    addPreparationToKeymap(item1Type, item1Id, processor.gallery->getKeymap(item2Id));
        else            removePreparationFromKeymap(item1Type, item1Id, processor.gallery->getKeymap(item2Id));
    }
    else if (item1Type == PreparationTypeTuning && item2Type <= PreparationTypeNostalgic)
    {
        linkPreparationWithTuning(item2Type, item2Id, processor.gallery->getTuning(item1Id));
    }
    else if (item1Type <= PreparationTypeNostalgic && item2Type == PreparationTypeTuning)
    {
        linkPreparationWithTuning(item1Type, item1Id, processor.gallery->getTuning(item2Id));
    }
    else if (item1Type == PreparationTypeTempo && item2Type == PreparationTypeSynchronic)
    {
        Tempo::Ptr thisTempo = processor.gallery->getTempo(item1Id);
        Synchronic::Ptr thisSynchronic = processor.gallery->getSynchronic(item2Id);
        
        linkSynchronicWithTempo(thisSynchronic, thisTempo);
    }
    else if (item1Type == PreparationTypeSynchronic && item2Type == PreparationTypeTempo)
    {
        Tempo::Ptr thisTempo = processor.gallery->getTempo(item2Id);
        Synchronic::Ptr thisSynchronic = processor.gallery->getSynchronic(item1Id);
        
        linkSynchronicWithTempo(thisSynchronic, thisTempo);
    }
    else if (item1Type == PreparationTypeNostalgic && item2Type == PreparationTypeSynchronic)
    {
        Nostalgic::Ptr thisNostalgic = processor.gallery->getNostalgic(item1Id);
        Synchronic::Ptr thisSynchronic = processor.gallery->getSynchronic(item2Id);
        
        linkNostalgicWithSynchronic(thisNostalgic, thisSynchronic);
    }
    else if (item1Type == PreparationTypeSynchronic && item2Type == PreparationTypeNostalgic)
    {
        Nostalgic::Ptr thisNostalgic = processor.gallery->getNostalgic(item2Id);
        Synchronic::Ptr thisSynchronic = processor.gallery->getSynchronic(item1Id);
        
        linkNostalgicWithSynchronic(thisNostalgic, thisSynchronic);
    }
    else if ((item1Type == PreparationTypeMod || item1Type == PreparationTypeReset) && item2Type <= PreparationTypeTempo)
    {
        ModificationMapper::Ptr thisMapper = item1->getMapper();
        
        BKPreparationType modType = item2Type;
        
        if (thisMapper->getType() == BKPreparationTypeNil)
        {
            thisMapper->setType(modType);
            thisMapper->setId(0);
        }
        
        if (thisMapper->getType() == PreparationTypeReset)
        {
            if (connect)
            {
                Array<int> reset = thisMapper->resets.getUnchecked(item2Type);
                reset.addIfNotAlreadyThere(item2Id);
                thisMapper->resets.set(item2Type, reset);
                
                DBG("resets: " + arrayIntArrayToString(thisMapper->resets));
                processor.currentPiano->configureModification(thisMapper);
            }
            else
            {
                processor.currentPiano->deconfigureModification(thisMapper);
                Array<int> resets = thisMapper->resets.getUnchecked(item2Type);
                for (int i = 0; i < resets.size(); i++)
                {
                    if (resets[i] == item2Id) thisMapper->resets.getUnchecked(item2Type).remove(item2Id);
                }
            }
        }
        else if (thisMapper->getType() == item2Type)
        {
            int Id = item2Id;
            
            if (connect)
            {
                thisMapper->addTarget(Id);
                processor.currentPiano->configureModification(thisMapper);
            }
            else
            {
                processor.currentPiano->deconfigureModification(thisMapper);
                thisMapper->clearTargets();
            }
            
            if (modType == PreparationTypeDirect)
            {
                item1->setImage(ImageCache::getFromMemory(BinaryData::mod_direct_icon_png, BinaryData::mod_direct_icon_pngSize));
            }
            else if (modType == PreparationTypeSynchronic)
            {
                item1->setImage(ImageCache::getFromMemory(BinaryData::mod_synchronic_icon_png, BinaryData::mod_synchronic_icon_pngSize));
            }
            else if (modType == PreparationTypeNostalgic)
            {
                item1->setImage(ImageCache::getFromMemory(BinaryData::mod_nostalgic_icon_png, BinaryData::mod_nostalgic_icon_pngSize));
            }
            else if (modType == PreparationTypeTempo)
            {
                item1->setImage(ImageCache::getFromMemory(BinaryData::mod_tempo_icon_png, BinaryData::mod_tempo_icon_pngSize));
            }
            else if (modType == PreparationTypeTuning)
            {
                item1->setImage(ImageCache::getFromMemory(BinaryData::mod_tuning_icon_png, BinaryData::mod_tuning_icon_pngSize));
            }
        }
        
    }
    else if (item1Type <= PreparationTypeTempo && (item2Type == PreparationTypeMod || item2Type == PreparationTypeReset))
    {
        ModificationMapper::Ptr thisMapper = item2->getMapper();
        
        BKPreparationType modType = item1Type;
        
        if (thisMapper->getType() == BKPreparationTypeNil)
        {
            thisMapper->setType(modType);
            thisMapper->setId(0);
        }
        
        if (thisMapper->getType() == PreparationTypeReset)
        {
            if (connect)
            {
                thisMapper->resets.getUnchecked(item1Type).add(item1Id);
                processor.currentPiano->configureModification(thisMapper);
            }
            else
            {
                processor.currentPiano->deconfigureModification(thisMapper);
                Array<int> resets = thisMapper->resets.getUnchecked(item1Type);
                for (int i = 0; i < resets.size(); i++)
                {
                    if (resets[i] == item1Id) thisMapper->resets.getUnchecked(item1Type).remove(item1Id);
                }
            }
            
        }
        else if (thisMapper->getType() == item1Type)
        {
            ModificationMapper::Ptr thisMapper = item2->getMapper();
            
            int Id = item1Id;
            
            if (connect)
            {
                thisMapper->addTarget(Id);
                processor.currentPiano->configureModification(thisMapper);
            }
            else
            {
                processor.currentPiano->deconfigureModification(thisMapper);
                thisMapper->clearTargets();
            }
        }
    }
    else if (item1Type == PreparationTypeKeymap && (item2Type == PreparationTypeMod || item2Type == PreparationTypeReset))
    {
        ModificationMapper::Ptr thisMapper = item2->getMapper();
        
        if (connect)
        {
            thisMapper->addKeymap(item1Id);
            processor.currentPiano->configureModification(thisMapper);
        }
        else
        {
            processor.currentPiano->deconfigureModification(thisMapper);
            thisMapper->clearKeymaps();
        }
    }
    else if (item2Type == PreparationTypeKeymap && (item1Type == PreparationTypeMod || item1Type == PreparationTypeReset))
    {
        ModificationMapper::Ptr thisMapper = item1->getMapper();
        
        if (connect)
        {
            thisMapper->addKeymap(item2Id);
            processor.currentPiano->configureModification(thisMapper);
        }
        else
        {
            processor.currentPiano->deconfigureModification(thisMapper);
            thisMapper->clearKeymaps();
        }
    }
    else
    {
        item1->removeConnection(item2);
        item2->removeConnection(item1);
    }
}



void BKItemGraph::reconnect(BKItem* item1, BKItem* item2)
{
    route(false, item1, item2);
    route(true, item1, item2);
    
    print();
}

void BKItemGraph::update(BKPreparationType type, int Id)
{
    // Only applies to Keymaps and Modification types, so as to make sure that when ModPreparations are changed, so are the Modifications.
    // Also applies to Resets and PianoMaps.

    if (type == PreparationTypeKeymap || type == PreparationTypePianoMap) // All the mods
    {
        for (auto item : items)
        {
            if (item->getType() == type && item->getId() == Id)
            {
                for (auto connection : item->getConnections())
                {
                    reconnect(item, connection);
                }
            }
        }
    }
    

}

void BKItemGraph::updateMod(BKPreparationType modType, int modId)
{
    for (auto item : items)
    {
        if (item->getMapper()->getType() == modType && item->getMapper()->getId() == modId)
        {
            for (auto connection : item->getConnections())
            {
                reconnect(item, connection);
            }
        }
    }
}

void BKItemGraph::disconnectUI(BKItem* item1, BKItem* item2)
{
    item1->removeConnection(item2);
    item2->removeConnection(item1);
}

void BKItemGraph::connectUI(BKItem* item1, BKItem* item2)
{
    item1->addConnection(item2);
    item2->addConnection(item1);
}

void BKItemGraph::connect(BKItem* item1, BKItem* item2)
{
    route(true, item1, item2);
    
    print();
}

void BKItemGraph::disconnect(BKItem* item1, BKItem* item2)
{
    route(false, item1, item2);
    
    print();
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

Array<Line<float>> BKItemGraph::getLines(void)
{
    Array<Line<float>> lines;
    
    for (auto thisItem : items)
    {
        for (auto otherItem : thisItem->getConnections())
        {
            Point<float> otherOrigin = otherItem->origin;

            lines.add(Line<float>(thisItem->getX() + thisItem->getWidth()/2.0f, thisItem->getY() + thisItem->getHeight()/2.0f, otherItem->getX() + otherItem->getWidth()/2.0f, otherItem->getY() + otherItem->getHeight()/2.0f));
        }
    }
    
    return lines;
}

void BKItemGraph::reconstruct(void)
{
    itemIdCount = 0;
    
    preparations.clear();
    
    Piano::Ptr thisPiano = processor.currentPiano;
    
    int pmapcount = 0;
    for (auto pmap : thisPiano->getPreparationMaps())
    {
        
        DBG("--------------------------------");
        pmap->print();
        // Keymap
        int keymapId = pmap->getKeymapId();
        
        DBG("PMAP"+String(pmapcount++));
        DBG("    keymap"+String(keymapId));
        
        BKItem* keymap;
        BKItem* newPreparation;
        
        keymap = itemWithTypeAndId(PreparationTypeKeymap, keymapId);

        if (!contains(keymap)) add(keymap);
    
    
        for (auto p : pmap->getTuning())
        {
            int Id = p->getId();
            
            DBG("    tuning"+String(Id));
            
            newPreparation = itemWithTypeAndId(PreparationTypeTuning, Id);
            
            if (!contains(newPreparation))
            {
                add(newPreparation);
                preparations.add(newPreparation);
            }
        }
        
        for (auto p : pmap->getTempo())
        {
            int Id = p->getId();
            
            DBG("    tempo"+String(Id));
            
            newPreparation = itemWithTypeAndId(PreparationTypeTempo, Id);
            
            if (!contains(newPreparation))
            {
                add(newPreparation);
                preparations.add(newPreparation);
            }
        }
        
        for (auto p : pmap->getDirect())
        {
            int Id = p->getId();
            
            DBG("    direct"+String(Id));
            
            newPreparation = itemWithTypeAndId(PreparationTypeDirect, Id);
            
            if (!contains(newPreparation))
            {
                add(newPreparation);
                
                preparations.add(newPreparation);
                
                int tuningId = p->getTuningId();
                
                BKItem* thisTuning = itemWithTypeAndId(PreparationTypeTuning, tuningId);
                
                if (!contains(thisTuning))  add(thisTuning);
                
                connectUI(newPreparation, thisTuning);
            }
            
        }
        
        for (auto p : pmap->getSynchronic())
        {
            int Id = p->getId();
            
            DBG("    synchronic"+String(Id));
            
            newPreparation = itemWithTypeAndId(PreparationTypeSynchronic, Id);
            
            if (!contains(newPreparation))
            {
                add(newPreparation);
                preparations.add(newPreparation);
            
                // TUNING
                int tuningId = p->getTuningId();
                
                BKItem* thisTuning = itemWithTypeAndId(PreparationTypeTuning, tuningId);
                
                if (!contains(thisTuning))  add(thisTuning);
                
                connectUI(newPreparation, thisTuning);
                
                
                // TEMPO
                int tempoId = p->getTempoId();
                
                BKItem* thisTempo = itemWithTypeAndId(PreparationTypeTempo, tempoId);
                
                if (!contains(thisTempo))  add(thisTempo);
                
                connectUI(newPreparation, thisTempo);
            }
            
            
        }
        
        for (auto p : pmap->getNostalgic())
        {
            int Id = p->getId();
            
            DBG("    nostalgic"+String(Id));
            
            newPreparation = itemWithTypeAndId(PreparationTypeNostalgic, Id);

            if (!contains(newPreparation))
            {
                add(newPreparation);
                preparations.add(newPreparation);
                
                // TUNING
                int tuningId = p->getTuningId();
                
                BKItem* thisTuning = itemWithTypeAndId(PreparationTypeTuning, tuningId);
                
                if (!contains(thisTuning))  add(thisTuning);
                
                connectUI(newPreparation, thisTuning);
                
                
                // SYNC TARGET
                int syncId = p->getSynchronicTargetId();
                
                BKItem* thisSyncTarget = itemWithTypeAndId(PreparationTypeSynchronic, syncId);
                
                if (!contains(thisSyncTarget))  add(thisSyncTarget);
                
                connectUI(newPreparation, thisSyncTarget);
            }
            
            
        }
        
        for (auto p : preparations)
        {
            connectUI(keymap, p);
        }
        
    }

    int cc = 0;
    for (auto map : thisPiano->getMappers())
    {
        DBG("heyyyy " + String(cc++));
        BKPreparationType targetType = map->getType();
        Array<int> targetIds = map->getTargets();
        Array<int> keymaps = map->getKeymaps();
        
        BKItem* thisMod;
        if (map->getType() == PreparationTypeReset)     thisMod = itemWithTypeAndId(PreparationTypeReset, -1);
        else if (map->getType() == PreparationTypeMod)  thisMod = itemWithTypeAndId(PreparationTypeMod, -1);
        else continue;
        
        thisMod->setMapper(map);
        
        add(thisMod);
        
        for (auto k : keymaps)
        {
            BKItem* thisKeymap = itemWithTypeAndId(PreparationTypeKeymap, k);
            if (!contains(thisKeymap)) add(thisKeymap);
            
            connectUI(thisKeymap, thisMod);
        }
        
        if (map->getType() == PreparationTypeReset)
        {
            for (auto t : targetIds)
            {
                BKItem* thisTarget;
                
                if (targetType == PreparationTypeDirect)
                {
                    thisTarget = itemWithTypeAndId(PreparationTypeDirect, t);
                }
                else if (targetType == PreparationTypeSynchronic)
                {
                    thisTarget = itemWithTypeAndId(PreparationTypeSynchronic, t);
                }
                else if (targetType == PreparationTypeNostalgic)
                {
                    thisTarget = itemWithTypeAndId(PreparationTypeNostalgic, t);
                }
                else if (targetType == PreparationTypeTempo)
                {
                    thisTarget = itemWithTypeAndId(PreparationTypeTempo, t);
                }
                else if (targetType == PreparationTypeTuning)
                {
                    thisTarget = itemWithTypeAndId(PreparationTypeTuning, t);
                }
                
                if (!contains(thisTarget)) add(thisTarget);
                
                connectUI(thisTarget, thisMod);
            }
        }
        else
        {
            for (int rtype = 0; rtype < 5; rtype++)
            {
                for (auto t : map->resets[rtype])
                {
                    BKItem* thisTarget;
                    
                    if (rtype == PreparationTypeDirect)
                    {
                        thisTarget = itemWithTypeAndId(PreparationTypeDirect, t);
                    }
                    else if (rtype == PreparationTypeSynchronic)
                    {
                        thisTarget = itemWithTypeAndId(PreparationTypeSynchronic, t);
                    }
                    else if (rtype == PreparationTypeNostalgic)
                    {
                        thisTarget = itemWithTypeAndId(PreparationTypeNostalgic, t);
                    }
                    else if (rtype == PreparationTypeTempo)
                    {
                        thisTarget = itemWithTypeAndId(PreparationTypeTempo, t);
                    }
                    else if (rtype == PreparationTypeTuning)
                    {
                        thisTarget = itemWithTypeAndId(PreparationTypeTuning, t);
                    }
                    
                    if (!contains(thisTarget)) add(thisTarget);
                    
                    connectUI(thisTarget, thisMod);
                }
            }
        }
        
    }
    
    Array<int> pianoMap = thisPiano->pianoMap;
    int count = 0;
    for (int i = 0; i < 128; i++)
    {
        if (pianoMap[i] != -1)
        {
            BKItem* thisMap = itemWithTypeAndId(PreparationTypePianoMap, count++);
            
            if (!contains(thisMap)) add(thisMap);
            
            thisMap->setSelectedId(pianoMap[i]);
            
            Keymap::PtrArr keymaps = processor.gallery->getKeymaps();
            
            Keymap::Ptr newKeymap = new Keymap(keymaps.size());
            newKeymap->addNote(i);
            
            bool found = false;
            
            for (auto keymap : keymaps)
            {
                if (newKeymap->compare(keymap))
                {
                    newKeymap = keymap;
                    found = true;
                    break;
                }
            }
            
            if (!found) processor.gallery->addKeymap(newKeymap);
            
            BKItem* thisKeymap = itemWithTypeAndId(PreparationTypeKeymap, newKeymap->getId());
            
            if (!contains(thisKeymap)) add(thisKeymap);
            
            connectUI(thisMap, thisKeymap);
        }
    }
    
}


