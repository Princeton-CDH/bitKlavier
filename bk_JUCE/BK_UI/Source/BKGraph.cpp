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

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ BKGraph ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
void BKItemGraph::add(BKItem* itemToAdd)
{
    items.add(itemToAdd);
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

void BKItemGraph::remove(BKItem* itemToRemove)
{
    for (auto item : itemToRemove->getConnections())
    {
        disconnect(itemToRemove, item);
    }
    
    items.removeObject(itemToRemove);
    
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

void BKItemGraph::linkPreparationWithKeymap(bool link, BKPreparationType thisType, int thisId, Keymap::Ptr thisKeymap)
{
    PreparationMap::Ptr thisPreparationMap = processor.currentPiano->getPreparationMapWithKeymap(thisKeymap);
    
    if (link && thisPreparationMap == nullptr)
    {
        int whichPMap = processor.currentPiano->addPreparationMap(thisKeymap);
        
        thisPreparationMap = processor.currentPiano->prepMaps[whichPMap];
    }
    
    
    if (thisType == PreparationTypeDirect)
    {
        Direct::Ptr thisDirect = processor.gallery->getDirect(thisId);
        
        if (link)   thisPreparationMap->addDirect(thisDirect);
        else        thisPreparationMap->removeDirect(thisDirect);
    }
    else if (thisType == PreparationTypeSynchronic)
    {
        Synchronic::Ptr thisSynchronic = processor.gallery->getSynchronic(thisId);
        
        if (link)   thisPreparationMap->addSynchronic(thisSynchronic);
        else        thisPreparationMap->removeSynchronic(thisSynchronic);
    }
    else if (thisType == PreparationTypeNostalgic)
    {
        Nostalgic::Ptr thisNostalgic = processor.gallery->getNostalgic(thisId);
        
        if (link)   thisPreparationMap->addNostalgic(thisNostalgic);
        else        thisPreparationMap->removeNostalgic(thisNostalgic);
    }
    else if (thisType == PreparationTypeTempo)
    {
        Tempo::Ptr thisTempo = processor.gallery->getTempo(thisId);
        
        if (link)   thisPreparationMap->addTempo(thisTempo);
        else        thisPreparationMap->removeTempo(thisTempo);
    }
    else if (thisType == PreparationTypeTuning)
    {
        Tuning::Ptr thisTuning = processor.gallery->getTuning(thisId);
        
        if (link)   thisPreparationMap->addTuning(thisTuning);
        else        thisPreparationMap->removeTuning(thisTuning);
    }
    
    processor.updateState->galleryDidChange = true;

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
    

    if (item1Type == PreparationTypeKeymap && item2Type <= PreparationTypeKeymap)
    {
        linkPreparationWithKeymap(connect, item2Type, item2Id, processor.gallery->getKeymap(item1Id));
    }
    else if (item1Type <= PreparationTypeKeymap && item2Type == PreparationTypeKeymap)
    {
        linkPreparationWithKeymap(connect, item1Type, item1Id, processor.gallery->getKeymap(item2Id));
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
    // SynchronicMod
    else if ((item1Type == PreparationTypeSynchronic && item2Type == PreparationTypeSynchronicMod) ||
             (item2Type == PreparationTypeSynchronic && item1Type == PreparationTypeSynchronicMod))
    {
        item2->addModification(item1);
        item1->addModification(item2);
    }
    else if ((item1Type == PreparationTypeKeymap && item2Type == PreparationTypeSynchronicMod) ||
        (item2Type == PreparationTypeKeymap && item1Type == PreparationTypeSynchronicMod))
    {
        Keymap::Ptr thisKeymap = processor.gallery->getKeymap( (item1Type == PreparationTypeKeymap) ?
                                                              item1Id : item2Id );
        SynchronicModPreparation::Ptr thisMod = processor.gallery->getSynchronicModPreparation((item1Type == PreparationTypeSynchronicMod) ?
                                                                                               item1Id : item2Id);

        processor.currentPiano->configureSynchronicModification(thisKeymap, thisMod,
                                                                getPreparationIds((item1Type == PreparationTypeSynchronicMod) ?
                                                                                  item1->getModifications() : item2->getModifications()));
    }
    // DirectMod
    else if ((item1Type == PreparationTypeDirect && item2Type == PreparationTypeDirectMod) ||
             (item2Type == PreparationTypeDirect && item1Type == PreparationTypeDirectMod))
    {
        DBG("SHOULD CONFIGURE MOD AND DIRECT");
        item2->addModification(item1);
        item1->addModification(item2);
    }
    else if (item1Type == PreparationTypeKeymap && item2Type == PreparationTypeDirectMod)
    {
        DBG("SHOULD CONFIGURE MOD WITH KEYMAP");
        Keymap::Ptr thisKeymap = processor.gallery->getKeymap(item1Id);
        
        DirectModPreparation::Ptr thisMod = processor.gallery->getDirectModPreparation(item2Id);
        
        processor.currentPiano->configureDirectModification(thisKeymap, thisMod, getPreparationIds(item2->getModifications()));
    }
    else if (item2Type == PreparationTypeKeymap && item1Type == PreparationTypeDirectMod)
    {
        DBG("SHOULD CONFIGURE MOD WITH KEYMAP");
        Keymap::Ptr thisKeymap = processor.gallery->getKeymap(item2Id);
        
        DirectModPreparation::Ptr thisMod = processor.gallery->getDirectModPreparation(item1Id);
        
        processor.currentPiano->configureDirectModification(thisKeymap, thisMod, getPreparationIds(item1->getModifications()));
    }
    
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


// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ BKItem ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
BKItem::BKItem(BKPreparationType type, int Id, BKAudioProcessor& p):
BKDraggableComponent(true,false,true),
processor(p),
type(type),
Id(Id)
{
    fullChild.setAlwaysOnTop(true);
    addAndMakeVisible(fullChild);
    
    addAndMakeVisible(label);
    
    label.setJustificationType(Justification::centred);
    label.setBorderSize(BorderSize<int>(2));
    
    String name = cPreparationTypes[type]+String(Id);
    
    label.setText(name, dontSendNotification);
    
    DBG("constructed: " + name);
    
}

BKItem::~BKItem()
{
    
}

void BKItem::itemIsBeingDragged(const MouseEvent& e)
{
    getParentComponent()->repaint();
}

void BKItem::mouseDown(const MouseEvent& e)
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
    
    processor.updateState->displayDidChange = true;
}

void BKItem::keyPressedWhileSelected(const KeyPress& e)
{
    
}

void BKItem::mouseUp(const MouseEvent& e)
{
    mouseExit(e);
}

void BKItem::paint(Graphics& g)
{
    if (type == PreparationTypeTuning)
    {
        g.setColour(Colours::lightcoral);
    }
    else if (type == PreparationTypeTempo)
    {
        g.setColour(Colours::palegreen);
    }
    else if (type == PreparationTypeKeymap)
    {
        g.setColour(Colours::lightyellow);
        
    }
    else if (type >= PreparationTypeKeymap) //mod
    {
        g.setColour(Colours::palevioletred);
    }
    else
    {
        g.setColour(Colours::lightblue);
    }
    
    
    g.fillAll();
    
    if (isSelected)
    {
        g.setColour(Colours::white);
        g.drawRect(getLocalBounds(),4);
    }
    else
    {
        g.setColour(Colours::black);
        g.drawRect(getLocalBounds(),2);
    }
    
    
}

void BKItem::resized(void)
{
    label.setBounds(0,0,getWidth(),getHeight());
    fullChild.setBounds(0,0,getWidth(),getHeight());
}


void BKItem::addModification(BKItem* modification)
{
    modifications.add(modification);
}

void BKItem::removeModification(BKItem* modification)
{
    int index = 0;
    for (auto item : modifications)
    {
        if (modification == item)
        {
            connections.remove(index);
            break;
        }
        index++;
    }
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
