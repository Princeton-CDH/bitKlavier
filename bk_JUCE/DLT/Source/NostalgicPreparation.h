//
//  NostalgicPreparation.h
//  MRM
//
//  Created by Daniel Trueman on 11/1/16.
//
//

#ifndef NostalgicPreparation_h
#define NostalgicPreparation_h

#include "../JuceLibraryCode/JuceHeader.h"
#include "BKSynthesiser.h"
#include "AudioConstants.h"

class NostalgicPreparation
{
public:
    //==============================================================================
    /** Creates a voice. */
    NostalgicPreparation();
    
    /** Destructor. */
    virtual ~NostalgicPreparation();
    
    void noteLengthTimerOn(int midiNoteNumber, float midiNoteVelocity);
    void noteLengthTimerOff(int midiNoteNumber);
    
    void reverseNoteLengthTimerOn(int midiNoteNumber, float noteLength);
    
    void playNote(int midiNoteNumber, int midiChannel);
    
    void processBlock(int numSamples, int midiChannel);
    void incrementTimers(int numSamples);
    
    void keymapSet(int midiNoteNumber, bool on);
    
    int getNoteLengthTimer(int midiNoteNumber) const noexcept;
    float getVelocity(int midiNoteNumber) const noexcept;
    int getReverseNoteLengthTimer(int midiNoteNumber) const noexcept;
    
    int getWaveDistance() const noexcept { return waveDistance; }
    int getUndertow() const noexcept { return undertow; }
    
    void attachToSynth(BKSynthesiser *synth);
    
    
protected:
    //==============================================================================
    
private:
    //==============================================================================
    Array<bool> keymap;
    
    Array<int> noteLengthTimers;
    Array<int> activeNotes;
    Array<float> velocities;
    
    Array<int> reverseLengthTimers;
    Array<int> activeReverseNotes;
    Array<int> reverseTargetLength; //in samples
    Array<float> undertowVelocities;
    
    int waveDistance; //ms
    int undertow;     //ms
    
    //need callbacks to set/get these....
    Array<float> tuningOffsets = Array<float>(aPartialTuning,aNumScaleDegrees);
    int tuningBasePitch = 0;
    
    BKSynthesiser *synth;
    double sampleRate;
    
    JUCE_LEAK_DETECTOR (NostalgicPreparation) //is this the right one to use here?
};

#endif /* NostalgicPreparation_h */
