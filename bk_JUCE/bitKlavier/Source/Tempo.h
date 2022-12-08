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

#include "GenericProcessor.h"
class TempoModification;

class TempoPreparation : public ReferenceCountedObject
{
public:
    typedef ReferenceCountedObjectPtr<TempoPreparation>   Ptr;
    typedef Array<TempoPreparation::Ptr>                  PtrArr;
    typedef Array<TempoPreparation::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<TempoPreparation>                  Arr;
    typedef OwnedArray<TempoPreparation, CriticalSection> CSArr;
    
    // Copy Constructor
    TempoPreparation(TempoPreparation::Ptr s) :
    sWhichTempoSystem(s->getTempoSystem()),
    sTempo(s->getTempo()),
    subdivisions(s->getSubdivisions()),
    at1History(s->getAdaptiveTempo1History()),
    at1Min(s->getAdaptiveTempo1Min()),
    at1Max(s->getAdaptiveTempo1Max()),
    at1Subdivisions(s->getAdaptiveTempo1Subdivisions()),
    at1Mode(s->getAdaptiveTempo1Mode())
    {
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
    }
    
    void performModification(TempoModification* s, Array<bool> dirty);
    
    void stepModdables()
    {
        sTempo.step();
        subdivisions.step();
        sWhichTempoSystem.step();
        at1History.step();
        at1Min.step();
        at1Max.step();
        at1Subdivisions.step();
        at1Mode.step();
    }
    
    void resetModdables()
    {
        sTempo.reset();
        subdivisions.reset();
        sWhichTempoSystem.reset();
        at1History.reset();
        at1Min.reset();
        at1Max.reset();
        at1Subdivisions.reset();
        at1Mode.reset();
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
        at1Subdivisions = (TempoType)r[idx++];
		at1Mode = (AdaptiveTempo1Mode)(int)(r[idx++] * AdaptiveTempo1ModeNil);
	}
    
    inline const TempoType getTempoSystem() const noexcept      {return sWhichTempoSystem.value; }
    inline const float getTempo() const noexcept                {return sTempo.value; }
    inline const float getBeatThresh() const noexcept           {return (60.0/sTempo.value); }
    inline const float getBeatThreshMS() const noexcept         {return (60.0/sTempo.value) * 1000.; }
  
    //Adaptive Tempo 1
    inline AdaptiveTempo1Mode getAdaptiveTempo1Mode(void)       {return at1Mode.value;   }
    inline int getAdaptiveTempo1History(void)                   {return at1History.value;}
    inline float getAdaptiveTempo1Subdivisions(void)            {return at1Subdivisions.value;}
    inline float getAdaptiveTempo1Min(void)                     {return at1Min.value;}
    inline float getAdaptiveTempo1Max(void)                     {return at1Max.value;}

    inline const String getName() const noexcept                {return name;}
    inline void setName(String n)                               {name = n;}
    inline void setTempoSystem(TempoType ts)                    {sWhichTempoSystem = ts;}
    inline void setTempo(float tempo)                           { sTempo = tempo; }
    
    inline void setHostTempo(float tempo)
    {
        if(sWhichTempoSystem == HostTempo){
            sTempo = tempo;
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
        return subdivisions.value;
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
        DBG("sTempo: " + String(sTempo.value));
        DBG("| - - - - - - - - -- - - - - - - - - |");
    }
    
    ValueTree getState(void)
    {
        ValueTree prep("params");
        
        sTempo.getState(prep, ptagTempo_tempo);
        sWhichTempoSystem.getState(prep, ptagTempo_system);
        at1Mode.getState(prep, ptagTempo_at1Mode);
        at1History.getState(prep, ptagTempo_at1History);
        at1Subdivisions.getState(prep, ptagTempo_at1Subdivisions);
        at1Min.getState(prep, ptagTempo_at1Min);
        at1Max.getState(prep, ptagTempo_at1Max);
        subdivisions.getState(prep, "subdivisions");
        
        return prep;
    }
    
    void setState(XmlElement* e)
    {
        sTempo.setState(e, ptagTempo_tempo, 120);
        sWhichTempoSystem.setState(e, ptagTempo_system, ConstantTempo);
        at1Mode.setState(e, ptagTempo_at1Mode, TimeBetweenNotes);
        at1History.setState(e, ptagTempo_at1History, 4);
        at1Subdivisions.setState(e, ptagTempo_at1Subdivisions, 1.);
        at1Min.setState(e, ptagTempo_at1Min, 100);
        at1Max.setState(e, ptagTempo_at1Max, 2000);
        subdivisions.setState(e, "subdivisions", 1.0);
    }
    
    bool modded;
    
    Moddable<TempoType> sWhichTempoSystem;
    
    Moddable<float> sTempo;
    
    Moddable<float> subdivisions;
    
    // Adaptive Tempo 1
    Moddable<int> at1History;
    Moddable<float> at1Min, at1Max;
    Moddable<float> at1Subdivisions;
    Moddable<AdaptiveTempo1Mode> at1Mode;
    
private:
    String name;
    
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

class TempoProcessor  : public GenericProcessor
{
    
public:
//    typedef ReferenceCountedObjectPtr<TempoProcessor>   Ptr;
//    typedef Array<TempoProcessor::Ptr>                  PtrArr;
//    typedef Array<TempoProcessor::Ptr, CriticalSection> CSPtrArr;
//    typedef OwnedArray<TempoProcessor>                  Arr;
//    typedef OwnedArray<TempoProcessor,CriticalSection>  CSArr;
    
    TempoProcessor(BKAudioProcessor& processor, Tempo::Ptr tempo);
    
    ~TempoProcessor();
    
    void processBlock(AudioSampleBuffer& buffer, MidiBuffer& midiMessages, int numSamples, int midiChannel, BKSampleLoadType type);
    void keyPressed(int noteNumber, Array<float>& targetVelocities, bool fromPress);
    void keyReleased(int noteNumber, Array<float>& targetVelocities, bool fromPress);
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
    
    Array<Array<float>>& getVelocities() { return velocities; }
    Array<Array<float>>& getInvertVelocities() { return invertVelocities; }
    
    void setVelocities(Array<Array<float>>& newVel) { velocities = newVel; }
    void setInvertVelocities(Array<Array<float>>& newVel) { invertVelocities = newVel; }
    void copyProcessorState(GenericProcessor::Ptr copy)
    {
        GenericProcessor::copyProcessorState(copy);
        TempoProcessor* prev = dynamic_cast<TempoProcessor*>(copy.get());
        setAtTimer(prev->getAtTimer());
        setAtLastTime(prev->getAtLastTime());
        setAtDeltaHistory(prev->getAtDeltaHistory());
        setAdaptiveTempoPeriodMultiplier(prev->getAdaptiveTempoPeriodMultiplier());
    }
    
    void prepareToPlay(GeneralSettings::Ptr)
    {
        
    }

    void handleMidiEvent (const MidiMessage& m)
    {
        
    }
private:
    
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
    
    Array<Array<float>> velocities;
    Array<Array<float>> invertVelocities;
    
    JUCE_LEAK_DETECTOR(TempoProcessor);
};


#endif  // TEMPO_H_INCLUDED
