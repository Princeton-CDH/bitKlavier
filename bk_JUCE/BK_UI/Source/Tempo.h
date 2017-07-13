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
        sWhichTempoSystem = s->getTempoSystem();
        at1History = s->getAdaptiveTempo1History();
        at1Min = s->getAdaptiveTempo1Min();
        at1Max = s->getAdaptiveTempo1Max();
        at1Subdivisions = s->getAdaptiveTempo1Subdivisions();
        at1Mode = s->getAdaptiveTempo1Mode();
        
        sBeatThreshSec = (60.0/sTempo);
        sBeatThreshMS = sBeatThreshSec * 1000.;
    }
    
    bool compare(TempoPreparation::Ptr s)
    {
        
        return (sTempo == s->getTempo() &&
                sWhichTempoSystem == s->getTempoSystem() &&
                at1History == s->getAdaptiveTempo1History() &&
                at1Min == s->getAdaptiveTempo1Min() &&
                at1Max == s->getAdaptiveTempo1Max() &&
                at1Subdivisions == s->getAdaptiveTempo1Subdivisions() &&
                at1Mode == s->getAdaptiveTempo1Mode());
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
    inline void setTempoSystem(TempoType ts)                    {sWhichTempoSystem = ts; DBG("Tempo System = " + String(ts));}
    inline void setTempo(float tempo)
    {
        sTempo = tempo;
        sBeatThreshSec = (60.0/sTempo);
        sBeatThreshMS = sBeatThreshSec * 1000.;
        DBG("tempo = " + String(sTempo));
    }
    
    //Adaptive Tempo 1
    inline void setAdaptiveTempo1Mode(AdaptiveTempo1Mode mode)          {at1Mode = mode; DBG("AT1mode = " + String(mode));}
    inline void setAdaptiveTempo1History(int hist)                      {at1History = hist; DBG("AT1history = " + String(hist));}
    inline void setAdaptiveTempo1Subdivisions(float sub)                {at1Subdivisions = sub; DBG("at1Subdivisions = " + String(sub));}
    inline void setAdaptiveTempo1Min(float min)                         {at1Min = min; DBG("at1Min = " + String(min));}
    inline void setAdaptiveTempo1Max(float max)                         {at1Max = max; DBG("at1Man = " + String(max));}

    
    void print(void)
    {
        DBG("| - - - Tempo Preparation - - - |");
        DBG("sTempo: " + String(sTempo));
        DBG("| - - - - - - - - -- - - - - - - - - |");
    }
    
    
private:
    String name;
    TempoType sWhichTempoSystem;
    
    float sTempo;
    float sBeatThreshSec;      //length of time between pulses, as set by tempo
    float sBeatThreshMS;
    
    // Adaptive Tempo 1
    int at1History;
    float at1Min, at1Max;
    float at1Subdivisions;
    AdaptiveTempo1Mode at1Mode;
  
    JUCE_LEAK_DETECTOR(TempoPreparation);
};


class TempoProcessor  : public ReferenceCountedObject
{
    
public:
    typedef ReferenceCountedObjectPtr<TempoProcessor>   Ptr;
    typedef Array<TempoProcessor::Ptr>                  PtrArr;
    typedef Array<TempoProcessor::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<TempoProcessor>                  Arr;
    typedef OwnedArray<TempoProcessor,CriticalSection>  CSArr;
    
    TempoProcessor(TempoPreparation::Ptr active);
    
    ~TempoProcessor();
    
    void setCurrentPlaybackSampleRate(double sr) { sampleRate = sr; /*DBG("setting tempo sample rate " + String(sampleRate));*/}
    
    void processBlock(int numSamples, int channel);
    void keyPressed(int noteNumber, float velocity);
    void keyReleased(int noteNumber, int channel);
    inline float getPeriodMultiplier(void)              {return adaptiveTempoPeriodMultiplier;}
    inline float getAdaptedTempo(void)                  {return active->getTempo() / adaptiveTempoPeriodMultiplier;}
    
    void  reset();
    
private:
    int Id;
    GeneralSettings::Ptr general;
    TempoPreparation::Ptr active;
    
    double sampleRate;
    
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

class Tempo : public ReferenceCountedObject
{
    
public:
    typedef ReferenceCountedObjectPtr<Tempo>   Ptr;
    typedef Array<Tempo::Ptr>                  PtrArr;
    typedef Array<Tempo::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<Tempo>                  Arr;
    typedef OwnedArray<Tempo, CriticalSection> CSArr;
    
    
    Tempo(TempoPreparation::Ptr prep,
          int Id,
          BKUpdateState::Ptr us):
    sPrep(new TempoPreparation(prep)),
    aPrep(new TempoPreparation(sPrep)),
    processor(new TempoProcessor(aPrep)),
    Id(Id),
    name(String(Id)),
    updateState(us),
    X(-1),Y(-1),
    editted(false)
    {
        
    }
    
    Tempo(int Id,
          BKUpdateState::Ptr us):
    Id(Id),
    name(String(Id)),
    updateState(us),
    X(-1),Y(-1),
    editted(false)
    {
        sPrep = new TempoPreparation();
        aPrep = new TempoPreparation(sPrep);
        processor = new TempoProcessor(aPrep);
    }
    
    void prepareToPlay(double sampleRate)
    {
        processor->setCurrentPlaybackSampleRate(sampleRate);
    }
    
    
    inline ValueTree getState(void)
    {
        ValueTree prep(vtagTempo + String(Id));
        
        prep.setProperty( ptagTempo_Id,                    Id, 0);
        prep.setProperty( "name",                          name, 0);
        prep.setProperty( ptagTempo_tempo,                 sPrep->getTempo(), 0);
        prep.setProperty( ptagTempo_system,                sPrep->getTempoSystem(), 0);
        prep.setProperty( ptagTempo_at1Mode,               sPrep->getAdaptiveTempo1Mode(), 0 );
        prep.setProperty( ptagTempo_at1History,            sPrep->getAdaptiveTempo1History(), 0 );
        prep.setProperty( ptagTempo_at1Subdivisions,       sPrep->getAdaptiveTempo1Subdivisions(), 0 );
        prep.setProperty( ptagTempo_at1Min,                sPrep->getAdaptiveTempo1Min(), 0 );
        prep.setProperty( ptagTempo_at1Max,                sPrep->getAdaptiveTempo1Max(), 0 );
        
        prep.setProperty( posX, X, 0);
        prep.setProperty( posY, Y, 0);
        
        return prep;
    }
    
    inline void setState(XmlElement* e)
    {
        editted = true;
        
        float f; int i;
        
        String n = e->getStringAttribute("name");
        
        if (n != String::empty)     name = n;
        else                        name = String(Id);
        
        f = e->getStringAttribute(ptagTempo_tempo).getFloatValue();
        DBG("new tempo::: " + String(f));
        sPrep->setTempo(f);
        
        i = e->getStringAttribute(ptagTempo_system).getIntValue();
        sPrep->setTempoSystem((TempoType)i);
        
        i = e->getStringAttribute(ptagTempo_at1Mode).getIntValue();
        sPrep->setAdaptiveTempo1Mode((AdaptiveTempo1Mode)i);
        
        i = e->getStringAttribute(ptagTempo_at1History).getIntValue();
        sPrep->setAdaptiveTempo1History(i);
        
        f = e->getStringAttribute(ptagTempo_at1Subdivisions).getFloatValue();
        sPrep->setAdaptiveTempo1Subdivisions(f);
        
        f = e->getStringAttribute(ptagTempo_at1Min).getFloatValue();
        sPrep->setAdaptiveTempo1Min(f);
        
        f = e->getStringAttribute(ptagTempo_at1Max).getFloatValue();
        sPrep->setAdaptiveTempo1Max(f);
        
        n = e->getStringAttribute(posX);
        if (n != String::empty) X = n.getIntValue();
        else                    X = -1;
        
        n = e->getStringAttribute(posY);
        if (n != String::empty) Y = n.getIntValue();
        else                    Y = -1;
        
        
        aPrep->copy(sPrep);
    }
    
    ~Tempo() {};
    
    inline int getId() {return Id;};
    
    
    TempoPreparation::Ptr      sPrep;
    TempoPreparation::Ptr      aPrep;
    TempoProcessor::Ptr        processor;
    
    
    void reset()
    {
        aPrep->copy(sPrep);
        processor->reset();
        DBG("resetting tempo");
    }
    
    inline void copy(Tempo::Ptr from)
    {
        sPrep->copy(from->sPrep);
        aPrep->copy(sPrep);
    }
    
    inline String getName(void) const noexcept {return name;}
    
    inline void setName(String newName)
    {
        name = newName;
        updateState->tempoPreparationDidChange = true;
    }
    
    inline void setPosition(int x, int y) { X=x;Y=y;}
    inline Point<int> getPosition(void) { return Point<int>(X,Y);}
    inline void setPosition(Point<int> point) { X = point.getX(); Y= point.getY();}
    inline void setX(int x) { X = x; }
    inline void setY(int y) { Y = y; }
    inline int getX(void) const noexcept { return X; }
    inline int getY(void) const noexcept { return Y; }
    
    bool editted;
private:
    int Id;
    String name;
    
    BKUpdateState::Ptr updateState;
    
    int X,Y;
    
    JUCE_LEAK_DETECTOR(Tempo)
};

class TempoModPreparation : public ReferenceCountedObject
{
public:
    
    typedef ReferenceCountedObjectPtr<TempoModPreparation>   Ptr;
    typedef Array<TempoModPreparation::Ptr>                  PtrArr;
    typedef Array<TempoModPreparation::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<TempoModPreparation>                  Arr;
    typedef OwnedArray<TempoModPreparation, CriticalSection> CSArr;
    
    /*
     TempoId = 0,
     Tempo,
     TempoMode,
     AT1History,
     AT1Subdivisions,
     AT1Min,
     AT1Max,
     AT1Mode,
     */
    
    TempoModPreparation(TempoPreparation::Ptr p, int Id):
    Id(Id),
    X(-1),Y(-1),
    editted(false)
    {
        param.ensureStorageAllocated(cTempoParameterTypes.size());
        
        param.set(TempoBPM, String(p->getTempo()));
        param.set(TempoSystem, String(p->getTempoSystem()));
        param.set(AT1History, String(p->getAdaptiveTempo1History()));
        param.set(AT1Subdivisions, String(p->getAdaptiveTempo1Subdivisions()));
        param.set(AT1Min, String(p->getAdaptiveTempo1Min()));
        param.set(AT1Max, String(p->getAdaptiveTempo1Max()));
        param.set(AT1Mode, String(p->getAdaptiveTempo1Mode()));
        
    }
    
    
    TempoModPreparation(int Id):
    Id(Id),
    X(-1),Y(-1),
    editted(false)
    {
        param.set(TempoBPM, "");
        param.set(TempoSystem, "");
        param.set(AT1History, "");
        param.set(AT1Subdivisions, "");
        param.set(AT1Min, "");
        param.set(AT1Max, "");
        param.set(AT1Mode, "");
    }
    
    
    ~TempoModPreparation(void)
    {
        
    }
    
    inline void copy(TempoPreparation::Ptr p)
    {
        param.set(TempoBPM, String(p->getTempo()));
        param.set(TempoSystem, String(p->getTempoSystem()));
        param.set(AT1History, String(p->getAdaptiveTempo1History()));
        param.set(AT1Subdivisions, String(p->getAdaptiveTempo1Subdivisions()));
        param.set(AT1Min, String(p->getAdaptiveTempo1Min()));
        param.set(AT1Max, String(p->getAdaptiveTempo1Max()));
        param.set(AT1Mode, String(p->getAdaptiveTempo1Mode()));
    }
    
    inline void copy(TempoModPreparation::Ptr p)
    {
        for (int i = TempoId+1; i < TempoParameterTypeNil; i++)
        {
            param.set(i, p->getParam((TempoParameterType)i));
        }
    }
    
    inline bool compare(TempoModPreparation::Ptr t)
    {
        return (getParam(TempoBPM)          == t->getParam(TempoBPM) &&
                getParam(TempoSystem)       == t->getParam(TempoSystem) &&
                getParam(AT1History)        == t->getParam(AT1History) &&
                getParam(AT1Subdivisions)   == t->getParam(AT1Subdivisions) &&
                getParam(AT1Min)            == t->getParam(AT1Min) &&
                getParam(AT1Max)            == t->getParam(AT1Max) &&
                getParam(AT1Mode)           == t->getParam(AT1Mode));
    }
    
    void clearAll()
    {
        for (int i = TempoId+1; i < TempoParameterTypeNil; i++)
        {
            param.set(i, "");
        }
    }
    
    inline void setId(int newId) { Id = newId; }
    inline int getId(void) const noexcept { return Id; }
    
    inline ValueTree getState(int Id)
    {
        
        ValueTree prep(vtagModTempo + String(Id));
        
        prep.setProperty("name",name,0);
        
        String p = getParam(TempoBPM);
        if (p != String::empty) prep.setProperty( ptagTempo_tempo, p.getFloatValue(), 0);
        
        p = getParam(TempoSystem);
        if (p != String::empty) prep.setProperty( ptagTempo_system, p.getIntValue(), 0);
        
        p = getParam(AT1History);
        if (p != String::empty) prep.setProperty( ptagTempo_at1History, p.getIntValue(), 0 );
        
        p = getParam(AT1Subdivisions);
        if (p != String::empty) prep.setProperty( ptagTempo_at1Subdivisions, p.getIntValue(), 0 );
        
        p = getParam(AT1Min);
        if (p != String::empty) prep.setProperty( ptagTempo_at1Min, p.getIntValue(), 0 );
        
        p = getParam(AT1Max);
        if (p != String::empty) prep.setProperty( ptagTempo_at1Max, p.getIntValue(), 0 );
        
        p = getParam(AT1Mode);
        if (p != String::empty) prep.setProperty( ptagTempo_at1Mode, p.getIntValue(), 0 );
        
    
        return prep;
        
    }
    
    inline void setState(XmlElement* e)
    {
        String n = e->getStringAttribute("name");
        
        if (n != String::empty)     name = n;
        else                        name = "mm"+String(Id);
 
        String p = e->getStringAttribute(ptagTempo_tempo);
        setParam(TempoBPM, p);
        
        p = e->getStringAttribute(ptagTempo_system);
        setParam(TempoSystem, p);
        
        p = e->getStringAttribute(ptagTempo_at1Mode);
        setParam(AT1Mode, p);
        
        p = e->getStringAttribute(ptagTempo_at1History);
        setParam(AT1History, p);
        
        p = e->getStringAttribute(ptagTempo_at1Subdivisions);
        setParam(AT1Subdivisions, p);
        
        p = e->getStringAttribute(ptagTempo_at1Min);
        setParam(AT1Min, p);
        
        p = e->getStringAttribute(ptagTempo_at1Max);
        setParam(AT1Max, p);
        
    }
    
    
    inline const String getParam(TempoParameterType type)
    {
        if (type != TempoId)   return param[type];
        else                    return "";
    }
    
    inline const StringArray getStringArray(void) { return param; }
    
    inline void setParam(TempoParameterType type, String val)
    {
        param.set(type, val);
    }
    
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
    
    bool editted;
    
private:
    int Id;
    String name;
    StringArray          param;
    
    int X,Y;
    
    
    
    
    JUCE_LEAK_DETECTOR(TempoModPreparation);
};


#endif  // TEMPO_H_INCLUDED
