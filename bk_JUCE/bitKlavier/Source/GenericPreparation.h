/*
  ==============================================================================

    GenericPreparation.h
    Created: 1 Dec 2022 11:37:52am
    Author:  Davis Polito

  ==============================================================================
*/

#pragma once
#include "BKUtilities.h"

class GenericPreparation : public ReferenceCountedObject
{

public:
    typedef ReferenceCountedObjectPtr<GenericPreparation>   Ptr;
    typedef Array<GenericPreparation::Ptr>                  PtrArr;
    typedef Array<GenericPreparation::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<GenericPreparation>                  Arr;
    typedef OwnedArray<GenericPreparation, CriticalSection> CSArr;
   
   
    GenericPreparation(BKPreparationType prep, int Id, bool random = false) :
    Id(Id),
    type(prep),
    name(cPreparationNames[prep] + " " + std::to_string(Id)),
    prepvTag(cPreparationvTags[prep]),
    soundSet(-1),
    useGlobalSoundSet(true),
    soundSetName(String()),
    defaultGain(0.5, true)
    {
        if (random) randomize();
    }
    
    
    GenericPreparation(BKPreparationType prep, int Id, String name, bool random = false):
    Id(Id),
    name(name),
    type(prep),
    prepvTag(cPreparationvTags[type]),
    soundSet(-1),
    useGlobalSoundSet(true),
    soundSetName(String()),
    defaultGain(0.5, true)
    {
        if (random) randomize();
    }
    
    
    GenericPreparation(BKPreparationType prep, int Id, String name, float defaultGain, bool random = false):
    Id(Id),
    name(name),
    type(prep),
    prepvTag(cPreparationvTags[type]),
    soundSet(-1),
    useGlobalSoundSet(true),
    soundSetName(String()),
    defaultGain(defaultGain, true)
    {
        if (random) randomize();
    }
    
   
    
    
    virtual GenericPreparation::Ptr duplicate(){};
    
    virtual ValueTree getState(){};//bool active = false)
    
    virtual void setState(XmlElement* e){};
    
    ~GenericPreparation() {};
    
    inline int getId() {return Id;}
    inline void setId(int newId) { Id = newId;}

    
  
    
    inline String getName(void) const noexcept {return name;}
    inline void setName(String newName)
    {
        name = newName;
    }
    
    const BKPreparationType getType()
    {
        return type;
    }

    virtual bool compare(GenericPreparation::Ptr from){};

    virtual void copy(GenericPreparation::Ptr from)
    {
        Id = from->getId();
        type = from->getType();
        name = String(cPreparationNames[type]) + " " + String(Id);
        prepvTag = cPreparationvTags[type];
        soundSet = from->soundSet;
        useGlobalSoundSet = from->useGlobalSoundSet;
        soundSetName = from->soundSetName;
        defaultGain = from->defaultGain;
    }

    // for unit-testing
    virtual void randomize(){};

    //virtual ValueTree getState(bool active = false);

    virtual void resetModdables(){};
    
    virtual void stepModdables(){};
    
    inline int getSoundSet() { return useGlobalSoundSet.value ? -1 : soundSet.value; }
    inline float* getDefaultGainPtr() { return &defaultGain.value; }
    inline void setSoundSet(int Id) {soundSet = Id;}
    Moddable<int> soundSet;
    Moddable<bool> useGlobalSoundSet;
    Moddable<String> soundSetName;
    Moddable<float> defaultGain;
   inline float getDefGain()
   {
       return defaultGain.value;
   }
    
    inline void setDefGain(float val)
    {
        defaultGain = val;
    }
    
    virtual void clear(){};
private:
    int Id;
    String name;
    BKPreparationType type;
    String prepvTag;
    JUCE_LEAK_DETECTOR(GenericPreparation)
};
