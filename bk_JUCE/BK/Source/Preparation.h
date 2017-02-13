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
#include "General.h"


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
        processor = new DirectProcessor(s, res, ham, aPrep, Id);
    };
    
    inline ValueTree getState(void)
    {
        ValueTree prep( vtagDirectPrep+String(Id));
        
        prep.setProperty( ptagDirect_id,                Id, 0);
        prep.setProperty( ptagDirect_tuning,            sPrep->getTuning()->getId(), 0);
        prep.setProperty( ptagDirect_transposition,     sPrep->getTransposition(), 0);
        prep.setProperty( ptagDirect_gain,              sPrep->getGain(), 0);
        prep.setProperty( ptagDirect_resGain,           sPrep->getResonanceGain(), 0);
        prep.setProperty( ptagDirect_hammerGain,        sPrep->getHammerGain(), 0);
        
        return prep;
    }
    
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
               GeneralSettings::Ptr general,
               int Id):
    Id(Id)
    {
        sPrep = new SynchronicPreparation(tuning);
        aPrep = new SynchronicPreparation(sPrep);
        processor = new SynchronicProcessor(s, aPrep, general, Id);
    };
    
    inline ValueTree getState(void)
    {
        ValueTree prep( vtagSynchronicPrep + String(Id));
        
        /*
        "Synchronic Id",
        "Tuning Id",
        "Tempo",
        "NumPulses",
        "ClusterMin",
        "ClusterMax",
        "ClusterThresh",
        "Mode",
        "BeatsToSkip",
        "BeatMults",
        "LengthMults",
        "AccentMults",
        "TranspOffsets",
        "AT1Mode",
        "AT1History",
        "AT1Subdivs",
        "AT1Min",
        "AT1Max"
        */
        
        prep.setProperty( ptagSynchronic_Id,                  Id, 0);
        prep.setProperty( ptagSynchronic_tuning,              sPrep->getTuning()->getId(), 0);
        prep.setProperty( ptagSynchronic_tempo,               sPrep->getTempo(), 0);
        prep.setProperty( ptagSynchronic_numBeats,            sPrep->getNumBeats(), 0);
        prep.setProperty( ptagSynchronic_clusterMin,          sPrep->getClusterMin(), 0);
        prep.setProperty( ptagSynchronic_clusterMax,          sPrep->getClusterMax(), 0);
        prep.setProperty( ptagSynchronic_clusterThresh,       sPrep->getClusterThreshMS(), 0);
        prep.setProperty( ptagSynchronic_mode,                sPrep->getMode(), 0);
        prep.setProperty( ptagSynchronic_beatsToSkip,         sPrep->getBeatsToSkip(), 0);
        
        
        ValueTree beatMults( vtagSynchronic_beatMults);
        int count = 0;
        for (auto f : sPrep->getBeatMultipliers())
        {
            beatMults.      setProperty( ptagFloat + String(count++), f, 0);
        }
        prep.addChild(beatMults, -1, 0);
        
        
        ValueTree lenMults( vtagSynchronic_lengthMults);
        count = 0;
        for (auto f : sPrep->getLengthMultipliers())
        {
            lenMults.       setProperty( ptagFloat + String(count++), f, 0);
        }
        prep.addChild(lenMults, -1, 0);
        
        
        ValueTree accentMults( vtagSynchronic_accentMults);
        count = 0;
        for (auto f : sPrep->getAccentMultipliers())
        {
            accentMults.    setProperty( ptagFloat + String(count++), f, 0);
        }
        prep.addChild(accentMults, -1, 0);
        
        
        ValueTree transpOffsets( vtagSynchronic_transpOffsets);
        count = 0;
        for (auto f : sPrep->getTranspOffsets())
        {
            transpOffsets.  setProperty( ptagFloat + String(count++), f, 0);
        }
        prep.addChild(transpOffsets, -1, 0);

        
        prep.setProperty( ptagSynchronic_at1Mode,             sPrep->getAdaptiveTempo1Mode(), 0);
        prep.setProperty( ptagSynchronic_at1History,          sPrep->getAdaptiveTempo1History(), 0);
        prep.setProperty( ptagSynchronic_at1Subdivisions,     sPrep->getAdaptiveTempo1Subdivisions(), 0);
        prep.setProperty( ptagSynchronic_AT1Min,              sPrep->getAdaptiveTempo1Min(), 0);
        prep.setProperty( ptagSynchronic_AT1Max,              sPrep->getAdaptiveTempo1Max(), 0);
        
        return prep;
        
    }
    
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
        processor = new NostalgicProcessor(s, aPrep, Id);
    };
    
    ~Nostalgic() {};
    
    /*
     NostalgicId = 0,
     NostalgicTuning,
     NostalgicWaveDistance,
     NostalgicUndertow,
     NostalgicTransposition,
     NostalgicGain,
     NostalgicLengthMultiplier,
     NostalgicBeatsToSkip,
     NostalgicMode,
     NostalgicSyncTarget,
        */
     inline int getId() {return Id;};
    
    inline ValueTree getState(void)
    {
        ValueTree prep( vtagNostalgicPrep + String(Id));
        
        prep.setProperty( ptagNostalgic_Id,                 Id, 0);
        prep.setProperty( ptagNostalgic_tuning,             sPrep->getTuning()->getId(), 0);
        prep.setProperty( ptagNostalgic_waveDistance,       sPrep->getWavedistance(), 0);
        prep.setProperty( ptagNostalgic_undertow,           sPrep->getUndertow(), 0);
        prep.setProperty( ptagNostalgic_transposition,      sPrep->getTransposition(), 0);
        prep.setProperty( ptagNostalgic_gain,               sPrep->getGain(), 0);
        prep.setProperty( ptagNostalgic_lengthMultiplier,   sPrep->getLengthMultiplier(), 0);
        prep.setProperty( ptagNostalgic_beatsToSkip,        sPrep->getBeatsToSkip(), 0);
        prep.setProperty( ptagNostalgic_mode,               sPrep->getMode(), 0);
        prep.setProperty( ptagNostalgic_syncTarget,         sPrep->getSyncTarget(), 0);

        return prep;
    }
    
    NostalgicProcessor::Ptr        processor;
    NostalgicPreparation::Ptr      sPrep;
    NostalgicPreparation::Ptr      aPrep;
    
    
private:
    int Id;
    
    
    
    JUCE_LEAK_DETECTOR(Nostalgic)
};


#endif  // PREPARATION_H_INCLUDED