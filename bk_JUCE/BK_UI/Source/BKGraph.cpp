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
Id(Id)
{
    fullChild.setAlwaysOnTop(true);
    addAndMakeVisible(fullChild);
    
    if (type == PreparationTypeTuning)
    {
        image = ImageCache::getFromFile(File("~/bK_icons/tuning_icon.png"));
    }
    else if (type == PreparationTypeTempo)
    {
        image = ImageCache::getFromFile(File("~/bK_icons/tempo_icon.png"));
    }
    else if (type == PreparationTypeSynchronic)
    {
        image = ImageCache::getFromFile(File("~/bK_icons/synchronic_icon.png"));
    }
    else if (type == PreparationTypeNostalgic)
    {
        image = ImageCache::getFromFile(File("~/bK_icons/nostalgic_icon.png"));
    }
    else if (type == PreparationTypeDirect)
    {
        image = ImageCache::getFromFile(File("~/bK_icons/direct_icon.png"));
    }
    else if (type == PreparationTypeKeymap)
    {
        image = ImageCache::getFromFile(File("~/bK_icons/keymap_icon.png"));
    }
    else if (type > PreparationTypeKeymap && type < PreparationTypePianoMap) //mod
    {
        image = ImageCache::getFromFile(File("~/bK_icons/mod_icon.png"));
    }
    else if (type == PreparationTypePianoMap)
    {
        image = ImageCache::getFromFile(File("~/bK_icons/piano_icon.png"));
        
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
        image = ImageCache::getFromFile(File("~/bK_icons/reset_icon.png"));
    }
    
    placement = RectanglePlacement::centred;
    

    int val = ((type > PreparationTypeKeymap && type < PreparationTypePianoMap) || type == PreparationTypeReset) ? 125 : ((type == PreparationTypeKeymap) ? 85 : 75);
    
    
    while (!(image.getWidth() < val || image.getHeight() < val))
    {
        DBG(String(image.getWidth()) +  " " + String(image.getHeight()));
        
       image = image.rescaled(image.getWidth() * 0.5, image.getHeight() * 0.5);
    }
    
    if (type != PreparationTypePianoMap)    setSize(image.getWidth(), image.getHeight());
    else                                    setSize(image.getWidth(), image.getHeight() + 25);
    
    //image.rescaled(image.getWidth() * .25, image.getHeight() * .25);
}

BKItem::~BKItem()
{
    
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
    name = itemToCopy->getName();
    position = itemToCopy->getPosition();
    type = itemToCopy->getType();
    Id = itemToCopy->getId();
    currentId = itemToCopy->getSelectedId();
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
            processor.updateState->currentPreparationDisplay = DisplayDirect;
        }
        else if (type == PreparationTypeSynchronic)
        {
            processor.updateState->currentSynchronicId = Id;
            processor.updateState->synchronicPreparationDidChange = true;
            processor.updateState->currentPreparationDisplay = DisplaySynchronic;
        }
        else if (type == PreparationTypeNostalgic)
        {
            processor.updateState->currentNostalgicId = Id;
            processor.updateState->nostalgicPreparationDidChange = true;
            processor.updateState->currentPreparationDisplay = DisplayNostalgic;
        }
        else if (type == PreparationTypeTuning)
        {
            processor.updateState->currentTuningId = Id;
            processor.updateState->tuningPreparationDidChange = true;
            processor.updateState->currentPreparationDisplay = DisplayTuning;
        }
        else if (type == PreparationTypeTempo)
        {
            processor.updateState->currentTempoId = Id;
            processor.updateState->tempoPreparationDidChange = true;
            processor.updateState->currentPreparationDisplay = DisplayTempo;
        }
        else if (type == PreparationTypeKeymap)
        {
            processor.updateState->currentKeymapId = Id;
            processor.updateState->keymapDidChange = true;
            processor.updateState->currentPreparationDisplay = DisplayKeymap;
        }
        else if (type == PreparationTypeDirectMod)
        {
            processor.updateState->currentModDirectId = Id;
            processor.updateState->directDidChange = true;
            processor.updateState->currentPreparationDisplay = DisplayDirect;
        }
        else if (type == PreparationTypeNostalgicMod)
        {
            processor.updateState->currentModNostalgicId = Id;
            processor.updateState->nostalgicPreparationDidChange = true;
            processor.updateState->currentPreparationDisplay = DisplayNostalgic;
        }
        else if (type == PreparationTypeSynchronicMod)
        {
            processor.updateState->currentModSynchronicId = Id;
            processor.updateState->synchronicPreparationDidChange = true;
            processor.updateState->currentPreparationDisplay = DisplaySynchronic;
        }
        else if (type == PreparationTypeTuningMod)
        {
            processor.updateState->currentModTuningId = Id;
            processor.updateState->tuningPreparationDidChange = true;
            processor.updateState->currentPreparationDisplay = DisplayTuning;
        }
        else if (type == PreparationTypeTempoMod)
        {
            processor.updateState->currentModTempoId = Id;
            processor.updateState->tempoPreparationDidChange = true;
            processor.updateState->currentPreparationDisplay = DisplayTempo;
        }
        else if (type == PreparationTypePianoMap)
        {
            menu.showPopup();
        }
    }
    
    processor.updateState->displayDidChange = true;
}

void BKItem::keyPressedWhileSelected(const KeyPress& e)
{
    
}

inline void BKItem::connectWith(BKItem* item)
{
    connections.add(item);
}

bool BKItem::isConnectedWith(BKItem* item)
{
    return connections.contains(item);
}

void BKItem::disconnectFrom(BKItem* toDisconnect)
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

void BKItemGraph::add(BKItem* itemToAdd)
{
    
    itemToAdd->addActionListener(this);
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
        
        item1->connectWith(item2);
        item2->connectWith(item1);
        
    }
    else // !connect
    {
        if (!item1->isConnectedWith(item2) && !item2->isConnectedWith(item1)) return;
        
        item1->disconnectFrom(item2);
        item2->disconnectFrom(item1);
    }
    
    
    
    // CONFIGURATIONS
    if (item1Type == PreparationTypeKeymap && item2Type == PreparationTypePianoMap)
    {
        Keymap::Ptr thisKeymap = processor.gallery->getKeymap(item1Id);
        int pianoId = item2->getSelectedId();
        
        if (connect)    processor.currentPiano->configurePianoMap(thisKeymap, pianoId);
        else            processor.currentPiano->deconfigurePianoMap(thisKeymap);
        
    }
    else if (item1Type == PreparationTypePianoMap && item2Type == PreparationTypeKeymap)
    {
        Keymap::Ptr thisKeymap = processor.gallery->getKeymap(item2Id);
        int pianoId = item1->getSelectedId();
        
        if (connect)    processor.currentPiano->configurePianoMap(thisKeymap, pianoId);
        else            processor.currentPiano->deconfigurePianoMap(thisKeymap);
    }
    else if (item1Type == PreparationTypeKeymap && item2Type == PreparationTypeReset)
    {
        ModificationMapper::Ptr thisMapper = processor.currentPiano->getMapper(PreparationTypeReset, item2Id);
        int keymapId = item1Id;
        
        if (connect)
        {
            thisMapper->addKeymap(keymapId);
            processor.currentPiano->configureModification(thisMapper);
        }
        else
        {
            processor.currentPiano->deconfigureModification(thisMapper);
            thisMapper->clearKeymaps();
        }
    }
    else if (item1Type == PreparationTypeReset && item2Type == PreparationTypeKeymap)
    {
        ModificationMapper::Ptr thisMapper = processor.currentPiano->getResetMapper(PreparationTypeReset, item1Id);
        int keymapId = item2Id;
        
        if (connect)
        {
            thisMapper->addKeymap(keymapId);
            processor.currentPiano->configureModification(thisMapper);
        }
        else
        {
            processor.currentPiano->deconfigureModification(thisMapper);
            thisMapper->clearKeymaps();
        }
    }
    else if (item1Type == PreparationTypeReset && item2Type < PreparationTypeKeymap)
    {
        ModificationMapper::Ptr thisMapper = processor.currentPiano->getMapper(item1Type, item1Id);
        
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
    }
    else if (item1Type < PreparationTypeKeymap && item2Type == PreparationTypeReset)
    {
        ModificationMapper::Ptr thisMapper = processor.currentPiano->getMapper(item2Type, item2Id);
        
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
    else if (item1Type == PreparationTypeKeymap && item2Type <= PreparationTypeKeymap)
    {
        if (connect)    addPreparationToKeymap(item2Type, item2Id, processor.gallery->getKeymap(item1Id));
        else
        {
            Keymap::Ptr thisKeymap = processor.gallery->getKeymap(item1Id);
            
            processor.currentPiano->removePreparationMapWithKeymap(thisKeymap);
        }
    }
    else if (item1Type <= PreparationTypeKeymap && item2Type == PreparationTypeKeymap)
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
    // Direct Modifications
    else if (item1Type == PreparationTypeDirect && item2Type == PreparationTypeDirectMod)
    {
        ModificationMapper::Ptr thisMapper = processor.currentPiano->getMapper(PreparationTypeDirectMod, item2Id);
        
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
    else if (item1Type == PreparationTypeDirectMod && item2Type == PreparationTypeDirect)
    {
        ModificationMapper::Ptr thisMapper = processor.currentPiano->getMapper(PreparationTypeDirectMod, item1Id);
        
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
    }
    else if (item1Type == PreparationTypeKeymap && item2Type == PreparationTypeDirectMod)
    {
        ModificationMapper::Ptr thisMapper = processor.currentPiano->getMapper(PreparationTypeDirectMod, item2Id);
        int keymapId = item1Id;
        
        if (connect)
        {
            thisMapper->addKeymap(keymapId);
            processor.currentPiano->configureModification(thisMapper);
        }
        else
        {
            processor.currentPiano->deconfigureModification(thisMapper);
            thisMapper->clearKeymaps();
        }
    }
    else if (item2Type == PreparationTypeKeymap && item1Type == PreparationTypeDirectMod)
    {
        ModificationMapper::Ptr thisMapper = processor.currentPiano->getMapper(PreparationTypeDirectMod, item1Id);
        int keymapId = item2Id;
        
        if (connect)
        {
            thisMapper->addKeymap(keymapId);
            processor.currentPiano->configureModification(thisMapper);
        }
        else
        {
            processor.currentPiano->deconfigureModification(thisMapper);
            thisMapper->clearKeymaps();
        }
    }
    // Nostalgic Modifications
    else if (item1Type == PreparationTypeNostalgic && item2Type == PreparationTypeNostalgicMod)
    {
        ModificationMapper::Ptr thisMapper = processor.currentPiano->getMapper(PreparationTypeNostalgicMod, item2Id);
        
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
    else if (item2Type == PreparationTypeNostalgic && item1Type == PreparationTypeNostalgicMod)
    {
        ModificationMapper::Ptr thisMapper = processor.currentPiano->getMapper(PreparationTypeNostalgicMod, item1Id);
        
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
    }
    else if (item1Type == PreparationTypeKeymap && item2Type == PreparationTypeNostalgicMod)
    {
        ModificationMapper::Ptr thisMapper = processor.currentPiano->getMapper(PreparationTypeNostalgicMod, item2Id);
        int keymapId = item1Id;
    
        if (connect)
        {
            thisMapper->addKeymap(keymapId);
            processor.currentPiano->configureModification(thisMapper);
        }
        else
        {
            processor.currentPiano->deconfigureModification(thisMapper);
            thisMapper->clearKeymaps();
        }
    }
    else if (item2Type == PreparationTypeKeymap && item1Type == PreparationTypeNostalgicMod)
    {
        ModificationMapper::Ptr thisMapper = processor.currentPiano->getMapper(PreparationTypeNostalgicMod, item1Id);
        int keymapId = item2Id;
        
        if (connect)
        {
            thisMapper->addKeymap(keymapId);
            processor.currentPiano->configureModification(thisMapper);
        }
        else
        {
            processor.currentPiano->deconfigureModification(thisMapper);
            thisMapper->clearKeymaps();
        }
    }
    // Synchronic Modifications
    else if (item1Type == PreparationTypeSynchronic && item2Type == PreparationTypeSynchronicMod)
    {
        ModificationMapper::Ptr thisMapper = processor.currentPiano->getMapper(PreparationTypeSynchronicMod, item2Id);
        
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
    else if (item2Type == PreparationTypeSynchronic && item1Type == PreparationTypeSynchronicMod)
    {
        ModificationMapper::Ptr thisMapper = processor.currentPiano->getMapper(PreparationTypeSynchronicMod, item1Id);
        
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
    }
    else if (item1Type == PreparationTypeKeymap && item2Type == PreparationTypeSynchronicMod)
    {
        ModificationMapper::Ptr thisMapper = processor.currentPiano->getMapper(PreparationTypeSynchronicMod, item2Id);
        int keymapId = item1Id;
        
        if (connect)
        {
            thisMapper->addKeymap(keymapId);
            processor.currentPiano->configureModification(thisMapper);
        }
        else
        {
            processor.currentPiano->deconfigureModification(thisMapper);
            thisMapper->clearKeymaps();
        }
    }
    else if (item2Type == PreparationTypeKeymap && item1Type == PreparationTypeSynchronicMod)
    {
        ModificationMapper::Ptr thisMapper = processor.currentPiano->getMapper(PreparationTypeSynchronicMod, item1Id);
        int keymapId = item2Id;
        
        if (connect)
        {
            thisMapper->addKeymap(keymapId);
            processor.currentPiano->configureModification(thisMapper);
        }
        else
        {
            processor.currentPiano->deconfigureModification(thisMapper);
            thisMapper->clearKeymaps();
        }
    }
    // Tuning Modifications
    else if (item1Type == PreparationTypeTuning && item2Type == PreparationTypeTuningMod)
    {
        ModificationMapper::Ptr thisMapper = processor.currentPiano->getMapper(PreparationTypeTuningMod, item2Id);
        
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
    else if (item2Type == PreparationTypeTuning && item1Type == PreparationTypeTuningMod)
    {
        ModificationMapper::Ptr thisMapper = processor.currentPiano->getMapper(PreparationTypeTuningMod, item1Id);
        
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
    }
    else if (item1Type == PreparationTypeKeymap && item2Type == PreparationTypeTuningMod)
    {
        ModificationMapper::Ptr thisMapper = processor.currentPiano->getMapper(PreparationTypeTuningMod, item2Id);
        int keymapId = item1Id;
        
        if (connect)
        {
            thisMapper->addKeymap(keymapId);
            processor.currentPiano->configureModification(thisMapper);
        }
        else
        {
            processor.currentPiano->deconfigureModification(thisMapper);
            thisMapper->clearKeymaps();
        }
    }
    else if (item2Type == PreparationTypeKeymap && item1Type == PreparationTypeTuningMod)
    {
        ModificationMapper::Ptr thisMapper = processor.currentPiano->getMapper(PreparationTypeTuningMod, item1Id);
        int keymapId = item2Id;
        
        if (connect)
        {
            thisMapper->addKeymap(keymapId);
            processor.currentPiano->configureModification(thisMapper);
        }
        else
        {
            processor.currentPiano->deconfigureModification(thisMapper);
            thisMapper->clearKeymaps();
        }
    }
    // Tempo Modifications
    else if (item1Type == PreparationTypeTempo && item2Type == PreparationTypeTempoMod)
    {
        ModificationMapper::Ptr thisMapper = processor.currentPiano->getMapper(PreparationTypeTempoMod, item2Id);
        
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
    else if (item2Type == PreparationTypeTempo && item1Type == PreparationTypeTempoMod)
    {
        ModificationMapper::Ptr thisMapper = processor.currentPiano->getMapper(PreparationTypeTempoMod, item1Id);
        
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
    }
    else if (item1Type == PreparationTypeKeymap && item2Type == PreparationTypeTempoMod)
    {
        ModificationMapper::Ptr thisMapper = processor.currentPiano->getMapper(PreparationTypeTempoMod, item2Id);
        int keymapId = item1Id;
        
        if (connect)
        {
            thisMapper->addKeymap(keymapId);
            processor.currentPiano->configureModification(thisMapper);
        }
        else
        {
            processor.currentPiano->deconfigureModification(thisMapper);
            thisMapper->clearKeymaps();
        }
    }
    else if (item2Type == PreparationTypeKeymap && item1Type == PreparationTypeTempoMod)
    {
        ModificationMapper::Ptr thisMapper = processor.currentPiano->getMapper(PreparationTypeTempoMod, item1Id);
        int keymapId = item2Id;
        
        if (connect)
        {
            thisMapper->addKeymap(keymapId);
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
        item1->disconnectFrom(item2);
        item2->disconnectFrom(item1);
    }
}



void BKItemGraph::reconnect(BKItem* item1, BKItem* item2)
{
    route(false, item1, item2);
    route(true, item1, item2);
    
    print();
}

void BKItemGraph::update(BKPreparationType type, int which)
{
    // Only applies to Keymaps and Modification types, so as to make sure that when ModPreparations are changed, so are the Modifications.
    // Also applies to Resets and PianoMaps.
    if (type >= PreparationTypeKeymap && type <= PreparationTypePianoMap) // All the mods
    {
        for (auto item : items)
        {
            if (item->getType() == type && item->getId() == which)
            {
                for (auto connection : item->getConnections())
                {
                    reconnect(item, connection);
                }
            }
        }
    }

}

void BKItemGraph::disconnectUI(BKItem* item1, BKItem* item2)
{
    item1->disconnectFrom(item2);
    item2->disconnectFrom(item1);
}

void BKItemGraph::connectUI(BKItem* item1, BKItem* item2)
{
    item1->connectWith(item2);
    item2->connectWith(item1);
#if 0
    BKPreparationType item1Type = item1->getType();
    int item1Id = item1->getId();
    
    BKPreparationType item2Type = item2->getType();
    int item2Id = item2->getId();
    
    
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
    
    item1->connectWith(item2);
    item2->connectWith(item1);


    
    // CONFIGURATIONS
    if (!(
          (item1Type == PreparationTypeKeymap && item2Type == PreparationTypePianoMap) ||
        (item1Type == PreparationTypePianoMap && item2Type == PreparationTypeKeymap) ||
        (item1Type == PreparationTypeKeymap && item2Type == PreparationTypeReset) ||
        (item1Type == PreparationTypeKeymap && item2Type == PreparationTypeReset) ||
        (item1Type == PreparationTypeReset && item2Type == PreparationTypeKeymap) ||
          (item1Type == PreparationTypeReset && item2Type < PreparationTypeKeymap) ||
          (item1Type < PreparationTypeKeymap && item2Type == PreparationTypeReset) ||
          (item1Type == PreparationTypeKeymap && item2Type <= PreparationTypeKeymap) ||
          (item1Type <= PreparationTypeKeymap && item2Type == PreparationTypeKeymap) ||
          (item1Type == PreparationTypeTuning && item2Type <= PreparationTypeNostalgic) ||
          (item1Type <= PreparationTypeNostalgic && item2Type == PreparationTypeTuning) ||
          (item1Type == PreparationTypeTempo && item2Type == PreparationTypeSynchronic) ||
          (item1Type == PreparationTypeSynchronic && item2Type == PreparationTypeTempo) ||
          (item1Type == PreparationTypeNostalgic && item2Type == PreparationTypeSynchronic) ||
          (item1Type == PreparationTypeSynchronic && item2Type == PreparationTypeNostalgic) ||
          (item1Type == PreparationTypeDirect && item2Type == PreparationTypeDirectMod) ||
          (item2Type == PreparationTypeDirect && item1Type == PreparationTypeDirectMod) ||
          (item1Type == PreparationTypeKeymap && item2Type == PreparationTypeDirectMod) ||
          (item2Type == PreparationTypeKeymap && item1Type == PreparationTypeDirectMod) ||
          (item1Type == PreparationTypeNostalgic && item2Type == PreparationTypeNostalgicMod) ||
          (item2Type == PreparationTypeNostalgic && item1Type == PreparationTypeNostalgicMod) ||
          (item1Type == PreparationTypeKeymap && item2Type == PreparationTypeNostalgicMod) ||
          (item2Type == PreparationTypeKeymap && item1Type == PreparationTypeNostalgicMod) ||
          (item1Type == PreparationTypeSynchronic && item2Type == PreparationTypeSynchronicMod) ||
          (item2Type == PreparationTypeSynchronic && item1Type == PreparationTypeSynchronicMod) ||
          (item1Type == PreparationTypeKeymap && item2Type == PreparationTypeSynchronicMod) ||
          (item2Type == PreparationTypeKeymap && item1Type == PreparationTypeSynchronicMod) ||
          (item1Type == PreparationTypeTuning && item2Type == PreparationTypeTuningMod) ||
          (item2Type == PreparationTypeTuning && item1Type == PreparationTypeTuningMod) ||
          (item1Type == PreparationTypeKeymap && item2Type == PreparationTypeTuningMod) ||
          (item2Type == PreparationTypeKeymap && item1Type == PreparationTypeTuningMod) ||
          (item1Type == PreparationTypeTempo && item2Type == PreparationTypeTempoMod) ||
          (item2Type == PreparationTypeTempo && item1Type == PreparationTypeTempoMod) ||
          (item1Type == PreparationTypeKeymap && item2Type == PreparationTypeTempoMod) ||
          (item2Type == PreparationTypeKeymap && item1Type == PreparationTypeTempoMod)
          ))
    {
        item1->disconnectFrom(item2);
        item2->disconnectFrom(item1);
    }
#endif
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
            
            synchronicItem->disconnectFrom(otherItem);
            otherItem->disconnectFrom(synchronicItem);
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
            
            nostalgicItem->disconnectFrom(otherItem);
            otherItem->disconnectFrom(nostalgicItem);
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
            
            directItem->disconnectFrom(otherItem);
            otherItem->disconnectFrom(directItem);
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
            
            synchronicItem->disconnectFrom(otherItem);
            otherItem->disconnectFrom(synchronicItem);
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
            
            nostalgicItem->disconnectFrom(otherItem);
            otherItem->disconnectFrom(nostalgicItem);
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

    for (auto map : thisPiano->getMappers())
    {
        BKPreparationType type = map->getType();
        int Id = map->getId();
        
        Array<int> keymaps = map->getKeymaps();
        Array<int> targets = map->getTargets();
        
        BKItem* thisMod = itemWithTypeAndId(type, Id);
        
        if (!contains(thisMod) && (keymaps.size() || targets.size())) add(thisMod);
        
        for (auto k : keymaps)
        {
            BKItem* thisKeymap = itemWithTypeAndId(PreparationTypeKeymap, k);
            if (!contains(thisKeymap)) add(thisKeymap);
            
            connectUI(thisKeymap, thisMod);
        }
        
        for (auto t : targets)
        {
            BKItem* thisTarget;
            
            if (type == PreparationTypeDirectMod)
            {
                thisTarget = itemWithTypeAndId(PreparationTypeDirect, t);
            }
            else if (type == PreparationTypeSynchronicMod)
            {
                thisTarget = itemWithTypeAndId(PreparationTypeSynchronic, t);
            }
            else if (type == PreparationTypeNostalgicMod)
            {
                thisTarget = itemWithTypeAndId(PreparationTypeNostalgic, t);
            }
            else if (type == PreparationTypeTempoMod)
            {
                thisTarget = itemWithTypeAndId(PreparationTypeTempo, t);
            }
            else if (type == PreparationTypeTuningMod)
            {
                thisTarget = itemWithTypeAndId(PreparationTypeTuning, t);
            }
            
            if (!contains(thisTarget)) add(thisTarget);
            
            connectUI(thisTarget, thisMod);
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


