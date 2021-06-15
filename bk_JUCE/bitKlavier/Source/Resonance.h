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
        name(r->name)
    {
        
    }

    //constructor with input
    ResonancePreparation(String newName, float defaultGain, float blendGain) :
        rSoundSet(-1),
        rUseGlobalSoundSet(true),
        rSoundSetName(String()),
        rAttack(0),
        rDecay(3),
        rRelease(50),
        rSustain(1.),
        rDefaultGain(defaultGain, true),
        rBlendronicGain(blendGain, true),
        rMinStartTimeMS(400),
        rMaxStartTimeMS(4000),
        rMaxSympStrings(8),
        name(newName)
    {

    }

    //empty constructor, values will need to be tweaked
    ResonancePreparation(void) : 
        rSoundSet(-1),
        rUseGlobalSoundSet(true),
        rSoundSetName(String()),
        rAttack(0),
        rDecay(3),
        rRelease(50),
        rSustain(1.),
        rDefaultGain(0.5, true),
        rBlendronicGain(0.0, true),
        rMinStartTimeMS(400),
        rMaxStartTimeMS(4000),
        rMaxSympStrings(8),
        name("test resonance preparation")
        
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
    
    inline const int getMinStartTime() const noexcept { return rMinStartTimeMS.value; }
    inline const int getMaxStartTime() const noexcept { return rMaxStartTimeMS.value; }
    inline const int getMaxSympStrings() const noexcept { return rMaxSympStrings.value; }
    
    inline void setMinStartTime(int inval) { rMinStartTimeMS = inval; }
    inline void setMaxStartTime(int inval) { rMaxStartTimeMS = inval; }
    inline void setMaxSympStrings(int inval) { rMaxSympStrings = inval; }

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
    uint64 playPosition;        // current play position for this resonance (samples)
                                // ==> initialize to large number! perhaps 5 minutes * sampling rate, and cap it there in ProcessBlock

    const uint64 maxPlayPosition = 5 * 60 * 96000; // really high number, longer than any of the samples
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
    
    Resonance::Ptr              resonance;
    BKSynthesiser*              synth;
    TuningProcessor::Ptr        tuning;
    GeneralSettings::Ptr        general;
    Keymap::PtrArr              keymaps;
    BlendronicProcessor::PtrArr blendronic;
    
    // basic API
    void ringSympStrings(int noteNumber, float velocity, int midiChannel, Array<KeymapTargetState> targetStates); 
    void addSympStrings(int noteNumber, float velocity, int midiChannel, Array<KeymapTargetState> targetStates);
    void removeSympStrings(int noteNumber, float velocity, int midiChannel, Array<KeymapTargetState> targetStates, bool post);

    // => sympStrings
    // data structure for pointing to all of the undamped strings and their partials
    //      outside map is indexed by held note (midiNoteNumber), inside array resizes depending on the number of partials
    //      so this includes all of the partials for all of the currently undamped strings
    
    HashMap<int, Array<SympPartial::Ptr>> sympStrings;

    // => partialStructure
    // 2D array for partial structure
    //      index is partial #
    //      contents are interval from fundamental, gain, and offset from ET
    //      by default, first 8 partials for conventional overtone series
    //      user needs to be able to set these
    //Array<float[3]> partialStructure;
    Array<Array<float>> partialStructure;
    
    // => current strings
    // A queue to store the currently active notes in sympStrings
    // so we can remove the oldest one when we exceed maxSympStrings
    Array<int> activeSympStrings;

    JUCE_LEAK_DETECTOR(ResonanceProcessor);
};
#endif

