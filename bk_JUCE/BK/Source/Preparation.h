/*
  ==============================================================================

    Preparation.h
    Created: 2 Feb 2017 5:00:19pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef PREPARATION_H_INCLUDED
#define PREPARATION_H_INCLUDED

#include "Direct.h"
#include "Nostalgic.h"
#include "Synchronic.h"
#include "Tuning.h"


class Direct : public ReferenceCountedObject
{
    
public:
    typedef ReferenceCountedObjectPtr<Direct>   Ptr;
    typedef Array<Direct::Ptr>                  PtrArr;
    typedef Array<Direct::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<Direct>                  Arr;
    typedef OwnedArray<Direct, CriticalSection> CSArr;
   
    Direct(BKSynthesiser *s,
           BKSynthesiser *res,
           BKSynthesiser *ham,
           Tuning::Ptr tuning,
           int Id):
    Id(Id),
    synth(s),
    resonanceSynth(res),
    hammerSynth(ham)
    {
        sPrep = new DirectPreparation(tuning);
        aPrep = new DirectPreparation(sPrep);
        processor = new DirectProcessor(s, res, ham, sPrep, aPrep, Id);
    };
    
    ~Direct() {};
    
    inline int getId() {return Id;};
    
    DirectProcessor::Ptr        processor;
    DirectPreparation::Ptr      sPrep;
    DirectPreparation::Ptr      aPrep;
    
    
private:
    int Id;
    
    BKSynthesiser*              synth;
    BKSynthesiser*              resonanceSynth;
    BKSynthesiser*              hammerSynth;
    
    JUCE_LEAK_DETECTOR(Direct)
};

class Synchronic : public ReferenceCountedObject
{
    
public:
    typedef ReferenceCountedObjectPtr<Synchronic>   Ptr;
    typedef Array<Synchronic::Ptr>                  PtrArr;
    typedef Array<Synchronic::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<Synchronic>                  Arr;
    typedef OwnedArray<Synchronic, CriticalSection> CSArr;
   
    Synchronic(BKSynthesiser *s,
               Tuning::Ptr tuning,
           int Id):
    Id(Id)
    {
        sPrep = new SynchronicPreparation(tuning);
        aPrep = new SynchronicPreparation(sPrep);
        processor = new SynchronicProcessor(s, sPrep, aPrep, Id);
    };
    
    ~Synchronic() {};
    
    inline int getId() {return Id;};
    
    SynchronicProcessor::Ptr        processor;
    SynchronicPreparation::Ptr      sPrep;
    SynchronicPreparation::Ptr      aPrep;
    
    
private:
    int Id;
    
    
    
    JUCE_LEAK_DETECTOR(Synchronic)
};

class Nostalgic : public ReferenceCountedObject
{
    
public:
    typedef ReferenceCountedObjectPtr<Nostalgic>   Ptr;
    typedef Array<Nostalgic::Ptr>                  PtrArr;
    typedef Array<Nostalgic::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<Nostalgic>                  Arr;
    typedef OwnedArray<Nostalgic, CriticalSection> CSArr;
   
    Nostalgic(BKSynthesiser *s,
              Tuning::Ptr tuning,
           int Id):
    Id(Id)
    {
        sPrep = new NostalgicPreparation(tuning);
        aPrep = new NostalgicPreparation(sPrep);
        processor = new NostalgicProcessor(s, sPrep, aPrep, Id);
    };
    
    ~Nostalgic() {};
    
    inline int getId() {return Id;};
    
    NostalgicProcessor::Ptr        processor;
    NostalgicPreparation::Ptr      sPrep;
    NostalgicPreparation::Ptr      aPrep;
    
    
private:
    int Id;
    
    
    
    JUCE_LEAK_DETECTOR(Nostalgic)
};


#endif  // PREPARATION_H_INCLUDED
