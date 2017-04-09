/*
  ==============================================================================

    Modifications.h
    Created: 31 Jan 2017 3:09:23pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef DirectModificationMap_H_INCLUDED
#define DirectModificationMap_H_INCLUDED

#include "BKUtilities.h"

#include "Modification.h"

class ModPrepMap : public ReferenceCountedObject
{
public:
    typedef ReferenceCountedObjectPtr<ModPrepMap>   Ptr;
    typedef Array<ModPrepMap::Ptr>                  PtrArr;
    typedef Array<ModPrepMap::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<ModPrepMap>                  Arr;
    typedef OwnedArray<ModPrepMap, CriticalSection> CSArr;
    
    ModPrepMap(BKPreparationType type, int Id):
    type(type),
    Id(Id),
    preparations(Array<int>())
    {
        
        
    }
    
    ModPrepMap(BKPreparationType type, int Id, Array<int> preps):
    type(type),
    Id(Id),
    preparations(preps)
    {
        
        
    }
    
    
    ~ModPrepMap(void)
    {
        
    }
    
    inline const BKPreparationType getType(void)                    {   return type;                   }
    inline const int getId(void)                    {   return Id;                   }
    inline const Array<int> getPreparations(void)   {   return preparations;        }
    inline const void addPreparation(int prepId)    {   preparations.add(prepId);   }
    inline const void clearPreparations(int prepId) {   preparations.clear();   }
    
private:
    BKPreparationType type;
    int Id;
    Array<int> preparations;
    
    
    JUCE_LEAK_DETECTOR(ModPrepMap);
};



class ModificationMap : public ReferenceCountedObject
{
public:
    typedef ReferenceCountedObjectPtr<ModificationMap>   Ptr;
    typedef Array<ModificationMap::Ptr>                  PtrArr;
    typedef Array<ModificationMap::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<ModificationMap>                  Arr;
    typedef OwnedArray<ModificationMap, CriticalSection> CSArr;
    
    ModificationMap(void)
    {
    
    }
    
    ~ModificationMap(void)
    {
        
    }
    
    inline void addModPrepMap(ModPrepMap::Ptr mpmap)
    {
        modPrepMaps.add(mpmap);
    }
    
    inline void removeModPrepMap(ModPrepMap::Ptr mpmap)
    {
        for (int i = modPrepMaps.size(); --i >= 0;)
        {
            if (modPrepMaps[i] == mpmap)
            {
                modPrepMaps.remove(i);
                break;
            }
        }
    }
    
    inline void removeModPrepMap(BKPreparationType type, int Id)
    {
        for (int i = modPrepMaps.size(); --i >= 0;)
        {
            if ((modPrepMaps[i]->getType() == type) && (modPrepMaps[i]->getId() == Id))
            {
                modPrepMaps.remove(i);
                break;
            }
        }
    }
    
    inline ModPrepMap::Ptr getModPrepMap(BKPreparationType type, int Id)
    {
        ModPrepMap::Ptr thisModPrepMap;
        
        for (int i = modPrepMaps.size(); --i >= 0;)
        {
            if ((modPrepMaps[i]->getType() == type) && (modPrepMaps[i]->getId() == Id))
            {
                thisModPrepMap == modPrepMaps[i];
                break;
            }
        }
        
        return thisModPrepMap;
    }
    
    inline ModPrepMap::PtrArr  getModPrepMaps(void) const noexcept
    {
        return modPrepMaps;
    }
    
    void clear(void) { modPrepMaps.clear();}
    
private:
    ModPrepMap::PtrArr modPrepMaps;
    
    
    JUCE_LEAK_DETECTOR(ModificationMap);
};





class Modifications : public ReferenceCountedObject
{
public:
    typedef ReferenceCountedObjectPtr<Modifications>   Ptr;
    typedef Array<Modifications::Ptr>                  PtrArr;
    typedef Array<Modifications::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<Modifications>                  Arr;
    typedef OwnedArray<Modifications, CriticalSection> CSArr;
  
    Modifications(void);
    ~Modifications(void);
    
    void addSynchronicModification(SynchronicModification::Ptr m);

    void addNostalgicModification(NostalgicModification::Ptr m);
    
    void addDirectModification(DirectModification::Ptr m);
    void removeDirectModification(DirectModification::Ptr m);
    void removeDirectModification(int which);
    
    void addTuningModification(TuningModification::Ptr m);
    
    void addTempoModification(TempoModification::Ptr m);

    SynchronicModification::PtrArr getSynchronicModifications(void);
    
    NostalgicModification::PtrArr getNostalgicModifications(void);
    
    DirectModification::PtrArr getDirectModifications(void);
    
    TuningModification::PtrArr getTuningModifications(void);
    
    TempoModification::PtrArr getTempoModifications(void);
    
    String  stringRepresentation(void);
    
    void clearModifications(void);
    void clearResets(void);
    
    Array<int> directReset;
    Array<int> nostalgicReset;
    Array<int> synchronicReset;
    Array<int> tuningReset;
    Array<int> tempoReset;
    
private:
    DirectModification::PtrArr      directMods;
    SynchronicModification::PtrArr  synchronicMods;
    NostalgicModification::PtrArr   nostalgicMods;
    TuningModification::PtrArr      tuningMods;
    TempoModification::PtrArr       tempoMods;
    
    
    JUCE_LEAK_DETECTOR(Modifications)
};


#endif  // DirectModPrepMap_H_INCLUDED
