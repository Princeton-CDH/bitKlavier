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
    typedef ReferenceCountedObjectPtr<ModificationMap>   Ptr;
    typedef Array<ModificationMap::Ptr>                  PtrArr;
    typedef Array<ModificationMap::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<ModificationMap>                  Arr;
    typedef OwnedArray<ModificationMap, CriticalSection> CSArr;
  
    ModificationMap(void);
    ~ModificationMap(void);
    
    void addSynchronicModification(SynchronicModification::Ptr m);

    void addNostalgicModification(NostalgicModification::Ptr m);

    void addDirectModification(DirectModification::Ptr m);
    
    void addTuningModification(TuningModification::Ptr m);
    
    void addTempoModification(TempoModification::Ptr m);
    
    SynchronicModification::PtrArr getSynchronicModifications(void);
    
    NostalgicModification::PtrArr getNostalgicModifications(void);
    
    DirectModification::PtrArr getDirectModifications(void);
    
    TuningModification::PtrArr getTuningModifications(void);
    
    TempoModification::PtrArr getTempoModifications(void);
    
    String  stringRepresentation(void);
    
    void clearModifications(void);
    
private:
    
    DirectModification::PtrArr      directMods;
    SynchronicModification::PtrArr  synchronicMods;
    NostalgicModification::PtrArr   nostalgicMods;
    TuningModification::PtrArr      tuningMods;
    TempoModification::PtrArr       tempoMods;
    
    JUCE_LEAK_DETECTOR(ModificationMap)
};


#endif  // MODIFICATIONMAP_H_INCLUDED
