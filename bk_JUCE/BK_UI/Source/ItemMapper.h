/*
  ==============================================================================

    ItemMapper.h
    Created: 18 Jul 2017 5:08:33pm
    Author:  airship

  ==============================================================================
*/

#pragma once

#include "BKUtilities.h"

class ItemMapper : public ReferenceCountedObject
{
public:
    
    typedef ReferenceCountedObjectPtr<ItemMapper>   Ptr;
    typedef Array<ItemMapper::Ptr>                  PtrArr;
    
    ItemMapper(BKPreparationType type, int Id):
    type(type),
    Id(Id),
    editted(false)
    {
    
    }
    
    void print(void)
    {
        DBG("~ ~ ~ ~ ~ ~ ~ MAPPER ~ ~ ~ ~ ~ ~ ~ ~");
        DBG("type: " + String(type)  + "\nId: " + String(Id) + "\nConnections: ");
        for (auto item : connections) DBG("ctype: " + String(item->getType()) + " cId: " + String(item->getId()));
        DBG("~ ~ ~ ~ ~ ~ ~ ~ ~~ ~ ~ ~ ~ ~ ~ ~ ~ ~");
    }
    
    inline int getId(void) const noexcept { return Id; }
    inline void setId(int newId) { Id = newId; }
    
    inline BKPreparationType getType(void) const noexcept { return type; }
    inline void setType(BKPreparationType newType) { type = newType; }
    
    inline void setConnections(ItemMapper::PtrArr newConnections)
    {
        connections = newConnections;
    }
    
    inline bool addConnection(ItemMapper::Ptr thisItem)
    {
        bool added = connections.addIfNotAlreadyThere(thisItem);
        
        return added;
    }
    
    inline void addConnections(ItemMapper::PtrArr theseItems)
    {
        for (auto item : theseItems) connections.addIfNotAlreadyThere(item);
    }
    
    
    inline void removeConnection(BKPreparationType type, int Id)
    {
        for (int i = connections.size(); --i >= 0;)
        {
            if ((connections.getUnchecked(i)->getType() == type) && (connections.getUnchecked(i)->getId() == Id))
            {
                connections.remove(i);
                break;
            }
        }
    }
    
    inline void removeConnection(ItemMapper::Ptr thisItem)
    {
        int index = 0;
        for (auto item : connections)
        {
            if (item == thisItem) connections.remove(index);
            
            index++;
        }
    }
    
    inline bool isConnectedTo(BKPreparationType type, int Id)
    {
        for (auto item : connections)
        {
            if (item->getType() == type && item->getId() == Id)
            {
                return true;
            }
        }
        
        return false;
    }
    
    inline bool isConnectedTo(ItemMapper::Ptr thisItem)
    {
        for (auto item : connections)
        {
            if ((item->getType() == thisItem->getType()) && (item->getId() == thisItem->getId()))
            {
                return true;
            }
        }
        
        return false;
    }
    
    inline void changeIdOfConnection(BKPreparationType type, int oldId, int newId)
    {
        for (auto item : connections)
        {
            if (item->getType() == type && item->getId() == oldId)
            {
                item->setId(newId);
                break;
            }
        }
    }
    
    inline bool isConnectedToAnyPreparation(void)
    {
        ItemMapper::PtrArr theseItems;
        
        for (auto item : connections)
        {
            if (item->getType() >= PreparationTypeDirect && item->getType() <= PreparationTypeTempo)
            {
                return true;
            }
        }
        return false;
    }
    
    inline ItemMapper::PtrArr getConnectionsOfType(BKPreparationType type)
    {
        ItemMapper::PtrArr theseItems;
        
        for (auto item : connections)
        {
            if (item->getType() == type) theseItems.add(item);
        }
        
        return theseItems;
    }
    
    inline Array<int> getConnectionIdsOfType(BKPreparationType type)
    {
        Array<int> theseItems;
        
        for (auto item : connections)
        {
            if (item->getType() == type) theseItems.add(item->getId());
        }
        
        return theseItems;
    }
    
    inline ItemMapper::PtrArr getConnections(void) const noexcept { return connections; }
    
    inline Array<Array<int>> getConnectionIds(void)
    {
        Array<Array<int>> connectionIds;
        
        for (int i = 0; i < BKPreparationTypeNil; i++)
        {
            Array<int> theseItems = getConnectionIdsOfType((BKPreparationType)i);
            connectionIds.add(theseItems);
        }
        return connectionIds;
    }
    
    
    inline String connectionsToString(void)
    {
        String s = "";
        for (int type = 0; type < BKPreparationTypeNil; type++)
        {
            ItemMapper::PtrArr connex = getConnectionsOfType((BKPreparationType)type);
            
            s += cPreparationTypes[type]+":";
            
            for (auto item : connex)
            {
                s += " " + String(item->getId());
            }
        }
        return s;
    }
    

    inline void clearConnections(void)
    {
        connections.clear();
    }
    
    inline void clearConnectionsOfType(BKPreparationType type)
    {
        for (int i = connections.size(); --i >= 0;)
        {
            if (connections.getUnchecked(i)->getType() == type) connections.remove(i);
        }
    }
    
    inline void setItemName(String s) { name = s; }
    inline String getItemName(void) const noexcept { return name;}
    
    // CONFIGURATION
    inline int retrieveX(void) const noexcept { return bounds.getX(); };
    inline int retrieveY(void) const noexcept { return bounds.getY(); };
    
    inline Point<int> retrieveXY(void) { return bounds.getPosition(); };
    inline int retrieveWidth(void) { return bounds.getWidth(); }
    inline int retrieveHeight(void) { return bounds.getHeight(); }
    
    inline void saveBounds(Rectangle<int> newBounds) { bounds = newBounds; }
    inline Rectangle<int> retrieveBounds(void) const noexcept { return bounds; }
    
    // ACTIVE
    inline bool isActive(void) const noexcept {return active;}
    inline void setActive(bool a) { active = a; }
    
    
    // EDITTED
    inline bool getEditted(void) const noexcept { return editted;}
    inline void setEditted(bool e) { editted = e; }
    
    inline void setPianoTarget(int target) { pianoTarget = target; }
    inline int getPianoTarget(void) const noexcept { return pianoTarget; }
    
protected:
    BKPreparationType type;
    int Id;
    String name;
    
    int pianoTarget;
    
    ItemMapper::PtrArr connections;
    
    Rectangle<int> bounds;
    
    bool active;
    bool editted;
    
private:
    
    JUCE_LEAK_DETECTOR(ItemMapper);
};
