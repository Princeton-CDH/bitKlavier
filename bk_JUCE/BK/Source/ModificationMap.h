/*
  ==============================================================================

    ModificationMap.h
    Created: 31 Jan 2017 3:09:23pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef MODIFICATIONMAP_H_INCLUDED
#define MODIFICATIONMAP_H_INCLUDED

#include "BKUtilities.h"

#include "Modification.h"

class ModificationMap : public ReferenceCountedObject
{
public:
    ModificationMap(void);
    ~ModificationMap(void);
    typedef ReferenceCountedObjectPtr<ModificationMap>   Ptr;
    typedef Array<ModificationMap::Ptr>                  PtrArr;
    typedef Array<ModificationMap::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<ModificationMap>                  Arr;
    typedef OwnedArray<ModificationMap, CriticalSection> CSArr;
    
private:
    Modification::PtrArr    modifications;
    
    JUCE_LEAK_DETECTOR(ModificationMap)
};


#endif  // MODIFICATIONMAP_H_INCLUDED
