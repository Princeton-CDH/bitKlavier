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

BKItem::~BKItem()
{
    processor.updateState->removeActive(type, Id);
    
    if (type != BKPreparationTypeNil) processor.currentPiano->remove(this);
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
    
    if (type != PreparationTypeGenericMod) processor.updateState->removeActive(type, Id);

    if (create)
    {
        Id = processor.gallery->getNewId(newType);
        processor.gallery->addTypeWithId(newType, Id);
    }
    
    type = newType;
    
    BKPreparationType mapperType = (type == PreparationTypeGenericMod || type == PreparationTypeReset || type == PreparationTypePianoMap) ? type : (BKPreparationType)(type - 6);
    
    ItemMapper::setType(mapperType);
    
    ItemMapper::setId(Id);
    
    if (type == PreparationTypeGenericMod)
    {
        setImage(ImageCache::getFromMemory(BinaryData::mod_unassigned_icon_png, BinaryData::mod_unassigned_icon_pngSize));
        
        processor.currentPiano->remove(this);
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
        processor.updateState->addActive(type, Id);
        
        saveXY(getPosition());
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



void BKItemGraph::add(BKItem* itemToAdd)
{
    itemToAdd->saveXY(itemToAdd->getPosition());
    processor.currentPiano->add(itemToAdd);
    processor.updateState->addActive(itemToAdd->getType(), itemToAdd->getId());
    items.add(itemToAdd);
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
    for (int type = 0; type < BKPreparationTypeNil; type++)
    {
        BKItem::PtrArr connex = getConnections(itemToRemove);
        
        for (auto item : connex)
        {
            disconnectUI(itemToRemove, item);
        }
    }
    
    items.removeObject(itemToRemove);
    
}

void BKItemGraph::remove(BKItem* itemToRemove)
{
    processor.currentPiano->remove(itemToRemove);
    
    // Remove itemToRemove from connection lists of all connected items
    for (auto item : itemToRemove->getConnections())
    {
        item->removeConnection(itemToRemove);
    }
    
    processor.currentPiano->remove(itemToRemove);
    items.removeObject(itemToRemove);
}

void BKItemGraph::remove(BKPreparationType type, int Id)
{
    for (auto item : items)
    {
        if (item->getType() == type && item->getId() == Id)
        {
            remove(item);
        }
    }
}

void BKItemGraph::clear(void)
{
    for (auto itemToRemove : items)
    {
        remove(itemToRemove);
    }
    
    
}

void BKItemGraph::route(bool connect, bool reconfigure, BKItem* item1, BKItem* item2)
{
    BKPreparationType item1Type = item1->getType();
    int item1Id = item1->getId();
    
    BKPreparationType item2Type = item2->getType();
    int item2Id = item2->getId();

    
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
    
    
    // CONFIGURATIONS
    if (item1Type == PreparationTypeKeymap && item2Type == PreparationTypePianoMap)
    {
        Keymap::Ptr thisKeymap = processor.gallery->getKeymap(item1Id);
        
        
        if (connect)
        {
            item2->addConnection(item1);
            processor.currentPiano->configureModification(thisMapper);
            
            processor.currentPiano->add(thisMapper);
        }
        else
        {
            processor.currentPiano->deconfigureModification(thisMapper);
            thisMapper->clearKeymaps();
            
            processor.currentPiano->remove(thisMapper);
        }
        
    }
    else if (item1Type == PreparationTypePianoMap && item2Type == PreparationTypeKeymap)
    {
        Keymap::Ptr thisKeymap = processor.gallery->getKeymap(item2Id);
        
        ItemMapper::Ptr thisMapper = item1->getMapper();
        
        if (connect)
        {
            thisMapper->addKeymap(item2Id);
            processor.currentPiano->configureModification(thisMapper);
            
            processor.currentPiano->add(thisMapper);
        }
        else
        {
            processor.currentPiano->deconfigureModification(thisMapper);
            thisMapper->clearKeymaps();
            
            processor.currentPiano->remove(thisMapper);
        }
    }

    else if (item1Type == PreparationTypeKeymap && item2Type <= PreparationTypeTempo)
    {
        if (connect)    addPreparationToKeymap(item2Type, item2Id, item1Id);
        else            removePreparationFromKeymap(item2Type, item2Id, item1Id);
    }
    else if (item1Type <= PreparationTypeTempo && item2Type == PreparationTypeKeymap)
    {
        if (connect)    addPreparationToKeymap(item1Type, item1Id, item2Id);
        else            removePreparationFromKeymap(item1Type, item1Id, item2Id);
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
    else if ((item1Type == PreparationTypeGenericMod || item1Type == PreparationTypeReset || (item1Type >= PreparationTypeDirectMod && item1Type <= PreparationTypeTempoMod)) && item2Type <= PreparationTypeTempo)
    {
        ItemMapper::Ptr thisMapper = item1->getMapper();
        
        BKPreparationType mapperType = (item1Type == PreparationTypeReset) ?  PreparationTypeReset : item2Type;
        
        if (mapperType == PreparationTypeReset)
        {
            if (connect)
            {
                Array<int> reset = thisMapper->resets.getUnchecked(item2Type);
                reset.addIfNotAlreadyThere(item2Id);
                thisMapper->resets.set(item2Type, reset);
                
                DBG("resets: " + arrayIntArrayToString(thisMapper->resets));
                processor.currentPiano->configureModification(thisMapper);
                
                processor.currentPiano->add(thisMapper);
            }
            else
            {
                processor.currentPiano->deconfigureModification(thisMapper);
                Array<int> resets = thisMapper->resets.getUnchecked(item2Type);
                for (int i = 0; i < resets.size(); i++)
                {
                    if (resets[i] == item2Id) thisMapper->resets.getUnchecked(item2Type).remove(item2Id);
                }
                processor.currentPiano->remove(thisMapper);
            }
        }
        else
        {
            int Id = item2Id;
            
            if (connect)
            {
                thisMapper->addTarget(Id);
                processor.currentPiano->configureModification(thisMapper);
                
                if (!reconfigure && item1Type == PreparationTypeGenericMod)    item1->setType(getModType(item2Type), true);
                else                                                           item1->setType(getModType(item2Type), false);
            }
            else
            {
                processor.currentPiano->deconfigureModification(thisMapper);
                thisMapper->clearTargets();
                
                if (!reconfigure && !thisMapper->getTargets().size())
                {
                    item1->setType(PreparationTypeGenericMod, false);
                    item1->setId(-1);
                }
            }
        }
        
    }
    else if (item1Type <= PreparationTypeTempo && (item2Type == PreparationTypeGenericMod || item2Type == PreparationTypeReset || (item2Type >= PreparationTypeDirectMod && item2Type <= PreparationTypeTempoMod)))
    {
        ItemMapper::Ptr thisMapper = item2->getMapper();
        
        BKPreparationType mapperType = (item2Type == PreparationTypeReset) ? PreparationTypeReset : item1Type;
        
        if (mapperType == PreparationTypeReset)
        {
            if (connect)
            {
                Array<int> reset = thisMapper->resets.getUnchecked(item1Type);
                reset.addIfNotAlreadyThere(item1Id);
                thisMapper->resets.set(item1Type, reset);
                
                DBG("resets: " + arrayIntArrayToString(thisMapper->resets));
                processor.currentPiano->configureModification(thisMapper);
                
                processor.currentPiano->add(thisMapper);
            }
            else
            {
                processor.currentPiano->deconfigureModification(thisMapper);
                Array<int> resets = thisMapper->resets.getUnchecked(item1Type);
                for (int i = 0; i < resets.size(); i++)
                {
                    if (resets[i] == item1Id) thisMapper->resets.getUnchecked(item1Type).remove(item1Id);
                }
                processor.currentPiano->remove(thisMapper);
            }
        }
        else
        {
            int Id = item1Id;
            
            if (connect)
            {
                thisMapper->addTarget(Id);
                processor.currentPiano->configureModification(thisMapper);
                
                if (!reconfigure && item2Type == PreparationTypeGenericMod)    item2->setType(getModType(item1Type), true);
                else                                                           item2->setType(getModType(item1Type), false);
            }
            else
            {
                processor.currentPiano->deconfigureModification(thisMapper);
                thisMapper->clearTargets();
                
                if (!reconfigure && !thisMapper->getTargets().size())
                {
                    item2->setType(PreparationTypeGenericMod, false);
                    item2->setId(-1);
                }
            }
        }
    }
    else if (item1Type == PreparationTypeKeymap &&
            (item2Type == PreparationTypeGenericMod || item2Type == PreparationTypeReset || (item2Type >= PreparationTypeDirectMod && item2Type <= PreparationTypeTempoMod)))
    {
        ItemMapper::Ptr thisMapper = item2->getMapper();
        
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
    else if (item2Type == PreparationTypeKeymap &&
            (item1Type == PreparationTypeGenericMod || item1Type == PreparationTypeReset|| (item1Type >= PreparationTypeDirectMod && item1Type <= PreparationTypeTempoMod)))
    {
        ItemMapper::Ptr thisMapper = item1->getMapper();
        
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
    route(false, true, item1, item2);
    route(true, true, item1, item2);
    
    print();
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
    item1->removeConnection(item2->getType(), item2->getId());
    item2->removeConnection(item1->getType(), item1->getId());
}

void BKItemGraph::connectUI(BKItem* item1, BKItem* item2)
{
    item1->addConnection(item2->getType(), item2->getId());
    item2->addConnection(item1->getType(), item1->getId());
}

void BKItemGraph::connect(BKItem* item1, BKItem* item2)
{
    route(true, false, item1, item2);
    
    print();
}

void BKItemGraph::connectWithoutCreatingNew(BKItem* item1, BKItem* item2)
{
    route(true, true, item1, item2);
    
    print();
}

void BKItemGraph::disconnect(BKItem* item1, BKItem* item2)
{
    route(false, false, item1, item2);
    
    print();
}

void BKItemGraph::disconnect(BKItem* item1, BKItem* item2)
{
    route(false, false, item1, item2);
    
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
    
    processor.updateState->clearActive();
    
    Piano::Ptr thisPiano = processor.currentPiano;
    
    int pmapcount = 0;
    DBG("PMAPZ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~~ ");
    for (auto pmap : thisPiano->getPreparationMaps())
    {
        preparations.clear();
        
        pmap->print();
        // Keymap
        int keymapId = pmap->getKeymapId();
        
        DBG("---PMAP"+String(pmapcount++));
        DBG("    keymap"+String(keymapId));
        
        BKItem* keymap;
        BKItem* newPreparation;
        
        keymap = itemWithTypeAndId(PreparationTypeKeymap, keymapId);
        
        if (keymap == nullptr) keymap = new BKItem(PreparationTypeKeymap, keymapId, processor);

        if (!contains(keymap))
        {
            add(keymap);
            
            for (auto p : pmap->getTuning())
            {
                int Id = p->getId();
                
                DBG("    tuning"+String(Id));
                
                newPreparation = itemWithTypeAndId(PreparationTypeTuning, Id);
                
                if (newPreparation == nullptr) newPreparation = new BKItem(PreparationTypeTuning, Id, processor);
                
                preparations.add(newPreparation);
                
                if (!contains(newPreparation))
                {
                    add(newPreparation);
                }
            }
            
            for (auto p : pmap->getTempo())
            {
                int Id = p->getId();
                
                DBG("    tempo"+String(Id));
                
                newPreparation = itemWithTypeAndId(PreparationTypeTempo, Id);
                
                if (newPreparation == nullptr) newPreparation = new BKItem(PreparationTypeTempo, Id, processor);
                
                preparations.add(newPreparation);
                
                if (!contains(newPreparation))
                {
                    add(newPreparation);
                }
            }
            
            for (auto p : pmap->getDirect())
            {
                int Id = p->getId();
                
                DBG("    direct"+String(Id));
                
                newPreparation = itemWithTypeAndId(PreparationTypeDirect, Id);
                
                if (newPreparation == nullptr) newPreparation = new BKItem(PreparationTypeDirect, Id, processor);
                
                preparations.add(newPreparation);
                
                if (!contains(newPreparation))
                {
                    add(newPreparation);
                    
                    int tuningId = p->getTuningId();
                    
                    BKItem* thisTuning = itemWithTypeAndId(PreparationTypeTuning, tuningId);
                    
                    if (thisTuning == nullptr) thisTuning = new BKItem(PreparationTypeTuning, tuningId, processor);
                    
                    thisTuning->setTopLeftPosition(10, 10);
                    
                    if (!contains(thisTuning))  add(thisTuning);
                    
                    connectUI(newPreparation, thisTuning);
                }
                
            }
            
            for (auto p : pmap->getSynchronic())
            {
                int Id = p->getId();
                
                newPreparation = itemWithTypeAndId(PreparationTypeSynchronic, Id);
                
                if (newPreparation == nullptr) newPreparation = new BKItem(PreparationTypeSynchronic, Id, processor);
                
                preparations.add(newPreparation);
                
                if (!contains(newPreparation))
                {
                    add(newPreparation);
                    
                    // TUNING
                    int tuningId = p->getTuningId();
                    
                    BKItem* thisTuning = itemWithTypeAndId(PreparationTypeTuning, tuningId);
                    
                    if (thisTuning == nullptr) thisTuning = new BKItem(PreparationTypeTuning, tuningId, processor);
                    
                    thisTuning->setTopLeftPosition(10, 10);
                    
                    if (!contains(thisTuning))  add(thisTuning);
                    
                    connectUI(newPreparation, thisTuning);
                    
                    
                    // TEMPO
                    int tempoId = p->getTempoId();
                    
                    BKItem* thisTempo = itemWithTypeAndId(PreparationTypeTempo, tempoId);
                    
                    if (thisTempo == nullptr)
                    {
                        thisTempo = new BKItem(PreparationTypeTempo, tempoId, processor);
                    }
                    
                    thisTempo->setTopLeftPosition(10, 10);
                    
                    if (!contains(thisTempo))  add(thisTempo);
                    
                    connectUI(newPreparation, thisTempo);
                }
                
                
            }
            
            for (auto p : pmap->getNostalgic())
            {
                int Id = p->getId();
                
                DBG("    nostalgic"+String(Id));
                
                newPreparation = itemWithTypeAndId(PreparationTypeNostalgic, Id);
                
                if (newPreparation == nullptr) newPreparation = new BKItem(PreparationTypeNostalgic, Id, processor);
                
                preparations.add(newPreparation);
                
                if (!contains(newPreparation))
                {
                    add(newPreparation);
                    
                    // TUNING
                    int tuningId = p->getTuningId();
                    
                    BKItem* thisTuning = itemWithTypeAndId(PreparationTypeTuning, tuningId);
                    
                    if (thisTuning == nullptr) thisTuning = new BKItem(PreparationTypeTuning, Id, processor);
                    
                    thisTuning->setTopLeftPosition(10, 10);
                    
                    if (!contains(thisTuning))  add(thisTuning);
                    
                    connectUI(newPreparation, thisTuning);
                    
                    
                    // SYNC TARGET
                    int syncId = p->getSynchronicTargetId();
                    
                    BKItem* thisSyncTarget = itemWithTypeAndId(PreparationTypeSynchronic, syncId);
                    
                    if (thisSyncTarget == nullptr) thisSyncTarget = new BKItem(PreparationTypeSynchronic, syncId, processor);
                    
                    thisSyncTarget->setTopLeftPosition(10, 10);
                    
                    if (!contains(thisSyncTarget))  add(thisSyncTarget);
                    
                    connectUI(newPreparation, thisSyncTarget);
                }
                
                
            }
            
            for (auto p : preparations)
            {
                p->print();
                connectUI(keymap, p);
            }
        }
    }

    DBG("MAPPER COUNT: " + String(thisPiano->getMappers().size()));
        
    for (auto map : thisPiano->getMappers())
    {
        
        BKPreparationType modType = map->getType();
        int Id = map->getId();
        Array<int> targetIds = map->getTargets();
        Array<int> keymaps = map->getKeymaps();
        int piano = map->piano;
        
        Array< Array<int>> resets = map->resets;
        
        BKPreparationType itemType = (modType == PreparationTypeReset || modType == PreparationTypePianoMap) ? modType : getModType(modType);
        
        BKItem* thisMod = itemWithTypeAndId(itemType, Id);
        
        if (thisMod == nullptr) thisMod = new BKItem(itemType, Id, processor);
        
        thisMod->setSelectedPianoId(piano);
        
        thisMod->setMapper(map);
        
        
        
        if (!contains(thisMod))
        {
            add(thisMod);
            
            if (thisMod->getType() == PreparationTypePianoMap) DBG("HOLY SHIT ADDING PIANO!");
            
            for (auto k : keymaps)
            {
                BKItem* thisKeymap = itemWithTypeAndId(PreparationTypeKeymap, k);
                
                if (thisKeymap == nullptr) thisKeymap = new BKItem(PreparationTypeKeymap, k, processor);
                
                if (!contains(thisKeymap)) add(thisKeymap);
                
                connectUI(thisKeymap, thisMod);
            }
            
            if (map->getType() >= PreparationTypeDirect && map->getType() <= PreparationTypeTempo)
            {
                for (auto t : targetIds)
                {
                    BKItem* thisTarget;
                    
                    thisTarget = itemWithTypeAndId(modType, t);
                    
                    if (thisTarget == nullptr) thisTarget = new BKItem(getModType(modType), t, processor);
                    
                    if (!contains(thisTarget)) add(thisTarget);
                    
                    connectUI(thisTarget, thisMod);
                }
            }
            else if (map->getType() == PreparationTypeReset)
            {
                for (int rtype = 0; rtype < 5; rtype++)
                {
                    Array<int> theseResets = map->resets.getUnchecked(rtype);
                    
                    for (auto t : theseResets)
                    {
                        BKItem* thisTarget;
                        
                        thisTarget = itemWithTypeAndId((BKPreparationType)rtype, t);
                        
                        if (thisTarget == nullptr) thisTarget = new BKItem((BKPreparationType)rtype, t, processor);
                        
                        if (!contains(thisTarget)) add(thisTarget);
                        
                        connectUI(thisTarget, thisMod);
                    }
                }
            }
        }
        
        
    }
    
}


