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

class TempoPreparation : public ReferenceCountedObject
{
public:
    typedef ReferenceCountedObjectPtr<TempoPreparation>   Ptr;
    typedef Array<TempoPreparation::Ptr>                  PtrArr;
    typedef Array<TempoPreparation::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<TempoPreparation>                  Arr;
    typedef OwnedArray<TempoPreparation, CriticalSection> CSArr;
    
    // Copy Constructor
    TempoPreparation(TempoPreparation::Ptr p):
    sTempo(p->getTempo()),
    at1History(p->getAdaptiveTempo1History()),
    at1Min(p->getAdaptiveTempo1Min()),
    at1Max(p->getAdaptiveTempo1Max()),
    at1Subdivisions(p->getAdaptiveTempo1Subdivisions()),
    at1Mode(p->getAdaptiveTempo1Mode())
    {
    }
    
    
    TempoPreparation():
    sTempo(120),
    at1History(0),
    at1Min(100),
    at1Max(2000),
    at1Subdivisions(1.0f),
    at1Mode(TimeBetweenNotes)
    {
    }
    
    inline void copy(TempoPreparation::Ptr s)
    {
        sTempo = s->getTempo();
        at1History = s->getAdaptiveTempo1History();
        at1Min = s->getAdaptiveTempo1Min();
        at1Max = s->getAdaptiveTempo1Max();
        at1Subdivisions = s->getAdaptiveTempo1Subdivisions();
        at1Mode = s->getAdaptiveTempo1Mode();
    }
    
    bool compare(TempoPreparation::Ptr s)
    {
        
        return (sTempo == s->getTempo() &&
                at1History == s->getAdaptiveTempo1History() &&
                at1Min == s->getAdaptiveTempo1Min() &&
                at1Max == s->getAdaptiveTempo1Max() &&
                at1Subdivisions == s->getAdaptiveTempo1Subdivisions() &&
                at1Mode == s->getAdaptiveTempo1Mode());
    }
    
    inline const TempoSystem getTempoSystem() const noexcept { return sWhichTempoSystem; }
    inline const float getTempo() const noexcept            {return sTempo; }
    inline const float getBeatThresh() const noexcept       {return sBeatThreshSec; }
  
    //Adaptive Tempo 1
    inline AdaptiveTempo1Mode getAdaptiveTempo1Mode(void)   {return at1Mode;   }
    inline int getAdaptiveTempo1History(void)               {return at1History;}
    inline float getAdaptiveTempo1Subdivisions(void)        {return at1Subdivisions;}
    inline float getAdaptiveTempo1Min(void)                 {return at1Min;}
    inline float getAdaptiveTempo1Max(void)                 {return at1Max;}
    
    inline const String getName() const noexcept {return name;}
    inline void setName(String n){name = n;}
    
    inline void setTempo(float tempo)
    {
        sTempo = tempo;
        sBeatThreshSec = (60.0/sTempo);
    }
    
    //Adaptive Tempo 1
    inline void setAdaptiveTempo1Mode(AdaptiveTempo1Mode mode)          {at1Mode = mode;    }
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
    
    
private:
    String name;
    TempoSystem sWhichTempoSystem;
    
    float sTempo;
    float sBeatThreshSec;      //length of time between pulses, as set by tempo
    
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
    
    void setCurrentPlaybackSampleRate(double sr) { sampleRate = sr;}
    
    void processBlock(int numSamples, int channel);
    void keyPressed(int noteNumber, float velocity);
    void keyReleased(int noteNumber, int channel);
    inline float getPeriodMultiplier(void)      {return adaptiveTempoPeriodMultiplier;}
    inline float getAdaptedTempo(void)          {return adaptiveTempoPeriodMultiplier * active->getTempo();}
    
    void  reset();
    
private:
    int Id;
    GeneralSettings::Ptr general;
    TempoPreparation::Ptr active;
    
    double sampleRate;
    
    //adaptive tempo stuff
    uint64 atTimer, atLastTime; //in samples
    int atDelta;                //in sec
    Array<int> atDeltaHistory;  //in sec
    void atNewNote();
    void atNewNoteOff();
    void atCalculatePeriodMultiplier();
    float adaptiveTempoPeriodMultiplier;

    
    JUCE_LEAK_DETECTOR(TempoProcessor);
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
     TempoScale,
     TempoFundamental,
     TempoOffset,
     TempoA1IntervalScale,
     TempoA1Inversional,
     TempoA1AnchorScale,
     TempoA1AnchorFundamental,
     TempoA1ClusterThresh,
     TempoA1History,
     TempoCustomScale,
     TempoAbsoluteOffsets
     */
    
    TempoModPreparation(TempoPreparation::Ptr p)
    {
        param.ensureStorageAllocated(cTempoParameterTypes.size());
        
        /*
        param.set(TempoScale, String(p->getTempo()));
        param.set(TempoFundamental, String(p->getFundamental()));
        param.set(TempoOffset, String(p->getFundamentalOffset()));
        param.set(TempoA1IntervalScale, String(p->getAdaptiveIntervalScale()));
        param.set(TempoA1Inversional, String(p->getAdaptiveInversional()));
        param.set(TempoA1AnchorScale, String(p->getAdaptiveAnchorScale()));
        param.set(TempoA1AnchorFundamental, String(p->getAdaptiveAnchorFundamental()));
        param.set(TempoA1ClusterThresh, String(p->getAdaptiveClusterThresh()));
        param.set(TempoA1History, String(p->getAdaptiveHistory()));
        param.set(TempoCustomScale, floatArrayToString(p->getCustomScale()));
        param.set(TempoAbsoluteOffsets, floatArrayToString(p->getAbsoluteOffsets()));
        //param.set(TempoReset, intArrayToString(p->getResetMap()->keys()));
         */
        
    }
    
    
    TempoModPreparation(void)
    {
        /*
        param.set(TempoScale, "");
        param.set(TempoFundamental, "");
        param.set(TempoOffset, "");
        param.set(TempoA1IntervalScale, "");
        param.set(TempoA1Inversional, "");
        param.set(TempoA1AnchorScale, "");
        param.set(TempoA1AnchorFundamental, "");
        param.set(TempoA1ClusterThresh, "");
        param.set(TempoA1History, "");
        param.set(TempoCustomScale, "");
        param.set(TempoAbsoluteOffsets, "");
        //param.set(TempoReset, "");
         */
    }
    
    
    ~TempoModPreparation(void)
    {
        
    }
    
    inline void copy(TempoPreparation::Ptr p)
    {
        /*
        param.set(TempoScale, String(p->getTempo()));
        param.set(TempoFundamental, String(p->getFundamental()));
        param.set(TempoOffset, String(p->getFundamentalOffset()));
        param.set(TempoA1IntervalScale, String(p->getAdaptiveIntervalScale()));
        param.set(TempoA1Inversional, String(p->getAdaptiveInversional()));
        param.set(TempoA1AnchorScale, String(p->getAdaptiveAnchorScale()));
        param.set(TempoA1AnchorFundamental, String(p->getAdaptiveAnchorFundamental()));
        param.set(TempoA1ClusterThresh, String(p->getAdaptiveClusterThresh()));
        param.set(TempoA1History, String(p->getAdaptiveHistory()));
        param.set(TempoCustomScale, floatArrayToString(p->getCustomScale()));
        param.set(TempoAbsoluteOffsets, offsetArrayToString(p->getAbsoluteOffsets()));
        //param.set(TempoReset, intArrayToString(p->getResetMap()->keys()));
         */
    }
    
    inline bool compare(TempoModPreparation::Ptr t)
    {
        /*
        return (getParam(TempoScale) == t->getParam(TempoScale) &&
                getParam(TempoFundamental) == t->getParam(TempoFundamental) &&
                getParam(TempoOffset) == t->getParam(TempoOffset) &&
                getParam(TempoA1IntervalScale) == t->getParam(TempoA1IntervalScale) &&
                getParam(TempoA1Inversional) == t->getParam(TempoA1Inversional) &&
                getParam(TempoA1AnchorScale) == t->getParam(TempoA1AnchorScale) &&
                getParam(TempoA1AnchorFundamental) == t->getParam(TempoA1AnchorFundamental) &&
                getParam(TempoA1ClusterThresh) == t->getParam(TempoA1ClusterThresh) &&
                getParam(TempoA1History) == t->getParam(TempoA1History) &&
                getParam(TempoCustomScale) == t->getParam(TempoCustomScale) &&
                getParam(TempoAbsoluteOffsets) == t->getParam(TempoAbsoluteOffsets));
         */
        
    }
    
    inline ValueTree getState(int Id)
    {
        /*
        ValueTree prep(vtagTempoModPrep + String(Id));
        
        String p = getParam(TempoScale);
        if (p != String::empty) prep.setProperty( ptagTempo_scale, p.getIntValue(), 0);
        
        p = getParam(TempoFundamental);
        if (p != String::empty) prep.setProperty( ptagTempo_fundamental,           p.getIntValue(), 0);
        
        p = getParam(TempoOffset);
        if (p != String::empty) prep.setProperty( ptagTempo_offset,                p.getFloatValue(), 0 );
        
        p = getParam(TempoA1IntervalScale);
        if (p != String::empty) prep.setProperty( ptagTempo_adaptiveIntervalScale, p.getIntValue(), 0 );
        
        p = getParam(TempoA1Inversional);
        if (p != String::empty) prep.setProperty( ptagTempo_adaptiveInversional,   (bool)p.getIntValue(), 0 );
        
        p = getParam(TempoA1AnchorScale);
        if (p != String::empty) prep.setProperty( ptagTempo_adaptiveAnchorScale,   p.getIntValue(), 0 );
        
        p = getParam(TempoA1AnchorFundamental);
        if (p != String::empty) prep.setProperty( ptagTempo_adaptiveAnchorFund,    p.getIntValue(), 0 );
        
        p = getParam(TempoA1ClusterThresh);
        if (p != String::empty) prep.setProperty( ptagTempo_adaptiveClusterThresh, p.getIntValue(), 0 );
        
        p = getParam(TempoA1History);
        if (p != String::empty) prep.setProperty( ptagTempo_adaptiveHistory,       p.getIntValue(), 0 );
        
        
        ValueTree scale( vtagTempo_customScale);
        int count = 0;
        p = getParam(TempoCustomScale);
        if (p != String::empty)
        {
            Array<float> scl = stringToFloatArray(p);
            for (auto note : scl)
                scale.setProperty( ptagFloat + String(count++), note, 0 );
        }
        prep.addChild(scale, -1, 0);
        
        ValueTree absolute( vTagTempo_absoluteOffsets);
        count = 0;
        p = getParam(TempoAbsoluteOffsets);
        if (p != String::empty)
        {
            Array<float> offsets = stringOrderedPairsToFloatArray(p, 128);
            for (auto note : offsets)
            {
                if(note != 0.) absolute.setProperty( ptagFloat + String(count), note * .01, 0 );
                count++;
            }
        }
        prep.addChild(absolute, -1, 0);
    
        return prep;
        */
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
    
    
private:
    StringArray          param;
    
    JUCE_LEAK_DETECTOR(TempoModPreparation);
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
    sPrep(new TempoPreparation(prep)),
    aPrep(new TempoPreparation(sPrep)),
    processor(new TempoProcessor(aPrep)),
    Id(Id)
    {
        
    }
    
    Tempo(int Id):
    Id(Id)
    {
        sPrep = new TempoPreparation();
        aPrep = new TempoPreparation(sPrep);
        processor = new TempoProcessor(aPrep);
    };
    
    /*
     TempoId = 0,
     Tempo,
     adaptiveTempoSystem,
     AT1History,
     AT1Subdivisions,
     AT1Min,
     AT1Max,
     */
    inline ValueTree getState(void)
    {
        
        ValueTree prep(vtagTempoPrep + String(Id));
        
        prep.setProperty( ptagTempo_Id,                    Id, 0);
        prep.setProperty( ptagTempo_tempo,                 sPrep->getTempo(), 0);
        prep.setProperty( ptagTempo_adaptiveSystem,        sPrep->getTempoSystem(), 0);
        prep.setProperty( ptagTempo_AT1History,            sPrep->getAdaptiveTempo1History(), 0 );
        prep.setProperty( ptagTempo_AT1Subdivisions,       sPrep->getAdaptiveTempo1Subdivisions(), 0 );
        prep.setProperty( ptagTempo_AT1Min,                sPrep->getAdaptiveTempo1Min(), 0 );
        prep.setProperty( ptagTempo_AT1Max,                sPrep->getAdaptiveTempo1Max(), 0 );

        return prep;
        
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
    
    
private:
    int Id;
    
    JUCE_LEAK_DETECTOR(Tempo)
};

#endif  // TEMPO_H_INCLUDED
