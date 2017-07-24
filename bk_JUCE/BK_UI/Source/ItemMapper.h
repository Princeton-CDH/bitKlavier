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
        for (auto item : connections) item->print();
        DBG("~ ~ ~ ~ ~ ~ ~ ~ ~~ ~ ~ ~ ~ ~ ~ ~ ~ ~");
    }
    
    inline int getId(void) const noexcept { return Id; }
    inline void setId(int newId) { Id = newId; }
    
    inline BKPreparationType getType(void) const noexcept { return type; }
    inline void setType(BKPreparationType newType) { type = newType; }
    
    inline void setAllConnections(ItemMapper::PtrArr newConnections)
    {
        connections = newConnections;
    }
    
    inline bool addConnection(ItemMapper::Ptr thisItem)
    {
        bool added = connections.addIfNotAlreadyThere(thisItem);
        
        return added;
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
    
    inline ItemMapper::PtrArr getConnections(BKPreparationType type)
    {
        ItemMapper::PtrArr theseItems;
        
        for (auto item : connections)
        {
            if (item->getType() == type) theseItems.add(item);
        }
        
        return theseItems;
    }
    
    inline ItemMapper::PtrArr getAllConnections(void) const noexcept { return connections; }
    
    inline void clearAllConnections(void)
    {
        connections.clear();
    }
    
    inline void clearConnections(BKPreparationType type)
    {
        for (int i = connections.size(); --i >= 0;)
        {
            if (connections.getUnchecked(i)->getType() == type) connections.remove(i);
        }
    }
    
    inline void setItemName(String s) { name = s; }
    inline String getItemName(void) const noexcept { return name;}
    
    // CONFIGURATION
    inline void saveX(int x) { XY.x = x; };
    inline void saveY(int y) { XY.y = y; };
    
    inline int retrieveX(void) const noexcept { return XY.x; };
    inline int retrieveY(void) const noexcept { return XY.y; };
    
    inline Point<int> retrieveXY(void) const noexcept { return XY; };
    
    inline void saveXY(int x, int y) { XY.x = x; XY.y = y;}
    
    inline void saveXY(Point<int> xy) { XY.x = xy.x; XY.y = xy.y;}
    
    // ACTIVE
    bool getActive(void) const noexcept {return active;}
    void setActive(bool a) { active = a; }
    
    
    // EDITTED
    bool getEditted(void) const noexcept { return editted;}
    void setEditted(bool e) { editted = e; }
    
protected:
    BKPreparationType type;
    int Id;
    String name;
    
    ItemMapper::PtrArr connections;
    
    Point<int> XY;
    
    bool active;
    bool editted;
    
    
    
private:
    
    JUCE_LEAK_DETECTOR(ItemMapper);
};
