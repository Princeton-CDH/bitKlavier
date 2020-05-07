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


/*
TuningPreparation holds all the state variable values for the
Tuning. As with other preparation types, bK will use
two instantiations of TuningPreparation for every active
Synchronic in the gallery, one to store the static state of the
preparation, and the other to store the active state. These will
be the same, unless a Modification is triggered, in which case the
active state will be changed (and a Reset will revert the active state
to the static state).
*/

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
    
    void performModification(TuningPreparation::Ptr p, Array<bool> dirty)
    {
        if (dirty[TuningScale]) tScale = p->getScale();
        if (dirty[TuningFundamental]) tFundamental = p->getFundamental();
        if (dirty[TuningOffset]) tFundamentalOffset = p->getFundamentalOffset();
        if (dirty[TuningA1IntervalScale]) tAdaptiveIntervalScale = p->getAdaptiveIntervalScale();
        if (dirty[TuningA1Inversional]) tAdaptiveInversional = p->getAdaptiveInversional();
        if (dirty[TuningA1AnchorScale]) tAdaptiveAnchorScale = p->getAdaptiveAnchorScale();
        if (dirty[TuningA1AnchorFundamental]) tAdaptiveAnchorFundamental = p->getAdaptiveAnchorFundamental();
        if (dirty[TuningA1ClusterThresh]) tAdaptiveClusterThresh = p->getAdaptiveClusterThresh();
        if (dirty[TuningA1History]) tAdaptiveHistory = p->getAdaptiveHistory();
        if (dirty[TuningCustomScale])
        {
            // tCustom = p->getCustomScale();
            Array<float> temp = p->getCustomScale();
            for (int i = 0; i < tCustom.size(); i++) tCustom.set(i, temp[i]);
            //tCustom = Array<float>(p->getCustomScale());
            tScale = CustomTuning;
        }
        if (dirty[TuningAbsoluteOffsets]) tAbsolute = p->getAbsoluteOffsets();
        if (dirty[TuningNToneSemitoneWidth]) nToneSemitoneWidth = p->getNToneSemitoneWidth();
        if (dirty[TuningNToneRootCB]) nToneRoot = p->getNToneRoot();
        if (dirty[TuningAdaptiveSystem]) adaptiveType = p->getAdaptiveType();

        stuning->performModification(p->getSpringTuning(), dirty);
        
        if (adaptiveType == AdaptiveSpring)
        {
            stuning->setActive(true);
        }
        else
        {
            stuning->setActive(false);
        }
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
        if ((tScale == AdaptiveTuning) || (tScale == AdaptiveAnchoredTuning)) tScale = (TuningSystem)((int)tScale +  2);
        
		tFundamental = (PitchClass)(int)(r[idx++] * PitchClassNil);
		tFundamentalOffset = r[idx++] * 48.0f - 24.0f;
		tAdaptiveIntervalScale = (TuningSystem)(int)(r[idx++] * TuningSystemNil);
		tAdaptiveInversional = (bool)((int) (r[idx++] * 2));
		tAdaptiveAnchorScale = (TuningSystem)(int)(r[idx++] * TuningSystemNil);
		tAdaptiveAnchorFundamental = (PitchClass)(int)(r[idx++] * PitchClassNil);
		tAdaptiveClusterThresh = (uint64) (r[idx++] * 50000);
		tAdaptiveHistory = (int)(r[idx++] * 100) + 1;
		tCustom.clear();
		for (int i = 0; i < 12; ++i)
		{
			tCustom.add(i, (Random::getSystemRandom().nextFloat() * 2.0f - 1.0f));
		}
		tAbsolute.clear();
		for (int i = 0; i < 128; ++i)
		{
			tAbsolute.add(i, (Random::getSystemRandom().nextFloat() * 2.0f - 1.0f));
		}
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
                      uint64 adaptiveClusterThresh,
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
    nToneSemitoneWidth(semitoneWidth),
    nToneRoot(semitoneRoot),
    stuning(new SpringTuning(st))
    {
        tAbsolute.ensureStorageAllocated(128);
        for(int i=0; i<128; i++) tAbsolute.set(i, 0.);
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
    nToneSemitoneWidth(100),
    nToneRoot(60)
    {
        tAbsolute.ensureStorageAllocated(128);
        for(int i=0; i<128; i++) tAbsolute.set(i, 0.);
        
        stuning = new SpringTuning();

    }
    
    ~TuningPreparation()
    {
        
    }
    
    inline const String getName() const noexcept {return name;}
    inline const TuningSystem getScale() const noexcept                     {return tScale;                     }
    inline const PitchClass getFundamental() const noexcept                 {return tFundamental;               }
    inline const String getScaleName() const noexcept                       {return cTuningSystemNames[(int)tScale];}
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
    inline const float getNToneSemitoneWidth() const noexcept               {return nToneSemitoneWidth;         }
    inline const int getNToneRoot() const noexcept                          {return nToneRoot;                  }
    inline const int getNToneRootPC() const noexcept                        {return nToneRoot % 12;             }
    inline const int getNToneRootOctave() const noexcept                    {return (nToneRoot / 12) - 1;       }
    
    inline const Array<float> getAbsoluteOffsetsCents() const noexcept {
        Array<float> tAbsoluteCents;
        tAbsoluteCents.ensureStorageAllocated(128);
        for(int i=0; i<tAbsolute.size(); i++)
        {
            tAbsoluteCents.set(i, tAbsolute.getUnchecked(i) * 100.);
        }
        return tAbsoluteCents;
    }
    
    inline const Array<float> getCustomScaleCents() const noexcept {
        Array<float> tCustomCents;
        tCustomCents.ensureStorageAllocated(12);
        for(int i=0; i<tCustom.size(); i++)
        {
            tCustomCents.set(i, tCustom.getUnchecked(i) * 100.);
        }
        return tCustomCents;
    }
    
    inline void setName(String n)                                           {name = n; DBG("set tuning name " + name);}
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
    inline void setFundamental(PitchClass fundamental)                              {tFundamental = fundamental; }
    inline void setFundamentalOffset(float offset)                                  {tFundamentalOffset = offset; }
    inline void setAdaptiveIntervalScale(TuningSystem adaptiveIntervalScale)        {tAdaptiveIntervalScale = adaptiveIntervalScale;}
    inline void setAdaptiveInversional(bool adaptiveInversional)                    {tAdaptiveInversional = adaptiveInversional; }
    inline void setAdaptiveAnchorScale(TuningSystem adaptiveAnchorScale)            {tAdaptiveAnchorScale = adaptiveAnchorScale;  }
    inline void setAdaptiveAnchorFundamental(PitchClass adaptiveAnchorFundamental)  {tAdaptiveAnchorFundamental = adaptiveAnchorFundamental;}
    inline void setAdaptiveClusterThresh(uint64 adaptiveClusterThresh)              {tAdaptiveClusterThresh = adaptiveClusterThresh; }
    inline void setAdaptiveHistory(int adaptiveHistory)                             {tAdaptiveHistory = adaptiveHistory; }
    inline void setCustomScale(Array<float> tuning)                                 {tCustom = tuning;      }
    inline void setAbsoluteOffsets(Array<float> abs)                                {tAbsolute = abs;   }
    void setAbsoluteOffset(int which, float val)                                    {tAbsolute.set(which, val);  }
    inline void setNToneSemitoneWidth(float width)                                  {nToneSemitoneWidth = width; }
    inline void setNToneRoot(int root)
    {
        nToneRoot = root;
        nToneRootPC = nToneRoot % 12;
        nToneRootOctave = (nToneRoot / 12) - 1;
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

    inline void setCustomScaleCents(Array<float> tuning) {
        for(int i=0; i<tCustom.size() && i<tuning.size(); i++)
        {
            tCustom.setUnchecked(i, tuning.getUnchecked(i) * 0.01f);
        }
    }
    
    inline void setAbsoluteOffsetCents(Array<float> abs) {
        for(int i=0; i<tAbsolute.size() && i<abs.size(); i++)
            tAbsolute.setUnchecked(i, abs.getUnchecked(i) * 0.01f);
    }
    
    
    void print(void)
    {
        DBG("tScale: " +                  String(tScale));
        DBG("tFundamental: " +                  String(tFundamental));
        DBG("tFundamentalOffset: " +            String(tFundamentalOffset));
        DBG("tAdaptiveIntervalScale: " +        String(tAdaptiveIntervalScale));
        DBG("tAdaptiveInversional: " +          String((int)tAdaptiveInversional));
        DBG("tAdaptiveAnchorScale: " +          String(tAdaptiveAnchorScale));
        DBG("tAdaptiveAnchorFundamental: " +    String(tAdaptiveAnchorFundamental));
        DBG("tAdaptiveClusterThresh: " +        String(tAdaptiveClusterThresh));
        DBG("tAdaptiveHistory: " +              String(tAdaptiveHistory));
        DBG("tCustom: " +                       floatArrayToString(tCustom));
        DBG("tAbsolute: " +                     floatArrayToString(tAbsolute));
        DBG("nToneSemitoneWidth: " +            String(nToneSemitoneWidth));
        DBG("nToneRoot: " +                     String(nToneRoot));
    }
    
    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ SPRING TUNING ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
    inline SpringTuning::Ptr getSpringTuning(void) { return stuning; }
    inline void setSpringTuning(SpringTuning::Ptr st) { stuning = st;}
    
    inline Particle::PtrArr getParticles(void) { return getSpringTuning()->getParticles(); }
    inline Particle::PtrArr getTetherParticles(void) { return getSpringTuning()->getTetherParticles(); }
    inline Spring::PtrArr getSprings(void) { return getSpringTuning()->getSprings(); }
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
    inline TuningAdaptiveSystemType getAdaptiveType (void) { return adaptiveType; }
    
    inline ValueTree getState(void)
    {
        ValueTree prep("params");
        
        prep.setProperty( ptagTuning_scale,                 getScale(), 0);
        prep.setProperty( ptagTuning_scaleName,             getScaleName(), 0);
        prep.setProperty( ptagTuning_fundamental,           getFundamental(), 0);
        prep.setProperty( ptagTuning_offset,                getFundamentalOffset(), 0 );
        prep.setProperty( ptagTuning_adaptiveIntervalScale, getAdaptiveIntervalScale(), 0 );
        prep.setProperty( ptagTuning_adaptiveInversional,   getAdaptiveInversional(), 0 );
        prep.setProperty( ptagTuning_adaptiveAnchorScale,   getAdaptiveAnchorScale(), 0 );
        prep.setProperty( ptagTuning_adaptiveAnchorFund,    getAdaptiveAnchorFundamental(), 0 );
        prep.setProperty( ptagTuning_adaptiveClusterThresh, (int)getAdaptiveClusterThresh(), 0 );
        prep.setProperty( ptagTuning_adaptiveHistory,       getAdaptiveHistory(), 0 );
        
        prep.setProperty( ptagTuning_nToneRoot,             getNToneRoot(), 0);
        prep.setProperty( ptagTuning_nToneSemitoneWidth,    getNToneSemitoneWidth(), 0 );
        
        prep.setProperty( "adaptiveSystem", getAdaptiveType(), 0);
        
        ValueTree scale( vtagTuning_customScale);
        int count = 0;
        for (auto note : getCustomScale())
            scale.setProperty( ptagFloat + String(count++), note, 0 );
        prep.addChild(scale, -1, 0);
        
        ValueTree absolute( vTagTuning_absoluteOffsets);
        count = 0;
        for (auto note : getAbsoluteOffsets())
        {
            if(note != 0.) absolute.setProperty( ptagFloat + String(count), note, 0 );
            count++;
        }
        prep.addChild(absolute, -1, 0);
        
        prep.addChild(getSpringTuning()->getState(), -1, 0);
        
        return prep;
    }
    
    inline void setState(XmlElement* e)
    {
        int i; float f; bool b;
        
        i = e->getStringAttribute( ptagTuning_scale).getIntValue();
        setScale((TuningSystem)i);
        
        TuningSystem scale = (TuningSystem) i;
        
        if (scale == AdaptiveTuning)
        {
            setAdaptiveType(AdaptiveNormal);
            setScale(EqualTemperament);
        }
        else if (scale == AdaptiveAnchoredTuning)
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
        
        i = e->getStringAttribute( ptagTuning_fundamental).getIntValue();
        setFundamental((PitchClass)i);
        
        f = e->getStringAttribute( ptagTuning_offset).getFloatValue();
        setFundamentalOffset(f);
        
        i = e->getStringAttribute( ptagTuning_adaptiveIntervalScale).getIntValue();
        setAdaptiveIntervalScale((TuningSystem)i);
        
        i = e->getStringAttribute( ptagTuning_adaptiveAnchorScale).getIntValue();
        setAdaptiveAnchorScale((TuningSystem)i);
        
        i = e->getStringAttribute( ptagTuning_adaptiveHistory).getIntValue();
        setAdaptiveHistory(i);
        
        b = (bool) e->getStringAttribute( ptagTuning_adaptiveInversional).getIntValue();
        setAdaptiveInversional(b);
        
        i = e->getStringAttribute( ptagTuning_adaptiveClusterThresh).getIntValue();
        setAdaptiveClusterThresh(i);
        
        i = e->getStringAttribute( ptagTuning_adaptiveAnchorFund).getIntValue();
        setAdaptiveAnchorFundamental((PitchClass)i);
        
        i = e->getStringAttribute( ptagTuning_nToneRoot).getIntValue();
        if(i > 0) setNToneRoot(i);
        else setNToneRoot(60);
        
        f = e->getStringAttribute( ptagTuning_nToneSemitoneWidth).getFloatValue();
        if(f > 0) setNToneSemitoneWidth(f);
        else setNToneSemitoneWidth(100);
        
        String str = e->getStringAttribute("adaptiveSystem");
        
        if (str != "")
        {
            i = str.getIntValue();
            
            TuningAdaptiveSystemType type = (TuningAdaptiveSystemType) i;
            
            setAdaptiveType(type);
            
            if (type == AdaptiveSpring)
            {
                setSpringsActive(true);
            }
        }
        
        // custom scale
        forEachXmlChildElement (*e, sub)
        {
            if (sub->hasTagName("springtuning"))
            {
                getSpringTuning()->setState(sub);
            }
            else if (sub->hasTagName(vtagTuning_customScale))
            {
                // DBG("TuningModification::setState");
                Array<float> scale;
                for (int k = 0; k < sub->getNumAttributes(); k++)
                {
                    String attr = sub->getStringAttribute(ptagFloat + String(k));
                    
                    if (attr == String()) break;
                    else
                    {
                        
                        f = attr.getFloatValue();
                        scale.add(f);
                        // if (f != 0) DBG("TuningModification::setState Val = " + String(f));
                    }
                }
                setCustomScale(scale);
            }
            else if (sub->hasTagName(vTagTuning_absoluteOffsets))
            {
                Array<float> absolute;
                absolute.ensureStorageAllocated(ABSOLUTE_OFFSET_SIZE);
                for (int k = 0; k < ABSOLUTE_OFFSET_SIZE; k++)
                {
                    String attr = sub->getStringAttribute(ptagFloat + String(k));
                    f = attr.getFloatValue();
                    absolute.set(k, f);
                    
                }
                
                setAbsoluteOffsets(absolute);
            }
        }
        
        if (getSpringTuning()->getActive())
        {
            setAdaptiveType(AdaptiveSpring);
        }
        
        // MOD REWORK FIX
        // getSpringTuning()->setTetherTuning(getStaticScale());
        
    }
    
private:
    String name;
    // basic tuning settings, for static tuning
    TuningSystem    tScale;               //which tuning system to use
    PitchClass      tFundamental;               //fundamental for tuning system
    float           tFundamentalOffset;         //offset, in MIDI fractional offset
    
    // adaptive tuning params
    TuningSystem    tAdaptiveIntervalScale;     //scale to use to determine successive interval tuning
    bool            tAdaptiveInversional;       //treat the scale inversionally?
    
    TuningSystem    tAdaptiveAnchorScale;       //scale to tune new fundamentals to when in anchored
    PitchClass      tAdaptiveAnchorFundamental; //fundamental for anchor scale
    
    uint64          tAdaptiveClusterThresh;     //ms; max time before fundamental is reset
    int             tAdaptiveHistory;           //cluster max; max number of notes before fundamental is reset
    
    
    
    // custom scale and absolute offsets
    Array<float>    tCustom = Array<float>({0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.}); //custom scale
    Array<float>    tAbsolute;  //offset (in MIDI fractional offsets, like other tunings) for specific notes; size = 128
    
    float nToneSemitoneWidth;
    int nToneRoot;              //which key matches 12-tone ET; 60 by default
    int nToneRootPC = 0;        //which pitch class; 0 by default
    int nToneRootOctave = 4;    //which octave; 4 by default, so C4
    
    // SPRING TUNING STUFF
    SpringTuning::Ptr stuning;
    
    TuningAdaptiveSystemType adaptiveType;
    
    JUCE_LEAK_DETECTOR(TuningPreparation);
};


/*
This class owns two TuningPreparations: sPrep and aPrep
As with other preparation, sPrep is the static preparation, while
aPrep is the active preparation currently in use. sPrep and aPrep
remain the same unless a Modification is triggered, which will change
aPrep but not sPrep. aPrep will be restored to sPrep when a Reset
is triggered.
*/

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
    sPrep(new TuningPreparation(prep)),
    aPrep(new TuningPreparation(sPrep)),
    Id(Id),
    name("Tuning "+String(Id))
    {
        tuningLibrary.ensureStorageAllocated((int)cTuningSystemNames.size());
        for(int i=0; i<cTuningSystemNames.size(); i++) tuningLibrary.insert(EqualTemperament, tEqualTuning);
        
        setupTuningLibrary();
        
        sPrep->getSpringTuning()->stop();
    }
    
	Tuning(int Id, bool random = false) :
    Id(Id),
    name("Tuning "+String(Id))
    {
		sPrep = new TuningPreparation();
		aPrep = new TuningPreparation(sPrep);
        
		if (random) randomize();
        
        tuningLibrary.ensureStorageAllocated((int)cTuningSystemNames.size());
        for(int i=0; i<cTuningSystemNames.size(); i++) tuningLibrary.insert(EqualTemperament, tEqualTuning);
        
        setupTuningLibrary();
        
        sPrep->getSpringTuning()->stop();
    }
    
    inline Tuning::Ptr duplicate()
    {
        TuningPreparation::Ptr copyPrep = new TuningPreparation(sPrep);
        
        Tuning::Ptr copy = new Tuning(copyPrep, -1);
        
        copy->sPrep->getSpringTuning()->stop();
        
        copy->setName(name );
        
        return copy;
    }
    
    inline void clear(void)
    {
        sPrep       = new TuningPreparation();
        aPrep       = new TuningPreparation(sPrep);
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
    
    ~Tuning() {};
    
    inline int getId() {return Id;}
    inline void setId(int newId) { Id = newId;}
    
    TuningPreparation::Ptr      sPrep;
    TuningPreparation::Ptr      aPrep;
    
    void reset()
    {
        aPrep->copy(sPrep);
        DBG("resetting tuning");
    }
    
    inline void copy(Tuning::Ptr from)
    {
        sPrep->copy(from->sPrep);
        aPrep->copy(sPrep);
    }

	inline void randomize()
	{
		clear();
		sPrep->randomize();
		aPrep->randomize();
        
		Id = Random::getSystemRandom().nextInt(Range<int>(1, 1000));
		name = "random";
	}
    
    inline TuningSystem getCurrentScaleId(void)
    {
        return aPrep->getScale();
    }
    
    inline SpringTuning::Ptr getCurrentSpringTuning(void)
    {
        return aPrep->getSpringTuning();
    }
    
    inline SpringTuning::Ptr getStaticSpringTuning(void)
    {
        return sPrep->getSpringTuning();
    }

    
    inline String getName(void) const noexcept {return name;}
    
    inline void setName(String newName)
    {
        name = newName;
    }
    
    Array<float> getCurrentScale()
    {
        if(aPrep->getScale() == CustomTuning)
        {
            return aPrep->getCustomScale();
        }
        
        return getTuningOffsets(aPrep->getScale());
    }
    
    Array<float> getStaticScale()
    {
        if(sPrep->getScale() == CustomTuning)
        {
            return sPrep->getCustomScale();
        }
        
        return getTuningOffsets(sPrep->getScale());
    }
    
    Array<float> getScale(int which)
    {
        TuningSystem tuning = (TuningSystem) which;
        
        if (tuning == CustomTuning)
        {
            return aPrep->getCustomScale();
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
    void keyPressed(int midiNoteNumber);
    void keyReleased(int midiNoteNumber);
    
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
        tuning->aPrep->copy(tuning->sPrep);
    }
    
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
    uint64  clusterTime;
    
    int     adaptiveFundamentalNote = 60; //moves with adaptive tuning
    float   adaptiveFundamentalFreq = mtof(adaptiveFundamentalNote);
    int     adaptiveHistoryCounter = 0;
    
    JUCE_LEAK_DETECTOR(TuningProcessor);
};

#endif  // TUNING_H_INCLUDED
