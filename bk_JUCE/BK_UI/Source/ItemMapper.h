/*
  ==============================================================================

    ItemMapper.h
    Created: 18 Jul 2017 5:08:33pm
    Author:  airship

  ==============================================================================
*/

#pragma once

#include "BKUtilities.h"

#include "BKGraph.h"

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
        DBG("ITEM type: " + String(type)  + " Id: " + String(Id));
    }
    
    inline int getId(void) const noexcept { return Id; }
    inline void setId(int newId) { Id = newId; }
    
    inline BKPreparationType getType(void) const noexcept { return type; }
    inline void setType(BKPreparationType newType) { type = newType; }
    
    inline void setItemName(String s) { name = s; }
    inline String getItemName(void) const noexcept { return name;}
    
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
    
    bool active;
    bool editted;
    
private:
    
    JUCE_LEAK_DETECTOR(ItemMapper);
};

