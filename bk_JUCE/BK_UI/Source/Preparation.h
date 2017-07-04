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


/* ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ DIRECT ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ */

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
    name(String(Id)),
    X(-1),Y(-1)
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
    updateState(us),
    X(-1),Y(-1)
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
    
    inline void setTuning(Tuning::Ptr tuning)
    {
        sPrep->setTuning(tuning);
        aPrep->setTuning(tuning);
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
        
        prep.setProperty( posX, X, 0);
        prep.setProperty( posY, Y, 0);
        
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
        
        n = e->getStringAttribute(posX);
        if (n != String::empty) X = n.getIntValue();
        else                    X = -1;
        
        n = e->getStringAttribute(posY);
        if (n != String::empty) Y = n.getIntValue();
        else                    Y = -1;
        
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
    
    inline int getTuningId() {return sPrep->getTuning()->getId();}
    
    
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
   
    inline void setPosition(int x, int y) { X=x;Y=y;}
    inline Point<int> getPosition(void) { return Point<int>(X,Y);}
    inline void setPosition(Point<int> point) { X = point.getX(); Y= point.getY();}
    inline void setX(int x) { X = x; }
    inline void setY(int y) { Y = y; }
    inline int getX(void) const noexcept { return X; }
    inline int getY(void) const noexcept { return Y; }
    
private:
    int Id;
    String name;
    BKUpdateState::Ptr          updateState;
    
    int X,Y;
    
    JUCE_LEAK_DETECTOR(Direct)
};

class DirectModPreparation : public ReferenceCountedObject
{
public:
    
    typedef ReferenceCountedObjectPtr<DirectModPreparation>   Ptr;
    typedef Array<DirectModPreparation::Ptr>                  PtrArr;
    typedef Array<DirectModPreparation::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<DirectModPreparation>                  Arr;
    typedef OwnedArray<DirectModPreparation, CriticalSection> CSArr;
    
    /*
     DirectId = 0,
     DirectTuning,
     DirectTransposition,
     DirectGain,
     DirectResGain,
     DirectHammerGain,
     DirectParameterTypeNil,
     */
    
    DirectModPreparation(DirectPreparation::Ptr p, int Id):
    Id(Id),
    X(-1),Y(-1)
    {
        param.ensureStorageAllocated(cDirectParameterTypes.size());
        
        param.set(DirectTuning, String(p->getTuning()->getId()));
        param.set(DirectTransposition, floatArrayToString(p->getTransposition()));
        param.set(DirectGain, String(p->getGain()));
        param.set(DirectResGain, String(p->getResonanceGain()));
        param.set(DirectHammerGain, String(p->getHammerGain()));
    }
    
    
    DirectModPreparation(int Id):
    Id(Id),
    X(-1),Y(-1)
    {
        param.add("");
        param.add("");
        param.add("");
        param.add("");
        param.add("");
        //param.add("");
    }
    
    inline ValueTree getState(int Id)
    {
        ValueTree prep( vtagModDirect+String(Id));
        
        String p = "";
        
        p = getParam(DirectTuning);
        if (p != String::empty) prep.setProperty( ptagDirect_tuning,            p.getIntValue(), 0);
        
        ValueTree transp( vtagDirect_transposition);
        int count = 0;
        p = getParam(DirectTransposition);
        if (p != String::empty)
        {
            Array<float> m = stringToFloatArray(p);
            for (auto f : m)
            {
                transp.      setProperty( ptagFloat + String(count++), f, 0);
            }
        }
        prep.addChild(transp, -1, 0);
        
        p = getParam(DirectGain);
        if (p != String::empty) prep.setProperty( ptagDirect_gain,              p.getFloatValue(), 0);
        
        p = getParam(DirectResGain);
        if (p != String::empty) prep.setProperty( ptagDirect_resGain,           p.getFloatValue(), 0);
        
        p = getParam(DirectHammerGain);
        if (p != String::empty) prep.setProperty( ptagDirect_hammerGain,        p.getFloatValue(), 0);

        
        return prep;
    }
    
    inline void setState(XmlElement* e)
    {
        float f;
        
        String p = e->getStringAttribute(ptagDirect_tuning);
        setParam(DirectTuning, p);
        
        p = e->getStringAttribute(ptagDirect_gain);
        setParam(DirectGain, p);
        
        p = e->getStringAttribute(ptagDirect_hammerGain);
        setParam(DirectHammerGain, p);
        
        p = e->getStringAttribute(ptagDirect_resGain);
        setParam(DirectResGain, p);
        
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
                
                setParam(DirectTransposition, floatArrayToString(transp));
                
            }
        }
    }
    
    ~DirectModPreparation(void)
    {
        
    }
    
    inline void copy(DirectPreparation::Ptr d)
    {
        param.set(DirectTuning, String(d->getTuning()->getId()));
        param.set(DirectTransposition, floatArrayToString(d->getTransposition()));
        param.set(DirectGain, String(d->getGain()));
        param.set(DirectResGain, String(d->getResonanceGain()));
        param.set(DirectHammerGain, String(d->getHammerGain()));
    }
    
    
    inline const String getParam(DirectParameterType type)
    {
        if (type != DirectId)   return param[type];
        else                    return "";
    }
    
    inline void setParam(DirectParameterType type, String val) { param.set(type, val);}
    
    inline const StringArray getStringArray(void) { return param; }
    
    void print(void)
    {
        
    }
    
    inline void setId(int newId) { Id = newId; }
    inline int getId(void)const noexcept {return Id;}
    
    inline String getName(void) const noexcept {return name;}
    inline void setName(String newName) {name = newName;}

    inline void setPosition(int x, int y) { X=x;Y=y;}
    inline Point<int> getPosition(void) { return Point<int>(X,Y);}
    inline void setPosition(Point<int> point) { X = point.getX(); Y= point.getY();}
    inline void setX(int x) { X = x; }
    inline void setY(int y) { Y = y; }
    inline int getX(void) const noexcept { return X; }
    inline int getY(void) const noexcept { return Y; }

    
private:
    int Id;
    String name;
    StringArray          param;
    
    int X,Y;

    JUCE_LEAK_DETECTOR(DirectModPreparation);
};


/* ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ SYNCHRONIC ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ */

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
    name(String(Id)),
    X(-1),Y(-1)
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
    updateState(us),
    X(-1),Y(-1)
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
    
    inline void setTuning(Tuning::Ptr tuning)
    {
        sPrep->setTuning(tuning);
        aPrep->setTuning(tuning);
    }
    
    inline void setTempo(Tempo::Ptr tempo)
    {
        sPrep->setTempoControl(tempo);
        aPrep->setTempoControl(tempo);
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
        
        prep.setProperty( posX, X, 0);
        prep.setProperty( posY, Y, 0);
        
        
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
        
        n = e->getStringAttribute(posX);
        if (n != String::empty) X = n.getIntValue();
        else                    X = -1;
        
        n = e->getStringAttribute(posY);
        if (n != String::empty) Y = n.getIntValue();
        else                    Y = -1;
        
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
    
    inline int getId() {return Id;}
    
    inline int getTuningId() {return sPrep->getTuning()->getId();}
    
    inline int getTempoId() {return sPrep->getTempoControl()->getId();}
    
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
    
    inline void setPosition(int x, int y) { X=x;Y=y;}
    inline Point<int> getPosition(void) { return Point<int>(X,Y);}
    inline void setPosition(Point<int> point) { X = point.getX(); Y= point.getY();}
    inline void setX(int x) { X = x; }
    inline void setY(int y) { Y = y; }
    inline int getX(void) const noexcept { return X; }
    inline int getY(void) const noexcept { return Y; }
    
    
    
private:
    int Id;
    String name;
    BKUpdateState::Ptr updateState;
    
    int X,Y;
    
    
    JUCE_LEAK_DETECTOR(Synchronic)
};

class SynchronicModPreparation : public ReferenceCountedObject
{
public:
    
    typedef ReferenceCountedObjectPtr<SynchronicModPreparation>   Ptr;
    typedef Array<SynchronicModPreparation::Ptr>                  PtrArr;
    typedef Array<SynchronicModPreparation::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<SynchronicModPreparation>                  Arr;
    typedef OwnedArray<SynchronicModPreparation, CriticalSection> CSArr;
    
    /*
     SynchronicId = 0,
     SynchronicTuning,
     SynchronicTempo,
     SynchronicNumPulses,
     SynchronicClusterMin,
     SynchronicClusterMax,
     SynchronicClusterThresh,
     SynchronicMode,
     SynchronicBeatsToSkip,
     SynchronicBeatMultipliers,
     SynchronicLengthMultipliers,
     SynchronicAccentMultipliers,
     SynchronicTranspOffsets,
     AT1Mode,
     AT1History,
     AT1Subdivisions,
     AT1Min,
     AT1Max,
     */
    
    SynchronicModPreparation(SynchronicPreparation::Ptr p, int Id):
    Id(Id),
    X(-1),Y(-1)
    {
        param.ensureStorageAllocated(cSynchronicParameterTypes.size());
        
        param.set(SynchronicTuning, String(p->getTuning()->getId()));
        param.set(SynchronicTempo, String(p->getTempoControl()->getId()));
        param.set(SynchronicNumPulses, String(p->getNumBeats()));
        param.set(SynchronicClusterMin, String(p->getClusterMin()));
        param.set(SynchronicClusterMax, String(p->getClusterMax()));
        param.set(SynchronicClusterThresh, String(p->getClusterThreshMS()));
        param.set(SynchronicMode, String(p->getMode()));
        param.set(SynchronicBeatsToSkip, String(p->getBeatsToSkip()));
        param.set(SynchronicBeatMultipliers, floatArrayToString(p->getBeatMultipliers()));
        param.set(SynchronicLengthMultipliers, floatArrayToString(p->getLengthMultipliers()));
        param.set(SynchronicAccentMultipliers, floatArrayToString(p->getAccentMultipliers()));
        param.set(SynchronicTranspOffsets, arrayFloatArrayToString(p->getTransposition()));
        
    }
    
    
    SynchronicModPreparation(int Id):
    Id(Id),
    X(-1),Y(-1)
    {
        param.set(SynchronicTuning, "");
        param.set(SynchronicTempo, "");
        param.set(SynchronicNumPulses, "");
        param.set(SynchronicClusterMin, "");
        param.set(SynchronicClusterMax, "");
        param.set(SynchronicClusterThresh, "");
        param.set(SynchronicMode, "");
        param.set(SynchronicBeatsToSkip, "");
        param.set(SynchronicBeatMultipliers, "");
        param.set(SynchronicLengthMultipliers, "");
        param.set(SynchronicAccentMultipliers, "");
        param.set(SynchronicTranspOffsets, "");
    }
    
    inline void setId(int newId) { Id = newId; }
    inline int getId(void) const noexcept { return Id; }
    
    inline ValueTree getState(int Id)
    {
        ValueTree prep( vtagModSynchronic + String(Id));
        
        String p = "";
        
        p = getParam(SynchronicTuning);
        if (p != String::empty) prep.setProperty( ptagSynchronic_tuning,              p.getIntValue(), 0);
        
        p = getParam(SynchronicTempo);
        if (p != String::empty) prep.setProperty( ptagSynchronic_tempo,               p.getIntValue(), 0);
        
        p = getParam(SynchronicNumPulses);
        if (p != String::empty) prep.setProperty( ptagSynchronic_numBeats,            p.getIntValue(), 0);
        
        p = getParam(SynchronicClusterMin);
        if (p != String::empty) prep.setProperty( ptagSynchronic_clusterMin,          p.getIntValue(), 0);
        
        p = getParam(SynchronicClusterMax);
        if (p != String::empty) prep.setProperty( ptagSynchronic_clusterMax,          p.getIntValue(), 0);
        
        p = getParam(SynchronicClusterThresh);
        if (p != String::empty) prep.setProperty( ptagSynchronic_clusterThresh,       p.getIntValue(), 0);
        
        p = getParam(SynchronicMode);
        if (p != String::empty) prep.setProperty( ptagSynchronic_mode,                p.getIntValue(), 0);
        
        p = getParam(SynchronicBeatsToSkip);
        if (p != String::empty) prep.setProperty( ptagSynchronic_beatsToSkip,         p.getIntValue(), 0);
        
        ValueTree beatMults( vtagSynchronic_beatMults);
        int count = 0;
        p = getParam(SynchronicBeatMultipliers);
        if (p != String::empty)
        {
            Array<float> m = stringToFloatArray(p);
            for (auto f : m)
            {
                beatMults.      setProperty( ptagFloat + String(count++), f, 0);
            }
        }
        prep.addChild(beatMults, -1, 0);
        
        
        
        ValueTree lengthMults( vtagSynchronic_lengthMults);
        count = 0;
        p = getParam(SynchronicLengthMultipliers);
        if (p != String::empty)
        {
            Array<float> m = stringToFloatArray(p);
            for (auto f : m)
            {
                lengthMults.      setProperty( ptagFloat + String(count++), f, 0);
            }
        }
        prep.addChild(lengthMults, -1, 0);
        
        
        ValueTree accentMults( vtagSynchronic_accentMults);
        count = 0;
        p = getParam(SynchronicAccentMultipliers);
        if (p != String::empty)
        {
            Array<float> m = stringToFloatArray(p);
            for (auto f : m)
            {
                accentMults.      setProperty( ptagFloat + String(count++), f, 0);
            }
        }
        prep.addChild(accentMults, -1, 0);
        
        
        ValueTree transpOffsets( vtagSynchronic_transpOffsets);
        count = 0;
        p = getParam(SynchronicTranspOffsets);
        if (p != String::empty)
        {
            Array<float> m = stringToFloatArray(p);
            for (auto f : m)
            {
                transpOffsets.      setProperty( ptagFloat + String(count++), f, 0);
            }
        }
        prep.addChild(transpOffsets, -1, 0);

        
        return prep;
        
    }
    
    inline void setState(XmlElement* e)
    {
        float f;
        
        String p = e->getStringAttribute(ptagSynchronic_tuning);
        setParam(SynchronicTuning, p);
        
        p = e->getStringAttribute(ptagSynchronic_tempo);
        setParam(SynchronicTempo, p);
        
        p = e->getStringAttribute(ptagSynchronic_numBeats);
        setParam(SynchronicNumPulses, p);
        
        p = e->getStringAttribute(ptagSynchronic_clusterMin);
        setParam(SynchronicClusterMin, p);
        
        p = e->getStringAttribute(ptagSynchronic_clusterMax);
        setParam(SynchronicClusterMax, p);
        
        p = e->getStringAttribute(ptagSynchronic_clusterThresh);
        setParam(SynchronicClusterThresh, p);
        
        p = e->getStringAttribute(ptagSynchronic_mode);
        setParam(SynchronicMode, p);
        
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
                
                setParam(SynchronicBeatMultipliers, floatArrayToString(beats));
                
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
                
                setParam(SynchronicAccentMultipliers, floatArrayToString(accents));
                
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
                
                setParam(SynchronicLengthMultipliers, floatArrayToString(lens));
                
            }
            else  if (sub->hasTagName(vtagSynchronic_transpOffsets))
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
                
                setParam(SynchronicTranspOffsets, floatArrayToString(transp));
            }
        }
        
    }
    
    
    ~SynchronicModPreparation(void)
    {
        
    }
    
    inline void copy(SynchronicPreparation::Ptr p)
    {
        param.set(SynchronicTuning, String(p->getTuning()->getId()));
        param.set(SynchronicTempo, String(p->getTempoControl()->getId()));
        param.set(SynchronicNumPulses, String(p->getNumBeats()));
        param.set(SynchronicClusterMin, String(p->getClusterMin()));
        param.set(SynchronicClusterMax, String(p->getClusterMax()));
        param.set(SynchronicClusterThresh, String(p->getClusterThreshMS()));
        param.set(SynchronicMode, String(p->getMode()));
        param.set(SynchronicBeatsToSkip, String(p->getBeatsToSkip()));
        param.set(SynchronicBeatMultipliers, floatArrayToString(p->getBeatMultipliers()));
        param.set(SynchronicLengthMultipliers, floatArrayToString(p->getLengthMultipliers()));
        param.set(SynchronicAccentMultipliers, floatArrayToString(p->getAccentMultipliers()));
        param.set(SynchronicTranspOffsets, arrayFloatArrayToString(p->getTransposition()));
    }
    
    inline const StringArray getStringArray(void) { return param; }
    
    
    inline const String getParam(SynchronicParameterType type)
    {
        if (type != SynchronicId)
            return param[type];
        else
            return "";
    }
    
    inline void setParam(SynchronicParameterType type, String val) { param.set(type, val);}
    
    void print(void)
    {
        
    }
    
    inline String getName(void) const noexcept {return name;}
    inline void setName(String newName) {name = newName;}
    
    inline void setPosition(int x, int y) { X=x;Y=y;}
    inline Point<int> getPosition(void) { return Point<int>(X,Y);}
    inline void setPosition(Point<int> point) { X = point.getX(); Y= point.getY();}
    inline void setX(int x) { X = x; }
    inline void setY(int y) { Y = y; }
    inline int getX(void) const noexcept { return X; }
    inline int getY(void) const noexcept { return Y; }
    
    
private:
    int Id;
    String name;
    StringArray          param;
    
    int X,Y;

    
    JUCE_LEAK_DETECTOR(SynchronicModPreparation);
};



/* ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ NOSTALGIC ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ */

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
    name(String(Id)),
    X(-1),Y(-1)
    {
        
    }
    
    
    Nostalgic(BKSynthesiser *s,
              Tuning::Ptr tuning,
              BKUpdateState::Ptr us,
              int Id):
    Id(Id),
    name(String(Id)),
    updateState(us),
    X(-1),Y(-1)
    {
        sPrep       = new NostalgicPreparation(tuning);
        aPrep       = new NostalgicPreparation(sPrep);
        processor   = new NostalgicProcessor(s, aPrep, Id);
    };
    
    ~Nostalgic() {};
    
    inline void setSynchronic(Synchronic::Ptr synchronic)
    {
        sPrep->setSyncTarget(synchronic->getId());
        sPrep->setSyncTargetProcessor(synchronic->processor);
        
        aPrep->setSyncTarget(synchronic->getId());
        aPrep->setSyncTargetProcessor(synchronic->processor);
    }
    
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

        prep.setProperty( posX, X, 0);
        prep.setProperty( posY, Y, 0);
        
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
        
        n = e->getStringAttribute(posX);
        if (n != String::empty) X = n.getIntValue();
        else                    X = -1;
        
        n = e->getStringAttribute(posY);
        if (n != String::empty) Y = n.getIntValue();
        else                    Y = -1;
        
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
    
    inline int getTuningId() {return sPrep->getTuning()->getId();}
    
    inline int getId() {return Id;}
    
    inline int getSynchronicTargetId(void)
    {
        return sPrep->getSyncTarget();
    }
    
    
    
    NostalgicPreparation::Ptr      sPrep;
    NostalgicPreparation::Ptr      aPrep;
    NostalgicProcessor::Ptr        processor;
    
    inline void setTuning(Tuning::Ptr tuning)
    {
        sPrep->setTuning(tuning);
        aPrep->setTuning(tuning);
        
        processor->setTuner(tuning->processor);
    }
    
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
    
    inline void setPosition(int x, int y) { X=x;Y=y;}
    inline Point<int> getPosition(void) { return Point<int>(X,Y);}
    inline void setPosition(Point<int> point) { X = point.getX(); Y= point.getY();}
    inline void setX(int x) { X = x; }
    inline void setY(int y) { Y = y; }
    inline int getX(void) const noexcept { return X; }
    inline int getY(void) const noexcept { return Y; }
    
private:
    int Id;
    String name;
    BKUpdateState::Ptr updateState;
    
    int X,Y;
    
    JUCE_LEAK_DETECTOR(Nostalgic)
};

class ModificationMapper : public ReferenceCountedObject
{
public:
    typedef ReferenceCountedObjectPtr<ModificationMapper>   Ptr;
    typedef Array<ModificationMapper::Ptr>                  PtrArr;
    typedef Array<ModificationMapper::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<ModificationMapper>                  Arr;
    typedef OwnedArray<ModificationMapper, CriticalSection> CSArr;
    
    
    typedef ReferenceCountedArray<ModificationMapper>       RCArr;
    
    ModificationMapper(BKPreparationType type, int Id):
    type(type),
    Id(Id)
    {
        for (int i = 0; i < 5; i++) resets.add(Array<int>());
    }
    
    
    ModificationMapper(BKPreparationType type, int Id, Array<int> keymaps, Array<int> targets):
    type(type),
    Id(Id),
    keymaps(keymaps),
    targets(targets)
    {
        
    }

    
    void print(void)
    {
        DBG("MAPPER type: " + String(type)  + " Id: " + String(Id) + " Keymaps: " + intArrayToString(keymaps) + " Targets: " + intArrayToString(targets));
    }
    
    inline int getId(void) const noexcept { return Id; }
    inline void setId(int newId) { Id = newId; }
    
    inline BKPreparationType getType(void) const noexcept { return type; }
    inline void setType(BKPreparationType newType) { type = newType; }
    
    inline void addTarget(int target) { targets.add(target); }
    inline Array<int> getTargets(void) {return targets;}
    
    inline void addKeymap(int keymap) { keymaps.add(keymap); }
    inline Array<int> getKeymaps(void) {return keymaps;}
    
    
    inline void removeKeymap(int keymap)
    {
        for (int i = keymaps.size(); --i>=0;)
        {
            if (keymaps[i] == keymap) keymaps.remove(i);
        }
    }
    
    inline void clearKeymaps(void) {keymaps.clear();}
    inline void clearTargets(void) {targets.clear();}
    
    Array<Array<int>> resets;
    
private:
    BKPreparationType type;
    int Id;
    
    Array<int> targets;
    Array<int> keymaps;
    
    
    
    JUCE_LEAK_DETECTOR(ModificationMapper);
};

class NostalgicModPreparation : public ReferenceCountedObject
{
public:
    
    typedef ReferenceCountedObjectPtr<NostalgicModPreparation>   Ptr;
    typedef Array<NostalgicModPreparation::Ptr>                  PtrArr;
    typedef Array<NostalgicModPreparation::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<NostalgicModPreparation>                  Arr;
    typedef OwnedArray<NostalgicModPreparation, CriticalSection> CSArr;
    
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
     NostalgicParameterTypeNil
     
     */
    
    NostalgicModPreparation(NostalgicPreparation::Ptr p, int Id):
    Id(Id),
    X(-1),Y(-1)
    {
        param.ensureStorageAllocated(cNostalgicParameterTypes.size());
        
        param.set(NostalgicTuning, String(p->getTuning()->getId()));
        param.set(NostalgicWaveDistance, String(p->getWavedistance()));
        param.set(NostalgicUndertow, String(p->getUndertow()));
        param.set(NostalgicTransposition, floatArrayToString(p->getTransposition()));
        param.set(NostalgicGain, String(p->getGain()));
        param.set(NostalgicLengthMultiplier, String(p->getLengthMultiplier()));
        param.set(NostalgicBeatsToSkip, String(p->getBeatsToSkip()));
        param.set(NostalgicMode, String(p->getMode()));
        param.set(NostalgicSyncTarget, String(p->getSyncTarget()));
        
    }
    
    
    NostalgicModPreparation(int Id):
    Id(Id),
    X(-1),Y(-1)
    {
        param.set(NostalgicTuning, "");
        param.set(NostalgicWaveDistance, "");
        param.set(NostalgicUndertow, "");
        param.set(NostalgicTransposition, "");
        param.set(NostalgicGain, "");
        param.set(NostalgicLengthMultiplier, "");
        param.set(NostalgicBeatsToSkip, "");
        param.set(NostalgicMode, "");
        param.set(NostalgicSyncTarget, "");
    }
    
    inline void setId(int newId) { Id = newId; }
    inline int getId(void) const noexcept { return Id; }
    
    inline ValueTree getState(int Id)
    {
        ValueTree prep( vtagModNostalgic + String(Id));
        
        String p = "";
        
        p = getParam(NostalgicTuning);
        if (p != String::empty) prep.setProperty( ptagNostalgic_tuning,             p.getIntValue(), 0);
        
        p = getParam(NostalgicWaveDistance);
        if (p != String::empty) prep.setProperty( ptagNostalgic_waveDistance,       p.getIntValue(), 0);
        
        p = getParam(NostalgicUndertow);
        if (p != String::empty) prep.setProperty( ptagNostalgic_undertow,           p.getIntValue(), 0);
        
        ValueTree transp( vtagNostalgic_transposition);
        int count = 0;
        p = getParam(NostalgicTransposition);
        if (p != String::empty)
        {
            Array<float> m = stringToFloatArray(p);
            for (auto f : m)
            {
                transp.      setProperty( ptagFloat + String(count++), f, 0);
            }
        }
        prep.addChild(transp, -1, 0);
        
        p = getParam(NostalgicGain);
        if (p != String::empty) prep.setProperty( ptagNostalgic_gain,               p.getFloatValue(), 0);
        
        p = getParam(NostalgicLengthMultiplier);
        if (p != String::empty) prep.setProperty( ptagNostalgic_lengthMultiplier,   p.getFloatValue(), 0);
        
        p = getParam(NostalgicBeatsToSkip);
        if (p != String::empty) prep.setProperty( ptagNostalgic_beatsToSkip,        p.getFloatValue(), 0);
        
        p = getParam(NostalgicMode);
        if (p != String::empty) prep.setProperty( ptagNostalgic_mode,               p.getIntValue(), 0);
        
        p = getParam(NostalgicSyncTarget);
        if (p != String::empty) prep.setProperty( ptagNostalgic_syncTarget,         p.getIntValue(), 0);

        
        return prep;
    }
    
    inline void setState(XmlElement* e)
    {
        float f;
        
        String p = e->getStringAttribute(ptagNostalgic_tuning);
        setParam(NostalgicTuning, p);
        
        p = e->getStringAttribute(ptagNostalgic_waveDistance);
        setParam(NostalgicWaveDistance, p);
        
        p = e->getStringAttribute(ptagNostalgic_undertow);
        setParam(NostalgicUndertow, p);
        
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
                
                setParam(NostalgicTransposition, floatArrayToString(transp));
                
            }
        }
        
        p = e->getStringAttribute(ptagNostalgic_lengthMultiplier);
        setParam(NostalgicLengthMultiplier, p);
        
        p = e->getStringAttribute(ptagNostalgic_beatsToSkip);
        setParam(NostalgicBeatsToSkip, p);
        
        p = e->getStringAttribute(ptagNostalgic_gain);
        setParam(NostalgicGain, p);
        
        p = e->getStringAttribute(ptagNostalgic_mode);
        setParam(NostalgicMode, p);
        
        p = e->getStringAttribute(ptagNostalgic_syncTarget);
        setParam(NostalgicSyncTarget, p);
    }
    
    
    ~NostalgicModPreparation(void)
    {
        
    }
    
    inline void copy(NostalgicPreparation::Ptr p)
    {
        param.set(NostalgicTuning, String(p->getTuning()->getId()));
        param.set(NostalgicWaveDistance, String(p->getWavedistance()));
        param.set(NostalgicUndertow, String(p->getUndertow()));
        param.set(NostalgicTransposition, floatArrayToString(p->getTransposition()));
        param.set(NostalgicGain, String(p->getGain()));
        param.set(NostalgicLengthMultiplier, String(p->getLengthMultiplier()));
        param.set(NostalgicBeatsToSkip, String(p->getBeatsToSkip()));
        param.set(NostalgicMode, String(p->getMode()));
        param.set(NostalgicSyncTarget, String(p->getSyncTarget()));
    }
    
    inline const StringArray getStringArray(void) { return param; }
    
    inline const String getParam(NostalgicParameterType type)
    {
        if (type != NostalgicId)
            return param[type];
        else
            return "";
    }
    
    inline void setParam(NostalgicParameterType type, String val) { param.set(type, val);}
    
    void print(void)
    {
        
    }
    
    inline String getName(void) const noexcept {return name;}
    inline void setName(String newName) {name = newName;}
    
    inline void setPosition(int x, int y) { X=x;Y=y;}
    inline Point<int> getPosition(void) { return Point<int>(X,Y);}
    inline void setPosition(Point<int> point) { X = point.getX(); Y= point.getY();}
    inline void setX(int x) { X = x; }
    inline void setY(int y) { Y = y; }
    inline int getX(void) const noexcept { return X; }
    inline int getY(void) const noexcept { return Y; }

    
private:
    int Id;
    String name;
    StringArray          param;
    
    int X,Y;
    
    JUCE_LEAK_DETECTOR(NostalgicModPreparation);
};



#endif  // PREPARATION_H_INCLUDED
