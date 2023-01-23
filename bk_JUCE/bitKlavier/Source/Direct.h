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
#include "Effects.h"
#include "BKPianoSampler.h"
#include "GenericProcessor.h"
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

class DirectPreparation : public GenericPreparation
{
public:
//    typedef ReferenceCountedObjectPtr<DirectPreparation>   Ptr;
//    typedef Array<DirectPreparation::Ptr>                  PtrArr;
//    typedef Array<DirectPreparation::Ptr, CriticalSection> CSPtrArr;
//    typedef OwnedArray<DirectPreparation>                  Arr;
//    typedef OwnedArray<DirectPreparation, CriticalSection> CSArr;
    
    
    DirectPreparation(String newName, int Id, Array<float> transp,
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
    velocityMax(127),
    GenericPreparation(BKPreparationType::PreparationTypeDirect, Id, newName)
    {

    }
    
    DirectPreparation(int Id):
    modded(false),
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
    velocityMax(127),
    GenericPreparation(BKPreparationType::PreparationTypeDirect, Id)
    {
        
    }
    
    ~DirectPreparation()
    {
        
    }
    
    inline void copy(DirectPreparation::Ptr d)
    {
        DirectPreparation* _d = dynamic_cast<DirectPreparation*>(d.get());
        defaultGain = d->defaultGain;
        dResonanceGain = _d->dResonanceGain;
        dHammerGain = _d->dHammerGain;
        dBlendronicGain = _d->dBlendronicGain;
        dAttack = _d->dAttack;
        dDecay = _d->dDecay;
        dSustain = _d->dSustain;
        dRelease = _d->dRelease;
        dTransposition = _d->dTransposition;
        dTranspUsesTuning = _d->dTranspUsesTuning;
        useGlobalSoundSet = _d->dUseGlobalSoundSet;
        soundSet = d->getSoundSet();
        
        soundSetName = d->soundSetName;
        velocityMin = _d->velocityMin;
        velocityMax = _d->velocityMax;
    }
    
    // Using a raw pointer here instead of ReferenceCountedObjectPtr (Ptr)
    // so we can use forwarded declared DirectModification
    void performModification(DirectModification* d, Array<bool> dirty);
    
    void stepModdables()
    {
        defaultGain.step();
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
        defaultGain.reset();
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
        DirectPreparation* _d = dynamic_cast<DirectPreparation*>(d.get());
        return  (dTransposition         ==      _d->dTransposition       )   &&
        (dTranspUsesTuning      ==      _d->dTranspUsesTuning    )   &&
        (defaultGain                  ==      d->defaultGain           )   &&
        (dResonanceGain         ==      _d->dResonanceGain  )   &&
        (dHammerGain            ==      _d->dHammerGain     )   &&
        (dBlendronicGain        ==      _d->dBlendronicGain )   &&
        (dAttack                ==      _d->dAttack         )   &&
        (dDecay                 ==      _d->dDecay          )   &&
        (dSustain               ==      _d->dSustain        )   &&
        (dRelease               ==      _d->dRelease        )   ;
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
        
        defaultGain = r[idx++];
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
    
    //inline float* getGainPtr() { return &defaultGain.value; }
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
        DBG("dGain: "           + String(defaultGain.value));
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
        ValueTree vt(vtagResonance);
        
        vt.setProperty( "Id",getId(), 0);
        vt.setProperty( "name", getName(), 0);
        ValueTree prep( "params" );
        
        defaultGain.getState(prep, ptagDirect_gain);
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
        vt.addChild(prep, -1, 0);
        return vt;
    }
    
    void setState(XmlElement* _e)
    {
        setId(_e->getStringAttribute("Id").getIntValue());
        
        String n = _e->getStringAttribute("name");

        if (n != String())     setName(n);
        else                   setName(String(getId()));


        XmlElement *e = _e->getChildByName("params");
        if (e == nullptr)
        {
            e = _e;
        }
        
        defaultGain.setState(e, ptagDirect_gain, 1.0f);
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
    
   
    
    bool modded = false;
    
    // output gain multiplier
    // gain multipliers for release resonance and hammer
    Moddable<float> dResonanceGain, dHammerGain, dBlendronicGain;
    
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


/*
 DirectProcessor does the main work, including processing a block
 of samples and sending it out. It connects Keymap, Tuning, and
 Blendronic preparations together as needed, and gets the Direct
 values it needs to behave as expected.
 */

class DirectProcessor : public GenericProcessor
{
    
public:
//    typedef ReferenceCountedObjectPtr<DirectProcessor>   Ptr;
//    typedef Array<DirectProcessor::Ptr>                  PtrArr;
//    typedef Array<DirectProcessor::Ptr, CriticalSection> CSPtrArr;
//    typedef OwnedArray<DirectProcessor>                  Arr;
//    typedef OwnedArray<DirectProcessor, CriticalSection> CSArr;
//    
    
    DirectProcessor(DirectPreparation::Ptr direct,
                    TuningProcessor::Ptr tuning,
                    BKAudioProcessor& processor, GeneralSettings::Ptr);
    
    ~DirectProcessor();
    
    BKSampleLoadType sampleType;
    void processBlock(AudioSampleBuffer& buffer, MidiBuffer& midiMessages, int numSamples, int midiChannel, BKSampleLoadType type) override;
    
    void    keyPressed(int noteNumber, Array<float>& targetVelocities, bool fromPress) override;
    void    keyReleased(int noteNumber, Array<float>& targetVelocities, bool fromPress) override;
    void    playReleaseSample(int noteNumber, Array<float>& targetVelocities,
                              bool fromPress, bool soundfont = false);
    
    inline void prepareToPlay(GeneralSettings::Ptr gen) override
    {
        synth->playbackSampleRateChanged();
        synth->setGeneralSettings(gen);
        
        hammerSynth->playbackSampleRateChanged();
        hammerSynth->setGeneralSettings(gen);
        
        resonanceSynth->playbackSampleRateChanged();
        resonanceSynth->setGeneralSettings(gen);
        
        pedalSynth->playbackSampleRateChanged();
        pedalSynth->setGeneralSettings(gen);
        
        synth->clearVoices();
        hammerSynth->clearVoices();
        resonanceSynth->clearVoices();
        pedalSynth->clearVoices();
        
        
        for (int i = 0; i < 300; i++)
        {
            synth->addVoice(new BKPianoSamplerVoice(gen));
        }
        for (int i = 0; i < 128; i++)
        {
            resonanceSynth->addVoice(new BKPianoSamplerVoice(gen));
            hammerSynth->addVoice(new BKPianoSamplerVoice(gen));
            pedalSynth->addVoice(new BKPianoSamplerVoice(gen));
        }
        DBG("direct prepartoplay end");
    }
    
    inline void reset(void)
    {
        prep->resetModdables();
        DBG("direct reset called");
    }
    
    inline int getId(void)  const noexcept { return prep->getId(); }
    
    inline void setTuning(TuningProcessor::Ptr tuning)
    {
        tuner = tuning;
    }
    
    inline TuningProcessor::Ptr getTuning(void)
    {
        return tuner;
    }
    
    
    
    
    
    // Return vel if within range, else return -1.f
    float filterVelocity(float vel) override;
    
    Array<Array<float>>& getVelocities() { return velocities; }
    Array<Array<float>>& getInvertVelocities() { return invertVelocities; }
    
    void setVelocities(Array<Array<float>>& newVel) { velocities = newVel; }
    void setInvertVelocities(Array<Array<float>>& newVel) { invertVelocities = newVel; }
    void sustainPedalPressed() 
    {
        pedalSynth->keyOn(1 ,
                           //synthNoteNumber,
                           21,
                           21,
                           0,
                           0.02, //gain
                           1.,
                           Forward,
                           Normal, //FixedLength,
                           PedalNote,
                           0,
                           0,
                           0,
                           20000,
                           3,
                           3 );
    }
    
    void handleMidiEvent (const MidiMessage& m) override
    {
        synth->handleMidiEvent(m);
        resonanceSynth->handleMidiEvent(m);
        hammerSynth->handleMidiEvent(m);
        pedalSynth->handleMidiEvent(m);
        
    }
    
    inline void allNotesOff()
    {
        for(int i = 0; i < 15; i++)
        {
            synth->allNotesOff(i,true);
            resonanceSynth->allNotesOff(i,true);
            hammerSynth->allNotesOff(i,true);
            pedalSynth->allNotesOff(i,true);
        }
    }
    
    inline BKSynthesiser* getPedalSynth()
    {
        return pedalSynth;
    }
    void copyProcessorState(GenericProcessor::Ptr copy) override
    {
        
    }
    
private:
    BKSynthesiser::Ptr      synth;
    BKSynthesiser::Ptr  resonanceSynth;
    BKSynthesiser::Ptr      hammerSynth;
    BKSynthesiser::Ptr      pedalSynth;
    TuningProcessor::Ptr    tuner;
    
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

