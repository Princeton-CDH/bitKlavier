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
    typedef ReferenceCountedObjectPtr<TuningPreparation>    Ptr;
    typedef Array<TuningPreparation::Ptr>                   PtrArr;
    typedef Array<TuningPreparation::Ptr, CriticalSection>  CSPtrArr;
    typedef OwnedArray<TuningPreparation>                        Arr;
    typedef OwnedArray<TuningPreparation, CriticalSection>       CSArr;
    
    TuningPreparation(TuningPreparation::Ptr p):
    tWhichTuning(p->getTuning()),
    tFundamental(p->getFundamental()),
    tFundamentalOffset(p->getFundamentalOffset()),
    tAdaptiveIntervalScale(p->getAdaptiveIntervalScale()),
    tAdaptiveInversional(p->getAdaptiveInversional()),
    tAdaptiveAnchorScale(p->getAdaptiveAnchorScale()),
    tAdaptiveAnchorFundamental(p->getAdaptiveAnchorFundamental()),
    tAdaptiveClusterThresh(p->getAdaptiveClusterThresh()),
    tAdaptiveHistory(p->getAdaptiveHistory()),
    tCustom(p->getCustomScale()),
    tAbsolute(p->getAbsoluteOffsets())
    {

    }
    
    inline void copy(TuningPreparation::Ptr p)
    {
        tWhichTuning = p->getTuning();
        tFundamental = p->getFundamental();
        tFundamentalOffset = p->getFundamentalOffset();
        tAdaptiveIntervalScale = p->getAdaptiveIntervalScale();
        tAdaptiveInversional = p->getAdaptiveInversional();
        tAdaptiveAnchorScale = p->getAdaptiveAnchorScale();
        tAdaptiveAnchorFundamental = p->getAdaptiveAnchorFundamental();
        tAdaptiveClusterThresh = p->getAdaptiveClusterThresh();
        tAdaptiveHistory = p->getAdaptiveHistory();
        tCustom = p->getCustomScale();
        tAbsolute = p->getAbsoluteOffsets();
        //resetMap->copy(p->resetMap);
        
    }
    
    inline bool compare (TuningPreparation::Ptr p)
    {
        bool custom = true;
        bool absolute = true;
        
        for (int i = p->getCustomScale().size(); --i>=0;)
        {
            if (p->getCustomScale()[i] != tCustom[i])
            {
                custom = false;
                break;
 
            }
        }
        
        for (int i = p->getAbsoluteOffsets().size(); --i>=0;)
        {
            if (p->getAbsoluteOffsets()[i] != tAbsolute[i])
            {
                absolute = false;
                break;
                
            }
        }
        
        return (tWhichTuning == p->getTuning() &&
                tFundamental == p->getFundamental() &&
                tFundamentalOffset == p->getFundamentalOffset() &&
                tAdaptiveIntervalScale == p->getAdaptiveIntervalScale() &&
                tAdaptiveInversional == p->getAdaptiveInversional() &&
                tAdaptiveAnchorScale == p->getAdaptiveAnchorScale() &&
                tAdaptiveAnchorFundamental == p->getAdaptiveAnchorFundamental() &&
                tAdaptiveClusterThresh == p->getAdaptiveClusterThresh() &&
                (tAdaptiveHistory == p->getAdaptiveHistory()) && custom && absolute);
    }
    
    TuningPreparation(TuningSystem whichTuning,
                      PitchClass fundamental,
                      float fundamentalOffset,
                      TuningSystem adaptiveIntervalScale,
                      bool adaptiveInversional,
                      TuningSystem adaptiveAnchorScale,
                      PitchClass adaptiveAnchorFundamental,
                      uint64 adaptiveClusterThresh,
                      int adaptiveHistory,
                      Array<float> customScale):
    tWhichTuning(whichTuning),
    tFundamental(fundamental),
    tFundamentalOffset(fundamentalOffset),
    tAdaptiveIntervalScale(adaptiveIntervalScale),
    tAdaptiveInversional(adaptiveInversional),
    tAdaptiveAnchorScale(adaptiveAnchorScale),
    tAdaptiveAnchorFundamental(adaptiveAnchorFundamental),
    tAdaptiveClusterThresh(adaptiveClusterThresh),
    tAdaptiveHistory(adaptiveHistory),
    tCustom(customScale)
    {
        tAbsolute.ensureStorageAllocated(128);
        for(int i=0; i<128; i++) tAbsolute.set(i, 0.);

    }
    
    TuningPreparation(void):
    tWhichTuning(EqualTemperament),
    tFundamental(C),
    tFundamentalOffset(0.),
    tAdaptiveIntervalScale(JustTuning),
    tAdaptiveInversional(true),
    tAdaptiveAnchorScale(EqualTemperament),
    tAdaptiveAnchorFundamental(C),
    tAdaptiveClusterThresh(100),
    tAdaptiveHistory(4),
    tCustom({0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.})
    {
        tAbsolute.ensureStorageAllocated(128);
        for(int i=0; i<128; i++) tAbsolute.set(i, 0.);

    }
    
    ~TuningPreparation()
    {
        
    }
    
    inline const String getName() const noexcept {return name;}
    inline const TuningSystem getTuning() const noexcept                    {return tWhichTuning;               }
    inline const PitchClass getFundamental() const noexcept                 {return tFundamental;               }
    inline const float getFundamentalOffset() const noexcept                {return tFundamentalOffset;         }
    inline const TuningSystem getAdaptiveIntervalScale() const noexcept     {return tAdaptiveIntervalScale;     }
    inline const bool getAdaptiveInversional() const noexcept               {return tAdaptiveInversional;       }
    inline const TuningSystem getAdaptiveAnchorScale() const noexcept       {return tAdaptiveAnchorScale;       }
    inline const PitchClass getAdaptiveAnchorFundamental() const noexcept   {return tAdaptiveAnchorFundamental; }
    inline const uint64 getAdaptiveClusterThresh() const noexcept           {return tAdaptiveClusterThresh;     }
    inline const int getAdaptiveHistory() const noexcept                    {return tAdaptiveHistory;           }
    inline const Array<float> getCustomScale() const noexcept               {return tCustom;                    }
    inline const Array<float> getAbsoluteOffsets() const noexcept           {return tAbsolute;                  }
    float getAbsoluteOffset(int midiNoteNumber) const noexcept              {return tAbsolute.getUnchecked(midiNoteNumber);}
    
    
    inline void setName(String n){name = n;}
    inline void setTuning(TuningSystem tuning)                                      {tWhichTuning = tuning;                                 }
    inline void setFundamental(PitchClass fundamental)                              {tFundamental = fundamental;                            }
    inline void setFundamentalOffset(float offset)                                  {tFundamentalOffset = offset;                           }
    inline void setAdaptiveIntervalScale(TuningSystem adaptiveIntervalScale)        {tAdaptiveIntervalScale = adaptiveIntervalScale;        }
    inline void setAdaptiveInversional(bool adaptiveInversional)                    {tAdaptiveInversional = adaptiveInversional;            }
    inline void setAdaptiveAnchorScale(TuningSystem adaptiveAnchorScale)            {tAdaptiveAnchorScale = adaptiveAnchorScale;            }
    inline void setAdaptiveAnchorFundamental(PitchClass adaptiveAnchorFundamental)  {tAdaptiveAnchorFundamental = adaptiveAnchorFundamental;}
    inline void setAdaptiveClusterThresh(uint64 adaptiveClusterThresh)              {tAdaptiveClusterThresh = adaptiveClusterThresh;        }
    inline void setAdaptiveHistory(int adaptiveHistory)                             {tAdaptiveHistory = adaptiveHistory;                    }
    inline void setCustomScale(Array<float> tuning)                                 {tCustom = tuning;                                      }
    inline void setAbsoluteOffsets(Array<float> abs)                                {tAbsolute = abs;                                       }
    void setAbsoluteOffset(int which, float val)                                    {tAbsolute.set(which, val);                             }

    inline void setCustomScaleCents(Array<float> tuning) {
        tCustom = tuning;
        for(int i=0; i<tCustom.size(); i++) tCustom.setUnchecked(i, tCustom.getUnchecked(i) * 0.01f);
    }
    
    inline void setAbsoluteOffsetCents(Array<float> abs) {
        tAbsolute = abs;
        for(int i=tAbsolute.size(); --i >= 0;)
            tAbsolute.setUnchecked(i, tAbsolute.getUnchecked(i) * 0.01f);
    }
    
    
    void print(void)
    {
        DBG("tWhichTuning: " +                  String(tWhichTuning));
        DBG("tFundamental: " +                  String(tFundamental));
        DBG("tFundamentalOffset: " +            String(tFundamentalOffset));
        DBG("tAdaptiveIntervalScale: " +        String(tAdaptiveIntervalScale));
        DBG("tAdaptiveInversional: " +          String(tAdaptiveInversional));
        DBG("tAdaptiveAnchorScale: " +          String(tAdaptiveAnchorScale));
        DBG("tAdaptiveAnchorFundamental: " +    String(tAdaptiveAnchorFundamental));
        DBG("tAdaptiveClusterThresh: " +        String(tAdaptiveClusterThresh));
        DBG("tAdaptiveHistory: " +              String(tAdaptiveHistory));
        DBG("tCustom: " +                       floatArrayToString(tCustom));
        DBG("tAbsolute: " +                     floatArrayToString(tAbsolute));
    }
    
private:
    String name;
    // basic tuning settings, for static tuning
    TuningSystem    tWhichTuning;               //which tuning system to use
    PitchClass      tFundamental;               //fundamental for tuning system
    float           tFundamentalOffset;         //offset
    
    // adaptive tuning params
    TuningSystem    tAdaptiveIntervalScale;     //scale to use to determine successive interval tuning
    bool            tAdaptiveInversional;       //treat the scale inversionally?
    
    TuningSystem    tAdaptiveAnchorScale;       //scale to tune new fundamentals to when in anchored
    PitchClass      tAdaptiveAnchorFundamental; //fundamental for anchor scale
    
    uint64          tAdaptiveClusterThresh;     //ms; max time before fundamental is reset
    int             tAdaptiveHistory;           //cluster max; max number of notes before fundamental is reset
    
    // custom scale and absolute offsets
    Array<float>    tCustom = {0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.}; //custom scale
    Array<float>    tAbsolute;  //offset (in MIDI fractional offsets, like other tunings) for specific notes; size = 128
    
    JUCE_LEAK_DETECTOR(TuningPreparation);
};

class TuningModPreparation : public ReferenceCountedObject
{
public:
    
    typedef ReferenceCountedObjectPtr<TuningModPreparation>   Ptr;
    typedef Array<TuningModPreparation::Ptr>                  PtrArr;
    typedef Array<TuningModPreparation::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<TuningModPreparation>                  Arr;
    typedef OwnedArray<TuningModPreparation, CriticalSection> CSArr;
    
    /*
     TuningId = 0,
     TuningScale,
     TuningFundamental,
     TuningOffset,
     TuningA1IntervalScale,
     TuningA1Inversional,
     TuningA1AnchorScale,
     TuningA1AnchorFundamental,
     TuningA1ClusterThresh,
     TuningA1History,
     TuningCustomScale,
     TuningAbsoluteOffsets
     */
    
    TuningModPreparation(TuningPreparation::Ptr p)
    {
        param.ensureStorageAllocated(cTuningParameterTypes.size());
        
        param.set(TuningScale, String(p->getTuning()));
        param.set(TuningFundamental, String(p->getFundamental()));
        param.set(TuningOffset, String(p->getFundamentalOffset()));
        param.set(TuningA1IntervalScale, String(p->getAdaptiveIntervalScale()));
        param.set(TuningA1Inversional, String(p->getAdaptiveInversional()));
        param.set(TuningA1AnchorScale, String(p->getAdaptiveAnchorScale()));
        param.set(TuningA1AnchorFundamental, String(p->getAdaptiveAnchorFundamental()));
        param.set(TuningA1ClusterThresh, String(p->getAdaptiveClusterThresh()));
        param.set(TuningA1History, String(p->getAdaptiveHistory()));
        param.set(TuningCustomScale, floatArrayToString(p->getCustomScale()));
        param.set(TuningAbsoluteOffsets, floatArrayToString(p->getAbsoluteOffsets()));
        
    }
    
    
    TuningModPreparation(void)
    {
        param.set(TuningScale, "");
        param.set(TuningFundamental, "");
        param.set(TuningOffset, "");
        param.set(TuningA1IntervalScale, "");
        param.set(TuningA1Inversional, "");
        param.set(TuningA1AnchorScale, "");
        param.set(TuningA1AnchorFundamental, "");
        param.set(TuningA1ClusterThresh, "");
        param.set(TuningA1History, "");
        param.set(TuningCustomScale, "");
        param.set(TuningAbsoluteOffsets, "");
    }
    
    
    ~TuningModPreparation(void)
    {
        
    }
    
    inline void copy(TuningPreparation::Ptr p)
    {
        param.set(TuningScale, String(p->getTuning()));
        param.set(TuningFundamental, String(p->getFundamental()));
        param.set(TuningOffset, String(p->getFundamentalOffset()));
        param.set(TuningA1IntervalScale, String(p->getAdaptiveIntervalScale()));
        param.set(TuningA1Inversional, String(p->getAdaptiveInversional()));
        param.set(TuningA1AnchorScale, String(p->getAdaptiveAnchorScale()));
        param.set(TuningA1AnchorFundamental, String(p->getAdaptiveAnchorFundamental()));
        param.set(TuningA1ClusterThresh, String(p->getAdaptiveClusterThresh()));
        param.set(TuningA1History, String(p->getAdaptiveHistory()));
        param.set(TuningCustomScale, floatArrayToString(p->getCustomScale()));
        param.set(TuningAbsoluteOffsets, offsetArrayToString(p->getAbsoluteOffsets()));
    }
    
    inline bool compare(TuningModPreparation::Ptr t)
    {
        return (getParam(TuningScale) == t->getParam(TuningScale) &&
                getParam(TuningFundamental) == t->getParam(TuningFundamental) &&
                getParam(TuningOffset) == t->getParam(TuningOffset) &&
                getParam(TuningA1IntervalScale) == t->getParam(TuningA1IntervalScale) &&
                getParam(TuningA1Inversional) == t->getParam(TuningA1Inversional) &&
                getParam(TuningA1AnchorScale) == t->getParam(TuningA1AnchorScale) &&
                getParam(TuningA1AnchorFundamental) == t->getParam(TuningA1AnchorFundamental) &&
                getParam(TuningA1ClusterThresh) == t->getParam(TuningA1ClusterThresh) &&
                getParam(TuningA1History) == t->getParam(TuningA1History) &&
                getParam(TuningCustomScale) == t->getParam(TuningCustomScale) &&
                getParam(TuningAbsoluteOffsets) == t->getParam(TuningAbsoluteOffsets));
            
    }
    
    inline ValueTree getState(int Id)
    {
        ValueTree prep(vtagModTuning + String(Id));
        
        String p = getParam(TuningScale);
        if (p != String::empty) prep.setProperty( ptagTuning_scale, p.getIntValue(), 0);
        
        p = getParam(TuningFundamental);
        if (p != String::empty) prep.setProperty( ptagTuning_fundamental,           p.getIntValue(), 0);
        
        p = getParam(TuningOffset);
        if (p != String::empty) prep.setProperty( ptagTuning_offset,                p.getFloatValue(), 0 );
        
        p = getParam(TuningA1IntervalScale);
        if (p != String::empty) prep.setProperty( ptagTuning_adaptiveIntervalScale, p.getIntValue(), 0 );
        
        p = getParam(TuningA1Inversional);
        if (p != String::empty) prep.setProperty( ptagTuning_adaptiveInversional,   (bool)p.getIntValue(), 0 );
        
        p = getParam(TuningA1AnchorScale);
        if (p != String::empty) prep.setProperty( ptagTuning_adaptiveAnchorScale,   p.getIntValue(), 0 );
        
        p = getParam(TuningA1AnchorFundamental);
        if (p != String::empty) prep.setProperty( ptagTuning_adaptiveAnchorFund,    p.getIntValue(), 0 );
        
        p = getParam(TuningA1ClusterThresh);
        if (p != String::empty) prep.setProperty( ptagTuning_adaptiveClusterThresh, p.getIntValue(), 0 );
        
        p = getParam(TuningA1History);
        if (p != String::empty) prep.setProperty( ptagTuning_adaptiveHistory,       p.getIntValue(), 0 );

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
        
        ValueTree absolute( vTagTuning_absoluteOffsets);
        count = 0;
        p = getParam(TuningAbsoluteOffsets);
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
    }
    
    
    inline const String getParam(TuningParameterType type)
    {
        if (type != TuningId)   return param[type];
        else                    return "";
    }
    
    inline const StringArray getStringArray(void) { return param; }
    
    inline void setParam(TuningParameterType type, String val)
    {
        param.set(type, val);
    }
    
    void print(void)
    {
        
    }
    
    
private:
    StringArray          param;
    
    JUCE_LEAK_DETECTOR(TuningModPreparation);
};

class TuningProcessor : public ReferenceCountedObject
{
public:
    typedef ReferenceCountedObjectPtr<TuningProcessor>      Ptr;
    typedef Array<TuningProcessor::Ptr>                     Arr;
    typedef Array<TuningProcessor::Ptr, CriticalSection>    CSArr;
    typedef OwnedArray<TuningProcessor>                          PtrArr;
    typedef OwnedArray<TuningProcessor, CriticalSection>         CSPtrArr;
    
    TuningProcessor(TuningPreparation::Ptr active);
    ~TuningProcessor();
    
    inline void setCurrentPlaybackSampleRate(double sr) { sampleRate = sr;}
    
    //returns tuning offsets; add to integer PitchClass
    float getOffset(int midiNoteNumber) const;
    
    //for calculating adaptive tuning
    void keyPressed(int midiNoteNumber);
    
    //for cluster timing
    void processBlock(int numSamples);
    
    //for global tuning adjustment, A442, etc...
    void setGlobalTuningReference(float tuningRef) { globalTuningReference = tuningRef;}
    const float getGlobalTuningReference(void) const noexcept {return globalTuningReference;}
    
    //reset adaptive tuning
    void adaptiveReset();
    
private:
    
    Array<Array<float>> tuningLibrary;
    TuningPreparation::Ptr active;
    
    float   intervalToRatio(float interval) const noexcept { return mtof(interval + 60.) / mtof(60.); }
    float   lastNote[128];
    float   globalTuningReference = 440.; //A440
    
    
    //adaptive tuning functions
    float   adaptiveCalculate(int midiNoteNumber) const;
    void    newNote(int midiNoteNumber, TuningSystem tuningType);
    float   adaptiveCalculateRatio(int midiNoteNumber) const;
    uint64  clusterTime;
    
    int     adaptiveFundamentalNote = 60; //moves with adaptive tuning
    float   adaptiveFundamentalFreq = mtof(adaptiveFundamentalNote);
    int     adaptiveHistoryCounter = 0;
    
    double sampleRate;
    
    
    /* Array of all the default tunings
     Just:       1/1,    16/15,  9/8,    6/5,   5/4,    4/3,    7/5,    3/2,    8/5,    5/3,    7/4,    15/8
     Partial:    1/1,    16/15,  9/8,    7/6,   5/4,    4/3,    11/8,   3/2,    13/8,   5/3,    7/4,    11/6
     Duodene:    1/1,    16/15,  9/8,    6/5,   5/4,    4/3,    45/32,  3/2,    8/5,    5/3,    16/9,   15/8
     Otonal:     1/1,    17/16,  9/8,    19/16, 5/4,    21/16,  11/8,   3/2,    13/8,   27/16,  7/4,    15/8
     Utonal:     1/1,    16/15,  8/7,    32/27, 16/13,  4/3,    16/11,  32/21,  8/5,    32/19,  16/9,   32/17
     */
    const Array<float> tEqualTuning       = {0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.};
    const Array<float> tJustTuning        = {0., .117313, .039101,  .156414, -.13686, -.019547, -.174873, .019547, .136864, -.15641, -.311745, -.11731};
    const Array<float> tPartialTuning     = {0., .117313, .039101, -.331291, -.13686, -.019547, -.486824, .019547, .405273, -.15641, -.311745, -.506371};
    const Array<float> tDuodeneTuning     = {0., .117313, .039101, .156414, -.13686, -.019547, -.097763, .019547, .136864, -.15641, -.039101, -.11731};
    const Array<float> tOtonalTuning      = {0., .049553, .039101, -.02872, -.13686, -.292191, -.486824, .019547, .405273, .058647, -.311745, -.11731};
    const Array<float> tUtonalTuning      = {0., .117313, .311745, .156414, -.405273, -.019547, .486824, .292191, .136864, .024847, -.039101,  -.049553};
    
    JUCE_LEAK_DETECTOR(TuningProcessor);
};

class Tuning : public ReferenceCountedObject
{
    
public:
    typedef ReferenceCountedObjectPtr<Tuning>   Ptr;
    typedef Array<Tuning::Ptr>                  PtrArr;
    typedef Array<Tuning::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<Tuning>                  Arr;
    typedef OwnedArray<Tuning, CriticalSection> CSArr;
    
    
    Tuning(TuningPreparation::Ptr prep,
              int Id):
    sPrep(new TuningPreparation(prep)),
    aPrep(new TuningPreparation(sPrep)),
    processor(new TuningProcessor(aPrep)),
    Id(Id)
    {
        
    }
    
    Tuning(int Id):
    Id(Id)
    {
        sPrep = new TuningPreparation();
        aPrep = new TuningPreparation(sPrep);
        processor = new TuningProcessor(aPrep);
    };
    
    /*
     TuningId = 0,
     TuningScale,
     TuningFundamental,
     TuningOffset,
     TuningA1IntervalScale,
     TuningA1Inversional,
     TuningA1AnchorScale,
     TuningA1AnchorFundamental,
     TuningA1ClusterThresh,
     TuningA1History,
     TuningCustomScale,
   */
    inline ValueTree getState(void)
    {
        ValueTree prep(vtagTuning + String(Id));
        
        prep.setProperty( ptagTuning_Id,                    Id, 0);
        prep.setProperty( ptagTuning_scale,                 sPrep->getTuning(), 0);
        prep.setProperty( ptagTuning_fundamental,           sPrep->getFundamental(), 0);
        prep.setProperty( ptagTuning_offset,                sPrep->getFundamentalOffset(), 0 );
        prep.setProperty( ptagTuning_adaptiveIntervalScale, sPrep->getAdaptiveIntervalScale(), 0 );
        prep.setProperty( ptagTuning_adaptiveInversional,   sPrep->getAdaptiveInversional(), 0 );
        prep.setProperty( ptagTuning_adaptiveAnchorScale,   sPrep->getAdaptiveAnchorScale(), 0 );
        prep.setProperty( ptagTuning_adaptiveAnchorFund,    sPrep->getAdaptiveAnchorFundamental(), 0 );
        prep.setProperty( ptagTuning_adaptiveClusterThresh, (int)sPrep->getAdaptiveClusterThresh(), 0 );
        prep.setProperty( ptagTuning_adaptiveHistory,       sPrep->getAdaptiveHistory(), 0 );
        
        ValueTree scale( vtagTuning_customScale);
        int count = 0;
        for (auto note : sPrep->getCustomScale())
            scale.setProperty( ptagFloat + String(count++), note, 0 );
        prep.addChild(scale, -1, 0);
        
        ValueTree absolute( vTagTuning_absoluteOffsets);
        count = 0;
        for (auto note : sPrep->getAbsoluteOffsets())
        {
            if(note != 0.) absolute.setProperty( ptagFloat + String(count), note, 0 );
            count++;
        }
        prep.addChild(absolute, -1, 0);
        
        //prep.addChild(sPrep->getResetMap()->getState(Id), -1, 0);
        
        return prep;
    }
    
    ~Tuning() {};
    
    inline int getId() {return Id;};
    
    
    TuningPreparation::Ptr      sPrep;
    TuningPreparation::Ptr      aPrep;
    TuningProcessor::Ptr        processor;
    
    
    void reset()
    {
        aPrep->copy(sPrep);
        processor->adaptiveReset();
        DBG("resetting tuning");
    }
    
    
private:
    int Id;
    
    JUCE_LEAK_DETECTOR(Tuning)
};

#endif  // TUNING_H_INCLUDED
