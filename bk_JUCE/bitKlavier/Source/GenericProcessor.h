/*
  ==============================================================================

    GenericProcessor.h
    Created: 19 Oct 2022 1:49:29pm
    Author:  Davis Polito

  ==============================================================================
*/

#pragma once
#include "BKUtilities.h"
#include "General.h"
#include "Keymap.h"
//#include "Piano.h"
#include "AudioConstants.h"
class GenericProcessor : public ReferenceCountedObject
{
    
public:
    typedef ReferenceCountedObjectPtr<GenericProcessor>   Ptr;
    typedef Array<GenericProcessor::Ptr>                  PtrArr;
    typedef Array<GenericProcessor::Ptr, CriticalSection> CSPtrArr;
    typedef OwnedArray<GenericProcessor>                  Arr;
    typedef OwnedArray<GenericProcessor, CriticalSection> CSArr;
    
    
    GenericProcessor(BKPreparationType type, KeymapTargetType start, KeymapTargetType end) : targetTypeStart(start), type(type), targetTypeEnd(end)
    {
        
    }
    
    ~GenericProcessor()
    {}
    
    BKSampleLoadType sampleType;
    virtual void processBlock(AudioSampleBuffer& buffer, MidiBuffer& midiMessages, int numSamples, int midiChannel, BKSampleLoadType type) = 0;
    
    virtual void    keyPressed(int noteNumber, Array<float>& targetVelocities, bool fromPress){}
    virtual void    keyReleased(int noteNumber, Array<float>& targetVelocities, bool fromPress){}
//    virtual void    playReleaseSample(int noteNumber, Array<float>& targetVelocities,
//                                      bool fromPress, bool soundfont = false) {return;};
    
    virtual void prepareToPlay(GeneralSettings::Ptr gen) = 0;
    
    
    virtual int getId(void) const noexcept = 0;
    
    
    
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
    virtual float filterVelocity(float vel)
    {
        return vel;
    };
    void resetLastVelocity() { lastVelocityInRange = false; }
    
    Array<Array<float>>& getVelocities() { return velocities; }
    Array<Array<float>>& getInvertVelocities() { return invertVelocities; }
    
    void setVelocities(Array<Array<float>>& newVel) { velocities = newVel; }
    void setInvertVelocities(Array<Array<float>>& newVel) { invertVelocities = newVel; }
   
    
    virtual void handleMidiEvent (const MidiMessage& m) = 0;
    
    inline BKPreparationType getType() const noexcept {return type;}
    
   
    virtual void copyProcessorState(GenericProcessor::Ptr) = 0;
    void pianoSwitched(Array<GenericProcessor::PtrArr> prevPiano) {}
    virtual void reset() = 0;
    virtual void sustainPedalPressed() {} //hack for direct fix later
    virtual void allNotesOff(){}
//    void handleKeyPress(int noteNumber, Array<float> pressTargetVelocities,
//                        Array<float> releaseTargetVelocities, bool fromPress)
//    {
//
//    }
    KeymapTargetType targetTypeStart;
    KeymapTargetType targetTypeEnd;
protected:
    Keymap::PtrArr      keymaps;
    BKPreparationType type;
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
private:

    
    
    
    JUCE_LEAK_DETECTOR(GenericProcessor);
};
