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
#include "BKSynthesiser.h"
#include "Tuning.h"
#include "General.h"
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
    
    
    TempoProcessor(BKSynthesiser *synth,
                        TempoProcessor::Ptr active,
                        GeneralSettings::Ptr general,
                        int Id);
    
    ~TempoProcessor();
    
    void setCurrentPlaybackSampleRate(double sr);
    
    void processBlock(int numSamples, int channel);
    void keyPressed(int noteNumber, float velocity);
    void keyReleased(int noteNumber, int channel);
    inline float getPeriodMultiplier(void)     {return adaptiveTempoPeriodMultiplier;}
    
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



#endif  // TEMPO_H_INCLUDED
