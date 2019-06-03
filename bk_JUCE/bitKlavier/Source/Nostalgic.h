/*
  ==============================================================================

    Nostalgic.h
    Created: 22 Nov 2016 3:46:45pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef NOSTALGIC_H_INCLUDED
#define NOSTALGIC_H_INCLUDED

#include "BKUtilities.h"

#include "BKSynthesiser.h"

#include "Synchronic.h"
#include "Tuning.h"
#include "Keymap.h"

class NostalgicPreparation : public ReferenceCountedObject
{
public:
    typedef ReferenceCountedObjectPtr<NostalgicPreparation>   Ptr;
    typedef Array<NostalgicPreparation::Ptr>                  PtrArr;
    typedef Array<NostalgicPreparation::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<NostalgicPreparation>                  Arr;
    typedef OwnedArray<NostalgicPreparation, CriticalSection> CSArr;
    
    
    NostalgicPreparation(NostalgicPreparation::Ptr p):
    nWaveDistance(p->getWavedistance()),
    nUndertow(p->getUndertow()),
    nTransposition(p->getTransposition()),
    nGain(p->getGain()),
    nLengthMultiplier(p->getLengthMultiplier()),
    nBeatsToSkip(p->getBeatsToSkip()),
    nMode(p->getMode()),
    nReverseAttack(p->getReverseAttack()),
    nReverseDecay(p->getReverseDecay()),
    nReverseRelease(p->getReverseRelease()),
    nReverseSustain(p->getReverseSustain()),
    nUndertowAttack(p->getUndertowAttack()),
    nUndertowDecay(p->getUndertowDecay()),
    nUndertowRelease(p->getUndertowRelease()),
    nUndertowSustain(p->getUndertowSustain()),
    holdMin(p->getHoldMin()),
    holdMax(p->getHoldMax()),
    clusterMin(p->getClusterMin()),
    clusterThreshold(p->getClusterThreshold()),
    keyOnReset(p->getKeyOnReset()),
    velocityMin(p->getVelocityMin()),
    velocityMax(p->getVelocityMax())
    {
        
    }
    
    NostalgicPreparation(int waveDistance,
                         int undertow,
                         Array<float> transposition,
                         float gain,
                         float lengthMultiplier,
                         float beatsToSkip,
                         NostalgicSyncMode mode,
                         TuningSystem tuning,
                         PitchClass basePitch,
                         Tuning::Ptr t):
    nWaveDistance(waveDistance),
    nUndertow(undertow),
    nTransposition(transposition),
    nGain(gain),
    nLengthMultiplier(lengthMultiplier),
    nBeatsToSkip(beatsToSkip),
    nMode(mode)
    {
        holdMin = 0;
        holdMax = 12000;
        
        clusterMin = 1;
        clusterThreshold = 150;
        
        velocityMin = 0;
        velocityMax = 127;
        
        keyOnReset = false;
    }
    
    NostalgicPreparation(void):
    nWaveDistance(0),
    nUndertow(0),
    nTransposition(Array<float>({0.0})),
    nGain(1.0),
    nLengthMultiplier(1.0),
    nBeatsToSkip(0.0),
    nMode(NoteLengthSync),
    nReverseAttack(30),
    nReverseDecay(3),
    nReverseRelease(50),
    nReverseSustain(1.),
    nUndertowAttack(50),
    nUndertowDecay(3),
    nUndertowRelease(2000),
    nUndertowSustain(1.),
    holdMin(0),
    holdMax(12000),
    clusterMin(1),
    clusterThreshold(150),
    keyOnReset(false),
    velocityMin(0),
    velocityMax(127)
    {

    }
    
    ~NostalgicPreparation()
    {
        
    }
    
    
    
    inline void copy(NostalgicPreparation::Ptr n)
    {
        nWaveDistance = n->getWavedistance();
        nUndertow = n->getUndertow();
        nTransposition = n->getTransposition();
        nGain = n->getGain();
        nLengthMultiplier = n->getLengthMultiplier();
        nBeatsToSkip = n->getBeatsToSkip();
        nMode = n->getMode();
        nReverseAttack = n->getReverseAttack();
        nReverseDecay = n->getReverseDecay();
        nReverseSustain = n->getReverseSustain();
        nReverseRelease = n->getReverseRelease();
        nUndertowAttack = n->getUndertowAttack();
        nUndertowDecay = n->getUndertowDecay();
        nUndertowSustain = n->getUndertowSustain();
        nUndertowRelease = n->getUndertowRelease();
        holdMin = n->getHoldMin();
        holdMax = n->getHoldMax();
        clusterMin = n->getClusterMin();
        clusterThreshold = n->getClusterThreshold();
        keyOnReset = n->getKeyOnReset();
        velocityMin = n->getVelocityMin();
        velocityMax = n->getVelocityMax();
    }
    
    inline void performModification(NostalgicPreparation::Ptr n, Array<bool> dirty)
    {
        if (dirty[NostalgicWaveDistance]) nWaveDistance = n->getWavedistance();
        if (dirty[NostalgicUndertow]) nUndertow = n->getUndertow();
        if (dirty[NostalgicTransposition]) nTransposition = n->getTransposition();
        if (dirty[NostalgicGain]) nGain = n->getGain();
        if (dirty[NostalgicLengthMultiplier]) nLengthMultiplier = n->getLengthMultiplier();
        if (dirty[NostalgicBeatsToSkip]) nBeatsToSkip = n->getBeatsToSkip();
        if (dirty[NostalgicMode]) nMode = n->getMode();
        if (dirty[NostalgicReverseADSR])
        {
            nReverseAttack = n->getReverseAttack();
            nReverseDecay = n->getReverseDecay();
            nReverseSustain = n->getReverseSustain();
            nReverseRelease = n->getReverseRelease();
        }
        
        if (dirty[NostalgicUndertowADSR])
        {
            nUndertowAttack = n->getUndertowAttack();
            nUndertowDecay = n->getUndertowDecay();
            nUndertowSustain = n->getUndertowSustain();
            nUndertowRelease = n->getUndertowRelease();
        }
        
        if (dirty[NostalgicHoldMin]) holdMin = n->getHoldMin();
        if (dirty[NostalgicHoldMax]) holdMax = n->getHoldMax();
        if (dirty[NostalgicClusterMin]) clusterMin = n->getClusterMin();
        if (dirty[NostalgicClusterThreshold]) clusterThreshold = n->getClusterThreshold();
        if (dirty[NostalgicKeyOnReset]) keyOnReset = n->getKeyOnReset();
        if (dirty[NostalgicVelocityMin]) velocityMin = n->getVelocityMin();
        if (dirty[NostalgicVelocityMax]) velocityMax = n->getVelocityMax();
    }
    
    inline bool compare (NostalgicPreparation::Ptr n)
    {
        return (nWaveDistance == n->getWavedistance() &&
                nUndertow == n->getUndertow() &&
                nTransposition == n->getTransposition() &&
                nGain == n->getGain() &&
                nLengthMultiplier == n->getLengthMultiplier() &&
                nBeatsToSkip == n->getBeatsToSkip() &&
                nReverseAttack == n->getReverseAttack() &&
                nReverseDecay == n->getReverseDecay() &&
                nReverseSustain == n->getReverseSustain() &&
                nReverseRelease == n->getReverseRelease() &&
                nUndertowAttack == n->getUndertowAttack() &&
                nUndertowDecay == n->getUndertowDecay() &&
                nUndertowSustain == n->getUndertowSustain() &&
                nUndertowRelease == n->getUndertowRelease() &&
                nMode == n->getMode() &&
                holdMin == n->getHoldMin() &&
                holdMax == n->getHoldMax() &&
                clusterMin == n->getClusterMin() &&
                clusterThreshold == n->getClusterThreshold() &&
                keyOnReset == n->getKeyOnReset() &&
                velocityMin == n->getVelocityMin() &&
                velocityMax == n->getVelocityMax());
    }

	inline void randomize()
	{
		Random::getSystemRandom().setSeedRandomly();

		float r[30];

		for (int i = 0; i < 30; i++)  r[i] = (Random::getSystemRandom().nextFloat());
		int idx = 0;

		nWaveDistance = (int)(r[idx++] * 20000);
		nUndertow = (int)(r[idx++] * 20000);
		nTransposition.clear();
		for (int i = 0; i < Random::getSystemRandom().nextInt(10); ++i)
		{
			nTransposition.add(i, (Random::getSystemRandom().nextFloat()) * 48.0f - 24.0f);
		}
		nGain = r[idx++] * 10.0f;
		nLengthMultiplier = r[idx++] * 10.0f;
		nBeatsToSkip = r[idx++] * 10.0f;
		nMode = (NostalgicSyncMode)(int)(r[idx++] * NostalgicSyncModeNil);
		nReverseAttack = (int)(r[idx++] * 1000) + 1;
		nReverseDecay = (int)(r[idx++] * 1000) + 1;
		nReverseSustain = r[idx++];
		nReverseRelease = (int)(r[idx++] * 1000) + 1;
		nUndertowAttack = (int)(r[idx++] * 1000) + 1;
		nUndertowDecay = (int)(r[idx++] * 1000) + 1;
		nUndertowSustain = r[idx++];
		nUndertowRelease = (int)(r[idx++] * 2000) + 1;
        holdMin = (float)(r[idx++] * 12000.);
        holdMax = (float)(r[idx++] * 12000.);
        clusterMin = (int)(r[idx++] * 12) + 1;
        keyOnReset = (r[idx++] < 0.5) ? true : false;
        velocityMin = (int)(r[idx++] * 127) + 1;
        velocityMax = (int)(r[idx++] * 127) + 1;
	}
    
    inline const String getName() const noexcept {return name;}
    inline void setName(String n){name = n;}
    
    inline const int getWavedistance() const noexcept                      {return nWaveDistance;      }
    inline const int getUndertow() const noexcept                          {return nUndertow;          }
    inline const Array<float> getTransposition() const noexcept            {return nTransposition;     }
    inline const float getGain() const noexcept                            {return nGain;              }
    inline const float getLengthMultiplier() const noexcept                {return nLengthMultiplier;  }
    inline const float getBeatsToSkip() const noexcept                     {return nBeatsToSkip;       }
    inline const NostalgicSyncMode getMode() const noexcept                {return nMode;              }
    
    inline const int getReverseAttack() const noexcept                     {return nReverseAttack;        }
    inline const int getReverseDecay() const noexcept                      {return nReverseDecay;         }
    inline const float getReverseSustain() const noexcept                  {return nReverseSustain;       }
    inline const int getReverseRelease() const noexcept                    {return nReverseRelease;       }
    inline const Array<float> getReverseADSRvals() const noexcept         {return { (float) nReverseAttack, (float)nReverseDecay,(float)nReverseSustain, (float)nReverseRelease}; }
    
    
    inline const int getUndertowAttack() const noexcept                    {return nUndertowAttack;        }
    inline const int getUndertowDecay() const noexcept                     {return nUndertowDecay;         }
    inline const float getUndertowSustain() const noexcept                 {return nUndertowSustain;       }
    inline const int getUndertowRelease() const noexcept                   {return nUndertowRelease;       }
    inline const Array<float> getUndertowADSRvals() const noexcept         {return { (float)nUndertowAttack, (float)nUndertowDecay,(float)nUndertowSustain,(float)nUndertowRelease}; }
    
    inline void setWaveDistance(int waveDistance)                          {nWaveDistance = waveDistance;          }
    inline void setUndertow(int undertow)                                  {nUndertow = undertow;                  }
    inline void setTransposition(Array<float> transposition)               {nTransposition = transposition;        }
    inline void setGain(float gain)                                        {nGain = gain;                          }
    inline void setLengthMultiplier(float lengthMultiplier)                {nLengthMultiplier = lengthMultiplier;  }
    inline void setBeatsToSkip(float beatsToSkip)                          {nBeatsToSkip = beatsToSkip;            }
    inline void setMode(NostalgicSyncMode mode)                            {nMode = mode;                          }
    
    inline void setReverseAttack(int val)                                  {nReverseAttack = val;         }
    inline void setReverseDecay(int val)                                   {nReverseDecay = val;          }
    inline void setReverseSustain(float val)                               {nReverseSustain = val;        }
    inline void setReverseRelease(int val)                                 {nReverseRelease = val;        }
    
    inline void setUndertowAttack(int val)                                 {nUndertowAttack = val;         }
    inline void setUndertowDecay(int val)                                  {nUndertowDecay = val;          }
    inline void setUndertowSustain(float val)                              {nUndertowSustain = val;        }
    inline void setUndertowRelease(int val)                                {nUndertowRelease = val;        }
    
    inline const float getHoldMin() const noexcept { return holdMin; }
    inline const float getHoldMax() const noexcept { return holdMax; }
    
    inline const void setHoldMin(float min)  { holdMin = min; }
    inline const void setHoldMax(float max)  { holdMax = max; }
    
    inline const int getVelocityMin() const noexcept { return velocityMin; }
    inline const int getVelocityMax() const noexcept { return velocityMax; }
    
    inline const void setVelocityMin(int min)  { velocityMin = min; }
    inline const void setVelocityMax(int max)  { velocityMax = max; }
    
    inline const int getClusterMin() const noexcept { return clusterMin; }
    inline const int getClusterThreshold() const noexcept { return clusterThreshold; }
    
    inline const void setClusterMin(int min)  { clusterMin = min; }
    inline const void setClusterThreshold(int min)  { clusterThreshold = min; }
    
    inline const bool getKeyOnReset() const noexcept { return keyOnReset; }
    
    inline const void setKeyOnReset(bool reset)  { keyOnReset = reset; }
    
    inline void setReverseADSRvals(Array<float> vals)
    {
        nReverseAttack = vals[0];
        nReverseDecay = vals[1];
        nReverseSustain = vals[2];
        nReverseRelease = vals[3];
    }
    
    inline void setUndertowADSRvals(Array<float> vals)
    {
        nUndertowAttack = vals[0];
        nUndertowDecay = vals[1];
        nUndertowSustain = vals[2];
        nUndertowRelease = vals[3];
    }

    void print(void)
    {
        DBG("nWaveDistance: " + String(nWaveDistance));
        DBG("nUndertow: " + String(nUndertow));
        DBG("nTransposition: " + floatArrayToString(nTransposition));
        DBG("nGain: " + String(nGain));
        DBG("nLengthMultiplier: " + String(nLengthMultiplier));
        DBG("nBeatsToSkip: " + String(nBeatsToSkip));
        DBG("nMode: " + String(nMode));
    }
    
    ValueTree getState(void)
    {
        ValueTree prep("params");
        
        prep.setProperty( ptagNostalgic_waveDistance,       getWavedistance(), 0);
        prep.setProperty( ptagNostalgic_undertow,           getUndertow(), 0);
        
        ValueTree transp( vtagNostalgic_transposition);
        int count = 0;
        for (auto f : getTransposition())
        {
            transp.      setProperty( ptagFloat + String(count++), f, 0);
        }
        prep.addChild(transp, -1, 0);
        
        prep.setProperty( ptagNostalgic_gain,               getGain(), 0);
        prep.setProperty( ptagNostalgic_lengthMultiplier,   getLengthMultiplier(), 0);
        prep.setProperty( ptagNostalgic_beatsToSkip,        getBeatsToSkip(), 0);
        prep.setProperty( ptagNostalgic_mode,               getMode(), 0);
        
        prep.setProperty( "holdMin", getHoldMin(), 0);
        prep.setProperty( "holdMax", getHoldMax(), 0);
        
        prep.setProperty( "clusterMin", getClusterMin(), 0);
        prep.setProperty( "clusterThreshold", getClusterThreshold(), 0);
        
        prep.setProperty( "velocityMin", getVelocityMin(), 0);
        prep.setProperty( "velocityMax", getVelocityMax(), 0);
        
        prep.setProperty( "keyOnReset", getKeyOnReset() ? 1 : 0, 0);
        
        ValueTree reverseADSRvals( vtagNostalgic_reverseADSR);
        count = 0;
        for (auto f : getReverseADSRvals())
        {
            reverseADSRvals.setProperty( ptagFloat + String(count++), f, 0);
        }
        prep.addChild(reverseADSRvals, -1, 0);
        
        ValueTree undertowADSRvals( vtagNostalgic_undertowADSR);
        count = 0;
        for (auto f : getUndertowADSRvals())
        {
            undertowADSRvals.setProperty( ptagFloat + String(count++), f, 0);
        }
        prep.addChild(undertowADSRvals, -1, 0);
        
        return prep;
    }
    
    void setState(XmlElement* e)
    {
        int i; float f;
        
        i = e->getStringAttribute(ptagNostalgic_waveDistance).getIntValue();
        setWaveDistance(i);
        
        i = e->getStringAttribute(ptagNostalgic_undertow).getIntValue();
        setUndertow(i);
        
        forEachXmlChildElement (*e, sub)
        {
            if (sub->hasTagName(vtagNostalgic_transposition))
            {
                Array<float> transp;
                for (int k = 0; k < 128; k++)
                {
                    String attr = sub->getStringAttribute(ptagFloat + String(k));
                    
                    if (attr == String::empty) break;
                    else
                    {
                        f = attr.getFloatValue();
                        transp.add(f);
                    }
                }
                
                setTransposition(transp);
                
            }
            else  if (sub->hasTagName(vtagNostalgic_reverseADSR))
            {
                Array<float> envVals;
                for (int k = 0; k < 4; k++)
                {
                    String attr = sub->getStringAttribute(ptagFloat + String(k));
                    
                    if (attr == String::empty) break;
                    else
                    {
                        f = attr.getFloatValue();
                        envVals.add(f);
                    }
                }
                
                setReverseADSRvals(envVals);
                
            }
            else  if (sub->hasTagName(vtagNostalgic_undertowADSR))
            {
                Array<float> envVals;
                for (int k = 0; k < 4; k++)
                {
                    String attr = sub->getStringAttribute(ptagFloat + String(k));
                    
                    if (attr == String::empty) break;
                    else
                    {
                        f = attr.getFloatValue();
                        envVals.add(f);
                    }
                }
                
                setUndertowADSRvals(envVals);
                
            }
        }
        
        
        f = e->getStringAttribute(ptagNostalgic_lengthMultiplier).getFloatValue();
        setLengthMultiplier(f);
        
        f = e->getStringAttribute(ptagNostalgic_beatsToSkip).getFloatValue();
        setBeatsToSkip(f);
        
        f = e->getStringAttribute(ptagNostalgic_gain).getFloatValue();
        setGain(f);
        
        i = e->getStringAttribute(ptagNostalgic_mode).getIntValue();
        setMode((NostalgicSyncMode)i);
        
        
        // HOLD MIN / MAX
        String str = e->getStringAttribute("holdMin");
        
        if (str != "")
        {
            f = str.getFloatValue();
            setHoldMin(f);
        }
        else
        {
            setHoldMin(0);
        }
        
        str = e->getStringAttribute("holdMax");
        
        if (str != "")
        {
            f = str.getFloatValue();
            setHoldMax(f);
        }
        else
        {
            setHoldMax(12000);
        }
        
        
        // CLUSTER MIN
        str = e->getStringAttribute("clusterMin");
        
        if (str != "")
        {
            i = str.getIntValue();
            setClusterMin(i);
        }
        else
        {
            setClusterMin(1);
        }
        
        // CLUSTER THRESHOLD
        str = e->getStringAttribute("clusterThreshold");
        
        if (str != "")
        {
            i = str.getIntValue();
            setClusterThreshold(i);
        }
        else
        {
            setClusterThreshold(150);
        }
        
        // VELOCITY MIN
        str = e->getStringAttribute("velocityMin");
        
        if (str != "")
        {
            i = atoi(str.getCharPointer());
            setVelocityMin(i);
        }
        else
        {
            setVelocityMin(0);
        }
        
        // VELOCITY MAX
        str = e->getStringAttribute("velocityMax");
        
        if (str != "")
        {
            i = atoi(str.getCharPointer());
            setVelocityMax(i);
        }
        else
        {
            setVelocityMax(127);
        }
        
        str = e->getStringAttribute("keyOnReset");
        
        if (str != "")
        {
            i = str.getIntValue();
            setKeyOnReset((bool) i);
        }
        else
        {
            setKeyOnReset(false);
        }
        
    }
    
    
private:
    String name;
    int nWaveDistance;  //ms; distance from beginning of sample to stop reverse playback and begin undertow
    int nUndertow;      //ms; length of time to play forward after directional change
    /*
     one thing i discovered is that the original bK actually plays the forward undertow
     sample for TWICE this value; the first half at steady gain, and then the second
     half with a ramp down. i'm not sure why, and i'm not sure i want to keep that
     behavior, but if we don't, then the instrument will sound different when we import
     old presets
     --dt
     */
    
    Array<float> nTransposition;       //transposition, in half steps
    float nGain;                //gain multiplier
    float nLengthMultiplier;    //note-length mode: toscale reverse playback time
    float nBeatsToSkip;         //synchronic mode: beats to skip before reverse peak
    NostalgicSyncMode nMode;    //which sync mode to use
    
    int     nReverseAttack, nReverseDecay, nReverseRelease;         //reverse ADSR, in ms
    float   nReverseSustain;
    
    int     nUndertowAttack, nUndertowDecay, nUndertowRelease;      //undertow ADSR, in ms
    float   nUndertowSustain;
    
    float holdMin, holdMax;
    int clusterMin;
    int clusterThreshold; //in ms
    int velocityMin, velocityMax;
    
    bool keyOnReset;
    
    //internal keymap for resetting internal values to static
    //Keymap::Ptr resetMap = new Keymap(0);
    
    JUCE_LEAK_DETECTOR(NostalgicPreparation);
};


class NostalgicNoteStuff : public ReferenceCountedObject
{
public:
    
    typedef ReferenceCountedObjectPtr<NostalgicNoteStuff>   Ptr;
    typedef Array<NostalgicNoteStuff::Ptr>                  PtrArr;
    typedef Array<NostalgicNoteStuff::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<NostalgicNoteStuff>                  Arr;
    typedef OwnedArray<NostalgicNoteStuff, CriticalSection> CSArr;
    
    NostalgicNoteStuff(int noteNumber) : notenumber(noteNumber)
    {
        resetReverseTimer();
        resetUndertowTimer();
    }
    
    ~NostalgicNoteStuff() {}
    
    void setNoteNumber(int newnote)                         { notenumber = newnote; }
    inline const int getNoteNumber() const noexcept         { return notenumber; }
    
    void setPrepAtKeyOn(NostalgicPreparation::Ptr nprep)    { prepAtKeyOn = nprep; }
    NostalgicPreparation::Ptr getPrepAtKeyOn()              { return prepAtKeyOn; }
    
    void setTuningAtKeyOn(float t)                          { tuningAtKeyOn = t; }
    inline const float getTuningAtKeyOn() const noexcept    { return tuningAtKeyOn; }
    
    void setVelocityAtKeyOn(float v)                        { velocityAtKeyOn = v; }
    inline const float getVelocityAtKeyOn() const noexcept  { return velocityAtKeyOn; }
    
    void incrementReverseTimer(uint64 numsamples)           { reverseTimer += numsamples; }
    void incrementUndertowTimer(uint64 numsamples)          { undertowTimer += numsamples; }
    
    void resetReverseTimer()                                { reverseTimer = 0; }
    void resetUndertowTimer()                               { undertowTimer = 0; }
    
    void setReverseStartPosition(uint64 rsp)                        { reverseStartPosition = rsp; }
    inline const uint64 getReverseStartPosition() const noexcept    { return reverseStartPosition; }
    
    void setUndertowStartPosition(uint64 usp)                        { undertowStartPosition = usp; }
    inline const uint64 getUndertowStartPosition() const noexcept    { return undertowStartPosition; }
    
    void setReverseTargetLength(uint64 rtl)                         { reverseTargetLength = rtl; }
    void setUndertowTargetLength(uint64 utl)                        { undertowTargetLength = utl; }
    inline const uint64 getUndertowTargetLength() const noexcept    { return undertowTargetLength; }
    
    bool reverseTimerExceedsTarget()    { if(reverseTimer > reverseTargetLength) return true; else return false; }
    bool undertowTimerExceedsTarget()   { if(undertowTimer > undertowTargetLength) return true; else return false; }
    
    inline const uint64 getReversePlayPosition()     { return (reverseStartPosition - reverseTimer); }
    inline const uint64 getUndertowPlayPosition()    { return (undertowStartPosition + undertowTimer); }
    
    bool isActive() { if(reverseStartPosition < reverseTimer) return false; else return true; }
    
private:
    
    int notenumber;
    NostalgicPreparation::Ptr prepAtKeyOn;
    float tuningAtKeyOn;
    float velocityAtKeyOn;
    
    uint64 reverseTimer;
    uint64 undertowTimer;
    
    uint64 reverseStartPosition;
    uint64 reversePosition;
    
    uint64 undertowStartPosition;
    uint64 undertowPosition;
    
    uint64 reverseTargetLength;
    uint64 undertowTargetLength;
    
    JUCE_LEAK_DETECTOR(NostalgicNoteStuff);
};


/* ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ NOSTALGIC ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ */

class Nostalgic : public ReferenceCountedObject
{
    
public:
    typedef ReferenceCountedObjectPtr<Nostalgic>   Ptr;
    typedef Array<Nostalgic::Ptr>                  PtrArr;
    typedef Array<Nostalgic::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<Nostalgic>                  Arr;
    typedef OwnedArray<Nostalgic, CriticalSection> CSArr;
    
    
    Nostalgic(NostalgicPreparation::Ptr prep,
              int Id):
    sPrep(new NostalgicPreparation(prep)),
    aPrep(new NostalgicPreparation(sPrep)),
    Id(Id),
    name(String(Id))
    {
    }
    
    
    Nostalgic(int Id, bool random = false):
    name("Nostalgic"+String(Id)),
    Id(Id)
    {
		sPrep = new NostalgicPreparation();
		aPrep = new NostalgicPreparation(sPrep);
		if (random) randomize();
    }
    
    inline void clear(void)
    {
        sPrep       = new NostalgicPreparation();
        aPrep       = new NostalgicPreparation(sPrep);
    }
    
    inline Nostalgic::Ptr duplicate()
    {
        NostalgicPreparation::Ptr copyPrep = new NostalgicPreparation(sPrep);
        
        Nostalgic::Ptr copy = new Nostalgic(copyPrep, -1);
        
        copy->setName(name);
        
        return copy;
    }
    
    ~Nostalgic() {};
    
    inline void copy(Nostalgic::Ptr from)
    {
        sPrep->copy(from->sPrep);
        aPrep->copy(sPrep);
    }
    
    inline ValueTree getState(bool active = false)
    {
        ValueTree prep(vtagNostalgic);
        
        prep.setProperty( "Id",Id, 0);
        prep.setProperty( "name",                          name, 0);
        
        prep.addChild(active ? aPrep->getState() : sPrep->getState(), -1, 0);
        
        return prep;
    }
    
    inline void setState(XmlElement* e)
    {
        Id = e->getStringAttribute("Id").getIntValue();
        
        String n = e->getStringAttribute("name");
        
        if (n != String::empty)     name = n;
        else                        name = String(Id);
        
        
        XmlElement* params = e->getChildByName("params");
        
        if (params != nullptr)
        {
            sPrep->setState(params);
        }
        else
        {
            sPrep->setState(e);
        }
        
        aPrep->copy(sPrep);
    }
    
    inline int getId() {return Id;}
    inline void setId(int newId) { Id = newId;}
    
    NostalgicPreparation::Ptr      sPrep;
    NostalgicPreparation::Ptr      aPrep;
    
    
    inline String getName(void) const noexcept {return name;}
    
    inline void setName(String newName)
    {
        name = newName;
    }

	inline void randomize()
	{
		clear();
		sPrep->randomize();
		aPrep->randomize();
		Id = Random::getSystemRandom().nextInt(Range<int>(1, 1000));
		name = "random";
	}
    
private:
    
    String name;
    int Id;
    
    JUCE_LEAK_DETECTOR(Nostalgic)
};

class NostalgicProcessor : public ReferenceCountedObject
{
    
public:
    typedef ReferenceCountedObjectPtr<NostalgicProcessor>   Ptr;
    typedef Array<NostalgicProcessor::Ptr>                  PtrArr;
    typedef Array<NostalgicProcessor::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<NostalgicProcessor>                  Arr;
    typedef OwnedArray<NostalgicProcessor, CriticalSection> CSArr;
    
    NostalgicProcessor(Nostalgic::Ptr nostalgic,
                       TuningProcessor::Ptr tuning,
                       SynchronicProcessor::Ptr synchronic,
                       BKSynthesiser *s);
    
    virtual ~NostalgicProcessor();
    
    //called with every audio vector
    BKSampleLoadType sampleType;
    void processBlock(int numSamples, int midiChannel, BKSampleLoadType type);
    
    //begin timing played note length, called with noteOn
    void keyPressed(int midiNoteNumber, float midiNoteVelocity, int midiChannel);
    
    //begin playing reverse note, called with noteOff
    void keyReleased(int midiNoteNumber, float midiVelocity, int midiChannel, bool post = false);
    
    void postRelease(int midiNoteNumber, int midiChannel);
    
    inline void attachToSynthesiser(BKSynthesiser* main)
    {
        synth = main;
    }
    
    inline void setNostalgic(Nostalgic::Ptr nost)
    {
        nostalgic = nost;
    }
    
    inline Nostalgic::Ptr getNostalgic(void) const noexcept
    {
        return nostalgic;
    }
    
    inline void setSynchronic(SynchronicProcessor::Ptr sync)
    {
        synchronic = sync;
    }
    
    inline SynchronicProcessor::Ptr getSynchronic(void) const noexcept
    {
        return synchronic;
    }
    
    inline int getSynchronicId(void)
    {
        return synchronic->getId();
    }
    
    inline void setTuning(TuningProcessor::Ptr p)
    {
        tuner = p;
    }
    
    inline TuningProcessor::Ptr getTuning(void)
    {
        return tuner;
    }
    
    void prepareToPlay(double sr, BKSynthesiser* main)
    {
        sampleRate = sr;
        synth = main;
    }
    
    inline void reset(void)
    {
        nostalgic->aPrep->copy(nostalgic->sPrep);
    }
    
    inline int getId(void) const noexcept { return nostalgic->getId(); }
    
    Array<int> getPlayPositions();
    Array<int> getUndertowPositions();
    
    inline int getHoldTime() const noexcept
    {
        if(activeNotes.size() == 0) return 0;
        else return 1000. * noteLengthTimers[lastKeyPlayed] / sampleRate;
    }
    
    inline float getLastVelocity() const noexcept { return lastVelocity; }
    inline int getNumActiveNotes() const noexcept {return activeNotes.size(); }
    inline int getCurrentClusterSize() const noexcept {return currentClusterSize;}
    inline int getClusterThresholdTimer() const noexcept {return clusterThresholdTimer * 1000. / sampleRate;}
    
private:
    BKSynthesiser*              synth;
    
    Nostalgic::Ptr                  nostalgic;
    TuningProcessor::Ptr            tuner;
    SynchronicProcessor::Ptr        synchronic;
    
    Array<uint64> noteLengthTimers;     // store current length of played notes here
    Array<int> activeNotes;             // table of notes currently being played by player
    Array<bool> noteOn;                 // table of booleans representing state of each note
    Array<float> velocities;            // table of velocities played
    
    uint64 lastHoldTime;
    int lastKeyPlayed;
    float lastVelocity;
    
    // CLUSTER STUFF
    bool inCluster;
    Array<int> cluster;
    int currentClusterSize;
    bool playCluster;
    uint64 clusterThresholdTimer;

    Array<int> clusterNotesPlayed;
    
    OwnedArray<NostalgicNoteStuff> reverseNotes;
    OwnedArray<NostalgicNoteStuff> undertowNotes;
    
    double sampleRate;
    
    //move timers forward by blocksize
    void incrementTimers(int numSamples);
    
    JUCE_LEAK_DETECTOR (NostalgicProcessor) //is this the right one to use here?
};

#endif  // NOSTALGIC_H_INCLUDED
