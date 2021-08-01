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

	void addBlendronicModification(BlendronicModification::Ptr b);
	void removeBlendronicModification(BlendronicModification::Ptr b);
	void removeBlendronicModification(int which);
    
    void addResonanceModification(ResonanceModification::Ptr b);
    void removeResonanceModification(ResonanceModification::Ptr b);
    void removeResonanceModification(int which);

    SynchronicModification::PtrArr getSynchronicModifications(void);
    
    NostalgicModification::PtrArr getNostalgicModifications(void);
    
    DirectModification::PtrArr getDirectModifications(void);
    
    TuningModification::PtrArr getTuningModifications(void);
    
    TempoModification::PtrArr getTempoModifications(void);

	BlendronicModification::PtrArr getBlendronicModifications(void);

    
    void clearModifications(void);
    void clearResets(void);
    
    struct Reset
    {
        int prepId;
        Array<int> keymapIds;
    };
    Array<Reset> directResets;
    Array<Reset> nostalgicResets;
    Array<Reset> synchronicResets;
    Array<Reset> tuningResets;
    Array<Reset> tempoResets;
	Array<Reset> blendronicResets;
    Array<Reset> resonanceResets;
    
    Array<Reset> directModResets;
    Array<Reset> nostalgicModResets;
    Array<Reset> synchronicModResets;
    Array<Reset> tuningModResets;
    Array<Reset> tempoModResets;
    Array<Reset> blendronicModResets;
    Array<Reset> resonanceModResets;
    
    struct PianoMap
    {
        int pianoTarget;
        Keymap::PtrArr keymaps;
    };
    Array<PianoMap>             pianoMaps;
    
private:
    DirectModification::PtrArr      directMods;
    SynchronicModification::PtrArr  synchronicMods;
    NostalgicModification::PtrArr   nostalgicMods;
    TuningModification::PtrArr      tuningMods;
    TempoModification::PtrArr       tempoMods;
	BlendronicModification::PtrArr	blendronicMods;
    ResonanceModification::PtrArr   resonanceMods;
    
    JUCE_LEAK_DETECTOR(Modifications)
};


#endif  // DirectModPrepMap_H_INCLUDED
