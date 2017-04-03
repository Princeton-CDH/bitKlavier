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
#include "Tempo.h"
#include "General.h"
#include "BKUpdateState.h"


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
           DirectPreparation::Ptr d,
           int Id):
    sPrep(new DirectPreparation(d)),
    aPrep(new DirectPreparation(sPrep)),
    processor(new DirectProcessor(s, res, ham, aPrep, Id)),
    Id(Id),
    name(String(Id))
    {
        
    }
    
    Direct(BKSynthesiser *s,
           BKSynthesiser *res,
           BKSynthesiser *ham,
           Tuning::Ptr tuning,
           BKUpdateState::Ptr us,
           int Id):
    Id(Id),
    name(String(Id)),
    updateState(us)
    {
        sPrep       = new DirectPreparation(tuning);
        aPrep       = new DirectPreparation(sPrep);
        processor   = new DirectProcessor(s, res, ham, aPrep, Id);
    };
    
    inline void prepareToPlay(double sampleRate, BKSynthesiser* main, BKSynthesiser* res, BKSynthesiser* hammer)
    {
        processor->attachToSynthesiser(main, res, hammer);
        processor->setCurrentPlaybackSampleRate(sampleRate);
    }
    
    inline void prepareToPlay(double sampleRate)
    {
        processor->setCurrentPlaybackSampleRate(sampleRate);
    }
    
    inline ValueTree getState(void)
    {
        ValueTree prep( vtagDirect+String(Id));
        
        prep.setProperty( ptagDirect_id,                Id, 0);
        prep.setProperty( "name", name, 0);
        prep.setProperty( ptagDirect_tuning,            sPrep->getTuning()->getId(), 0);
        
        ValueTree transp( vtagDirect_transposition);
        Array<float> m = sPrep->getTransposition();
        int count = 0;
        for (auto f : m)    transp.setProperty( ptagFloat + String(count++), f, 0);
        prep.addChild(transp, -1, 0);
        
        prep.setProperty( ptagDirect_gain,              sPrep->getGain(), 0);
        prep.setProperty( ptagDirect_resGain,           sPrep->getResonanceGain(), 0);
        prep.setProperty( ptagDirect_hammerGain,        sPrep->getHammerGain(), 0);
        
        return prep;
    }
    
    inline void setState(XmlElement* e, Tuning::PtrArr tuning)
    {
        float f; int i;
        
        String n = e->getStringAttribute("name");
        
        if (n != String::empty)     name = n;
        else                        name = String(Id);
        
        i = e->getStringAttribute(ptagDirect_tuning).getIntValue();
        sPrep->setTuning(tuning[i]);
        
        f = e->getStringAttribute(ptagDirect_gain).getFloatValue();
        sPrep->setGain(f);
        
        f = e->getStringAttribute(ptagDirect_hammerGain).getFloatValue();
        sPrep->setHammerGain(f);
        
        f = e->getStringAttribute(ptagDirect_resGain).getFloatValue();
        sPrep->setResonanceGain(f);
        
        forEachXmlChildElement (*e, sub)
        {
            if (sub->hasTagName(vtagDirect_transposition))
            {
                Array<float> transp;
                for (int k = 0; k < 128; k++)
                {
                    String attr = sub->getStringAttribute(ptagFloat + String(k));
                    
                    if (attr == String::empty) break;
                    else
                    {
                        f = attr.getFloatValue();
                        transp.add(f);
                    }
                }
                
                sPrep->setTransposition(transp);
                
            }
        }
        // copy static to active
        aPrep->copy(sPrep);
    }
    
    ~Direct() {};
    
    inline int getId() {return Id;};
    
    
    DirectPreparation::Ptr      sPrep;
    DirectPreparation::Ptr      aPrep;
    DirectProcessor::Ptr        processor;
    
    
    void reset()
    {
        aPrep->copy(sPrep);
        updateState->directPreparationDidChange = true;
    }
    
    //void didChange(bool which) { updateState->directPreparationDidChange = which; }
    
    inline String getName(void) const noexcept {return name;}
    inline void setName(String newName)
    {
        name = newName;
        updateState->directPreparationDidChange = true;
    }
   
    
private:
    int Id;
    String name;
    

    BKUpdateState::Ptr          updateState;
    
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
           SynchronicPreparation::Ptr prep,
           GeneralSettings::Ptr general,
           int Id):
    sPrep(new SynchronicPreparation(prep)),
    aPrep(new SynchronicPreparation(sPrep)),
    processor(new SynchronicProcessor(s, aPrep, general, Id)),
    Id(Id),
    name(String(Id))
    {
        
    }
    
    Synchronic(BKSynthesiser *s,
               Tuning::Ptr tuning,
               Tempo::Ptr tempo,
               GeneralSettings::Ptr general,
               BKUpdateState::Ptr us,
               int Id):
    Id(Id),
    name(String(Id)),
    updateState(us)
    {
        sPrep       = new SynchronicPreparation(tuning, tempo);
        aPrep       = new SynchronicPreparation(sPrep);
        processor   = new SynchronicProcessor(s, aPrep, general, Id);
    };
    
    void prepareToPlay(double sampleRate, BKSynthesiser* main)
    {
        processor->attachToSynthesiser(main);
        processor->setCurrentPlaybackSampleRate(sampleRate);
    }
    
    void prepareToPlay(double sampleRate)
    {
        processor->setCurrentPlaybackSampleRate(sampleRate);
    }
    
    inline ValueTree getState(void)
    {
        ValueTree prep( vtagSynchronic + String(Id));
        
        prep.setProperty( ptagSynchronic_Id,                  Id, 0);
        prep.setProperty( "name", name, 0);
        prep.setProperty( ptagSynchronic_tuning,              sPrep->getTuning()->getId(), 0);
        prep.setProperty( ptagSynchronic_tempo,               sPrep->getTempoControl()->getId(), 0);
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
        
        
        ValueTree transposition( vtagSynchronic_transpOffsets);
        
        int tcount = 0;
        for (auto arr : sPrep->getTransposition())
        {
            ValueTree t("t"+String(tcount++));
            count = 0;
            for (auto f : arr)  t.setProperty( ptagFloat + String(count++), f, 0);
            transposition.addChild(t,-1,0);
        }
        prep.addChild(transposition, -1, 0);
        
        return prep;
        
    }
    
    inline void setState(XmlElement* e, Tuning::PtrArr tuning, Tempo::PtrArr tempo)
    {
        int i; float f;
        
        String n = e->getStringAttribute("name");
        
        if (n != String::empty)     name = n;
        else                        name = String(Id);
        
        i = e->getStringAttribute(ptagSynchronic_tuning).getIntValue();
        sPrep->setTuning(tuning[i]);
        
        if (e->getStringAttribute(ptagSynchronic_tempo) != String::empty)
        {
            i = e->getStringAttribute(ptagSynchronic_tempo).getIntValue();
            sPrep->setTempoControl(tempo[i]);
        }
        else
        {
            sPrep->setTempoControl(tempo[0]);
        }
        
        i = e->getStringAttribute(ptagSynchronic_numBeats).getIntValue();
        sPrep->setNumBeats(i);
        
        i = e->getStringAttribute(ptagSynchronic_clusterMin).getIntValue();
        sPrep->setClusterMin(i);
        
        i = e->getStringAttribute(ptagSynchronic_clusterMax).getIntValue();
        sPrep->setClusterMax(i);
        
        i = e->getStringAttribute(ptagSynchronic_clusterThresh).getIntValue();
        sPrep->setClusterThresh(i);
        
        i = e->getStringAttribute(ptagSynchronic_mode).getIntValue();
        sPrep->setMode((SynchronicSyncMode) i);
        
        i = e->getStringAttribute(ptagSynchronic_beatsToSkip).getIntValue();
        sPrep->setBeatsToSkip(i);
        
        forEachXmlChildElement (*e, sub)
        {
            if (sub->hasTagName(vtagSynchronic_beatMults))
            {
                Array<float> beats;
                for (int k = 0; k < 128; k++)
                {
                    String attr = sub->getStringAttribute(ptagFloat + String(k));
                    
                    if (attr == String::empty) break;
                    else
                    {
                        f = attr.getFloatValue();
                        beats.add(f);
                    }
                }
                
                sPrep->setBeatMultipliers(beats);
                
            }
            else  if (sub->hasTagName(vtagSynchronic_accentMults))
            {
                Array<float> accents;
                for (int k = 0; k < 128; k++)
                {
                    String attr = sub->getStringAttribute(ptagFloat + String(k));
                    
                    if (attr == String::empty) break;
                    else
                    {
                        f = attr.getFloatValue();
                        accents.add(f);
                    }
                }
                
                sPrep->setAccentMultipliers(accents);
                
            }
            else  if (sub->hasTagName(vtagSynchronic_lengthMults))
            {
                Array<float> lens;
                for (int k = 0; k < 128; k++)
                {
                    String attr = sub->getStringAttribute(ptagFloat + String(k));
                    
                    if (attr == String::empty) break;
                    else
                    {
                        f = attr.getFloatValue();
                        lens.add(f);
                    }
                }
                
                sPrep->setLengthMultipliers(lens);
                
            }
            else  if (sub->hasTagName(vtagSynchronic_transpOffsets))
            {
                Array<Array<float>> atransp;
                int tcount = 0;
                forEachXmlChildElement (*sub, asub)
                {
                    if (asub->hasTagName("t"+String(tcount++)))
                    {
                        Array<float> transp;
                        for (int k = 0; k < 128; k++)
                        {
                            String attr = asub->getStringAttribute(ptagFloat + String(k));
                            
                            if (attr == String::empty) break;
                            else
                            {
                                f = attr.getFloatValue();
                                transp.add(f);
                            }
                        }
                        atransp.set(tcount-1, transp);
                    }
                }
                
                sPrep->setTransposition(atransp);
            }
        }
        
        aPrep->copy(sPrep);
    }
    
    ~Synchronic() {};
    
    inline int getId() {return Id;};
    
    SynchronicPreparation::Ptr      sPrep;
    SynchronicPreparation::Ptr      aPrep;
    SynchronicProcessor::Ptr        processor;

    void reset()
    {
        aPrep->copy(sPrep);
        //processor->atReset();
        updateState->synchronicPreparationDidChange = true;
        DBG("synchronic reset");
    }
    
    //void didChange(bool which) { updateState->synchronicPreparationDidChange = which; }
    
    inline String getName(void) const noexcept {return name;}
    
    inline void setName(String newName)
    {
        name = newName;
        updateState->synchronicPreparationDidChange = true;
    }
    
private:
    int Id;
    String name;
    BKUpdateState::Ptr updateState;
    
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
               NostalgicPreparation::Ptr prep,
               int Id):
    sPrep(new NostalgicPreparation(prep)),
    aPrep(new NostalgicPreparation(sPrep)),
    processor(new NostalgicProcessor(s, aPrep, Id)),
    Id(Id),
    name(String(Id))
    {
        
    }
    
    
    Nostalgic(BKSynthesiser *s,
              Tuning::Ptr tuning,
              BKUpdateState::Ptr us,
              int Id):
    Id(Id),
    name(String(Id)),
    updateState(us)
    {
        sPrep       = new NostalgicPreparation(tuning);
        aPrep       = new NostalgicPreparation(sPrep);
        processor   = new NostalgicProcessor(s, aPrep, Id);
    };
    
    ~Nostalgic() {};
    
    void prepareToPlay(double sampleRate, BKSynthesiser* main)
    {
        processor->attachToSynthesiser(main);
        processor->setCurrentPlaybackSampleRate(sampleRate);
    }
    
    void prepareToPlay(double sampleRate)
    {
        processor->setCurrentPlaybackSampleRate(sampleRate);
    }
    
    inline int getId() {return Id;}
    
    inline ValueTree getState(void)
    {
        ValueTree prep( vtagNostalgic + String(Id));
        
        prep.setProperty( ptagNostalgic_Id,                 Id, 0);
        prep.setProperty( "name", name, 0);
        prep.setProperty( ptagNostalgic_tuning,             sPrep->getTuning()->getId(), 0);
        prep.setProperty( ptagNostalgic_waveDistance,       sPrep->getWavedistance(), 0);
        prep.setProperty( ptagNostalgic_undertow,           sPrep->getUndertow(), 0);
        
        ValueTree transp( vtagNostalgic_transposition);
        int count = 0;
        for (auto f : sPrep->getTransposition())
        {
            transp.      setProperty( ptagFloat + String(count++), f, 0);
        }
        prep.addChild(transp, -1, 0);
        
        prep.setProperty( ptagNostalgic_gain,               sPrep->getGain(), 0);
        prep.setProperty( ptagNostalgic_lengthMultiplier,   sPrep->getLengthMultiplier(), 0);
        prep.setProperty( ptagNostalgic_beatsToSkip,        sPrep->getBeatsToSkip(), 0);
        prep.setProperty( ptagNostalgic_mode,               sPrep->getMode(), 0);
        prep.setProperty( ptagNostalgic_syncTarget,         sPrep->getSyncTarget(), 0);

        return prep;
    }
    
    inline void setState(XmlElement* e, Tuning::PtrArr tuning, Synchronic::PtrArr synchronic)
    {
        int i; float f;
        
        String n = e->getStringAttribute("name");
        
        if (n != String::empty)     name = n;
        else                        name = String(Id);
        
        i = e->getStringAttribute(ptagNostalgic_tuning).getIntValue();
        sPrep->setTuning(tuning[i]);
        
        i = e->getStringAttribute(ptagNostalgic_waveDistance).getIntValue();
        sPrep->setWaveDistance(i);
        
        i = e->getStringAttribute(ptagNostalgic_undertow).getIntValue();
        sPrep->setUndertow(i);
        
        forEachXmlChildElement (*e, sub)
        {
            if (sub->hasTagName(vtagNostalgic_transposition))
            {
                Array<float> transp;
                for (int k = 0; k < 128; k++)
                {
                    String attr = sub->getStringAttribute(ptagFloat + String(k));
                    
                    if (attr == String::empty) break;
                    else
                    {
                        f = attr.getFloatValue();
                        transp.add(f);
                    }
                }
                
                sPrep->setTransposition(transp);
                
            }
        }
        
        
        f = e->getStringAttribute(ptagNostalgic_lengthMultiplier).getFloatValue();
        sPrep->setLengthMultiplier(f);
        
        f = e->getStringAttribute(ptagNostalgic_beatsToSkip).getFloatValue();
        sPrep->setBeatsToSkip(f);
        
        f = e->getStringAttribute(ptagNostalgic_gain).getFloatValue();
        sPrep->setGain(f);
        
        i = e->getStringAttribute(ptagNostalgic_mode).getIntValue();
        sPrep->setMode((NostalgicSyncMode)i);
        
        i = e->getStringAttribute(ptagNostalgic_syncTarget).getIntValue();
        sPrep->setSyncTarget(i);
        
        sPrep->setSyncTargetProcessor(synchronic[i]->processor);
        aPrep->setSyncTargetProcessor(synchronic[i]->processor);
        
        aPrep->copy(sPrep);
    }
    
    
    NostalgicPreparation::Ptr      sPrep;
    NostalgicPreparation::Ptr      aPrep;
    NostalgicProcessor::Ptr        processor;
    
    void reset()
    {
        aPrep->copy(sPrep);
        updateState->nostalgicPreparationDidChange = true;
        DBG("nostalgic reset");
    }
    
    //void didChange(bool which) { updateState->nostalgicPreparationDidChange = which; }
    
    inline String getName(void) const noexcept {return name;}inline void setName(String newName)
    {
        name = newName;
        updateState->nostalgicPreparationDidChange = true;
    }
    
private:
    int Id;
    String name;
    BKUpdateState::Ptr updateState;
    
    JUCE_LEAK_DETECTOR(Nostalgic)
};


#endif  // PREPARATION_H_INCLUDED
