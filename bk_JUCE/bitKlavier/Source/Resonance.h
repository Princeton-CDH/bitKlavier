/*
  ==============================================================================

    Resonance.h
    Created: 12 May 2021 12:41:26pm
    Author:  Theodore R Trevisan

  ==============================================================================
*/

#ifndef RESONANCE_H_INCLUDED
#define RESONANCE_H_INCLUDED

#include "BKUtilities.h"
#include "BKSynthesiser.h"
#include "Tuning.h"
#include "General.h"
#include "Keymap.h"

////////////////////////////////////////////////////////////////////////////////////
/////////////////////////RESONANCE PREPARATION///////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

class ResonanceModification;

class ResonancePreparation : public ReferenceCountedObject
{

public:
    typedef ReferenceCountedObjectPtr<ResonancePreparation>   Ptr;
    typedef Array<ResonancePreparation::Ptr>                  PtrArr;
    typedef Array<ResonancePreparation::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<ResonancePreparation>                  Arr;
    typedef OwnedArray<ResonancePreparation, CriticalSection> CSArr;


    //copy constructor
    ResonancePreparation(ResonancePreparation::Ptr r) :
        name(r->name),
        rDefaultGain(r->rDefaultGain),
        rBlendronicGain(r->rBlendronicGain),
        rOvertoneDistances(r->rOvertoneDistances),
        rOvertoneGains(r->rOvertoneGains),
        rStartTimeMS(r->rStartTimeMS),
        rLengthMS(r->rLengthMS),
        rExciteThreshold(r->rExciteThreshold),
        rAttackThreshold(r->rAttackThreshold),
        rAttack(r->rAttack),
        rSustain(r->rSustain),
        rDecay(r->rDecay),
        rRelease(r->rRelease),
        rSoundSet(r->rSoundSet),
        rUseGlobalSoundSet(r->rUseGlobalSoundSet),
        rSoundSetName(r->rSoundSetName)
    {
        
    }

    //constructor with input
    ResonancePreparation(String newName, Array<int> distances, Array<float> gains, float defaultGain, float blendGain, int startTime, int length, float exciteThresh, float attackThresh) :
        name(newName),
        rDefaultGain(defaultGain, true),
        rBlendronicGain(blendGain, true),
        rOvertoneDistances(distances),
        rOvertoneGains(gains),
        rStartTimeMS(startTime),
        rLengthMS(length),
        rExciteThreshold(exciteThresh),
        rAttackThreshold(attackThresh),
        rAttack(0),
        rSustain(1.),
        rDecay(3),
        rRelease(2000),
        rUseGlobalSoundSet(true),
        rSoundSet(-1),
        rSoundSetName(String())
    {

    }

    //empty constructor, values will need to be tweaked
    ResonancePreparation(void) :
        name("test resonance preparation"),
        rOvertoneDistances(Array<int>({ 12, 19, 24, 28, 31, 34, 36, 38, 40, 42, 43, 44 })),
        rOvertoneGains(Array<float>({ 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0 })),
        rDefaultGain(0.5, true),
        rBlendronicGain(0.0, true),
        rStartTimeMS(2000),
        rLengthMS(5000),
        rExciteThreshold(0.5),
        rAttackThreshold(0.5),
        rAttack(0),
        rSustain(1.),
        rDecay(3),
        rRelease(2000),
        rUseGlobalSoundSet(true),
        rSoundSet(-1),
        rSoundSetName(String())
    {

    }

    // copy, modify, compare, randomize
    inline void copy(ResonancePreparation::Ptr r)
    {
    }

    void performModification(ResonancePreparation* r, Array<bool> dirty);

    inline bool compare(ResonancePreparation::Ptr r)
    {
        return true;
    }

    inline void randomize()
    {
        Random::getSystemRandom().setSeedRandomly();

        float r[100];

        for (int i = 0; i < 100; i++)  r[i] = (Random::getSystemRandom().nextFloat());
        int idx = 0;

        Array<float> fa;
        for (int i = 0; i < Random::getSystemRandom().nextInt(10); ++i)
        {
            fa.add(i, (Random::getSystemRandom().nextFloat() * 2.0f));
        }
        //do something, keeping this to show random iteration for when I add in randomizer later


        fa.clear();
        for (int i = 0; i < Random::getSystemRandom().nextInt(10); ++i)
        {
            fa.add(i, (Random::getSystemRandom().nextFloat() * 2.0f));
        }
        //etc etc etc
    }

    //accessors
    inline const String getName() const noexcept { return name; }
    inline const Array<int> getDistances() const noexcept { return rOvertoneDistances.value; }
    inline const Array<float> getDistancesFloat() const noexcept
    {
        Array<float> distanceFloats;
        for (int i : rOvertoneDistances.value)
        {
            distanceFloats.add(i);
        }
        return distanceFloats;
    }
    inline const Array<float> getGains() const noexcept { return rOvertoneGains.value; }
    inline const float getDefGain() const noexcept { return rDefaultGain.value; }
    inline const float getBlendGain() const noexcept { return rBlendronicGain.value; }
    inline const int getStartTime() const noexcept { return rStartTimeMS.value; }
    inline const int getLength() const noexcept { return rLengthMS.value; }
    inline const float getExciteThresh() const noexcept { return rExciteThreshold.value; }
    inline const float getAttackThresh() const noexcept { return rAttackThreshold.value; }
    inline const Array<float> getADSRvals() const noexcept
    {
        return
        {
            (float)rAttack.value, (float)rDecay.value,
            (float)rSustain.value, (float)rRelease.value

        };
    }
    inline int getSoundSet() { return rUseGlobalSoundSet.value ? -1 : rSoundSet.value; }

    //inline float* getOvertoneGainPtr(int overtoneIndex) { return &(rOvertoneGains.value[overtoneIndex]); }
    inline float* getDefaultGainPtr() { return &rDefaultGain.value; }
    inline float* getBlendGainPtr() { return &rBlendronicGain.value; }

    //mutators
    inline void setName(String n) { name = n; }
    inline void setStartTime(int startToSet) { rStartTimeMS = startToSet; }
    inline void setLength(int lengthToSet) { rLengthMS = lengthToSet; }
    inline void setDistances(Array<int> distancesToSet) { rOvertoneDistances.set(distancesToSet); }
    inline void setDistances(Array<float> distancesToSet)
    {
        rOvertoneDistances.value.clear();

        for (float i : distancesToSet)
        {
            rOvertoneDistances.value.add(int(i));
        }
    }
    inline void setGains(Array<float> gainsToSet) { rOvertoneGains.set(gainsToSet); }
    inline void setDefGain(float gainToSet) { rDefaultGain = gainToSet; }
    inline void setBlendGain(float gainToSet) { rBlendronicGain = gainToSet; }
    inline void setExciteThresh(float threshToSet) { rExciteThreshold = threshToSet; }
    inline void setAttackThresh(float threshToSet) { rAttackThreshold = threshToSet; }
    inline void setADSRvals(Array<float> vals)
    {
        rAttack = vals[0];
        rDecay = vals[1];
        rSustain = vals[2];
        rRelease = vals[3];
    }
    inline void setADSRvals(int attack, int decay, float sustain, int release)
    {
        rAttack = attack;
        rDecay = decay;
        rSustain = sustain;
        rRelease = release;
    }
    inline void setSoundSet(int Id) { rSoundSet = Id; }

    // TODO
    void print(void)
    {
    }

    ValueTree getState(void)
    {
        ValueTree prep("params");

        //TODO

        return prep;
    }

    void setState(XmlElement* e)
    {
        // TODO
    }


    // ideally the next few instance variables should be moved to private
    // overtone distances represented by distance in midi from fundamental
    // will be user changeable/possibly different per note, but hardwired for now
    // currently going up to 13th partial, not including fundamental in list, which will give us { 12, 19, 24, 28, 31, 34, 36, 38, 40, 42, 43, 44} as a default
    Moddable<Array<int>> rOvertoneDistances;

    // gains of overtones - probably all going to be hardwired to be the same initially but could vary later
    Moddable<Array<float>> rOvertoneGains;

    // start time in sample, in milliseconds, probably best to keep this unviersal for preparation?
    Moddable<int> rStartTimeMS;

    // length to play resonance note, in milliseconds, probably best to keep this unviersal for preparation?  Might change to match played note later
    Moddable<int> rLengthMS;

    //gain threshold - gain of strings has  to be below number to be excitable? for strings that are depressed
    Moddable<float> rExciteThreshold;

    //playing attack threshold - velocity (or gain?) of attacked string has to be above certain number for string to excite other strings
    Moddable<float> rAttackThreshold;

    //timer threshold to allow note to be excited?
    //something with ADSRs? will hold off on this for now

    Moddable<int> rSoundSet;
    Moddable<bool> rUseGlobalSoundSet;
    Moddable<String> rSoundSetName;

    Moddable<int> rAttack, rDecay, rRelease; //ADSR in MS
    Moddable<float> rSustain;

    Moddable<float> rDefaultGain; //shortcut until list of overtone gains is implemented
    Moddable<float> rBlendronicGain;

private:

    String name;

    JUCE_LEAK_DETECTOR(ResonancePreparation);
};

////////////////////////////////////////////////////////////////////////////////////
/////////////////////////RESONANCE///////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////


class Resonance : public ReferenceCountedObject
{

public:
    typedef ReferenceCountedObjectPtr<Resonance>   Ptr;
    typedef Array<Resonance::Ptr>                  PtrArr;
    typedef Array<Resonance::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<Resonance>                  Arr;
    typedef OwnedArray<Resonance, CriticalSection> CSArr;

    Resonance(ResonancePreparation::Ptr prep, int Id) :
        prep(new ResonancePreparation(prep)),
        Id(Id),
        name("Resonance " + String(Id))
    {

    }

    Resonance(int Id, bool random = false) :
        Id(Id),
        name("Resonance " + String(Id))
    {
        prep = new ResonancePreparation();
        if (random) randomize();
    }

    inline Resonance::Ptr duplicate()
    {
        ResonancePreparation::Ptr copyPrep = new ResonancePreparation(prep);

        Resonance::Ptr copy = new Resonance(copyPrep, -1);

        copy->setName(name);

        return copy;
    }

    inline void clear(void)
    {
        prep = new ResonancePreparation();
    }

    inline void copy(Resonance::Ptr from)
    {
        prep->copy(from->prep);
    }

    // for unit-testing
    inline void randomize()
    {
        clear();
        prep->randomize();
        Id = Random::getSystemRandom().nextInt(Range<int>(1, 1000));
        name = "random";
    }

    inline ValueTree getState(bool active = false)
    {
        ValueTree vt(vtagResonance);

        vt.setProperty("Id", Id, 0);
        vt.setProperty("name", name, 0);

        vt.addChild(prep->getState(), -1, 0);
        // prep.addChild(active ? aPrep->getState() : prep->getState(), -1, 0);

        return vt;
    }

    inline void setState(XmlElement* e)
    {
        Id = e->getStringAttribute("Id").getIntValue();

        String n = e->getStringAttribute("name");

        if (n != String())     name = n;
        else                   name = String(Id);


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

    ~Resonance() {};

    inline int getId() { return Id; }
    inline void setId(int newId) { Id = newId; }
    inline void setName(String newName) { name = newName; }
    inline String getName() const noexcept { return name; }

    ResonancePreparation::Ptr prep;

private:
    int Id;
    String name;

    JUCE_LEAK_DETECTOR(Resonance)
};


/*
ResonantNoteStuff to track stuff for array of resonant notes
It's modeled after Nostalgic (might not be as necessary because the timing should be simpler)
But probably better to set this up in case it needs to be expanded on later than to make it arbitrarily simple
*/

class ResonanceNoteStuff : public ReferenceCountedObject
{

public:
    typedef ReferenceCountedObjectPtr<ResonanceNoteStuff>   Ptr;
    typedef Array<ResonanceNoteStuff::Ptr>                  PtrArr;
    typedef Array<ResonanceNoteStuff::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<ResonanceNoteStuff>                  Arr;
    typedef OwnedArray<ResonanceNoteStuff, CriticalSection> CSArr;

    ResonanceNoteStuff(int noteNumber) : notenumber(noteNumber)
    {
        resetTimer();
    }

    ~ResonanceNoteStuff() {}

    void setNoteNumber(int newnote) { notenumber = newnote; }
    inline const int getNoteNumber() const noexcept { return notenumber; }

    void setPrepAtKeyOn(ResonancePreparation::Ptr rprep) { prepAtKeyOn = rprep; }
    ResonancePreparation::Ptr getPrepAtKeyOn() { return prepAtKeyOn; }

    void setTuningAtKeyOn(float t) { tuningAtKeyOn = t; }
    inline const float getTuningAtKeyOn() const noexcept { return tuningAtKeyOn; }

    void setVelocityAtKeyOn(float v) { velocityAtKeyOn = v; }
    inline const float getVelocityAtKeyOn() const noexcept { return velocityAtKeyOn; }

    void incrementTimer(uint64 numsamples) { timer += numsamples; }

    void resetTimer() { timer = 0; }

    void setStartPosition(uint64 sp) { startPosition = sp; }
    inline const uint64 getStartPosition() const noexcept { return startPosition; }

    void setTargetLength(uint64 tl) { targetLength = tl; }
    inline const uint64 getTargetLength() const noexcept { return targetLength; }

    bool timerExceedsTarget() { if (timer > targetLength) return true; else return false; }

    inline const uint64 getPlayPosition() { return (startPosition + timer); }

    bool isActive() { if (startPosition < timer) return false; else return true; }

private:

    int notenumber;
    ResonancePreparation::Ptr prepAtKeyOn;
    float tuningAtKeyOn;
    float velocityAtKeyOn;

    uint64 timer;

    uint64 startPosition;
    uint64 position;
    uint64 targetLength;

    JUCE_LEAK_DETECTOR(ResonanceNoteStuff);

};

////////////////////////////////////////////////////////////////////////////////////
/////////////////////////RESONANCE PROCESSOR/////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

/*
 Not sure how much stuff should go here vs resonance preparation?
*/

class ResonanceProcessor : public ReferenceCountedObject
{

public:
    typedef ReferenceCountedObjectPtr<ResonanceProcessor>   Ptr;
    typedef Array<ResonanceProcessor::Ptr>                  PtrArr;
    typedef Array<ResonanceProcessor::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<ResonanceProcessor>                  Arr;
    typedef OwnedArray<ResonanceProcessor, CriticalSection> CSArr;

    ResonanceProcessor(Resonance::Ptr rResonance,
        TuningProcessor::Ptr rTuning,
        GeneralSettings::Ptr rGeneral,
        BKSynthesiser* rMain
    );
    virtual ~ResonanceProcessor();

    //called with every audio vector
    BKSampleLoadType sampleType;

    //begin timing played note length, called with noteOn
    void keyPressed(int noteNumber, float velocity, int midiChannel, Array<KeymapTargetState> targetStates);

    //begin playing reverse note, called with noteOff
    void keyReleased(int noteNumber, float velocity, int midiChannel, Array<KeymapTargetState> targetStates, bool post = false);

    //probably not necessary but keeping just in case?
    //void postRelease(int noteNumber, int midiChannel);

    void prepareToPlay(double sr);

    //accessors
    inline Resonance::Ptr getResonance(void) const noexcept { return resonance; }
    inline TuningProcessor::Ptr getTuning(void) const noexcept { return tuning; }
    inline int getId(void) const noexcept { return resonance->getId(); }
    inline int getTuningId(void) const noexcept { return tuning->getId(); }
   
    //mutators
    inline void setResonance(Resonance::Ptr res) { resonance = res; }
    inline void setTuning(TuningProcessor::Ptr tun) { tuning = tun; }

    void processBlock(int numSamples, int midiChannel);

    inline void addKeymap(Keymap::Ptr keymap)
    {
        keymaps.add(keymap);
    }

    inline Keymap::PtrArr getKeymaps(void)
    {
        return keymaps;
    }

    inline void addBlendronic(BlendronicProcessor::Ptr blend)
    {
        blendronic.add(blend);
    }

    inline BlendronicProcessor::PtrArr getBlendronic(void) const noexcept
    {
        return blendronic;
    }

private:
    CriticalSection lock;

    void playNote(int channel, int note, float velocity);

    Resonance::Ptr         resonance;
    BKSynthesiser* synth;
    TuningProcessor::Ptr     tuning;
    GeneralSettings::Ptr    general;
    Keymap::PtrArr          keymaps;
    BlendronicProcessor::PtrArr blendronic;

    OwnedArray<ResonanceNoteStuff> resonantNotes;
    Array<uint64> noteLengthTimers;     // store current length of played notes here
    Array<float> velocities;    //record of velocities, probably not relevant

    Array<int> keysDepressed;   //current keys that are depressed
    Array<int> keysExcited; //current keys that are excited

    //not sure about this, this will track duplicate instances of a key being resonated for the sake of not accidentally removing the wrong thing with multiple shared partials
    //a better way to represent this could just be to represent keysExcited as an 88 int array of 0s, then increment/decrement when a resonant note would be added/decreased
    //depends on whether it's more efficient to have two short lists with a few more checks or one long list with possibly more iterating checks????
    Array<int> keysExcitedDupes; 

    void incrementTimers(int numsamples);

    //bool inSyncCluster, inClearCluster, inOpenCluster, inCloseCluster;
    //bool nextSyncOffIsFirst, nextClearOffIsFirst, nextOpenOffIsFirst, nextCloseOffIsFirst;

    //uint64 thresholdSamples;
    //uint64 syncThresholdTimer;
    //uint64 clearThresholdTimer;
    //uint64 openThresholdTimer;
    //uint64 closeThresholdTimer;

    JUCE_LEAK_DETECTOR(ResonanceProcessor);
};
#endif

