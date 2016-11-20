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
    
    //begin timing played note length, called with noteOn
    void noteLengthTimerOn(int midiNoteNumber, float midiNoteVelocity);
    
    //finish timing played note length, called with noteOff
    void noteLengthTimerOff(int midiNoteNumber);
    
    //begin playing reverse note
    void playNote(int midiNoteNumber, int midiChannel);
    
    //begin timing reverse note play time
    void reverseNoteLengthTimerOn(int midiNoteNumber, float noteLength);
    
    //called with every audio vector
    void processBlock(int numSamples, int midiChannel);
    void incrementTimers(int numSamples);
    
    //call to turn on/off preparation for particular key; not yet implemented
    void keymapSet(int midiNoteNumber, bool on);
    
    //data retrieval callbacks
    int getNoteLengthTimer(int midiNoteNumber) const noexcept;
    float getVelocity(int midiNoteNumber) const noexcept;
    int getReverseNoteLengthTimer(int midiNoteNumber) const noexcept;  
    int getWaveDistance() const noexcept { return waveDistance; }
    int getUndertow() const noexcept { return undertow; }
    
    //call to synth in parent for actual playback
    void attachToSynth(BKSynthesiser *synth);
    
    
protected:
    //==============================================================================
    
private:
    //==============================================================================
    Array<bool> keymap; //table of prepared keys; not yet implemented
    
    Array<int> noteLengthTimers; //store current length of played notes here
    Array<int> activeNotes; //table of notes currently being played by player
    Array<float> velocities; //table of velocities played
    
    Array<int> reverseLengthTimers; //keep track of how long reverse notes have been playing
    Array<int> activeReverseNotes; //table of active reverse notes
    Array<int> reverseTargetLength; //target reverse length (in samples)
    Array<float> undertowVelocities; //velocities stored for undertow
    
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
