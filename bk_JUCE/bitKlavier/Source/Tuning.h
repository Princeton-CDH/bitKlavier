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

#include "Keymap.h"

#include "SpringTuning.h"
#include "Tunings.h"
//#include "MTS-ESP/Client/libMTSClient.h"

class TuningModification;

class TuningPreparation : public ReferenceCountedObject
{
public:
    typedef ReferenceCountedObjectPtr<TuningPreparation>    Ptr;
    typedef Array<TuningPreparation::Ptr>                   PtrArr;
    typedef Array<TuningPreparation::Ptr, CriticalSection>  CSPtrArr;
    typedef OwnedArray<TuningPreparation>                        Arr;
    typedef OwnedArray<TuningPreparation, CriticalSection>       CSArr;
    
    TuningPreparation(TuningPreparation::Ptr p):
    tScale(p->getScale()),
    tFundamental(p->getFundamental()),
    tFundamentalOffset(p->getFundamentalOffset()),
    tAdaptiveIntervalScale(p->getAdaptiveIntervalScale()),
    tAdaptiveInversional(p->getAdaptiveInversional()),
    tAdaptiveAnchorScale(p->getAdaptiveAnchorScale()),
    tAdaptiveAnchorFundamental(p->getAdaptiveAnchorFundamental()),
    tAdaptiveClusterThresh(p->getAdaptiveClusterThresh()),
    tAdaptiveHistory(p->getAdaptiveHistory()),
    tCustom(p->getCustomScale()),
    tAbsolute(p->getAbsoluteOffsets()),
    nToneSemitoneWidth(p->getNToneSemitoneWidth()),
    nToneRoot(p->getNToneRoot()),
    adaptiveType(p->getAdaptiveType()),
    //client(nullptr),
    stuning(new SpringTuning(p->getSpringTuning()))
    {

    }
    
    inline void copy(TuningPreparation::Ptr p)
    {
        tScale = p->getScale();
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
        nToneSemitoneWidth = p->getNToneSemitoneWidth();
        nToneRoot = p->getNToneRoot();
        adaptiveType = p->getAdaptiveType();
        stuning->copy(p->getSpringTuning());
        
        if (adaptiveType == AdaptiveSpring)
        {
            stuning->setActive(true);
        }
        else
        {
            stuning->setActive(false);
        }
    }
    
    void performModification(TuningModification* p, Array<bool> dirty);

    void stepModdables()
    {
        tScale.step();
        tFundamental.step();
        tFundamentalOffset.step();
        tAdaptiveIntervalScale.step();
        tAdaptiveInversional.step();
        tAdaptiveAnchorScale.step();
        tAdaptiveAnchorFundamental.step();
        tAdaptiveClusterThresh.step();
        tAdaptiveHistory.step();
        tCustom.step();
        tAbsolute.step();
        nToneSemitoneWidth.step();
        nToneRoot.step();
        adaptiveType.step();
        
        stuning->stepModdables();
    }
    
    void resetModdables()
    {
        tFundamental.reset();
        tFundamentalOffset.reset();
        tAdaptiveIntervalScale.reset();
        tAdaptiveInversional.reset();
        tAdaptiveAnchorScale.reset();
        tAdaptiveAnchorFundamental.reset();
        tAdaptiveClusterThresh.reset();
        tAdaptiveHistory.reset();
        tCustom.reset();
        tAbsolute.reset();
        nToneSemitoneWidth.reset();
        nToneRoot.reset();
        adaptiveType.reset();
        
        stuning->resetModdables();
    }

    inline bool compare (TuningPreparation::Ptr p)
    {
        bool custom = true;
        bool absolute = true;
        
        for (int i = p->getCustomScale().size(); --i>=0;)
        {
            if (p->getCustomScale()[i] != getCustomScale()[i])
            {
                custom = false;
                break;
 
            }
        }
        
        for (int i = p->getAbsoluteOffsets().size(); --i>=0;)
        {
            if (p->getAbsoluteOffsets()[i] != tAbsolute.value[i])
            {
                absolute = false;
                break;
                
            }
        }
        
        return (tScale == p->getScale() &&
                tFundamental == p->getFundamental() &&
                tFundamentalOffset == p->getFundamentalOffset() &&
                tAdaptiveIntervalScale == p->getAdaptiveIntervalScale() &&
                tAdaptiveInversional == p->getAdaptiveInversional() &&
                tAdaptiveAnchorScale == p->getAdaptiveAnchorScale() &&
                tAdaptiveAnchorFundamental == p->getAdaptiveAnchorFundamental() &&
                tAdaptiveClusterThresh == p->getAdaptiveClusterThresh() &&
                nToneSemitoneWidth == p->getNToneSemitoneWidth() &&
                nToneRoot == p->getNToneRoot() &&
                (tAdaptiveHistory == p->getAdaptiveHistory()) && custom && absolute);
    }

	inline void randomize()
	{
		Random::getSystemRandom().setSeedRandomly();

		float r[21];

		for (int i = 0; i < 21; i++)  r[i] = (Random::getSystemRandom().nextFloat());
		int idx = 0;
        
		tScale = (TuningSystem)(int)( r[idx++] * TuningSystemNil);
        if ((tScale == AdaptiveTuning) || (tScale == AdaptiveAnchoredTuning)) tScale = (TuningSystem)((int)tScale.value +  2);
        
		tFundamental = (PitchClass)(int)(r[idx++] * PitchClassNil);
		tFundamentalOffset = r[idx++] * 48.0f - 24.0f;
		tAdaptiveIntervalScale = (TuningSystem)(int)(r[idx++] * TuningSystemNil);
		tAdaptiveInversional = (bool)((int) (r[idx++] * 2));
		tAdaptiveAnchorScale = (TuningSystem)(int)(r[idx++] * TuningSystemNil);
		tAdaptiveAnchorFundamental = (PitchClass)(int)(r[idx++] * PitchClassNil);
		tAdaptiveClusterThresh = (int) (r[idx++] * 50000);
		tAdaptiveHistory = (int)(r[idx++] * 100) + 1;
        
        Array<float> arr;
		for (int i = 0; i < 12; ++i)
		{
			arr.add(i, (Random::getSystemRandom().nextFloat() * 2.0f - 1.0f));
		}
        tCustom.set(arr);
        
		arr.clear();
		for (int i = 0; i < ABSOLUTE_OFFSET_SIZE; ++i)
		{
			arr.add(i, (Random::getSystemRandom().nextFloat() * 2.0f - 1.0f));
		}
        tAbsolute.set(arr);
        
		nToneSemitoneWidth = r[idx++] * 200.0f;
		nToneRoot = (int)(r[idx++] * 127) + 1;
        
        stuning->randomize();
	}
    
    TuningPreparation(TuningSystem whichTuning,
                      PitchClass fundamental,
                      float fundamentalOffset,
                      TuningSystem adaptiveIntervalScale,
                      bool adaptiveInversional,
                      TuningSystem adaptiveAnchorScale,
                      PitchClass adaptiveAnchorFundamental,
                      int adaptiveClusterThresh,
                      int adaptiveHistory,
                      Array<float> customScale,
                      float semitoneWidth,
                      int semitoneRoot,
                      SpringTuning::Ptr st):
    tScale(whichTuning),
    tFundamental(fundamental),
    tFundamentalOffset(fundamentalOffset),
    tAdaptiveIntervalScale(adaptiveIntervalScale),
    tAdaptiveInversional(adaptiveInversional),
    tAdaptiveAnchorScale(adaptiveAnchorScale),
    tAdaptiveAnchorFundamental(adaptiveAnchorFundamental),
    tAdaptiveClusterThresh(adaptiveClusterThresh),
    tAdaptiveHistory(adaptiveHistory),
    tCustom(customScale),
    tAbsolute(Array<float>()),
    nToneSemitoneWidth(semitoneWidth),
    nToneRoot(semitoneRoot),
    adaptiveType(AdaptiveNone),
    //client(nullptr),
    stuning(new SpringTuning(st))
    {
        Array<float> arr;
        for(int i=0; i<ABSOLUTE_OFFSET_SIZE; i++) arr.add(0.);
        tAbsolute.set(arr);
        
        if (tScale.value == AdaptiveTuning)
        {
            setAdaptiveType(AdaptiveNormal);
            setScale(EqualTemperament);
        }
        else if (tScale.value == AdaptiveAnchoredTuning)
        {
            setAdaptiveType(AdaptiveAnchored);
            setScale(EqualTemperament);
        }
    }
    
    TuningPreparation(void):
    tScale(EqualTemperament),
    tFundamental(C),
    tFundamentalOffset(0.),
    tAdaptiveIntervalScale(JustTuning),
    tAdaptiveInversional(true),
    tAdaptiveAnchorScale(EqualTemperament),
    tAdaptiveAnchorFundamental(C),
    tAdaptiveClusterThresh(100),
    tAdaptiveHistory(4),
    tCustom({0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.}),
    tAbsolute(Array<float>()),
    nToneSemitoneWidth(100),
    nToneRoot(60),
    adaptiveType(AdaptiveNone),
    //client(nullptr),
    stuning(new SpringTuning())
    {
        Array<float> arr;
        for(int i=0; i<ABSOLUTE_OFFSET_SIZE; i++) arr.add(0.);
        tAbsolute.set(arr);
    }
    
    ~TuningPreparation()
    {
//        if(MTS_HasMaster(client))
//            MTS_DeregisterClient(client);
    }
    
    inline const String getName() const noexcept {return name;}
    inline const TuningSystem getScale() const noexcept
    { return tScale.value;                     }
    inline const PitchClass getFundamental() const noexcept
    { return tFundamental.value;               }
    inline const String getScaleName() const noexcept
    { return cTuningSystemNames[(int)tScale.value];}
    inline const float getFundamentalOffset() const noexcept
    { return tFundamentalOffset.value;         }
    inline const TuningSystem getAdaptiveIntervalScale() const noexcept
    { return tAdaptiveIntervalScale.value;     }
    inline const bool getAdaptiveInversional() const noexcept
    { return tAdaptiveInversional.value;       }
    inline const TuningSystem getAdaptiveAnchorScale() const noexcept
    { return tAdaptiveAnchorScale.value;       }
    inline const PitchClass getAdaptiveAnchorFundamental() const noexcept
    { return tAdaptiveAnchorFundamental.value; }
    inline const int getAdaptiveClusterThresh() const noexcept   { return tAdaptiveClusterThresh.value;     }
    inline const int getAdaptiveHistory() const noexcept            { return tAdaptiveHistory.value; }
    inline const Array<float> getCustomScale() const noexcept
    {
        Array<float> tCustomSemitones;
        tCustomSemitones.ensureStorageAllocated(12);
        for(int i=0; i<tCustom.value.size(); i++)
        {
            tCustomSemitones.set(i, tCustom.value.getUnchecked(i) * 0.01f);
        }
        return tCustomSemitones;
    }
    inline const Array<float> getAbsoluteOffsets() const noexcept
    {
        Array<float> tAbsoluteSemitones;
        tAbsoluteSemitones.ensureStorageAllocated(ABSOLUTE_OFFSET_SIZE);
        for(int i=0; i<tAbsolute.value.size(); i++)
        {
            tAbsoluteSemitones.set(i, tAbsolute.value.getUnchecked(i) * 0.01f);
        }
        return tAbsoluteSemitones;
    }
    float getAbsoluteOffset(int midiNoteNumber) const noexcept      { return tAbsolute.value.getUnchecked(midiNoteNumber); }
    inline const float getNToneSemitoneWidth() const noexcept       { return nToneSemitoneWidth.value;         }
    inline const int getNToneRoot() const noexcept          {return nToneRoot.value;        }
    inline const int getNToneRootPC() const noexcept        {return nToneRoot.value % 12;   }
    inline const int getNToneRootOctave() const noexcept    {return (nToneRoot.value / 12) - 1;  }
    
    inline const Array<float> getAbsoluteOffsetsCents() const noexcept {
        return tAbsolute.value;
    }
    
    inline const Array<float> getCustomScaleCents() const noexcept {
        return tCustom.value;
    }
    
    
    
    
    inline void setName(String n)                                           {name = n; } // DBG("set tuning name " + name);}
    

    
    inline void setScale(TuningSystem tuning)
    {
        if (tuning != AdaptiveTuning && tuning != AdaptiveAnchoredTuning)
        {
           tScale = tuning;
        }
    }
    inline void setScaleByName(String tuningName)
    {
        for (int i=0; i<cTuningSystemNames.size(); i++)
        {
            if(cTuningSystemNames[i] == tuningName) {
                setScale((TuningSystem)i);
            }
        }
    }
    inline void setFundamental(PitchClass fundamental)  { tFundamental = fundamental; }
    inline void setFundamentalOffset(float offset)  { tFundamentalOffset = offset; }
    inline void setAdaptiveIntervalScale(TuningSystem adaptiveIntervalScale)    { tAdaptiveIntervalScale = adaptiveIntervalScale;}
    inline void setAdaptiveInversional(bool adaptiveInversional)            { tAdaptiveInversional = adaptiveInversional; }
    inline void setAdaptiveAnchorScale(TuningSystem adaptiveAnchorScale)    { tAdaptiveAnchorScale = adaptiveAnchorScale;  }
    inline void setAdaptiveAnchorFundamental(PitchClass adaptiveAnchorFundamental)  { tAdaptiveAnchorFundamental = adaptiveAnchorFundamental;}
    inline void setAdaptiveClusterThresh(int adaptiveClusterThresh)  { tAdaptiveClusterThresh = adaptiveClusterThresh; }
    inline void setAdaptiveHistory(int adaptiveHistory)     { tAdaptiveHistory = adaptiveHistory; }
    inline void setCustomScale(Array<float> tuning) //takes in fractional midi
    {
        if (tuning.size() > tCustom.value.size()) tuning.resize(tCustom.value.size());
        
        Array<float> tCustomCents;
        tCustomCents.ensureStorageAllocated(12);
        for(int i=0; i<tuning.size(); i++)
        {
            tCustomCents.set(i, tuning.getUnchecked(i) * 100.0f);
        }

        tCustom.set(tCustomCents);
    }
    inline void setAbsoluteOffsets(Array<float> abs)
    {
        Array<float> tAbsoluteCents;
        tAbsoluteCents.ensureStorageAllocated(12);
        for(int i=0; i<abs.size(); i++)
        {
            tAbsoluteCents.set(i, abs.getUnchecked(i) * 100.0f);
        }
        
        tAbsolute.set(tAbsoluteCents);
        
    }
    void setAbsoluteOffset(int which, float val)
    {
        tAbsolute.base.set(which, val);
        tAbsolute.value.set(which, val);
        tAbsolute.mod.set(which, val);
    }
    inline void setNToneSemitoneWidth(float width) { nToneSemitoneWidth = width; }
    inline void setNToneRoot(int root)
    {
        nToneRoot = root;
        nToneRootPC = nToneRoot.value % 12;
        nToneRootOctave = (nToneRoot.value / 12) - 1;
    }
    inline void setNToneRootPC(int pc)
    {
        nToneRootPC = pc;
        nToneRoot = (nToneRootOctave + 1) * 12 + nToneRootPC;
    }
    inline void setNToneRootOctave(int octave)
    {
        nToneRootOctave = octave;
        nToneRoot = (nToneRootOctave + 1) * 12 + nToneRootPC;
    }

    inline void setCustomScaleCents(Array<float> tuning)
    {
        if (tuning.size() > tCustom.value.size()) tuning.resize(tCustom.value.size());
        tCustom.set(tuning);
    }
    
    inline void setAbsoluteOffsetCents(Array<float> abs)
    {
        tAbsolute.set(abs);
    }
    
    
    
    void print(void)
    {
        DBG("tScale: " +                        String(tScale.value));
        DBG("tFundamental: " +                  String(tFundamental.value));
        DBG("tFundamentalOffset: " +            String(tFundamentalOffset.value));
        DBG("tAdaptiveIntervalScale: " +        String(tAdaptiveIntervalScale.value));
        DBG("tAdaptiveInversional: " +          String((int)tAdaptiveInversional.value));
        DBG("tAdaptiveAnchorScale: " +          String(tAdaptiveAnchorScale.value));
        DBG("tAdaptiveAnchorFundamental: " +    String(tAdaptiveAnchorFundamental.value));
        DBG("tAdaptiveClusterThresh: " +        String(tAdaptiveClusterThresh.value));
        DBG("tAdaptiveHistory: " +              String(tAdaptiveHistory.value));
        DBG("tCustom: " +                       floatArrayToString(tCustom.value));
        DBG("tAbsolute: " +                     floatArrayToString(tAbsolute.value));
        DBG("nToneSemitoneWidth: " +            String(nToneSemitoneWidth.value));
        DBG("nToneRoot: " +                     String(nToneRoot.value));
    }
    
    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ SPRING TUNING ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
    inline SpringTuning::Ptr getSpringTuning(void) { return stuning; }
    inline void setSpringTuning(SpringTuning::Ptr st) { stuning = st;}
    
    inline Particle::PtrArr getParticles(void) { return getSpringTuning()->getParticles(); }
    inline Particle::PtrArr getTetherParticles(void) { return getSpringTuning()->getTetherParticles(); }
//    inline Spring::PtrArr getSprings(void) { return getSpringTuning()->getSprings(); }
    inline Spring::PtrArr getTetherSprings(void) { return getSpringTuning()->getTetherSprings(); }
    
    inline Spring::PtrArr getEnabledSprings(void) { return getSpringTuning()->getEnabledSprings(); }
    
    inline void setSpringWeight(int which, double weight)
    {
        getSpringTuning()->setSpringWeight(which, weight);
    }
    
    inline void setTetherWeight(int which, double weight)
    {
        getSpringTuning()->setTetherWeight(which, weight);
    }
    
    inline void setSpringMode(int which, bool on)
    {
        getSpringTuning()->setSpringMode(which, on);
    }
    
    inline void setSpringsActive(bool status) { getSpringTuning()->setActive(status); }
    inline bool getSpringsActive(void) { return getSpringTuning()->getActive(); }
    
    inline void setSpringRate(double rate) { getSpringTuning()->setRate(rate);}
    inline double getSpringRate(void) { return getSpringTuning()->getRate();}
    
    inline void setSpringStiffness(double stiff) { getSpringTuning()->setStiffness(stiff);}
    inline double getSpringStiffness(void) {return getSpringTuning()->getStiffness();}
    
    inline TuningSystem getCurrentSpringScaleId(void) { return getSpringTuning()->getScaleId(); }
    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
    
    inline void setAdaptiveType (TuningAdaptiveSystemType type) { adaptiveType = type; }
    inline TuningAdaptiveSystemType getAdaptiveType (void) { return adaptiveType.value; }
    
    inline ValueTree getState(void)
    {
        ValueTree prep("params");
        
        tScale.getState(prep, ptagTuning_scale);
        prep.setProperty( ptagTuning_scaleName, String(cTuningSystemNames[(int)tScale.base]), 0);
        
        tFundamental.getState(prep, ptagTuning_fundamental);
        tFundamentalOffset.getState(prep, ptagTuning_offset);
        
        tAdaptiveIntervalScale.getState(prep, ptagTuning_adaptiveIntervalScale);
        tAdaptiveInversional.getState(prep, ptagTuning_adaptiveInversional);
        tAdaptiveAnchorScale.getState(prep, ptagTuning_adaptiveAnchorScale);
        tAdaptiveAnchorFundamental.getState(prep, ptagTuning_adaptiveAnchorFund);
        tAdaptiveClusterThresh.getState(prep, ptagTuning_adaptiveClusterThresh);
        tAdaptiveHistory.getState(prep, ptagTuning_adaptiveHistory);
        
        nToneRoot.getState(prep, ptagTuning_nToneRoot);
        nToneSemitoneWidth.getState(prep, ptagTuning_nToneSemitoneWidth);
        
        adaptiveType.getState(prep, "adaptiveSystem");
        
        tCustom.getState(prep, StringArray(vtagTuning_customScale, ptagFloat));
        
        tAbsolute.getState(prep, StringArray(vTagTuning_absoluteOffsets, ptagFloat));
        
        prep.addChild(getSpringTuning()->getState(), -1, 0);
        
        return prep;
    }
    
    inline void setState(XmlElement* e)
    {        
        tScale.setState(e, ptagTuning_scale, EqualTemperament);
        
        if (tScale.value == AdaptiveTuning)
        {
            setAdaptiveType(AdaptiveNormal);
            setScale(EqualTemperament);
        }
        else if (tScale.value == AdaptiveAnchoredTuning)
        {
            setAdaptiveType(AdaptiveAnchored);
            setScale(EqualTemperament);
        }
        else
        {
            setAdaptiveType(AdaptiveNone);
        }
        
        //if a tuning has been saved by name, use that instead of the index value; need to resave all built-in galleries to do this, eventually
        setScaleByName(e->getStringAttribute(ptagTuning_scaleName));
        
        tFundamental.setState(e, ptagTuning_fundamental, C);
        tFundamentalOffset.setState(e, ptagTuning_offset, 0.);
        tAdaptiveIntervalScale.setState(e, ptagTuning_adaptiveIntervalScale, JustTuning);
        tAdaptiveAnchorScale.setState(e, ptagTuning_adaptiveAnchorScale, EqualTemperament);
        tAdaptiveHistory.setState(e, ptagTuning_adaptiveHistory, 4);
        tAdaptiveInversional.setState(e, ptagTuning_adaptiveInversional, true);
        tAdaptiveClusterThresh.setState(e, ptagTuning_adaptiveClusterThresh, 100);
        tAdaptiveAnchorFundamental.setState(e, ptagTuning_adaptiveAnchorFund, C);

        nToneRoot.setState(e, ptagTuning_nToneRoot, 60);
        nToneSemitoneWidth.setState(e, ptagTuning_nToneSemitoneWidth, 100);
        
        adaptiveType.setState(e, "adaptiveSystem", (TuningAdaptiveSystemType)0);
        if (adaptiveType.value == AdaptiveSpring) setSpringsActive(true);
        
        // custom scale
        for (auto sub : e->getChildIterator())
        {
            if (sub->hasTagName("springtuning"))
            {
                getSpringTuning()->setState(sub);
            }
        }
        
        Array<float> arr;
        for(int i=0; i<12; i++) arr.add(0.);
        tCustom.setState(e, StringArray(vtagTuning_customScale, ptagFloat), arr);
        if (tCustom.value.isEmpty()) tCustom = arr;
        
        arr.clear();
        for(int i=0; i<ABSOLUTE_OFFSET_SIZE; i++) arr.add(0.);
        tAbsolute.setState(e, StringArray(vTagTuning_absoluteOffsets, ptagFloat), arr);
        if (tAbsolute.value.isEmpty()) tAbsolute = arr;
        
        if (getSpringTuning()->getActive())
        {
            setAdaptiveType(AdaptiveSpring);
        }
        
        print();
    }
    
    
    bool modded = false;
    
    // basic tuning settings, for static tuning
    Moddable<TuningSystem>    tScale;               //which tuning system to use
    Moddable<PitchClass>      tFundamental;               //fundamental for tuning system
    Moddable<float>           tFundamentalOffset;         //offset, in MIDI fractional offset
    
    // adaptive tuning params
    Moddable<TuningSystem>    tAdaptiveIntervalScale;     //scale to use to determine successive interval tuning
    Moddable<bool>            tAdaptiveInversional;       //treat the scale inversionally?
    
    Moddable<TuningSystem>    tAdaptiveAnchorScale;       //scale to tune new fundamentals to when in anchored
    Moddable<PitchClass>      tAdaptiveAnchorFundamental; //fundamental for anchor scale
    
    Moddable<int>             tAdaptiveClusterThresh;     //ms; max time before fundamental is reset
    Moddable<int>             tAdaptiveHistory;           //cluster max; max number of notes before fundamental is reset
    
    // custom scale and absolute offsets
    Moddable<Array<float>>    tCustom = Array<float>({0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.}); //custom scale
    Moddable<Array<float>>    tAbsolute;  //offset (in MIDI fractional offsets, like other tunings) for specific notes; size = 128
    
    Moddable<float> nToneSemitoneWidth;
    Moddable<int> nToneRoot;              //which key matches 12-tone ET; 60 by default
    int nToneRootPC = 0;        //which pitch class; 0 by default
    int nToneRootOctave = 4;    //which octave; 4 by default, so C4
    
    Moddable<TuningAdaptiveSystemType> adaptiveType;
   //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~MTS Tuning~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
    
private:
    String name;
    
    // SPRING TUNING STUFF
    SpringTuning::Ptr stuning;

    JUCE_LEAK_DETECTOR(TuningPreparation);
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
           int Id,
           SpringTuning::Ptr st = nullptr):
    prep(new TuningPreparation(prep)),
    Id(Id),
    name("Tuning "+String(Id))
    {
        tuningLibrary.ensureStorageAllocated((int)cTuningSystemNames.size());
        for(int i=0; i<cTuningSystemNames.size(); i++) tuningLibrary.insert(EqualTemperament, tEqualTuning);
        
        setupTuningLibrary();
        
        prep->getSpringTuning()->stop();
    }
    
	Tuning(int Id, bool random = false) :
    currentScale(Tunings::evenTemperament12NoteScale()),
    Id(Id),
    name("Tuning "+String(Id))
    {
		prep = new TuningPreparation();
        
		if (random) randomize();
        
        tuningLibrary.ensureStorageAllocated((int)cTuningSystemNames.size());
        for(int i=0; i<cTuningSystemNames.size(); i++) tuningLibrary.insert(EqualTemperament, tEqualTuning);
        
        setupTuningLibrary();
        
        prep->getSpringTuning()->stop();
    }
    
    inline Tuning::Ptr duplicate()
    {
        TuningPreparation::Ptr copyPrep = new TuningPreparation(prep);
        
        Tuning::Ptr copy = new Tuning(copyPrep, -1);
        
        copy->prep->getSpringTuning()->stop();
        
        copy->setName(name );
        
        return copy;
    }
    
    inline void clear(void)
    {
        prep       = new TuningPreparation();
    }
    
    void setupTuningLibrary()
    {
        //bK originals
        tuningLibrary.set(PartialTuning, tPartialTuning);
        tuningLibrary.set(JustTuning, tJustTuning);
        tuningLibrary.set(EqualTemperament, tEqualTuning);
        tuningLibrary.set(DuodeneTuning, tDuodeneTuning);
        tuningLibrary.set(OtonalTuning, tOtonalTuning);
        tuningLibrary.set(UtonalTuning, tUtonalTuning);
        
        //historical
        tuningLibrary.set(Pythagorean, tPythagorean);
        tuningLibrary.set(Grammateus, tGrammateus);
        tuningLibrary.set(KirnbergerII, tKirnbergerII);
        tuningLibrary.set(KirnbergerIII, tKirnbergerIII);
        tuningLibrary.set(WerkmeisterIII, tWerkmeisterIII);
        tuningLibrary.set(QuarterCommaMeantone, tQuarterCommaMeantone);
        tuningLibrary.set(SplitWolfQCMeantone, tSplitWolfQCMeantone);
        tuningLibrary.set(TransposingQCMeantone, tTransposingQCMeantone);
        tuningLibrary.set(Corrette, tCorrette);
        tuningLibrary.set(Rameau, tRameau);
        tuningLibrary.set(Marpourg, tMarpourg);
        tuningLibrary.set(EggarsEnglishOrd, tEggarsEnglishOrd);
        tuningLibrary.set(ThirdCommaMeantone, tThirdCommaMeantone);
        tuningLibrary.set(DAlembertRousseau, tDAlembertRousseau);
        tuningLibrary.set(Kellner, tKellner);
        tuningLibrary.set(Vallotti, tVallotti);
        tuningLibrary.set(YoungII, tYoungII);
        tuningLibrary.set(SixthCommaMeantone, tSixthCommaMeantone);
        tuningLibrary.set(BachBarnes, tBachBarnes);
        tuningLibrary.set(Neidhardt, tNeidhardt);
        tuningLibrary.set(BachLehman, tBachLehman);
        tuningLibrary.set(BachODonnell, tBachODonnell);
        tuningLibrary.set(BachHill, tBachHill);
        tuningLibrary.set(BachSwich, tBachSwich);
        tuningLibrary.set(Lambert, tLambert);
        tuningLibrary.set(EighthCommaWT, tEighthCommaWT);
        tuningLibrary.set(PinnockModern, tPinnockModern);
        
        //others
        tuningLibrary.set(CommonJust, tCommonJust);
        tuningLibrary.set(Symmetric, tSymmetric);
        tuningLibrary.set(WellTunedPiano, tWellTunedPiano);
        tuningLibrary.set(HarrisonStrict, tHarrisonStrict);

    }
    
    
    ValueTree getState(bool active = false);
    void setState(XmlElement*);
    void loadScalaFile(std::string fname);
    void loadScalaScale(Tunings::Scale& s);
    void loadKBM(Tunings::KeyboardMapping& kbm);
    void loadKBMFile(std::string fname);
    String currentScalaString;
    bool isAbsoluteTuning;
    String currentKBMString;
    Tunings::Scale currentScale;
    Tunings::KeyboardMapping currentKBM;
    
    String generateScalaString();
    
    ~Tuning() {};
    
    inline int getId() {return Id;}
    inline void setId(int newId) { Id = newId;}
    
    TuningPreparation::Ptr      prep;
    
    void reset()
    {
        prep->resetModdables();
        DBG("resetting tuning");
    }
    
    inline void copy(Tuning::Ptr from)
    {
        prep->copy(from->prep);
    }

	inline void randomize()
	{
		clear();
		prep->randomize();
        
		Id = Random::getSystemRandom().nextInt(Range<int>(1, 1000));
		name = "random";
	}
    
    inline TuningSystem getCurrentScaleId(void)
    {
        return prep->getScale();
    }
    
    inline SpringTuning::Ptr getCurrentSpringTuning(void)
    {
        return prep->getSpringTuning();
    }
    
    inline String getName(void) const noexcept {return name;}
    
    inline void setName(String newName)
    {
        name = newName;
    }
    
    Array<float> getCurrentScale()
    {
        if(prep->getScale() == CustomTuning)
        {
            return prep->getCustomScale();
        }
        
        return getTuningOffsets(prep->getScale());
    }
    
    Array<float> getScale(int which)
    {
        TuningSystem tuning = (TuningSystem) which;
        
        if (tuning == CustomTuning)
        {
            return prep->getCustomScale();
        }
        
        return getTuningOffsets(tuning);
    }
    
    Array<float> getScaleCents(int which)
    {
        Array<float> mScale = getScale(which);
        Array<float> cScale;
        
        for(int i=0; i<12; i++)
        {
            cScale.insert(i, mScale.getUnchecked(i) * 100.0);
        }
        
        return cScale;
    }
    
    Array<float> getCurrentScaleCents()
    {
        Array<float> mScale = getCurrentScale();
        Array<float> cScale;
        
        for(int i=0; i<12; i++)
        {
            cScale.insert(i, mScale.getUnchecked(i) * 100.0);
        }
        
        return cScale;
    }
    
    
    Array<Array<float>> tuningLibrary;
    
private:
    int Id;
    String name;

    
    Array<float> getTuningOffsets(TuningSystem which) {return tuningLibrary.getUnchecked(which); }
    
    
    const Array<float> tEqualTuning     = Array<float>( {0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.} );
	const Array<float> tJustTuning      = Array<float>({ 0., .117313, .039101, .156414, -.13686, -.019547, -.174873, .019547, .136864, -.15641, -.311745, -.11731 });
	const Array<float> tPartialTuning   = Array<float>({ 0., .117313, .039101, -.331291, -.13686, -.019547, -.486824, .019547, .405273, -.15641, -.311745, -.506371 });
	const Array<float> tDuodeneTuning   = Array<float>({ 0., .117313, .039101, .156414, -.13686, -.019547, -.097763, .019547, .136864, -.15641, -.039101, -.11731 });
	const Array<float> tOtonalTuning    = Array<float>({ 0., .049553, .039101, -.02872, -.13686, -.292191, -.486824, .019547, .405273, .058647, -.311745, -.11731 } );
	const Array<float> tUtonalTuning    = Array<float>({ 0., .117313, .311745, .156414, -.405273, -.019547, .486824, .292191, .136864, .024847, -.039101, -.049553 } );
    
    //historical temperaments
    const Array<float> tPythagorean     = Array<float>({ 0., 0.13685, 0.0391, -0.05865, 0.0782, -0.01955, 0.1173, 0.01955, 0.1564, 0.05865, -0.0391, 0.09775} );
    const Array<float> tGrammateus      = Array<float>({ 0., 0.01955, 0.0391, 0.05865, 0.0782, -0.01955, 0, 0.01955, 0.0391, 0.05865, 0.0782, 0.09775 });
    const Array<float> tKirnbergerII    = Array<float>({ 0., -0.0977871, 0.0391, -0.0586871, -0.1369, -0.0195871, -0.0978, 0.01955, -0.0782371, -0.0489, -0.0391371, -0.11735 });
    const Array<float> tKirnbergerIII   = Array<float>({ 0., -0.0977871, -0.06845, -0.0586871, -0.1369, -0.0195871, -0.0978, -0.034225, -0.0782371, -0.102675, -0.0391371, -0.11735 });
    
    const Array<float> tWerkmeisterIII           = Array<float>({0, -0.0977871, -0.06845, -0.0586871, -0.083125, -0.0195871, -0.0978, -0.034225, -0.0782371, -0.102675, -0.0391371, -0.063575 });
    const Array<float> tQuarterCommaMeantone     = Array<float>({0, -0.239575, -0.06845, 0.122175, -0.1369, 0.053725, -0.20535, -0.034225, -0.2738, -0.102675, 0.08795, -0.171125 });
    const Array<float> tSplitWolfQCMeantone      = Array<float>({0, -0.239575, -0.06845, -0.092925, -0.1369, 0.053725, -0.20535, -0.034225, -0.2738, -0.102675, 0.08795, -0.171125 });
    const Array<float> tTransposingQCMeantone    = Array<float>({0, -0.239575, -0.06845, 0.122175, -0.1369, 0.053725, -0.20535, -0.034225, 0.1564, -0.102675, 0.08795, -0.171125 });
    const Array<float> tCorrette                 = Array<float>({0, -0.239575, -0.06845, -0.0212249, -0.1369, 0.0509674, -0.20535, -0.034225, -0.23795, -0.102675, 0.0148713, -0.171125 });
    const Array<float> tRameau                   = Array<float>({0, -0.132025, -0.06845, -0.0301876, -0.1369, 0.0537248, -0.151575, -0.034225, -0.148325, -0.102675, 0.0879498, -0.171125 });
    const Array<float> tMarpourg                 = Array<float>({0, -0.14995, -0.06845, -0.0391498, -0.1369, 0.0716504, -0.20535, -0.034225, -0.0945499, -0.102675, 0.0162503, -0.171125 });
    const Array<float> tEggarsEnglishOrd         = Array<float>({0, -0.1858, -0.06845, -0.092925, -0.1369, 0.053725, -0.178463, -0.034225, -0.16625, -0.102675, -0.0196, -0.171125 });
    const Array<float> tThirdCommaMeantone       = Array<float>({0, -0.365049, -0.1043, 0.175951, -0.2086, 0.0716508, -0.3129, -0.0521499, -0.417199, -0.15645, 0.123801, -0.26075 });
    const Array<float> tDAlembertRousseau        = Array<float>({0, -0.132025, -0.06845, -0.0943036, -0.1369, -0.0221113, -0.13365, -0.034225, -0.1304, -0.102675, -0.0582074, -0.135275 });
    const Array<float> tKellner                  = Array<float>({0, -0.09775, -0.05474, -0.05865, -0.10948, -0.01955, -0.1173, -0.02737, -0.0782, -0.08211, -0.0391, -0.08993 });
    const Array<float> tVallotti                 = Array<float>({0, -0.0586504, -0.0391002, -0.0195504, -0.0782003, 0.0195496, -0.0782004, -0.0195501, -0.0391004, -0.0586502, 0, -0.0977504 });
    const Array<float> tYoungII                  = Array<float>({0, -0.0977505, -0.0391002, -0.0586505, -0.0782003, -0.0195505, -0.1173, -0.0195501, -0.0782005, -0.0586502, -0.0391005, -0.0977504 });
    const Array<float> tSixthCommaMeantone       = Array<float>({0, -0.136851, -0.0391002, 0.0586493, -0.0782003, 0.0195491, -0.1173, -0.0195501, -0.156401, -0.0586502, 0.0390992, -0.0977504 });
    const Array<float> tBachBarnes               = Array<float>({0, -0.0586504, -0.0391002, -0.0195504, -0.0782003, 0.0195496, -0.0782004, -0.0195501, -0.0391004, -0.0586502, 0, -0.0586503});
    const Array<float> tNeidhardt                = Array<float>({0, -0.0391, -0.0391002, -0.0195499, -0.0586502, -0.0195498, -0.0391001, -0.0195501, -0.0390999, -0.0586502, -0.0195498, -0.0391002 });
    const Array<float> tBachLehman               = Array<float>({0, -0.0195503, -0.0391002, -0.0195502, -0.0782003, 0.0180461, -0.0391003, -0.0195501, -0.0195502, -0.0586502, -0.0195501, -0.0586503 });
    const Array<float> tBachODonnell             = Array<float>({0, -0.0391002, -0.0391002, -0.0391001, -0.0391002, -0.01955, -0.0391003, -0.0195501, -0.0391002, -0.0586502, -0.0391, -0.0586503 });
    const Array<float> tBachHill                 = Array<float>({0, -0.0436113, -0.0330845, -0.0225575, -0.0661691, 0.0165425, -0.0451152, -0.0165423, -0.0421075, -0.0496268, -0.00300749, -0.0466191 });
    const Array<float> tBachSwich                = Array<float>({0, -0.0703799, -0.05474, -0.0182465, -0.10948, 0.0273702, -0.0899299, -0.02737, -0.0443132, -0.08211, 0.00782023, -0.08993 });
    const Array<float> tLambert                  = Array<float>({0, -0.0642355, -0.0279285, -0.0251355, -0.055857, 0.0139645, -0.0837855, -0.0139642, -0.0446855, -0.0418928, -0.00558551, -0.0698213 });
    const Array<float> tEighthCommaWT            = Array<float>({0, -0.0391, -0.01955, 0, -0.0391, 0.009775, -0.05865, -0.009775, -0.01955, -0.029325, 0.01955, -0.048875 });
    const Array<float> tPinnockModern            = Array<float>({0, -0.0390998, -0.01955, 0, -0.0391, 0.0195503, -0.0390998, -0.009775, -0.0195498, -0.029325, 0, -0.0390999 });
    
    /*
     also add:
     'common just': [1/1, 16/15, 9/8, 6/5, 5/4, 4/3, 45/32, 3/2, 8/5, 5/3, 9/5, 15/8, 2/1],
     'symmetric': [1/1, 16/15, 9/8, 6/5, 5/4, 4/3, Math.sqrt(2), 3/2, 8/5, 5/3, 16/9,  15/8,  2/1],
     'well tuned piano': [1/1, 567/512, 9/8, 147/128, 21/16, 1323/1024, 189/128, 3/2, 49/32, 7/4, 441/256, 63/32, 2/1],
     'harrison' : 1/1, 28/27, 9/8, 32/27, 5/4, 4/3, 112/81, 3/2, 128/81, 5/3, 16/9, 15/8 //from Strict Songs
    */
    
    const Array<float> tCommonJust              = Array<float>({0, 0.11731285269777758, 0.039100017307748376, 0.15641287000552553, -0.13686286135165177, -0.019550008653875465, -0.09776284404390367, 0.019550008653873192, 0.13686286135165232, -0.15641287000552553, 0.175962878659401, -0.117312852697778});
    const Array<float> tSymmetric               = Array<float>({0, 0.11731285269777758, 0.039100017307748376, 0.15641287000552553, -0.13686286135165177, -0.019550008653875465, 0., 0.019550008653873192, 0.13686286135165232, -0.15641287000552553, -0.03910001730774866, -0.117312852697778 });
    const Array<float> tWellTunedPiano          = Array<float>({0, 0.7664590993067458, 0.039100017307748376, -0.603931861963627, 0.7078090733451233, -0.564831844655879, 0.746909090652872, 0.019550008653873192, -0.6234818706175008, 0.6882590646912502, -0.5843818533097533, 0.727359081999 });
    const Array<float> tHarrisonStrict          = Array<float>({0, -0.3703909612703742, 0.039100017307748376, -0.05865002596162355, -0.13686286135165177, -0.019550008653875465, -0.3899409699242483, 0.019550008653873192, -0.07820003461549846, -0.15641287000552553, -0.03910001730774866, -0.117312852697778 });
    
    
    JUCE_LEAK_DETECTOR(Tuning)
};


/*
 TuningProcessor handles events (note messages, and timing) and updates
 values internally that other preparation can access as needed.
 */

class TuningProcessor : public ReferenceCountedObject
{
public:
    typedef ReferenceCountedObjectPtr<TuningProcessor>      Ptr;
    typedef Array<TuningProcessor::Ptr>                     PtrArr;
    typedef Array<TuningProcessor::Ptr, CriticalSection>    CSArr;
    typedef OwnedArray<TuningProcessor>                          Arr;
    typedef OwnedArray<TuningProcessor, CriticalSection>         CSPtrArr;
    
    TuningProcessor(BKAudioProcessor& processor, Tuning::Ptr tuning);
    ~TuningProcessor();
    
    inline void prepareToPlay(double sr) { ; }
    
    //returns tuning offsets; add to integer PitchClass
    float getOffset(int midiNoteNumber, bool updateLastInterval);
    
    //for calculating adaptive tuning
    void keyPressed(int midiNoteNumber, Array<float>& targetVelocities, bool fromPress);
    void keyReleased(int midiNoteNumber, Array<float>& targetVelocities, bool fromPress);
    
    inline int getId(void) const noexcept { return tuning->getId(); }
    
    inline void setTuning(Tuning::Ptr newTuning) { tuning = newTuning;}
    inline Tuning::Ptr getTuning(void) const noexcept { return tuning; }
    
    //for cluster timing
    void processBlock(int numSamples);
    
    //for global tuning adjustment, A442, etc...
    void setGlobalTuningReference(float tuningRef) { globalTuningReference = tuningRef;}
    const float getGlobalTuningReference(void) const noexcept {return globalTuningReference;}
    
    float getLastNoteTuning() { return lastNoteTuning; }
    float getLastIntervalTuning() { return lastIntervalTuning; }
    
    int getAdaptiveFundamentalNote() { return adaptiveFundamentalNote;}
    float getAdaptiveFundamentalFreq() { return adaptiveFundamentalFreq;}
    int getAdaptiveHistoryCounter() { return adaptiveHistoryCounter;}
    int getAdaptiveClusterTimer();
    
    void setAdaptiveFundamentalNote(int newNote) { adaptiveFundamentalNote = newNote;}
    void setAdaptiveFundamentalFreq(float newFreq) { adaptiveFundamentalFreq = newFreq;}
    void setAdaptiveHistoryCounter(int newCounter) { adaptiveHistoryCounter = newCounter;}
    
    //reset adaptive tuning
    void adaptiveReset();
    
    inline void reset(void)
    {
        adaptiveReset();
        tuning->prep->resetModdables();
    }
    
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
    

private:
    BKAudioProcessor& processor;
    
    Tuning::Ptr tuning;
    
    Keymap::PtrArr keymaps;

    float   globalTuningReference = 440.; //A440
    float   intervalToRatio(float interval) const noexcept {
        return mtof(interval + 60., globalTuningReference) / mtof(60., globalTuningReference);
    }
    float   lastNote[128];
    
    float lastNoteTuning;
    float lastIntervalTuning;
    
    //adaptive tuning functions
    float   adaptiveCalculate(int midiNoteNumber) const;
    void    newNote(int midiNoteNumber, TuningSystem tuningType);
    float   adaptiveCalculateRatio(int midiNoteNumber) const;
    int     clusterTime;
    
    int     adaptiveFundamentalNote = 60; //moves with adaptive tuning
    float   adaptiveFundamentalFreq = mtof(adaptiveFundamentalNote);
    int     adaptiveHistoryCounter = 0;
    
    Array<Array<float>> velocities;
    Array<Array<float>> invertVelocities;
   
    JUCE_LEAK_DETECTOR(TuningProcessor);
};

#endif  // TUNING_H_INCLUDED
