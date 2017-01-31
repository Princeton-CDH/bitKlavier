/*
  ==============================================================================

    Piano.h
    Created: 7 Dec 2016 10:25:40am
    Author:  Michael R Mulshine
 
    A "Piano" is an array of PreparationsMaps
    (equivalent to a "Preset" in the original bitKlavier)

  ==============================================================================
*/

#ifndef Piano_H_INCLUDED
#define Piano_H_INCLUDED

#include "BKUtilities.h"

#include "PreparationMap.h"

#include "Keymap.h"


class Piano : public ReferenceCountedObject
{
public:
    typedef ReferenceCountedObjectPtr<Piano>   Ptr;
    typedef Array<Piano::Ptr>                  PtrArr;
    typedef Array<Piano::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<Piano>                  Arr;
    typedef OwnedArray<Piano, CriticalSection> CSArr;
    
    Piano(BKSynthesiser *s,
          BKSynthesiser *res,
          BKSynthesiser *ham,
          Keymap::Ptr keymap,
          int Id);
    ~Piano();
    
    void storeCurrentPiano();
    
    void recallCurrentPiano();
    
    inline GeneralSettings::Ptr                getGeneralSettings(void)            {   return general;         }
    
    inline void setGeneralSettings(GeneralSettings::Ptr g)                          {   general = g;            }
    

    ValueTree*  getPianoValueTree(void);
    
    inline void setId(int Id) { Id = Id; }
    
    inline int getId(void) { return Id; }
    
    inline PreparationMap::CSPtrArr getPreparationMaps(void) { return prepMaps; }
    
    
    PreparationMap::Ptr         currentPMap;
    PreparationMap::CSPtrArr    activePMaps;
    PreparationMap::CSPtrArr    prepMaps;
    
    int numPMaps;
    
    void prepareToPlay(double sampleRate);
    
    int addPreparationMap(void);
    int removeLastPreparationMap(void);
    
private:
    int Id;
    
    double sampleRate;

    GeneralSettings::Ptr general;
    
    // Pointers to synths (flown in from BKAudioProcessor)
    BKSynthesiser*                  synth;
    BKSynthesiser*                  resonanceSynth;
    BKSynthesiser*                  hammerSynth;
    Keymap::Ptr                     initialKeymap;
    
    ValueTree vt;

    
    JUCE_LEAK_DETECTOR(Piano)
};


#endif  // Piano_H_INCLUDED
