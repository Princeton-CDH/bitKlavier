/*
  ==============================================================================

    GenericPreparation.h
    Created: 1 Dec 2022 11:37:52am
    Author:  Davis Polito

  ==============================================================================
*/

#pragma once
#include "BKUtilities.h"

#include "BKSynthesiser.h"

class GenericPreparation : public ReferenceCountedObject
{

public:
    typedef ReferenceCountedObjectPtr<GenericPreparation>   Ptr;
    typedef Array<GenericPreparation::Ptr>                  PtrArr;
    typedef Array<GenericPreparation::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<GenericPreparation>                  Arr;
    typedef OwnedArray<GenericPreparation, CriticalSection> CSArr;
    
    GenericPreparation(BKPreparationType prep, int* _id, String* name, bool random = false) :
        type(prep),
        objId(_id),
        objName(name)
    {
        
        if(random) randomize();
    }
    

    virtual GenericPreparation::Ptr duplicate();

    virtual bool compare(GenericPreparation::Ptr from);

    virtual void copy(GenericPreparation::Ptr from);

    // for unit-testing
    virtual void randomize();

    virtual ValueTree getState(bool active = false);

    virtual void resetModdables();
    
    virtual void stepModdables();
    
    virtual void setState(XmlElement* e);

    ~GenericPreparation() {};

    const BKPreparationType getType()
    {
        return type;
    }
    
    const int* objId;
    const String* objName;
private:
   
    const BKPreparationType type;

    JUCE_LEAK_DETECTOR(GenericPreparation)
};
