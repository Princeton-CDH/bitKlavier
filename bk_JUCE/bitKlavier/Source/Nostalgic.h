/*
  ==============================================================================

    Nostalgic.h
    Created: 22 Nov 2016 3:46:45pm
    Author:  Michael R Mulshine and Dan Trueman

  ==============================================================================
*/

#ifndef NOSTALGIC_H_INCLUDED
#define NOSTALGIC_H_INCLUDED

#include "BKUtilities.h"

#include "BKSynthesiser.h"

#include "Synchronic.h"
#include "Tuning.h"
#include "Keymap.h"
#include "Blendronic.h"

class NostalgicModification;

class NostalgicPreparation : public ReferenceCountedObject
{
public:
    typedef ReferenceCountedObjectPtr<NostalgicPreparation>   Ptr;
    typedef Array<NostalgicPreparation::Ptr>                  PtrArr;
    typedef Array<NostalgicPreparation::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<NostalgicPreparation>                  Arr;
    typedef OwnedArray<NostalgicPreparation, CriticalSection> CSArr;
    
    
    NostalgicPreparation(NostalgicPreparation::Ptr p)
    {
        copy(p);
    }
    
    NostalgicPreparation(int waveDistance,
                         int undertow,
                         Array<float> transposition,
                         float gain,
                         float blendGain,
                         float lengthMultiplier,
                         float beatsToSkip,
                         NostalgicSyncMode mode,
                         TuningSystem tuning,
                         PitchClass basePitch,
                         Tuning::Ptr t):
    modded(false),
    nGain(gain, true),
    nBlendronicGain(blendGain, true),
    nWaveDistance(waveDistance),
    nUndertow(undertow),
    nTransposition(transposition),
    nLengthMultiplier(lengthMultiplier),
    nBeatsToSkip(beatsToSkip),
    nMode(mode),
    nUseGlobalSoundSet(true),
    nSoundSet(-1)
    {
        holdMin = 0;
        holdMax = 12000;
        
        clusterMin = 1;
        clusterThreshold = 150;
        
        velocityMin = 0;
        velocityMax = 127;
        
        keyOnReset = false;
        
        nTranspUsesTuning = false;
    }
    
    NostalgicPreparation(void):
    modded(false),
    nGain(0.0, true),
    nBlendronicGain(0.0, true),
    nWaveDistance(0),
    nUndertow(0),
    nTransposition(Array<float>({0.0})),
    nTranspUsesTuning(false),
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
    velocityMin(0),
    velocityMax(127),
    keyOnReset(false),
    targetTypeNostalgicClear(NoteOn),
    nUseGlobalSoundSet(true),
    nSoundSet(-1)
    {

    }
    
    ~NostalgicPreparation()
    {
        
    }
    
    inline void copy(NostalgicPreparation::Ptr n)
    {
        nWaveDistance = n->nWaveDistance;
        nUndertow = n->nUndertow;
        nTransposition = n->nTransposition;
        nTranspUsesTuning = n->nTranspUsesTuning;
        nGain = n->nGain;
        nBlendronicGain = n->nBlendronicGain;
        nLengthMultiplier = n->nLengthMultiplier;
        nBeatsToSkip = n->nBeatsToSkip;
        nMode = n->nMode;
        nReverseAttack = n->nReverseAttack;
        nReverseDecay = n->nReverseDecay;
        nReverseSustain = n->nReverseSustain;
        nReverseRelease = n->nReverseRelease;
        nUndertowAttack = n->nUndertowAttack;
        nUndertowDecay = n->nUndertowDecay;
        nUndertowSustain = n->nUndertowSustain;
        nUndertowRelease = n->nUndertowRelease;
        holdMin = n->holdMin;
        holdMax = n->holdMax;
        clusterMin = n->clusterMin;
        clusterThreshold = n->clusterThreshold;
        keyOnReset = n->keyOnReset;
        velocityMin = n->velocityMin;
        velocityMax = n->velocityMax;
        targetTypeNostalgicClear = n->targetTypeNostalgicClear;
        nUseGlobalSoundSet = n->nUseGlobalSoundSet;
        nSoundSet = n->nSoundSet;
    }
    
    void performModification(NostalgicModification* n, Array<bool> dirty);
    
    void stepModdables()
    {
        nGain.step();
        nBlendronicGain.step();
        nWaveDistance.step();
        nUndertow.step();
        nTransposition.step();
        nTranspUsesTuning.step();
        nLengthMultiplier.step();
        nBeatsToSkip.step();;
        nMode.step();;
        
        nReverseAttack.step();
        nReverseDecay.step();
        nReverseRelease.step();
        nReverseSustain.step();
        
        nUndertowAttack.step();
        nUndertowDecay.step();
        nUndertowRelease.step();
        nUndertowSustain.step();
        
        holdMin.step();
        holdMax.step();
        clusterMin.step();
        clusterThreshold.step();
        velocityMin.step();
        velocityMax.step();
        
        keyOnReset.step();
        nUseGlobalSoundSet.step();
        nSoundSet.step();
        nSoundSetName.step();
    }
    
    void resetModdables()
    {
        nGain.reset();
        nBlendronicGain.reset();
        nWaveDistance.reset();
        nUndertow.reset();
        nTransposition.reset();
        nTranspUsesTuning.reset();
        nLengthMultiplier.reset();
        nBeatsToSkip.reset();;
        nMode.reset();;
        
        nReverseAttack.reset();
        nReverseDecay.reset();
        nReverseRelease.reset();
        nReverseSustain.reset();
        
        nUndertowAttack.reset();
        nUndertowDecay.reset();
        nUndertowRelease.reset();
        nUndertowSustain.reset();
        
        holdMin.reset();
        holdMax.reset();
        clusterMin.reset();
        clusterThreshold.reset();
        velocityMin.reset();
        velocityMax.reset();
        
        keyOnReset.reset();
        nUseGlobalSoundSet.reset();
        nSoundSet.reset();
        nSoundSetName.reset();
    }
    
    inline bool compare (NostalgicPreparation::Ptr n)
    {
        return (nWaveDistance == n->nWaveDistance &&
                nUndertow == n->nUndertow &&
                nTransposition == n->nTransposition &&
                nTranspUsesTuning == n->nTranspUsesTuning &&
                nGain == n->nGain &&
                nBlendronicGain == n->nBlendronicGain &&
                nLengthMultiplier == n->nLengthMultiplier &&
                nBeatsToSkip == n->nBeatsToSkip &&
                nReverseAttack == n->nReverseAttack &&
                nReverseDecay == n->nReverseDecay &&
                nReverseSustain == n->nReverseSustain &&
                nReverseRelease == n->nReverseRelease &&
                nUndertowAttack == n->nUndertowAttack &&
                nUndertowDecay == n->nUndertowDecay &&
                nUndertowSustain == n->nUndertowSustain &&
                nUndertowRelease == n->nUndertowRelease &&
                nMode == n->nMode &&
                holdMin == n->holdMin &&
                holdMax == n->holdMax &&
                clusterMin == n->clusterMin &&
                clusterThreshold == n->clusterThreshold &&
                keyOnReset == n->keyOnReset &&
                velocityMin == n->velocityMin &&
                velocityMax == n->velocityMax) &&
                targetTypeNostalgicClear == n->targetTypeNostalgicClear;
    }

	inline void randomize()
	{
		Random::getSystemRandom().setSeedRandomly();

		float r[30];

		for (int i = 0; i < 30; i++)  r[i] = (Random::getSystemRandom().nextFloat());
		int idx = 0;

		nWaveDistance = (int)(r[idx++] * 20000);
		nUndertow = (int)(r[idx++] * 20000);
        
        Array<float> dt;
        for (int i = 0; i < Random::getSystemRandom().nextInt(10); ++i)
        {
            dt.add(i, (Random::getSystemRandom().nextFloat() * 48.0f - 24.0f));
        }
        nTransposition.set(dt);

		nGain = r[idx++] * 10.0f;
        nBlendronicGain = r[idx++] * 2.0f;
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
    
    inline float* getGainPtr()                                             {return &nGain.value;       }
    inline float* getBlendronicGainPtr()                                   {return &nBlendronicGain.value;}
    
    inline const Array<float> getReverseADSRvals() const noexcept
    {
        return
        {
            (float) nReverseAttack.value, (float) nReverseDecay.value,
            (float) nReverseSustain.value, (float) nReverseRelease.value
        };
    }
    inline const Array<float> getUndertowADSRvals() const noexcept
    {
        return
        {
            (float) nUndertowAttack.value, (float) nUndertowDecay.value,
            (float) nUndertowSustain.value, (float) nUndertowRelease.value
            
        };
    }
    
    inline void setTargetTypeNostalgicClear(TargetNoteMode nm) { targetTypeNostalgicClear = nm; }
    inline void setTargetTypeNostalgic(KeymapTargetType which, TargetNoteMode nm)
    {
        if (which == TargetTypeNostalgicClear)   { targetTypeNostalgicClear = nm; }
    }
    
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
        DBG("nWaveDistance: " + String(nWaveDistance.value));
        DBG("nUndertow: " + String(nUndertow.value));
        DBG("nTransposition: " + floatArrayToString(nTransposition.value));
        DBG("nGain: " + String(nGain.value));
        DBG("nBlendronicGain: " + String(nBlendronicGain.value));
        DBG("nGain: " + String(nGain.value));
        DBG("nBlendronicGain: " + String(nBlendronicGain.value));
        DBG("nLengthMultiplier: " + String(nLengthMultiplier.value));
        DBG("nBeatsToSkip: " + String(nBeatsToSkip.value));
        DBG("nMode: " + String(nMode.value));
    }
    
    ValueTree getState(void)
    {
        ValueTree prep("params");
        
        nGain.getState(prep, ptagNostalgic_gain);
        nBlendronicGain.getState(prep, ptagNostalgic_blendronicGain);
        
        nWaveDistance.getState(prep, ptagNostalgic_waveDistance);
        nUndertow.getState(prep, ptagNostalgic_undertow);
    
        nTransposition.getState(prep, StringArray(vtagNostalgic_transposition, ptagFloat));
        nTranspUsesTuning.getState(prep, ptagNostalgic_transpUsesTuning);
        
        nLengthMultiplier.getState(prep, ptagNostalgic_lengthMultiplier);
        nBeatsToSkip.getState(prep, ptagNostalgic_beatsToSkip);
        nMode.getState(prep, ptagNostalgic_mode);
        
        holdMin.getState(prep, "holdMin");
        holdMax.getState(prep, "holdMax");
        
        clusterMin.getState(prep, "clusterMin");
        clusterThreshold.getState(prep, "clusterThreshold");
        
        velocityMin.getState(prep, "velocityMin");
        velocityMax.getState(prep, "velocityMax");
        
        keyOnReset.getState(prep, "keyOnReset");
        
        targetTypeNostalgicClear.getState(prep, ptagNostalgic_targetClearAll);
        
        ValueTree reverseADSRvals(vtagNostalgic_reverseADSR);
        int count = 0;
        nReverseAttack.getState(reverseADSRvals, ptagFloat + String(count++));
        nReverseDecay.getState(reverseADSRvals, ptagFloat + String(count++));
        nReverseSustain.getState(reverseADSRvals, ptagFloat + String(count++));
        nReverseRelease.getState(reverseADSRvals, ptagFloat + String(count));
        prep.addChild(reverseADSRvals, -1, 0);
        
        ValueTree undertowADSRvals(vtagNostalgic_undertowADSR);
        count = 0;
        nUndertowAttack.getState(undertowADSRvals, ptagFloat + String(count++));
        nUndertowDecay.getState(undertowADSRvals, ptagFloat + String(count++));
        nUndertowSustain.getState(undertowADSRvals, ptagFloat + String(count++));
        nUndertowRelease.getState(undertowADSRvals, ptagFloat + String(count));
        prep.addChild(undertowADSRvals, -1, 0);
        
        nUseGlobalSoundSet.getState(prep, ptagNostalgic_useGlobalSoundSet);
        nSoundSetName.getState(prep, ptagNostalgic_soundSet);
        
        return prep;
    }
    
    void setState(XmlElement* e)
    {
        nGain.setState(e, ptagNostalgic_gain, 1.0f);
        nBlendronicGain.setState(e, ptagNostalgic_blendronicGain, 1.0f);
        
        nWaveDistance.setState(e, ptagNostalgic_waveDistance, 0);
        nUndertow.setState(e, ptagNostalgic_undertow, 0);
        
        nTranspUsesTuning.setState(e, ptagNostalgic_transpUsesTuning, false);
        
        nLengthMultiplier.setState(e, ptagNostalgic_lengthMultiplier, 1.0f);
        nBeatsToSkip.setState(e, ptagNostalgic_beatsToSkip, 0.0);
        nMode.setState(e, ptagNostalgic_mode, NoteLengthSync);
        
        holdMin.setState(e, "holdMin", 0);
        holdMax.setState(e, "holdMax", 12000);
        
        clusterMin.setState(e, "clusterMin", 1);
        clusterThreshold.setState(e, "clusterThreshold", 150);
        
        velocityMin.setState(e, "velocityMin", 0);
        velocityMax.setState(e, "velocityMax", 127);
        
        keyOnReset.setState(e, "keyOnReset", false);
        
        targetTypeNostalgicClear.setState(e, ptagNostalgic_targetClearAll, NoteOn);
        
        nUseGlobalSoundSet.setState(e, ptagNostalgic_useGlobalSoundSet, true);
        nSoundSetName.setState(e, ptagNostalgic_soundSet, String());
        
        forEachXmlChildElement (*e, sub)
        {
            if (sub->hasTagName(vtagNostalgic_transposition))
            {
                nTransposition.setState(sub, ptagFloat, Array<float>(0.0f));
            }
            else if (sub->hasTagName(vtagNostalgic_reverseADSR))
            {
                int count = 0;
                nReverseAttack.setState(sub, ptagFloat + String(count++), 30);
                nReverseDecay.setState(sub, ptagFloat + String(count++), 3);
                nReverseSustain.setState(sub, ptagFloat + String(count++), 1.);
                nReverseRelease.setState(sub, ptagFloat + String(count), 50);
            }
            else if (sub->hasTagName(vtagNostalgic_undertowADSR))
            {
                int count = 0;
                nUndertowAttack.setState(sub, ptagFloat + String(count++), 50);
                nUndertowDecay.setState(sub, ptagFloat + String(count++), 3);
                nUndertowSustain.setState(sub, ptagFloat + String(count++), 1.);
                nUndertowRelease.setState(sub, ptagFloat + String(count), 2000);
            }
        }
    }
    
    inline void setSoundSet(int Id) { nSoundSet = Id; }
    inline int getSoundSet() { return nUseGlobalSoundSet.value ? -1 : nSoundSet.value; }

    bool modded = false;
    
    Moddable<float> nGain;                        // gain multiplier
    Moddable<float> nBlendronicGain;
    
    Moddable<int> nWaveDistance;  //ms; distance from beginning of sample to stop reverse playback and begin undertow
    Moddable<int> nUndertow;      //ms; length of time to play forward after directional change
    /*
     one thing i discovered is that the original bK actually plays the forward undertow
     sample for TWICE this value; the first half at steady gain, and then the second
     half with a ramp down. i'm not sure why, and i'm not sure i want to keep that
     behavior, but if we don't, then the instrument will sound different when we import
     old presets
     --dt
     */
    
    Moddable<Array<float>> nTransposition;        // transposition, in half steps
    Moddable<bool> nTranspUsesTuning;             // are transposition values in nTransposition filtered via attached Tuning?
    Moddable<float> nLengthMultiplier;            // note-length mode: toscale reverse playback time
    Moddable<float> nBeatsToSkip;                 // synchronic mode: beats to skip before reverse peak
    Moddable<NostalgicSyncMode> nMode;            // which sync mode to use
    
    Moddable<int>     nReverseAttack, nReverseDecay, nReverseRelease;     // reverse ADSR, in ms
    Moddable<float>   nReverseSustain;
    
    Moddable<int>     nUndertowAttack, nUndertowDecay, nUndertowRelease;  // undertow ADSR, in ms
    Moddable<float>   nUndertowSustain;
    
    Moddable<float> holdMin, holdMax;
    Moddable<int> clusterMin; // minimum number of notes needed to create nostalgic notes
    Moddable<int> clusterThreshold; // in ms; minimum time between notes to make cluster (irrelevant if clusterMin = 1)
    Moddable<int> velocityMin, velocityMax;
    
    Moddable<bool> keyOnReset;
    
    // Is this supposed to be moddable?
    Moddable<TargetNoteMode> targetTypeNostalgicClear;
    
    Moddable<bool> nUseGlobalSoundSet;
    Moddable<int> nSoundSet;
    Moddable<String> nSoundSetName;
private:
    String name;
    
    JUCE_LEAK_DETECTOR(NostalgicPreparation);
};


/*
 NostalgicNoteStuff is a class for containing a variety of information needed
 to create Nostalgic events. This is needed since the undertow note
 is created after the keys related to this note have been released, so
 we need to store that information to use when the undertow note is created.
 */

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
    prep(new NostalgicPreparation(prep)),
    name("Nostalgic "+String(Id)),
    Id(Id)
    {
    }
    
    
    Nostalgic(int Id, bool random = false):
    name("Nostalgic "+String(Id)),
    Id(Id)
    {
        prep = new NostalgicPreparation();
		if (random) randomize();
    }
    
    inline void clear(void)
    {
        prep       = new NostalgicPreparation();
    }
    
    inline Nostalgic::Ptr duplicate()
    {
        NostalgicPreparation::Ptr copyPrep = new NostalgicPreparation(prep);
        
        Nostalgic::Ptr copy = new Nostalgic(copyPrep, -1);
        
        copy->setName(name);
        
        return copy;
    }
    
    ~Nostalgic() {};
    
    inline void copy(Nostalgic::Ptr from)
    {
        prep->copy(from->prep);
    }
    
    inline ValueTree getState(bool active = false)
    {
        ValueTree vt(vtagNostalgic);
        
        vt.setProperty( "Id",Id, 0);
        vt.setProperty( "name",                          name, 0);
        
        vt.addChild(prep->getState(), -1, 0);
        
        return vt;
    }
    
    inline void setState(XmlElement* e)
    {
        Id = e->getStringAttribute("Id").getIntValue();
        
        String n = e->getStringAttribute("name");
        
        if (n != String())     name = n;
        else                        name = String(Id);
        
        
        XmlElement* params = e->getChildByName("params");
        
        if (params != nullptr)
        {
            prep->setState(params);
        }
        else
        {
            prep->setState(e);
        }
    }
    
    inline int getId() {return Id;}
    inline void setId(int newId) { Id = newId;}
    
    NostalgicPreparation::Ptr      prep;
    
    inline String getName(void) const noexcept {return name;}
    
    inline void setName(String newName)
    {
        name = newName;
    }

    // for unit-testing
	inline void randomize()
	{
		clear();
		prep->randomize();
		Id = Random::getSystemRandom().nextInt(Range<int>(1, 1000));
		name = "random";
	}
    
private:
    
    String name;
    int Id;
    
    JUCE_LEAK_DETECTOR(Nostalgic)
};


/*
 NostalgicProcessor does the main work, including processing a block
 of samples and sending it out. It connects Keymap, Tuning, Synchronic, and
 Blendronic preparations together as needed, and gets the Nostalgic
 values it needs to behave as expected
 */

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
                       BlendronicProcessor::PtrArr blend,
                       BKSynthesiser *s);
    
    virtual ~NostalgicProcessor();
    
    // called with every audio vector
    BKSampleLoadType sampleType;
    void processBlock(int numSamples, int midiChannel, BKSampleLoadType type);
    
    // begin timing played note length, called with noteOn
    void keyPressed(int midiNoteNumber, float midiNoteVelocity, int midiChannel, Array<KeymapTargetState> targetStates);
    
    // begin playing reverse note, called with noteOff
    void keyReleased(int midiNoteNumber, float midiVelocity, int midiChannel, Array<KeymapTargetState> targetStates, bool post = false);
    
    void postRelease(int midiNoteNumber, int midiChannel);
    
    // clear all sounding Nostalgic notes
    void clearAll(int midiChannel);
    
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

	inline void addBlendronic(BlendronicProcessor::Ptr blend)
	{
		blendronic.add(blend);
	}

	inline BlendronicProcessor::PtrArr getBlendronic(void) const noexcept
	{
		return blendronic;
	}

//    inline int getBlendronicId(void)
//    {
//        return blendronic->getId();
//    }
    
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
        synth = main;
    }
    
    void reset(void)
    {
        nostalgic->prep->resetModdables();
    }
    
    inline int getId(void) const noexcept { return nostalgic->getId(); }
    
    Array<int> getPlayPositions();
    Array<int> getUndertowPositions();
    
    inline int getHoldTime() const noexcept
    {
        if(activeNotes.size() == 0) return 0;
        else return 1000. * noteLengthTimers[lastKeyPlayed] / synth->getSampleRate();
    }
    
    inline float getLastVelocity() const noexcept { return lastVelocity; }
    inline int getNumActiveNotes() const noexcept { return activeNotes.size(); }
    inline int getNumReverseNotes() const noexcept { return reverseNotes.size(); }
    inline int getCurrentClusterSize() const noexcept { return currentClusterSize; }
    inline int getClusterThresholdTimer() const noexcept { return clusterThresholdTimer * 1000. / synth->getSampleRate(); }
    
    inline void addKeymap(Keymap::Ptr keymap)
    {
        keymaps.add(keymap);
    }
    
    inline Keymap::PtrArr getKeymaps(void)
    {
        return keymaps;
    }
    
private:
    BKSynthesiser*              synth;
    
    Nostalgic::Ptr                  nostalgic;
    TuningProcessor::Ptr            tuner;
    SynchronicProcessor::Ptr        synchronic;
	BlendronicProcessor::PtrArr		blendronic;
    
    Keymap::PtrArr              keymaps;
    
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
    
    //move timers forward by blocksize
    void incrementTimers(int numSamples);
    
    JUCE_LEAK_DETECTOR (NostalgicProcessor) //is this the right one to use here?
};

#endif  // NOSTALGIC_H_INCLUDED
