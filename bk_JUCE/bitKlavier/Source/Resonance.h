/*
  ==============================================================================

    Resonance.h
    Created: 12 May 2021 12:41:26pm
    Author:  Dan Trueman and Theodore R Trevisan

  ==============================================================================
*/

#ifndef RESONANCE_H_INCLUDED
#define RESONANCE_H_INCLUDED

#include "BKUtilities.h"
#include "BKSynthesiser.h"
#include "Tuning.h"
#include "General.h"
#include "Keymap.h"
#include "Blendronic.h"

////////////////////////////////////////////////////////////////////////////////////
/////////////////////////RESONANCE PREPARATION///////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

class ResonanceModification;
/*
 SympPartial stores information about an individual partial of an undamped string
*/
class SympPartial : public ReferenceCountedObject
{
public:
    typedef ReferenceCountedObjectPtr<SympPartial>   Ptr;
    typedef Array<SympPartial::Ptr>                  PtrArr;
    typedef Array<SympPartial::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<SympPartial>                  Arr;
    typedef OwnedArray<SympPartial, CriticalSection> CSArr;
    typedef HashMap<int, SympPartial::Ptr>           PtrMap; // not sure if this works

    SympPartial(int newHeldKey, int newPartialKey, float newGain, float newOffset);

    int heldKey;                // midiNoteNumber for key that is held down; for the undamped string that has this partial
    int partialKey;             // midiNoteNumber for nearest key to this partial; used to determine whether this partial gets excited
    float gain;                 // gain multiplier for this partial
    float offset;               // offset, in cents, from ET for this partial
    //uint64 playPosition;        // current play position for this resonance (samples)
                                // ==> initialize to large number! perhaps 5 minutes * sampling rate, and cap it there in ProcessBlock

    //const uint64 maxPlayPosition = 5 * 60 * 96000; // really high number, longer than any of the samples
    
    uint64 startTime = 0;       // time, in samples, that this partial began playing
    
    // sets the time that this sample started playing, in samples since this processor started
    void setStartTime(uint64 st, uint64 currentTime) { startTime = st + currentTime; }
    
    // gets current playback time (in samples) within the sample
    uint64 getPlayPosition(uint64 currentTime) { return currentTime - startTime; }
};

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
        rSoundSet(r->rSoundSet),
        rUseGlobalSoundSet(r->rUseGlobalSoundSet),
        rSoundSetName(r->rSoundSetName),
        rAttack(r->rAttack),
        rDecay(r->rDecay),
        rRelease(r->rRelease),
        rSustain(r->rSustain),
        rDefaultGain(r->rDefaultGain),
        rBlendronicGain(r->rBlendronicGain),
        rMinStartTimeMS(r->rMinStartTimeMS),
        rMaxStartTimeMS(r->rMaxStartTimeMS),
        rMaxSympStrings(r->rMaxSympStrings),
        rFundamentalKey(r->rFundamentalKey),
        rResonanceKeys(r->rResonanceKeys),
        rOffsetsKeys(r->rOffsetsKeys),
        rGainsKeys(r->rGainsKeys),
        rActiveHeldKeys(r->rActiveHeldKeys),
        resoId(r->resoId),
        name(r->name)
    {
        setDefaultPartialStructure();
    }

    //constructor with input
    ResonancePreparation(String newName, float defaultGain, float blendGain, int resoID) :
        rSoundSet(-1),
        rUseGlobalSoundSet(true),
        rSoundSetName(String()),
        rAttack(50),
        rDecay(3),
        rRelease(50),
        rSustain(1.),
        rDefaultGain(defaultGain, true),
        rBlendronicGain(blendGain, true),
        rMinStartTimeMS(400),
        rMaxStartTimeMS(4000),
        rMaxSympStrings(8),
        rFundamentalKey(0),
        rResonanceKeys({}),
        rOffsetsKeys({}),
        rGainsKeys({}),
        rActiveHeldKeys({}),
        resoId(resoID),
        name(newName)
    {
        setDefaultPartialStructure();
    }

    //empty constructor, values will need to be tweaked
    ResonancePreparation(int Id) :
    rSoundSet(-1),
    rUseGlobalSoundSet(true),
    rSoundSetName(String()),
    rAttack(50),
    rDecay(3),
    rRelease(50),
    rSustain(1.),
    rDefaultGain(0.5, true),
    rBlendronicGain(0.0, true),
    rMinStartTimeMS(400),
    rMaxStartTimeMS(4000),
    rMaxSympStrings(8),
    rFundamentalKey(0),
    rResonanceKeys({}),
    rOffsetsKeys({}),
    rGainsKeys({}),
    rActiveHeldKeys({}),
    resoId(Id)
    {
        setDefaultPartialStructure();
    }

    // copy, modify, compare, randomize
    inline void copy(ResonancePreparation::Ptr r)
    {
        rSoundSet               = r->rSoundSet;
        rUseGlobalSoundSet      = r->rUseGlobalSoundSet;
        rSoundSetName           = r->rSoundSetName;
        rAttack                 = r->rAttack;
        rDecay                  = r->rDecay;
        rRelease                = r->rRelease;
        rSustain                = r->rSustain;
        rDefaultGain            = r->rDefaultGain;
        rBlendronicGain         = r->rBlendronicGain;
        rMinStartTimeMS         = r->rMinStartTimeMS;
        rMaxStartTimeMS         = r->rMaxStartTimeMS;
        rMaxSympStrings         = r->rMaxSympStrings;
        rFundamentalKey         = r->rFundamentalKey;
        rResonanceKeys          = r->rResonanceKeys;
        rOffsetsKeys            = r->rOffsetsKeys;
        rGainsKeys              = r->rGainsKeys;
        rActiveHeldKeys         = r->rActiveHeldKeys;
    }

    void performModification(ResonanceModification* r, Array<bool> dirty);

    inline bool compare(ResonancePreparation::Ptr r)
    {
        return (
                rSoundSet               == r->rSoundSet &&
                rUseGlobalSoundSet      == r->rUseGlobalSoundSet &&
                rSoundSetName           == r->rSoundSetName &&
                rAttack                 == r->rAttack &&
                rDecay                  == r->rDecay &&
                rRelease                == r->rRelease &&
                rSustain                == r->rSustain &&
                rDefaultGain            == r->rDefaultGain &&
                rBlendronicGain         == r->rBlendronicGain &&
                rMinStartTimeMS         == r->rMinStartTimeMS &&
                rMaxStartTimeMS         == r->rMaxStartTimeMS &&
                rMaxSympStrings         == r->rMaxSympStrings &&
                rFundamentalKey         == r->rFundamentalKey &&
                rResonanceKeys          == r->rResonanceKeys &&
                rOffsetsKeys            == r->rOffsetsKeys &&
                rGainsKeys              == r->rGainsKeys &&
                rActiveHeldKeys         == r->rActiveHeldKeys
                );

    }

    inline void randomize()
    {
        Random::getSystemRandom().setSeedRandomly();

        float r[100];

        for (int i = 0; i < 100; i++)  r[i] = (Random::getSystemRandom().nextFloat());
        //int idx = 0;

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
    
    void stepModdables()
    {
        rSoundSet.step();
        rUseGlobalSoundSet.step();
        rSoundSetName.step();
        rAttack.step();
        rDecay.step();
        rRelease.step();
        rSustain.step();
        rDefaultGain.step();
        rBlendronicGain.step();
        rMinStartTimeMS.step();
        rMaxStartTimeMS.step();
        rMaxSympStrings.step();
        rFundamentalKey.step();
        rResonanceKeys.step();
        rOffsetsKeys.step();
        rGainsKeys.step();
        rActiveHeldKeys.step();
    }
    
    void resetModdables()
    {
        DBG("resetting Resonance Moddables");
        rSoundSet.reset();
        rUseGlobalSoundSet.reset();
        rSoundSetName.reset();
        rAttack.reset();
        rDecay.reset();
        rRelease.reset();
        rSustain.reset();
        rDefaultGain.reset();
        rBlendronicGain.reset();
        rMinStartTimeMS.reset();
        rMaxStartTimeMS.reset();
        rMaxSympStrings.reset();
        rFundamentalKey.reset();
        rResonanceKeys.reset();
        rOffsetsKeys.reset();
        rGainsKeys.reset();
        for (auto n : rActiveHeldKeys.value)
        {
            removeSympStrings(n, 0);
            clearSympString(n);
        }
        rActiveHeldKeys.reset();
        for (auto n : rActiveHeldKeys.value)
        {
            addSympStrings(n, 0, false);
        }
        
        updatePartialStructure();
    }

    //accessors
    inline const String getName() const noexcept { return name; }
    inline const float getDefGain() const noexcept { return rDefaultGain.value; }
    inline const float getBlendGain() const noexcept { return rBlendronicGain.value; }
    inline const Array<float> getADSRvals() const noexcept
    {
        return
        {
            (float)rAttack.value, (float)rDecay.value,
            (float)rSustain.value, (float)rRelease.value

        };
    }
    inline int getSoundSet() { return rUseGlobalSoundSet.value ? -1 : rSoundSet.value; }
    inline float* getDefaultGainPtr() { return &rDefaultGain.value; }
    inline float* getBlendGainPtr() { return &rBlendronicGain.value; }

    //mutators
    inline void setName(String n) { name = n; }
    inline void setDefGain(float gainToSet) { rDefaultGain = gainToSet; }
    inline void setBlendGain(float gainToSet) { rBlendronicGain = gainToSet; }
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
    
    void setFundamentalKey(int nf)
    {
        rFundamentalKey = nf;
        updatePartialStructure();
    }
    
    void toggleResonanceKey(int nr)
    {
        //if (rResonanceKeys.base.contains(nr)) {
        if (rResonanceKeys.arrayContains(nr)) {
            DBG("turning off this resonance note");
            //rResonanceKeys.value.removeAllInstancesOf(nr);
            rResonanceKeys.arrayRemoveAllInstancesOf(nr);
            //rOffsetsKeys.value.set(nr, 0.);
            //rGainsKeys.value.set(nr, 1.);
            rOffsetsKeys.setArrayValue(nr, 0.0f);
            rGainsKeys.setArrayValue(nr, 1.0f);
        }
        else {
            DBG("turning on this resonance note");
            //rResonanceKeys.value.add(nr);
            rResonanceKeys.addArrayValue(nr);
            //rOffsetsKeys.value.set(nr, 0.);
            //rGainsKeys.value.set(nr, 1.);
            rOffsetsKeys.setArrayValue(nr, 0.0f);
            rGainsKeys.setArrayValue(nr, 1.0f);
        }
        
        updatePartialStructure();
    }
    
    void addResonanceKey(int nr)
    {
        //if(!rResonanceKeys.base.contains(nr))
        if(!rResonanceKeys.arrayContains(nr))
        {
            //rResonanceKeys.value.add(nr);
            rResonanceKeys.addArrayValue(nr);
            //rOffsetsKeys.value.set(nr, 0.);
            //rGainsKeys.value.set(nr, 1.);
            rOffsetsKeys.setArrayValue(nr, 0.0f);
            rGainsKeys.setArrayValue(nr, 1.0f);
            
            updatePartialStructure();
        }
    }
    
    void addResonanceKey(int nr, float gain, float offset)
    {
        //if(!rResonanceKeys.base.contains(nr))
        if(!rResonanceKeys.arrayContains(nr))
        {
            //rResonanceKeys.value.add(nr);
            rResonanceKeys.addArrayValue(nr);
            //rOffsetsKeys.value.set(nr, offset);
            //rGainsKeys.value.set(nr, gain);
            rOffsetsKeys.setArrayValue(nr, offset);
            rGainsKeys.setArrayValue(nr, gain);
            
            updatePartialStructure();
        }
    }
    
    void setOffset(int nr)
    {
        //if (rResonanceKeys.base.contains(nr))
        if (rResonanceKeys.arrayContains(nr))
        {
            //rOffsetsKeys.value.set(nr, 0.);
            rOffsetsKeys.setArrayValue(nr, 0.0f);
            updatePartialStructure();
        }
    }
    
    void setGain(int nr)
    {
        //if (rResonanceKeys.base.contains(nr))
        if (rResonanceKeys.arrayContains(nr))
        {
            //rGainsKeys.value.set(nr, 1.);
            rGainsKeys.setArrayValue(nr, 1.0f);
            updatePartialStructure();
        }
    }
    
    void setOffsets(Array<float> no) {
        
        for (int i = 0; i < 128; i++)
        {
            //if (rResonanceKeys.base.contains(i))
            if (rResonanceKeys.arrayContains(i))
            {
                //rOffsetsKeys.value.set(i, no[i]);
                rOffsetsKeys.setArrayValue(i, no[i]);
            }
        }
        
        updatePartialStructure();
    }
    
    void setGains(Array<float> no) {
        
        for (int i = 0; i < 128; i++)
        {
            //if (rResonanceKeys.base.contains(i))
            if (rResonanceKeys.arrayContains(i))
            {
                //rGainsKeys.value.set(i, no[i]);
                rGainsKeys.setArrayValue(i, no[i]);
            }
        }
        
        updatePartialStructure();
    }
    
    void setHeldKeys(Array<int> no)
    {
        for (int i : no)
        {
            if(!rActiveHeldKeys.arrayContains(i))
                rActiveHeldKeys.addArrayValue(i);
        }
        
    }
    
    
    void addHeldKey(int no)
    {
        if(!rActiveHeldKeys.arrayContains(no))
            rActiveHeldKeys.addArrayValue(no);
    }
    int getFundamentalKey() { return rFundamentalKey.value; }
    Array<int> getResonanceKeys() { return rResonanceKeys.value; }
    Array<Array<float>> getPartialStructure() { return partialStructure; }
    Array<float> getOffsets() { return rOffsetsKeys.value; }
    Array<float> getGains() { return rGainsKeys.value; }
    //Array<int> getHeld() {return rActiveHeldKeys.value; }
    
    void updatePartialStructure()
    {
        // => partialStructure
        // 2D array for partial structure
        //      index is partial #
        //      contents are interval from fundamental, gain, and offset from ET
        //      by default, first 8 partials for conventional overtone series
        //      user needs to be able to set these
            
        //Array<Array<float>> partialStructure;
    
        partialStructure.clearQuick();
        
        for (auto i : rResonanceKeys.value)
		{
			Array<float> newres;
            newres.add(i - rFundamentalKey.value);
            newres.add(rGainsKeys.value[i]);
            newres.add(rOffsetsKeys.value[i]);
            partialStructure.add(newres);
            //DBG("updatePartialStructure " + String(i) + " " + String(gainsKeys[i]) + " " + String(offsetsKeys[i]));
            // syntax below doesn't work with Windows, hence the individual adds above.
            //partialStructure.add({i - rFundamentalKey.value, rGainsKeys.value[i], rOffsetsKeys.value[i]});
            //partialStructure.add(((static_cast<void>(i - rFundamentalKey.value)), static_cast<void>(rGainsKeys.value[i]), rOffsetsKeys.value[i]));
        }
        
        // printPartialStructure();
    }
    
    void setDefaultPartialStructure()
    {
        //fundamentalKey = 0;
        rFundamentalKey = 0;
        
        for(int i = 0; i < 128; i++)
        {
            //offsetsKeys.set(i, 0.);
            //gainsKeys.set(i, 1.);
            rOffsetsKeys.setArrayValue(i, 0.0f);
            rGainsKeys.setArrayValue(i, 1.0f);
        }
        
        addResonanceKey( 0, 1.0, 0.);
        addResonanceKey(12, 0.8, 0);
        addResonanceKey(19, 0.7, 2);
        addResonanceKey(24, 0.8, 0);
        addResonanceKey(28, 0.6, -13.7);
        addResonanceKey(31, 0.7, 2);
        addResonanceKey(34, 0.5, -31.175);
        addResonanceKey(36, 0.8, 0);

    }
    
    void printPartialStructure()
    {
        DBG("PRINTING PARTIAL STRUCTURE");
        for (auto part : partialStructure)
        {
            DBG(String(part[0]) + ":" + String(part[1]) + ":" + String(part[2]));
        }
    }

    // TODO
    void print(void)
    {
    }

    ValueTree getState(void)
    {
        ValueTree prep("params");

        rDefaultGain.getState(prep, ptagResonance_gain);
        rBlendronicGain.getState(prep, ptagResonance_blendronicGain);
        
        ValueTree ADSRvals( vtagResonance_ADSR);
        int count = 0;
        rAttack.getState(ADSRvals, ptagFloat + String(count++));
        rDecay.getState(ADSRvals, ptagFloat + String(count++));
        rSustain.getState(ADSRvals, ptagFloat + String(count++));
        rRelease.getState(ADSRvals, ptagFloat + String(count));
        prep.addChild(ADSRvals, -1, 0);
        
        rUseGlobalSoundSet.getState(prep, ptagResonance_useGlobalSoundSet);
        rSoundSetName.getState(prep, ptagResonance_soundSet);
        
        rMinStartTimeMS.getState(prep, ptagResonance_starttimeMin);
        rMaxStartTimeMS.getState(prep, ptagResonance_starttimeMax);
        
        rMaxSympStrings.getState(prep, ptagResonance_maxSympStrings);
        
        rFundamentalKey.getState(prep, ptagResonance_fundamentalKey);
        rResonanceKeys.getState(prep, StringArray(vtagResonance_resonanceKeys, ptagInt));
        rOffsetsKeys.getState(prep, StringArray(vtagResonance_offsets, ptagFloat));
        rGainsKeys.getState(prep, StringArray(vtagResonance_gains, ptagFloat));
        rActiveHeldKeys.getState(prep, StringArray(vtagResonance_add, ptagInt));
        return prep;
    }

    void setState(XmlElement* e)
    {
        //setDefaultPartialStructure();
        
        rDefaultGain.setState(e, ptagResonance_gain, 1.0f);
        rBlendronicGain.setState(e, ptagResonance_blendronicGain, 1.0f);
        
        XmlElement* sub = e->getChildByName(vtagResonance_ADSR);
        if (sub != nullptr)
        {
            int count = 0;
            rAttack.setState(sub, ptagFloat + String(count++), 3);
            rDecay.setState(sub, ptagFloat + String(count++), 3);
            rSustain.setState(sub, ptagFloat + String(count++), 1.);
            rRelease.setState(sub, ptagFloat + String(count), 30);
        }
        
        rUseGlobalSoundSet.setState(e, ptagResonance_useGlobalSoundSet, true);
        rSoundSetName.setState(e, ptagResonance_soundSet, String());
        
        rMinStartTimeMS.setState(e, ptagResonance_starttimeMin, 400);
        rMaxStartTimeMS.setState(e, ptagResonance_starttimeMax, 4000);
        
        rMaxSympStrings.setState(e, ptagResonance_maxSympStrings, 8);
        
        rFundamentalKey.setState(e, ptagResonance_fundamentalKey, 0);
        rResonanceKeys.setState(e, StringArray(vtagResonance_resonanceKeys, ptagInt), 0);
        DBG("before setState - rOffsetsKeys: " + floatArrayToString(rOffsetsKeys.value));
        rOffsetsKeys.setState(e, StringArray(vtagResonance_offsets, ptagFloat), 0.);
        DBG("after setState - rOffsetsKeys: " + floatArrayToString(rOffsetsKeys.value));
        rGainsKeys.setState(e, StringArray(vtagResonance_gains, ptagFloat), 1.);
        rActiveHeldKeys.setState(e, StringArray(vtagResonance_add, ptagInt), 0);
        updatePartialStructure();
    }
    
    bool modded = false;

    Moddable<int> rSoundSet;
    Moddable<bool> rUseGlobalSoundSet;
    Moddable<String> rSoundSetName;

    Moddable<int> rAttack, rDecay, rRelease; //ADSR in MS
    Moddable<float> rSustain;

    Moddable<float> rDefaultGain; //shortcut until list of overtone gains is implemented
    Moddable<float> rBlendronicGain;

    Moddable<int> rMinStartTimeMS;
    Moddable<int> rMaxStartTimeMS;
    
    // => cap the number of sympStrings to this
    Moddable<int> rMaxSympStrings;
    
    // key that partials are related to. usually the lowest C on the UI keyboard, but could be otherwise, for "undertones"
    Moddable<int> rFundamentalKey;
    
    // with the partials indicated in resonanceKeys, with offsets and gains for each resonance/partial, indexed by noteNumber
    Moddable<Array<int>> rResonanceKeys;
    Moddable<Array<float>> rOffsetsKeys;
    Moddable<Array<float>> rGainsKeys;
    
    //

    
    inline const int getMinStartTime() const noexcept { return rMinStartTimeMS.value; }
    inline const int getMaxStartTime() const noexcept { return rMaxStartTimeMS.value; }
    inline const int getMaxSympStrings() const noexcept { return rMaxSympStrings.value; }
    
    inline void setMinStartTime(int inval) { rMinStartTimeMS = inval; }
    inline void setMaxStartTime(int inval) { rMaxStartTimeMS = inval; }
    inline void setMaxSympStrings(int inval) { rMaxSympStrings = inval; }
    
    inline void updateCurrentTime(uint64 numSamples) { currentTime += numSamples; }
    uint64 getCurrentTime() { return currentTime; }
    
    // => current strings
    // A queue to store the currently active notes in sympStrings
    // so we can remove the oldest one when we exceed maxSympStrings
    Moddable<Array<int>> rActiveHeldKeys;
    Array<int> getHeldKeys() {return rActiveHeldKeys.value;};
    Array<int> getRingingStrings();
    BKSynthesiser*              synth;
    
    //symp strings code
    void addSympStrings(int noteNumber, float velocity);
    void addSympStrings(int noteNumber, float velocity, bool ignoreRepeatedNotes);
    void removeSympStrings(int noteNumber, float velocity);
    void clearSympString(int noteNumber)
    {
        const ScopedLock sl (lock);
        sympStrings.remove(noteNumber);
    }
    
    Array<int> getSympStrings();
    
    
    // => sympStrings
    // data structure for pointing to all of the undamped strings and their partials
    //      outside map is indexed by held note (midiNoteNumber), inside array resizes depending on the number of partials
    //      so this includes all of the partials for all of the currently undamped strings
    HashMap<int, Array<SympPartial::Ptr>> sympStrings;
    
    void setResoId(int Id) {resoId = Id;}
    int resoId;
    
    
private:

    CriticalSection lock;
    
    String name;
    
    // => partialStructure
    // 2D array for partial structure
    //      index is partial #
    //      contents are interval from fundamental, gain, and offset from ET
    //      by default, first 8 partials for conventional overtone series
    //      user needs to be able to set these
    Array<Array<float>> partialStructure;
    
    // key that partials are related to. usually the lowest C on the UI keyboard
    //int fundamentalKey;
    
    // with the partials indicated in resonanceKeys
    //Array<int> resonanceKeys;
    //HashMap<int, float> offsetsKeys;
    //HashMap<int, float> gainsKeys;
    
    // this is accessed by the audio thread as well as event thread(s), so making it atomic for thread safety
    std::atomic<std::uint64_t> currentTime;
    
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
        DBG("created Resonance with ID " + String(Id));
    }

    Resonance(int Id, bool random = false) :
        Id(Id),
        name("Resonance " + String(Id))
    {
        prep = new ResonancePreparation(Id);
        if (random) randomize();
        DBG("created Resonance with ID " + String(Id));
    }

    inline Resonance::Ptr duplicate()
    {
        ResonancePreparation::Ptr copyPrep = new ResonancePreparation(prep);

        Resonance::Ptr copy = new Resonance(copyPrep, -1);

        copy->setName(name);

        return copy;
    }

    void clear(void);
    

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
    inline void setId(int newId) {
        Id = newId;
        prep->setResoId(Id);
    }
    inline void setName(String newName) { name = newName; }
    inline String getName() const noexcept { return name; }

    ResonancePreparation::Ptr prep;

private:
    int Id;
    String name;

    JUCE_LEAK_DETECTOR(Resonance)
};





////////////////////////////////////////////////////////////////////////////////////
/////////////////////////RESONANCE PROCESSOR/////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

class ResonanceProcessor : public ReferenceCountedObject
{

public:
    typedef ReferenceCountedObjectPtr<ResonanceProcessor>   Ptr;
    typedef Array<ResonanceProcessor::Ptr>                  PtrArr;
    typedef Array<ResonanceProcessor::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<ResonanceProcessor>                  Arr;
    typedef OwnedArray<ResonanceProcessor, CriticalSection> CSArr;

    ResonanceProcessor( Resonance::Ptr rResonance,
                        TuningProcessor::Ptr rTuning,
                        GeneralSettings::Ptr rGeneral,
                        BKSynthesiser* rMain
    );
    ~ResonanceProcessor();
    
    inline void reset(void)
    {
        resonance->prep->resetModdables();
        DBG("resonance reset called");
    }

    //called with every audio vector
    BKSampleLoadType sampleType;

    void keyPressed(int noteNumber, Array<float>& targetVelocities, bool fromPress);
    void keyReleased(int noteNumber, Array<float>& targetVelocities, bool fromPress);

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

    inline void addBlendronic(EffectProcessor::Ptr blend)
    {
        effects.add(blend);
    }
    
    inline EffectProcessor::PtrArr getBlendronic(void)
    {
        EffectProcessor::PtrArr blends;
        for (auto e : effects)
        {
            if(e->getType() == EffectType::EffectBlendronic)
                blends.add(e);
        }
        return blends;
    }

    void ringSympStrings(int noteNumber, float velocity);
    
    
private:
    CriticalSection lock;
    
    Resonance::Ptr              resonance;
    TuningProcessor::Ptr        tuning;
    GeneralSettings::Ptr        general;
    Keymap::PtrArr              keymaps;
    EffectProcessor::PtrArr effects;
    
    Array<Array<float>> velocities;
    Array<Array<float>> invertVelocities;

    JUCE_LEAK_DETECTOR(ResonanceProcessor);
};
#endif

