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
    at1History(4),
    at1Min(100),
    at1Max(2000),
    at1Subdivisions(1.0f),
    at1Mode(TimeBetweenNotes)
    {
        sBeatThreshSec = (60.0/sTempo);
        sBeatThreshMS = sBeatThreshSec * 1000.;
    }
    
    inline void copy(TempoPreparation::Ptr s)
    {
        sTempo = s->getTempo();
        subdivisions = s->getSubdivisions();
        sWhichTempoSystem = s->getTempoSystem();
        at1History = s->getAdaptiveTempo1History();
        at1Min = s->getAdaptiveTempo1Min();
        at1Max = s->getAdaptiveTempo1Max();
        at1Subdivisions = s->getAdaptiveTempo1Subdivisions();
        at1Mode = s->getAdaptiveTempo1Mode();
        
        sBeatThreshSec = (60.0/sTempo);
        sBeatThreshMS = sBeatThreshSec * 1000.;
    }
    
    void performModification(TempoPreparation::Ptr s, Array<bool> dirty)
    {
        if (dirty[TempoBPM]) sTempo = s->getTempo();
        if (dirty[TempoSubdivisions]) subdivisions = s->getSubdivisions();
        if (dirty[TempoSystem]) sWhichTempoSystem = s->getTempoSystem();
        if (dirty[AT1History]) at1History = s->getAdaptiveTempo1History();
        if (dirty[AT1Min]) at1Min = s->getAdaptiveTempo1Min();
        if (dirty[AT1Max]) at1Max = s->getAdaptiveTempo1Max();
        if (dirty[AT1Subdivisions]) at1Subdivisions = s->getAdaptiveTempo1Subdivisions();
        if (dirty[AT1Mode]) at1Mode = s->getAdaptiveTempo1Mode();
        
        sBeatThreshSec = (60.0/sTempo);
        sBeatThreshMS = sBeatThreshSec * 1000.;
    }
    
    void stepModdables()
    {
//        dGain.step();
    }
    
    void resetModdables()
    {
//        dGain.reset();
    }
    
    bool compare(TempoPreparation::Ptr s)
    {
        
        return (sTempo == s->getTempo() &&
                subdivisions == s->getSubdivisions() &&
                sWhichTempoSystem == s->getTempoSystem() &&
                at1History == s->getAdaptiveTempo1History() &&
                at1Min == s->getAdaptiveTempo1Min() &&
                at1Max == s->getAdaptiveTempo1Max() &&
                at1Subdivisions == s->getAdaptiveTempo1Subdivisions() &&
                at1Mode == s->getAdaptiveTempo1Mode());
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
		at1History = r[idx++];
		at1Min = r[idx++];
		at1Max = r[idx++];
		at1Subdivisions = r[idx++];
		at1Mode = (AdaptiveTempo1Mode)(int)(r[idx++] * AdaptiveTempo1ModeNil);
	}
    
    inline const TempoType getTempoSystem() const noexcept      {return sWhichTempoSystem; }
    inline const float getTempo() const noexcept                {return sTempo; }
    inline const float getBeatThresh() const noexcept           {return sBeatThreshSec; }
    inline const float getBeatThreshMS() const noexcept         {return sBeatThreshMS; }
  
    //Adaptive Tempo 1
    inline AdaptiveTempo1Mode getAdaptiveTempo1Mode(void)       {return at1Mode;   }
    inline int getAdaptiveTempo1History(void)                   {return at1History;}
    inline float getAdaptiveTempo1Subdivisions(void)            {return at1Subdivisions;}
    inline float getAdaptiveTempo1Min(void)                     {return at1Min;}
    inline float getAdaptiveTempo1Max(void)                     {return at1Max;}

    inline const String getName() const noexcept                {return name;}
    inline void setName(String n)                               {name = n;}
    inline void setTempoSystem(TempoType ts)                    {sWhichTempoSystem = ts;}
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
    inline void setAdaptiveTempo1Mode(AdaptiveTempo1Mode mode)          {at1Mode = mode;}
    inline void setAdaptiveTempo1History(int hist)                      {at1History = hist;}
    inline void setAdaptiveTempo1Subdivisions(float sub)                {at1Subdivisions = sub;}
    inline void setAdaptiveTempo1Min(float min)                         {at1Min = min;}
    inline void setAdaptiveTempo1Max(float max)                         {at1Max = max;}
    
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
        prep.setProperty( ptagTempo_at1Mode,               getAdaptiveTempo1Mode(), 0 );
        prep.setProperty( ptagTempo_at1History,            getAdaptiveTempo1History(), 0 );
        prep.setProperty( ptagTempo_at1Subdivisions,       getAdaptiveTempo1Subdivisions(), 0 );
        prep.setProperty( ptagTempo_at1Min,                getAdaptiveTempo1Min(), 0 );
        prep.setProperty( ptagTempo_at1Max,                getAdaptiveTempo1Max(), 0 );
        prep.setProperty( "subdivisions",                  getSubdivisions(), 0);
        
        return prep;
    }
    
    void setState(XmlElement* e)
    {
        float f; int i;
        f = e->getStringAttribute(ptagTempo_tempo).getFloatValue();
        setTempo(f);
        
        i = e->getStringAttribute(ptagTempo_system).getIntValue();
        setTempoSystem((TempoType)i);
        
        i = e->getStringAttribute(ptagTempo_at1Mode).getIntValue();
        setAdaptiveTempo1Mode((AdaptiveTempo1Mode)i);
        
        i = e->getStringAttribute(ptagTempo_at1History).getIntValue();
        setAdaptiveTempo1History(i);
        
        f = e->getStringAttribute(ptagTempo_at1Subdivisions).getFloatValue();
        setAdaptiveTempo1Subdivisions(f);
        
        f = e->getStringAttribute(ptagTempo_at1Min).getFloatValue();
        setAdaptiveTempo1Min(f);
        
        f = e->getStringAttribute(ptagTempo_at1Max).getFloatValue();
        setAdaptiveTempo1Max(f);
        
        String s = e->getStringAttribute("subdivisions");
        
        if (s == "")    setSubdivisions(1.0);
        else            setSubdivisions(s.getFloatValue());
    }
    
    
private:
    String name;
    TempoType sWhichTempoSystem;
    
    float sTempo;
    float sBeatThreshSec;      //length of time between pulses, as set by tempo
    float sBeatThreshMS;
    
    float subdivisions;
    
    // Adaptive Tempo 1
    int at1History;
    float at1Min, at1Max;
    float at1Subdivisions;
    AdaptiveTempo1Mode at1Mode;
    
    JUCE_LEAK_DETECTOR(TempoPreparation);
};


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
    prep(new TempoPreparation(prep)),
    Id(Id),
    name("Tempo "+String(Id))
    {
        
    }
    
    Tempo(int Id, bool random = false):
    Id(Id),
    name("Tempo "+String(Id))
    {
		prep = new TempoPreparation();
		if (random) randomize();
    }
    
    inline void clear(void)
    {
        prep       = new TempoPreparation();
    }
    
    inline Tempo::Ptr duplicate()
    {
        TempoPreparation::Ptr copyPrep = new TempoPreparation(prep);
        
        Tempo::Ptr copy = new Tempo(copyPrep, -1);
        
        copy->setName(name);
        
        return copy;
    }
    
    inline ValueTree getState(bool active = false)
    {
        ValueTree vt(vtagTempo);
        
        vt.setProperty( "Id",Id, 0);
        vt.setProperty( "name",                          name, 0);
        
        vt.addChild(prep->getState(), -1, 0);
    
        return vt;
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
            prep->setState(params);
        }
        else
        {
            prep->setState(e);
        }
    }
    
    ~Tempo() {};
    
    inline int getId() {return Id;}
    inline void setId(int newId) { Id = newId;}
    
    TempoPreparation::Ptr      prep;
    
    void reset()
    {
//        aPrep->copy(prep);
    }
    
    inline void copy(Tempo::Ptr from)
    {
        prep->copy(from->prep);
    }

	inline void randomize()
	{
		clear();
		prep->randomize();
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
        return ((tempo->prep->getTempoSystem() == AdaptiveTempo1) ? adaptiveTempoPeriodMultiplier : 1.0);
        
    }
    inline float getAdaptedTempo(void)                  {return tempo->prep->getTempo() / adaptiveTempoPeriodMultiplier;}
    
    void  adaptiveReset();

    inline int getId(void) const noexcept { return tempo->getId(); }
    
    inline void setTempo(Tempo::Ptr newTempo) { tempo = newTempo;}
    inline Tempo::Ptr getTempo(void) const noexcept { return tempo; }
    
    inline void prepareToPlay(double sr)
    {

    }
    
    inline void reset(void)
    {
        tempo->prep->resetModdables();
        adaptiveReset();
    }
    
    uint64 getAtTimer() { return atTimer; }
    uint64 getAtLastTime() { return atLastTime; }
    int getAtDelta();
    Array<int> getAtDeltaHistory() { return atDeltaHistory; }
    float getAdaptiveTempoPeriodMultiplier() { return adaptiveTempoPeriodMultiplier; }
    
    void setAtTimer(uint64 newval) { atTimer = newval; }
    void setAtLastTime(uint64 newval) { atLastTime = newval; }
    void setAtDeltaHistory(Array<int> newvals)
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
    int atDelta;                //in ms
    Array<int> atDeltaHistory;  //in ms
    void atNewNote();
    void atNewNoteOff();
    void atCalculatePeriodMultiplier();
    float adaptiveTempoPeriodMultiplier;
    
    
    JUCE_LEAK_DETECTOR(TempoProcessor);
};


#endif  // TEMPO_H_INCLUDED
