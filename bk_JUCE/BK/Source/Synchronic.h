/*
  ==============================================================================

    Synchronic.h
    Created: 22 Nov 2016 3:46:35pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef SYNCHRONIC_H_INCLUDED
#define SYNCHRONIC_H_INCLUDED

#include "BKUtilities.h"
#include "BKSynthesiser.h"
#include "Tuning.h"
#include "General.h"
#include "Keymap.h"

class SynchronicPreparation : public ReferenceCountedObject
{
public:
    typedef ReferenceCountedObjectPtr<SynchronicPreparation>   Ptr;
    typedef Array<SynchronicPreparation::Ptr>                  PtrArr;
    typedef Array<SynchronicPreparation::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<SynchronicPreparation>                  Arr;
    typedef OwnedArray<SynchronicPreparation, CriticalSection> CSArr;
    
    // Copy Constructor
    SynchronicPreparation(SynchronicPreparation::Ptr p):
    sTempo(p->getTempo()),
    sNumBeats(p->getNumBeats()),
    sClusterMin(p->getClusterMin()),
    sClusterMax(p->getClusterMax()),
    sClusterCap(p->getClusterCap()),
    sMode(p->getMode()),
    sBeatsToSkip(p->getBeatsToSkip()),
    sBeatMultipliers(p->getBeatMultipliers()),
    sAccentMultipliers(p->getAccentMultipliers()),
    sLengthMultipliers(p->getLengthMultipliers()),
    sTransposition(p->getTransposition()),
    sBeatThreshSec(p->getBeatThresh()),
    sClusterThresh(p->getClusterThreshMS()),
    sClusterThreshSec(p->getClusterThreshSEC()),
    at1History(p->getAdaptiveTempo1History()),
    at1Min(p->getAdaptiveTempo1Min()),
    at1Max(p->getAdaptiveTempo1Max()),
    at1Subdivisions(p->getAdaptiveTempo1Subdivisions()),
    at1Mode(p->getAdaptiveTempo1Mode()),
    tuning(p->getTuning())
    {
    }
    
    SynchronicPreparation(float tempo,
                          int numBeats,
                          int clusterMin,
                          int clusterMax,
                          float clusterThresh,
                          SynchronicSyncMode mode,
                          int beatsToSkip,
                          Array<float> beatMultipliers,
                          Array<float> accentMultipliers,
                          Array<float> lengthMultipliers,
                          Array<Array<float>> transp,
                          Tuning::Ptr t):
    sTempo(tempo),
    sNumBeats(numBeats),
    sClusterMin(clusterMin),
    sClusterMax(clusterMax),
    sMode(mode),
    sBeatsToSkip(beatsToSkip),
    sBeatMultipliers(beatMultipliers),
    sAccentMultipliers(accentMultipliers),
    sLengthMultipliers(lengthMultipliers),
    sTransposition(transp),
    sBeatThreshSec(60.0/sTempo),
    sClusterThresh(clusterThresh),
    sClusterThreshSec(.001 * sClusterThresh),
    tuning(t)
    {
    }

    
    SynchronicPreparation(Tuning::Ptr t):
    sTempo(120),
    sNumBeats(0),
    sClusterMin(1),
    sClusterMax(100),
    sClusterCap(8),
    sMode(FirstNoteOnSync),
    sBeatsToSkip(0),
    sBeatMultipliers(Array<float>({1.0})),
    sAccentMultipliers(Array<float>({1.0})),
    sLengthMultipliers(Array<float>({1.0})),
    sBeatThreshSec(60.0/sTempo),
    sClusterThresh(500),
    sClusterThreshSec(.001 * sClusterThresh),
    at1History(0),
    at1Min(100),
    at1Max(2000),
    at1Subdivisions(1.0f),
    at1Mode(TimeBetweenNotes),
    tuning(t)
    {
        sTransposition.ensureStorageAllocated(1);
        sTransposition.add(Array<float>({0.0}));
    }
    
    inline void copy(SynchronicPreparation::Ptr s)
    {
        sTempo = s->getTempo();
        sNumBeats = s->getNumBeats();
        sClusterMin = s->getClusterMin();
        sClusterMax = s->getClusterMax();
        sClusterCap = s->getClusterCap();
        sMode = s->getMode();
        sBeatsToSkip = s->getBeatsToSkip();
        sBeatMultipliers = s->getBeatMultipliers();
        sAccentMultipliers = s->getAccentMultipliers();
        sLengthMultipliers = s->getLengthMultipliers();
        sTransposition = s->getTransposition();
        sBeatThreshSec = s->getBeatThresh();
        sClusterThresh = s->getClusterThreshMS();
        sClusterThreshSec = s->getClusterThreshSEC();
        at1History = s->getAdaptiveTempo1History();
        at1Min = s->getAdaptiveTempo1Min();
        at1Max = s->getAdaptiveTempo1Max();
        at1Subdivisions = s->getAdaptiveTempo1Subdivisions();
        at1Mode = s->getAdaptiveTempo1Mode();
        tuning = s->getTuning();
        //resetMap->copy(s->resetMap);
    }
    
    bool compare(SynchronicPreparation::Ptr s)
    {
        bool lens = true;
        bool accents = true;
        bool beats = true;
        bool transp = true;
        
        for (int i = s->getLengthMultipliers().size(); --i>=0;)
        {
            if (s->getLengthMultipliers()[i] != sLengthMultipliers[i])
            {
                lens = false;
                break;
                
            }
        }
        
        for (int i = s->getAccentMultipliers().size(); --i>=0;)
        {
            if (s->getAccentMultipliers()[i] != sAccentMultipliers[i])
            {
                accents = false;
                break;
                
            }
        }
        
        for (int i = s->getBeatMultipliers().size(); --i>=0;)
        {
            if (s->getBeatMultipliers()[i] != sBeatMultipliers[i])
            {
                beats = false;
                break;
                
            }
        }
        
        for (int i  = s->getTransposition().size(); --i >= 0;)
        {
            Array<float> transposition = s->getTransposition()[i];
            for (int j = transposition.size(); --j >= 0;)
            {
                if (transposition[j] != sTransposition[i][j])
                {
                    transp = false;
                    break;
                    
                }
            }
        }
        
        return (sTempo == s->getTempo() &&
                sNumBeats == s->getNumBeats() &&
                sClusterMin == s->getClusterMin() &&
                sClusterMax == s->getClusterMax() &&
                sClusterCap == s->getClusterCap() &&
                (sMode == s->getMode()) &&
                transp && lens && accents && beats &&
                sBeatThreshSec == s->getBeatThresh() &&
                sClusterThresh == s->getClusterThreshMS() &&
                sClusterThreshSec == s->getClusterThreshSEC() &&
                at1History == s->getAdaptiveTempo1History() &&
                at1Min == s->getAdaptiveTempo1Min() &&
                at1Max == s->getAdaptiveTempo1Max() &&
                at1Subdivisions == s->getAdaptiveTempo1Subdivisions() &&
                at1Mode == s->getAdaptiveTempo1Mode() &&
                tuning == s->getTuning());
    }
    
    inline const float getTempo() const noexcept                       {return sTempo;                 }
    inline const int getNumBeats() const noexcept                      {return sNumBeats;             }
    inline const int getClusterMin() const noexcept                    {return sClusterMin;            }
    inline const int getClusterMax() const noexcept                    {return sClusterMax;            }
    inline const int getClusterCap() const noexcept                    {return sClusterCap;            }
    inline const float getClusterThreshSEC() const noexcept            {return sClusterThreshSec;      }
    inline const float getBeatThresh() const noexcept                  {return sBeatThreshSec;        }
    inline const float getClusterThreshMS() const noexcept             {return sClusterThresh;         }
    inline const SynchronicSyncMode getMode() const noexcept           {return sMode;                  }
    inline const Array<float> getBeatMultipliers() const noexcept      {return sBeatMultipliers;       }
    inline const int getBeatsToSkip()                                  {return sBeatsToSkip;           }
    inline const Array<float> getAccentMultipliers() const noexcept    {return sAccentMultipliers;     }
    inline const Array<float> getLengthMultipliers() const noexcept    {return sLengthMultipliers;     }
    inline const Array<Array<float>> getTransposition() const noexcept        {return sTransposition;         }
    //inline const Keymap::Ptr getResetMap() const noexcept              {return resetMap;       }
    
    //Adaptive Tempo 1
    inline AdaptiveTempo1Mode getAdaptiveTempo1Mode(void)   {return at1Mode;   }
    inline int getAdaptiveTempo1History(void)               {return at1History;}
    inline float getAdaptiveTempo1Subdivisions(void)        {return at1Subdivisions;}
    inline float getAdaptiveTempo1Min(void)                 {return at1Min;}
    inline float getAdaptiveTempo1Max(void)                 {return at1Max;}

    
    inline void setTempo(float tempo)
    {
        sTempo = tempo;
        float tempoPeriodS = (60.0/sTempo);
        sBeatThreshSec = tempoPeriodS;
    }
    
    inline void setClusterThresh(float clusterThresh)
    {
        sClusterThresh = clusterThresh;
        sClusterThreshSec = sClusterThresh * .001;
    }
    
    
    inline const String getName() const noexcept {return name;}
    inline void setName(String n){name = n;}
    
    inline void setNumBeats(int numBeats)                              {sNumBeats = numBeats;                            }
    inline void setClusterMin(int clusterMin)                          {sClusterMin = clusterMin;                          }
    inline void setClusterMax(int clusterMax)                          {sClusterMax = clusterMax;                          }
    inline void setClusterCap(int clusterCap)                          {sClusterCap = clusterCap;                          }
    inline void setMode(SynchronicSyncMode mode)                       {sMode = mode;                                      }
    inline void setBeatsToSkip(int beatsToSkip)                        {sBeatsToSkip = beatsToSkip;                        }
    inline void setBeatMultipliers(Array<float> beatMultipliers)       {sBeatMultipliers.swapWith(beatMultipliers);        }
    inline void setAccentMultipliers(Array<float> accentMultipliers)   {sAccentMultipliers.swapWith(accentMultipliers);    }
    inline void setTransposition(Array<Array<float>> transp)           {sTransposition.swapWith(transp);                   }
    inline void setLengthMultipliers(Array<float> lengthMultipliers)   {sLengthMultipliers.swapWith(lengthMultipliers);    }
    
    //Adaptive Tempo 1
    inline void setAdaptiveTempo1Mode(AdaptiveTempo1Mode mode)          {at1Mode = mode;    }
    inline void setAdaptiveTempo1History(int hist)                      {at1History = hist;}
    inline void setAdaptiveTempo1Subdivisions(float sub)                {at1Subdivisions = sub;}
    inline void setAdaptiveTempo1Min(float min)                         {at1Min = min;}
    inline void setAdaptiveTempo1Max(float max)                         {at1Max = max;}
    
    inline const Tuning::Ptr getTuning() const noexcept                 {return tuning; }
    inline void setTuning(Tuning::Ptr t)                                {tuning = t;  }
    //inline void setResetMap(Keymap::Ptr k)                              {resetMap = k;          }
    
    void print(void)
    {
        DBG("| - - - Synchronic Preparation - - - |");
        DBG("sTempo: " + String(sTempo));
        DBG("sNumBeats: " + String(sNumBeats));
        DBG("sClusterMin: " + String(sClusterMin));
        DBG("sClusterMax: " + String(sClusterMax));
        DBG("sClusterCap: " + String(sClusterCap));
        DBG("sClusterThresh: " + String(sClusterThresh));
        DBG("sMode: " + String(sMode));
        DBG("sBeatsToSkip: " + String(sBeatsToSkip));
        DBG("sBeatMultipliers: " + floatArrayToString(sBeatMultipliers));
        DBG("sLengthMultipliers: " + floatArrayToString(sLengthMultipliers));
        DBG("sAccentMultipliers: " + floatArrayToString(sAccentMultipliers));
        
        String s = "";
        for (auto arr : sTransposition) s += "{ " + floatArrayToString(arr) + " },\n";
        DBG("sTransposition: " + s);
        
        DBG("sBeatThreshSec: " + String(sBeatThreshSec));
        DBG("sClusterThreshSec: " + String(sClusterThreshSec));
        //DBG("resetKeymap: " + intArrayToString(getResetMap()->keys()));
        DBG("| - - - - - - - - -- - - - - - - - - |");
    }

    
private:
    String name;
    float sTempo;
    int sNumBeats,sClusterMin,sClusterMax;
    int sClusterCap = 8; //max in cluster; 8 in original bK. pulseDepth?
    
    SynchronicSyncMode sMode;
    int sBeatsToSkip;
    
    Array<float> sBeatMultipliers;      //multiply pulse lengths by these
    Array<float> sAccentMultipliers;    //multiply velocities by these
    Array<float> sLengthMultipliers;    //multiply note duration by these
    Array<Array<float>> sTransposition;        //transpose by these

    float sBeatThreshSec;      //length of time between pulses, as set by temp
    float sClusterThresh;      //max time between played notes before new cluster is started, in MS
    float sClusterThreshSec;
    
    // Adaptive Tempo 1
    int at1History;
    float at1Min, at1Max;
    float at1Subdivisions;
    AdaptiveTempo1Mode at1Mode;
    
    Tuning::Ptr tuning;
    
    //Keymap::Ptr resetMap = new Keymap(0); //need to add to copy and mod
    
    JUCE_LEAK_DETECTOR(SynchronicPreparation);
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
    
    SynchronicModPreparation(SynchronicPreparation::Ptr p)
    {
        param.ensureStorageAllocated(cSynchronicParameterTypes.size());
        
        param.set(SynchronicTuning, String(p->getTuning()->getId()));
        param.set(SynchronicTempo, String(p->getTempo()));
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
        param.set(AT1Mode, String(p->getAdaptiveTempo1Mode()));
        param.set(AT1History, String(p->getAdaptiveTempo1History()));
        param.set(AT1Subdivisions, String(p->getAdaptiveTempo1Subdivisions()));
        param.set(AT1Min, String(p->getAdaptiveTempo1Min()));
        param.set(AT1Max, String(p->getAdaptiveTempo1Max()));
        //param.set(SynchronicReset, intArrayToString(p->getResetMap()->keys()));
        param.set(SynchronicReset, "");
        
    }
    
    
    SynchronicModPreparation(void)
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
        param.set(AT1Mode, "");
        param.set(AT1History, "");
        param.set(AT1Subdivisions, "");
        param.set(AT1Min, "");
        param.set(AT1Max, "");
        param.set(SynchronicReset, "");
    }
    
    inline ValueTree getState(int Id)
    {
        ValueTree prep( vtagSynchronicModPrep + String(Id));
        
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
        String p = "";
        
        p = getParam(SynchronicTuning);
        if (p != String::empty) prep.setProperty( ptagSynchronic_tuning,              p.getIntValue(), 0);
        
        p = getParam(SynchronicTempo);
        if (p != String::empty) prep.setProperty( ptagSynchronic_tempo,               p.getFloatValue(), 0);
        
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
        
        p = getParam(SynchronicReset);
        if (p != String::empty) prep.setProperty( ptagSynchronic_reset,         p.getIntValue(), 0);
        
        /*
         ValueTree scale( vtagTuning_customScale);
         int count = 0;
         p = getParam(TuningCustomScale);
         if (p != String::empty)
         {
         Array<float> scl = stringToFloatArray(p);
         for (auto note : scl)
         scale.setProperty( ptagFloat + String(count++), note, 0 );
         }
         prep.addChild(scale, -1, 0);
         */
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
        
        
        p = getParam(AT1Mode);
        if (p != String::empty) prep.setProperty( ptagSynchronic_at1Mode,          p.getIntValue(), 0);
        
        p = getParam(AT1History);
        if (p != String::empty) prep.setProperty( ptagSynchronic_at1History,          p.getIntValue(), 0);
        
        p = getParam(AT1Subdivisions);
        if (p != String::empty) prep.setProperty( ptagSynchronic_at1Subdivisions,  p.getFloatValue(), 0);
        
        p = getParam(AT1Min);
        if (p != String::empty) prep.setProperty( ptagSynchronic_AT1Min,          p.getFloatValue(), 0);
        
        p = getParam(AT1Max);
        if (p != String::empty) prep.setProperty( ptagSynchronic_AT1Max,          p.getFloatValue(), 0);

        /*
        ValueTree resetMap(ptagSynchronic_reset);
        count = 0;
        p = getParam(SynchronicReset);
        if (p != String::empty)
        {
            Array<int> rmap = stringToIntArray(p);
            for (auto note : rmap)
                resetMap.setProperty( ptagInt + String(count++), note, 0 );
        }
        prep.addChild(resetMap, -1, 0);
         */
        
        return prep;
        
    }
    
    
    ~SynchronicModPreparation(void)
    {
        
    }
    
    inline void copy(SynchronicPreparation::Ptr p)
    {
        param.set(SynchronicTuning, String(p->getTuning()->getId()));
        param.set(SynchronicTempo, String(p->getTempo()));
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
        param.set(AT1Mode, String(p->getAdaptiveTempo1Mode()));
        param.set(AT1History, String(p->getAdaptiveTempo1History()));
        param.set(AT1Subdivisions, String(p->getAdaptiveTempo1Subdivisions()));
        param.set(AT1Min, String(p->getAdaptiveTempo1Min()));
        param.set(AT1Max, String(p->getAdaptiveTempo1Max()));
        //param.set(SynchronicReset, intArrayToString(p->getResetMap()->keys()));
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
    
private:
    StringArray          param;
    
    JUCE_LEAK_DETECTOR(SynchronicModPreparation);
};

class SynchronicProcessor  : public ReferenceCountedObject
{
    
public:
    typedef ReferenceCountedObjectPtr<SynchronicProcessor>   Ptr;
    typedef Array<SynchronicProcessor::Ptr>                  PtrArr;
    typedef Array<SynchronicProcessor::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<SynchronicProcessor>                  Arr;
    typedef OwnedArray<SynchronicProcessor,CriticalSection>  CSArr;
    
    
    SynchronicProcessor(BKSynthesiser *synth,
                        SynchronicPreparation::Ptr active,
                        GeneralSettings::Ptr general,
                        int Id);
    
    ~SynchronicProcessor();
    
    void         setCurrentPlaybackSampleRate(double sr);
    inline const uint64 getCurrentNumSamplesBeat(void) const noexcept   { return numSamplesBeat;    }
    inline const uint64 getCurrentPhasor(void) const noexcept           { return phasor;            }
    
    void processBlock(int numSamples, int channel);
    void keyPressed(int noteNumber, float velocity);
    void keyReleased(int noteNumber, int channel);
    float getTimeToBeatMS(float beatsToSkip);
    void  atReset();
    
private:
    int Id;
    BKSynthesiser* synth;
    GeneralSettings::Ptr general;
    SynchronicPreparation::Ptr active;
    
    double sampleRate;
    
    TuningProcessor::Ptr tuner;
    Array<float> tuningOffsets;
    PitchClass tuningBasePitch;

    int beatCounter;  //beat (or pulse) counter; max set by users -- sNumBeats
    
    //parameter field counters
    int beatMultiplierCounter;   //beat length (time between beats) multipliers
    int accentMultiplierCounter; //accent multipliers
    int lengthMultiplierCounter; //note length (sounding length) multipliers (multiples of 50ms, at least for now)
    int transpCounter;     //transposition offsets
    
    //reset the phase, including of all the parameter fields
    void resetPhase(int skipBeats);
    
    void playNote(int channel, int note, float velocity);
    Array<float> velocities;    //record of velocities
    Array<int> keysDepressed;   //current keys that are depressed
    
    bool inCluster;
    uint64 clusterThresholdSamples;
    uint64 clusterThresholdTimer;
    uint64 clusterTimer;
    Array<int> cluster;         //cluster of notes played, with repetitions, limited to totalClusters (8?)
    Array<int> slimCluster;     //cluster without repetitions
    
    uint64 phasor;
    uint64 numSamplesBeat;          // = beatThresholdSamples * beatMultiplier
    uint64 beatThresholdSamples;    // # samples in a beat, as set by tempo

    //adaptive tempo stuff
    uint64 atTimer, atLastTime; //in samples
    int atDelta;                //in ms
    Array<int> atDeltaHistory;  //in ms
    void atNewNote();
    void atNewNoteOff();
    void atCalculatePeriodMultiplier();
    float adaptiveTempoPeriodMultiplier;
    
    bool shouldPlay;
    
    JUCE_LEAK_DETECTOR(SynchronicProcessor);
};




#endif  // SYNCHRONIC_H_INCLUDED
