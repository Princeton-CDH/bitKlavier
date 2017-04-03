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
#include "Preparation.h"

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
    
    void processBlock(int numSamples, int midiChannel);
    
    void keyPressed(int noteNumber, float velocity, int channel);
    void keyReleased(int noteNumber, float velocity, int channel);
    void postRelease(int noteNumber, float velocity, int channel);
    
    void setKeymap(Keymap::Ptr km);
    inline Keymap::Ptr getKeymap()              { return pKeymap; }
    inline int getKeymapId()                    { if (pKeymap) return pKeymap->getId(); else return 0; }
    
    inline String getPreparationIds()
    {
        String prep = "";
        for (auto p : synchronic)
        {
            prep.append("S",1);
            prep.append(String(p->getId()), 3);
            prep.append(" ",1);
        }
        
        for (auto p : nostalgic)
        {
            prep.append("N",1);
            prep.append(String(p->getId()), 3);
            prep.append(" ",1);
        }
        
        for (auto p : direct)
        {
            prep.append("D",1);
            prep.append(String(p->getId()), 3);
            prep.append(" ",1);
        }
        
        for (auto p : tempo)
        {
            prep.append("M",1);
            prep.append(String(p->getId()), 3);
            prep.append(" ",1);
        }
        
        for (auto p : tuning)
        {
            prep.append("T",1);
            prep.append(String(p->getId()), 3);
            prep.append(" ",1);
        }
        
        if (prep == "")
            return " ";
        
        return prep;
        
    }
    
    void setSynchronic  (Synchronic::PtrArr);
    void setNostalgic   (Nostalgic::PtrArr);
    void setDirect      (Direct::PtrArr);
    void setTempo       (Tempo::PtrArr);
    void setTuning      (Tuning::PtrArr);
    
    void addDirect(Direct::Ptr p);
    void addNostalgic(Nostalgic::Ptr p);
    void addSynchronic(Synchronic::Ptr p);
    void addTempo(Tempo::Ptr p);
    void addTuning(Tuning::Ptr p);
    
    Synchronic::PtrArr  getSynchronic(void);
    Nostalgic::PtrArr   getNostalgic(void);
    Direct::PtrArr      getDirect(void);
    Tempo::PtrArr       getTempo(void);
    Tuning::PtrArr      getTuning(void);

    void deactivateIfNecessary();
    
    void removeAllPreparations();
    
    bool isActive;
    
    void print(void)
    {
        DBG("PrepMapId: " + String(Id));
    }
    
private:
    int Id;
    
    // Keymap for this PreparationMap (one per PreparationMap)
    Keymap::Ptr                 pKeymap;
    
    Synchronic::PtrArr          synchronic;
    Nostalgic::PtrArr           nostalgic;
    Direct::PtrArr              direct;
    Tempo::PtrArr               tempo;
    Tuning::PtrArr              tuning;
    
    // Pointers to synths (flown in from BKAudioProcessor)
    BKSynthesiser*              synth;
    BKSynthesiser*              resonanceSynth;
    BKSynthesiser*              hammerSynth;
    
    double                      sampleRate;
    
    
    JUCE_LEAK_DETECTOR(PreparationMap)
};


#endif  // PreparationMap_H_INCLUDED
