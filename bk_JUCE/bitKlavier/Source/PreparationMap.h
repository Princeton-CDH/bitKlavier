/*
  ==============================================================================

    PreparationMap.h
    Created: 8 Dec 2016 12:59:46am
    Author:  Michael R Mulshine
 
    A "PreparationsMap" is a Keymap connected to one or more Preparations of any type

  ==============================================================================
*/

#ifndef PreparationMap_H_INCLUDED
#define PreparationMap_H_INCLUDED

#include "BKUtilities.h"
#include "Keymap.h"

#include "Synchronic.h"
#include "Direct.h"
#include "Nostalgic.h"
#include "Tempo.h"
#include "Tuning.h"

class PreparationMap : public ReferenceCountedObject
{
public:
    typedef ReferenceCountedObjectPtr<PreparationMap>    Ptr;
    typedef Array<PreparationMap::Ptr>                   PtrArr;
    typedef Array<PreparationMap::Ptr, CriticalSection>  CSPtrArr;
    typedef OwnedArray<PreparationMap>                   Arr;
    typedef OwnedArray<PreparationMap, CriticalSection>  CSArr;
    
    PreparationMap(Keymap::Ptr keymap,
                   int Id);
    ~PreparationMap();
    
    void prepareToPlay (double sampleRate);
    
    inline void setId(int val)         { Id = val; print();   }
    inline int getId(void)             { return Id;           }
    
    void processBlock(int numSamples, int midiChannel, bool onlyNostalgic = false);
    
    void keyPressed(int noteNumber, float velocity, int channel);
    void keyReleased(int noteNumber, float velocity, int channel);
    void postRelease(int noteNumber, float velocity, int channel);
    void sustainPedalPressed()  { sustainPedalIsDepressed = true;  }
    void sustainPedalReleased();
    
    void setKeymap(Keymap::Ptr km);
    inline Keymap::Ptr getKeymap()              { return pKeymap; }
    inline int getKeymapId()                    { if (pKeymap) return pKeymap->getId(); else return 0; }
    
    inline String getPreparationIds()
    {
        String prep = "";
        for (auto p : sprocessor)
        {
            prep.append("S",1);
            prep.append(String(p->getId()), 3);
            prep.append(" ",1);
        }
        
        for (auto p : nprocessor)
        {
            prep.append("N",1);
            prep.append(String(p->getId()), 3);
            prep.append(" ",1);
        }
        
        for (auto p : dprocessor)
        {
            prep.append("D",1);
            prep.append(String(p->getId()), 3);
            prep.append(" ",1);
        }
        
        for (auto p : mprocessor)
        {
            prep.append("M",1);
            prep.append(String(p->getId()), 3);
            prep.append(" ",1);
        }
        
        for (auto p : tprocessor)
        {
            prep.append("T",1);
            prep.append(String(p->getId()), 3);
            prep.append(" ",1);
        }
        
        if (prep == "")
            return " ";
        
        return prep;
        
    }
    
    void                        addDirectProcessor      (DirectProcessor::Ptr );
    void                        setDirectProcessors     (DirectProcessor::PtrArr);
    DirectProcessor::PtrArr     getDirectProcessors     (void);
    DirectProcessor::Ptr        getDirectProcessor      (int Id);
    bool                        contains                (DirectProcessor::Ptr);
    
    void                        addNostalgicProcessor   (NostalgicProcessor::Ptr );
    void                        setNostalgicProcessors  (NostalgicProcessor::PtrArr);
    NostalgicProcessor::PtrArr  getNostalgicProcessors  (void);
    NostalgicProcessor::Ptr     getNostalgicProcessor   (int Id);
    bool                        contains                (NostalgicProcessor::Ptr);
    
    void                        addSynchronicProcessor  (SynchronicProcessor::Ptr );
    void                        setSynchronicProcessors (SynchronicProcessor::PtrArr);
    SynchronicProcessor::PtrArr getSynchronicProcessors (void);
    SynchronicProcessor::Ptr    getSynchronicProcessor  (int Id);
    bool                        contains                (SynchronicProcessor::Ptr);
    
    void                        addTuningProcessor      (TuningProcessor::Ptr );
    void                        setTuningProcessors     (TuningProcessor::PtrArr);
    TuningProcessor::PtrArr     getTuningProcessors     (void);
    TuningProcessor::Ptr        getTuningProcessor      (int Id);
    bool                        contains                (TuningProcessor::Ptr);
    
    void                        addTempoProcessor       (TempoProcessor::Ptr );
    void                        setTempoProcessors      (TempoProcessor::PtrArr);
    TempoProcessor::PtrArr      getTempoProcessors      (void);
    TempoProcessor::Ptr         getTempoProcessor       (int Id);
    bool                        contains                (TempoProcessor::Ptr);
    

    void deactivateIfNecessary();
    
    bool isActive;
    
    void print(void)
    {
        DBG("PrepMapId: " + String(Id));
        DBG("Keymap: " + String(pKeymap->getId()));
        
        Array<int> ps;
        for (auto p : sprocessor) ps.add(p->getId());
        DBG("Synchronic: " + intArrayToString(ps));
        
        ps.clear();
        for (auto p : nprocessor) ps.add(p->getId());
        DBG("Nostalgic: " + intArrayToString(ps));
        
        ps.clear();
        for (auto p : dprocessor) ps.add(p->getId());
        DBG("Direct: " + intArrayToString(ps));
        
        ps.clear();
        for (auto p : tprocessor) ps.add(p->getId());
        DBG("Tuning: " + intArrayToString(ps));
        
        ps.clear();
        for (auto p : mprocessor) ps.add(p->getId());
        DBG("Tempo: " + intArrayToString(ps));
    }
    
private:
    int Id;
    
    // Keymap for this PreparationMap (one per PreparationMap)
    Keymap::Ptr                 pKeymap;
    
    DirectProcessor::PtrArr              dprocessor;
    SynchronicProcessor::PtrArr          sprocessor;
    NostalgicProcessor::PtrArr           nprocessor;
    TempoProcessor::PtrArr               mprocessor;
    TuningProcessor::PtrArr              tprocessor;
    
    // Pointers to synths (flown in from BKAudioProcessor)
    BKSynthesiser*              synth;
    BKSynthesiser*              resonanceSynth;
    BKSynthesiser*              hammerSynth;
    
    double                      sampleRate;
    
    bool sustainPedalIsDepressed;

    struct SustainedNote
    {
        int noteNumber;
        float velocity;
        int channel;
    };
    Array<SustainedNote> sustainedNotes;
    
    
    JUCE_LEAK_DETECTOR(PreparationMap)
};


#endif  // PreparationMap_H_INCLUDED
