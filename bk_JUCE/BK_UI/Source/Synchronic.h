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
#include "Tempo.h"
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
    sGain(p->getGain()),
    sClusterThresh(p->getClusterThreshMS()),
    sClusterThreshSec(p->getClusterThreshSEC()),
    sReleaseVelocitySetsSynchronic(p->getReleaseVelocitySetsSynchronic()),
    tuning(p->getTuning()),
    tempo(p->getTempoControl())
    {
    }
    
    SynchronicPreparation(int numBeats,
                          int clusterMin,
                          int clusterMax,
                          float clusterThresh,
                          SynchronicSyncMode mode,
                          bool velocityMode,
                          int beatsToSkip,
                          Array<float> beatMultipliers,
                          Array<float> accentMultipliers,
                          Array<float> lengthMultipliers,
                          Array<Array<float>> transp,
                          Tuning::Ptr tun,
                          Tempo::Ptr tmp):
    sNumBeats(numBeats),
    sClusterMin(clusterMin),
    sClusterMax(clusterMax),
    sMode(mode),
    sReleaseVelocitySetsSynchronic(velocityMode),
    sBeatsToSkip(beatsToSkip),
    sBeatMultipliers(beatMultipliers),
    sAccentMultipliers(accentMultipliers),
    sLengthMultipliers(lengthMultipliers),
    sTransposition(transp),
    sClusterThresh(clusterThresh),
    sClusterThreshSec(.001 * sClusterThresh),
    tuning(tun),
    tempo(tmp)
    {
    }

    
    SynchronicPreparation(Tuning::Ptr tun, Tempo::Ptr tmp):
    sNumBeats(0),
    sClusterMin(1),
    sClusterMax(100),
    sClusterCap(8), //8 in original bK, but behavior is different here?
    sMode(FirstNoteOnSync),
    sReleaseVelocitySetsSynchronic(false),
    sBeatsToSkip(0),
    sBeatMultipliers(Array<float>({1.0})),
    sAccentMultipliers(Array<float>({1.0})),
    sLengthMultipliers(Array<float>({1.0})),
    sGain(1.0),
    sClusterThresh(500),
    sClusterThreshSec(.001 * sClusterThresh),
    tuning(tun),
    tempo(tmp)
    {
        sTransposition.ensureStorageAllocated(1);
        sTransposition.add(Array<float>({0.0}));
    }
    
    inline void copy(SynchronicPreparation::Ptr s)
    {
        sNumBeats = s->getNumBeats();
        sClusterMin = s->getClusterMin();
        sClusterMax = s->getClusterMax();
        sClusterCap = s->getClusterCap();
        sMode = s->getMode();
        sBeatsToSkip = s->getBeatsToSkip();
        sBeatMultipliers = s->getBeatMultipliers();
        sAccentMultipliers = s->getAccentMultipliers();
        sLengthMultipliers = s->getLengthMultipliers();
        sGain = s->getGain();
        sTransposition = s->getTransposition();
        sClusterThresh = s->getClusterThreshMS();
        sClusterThreshSec = s->getClusterThreshSEC();
        sReleaseVelocitySetsSynchronic = s->getReleaseVelocitySetsSynchronic();
        tuning = s->getTuning();
        tempo = s->getTempoControl();
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
        
        return (sNumBeats == s->getNumBeats() &&
                sClusterMin == s->getClusterMin() &&
                sClusterMax == s->getClusterMax() &&
                sClusterCap == s->getClusterCap() &&
                (sMode == s->getMode()) &&
                transp && lens && accents && beats &&
                sGain == s->getGain() &&
                sClusterThresh == s->getClusterThreshMS() &&
                sClusterThreshSec == s->getClusterThreshSEC() &&
                sReleaseVelocitySetsSynchronic == s->getReleaseVelocitySetsSynchronic() &&
                tuning == s->getTuning() &&
                tempo == s->getTempoControl());
    }
    
    //inline const float getTempo() const noexcept                       {return sTempo;               }
    inline const int getNumBeats() const noexcept                      {return sNumBeats;              }
    inline const int getClusterMin() const noexcept                    {return sClusterMin;            }
    inline const int getClusterMax() const noexcept                    {return sClusterMax;            }
    inline const int getClusterCap() const noexcept                    {return sClusterCap;            }
    inline const float getClusterThreshSEC() const noexcept            {return sClusterThreshSec;      }
    inline const float getClusterThreshMS() const noexcept             {return sClusterThresh;         }
    inline const SynchronicSyncMode getMode() const noexcept           {return sMode;                  }
    inline const Array<float> getBeatMultipliers() const noexcept      {return sBeatMultipliers;       }
    inline const int getBeatsToSkip() const noexcept                   {return sBeatsToSkip;           }
    inline const int getOffsetParamToggle() const noexcept
    {
        if(getMode() == FirstNoteOnSync || getMode() == AnyNoteOnSync) return getBeatsToSkip() + 1;
        else return getBeatsToSkip();
    }
    
    inline const Array<float> getAccentMultipliers() const noexcept    {return sAccentMultipliers;     }
    inline const Array<float> getLengthMultipliers() const noexcept    {return sLengthMultipliers;     }
    inline const Array<Array<float>> getTransposition() const noexcept {return sTransposition;         }
    inline const bool getReleaseVelocitySetsSynchronic() const noexcept{return sReleaseVelocitySetsSynchronic; }
    inline const float getGain() const noexcept                        {return sGain;                   }
    
    //inline const Keymap::Ptr getResetMap() const noexcept              {return resetMap;       }
    
    /*
    //Adaptive Tempo 1
    inline AdaptiveTempo1Mode getAdaptiveTempo1Mode(void)   {return at1Mode;   }
    inline int getAdaptiveTempo1History(void)               {return at1History;}
    inline float getAdaptiveTempo1Subdivisions(void)        {return at1Subdivisions;}
    inline float getAdaptiveTempo1Min(void)                 {return at1Min;}
    inline float getAdaptiveTempo1Max(void)                 {return at1Max;}
     */
    
    inline void setClusterThresh(float clusterThresh)
    {
        sClusterThresh = clusterThresh;
        sClusterThreshSec = sClusterThresh * .001;
    }
    
    
    inline const String getName() const noexcept {return name;}
    inline void setName(String n){name = n;}
    
    inline void setNumBeats(int numBeats)                              {sNumBeats = numBeats;                              }
    inline void setClusterMin(int clusterMin)                          {sClusterMin = clusterMin;                          }
    inline void setClusterMax(int clusterMax)                          {sClusterMax = clusterMax;                          }
    inline void setClusterCap(int clusterCap)                          {sClusterCap = clusterCap;                          }
    inline void setMode(SynchronicSyncMode mode)                       {sMode = mode;                                      }
    inline void setBeatsToSkip(int beatsToSkip)                        {sBeatsToSkip = beatsToSkip;                        }
    inline void setBeatMultipliers(Array<float> beatMultipliers)       {sBeatMultipliers.swapWith(beatMultipliers);        }
    inline void setAccentMultipliers(Array<float> accentMultipliers)   {sAccentMultipliers.swapWith(accentMultipliers);    }
    inline void setTransposition(Array<Array<float>> transp)           {sTransposition.swapWith(transp);                   }
    inline void setLengthMultipliers(Array<float> lengthMultipliers)   {sLengthMultipliers.swapWith(lengthMultipliers);    }
    
    inline void setBeatMultiplier(int whichSlider, float value)        {sBeatMultipliers.set(whichSlider, value);           }
    inline void setAccentMultiplier(int whichSlider, float value)      {sAccentMultipliers.set(whichSlider, value);         }
    inline void setLengthMultiplier(int whichSlider, float value)      {sLengthMultipliers.set(whichSlider, value);         }
    inline void setSingleTransposition(int whichSlider, Array<float> values) {sTransposition.set(whichSlider, values); }
    inline void setReleaseVelocitySetsSynchronic(bool rvss)            {sReleaseVelocitySetsSynchronic = rvss;          }
    inline void setGain(float gain)                                    {sGain = gain;                          }
    
    //Adaptive Tempo 1
    /*
    inline void setAdaptiveTempo1Mode(AdaptiveTempo1Mode mode)          {at1Mode = mode;    }
    inline void setAdaptiveTempo1History(int hist)                      {at1History = hist;}
    inline void setAdaptiveTempo1Subdivisions(float sub)                {at1Subdivisions = sub;}
    inline void setAdaptiveTempo1Min(float min)                         {at1Min = min;}
    inline void setAdaptiveTempo1Max(float max)                         {at1Max = max;}
     */
    
    inline const Tuning::Ptr getTuning() const noexcept                 {return tuning; }
    inline void setTuning(Tuning::Ptr t)                                {tuning = t;  }
    
    inline const Tempo::Ptr getTempoControl() const noexcept            {return tempo; }
    inline void setTempoControl(Tempo::Ptr t)                           {tempo = t;  }
    
    void print(void)
    {
        DBG("| - - - Synchronic Preparation - - - |");
        DBG("sTempo: " + String(sTempo));
        DBG("sNumBeats: " + String(sNumBeats));
        DBG("sClusterMin: " + String(sClusterMin));
        DBG("sClusterMax: " + String(sClusterMax));
        DBG("sClusterCap: " + String(sClusterCap));
        DBG("sClusterThresh: " + String(sClusterThresh));
        DBG("sReleaseVelocitySetsSynchronic: " + String(sReleaseVelocitySetsSynchronic));
        DBG("sMode: " + String(sMode));
        DBG("sBeatsToSkip: " + String(sBeatsToSkip));
        DBG("sBeatMultipliers: " + floatArrayToString(sBeatMultipliers));
        DBG("sLengthMultipliers: " + floatArrayToString(sLengthMultipliers));
        DBG("sAccentMultipliers: " + floatArrayToString(sAccentMultipliers));
        
        String s = "";
        for (auto arr : sTransposition) s += "{ " + floatArrayToString(arr) + " },\n";
        DBG("sTransposition: " + s);
        
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

    float sGain;                //gain multiplier
    float sClusterThresh;      //max time between played notes before new cluster is started, in MS
    float sClusterThreshSec;
    
    bool sReleaseVelocitySetsSynchronic;
    
    /*
    // Adaptive Tempo 1
    int at1History;
    float at1Min, at1Max;
    float at1Subdivisions;
    AdaptiveTempo1Mode at1Mode;
     */
    
    Tuning::Ptr tuning;
    Tempo::Ptr tempo;
    
    //Keymap::Ptr resetMap = new Keymap(0); //need to add to copy and mod
    
    JUCE_LEAK_DETECTOR(SynchronicPreparation);
};

class SynchronicProcessor  : public ReferenceCountedObject
{
    
public:
    typedef ReferenceCountedObjectPtr<SynchronicProcessor>   Ptr;
    typedef Array<SynchronicProcessor::Ptr>                  PtrArr;
    typedef Array<SynchronicProcessor::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<SynchronicProcessor>                  Arr;
    typedef OwnedArray<SynchronicProcessor,CriticalSection>  CSArr;
    
    
    SynchronicProcessor(BKSynthesiser* main,
                        SynchronicPreparation::Ptr active,
                        GeneralSettings::Ptr general,
                        int Id);
    
    ~SynchronicProcessor();
    
    void         setCurrentPlaybackSampleRate(double sr);
    inline const uint64 getCurrentNumSamplesBeat(void) const noexcept   { return numSamplesBeat;    }
    inline const uint64 getCurrentPhasor(void) const noexcept           { return phasor;            }
    
    void processBlock(int numSamples, int channel);
    void keyPressed(int noteNumber, float velocity);
    void keyReleased(int noteNumber, float velocity, int channel);
    float getTimeToBeatMS(float beatsToSkip);
    
    inline const int getBeatMultiplierCounter() const noexcept { return beatMultiplierCounter; }
    inline const int getAccentMultiplierCounter() const noexcept { return accentMultiplierCounter; }
    inline const int getLengthMultiplierCounter() const noexcept { return lengthMultiplierCounter; }
    inline const int getTranspCounter() const noexcept { return transpCounter; }
    inline const SynchronicSyncMode getMode() const noexcept {return active->getMode(); }
    
    inline void attachToSynthesiser(BKSynthesiser* main)
    {
        synth = main;
    }
    //void  atReset();
    
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
    /*
    uint64 atTimer, atLastTime; //in samples
    int atDelta;                //in ms
    Array<int> atDeltaHistory;  //in ms
    void atNewNote();
    void atNewNoteOff();
    void atCalculatePeriodMultiplier();
    float adaptiveTempoPeriodMultiplier;
     */
    
    bool shouldPlay;
    
    JUCE_LEAK_DETECTOR(SynchronicProcessor);
};




#endif  // SYNCHRONIC_H_INCLUDED
