/*
  ==============================================================================

    GenericObject.h
    Created: 7 Dec 2022 11:25:41am
    Author:  Davis Polito

  ==============================================================================
*/

#pragma once
#include "GenericPreparation.h"
#include "BKUtilities.h"
#include "AudioConstants.h"
class GenericObject : public ReferenceCountedObject
{
    
public:
    typedef ReferenceCountedObjectPtr<GenericObject>   Ptr;
    typedef Array<GenericObject::Ptr>                  PtrArr;
    typedef Array<GenericObject::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<GenericObject>                  Arr;
    typedef OwnedArray<GenericObject, CriticalSection> CSArr;
    
    
    GenericObject(GenericPreparation::Ptr d,
           int Id, bool random = false):
    prep(d),
    Id(Id),
    name(String(cPreparationNames[prep->getType()]) + " " + String(Id))
    {
        if (random) randomize();
    }
    
    GenericObject()
    {
        
    }
    
    GenericObject(GenericPreparation::Ptr d,
           int Id, String name, bool random = false):
    prep(d),
    Id(Id),
    name(name)
    {
        if (random) randomize();
    }
    
    GenericObject(GenericPreparation::Ptr d,
           XmlElement* e):
    prep(d)
    {
        setState(e);
    }
    
    
    virtual GenericPreparation::Ptr duplicate();
    
    inline ValueTree getState()//bool active = false)
    {
        ValueTree vt(vtagDirect);
        
        vt.setProperty( "Id",Id, 0);
        vt.setProperty( "name",                          name, 0);
        
        vt.addChild(prep->getState(), -1, 0);
        
        return vt;
    }
    
    inline void setState(XmlElement* e)
    {
        Id = e->getStringAttribute("Id").getIntValue();
        
        String n = e->getStringAttribute("name");
        
        if (n != String())     name = n;
        else                   name = String(Id);
        
        
        XmlElement* params = e->getChildByName("params");
        
        if (params != nullptr)
        {
            prep->setState(params);
        }
        else
        {
            prep->setState(e);
        }
    }
    
    ~GenericObject() {};
    
    inline int getId() {return Id;}
    inline void setId(int newId) { Id = newId;}
    
    GenericPreparation::Ptr      prep;
    
    inline void copy(GenericObject::Ptr from)
    {
        prep->copy(from->prep);
    }
    
    inline void randomize()
    {
        Random::getSystemRandom().setSeedRandomly();
        prep->randomize();
        Id = Random::getSystemRandom().nextInt(Range<int>(1, 1000));
        name = "random";
    }
    
    inline String getName(void) const noexcept {return name;}
    inline void setName(String newName)
    {
        name = newName;
    }
    
    const BKPreparationType getType()
    {
        return prep->getType();
    }
    
private:
    int Id;
    String name;
    
    JUCE_LEAK_DETECTOR(GenericObject)
};
