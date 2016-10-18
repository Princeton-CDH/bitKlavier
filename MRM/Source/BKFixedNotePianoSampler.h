/*
  ==============================================================================

    BKFixedNotePianoSampler.h
    Created: 18 Oct 2016 11:54:49am
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef BKFIXEDNOTEPIANOSAMPLER_H_INCLUDED
#define BKFIXEDNOTEPIANOSAMPLER_H_INCLUDED


#include "../JuceLibraryCode/JuceHeader.h"

#include "BKSynthesiser.h"

#include "ReferenceCountedBuffer.h"

class   BKFixedNotePianoSamplerSound    : public BKSynthesiserSound
{
public:
    //==============================================================================
    /** Creates a sampled sound from an audio reader.
     
     This will attempt to load the audio from the source into memory and store
     it in this object.
     
     @param name         a name for the sample
     @param source       the audio to load. This object can be safely deleted by the
     caller after this constructor returns
     @param midiNotes    the set of midi keys that this sound should be played on. This
     is used by the BKSynthesiserSound::appliesToNote() method
     @param rootMidiNote   the midi note at which the sample should be played
     with its natural rate. All other notes will be pitched
     up or down relative to this one
     @param midiVelocities    the set of midi velocities that this sound should be played on. This
     is used by the BKSynthesiserSound::appliesToVelocity() method
     @param maxSampleLengthSeconds   a maximum length of audio to read from the audio
     source, in seconds
     */
    BKFixedNotePianoSamplerSound (const String& name,
                             AudioFormatReader& reader,
                             const BigInteger& midiNotes,
                             int rootMidiNote,
                             const BigInteger& midiVelocities,
                             double maxSampleLengthSeconds);
    
    /** Destructor. */
    ~BKFixedNotePianoSamplerSound();
    
    //==============================================================================
    /** Returns the sample's name */
    const String& getName() const noexcept                  { return name; }
    
    /** Returns the audio sample data.
     This could return nullptr if there was a problem loading the data.
     */
    
    AudioSampleBuffer* getAudioData() const noexcept { return data; }
    
    //==============================================================================
    bool appliesToNote (int midiNoteNumber) override;
    bool appliesToVelocity (int midiNoteVelocity) override;
    bool appliesToChannel (int midiChannel) override;
    
    
private:
    //==============================================================================
    friend class BKFixedNotePianoSamplerVoice;
    
    String name;
    
    ScopedPointer<AudioSampleBuffer> data;
    
    double sourceSampleRate;
    BigInteger midiNotes;
    BigInteger midiVelocities;
    int length;
    int rampOnSamples, rampOffSamples;
    int midiRootNote;
    
    JUCE_LEAK_DETECTOR (BKFixedNotePianoSamplerSound)
};


//==============================================================================
/**
 A subclass of BKSynthesiserVoice that can play a BKFixedNotePianoSamplerSound.
 
 To use it, create a BKSynthesiser, add some BKFixedNotePianoSamplerVoice objects to it, then
 give it some BKFixedNotePianoSamplerSound objects to play.
 
 @see BKFixedNotePianoSamplerSound, BKSynthesiser, BKSynthesiserVoice
 */
class  BKFixedNotePianoSamplerVoice    : public BKSynthesiserVoice
{
public:
    //==============================================================================
    /** Creates a BKFixedNotePianoSamplerVoice. */
    BKFixedNotePianoSamplerVoice();
    
    /** Destructor. */
    ~BKFixedNotePianoSamplerVoice();
    
    //==============================================================================
    bool canPlaySound (BKSynthesiserSound*) override;
    
    void startNote (int midiNoteNumber, float velocity, BKSynthesiserSound*, int pitchWheel) override;
    void stopNote (float velocity, bool allowTailOff) override;
    
    void pitchWheelMoved (int newValue) override;
    void controllerMoved (int controllerNumber, int newValue) override;
    
    void renderNextBlock (AudioSampleBuffer&, int startSample, int numSamples) override;
    
    
private:
    //==============================================================================
    double pitchRatio;
    double sourceSamplePosition;
    float lgain, rgain, rampOnOffLevel, rampOnDelta, rampOffDelta;
    bool isInRampOn, isInRampOff;
    
    JUCE_LEAK_DETECTOR (BKFixedNotePianoSamplerVoice)
};


#endif  // BKFIXEDNOTEPIANOSAMPLER_H_INCLUDED
