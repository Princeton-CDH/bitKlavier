/*
  ==============================================================================

    Effects.h
    Created: 12 Sep 2022 10:50:04am
    Author:  Davis Polito

  ==============================================================================
*/

#include "BKUtilities.h"
#include "General.h"
#include "Keymap.h"

#pragma once

// Forward declaration to allow include of EffectProcessor in BKSynthesiser
class BKSynthesiser;

typedef enum EffectType {
    EffectBlendronic
} EffectType;
class EffectProcessor : public ReferenceCountedObject
{
public:
    typedef ReferenceCountedObjectPtr<EffectProcessor>   Ptr;
    typedef Array<EffectProcessor::Ptr>                  PtrArr;
    typedef Array<EffectProcessor::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<EffectProcessor>                  Arr;
    typedef OwnedArray<EffectProcessor, CriticalSection> CSArr;
    EffectProcessor(GeneralSettings::Ptr bGeneral, BKSynthesiser* bMain, EffectType type, TempoProcessor::Ptr tempo) : effectActive(true), keymaps(Keymap::PtrArr()), general(bGeneral), synth(bMain), type(type), tempo(tempo)
    {
        
    }
    //begin timing played note length, called with noteOn
    virtual void keyPressed(int noteNumber, Array<float>& targetVelocities, bool fromPress) =0;

    //begin playing reverse note, called with noteOff
    virtual void keyReleased(int noteNumber, Array<float>& targetVelocities, bool fromPress)=0;
    inline void addKeymap(Keymap::Ptr keymap)
    {
        keymaps.add(keymap);
    }
    inline Keymap::PtrArr getKeymaps(void)
    {
        return keymaps;
    }
    virtual void setClearDelayOnNextBeat(bool clear)=0;
    virtual void postRelease(int noteNumber, int midiChannel)=0;
    virtual void prepareToPlay(double sr)=0;
    virtual int getId(void) const noexcept=0;
    const bool getActive() const noexcept {return effectActive;}
    virtual void tick(float* outputs)=0;
    inline void setActive(bool newActive) { effectActive = newActive; }
    inline void toggleActive(
                             ) { effectActive = !effectActive; }
    inline EffectType getType() const noexcept {return type;}
    inline BKSynthesiser* getSynth(void) const noexcept { return synth; }
    Array<Array<float>>& getVelocities() { return velocities; }
    Array<Array<float>>& getInvertVelocities() { return invertVelocities; }
    virtual void reset()=0;
    inline void setTempo(TempoProcessor::Ptr temp) { tempo = temp; }
    void setVelocities(Array<Array<float>>& newVel) { velocities = newVel; }
    void setInvertVelocities(Array<Array<float>>& newVel) { invertVelocities = newVel; }
private:
    Array<Array<float>> velocities;
    Array<Array<float>> invertVelocities;
    Keymap::PtrArr          keymaps;
protected:
    bool effectActive;
    GeneralSettings::Ptr    general;
    BKSynthesiser*          synth;
    TempoProcessor::Ptr tempo;
    CriticalSection lock;
    EffectType type;
};
