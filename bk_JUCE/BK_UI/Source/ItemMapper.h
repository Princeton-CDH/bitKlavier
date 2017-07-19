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
        for (int i = 0; i < BKPreparationTypeNil; i++) connections.add(Array<int>());
    }
    
    void print(void)
    {
        DBG("~ ~ ~ ~ ~ ~ ~ MAPPER ~ ~ ~ ~ ~ ~ ~ ~");
        DBG("type: " + String(type)  + "\nId: " + "\nConnections: " + arrayIntArrayToString(connections));
        DBG("~ ~ ~ ~ ~ ~ ~ ~ ~~ ~ ~ ~ ~ ~ ~ ~ ~ ~");
    }
    
    inline int getId(void) const noexcept { return Id; }
    inline void setId(int newId) { Id = newId; }
    
    inline BKPreparationType getType(void) const noexcept { return type; }
    inline void setType(BKPreparationType newType) { type = newType; }
    
    inline void setConnections(BKPreparationType type, Array<int> connex)
    {
        connections.set(type, connex);
    }
    
    inline void setAllConnections(Array<Array<int>> newConnections)
    {
        connections = newConnections;
    }
    
    inline void addConnection(BKPreparationType type, int Id)
    {
        Array<int> connex = connections.getUnchecked(type);
        
        connex.add(Id);
        
        connections.set(type, connex);
    }
    
    inline void removeConnection(BKPreparationType type, int Id)
    {
        Array<int> connex = getConnections(type);
        
        for (int i = 0; i < connex.size(); i++)
        {
            if (connex[i] == Id)
            {
                connex.remove(i);
                break;
            }
        }
        
        connections.set(type, connex);
    }
    
    inline bool isConnectedTo(BKPreparationType type, int Id)
    {
        Array<int> connex = getConnections(type);
        
        for (int i = 0; i < connex.size(); i++)
        {
            if (connex[i] == Id)
            {
                return true;
            }
        }
        
        return false;
    }
    
    inline void changeIdOfConnection(BKPreparationType type, int oldId, int newId)
    {
        Array<int> connex = getConnections(type);
        
        for (int i = 0; i < connex.size(); i++)
        {
            if (connex[i] == oldId)
            {
                connex.set(i, newId);
                break;
            }
        }
    }
    
    inline Array<int> getConnections(BKPreparationType type)
    {
        return connections.getUnchecked(type);
    }
    
    inline Array<Array<int>> getAllConnections(void) const noexcept { return connections; }
    
    inline void clearAllConnections(void)
    {
        for (int i = 0; i < BKPreparationTypeNil; i++)
        {
            connections.getUnchecked(i).clear();
        }
    }
    
    inline void clearConnections(BKPreparationType type)
    {
        connections.getUnchecked(type).clear();
    }
    
    inline void setName(String s) { name = s; }
    inline String getName(void) const noexcept { return name;}
    
    inline void setX(int x) { X = x; };
    inline void setY(int y) { Y = y; };
    
    inline int getX(void) const noexcept { return X; };
    inline int getY(void) const noexcept { return Y; };
    
    inline void setXY(int x, int y) { X=x; Y=y;}
    
    inline void setXY(Point<int> xy) { X=xy.x; Y=xy.y;}
    
    bool editted;
    
protected:
    BKPreparationType type;
    int Id;
    String name;
    
    Array<Array<int>> connections;
    
    int X,Y;
    
private:
    
    JUCE_LEAK_DETECTOR(ItemMapper);
};
