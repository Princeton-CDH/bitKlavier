/*
 ==============================================================================
 
 Tuning.h
 Created: 4 Dec 2016 3:40:02pm
 Author:  Daniel Trueman
 
 ==============================================================================
 */

#ifndef TUNING_H_INCLUDED
#define TUNING_H_INCLUDED

#include "BKUtilities.h"

#include "AudioConstants.h"

class TuningPreparation : public ReferenceCountedObject
{
public:
    typedef ReferenceCountedObjectPtr<TuningPreparation>   Ptr;
    typedef Array<TuningPreparation::Ptr>                  Arr;
    typedef Array<TuningPreparation::Ptr, CriticalSection> CSArr;
    
    TuningPreparation(int Id,
                      TuningSystem whichTuning,
                      PitchClass fundamental,
                      TuningSystem adaptiveIntervalScale,
                      bool adaptiveInversional,
                      TuningSystem adaptiveAnchorScale,
                      PitchClass adaptiveAnchorFundamental,
                      uint64 adaptiveClusterThresh,
                      int adaptiveHistory,
                      Array<float> customScale):
    Id(Id),
    tWhichTuning(whichTuning),
    tFundamental(fundamental),
    tAdaptiveIntervalScale(adaptiveIntervalScale),
    tAdaptiveInversional(adaptiveInversional),
    tAdaptiveAnchorScale(adaptiveAnchorScale),
    tAdaptiveAnchorFundamental(adaptiveAnchorFundamental),
    tAdaptiveClusterThresh(adaptiveClusterThresh),
    tAdaptiveHistory(adaptiveHistory),
    tCustom(customScale)
    {
        
    }
    
    TuningPreparation(int Id):
    Id(Id),
    tWhichTuning(EqualTemperament),
    tFundamental(C),
    tAdaptiveIntervalScale(JustTuning),
    tAdaptiveInversional(true),
    tAdaptiveAnchorScale(EqualTemperament),
    tAdaptiveAnchorFundamental(C),
    tAdaptiveClusterThresh(100),
    tAdaptiveHistory(4),
    tCustom({0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.})
    {
        
    }
    
    ~TuningPreparation()
    {
        
    }
    
    inline const int getID() const noexcept                                 {return Id;                         }
    inline const TuningSystem getTuning() const noexcept                    {return tWhichTuning;               }
    inline const PitchClass getFundamental() const noexcept                 {return tFundamental;               }
    inline const TuningSystem getAdaptiveIntervalScale() const noexcept     {return tAdaptiveIntervalScale;     }
    inline const bool getAdaptiveInversional() const noexcept               {return tAdaptiveInversional;       }
    inline const TuningSystem getAdaptiveAnchorScale() const noexcept       {return tAdaptiveAnchorScale;       }
    inline const PitchClass getAdaptiveAnchorFundamental() const noexcept   {return tAdaptiveAnchorFundamental; }
    inline const uint64 getAdaptiveClusterThresh() const noexcept           {return tAdaptiveClusterThresh;     }
    inline const int getAdaptiveHistory() const noexcept                    {return tAdaptiveHistory;           }
    inline const Array<float> getCustomScale() const noexcept               {return tCustom;                    }
    
    
    inline void setTuning(TuningSystem tuning)                                      {tWhichTuning = tuning;                                 }
    inline void setFundamental(PitchClass fundamental)                              {tFundamental = fundamental;                            }
    inline void setAdaptiveIntervalScale(TuningSystem adaptiveIntervalScale)        {tAdaptiveIntervalScale = adaptiveIntervalScale;        }
    inline void setAdaptiveInversional(bool adaptiveInversional)                    {tAdaptiveInversional = adaptiveInversional;            }
    inline void setAdaptiveAnchorScale(TuningSystem adaptiveAnchorScale)            {tAdaptiveAnchorScale = adaptiveAnchorScale;            }
    inline void setAdaptiveAnchorFundamental(PitchClass adaptiveAnchorFundamental)  {tAdaptiveAnchorFundamental = adaptiveAnchorFundamental;}
    inline void setAdaptiveClusterThresh(uint64 adaptiveClusterThresh)              {tAdaptiveClusterThresh = adaptiveClusterThresh;        }
    inline void setAdaptiveHistory(int adaptiveHistory)                             {tAdaptiveHistory = adaptiveHistory;                    }
    inline void setCustomScale(Array<float> tuning)                                 {tCustom = tuning;                                      }
    
    
    void print(void)
    {
        DBG("tWhichTuning: " +                  String(tWhichTuning));
        DBG("tFundamental: " +                  String(tFundamental));
        DBG("tAdaptiveIntervalScale: " +        String(tAdaptiveIntervalScale));
        DBG("tAdaptiveInversional: " +          String(tAdaptiveInversional));
        DBG("tAdaptiveAnchorScale: " +          String(tAdaptiveAnchorScale));
        DBG("tAdaptiveAnchorFundamental: " +    String(tAdaptiveAnchorFundamental));
        DBG("tAdaptiveClusterThresh: " +        String(tAdaptiveClusterThresh));
        DBG("tAdaptiveHistory: " +              String(tAdaptiveHistory));
        DBG("tCustom: " +                       floatArrayToString(tCustom));
    }
private:
    
    int Id;
    
    // basic tuning settings, for static tuning
    TuningSystem    tWhichTuning;               //which tuning system to use
    PitchClass      tFundamental;               //fundamental for tuning system
    
    // adaptive tuning params
    TuningSystem    tAdaptiveIntervalScale;     //scale to use to determine successive interval tuning
    bool            tAdaptiveInversional;       //treat the scale inversionally?
    
    TuningSystem    tAdaptiveAnchorScale;       //scale to tune new fundamentals to when in anchored
    PitchClass      tAdaptiveAnchorFundamental; //fundamental for anchor scale
    
    uint64          tAdaptiveClusterThresh;     //ms; max time before fundamental is reset
    int             tAdaptiveHistory;           //cluster max; max number of notes before fundamental is reset
    
    // custom scale
    Array<float>    tCustom = {0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.}; //custom scale
    
    JUCE_LEAK_DETECTOR(TuningPreparation);
};


class TuningProcessor : public ReferenceCountedObject
{
public:
    typedef ReferenceCountedObjectPtr<TuningProcessor>   Ptr;
    typedef Array<TuningProcessor::Ptr>                  Arr;
    typedef Array<TuningProcessor::Ptr, CriticalSection> CSArr;
    
    TuningProcessor(TuningPreparation::Ptr prep, int layer);;
    ~TuningProcessor();
    
    void setPreparation(TuningPreparation::Ptr prep) {preparation = prep;}
    
    //returns tuning offsets; add to integer PitchClass
    float getOffset(int midiNoteNumber) const;
    
    //for calculating adaptive tuning
    void keyOn(int midiNoteNumber);
    
    //for cluster timing
    void incrementAdaptiveClusterTime(int numSamples);
    
    //for global tuning adjustment, A442, etc...
    void setGlobalTuningReference(float tuningRef) { globalTuningReference = tuningRef;}
    const float getGlobalTuningReference(void) const noexcept {return globalTuningReference;}
    
private:
    
    Array<Array<float>> tuningLibrary;
    
    TuningPreparation::Ptr preparation;
    int layer;
    
    float   intervalToRatio(float interval) const noexcept { return mtof(interval + 60.) / mtof(60.); }
    
    //adaptive tuning functions
    float   calculateRatio(int midiNoteNumber);
    void    newNote(int midiNoteNumber, TuningSystem tuningType);
    float   calculate(int midiNoteNumber, int anchored);
    uint64  clusterTime;
    
    
    float   lastNote[128];
    int     fundamentalNote; //moves with adaptive tuning
    float   fundamentalFreq;
    int     historyCounter;
    bool    reset;
    
    float globalTuningReference = 440.; //A440
    
    /* Array of all the default tunings
     Just:       1/1,    16/15,  9/8,    6/5,   5/4,    4/3,    7/5,    3/2,    8/5,    5/3,    7/4,    15/8
     Partial:    1/1,    16/15,  9/8,    7/6,   5/4,    4/3,    11/8,   3/2,    13/8,   5/3,    7/4,    11/6
     Duodene:    1/1,    16/15,  9/8,    6/5,   5/4,    4/3,    45/32,  3/2,    8/5,    5/3,    16/9,   15/8
     Otonal:     1/1,    17/16,  9/8,    19/16, 5/4,    21/16,  11/8,   3/2,    13/8,   27/16,  7/4,    15/8
     Utonal:     1/1,    16/15,  8/7,    32/27, 16/13,  4/3,    16/11,  32/21,  8/5,    32/19,  16/9,   32/17
     */
    Array<float> tEqualTuning       = {0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.};
    Array<float> tJustTuning        = {0., .117313, .039101,  .156414, -.13686, -.019547, -.174873, .019547, .136864, -.15641, -.311745, -.11731};
    Array<float> tPartialTuning     = {0., .117313, .039101, -.331291, -.13686, -.019547, -.486824, .019547, .405273, -.15641, -.311745, -.506371};
    Array<float> tDuodeneTuning     = {0., .117313, .039101, .156414, -.13686, -.019547, -.097763, .019547, .136864, -.15641, -.039101, -.11731};
    Array<float> tOtonalTuning      = {0., .049553, .039101, -.02872, -.13686, -.292191, -.486824, .019547, .405273, .058647, -.311745, -.11731};
    Array<float> tUtonalTuning      = {0., .117313, .311745, .156414, -.405273, -.019547, .486824, .292191, .136864, .024847, -.039101,  -.049553};
    
    JUCE_LEAK_DETECTOR(TuningProcessor);
};

#endif  // TUNING_H_INCLUDED
