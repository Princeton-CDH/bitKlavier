/*
 ==============================================================================
 
 Direct.h
 Created: 6 Dec 2016 12:46:37pm
 Author:  Michael R Mulshine
 
 ==============================================================================
 */

#ifndef DIRECT_H_INCLUDED
#define DIRECT_H_INCLUDED

#include "BKUtilities.h"
#include "BKSynthesiser.h"
#include "Keymap.h"
#include "Tuning.h"
#include "Blendronic.h"

class DirectModification;

/*
 DirectPreparation holds all the state variable values for the
 Direct preparation. As with other preparation types, bK will use
 two instantiations of DirectPreparation for every active
 Direct in the gallery, one to store the static state of the
 preparation, and the other to store the active state. These will
 be the same, unless a Modification is triggered, in which case the
 active state will be changed (and a Reset will revert the active state
 to the static state).
 */

class DirectPreparation : public ReferenceCountedObject
{
public:
    typedef ReferenceCountedObjectPtr<DirectPreparation>   Ptr;
    typedef Array<DirectPreparation::Ptr>                  PtrArr;
    typedef Array<DirectPreparation::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<DirectPreparation>                  Arr;
    typedef OwnedArray<DirectPreparation, CriticalSection> CSArr;
    
    DirectPreparation(DirectPreparation::Ptr d) :
    dGain(d->dGain),
    dResonanceGain(d->dResonanceGain),
    dHammerGain(d->dHammerGain),
    dBlendronicGain(d->dBlendronicGain),
    dAttack(d->dAttack),
    dDecay(d->dDecay),
    dRelease(d->dRelease),
    dSustain(d->dSustain),
    dTransposition(d->dTransposition),
    dTranspUsesTuning(d->dTranspUsesTuning),
    dUseGlobalSoundSet(d->dUseGlobalSoundSet),
    dSoundSet(d->getSoundSet()),
    dSoundSetName(d->dSoundSetName),
    velocityMin(d->velocityMin),
    velocityMax(d->velocityMax)
    {
    }
    
    DirectPreparation(Array<float> transp,
                      float gain,
                      bool resAndHammer,
                      float resGain,
                      float hamGain,
                      float blendGain,
                      int atk,
                      int dca,
                      float sust,
                      int rel):
    modded(false),
    dGain(gain, true),
    dResonanceGain(resGain, true),
    dHammerGain(hamGain, true),
    dBlendronicGain(blendGain, true),
    dAttack(atk),
    dDecay(dca),
    dRelease(rel),
    dSustain(sust),
    dTransposition(transp),
    dTranspUsesTuning(false),
    dUseGlobalSoundSet(true),
    dSoundSet(-1),
    dSoundSetName(String()),
    velocityMin(0),
    velocityMax(127)
    {

    }
    
    DirectPreparation(void):
    modded(false),
    dGain(0.0, true),
    dResonanceGain(-6.0, true),
    dHammerGain(-6.0, true),
    dBlendronicGain(0.0, true),
    dAttack(3),
    dDecay(3),
    dRelease(30),
    dSustain(1.),
    dTransposition(Moddable<Array<float>>(Array<float>(0.0f))),
    dTranspUsesTuning(false),
    dUseGlobalSoundSet(true),
    dSoundSet(-1),
    dSoundSetName(String()),
    velocityMin(0),
    velocityMax(127)
    {
        
    }
    
    ~DirectPreparation()
    {
        
    }
    
    inline void copy(DirectPreparation::Ptr d)
    {
        dGain = d->dGain;
        dResonanceGain = d->dResonanceGain;
        dHammerGain = d->dHammerGain;
        dBlendronicGain = d->dBlendronicGain;
        dAttack = d->dAttack;
        dDecay = d->dDecay;
        dSustain = d->dSustain;
        dRelease = d->dRelease;
        dTransposition = d->dTransposition;
        dTranspUsesTuning = d->dTranspUsesTuning;
        dUseGlobalSoundSet = d->dUseGlobalSoundSet;
        dSoundSet = d->getSoundSet();
        dSoundSetName = d->dSoundSetName;
        velocityMin = d->velocityMin;
        velocityMax = d->velocityMax;
    }
    
    // Using a raw pointer here instead of ReferenceCountedObjectPtr (Ptr)
    // so we can use forwarded declared DirectModification
    void performModification(DirectModification* d, Array<bool> dirty);
    
    void stepModdables()
    {
        dGain.step();
        dResonanceGain.step();
        dHammerGain.step();
        dBlendronicGain.step();
        
        dAttack.step();
        dDecay.step();
        dRelease.step();
        dSustain.step();
        
        dTransposition.step();
        
        dTranspUsesTuning.step();
        
        dUseGlobalSoundSet.step();
        
        dSoundSet.step();
        dSoundSetName.step();
        
        velocityMin.step();
        velocityMax.step();
    }
    
    void resetModdables()
    {
        dGain.reset();
        dResonanceGain.reset();
        dHammerGain.reset();
        dBlendronicGain.reset();
        
        dAttack.reset();
        dDecay.reset();
        dRelease.reset();
        dSustain.reset();
        
        dTransposition.reset();
        
        dTranspUsesTuning.reset();
        
        dUseGlobalSoundSet.reset();
        
        dSoundSet.reset();
        dSoundSetName.reset();
        
        velocityMin.reset();
        velocityMax.reset();
    }
    
    inline bool compare(DirectPreparation::Ptr d)
    {
        return  (dTransposition         ==      d->dTransposition       )   &&
        (dTranspUsesTuning      ==      d->dTranspUsesTuning    )   &&
        (dGain                  ==      d->dGain           )   &&
        (dResonanceGain         ==      d->dResonanceGain  )   &&
        (dHammerGain            ==      d->dHammerGain     )   &&
        (dBlendronicGain        ==      d->dBlendronicGain )   &&
        (dAttack                ==      d->dAttack         )   &&
        (dDecay                 ==      d->dDecay          )   &&
        (dSustain               ==      d->dSustain        )   &&
        (dRelease               ==      d->dRelease        )   ;
    }
    
    // for unit-testing
    inline void randomize(void)
    {
        Random::getSystemRandom().setSeedRandomly();
        
        float r[20];
        
        for (int i = 0; i < 20; i++)    r[i] = (Random::getSystemRandom().nextFloat());
        int idx = 0;
        
        Array<float> dt;
        for (int i = 0; i < Random::getSystemRandom().nextInt(10); ++i)
        {
            dt.add(i, (Random::getSystemRandom().nextFloat() * 48.0f - 24.0f));
        }
        dTransposition.set(dt);
        
        dGain = r[idx++];
        dResonanceGain = r[idx++];
        dHammerGain = r[idx++];
        dBlendronicGain = r[idx++];
        dAttack = r[idx++] * 2000.0f + 1.0f;
        dDecay = r[idx++] * 2000.0f + 1.0f;
        dSustain = r[idx++];
        dRelease = r[idx++] * 2000.0f + 1.0f;
    }
    
    inline const String getName() const noexcept {return name;}
    inline void setName(String n){name = n;}
    
    inline float* getGainPtr() { return &dGain.value; }
    inline float* getResonanceGainPtr() { return &(dResonanceGain.value); }
    inline float* getHammerGainPtr() { return &(dHammerGain.value); }
    inline float* getBlendronicGainPtr() { return &(dBlendronicGain.value); }
    
    inline const Array<float> getADSRvals() const noexcept
    { return {(float) dAttack.value, (float) dDecay.value,(float) dSustain.value, (float)dRelease.value}; }
    
    inline void setTranspUsesTuning(bool val)         { dTranspUsesTuning = val;}
    inline void setADSRvals(Array<float> vals)
    {
        dAttack = vals[0];
        dDecay = vals[1];
        dSustain = vals[2];
        dRelease = vals[3];
    }
    
    void print(void)
    {
        DBG("dTransposition: "  + floatArrayToString(dTransposition.value));
        DBG("dGain: "           + String(dGain.value));
        DBG("dResGain: "        + String(dResonanceGain.value));
        DBG("dHammerGain: "     + String(dHammerGain.value));
        DBG("dBlendronicGain: " + String(dBlendronicGain.value));
        DBG("dAttack: "         + String(dAttack.value));
        DBG("dDecay: "          + String(dDecay.value));
        DBG("dSustain: "        + String(dSustain.value));
        DBG("dRelease: "        + String(dRelease.value));
    }
    
    ValueTree getState(void)
    {
        ValueTree prep( "params" );
        
        dGain.getState(prep, ptagDirect_gain);
        dResonanceGain.getState(prep, ptagDirect_resGain);
        dHammerGain.getState(prep, ptagDirect_hammerGain);
        dBlendronicGain.getState(prep, ptagDirect_blendronicGain);
        
        dTranspUsesTuning.getState(prep, ptagDirect_transpUsesTuning);
        dTransposition.getState(prep, StringArray(vtagDirect_transposition, ptagFloat));
        
        ValueTree ADSRvals( vtagDirect_ADSR);
        int count = 0;
        dAttack.getState(ADSRvals, ptagFloat + String(count++));
        dDecay.getState(ADSRvals, ptagFloat + String(count++));
        dSustain.getState(ADSRvals, ptagFloat + String(count++));
        dRelease.getState(ADSRvals, ptagFloat + String(count));
        prep.addChild(ADSRvals, -1, 0);
        
        dUseGlobalSoundSet.getState(prep, ptagDirect_useGlobalSoundSet);
        dSoundSetName.getState(prep, ptagDirect_soundSet);
        
        velocityMin.getState(prep, ptagDirect_velocityMin);
        velocityMax.getState(prep, ptagDirect_velocityMax);
        
        return prep;
    }
    
    void setState(XmlElement* e)
    {
        dGain.setState(e, ptagDirect_gain, 1.0f);
        dResonanceGain.setState(e, ptagDirect_resGain, 1.0f);
        dHammerGain.setState(e, ptagDirect_hammerGain, 1.0f);
        dBlendronicGain.setState(e, ptagDirect_blendronicGain, 1.0f);
        
        dTranspUsesTuning.setState(e, ptagDirect_transpUsesTuning, false);
        
        dUseGlobalSoundSet.setState(e, ptagDirect_useGlobalSoundSet, true);
        dSoundSetName.setState(e, ptagDirect_soundSet, String());
        
        velocityMin.setState(e, ptagDirect_velocityMin, 0);
        velocityMax.setState(e, ptagDirect_velocityMax, 127);
        
        dTransposition.setState(e, StringArray(vtagDirect_transposition, ptagFloat), Array<float>(0.0f));
        
        XmlElement* sub = e->getChildByName(vtagDirect_ADSR);
        if (sub != nullptr)
        {
            int count = 0;
            dAttack.setState(sub, ptagFloat + String(count++), 3);
            dDecay.setState(sub, ptagFloat + String(count++), 3);
            dSustain.setState(sub, ptagFloat + String(count++), 1.);
            dRelease.setState(sub, ptagFloat + String(count), 30);
        }
    }
    
    void setSoundSet(int Id) { dSoundSet = Id; }
    int getSoundSet() { return dUseGlobalSoundSet.value ? -1 : dSoundSet.value; }
    
    bool modded = false;
    
    // output gain multiplier
    Moddable<float> dGain;
    // gain multipliers for release resonance and hammer
    Moddable<float> dResonanceGain, dHammerGain;
    Moddable<float> dBlendronicGain;
    
    // ADSR, in ms, or gain multiplier for sustain
    Moddable<int> dAttack, dDecay, dRelease;
    Moddable<float> dSustain;
    
    // transposition, in half steps
    Moddable<Array<float>> dTransposition;
    
    // are Transposition values filtered by the attached Tuning?
    // original/default = false
    Moddable<bool> dTranspUsesTuning;
    
    Moddable<bool> dUseGlobalSoundSet;
    
    Moddable<int> dSoundSet;
    Moddable<String> dSoundSetName;
    
    Moddable<int> velocityMin, velocityMax;
private:
    //* Instance Variables *//
    
    // name of this one
    String  name;
    
    JUCE_LEAK_DETECTOR(DirectPreparation);
};


class Direct : public ReferenceCountedObject
{
    
public:
    typedef ReferenceCountedObjectPtr<Direct>   Ptr;
    typedef Array<Direct::Ptr>                  PtrArr;
    typedef Array<Direct::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<Direct>                  Arr;
    typedef OwnedArray<Direct, CriticalSection> CSArr;
    
    
    Direct(DirectPreparation::Ptr d,
           int Id):
    prep(new DirectPreparation(d)),
    Id(Id),
    name("Direct "+String(Id))
    {
        
    }
    
    Direct(int Id, bool random = false):
    Id(Id),
    name("Direct "+String(Id))
    {
        prep = new DirectPreparation();
        if (random) randomize();
    };
    
    inline void clear(void)
    {
        prep       = new DirectPreparation();
    }
    
    inline Direct::Ptr duplicate()
    {
        DirectPreparation::Ptr copyPrep = new DirectPreparation(prep);
        
        Direct::Ptr copy = new Direct(copyPrep, -1);
        
        copy->setName(name);
        
        return copy;
    }
    
    inline ValueTree getState()//bool active = false)
    {
        ValueTree vt(vtagDirect);
        
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
    
    ~Direct() {};
    
    inline int getId() {return Id;}
    inline void setId(int newId) { Id = newId;}
    
    DirectPreparation::Ptr      prep;
    
    inline void copy(Direct::Ptr from)
    {
        prep->copy(from->prep);
    }
    
    inline void randomize()
    {
        clear();
        Random::getSystemRandom().setSeedRandomly();
        prep->randomize();
        Id = Random::getSystemRandom().nextInt(Range<int>(1, 1000));
        name = "random";
    }
    
    inline String getName(void) const noexcept {return name;}
    inline void setName(String newName)
    {
        name = newName;
    }
    
private:
    int Id;
    String name;;
    
    JUCE_LEAK_DETECTOR(Direct)
};


/*
 DirectProcessor does the main work, including processing a block
 of samples and sending it out. It connects Keymap, Tuning, and
 Blendronic preparations together as needed, and gets the Direct
 values it needs to behave as expected.
 */

class DirectProcessor : public ReferenceCountedObject
{
    
public:
    typedef ReferenceCountedObjectPtr<DirectProcessor>   Ptr;
    typedef Array<DirectProcessor::Ptr>                  PtrArr;
    typedef Array<DirectProcessor::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<DirectProcessor>                  Arr;
    typedef OwnedArray<DirectProcessor, CriticalSection> CSArr;
    
    
    DirectProcessor(Direct::Ptr direct,
                    TuningProcessor::Ptr tuning, BlendronicProcessor::PtrArr blend,
                    BKSynthesiser *s, BKSynthesiser *res, BKSynthesiser *ham);
    
    ~DirectProcessor();
    
    BKSampleLoadType sampleType;
    void processBlock(int numSamples, int midiChannel, BKSampleLoadType type);
    
    void    keyPressed(int noteNumber, Array<float>& targetVelocities, bool fromPress);
    void    keyReleased(int noteNumber, Array<float>& targetVelocities, bool fromPress);
    void    playReleaseSample(int noteNumber, Array<float>& targetVelocities,
                              bool fromPress, bool soundfont = false);
    
    inline void prepareToPlay(double sr, BKSynthesiser* main, BKSynthesiser* res, BKSynthesiser* hammer)
    {
        synth = main;
        resonanceSynth = res;
        hammerSynth = hammer;
    }
    
    inline void reset(void)
    {
        direct->prep->resetModdables();
        DBG("direct reset called");
    }
    
    inline int getId(void) const noexcept { return direct->getId(); }
    
    inline void setTuning(TuningProcessor::Ptr tuning)
    {
        tuner = tuning;
    }
    
    inline TuningProcessor::Ptr getTuning(void)
    {
        return tuner;
    }
    
    inline void addBlendronic(BlendronicProcessor::Ptr blend)
    {
        blendronic.add(blend);
    }
    
    inline BlendronicProcessor::PtrArr getBlendronic(void)
    {
        return blendronic;
    }
    
    inline void addKeymap(Keymap::Ptr keymap)
    {
        keymaps.add(keymap);
    }
    
    inline Keymap::PtrArr getKeymaps(void)
    {
        return keymaps;
    }
    
    inline float getLastVelocity() const noexcept { return lastVelocity; }
    
    // Return vel if within range, else return -1.f
    float filterVelocity(float vel);
    void resetLastVelocity() { lastVelocityInRange = false; }
    
    Array<Array<float>>& getVelocities() { return velocities; }
    Array<Array<float>>& getInvertVelocities() { return invertVelocities; }
    
    void setVelocities(Array<Array<float>>& newVel) { velocities = newVel; }
    void setInvertVelocities(Array<Array<float>>& newVel) { invertVelocities = newVel; }
    
private:
    BKSynthesiser*      synth;
    BKSynthesiser*      resonanceSynth;
    BKSynthesiser*      hammerSynth;
    
    Direct::Ptr             direct;
    TuningProcessor::Ptr    tuner;
    BlendronicProcessor::PtrArr blendronic;
    
    Keymap::PtrArr      keymaps;
    
    //need to keep track of the actual notes played and their offsets when a particular key is pressed
    //so that they can all be turned off properly, even in the event of a preparation change
    //while the key is held
    Array<int>      keyPlayed[128];//keep track of pitches played associated with particular key on keyboard
    Array<float>    keyPlayedOffset[128];   //and also the offsets
    
    Array<uint64> noteLengthTimers;     // store current length of played notes here
    Array<int> activeNotes;             // table of notes currently being played by player
    
    Array<Array<float>> velocities;
    Array<Array<float>> invertVelocities;
    
    float lastVelocity = 0.0f;
    bool lastVelocityInRange = false;
    
    JUCE_LEAK_DETECTOR(DirectProcessor);
};

#endif  // DIRECT_H_INCLUDED

