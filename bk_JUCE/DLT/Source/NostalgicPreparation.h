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
#include "AudioConstants.h"

class NostalgicPreparation
{
public:
    //==============================================================================
    /** Creates a voice. */
    NostalgicPreparation();
    
    /** Destructor. */
    virtual ~NostalgicPreparation();
    
    void startTimer(int midiNoteNumber, float midiNoteVelocity);
    int getTimer(int midiNoteNumber) const noexcept;
    float getVelocity(int midiNoteNumber) const noexcept;
    void clearNote(int midiNoteNumber);
    void incrementTimers(int numSamples);
    int getWaveDistance() const noexcept { return waveDistance; }
    int getUndertow() const noexcept { return undertow; }
    
    
protected:
    //==============================================================================
    
private:
    //==============================================================================
    Array<int> timers;
    Array<float> velocities;
    Array<int> activeNotes;
    int waveDistance; //ms
    int undertow;     //ms
    
    
    JUCE_LEAK_DETECTOR (NostalgicPreparation) //is this the right one to use here?
};

#endif /* NostalgicPreparation_h */
