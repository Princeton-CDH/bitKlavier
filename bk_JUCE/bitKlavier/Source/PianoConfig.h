/*
  ==============================================================================

    PianoConfig.h
    Created: 20 Apr 2017 11:46:08am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef PIANOCONFIG_H_INCLUDED
#define PIANOCONFIG_H_INCLUDED

#include "BKUtilities.h"

class ItemConfiguration : public ReferenceCountedObject
{
public:
    typedef ReferenceCountedObjectPtr<ItemConfiguration>   Ptr;
    typedef ReferenceCountedArray<ItemConfiguration>        RCArr;
    typedef Array<ItemConfiguration::Ptr>                  PtrArr;
    typedef Array<ItemConfiguration::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<ItemConfiguration>                  Arr;
    typedef OwnedArray<ItemConfiguration, CriticalSection> CSArr;
    
    ItemConfiguration(BKPreparationType type, int Id):
    type(type),
    Id(Id)
    {
        
    }
    
    ItemConfiguration(BKPreparationType type, int Id, int X, int Y):
    type(type),
    Id(Id)
    {
        setXY(X,Y);
    }
    
    ~ItemConfiguration()
    {
        
    }
    
    inline void setXY(int x, int y) { X= x; Y=y; }
    inline int getX(void) { return X; }
    inline int getY(void) { return Y; }
    inline BKPreparationType getType(void) const noexcept { return type; }
    inline int getId(void) const noexcept {return Id; }
    inline void setId(int Id){Id = Id; }
    
private:
    BKPreparationType type;
    int Id;
    int X,Y;
    
    
    JUCE_LEAK_DETECTOR( ItemConfiguration )
};

class PianoConfiguration : public ReferenceCountedObject
{
public:
    
    typedef ReferenceCountedObjectPtr<PianoConfiguration>   Ptr;
    typedef ReferenceCountedArray<PianoConfiguration>        RCArr;
    typedef Array<PianoConfiguration::Ptr>                  PtrArr;
    typedef Array<PianoConfiguration::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<PianoConfiguration>                  Arr;
    typedef OwnedArray<PianoConfiguration, CriticalSection> CSArr;
    
    PianoConfiguration(void)
    {
        
    }
    
    ~PianoConfiguration(void)
    {
        
    }
    inline void clear(void)
    {
        items.clear();
    }
    
    inline void setItemXY(BKPreparationType type, int Id, int x, int y)
    {
        for (auto item : items)
        {
            if (item->getType() == type && item->getId() == Id)
            {
                item->setXY(x,y);
                break;
            }
        }
    }
    
    inline void addItem(BKPreparationType type, int Id)
    {
        if (type == BKPreparationTypeNil) return;
        
        if (!contains(type,Id)) items.add(new ItemConfiguration(type, Id));
    }
    
    inline void addItem(BKPreparationType type, int Id, int X, int Y)
    {
        if (!contains(type,Id)) items.add(new ItemConfiguration(type, Id, X, Y));
    }
    
    inline bool contains(BKPreparationType type, int Id)
    {
        for (auto item : items)
        {
            if (item->getType() == type && item->getId() == Id)
            {
                return true;
            }
        }
        
        return false;
    }
    
    inline void removeItem(BKPreparationType type, int Id)
    {
        for (int i = items.size(); --i >= 0; )
        {
            if (items[i]->getType() == type && items[i]->getId() == Id)
            {
                items.remove(i);
                break;
            }
        }
    }
    
    inline void setIdOfItem(BKPreparationType type, int oldId, int newId)
    {
        for (auto item : items)
        {
            if (item->getType() == type && item->getId() == oldId)
            {
                item->setId(newId);
                break;
            }
        }
    }
    
    inline int getX(BKPreparationType type, int Id)
    {
        for (auto item : items)
        {
            if (item->getType() == type && item->getId() == Id)
            {
                return item->getX();
            }
        }
        
        return 50;
    }
    
    inline int getY(BKPreparationType type, int Id)
    {
        for (auto item : items)
        {
            if (item->getType() == type && item->getId() == Id)
            {
                return item->getY();
            }
        }
        
        return 50;
    }
    
    inline Point<int> getXY(BKPreparationType type, int Id)
    {
        for (auto item : items)
        {
            if (item->getType() == type && item->getId() == Id)
            {
                return Point<int>(item->getX(), item->getY());
            }
        }
        
        return Point<int>(50,50);
    }
    
    
    inline void print(void)
    {
        DBG("PIANO CONFIG: ");
        for (auto item : items)
        {
            DBG(String(item->getType()) + " " + String(item->getId()) + " " + String(item->getX()) + " " + String(item->getY()));
        }
    }
    
    
    inline void setState(XmlElement* e)
    {
        clear();
        
        int itemCount = 0;
        forEachXmlChildElement (*e, item)
        {
            if (item->hasTagName( "item" + String(itemCount++)))
            {
                BKPreparationType type  = (BKPreparationType) item->getStringAttribute("type").getIntValue();
                int Id = item->getStringAttribute("Id").getIntValue();
                int X = item->getStringAttribute("X").getIntValue();
                int Y = item->getStringAttribute("Y").getIntValue();
                
                DBG("X: " + String(X) + " Y: " + String(Y));
                
                addItem(type,Id, X, Y);
            }
            else
            {
                break;
            }
        }
        
        print();
    }
    
    inline ValueTree getState(void)
    {
        ValueTree configVT("configuration");
        
        int itemCount = 0;
        for (auto item : items)
        {
            ValueTree itemVT("item"+String(itemCount++));
            
            itemVT.setProperty("type", item->getType(), 0);
            itemVT.setProperty("Id", item->getId(), 0);
            itemVT.setProperty("X", item->getX(), 0);
            itemVT.setProperty("Y", item->getY(), 0);
            
            configVT.addChild(itemVT, -1, 0);
        }
        
        return configVT;
    }
    
private:
    ItemConfiguration::PtrArr items;
    
    JUCE_LEAK_DETECTOR( PianoConfiguration )
    
};



#endif  // PIANOCONFIG_H_INCLUDED
