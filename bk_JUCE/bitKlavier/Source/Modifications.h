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


    void addDirectModification(DirectModification::Ptr m);
    void removeDirectModification(DirectModification::Ptr m);
    void removeDirectModification(int which);
    
    void addSynchronicModification(SynchronicModification::Ptr m);
    void removeSynchronicModification(SynchronicModification::Ptr m);
    void removeSynchronicModification(int which);
    
    void addNostalgicModification(NostalgicModification::Ptr m);
    void removeNostalgicModification(NostalgicModification::Ptr m);
    void removeNostalgicModification(int which);
    
    void addTempoModification(TempoModification::Ptr m);
    void removeTempoModification(TempoModification::Ptr m);
    void removeTempoModification(int which);
    
    void addTuningModification(TuningModification::Ptr m);
    void removeTuningModification(TuningModification::Ptr m);
    void removeTuningModification(int which);

	void addBlendronomerModification(BlendronomerModification::Ptr b);
	void removeBlendronomerModification(BlendronomerModification::Ptr b);
	void removeBlendronomerModification(int which);

    SynchronicModification::PtrArr getSynchronicModifications(void);
    
    NostalgicModification::PtrArr getNostalgicModifications(void);
    
    DirectModification::PtrArr getDirectModifications(void);
    
    TuningModification::PtrArr getTuningModifications(void);
    
    TempoModification::PtrArr getTempoModifications(void);

	BlendronomerModification::PtrArr getBlendronomerModifications(void);

    
    void clearModifications(void);
    void clearResets(void);
    
    Array<int> directReset;
    Array<int> nostalgicReset;
    Array<int> synchronicReset;
    Array<int> tuningReset;
    Array<int> tempoReset;
	Array<int> blendronomerReset;
    
private:
    DirectModification::PtrArr      directMods;
    SynchronicModification::PtrArr  synchronicMods;
    NostalgicModification::PtrArr   nostalgicMods;
    TuningModification::PtrArr      tuningMods;
    TempoModification::PtrArr       tempoMods;
	BlendronomerModification::PtrArr	blendronomerMods;
    
    JUCE_LEAK_DETECTOR(Modifications)
};


#endif  // DirectModPrepMap_H_INCLUDED
