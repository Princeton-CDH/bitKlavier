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
    int getTimer(int midiNoteNumber);
    float getVelocity(int midiNoteNumber);
    void clearNote(int midiNoteNumber);
    void incrementTimer(int numSamples);
    
    
protected:
    //==============================================================================
    
private:
    //==============================================================================
    Array<int> timers;
    Array<float> velocities;
    Array<int> activeNotes;
    
    
    JUCE_LEAK_DETECTOR (NostalgicPreparation)
};

#endif /* NostalgicPreparation_h */
