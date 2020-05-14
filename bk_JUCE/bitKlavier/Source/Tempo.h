/*
  ==============================================================================

    Tempo.h
    Created: 26 Feb 2017 11:38:35pm
    Author:  Daniel Trueman

  ==============================================================================
*/

#ifndef TEMPO_H_INCLUDED
#define TEMPO_H_INCLUDED

#include "BKUtilities.h"
#include "AudioConstants.h"
#include "General.h"

#include "BKUpdateState.h"

#include "Keymap.h"

/*
TempoPreparation holds all the state variable values for the
Tempo preparation. As with other preparation types, bK will use
two instantiations of TempoPreparation for every active
Tuning in the gallery, one to store the static state of the
preparation, and the other to store the active state. These will
be the same, unless a Modification is triggered, in which case the
active state will be changed (and a Reset will revert the active state
to the static state).
*/

class TempoPreparation : public ReferenceCountedObject
{
public:
    typedef ReferenceCountedObjectPtr<TempoPreparation>   Ptr;
    typedef Array<TempoPreparation::Ptr>                  PtrArr;
    typedef Array<TempoPreparation::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<TempoPreparation>                  Arr;
    typedef OwnedArray<TempoPreparation, CriticalSection> CSArr;
    
    // Copy Constructor
    TempoPreparation(TempoPreparation::Ptr p)
    {
        copy(p);
        
        sBeatThreshSec = (60.0/sTempo);
        sBeatThreshMS = sBeatThreshSec * 1000.;
    }
    
    
    TempoPreparation():
    sWhichTempoSystem(ConstantTempo),
    sTempo(120),
    subdivisions(1.),
    atMinPulse(100),
    atMaxPulse(2000),
    atSubdivisions(1.0f),
    atDeltaHistorySize(4),
    metricWeights({0.0, 1.0, 0.8, 0.8, 0.6, 0.0, 0.6, 0.0, 0.4,
        0.6, 0.0, 0.0, 0.4, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.4, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.4}),
    emaAlpha(0.5f),
    useExponential(false),
    useWeights(false)
    {
        sBeatThreshSec = (60.0/sTempo);
        sBeatThreshMS = sBeatThreshSec * 1000.;
        atMode.ensureStorageAllocated(AdaptiveTempoModeNil);
        atMode.set(TimeBetweenOnsets, true);
        atMode.set(NoteLength, false);
        atMode.set(TimeBetweenReleases, false);
        
//        metricWeights.ensureStorageAllocated(28);
//        for (int i = 0; i <= 27; i++)
//        {
//            metricWeights.add(0);
//        }
        
    }
    
    inline void copy(TempoPreparation::Ptr s)
    {
        sTempo = s->getTempo();
        subdivisions = s->getSubdivisions();
        sWhichTempoSystem = s->getTempoSystem();
        atDeltaHistorySize = s->getAdaptiveTempoHistorySize();
        atMinPulse = s->getAdaptiveTempoMin();
        atMaxPulse = s->getAdaptiveTempoMax();
        atSubdivisions = s->getAdaptiveTempoSubdivisions();
        atMode = s->getAdaptiveTempoMode();
        
        metricWeights = s->getAdaptiveTempoWeights();
        emaAlpha = s->getAdaptiveTempoAlpha();
        
        sBeatThreshSec = (60.0/sTempo);
        sBeatThreshMS = sBeatThreshSec * 1000.;
    }
    
    inline void performModification(TempoPreparation::Ptr s, Array<bool> dirty)
    {
        if (dirty[TempoBPM]) sTempo = s->getTempo();
        if (dirty[TempoSubdivisions]) subdivisions = s->getSubdivisions();
        if (dirty[TempoSystem]) sWhichTempoSystem = s->getTempoSystem();
        if (dirty[ATHistory]) atDeltaHistorySize = s->getAdaptiveTempoHistorySize();
        if (dirty[ATMin]) atMinPulse = s->getAdaptiveTempoMin();
        if (dirty[ATMax]) atMaxPulse = s->getAdaptiveTempoMax();
        if (dirty[ATSubdivisions]) atSubdivisions = s->getAdaptiveTempoSubdivisions();
        if (dirty[ATMode]) atMode = s->getAdaptiveTempoMode();
        
        sBeatThreshSec = (60.0/sTempo);
        sBeatThreshMS = sBeatThreshSec * 1000.;
    }
    
    bool compare(TempoPreparation::Ptr s)
    {
        
        return (sTempo == s->getTempo() &&
                subdivisions == s->getSubdivisions() &&
                sWhichTempoSystem == s->getTempoSystem() &&
                atDeltaHistorySize == s->getAdaptiveTempoHistorySize() &&
                atMinPulse == s->getAdaptiveTempoMin() &&
                atMaxPulse == s->getAdaptiveTempoMax() &&
                atSubdivisions == s->getAdaptiveTempoSubdivisions() &&
                atMode == s->getAdaptiveTempoMode());
    }

    // for unit-testing
	inline void randomize(void)
	{
		Random::getSystemRandom().setSeedRandomly();

		float r[12];

		for (int i = 0; i < 10; i++)  r[i] = (Random::getSystemRandom().nextFloat());
		int idx = 0;

		sTempo = r[idx++];
        subdivisions = r[idx++] * 10.0;
		sWhichTempoSystem = (TempoType)(int)(r[idx++] * TempoSystemNil);
		atDeltaHistorySize = r[idx++];
		atMinPulse = r[idx++];
		atMaxPulse = r[idx++];
		atSubdivisions = r[idx++];
		atMode.set((r[idx++] * AdaptiveTempoModeNil), true);
	}
    
    inline const TempoType getTempoSystem() const noexcept      { return sWhichTempoSystem; }
    inline const float getTempo() const noexcept                { return sTempo; }
    inline const float getBeatThresh() const noexcept           { return sBeatThreshSec; }
    inline const float getBeatThreshMS() const noexcept         { return sBeatThreshMS; }
  
    //Adaptive Tempo 1
    inline Array<bool> getAdaptiveTempoMode(void)               { return atMode;   }
    inline int getAdaptiveTempoHistorySize(void)                { return atDeltaHistorySize; }
    inline float getAdaptiveTempoSubdivisions(void)             { return atSubdivisions; }
    inline float getAdaptiveTempoMin(void)                      { return atMinPulse; }
    inline float getAdaptiveTempoMax(void)                      { return atMaxPulse; }
    
    inline Array<float> getAdaptiveTempoWeights(void)           { return metricWeights;}
    inline float getAdaptiveTempoAlpha(void)                    { return emaAlpha; }
    inline bool getUseExponential(void)                         { return useExponential; }
    inline bool getUseWeights(void)                             { return useWeights; }

    inline const String getName() const noexcept                { return name;}
    inline void setName(String n)                               { name = n; }
    inline void setTempoSystem(TempoType ts)                    { sWhichTempoSystem = ts; }
    inline void setTempo(float tempo)
    {
        sTempo = tempo;
        sBeatThreshSec = (60.0/sTempo);
        sBeatThreshMS = sBeatThreshSec * 1000.;
        //DBG("tempo = " + String(sTempo));
    }
    
    inline void setHostTempo(float tempo)
    {
        if(sWhichTempoSystem == HostTempo){
            sTempo = tempo;
            sBeatThreshSec = (60.0/sTempo);
            sBeatThreshMS = sBeatThreshSec * 1000.;
            //DBG("tempo = " + String(sTempo));
        }
    }
    
    bool getHostTempo()
    {
        if(sWhichTempoSystem == HostTempo) return true;
        return false;
    }
    
    void setSubdivisions(float sub)
    {
        subdivisions = sub;
    }
    
    float getSubdivisions(void)
    {
        return subdivisions;
    }
    
    //Adaptive Tempo 1
    inline void setAdaptiveTempoMode(Array<bool> mode)                 { atMode = mode; }
    inline void setAdaptiveTempoHistory(int hist)                      { atDeltaHistorySize = hist; }
    inline void setAdaptiveTempoSubdivisions(float sub)                { atSubdivisions = sub; }
    inline void setAdaptiveTempoMin(float min)                         { atMinPulse = min; }
    inline void setAdaptiveTempoMax(float max)                         { atMaxPulse = max; }
    
    inline void setAdaptiveTempoWeights(Array<float> weights)          { metricWeights = weights; }
    inline void setAdaptiveTempoAlpha(float alpha)                     { emaAlpha = alpha; }
    inline void setUseExponential(bool use)                            { useExponential = use; }
    inline void setUseWeights(bool use)                                { useWeights = use; }
    
    void print(void)
    {
        DBG("| - - - Tempo Preparation - - - |");
        DBG("sTempo: " + String(sTempo));
        DBG("| - - - - - - - - -- - - - - - - - - |");
    }
    
    ValueTree getState(void)
    {
        ValueTree prep("params");
        
        prep.setProperty( ptagTempo_tempo,                 getTempo(), 0);
        prep.setProperty( ptagTempo_system,                getTempoSystem(), 0);
        
        for (int i = TimeBetweenOnsets; i < AdaptiveTempoModeNil; i++)
        {
            prep.setProperty(ptagTempo_atMode + String(i), atMode.getUnchecked(i) ? 1 : 0, 0);
        }
        
        prep.setProperty( ptagTempo_atHistory,             getAdaptiveTempoHistorySize(), 0 );
        prep.setProperty( ptagTempo_atSubdivisions,        getAdaptiveTempoSubdivisions(), 0 );
        prep.setProperty( ptagTempo_atMin,                 getAdaptiveTempoMin(), 0 );
        prep.setProperty( ptagTempo_atMax,                 getAdaptiveTempoMax(), 0 );
        prep.setProperty( "subdivisions",                  getSubdivisions(), 0);
        
        ValueTree weights( vtagTempo_atWeights);
        int count = 0;
        for (auto w : getAdaptiveTempoWeights())
        {
            weights.setProperty( ptagFloat + String(count++), w, 0);
        }
        prep.addChild(weights, -1, 0);
        
        return prep;
    }
    
    void setState(XmlElement* e)
    {
        float f; int i;
        f = e->getStringAttribute(ptagTempo_tempo).getFloatValue();
        setTempo(f);
        
        i = e->getStringAttribute(ptagTempo_system).getIntValue();
        setTempoSystem((TempoType)i);
        
        Array<bool> mode;
        mode.ensureStorageAllocated(AdaptiveTempoModeNil);
        for (int j = 0; j < AdaptiveTempoModeNil; j++)
        {
            mode.set(j, false);
            String attr = e->getStringAttribute(ptagTempo_atMode + String(j));
            if (attr != String()) mode.set(j, (bool) attr.getIntValue());
        }
        
        
        
        // to stay compatible with old xmls
        String attr = e->getStringAttribute(ptagTempo_atMode);
        if (attr != String()) mode.set(attr.getIntValue(), true);
        setAdaptiveTempoMode(mode);
        
        i = e->getStringAttribute(ptagTempo_atHistory).getIntValue();
        setAdaptiveTempoHistory(i);
        
        f = e->getStringAttribute(ptagTempo_atSubdivisions).getFloatValue();
        setAdaptiveTempoSubdivisions(f);
        
        f = e->getStringAttribute(ptagTempo_atMin).getFloatValue();
        setAdaptiveTempoMin(f);
        
        f = e->getStringAttribute(ptagTempo_atMax).getFloatValue();
        setAdaptiveTempoMax(f);
        
        String s = e->getStringAttribute("subdivisions");
        
        if (s == "")    setSubdivisions(1.0);
        else            setSubdivisions(s.getFloatValue());
        
        forEachXmlChildElement (*e, sub)
        {
            if (sub->hasTagName(vtagTempo_atWeights))
            {
                Array<float> weights;
                for (int k = 0; k < sub->getNumAttributes(); k++)
                {
                    String attr = sub->getStringAttribute(ptagFloat + String(k));
                    
                    if (attr == String()) break;
                    else
                    {
                        f = attr.getFloatValue();
                        weights.add(f);
                    }
                }
                setAdaptiveTempoWeights(weights);
            }
        }
    }
    

    
private:
    String name;
    TempoType sWhichTempoSystem;
    
    float sTempo;
    float sBeatThreshSec;      //length of time between pulses, as set by tempo
    float sBeatThreshMS;
    float subdivisions;
    
    // Adaptive Tempo
    float atMinPulse, atMaxPulse;
    float atSubdivisions;
    Array<bool> atMode;
    
    // Stuff for basic moving average
    int atDeltaHistorySize;
    int atOnsetHistorySize;

    Array<float> metricWeights;
    
    float emaAlpha;
    bool useExponential;
    bool useWeights;
    
    JUCE_LEAK_DETECTOR(TempoPreparation);
};


/*
This class owns two TempoPreparations: sPrep and aPrep
As with other preparation, sPrep is the static preparation, while
aPrep is the active preparation currently in use. sPrep and aPrep
remain the same unless a Modification is triggered, which will change
aPrep but not sPrep. aPrep will be restored to sPrep when a Reset
is triggered.
*/

class Tempo : public ReferenceCountedObject
{
    
public:
    typedef ReferenceCountedObjectPtr<Tempo>   Ptr;
    typedef Array<Tempo::Ptr>                  PtrArr;
    typedef Array<Tempo::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<Tempo>                  Arr;
    typedef OwnedArray<Tempo, CriticalSection> CSArr;
    
    
    Tempo(TempoPreparation::Ptr prep,
          int Id):
    sPrep(new TempoPreparation(prep)),
    aPrep(new TempoPreparation(sPrep)),
    Id(Id),
    name("Tempo "+String(Id))
    {
        
    }
    
    Tempo(int Id, bool random = false):
    Id(Id),
    name("Tempo "+String(Id))
    {
		sPrep = new TempoPreparation();
		aPrep = new TempoPreparation(sPrep);
		if (random) randomize();
    }
    
    inline void clear(void)
    {
        sPrep       = new TempoPreparation();
        aPrep       = new TempoPreparation(sPrep);
    }
    
    inline Tempo::Ptr duplicate()
    {
        TempoPreparation::Ptr copyPrep = new TempoPreparation(sPrep);
        
        Tempo::Ptr copy = new Tempo(copyPrep, -1);
        
        copy->setName(name);
        
        return copy;
    }
    
    inline ValueTree getState(bool active = false)
    {
        ValueTree prep(vtagTempo);
        
        prep.setProperty( "Id",Id, 0);
        prep.setProperty( "name",                          name, 0);
        
        prep.addChild(active ? aPrep->getState() : sPrep->getState(), -1, 0);
    
        return prep;
    }
    
    inline void setState(XmlElement* e)
    {
        Id = e->getStringAttribute("Id").getIntValue();
        
        String n = e->getStringAttribute("name");
        
        if (n != String())     name = n;
        else                        name = String(Id);
        
        
        XmlElement* params = e->getChildByName("params");
        
        if (params != nullptr)
        {
            sPrep->setState(params);
        }
        else
        {
            sPrep->setState(e);
        }
        
        aPrep->copy(sPrep);
    }
    
    ~Tempo() {};
    
    inline int getId() {return Id;}
    inline void setId(int newId) { Id = newId;}
    
    
    TempoPreparation::Ptr      sPrep;
    TempoPreparation::Ptr      aPrep;
    
    
    void reset()
    {
        aPrep->copy(sPrep);
    }
    
    inline void copy(Tempo::Ptr from)
    {
        sPrep->copy(from->sPrep);
        aPrep->copy(sPrep);
    }

	inline void randomize()
	{
		clear();
		sPrep->randomize();
		aPrep->randomize();
		name = "random";
	}
    
    inline String getName(void) const noexcept {return name;}
    
    inline void setName(String newName)
    {
        name = newName;
    }

private:
    int Id;
    String name;
    

    JUCE_LEAK_DETECTOR(Tempo)
};


/*
TempoProcessor handles events (note messages, and timing) and updates
values internally that other preparation can access as needed.
*/

class TempoProcessor  : public ReferenceCountedObject
{
    
public:
    typedef ReferenceCountedObjectPtr<TempoProcessor>   Ptr;
    typedef Array<TempoProcessor::Ptr>                  PtrArr;
    typedef Array<TempoProcessor::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<TempoProcessor>                  Arr;
    typedef OwnedArray<TempoProcessor,CriticalSection>  CSArr;
    
    TempoProcessor(BKAudioProcessor& processor, Tempo::Ptr tempo);
    
    ~TempoProcessor();
    
    void processBlock(int numSamples, int channel);
    void keyPressed(int noteNumber, float velocity);
    void keyReleased(int noteNumber, int channel);
    inline float getPeriodMultiplier(void)
    {
        return ((tempo->aPrep->getTempoSystem() == AdaptiveTempo) ? adaptiveTempoPeriodMultiplier : 1.0);
        
    }
    inline float getAdaptedTempo(void)                  {return tempo->aPrep->getTempo() / adaptiveTempoPeriodMultiplier;}
    
    void  adaptiveReset();

    inline int getId(void) const noexcept { return tempo->getId(); }
    
    inline void setTempo(Tempo::Ptr newTempo) { tempo = newTempo;}
    inline Tempo::Ptr getTempo(void) const noexcept { return tempo; }
    
    inline void prepareToPlay(double sr)
    {

    }
    
    inline void reset(void)
    {
        tempo->aPrep->copy(tempo->sPrep);
        adaptiveReset();
    }
    
    uint64 getAtTimer() { return atTimer; }
    uint64 getAtLastTime() { return atLastTime; }
    int getAtDelta();
    Array<float> getAtDeltaHistory() { return atDeltaHistory; }
    float getAdaptiveTempoPeriodMultiplier() { return adaptiveTempoPeriodMultiplier; }
    
    void setAtTimer(uint64 newval) { atTimer = newval; }
    void setAtLastTime(uint64 newval) { atLastTime = newval; }
    void setAtDeltaHistory(Array<float> newvals)
    {
        atDeltaHistory.clearQuick();
        for(int i=0; i<newvals.size(); i++)
        {
            atDeltaHistory.add(newvals.getUnchecked(i));
        }
    }
    void setAdaptiveTempoPeriodMultiplier(float val) { adaptiveTempoPeriodMultiplier = val; }
    
    inline void addKeymap(Keymap::Ptr keymap)
    {
        keymaps.add(keymap);
    }
    
    inline Keymap::PtrArr getKeymaps(void)
    {
        return keymaps;
    }
    
private:
    BKAudioProcessor& processor;
    
    GeneralSettings::Ptr general;
    
    Tempo::Ptr tempo;
    
    Keymap::PtrArr keymaps;
    
    //adaptive tempo stuff
    uint64 atTimer, atLastTime; //in samples
    float atDelta;                //in ms
    Array<float> atDeltaHistory;  //in ms
    Array<float> atWeightHistory;
    float emaSum;
    float emaCount;
    float exponentialMovingAverage;
    Array<float> atOnsetHistory;
    
    void atNewNote();
    void atNewNoteOff();
    void atCalculatePeriodMultiplier();
    float adaptiveTempoPeriodMultiplier;
    
    
    JUCE_LEAK_DETECTOR(TempoProcessor);
};


#endif  // TEMPO_H_INCLUDED
