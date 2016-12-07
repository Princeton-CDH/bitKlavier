/*
 ==============================================================================
 
 BKPianoSampler.h
 Created: 19 Oct 2016 10:00:04am
 Author:  Michael R Mulshine
 
 ==============================================================================
 */

#ifndef BKPIANOSAMPLER_H_INCLUDED
#define BKPIANOSAMPLER_H_INCLUDED


#include "../JuceLibraryCode/JuceHeader.h"
#include "BKSynthesiser.h"
#include "BKReferenceCountedBuffer.h"
#include "AudioConstants.h"

class   BKPianoSamplerSound    : public BKSynthesiserSound
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
    BKPianoSamplerSound (const String& name,
                         BKReferenceCountedBuffer::Ptr buffer,
                         uint64 soundLength,
                         double sourceSampleRate,
                         const BigInteger& midiNotes,
                         int rootMidiNote,
                         const BigInteger& midiVelocities);
    
    /** Destructor. */
    ~BKPianoSamplerSound();
    
    //==============================================================================
    /** Returns the sample's name */
    const String& getName() const noexcept                  { return name; }
    
    /** Returns the audio sample data.
     This could return nullptr if there was a problem loading the data.
     */
    
    AudioSampleBuffer* getAudioData() const noexcept { return data->getAudioSampleBuffer(); }
    
    //==============================================================================
    bool appliesToNote (int midiNoteNumber) override;
    bool appliesToVelocity (int midiNoteVelocity) override;
    bool appliesToChannel (int midiChannel) override;
    
private:
    //==============================================================================
    friend class BKPianoSamplerVoice;
    
    String name;
    
    BKReferenceCountedBuffer::Ptr data;
    
    double sourceSampleRate;
    BigInteger midiNotes;
    BigInteger midiVelocities;
    uint64 soundLength;
    int midiRootNote;
    int rampOnSamples, rampOffSamples;
    
    JUCE_LEAK_DETECTOR (BKPianoSamplerSound)
};


//==============================================================================
/**
 A subclass of BKSynthesiserVoice that can play a BKPianoSamplerSound.
 
 To use it, create a BKSynthesiser, add some BKPianoSamplerVoice objects to it, then
 give it some BKPianoSamplerSound objects to play.
 
 @see BKPianoSamplerSound, BKSynthesiser, BKSynthesiserVoice
 */
class  BKPianoSamplerVoice    : public BKSynthesiserVoice
{
public:
    //==============================================================================
    /** Creates a BKPianoSamplerVoice. */
    BKPianoSamplerVoice();
    
    /** Destructor. */
    ~BKPianoSamplerVoice();
    
    //==============================================================================
    bool canPlaySound (BKSynthesiserSound*) override;
    
    void startNote (
                    float midiNoteNumber,
                    float velocity,
                    PianoSamplerNoteDirection direction,
                    PianoSamplerNoteType type,
                    BKNoteType bktype,
                    uint64 startingPosition,
                    uint64 length,
                    int voiceRampOn,
                    int voiceRampOff,
                    BKSynthesiserSound* sound
                    ) override;
    
    void stopNote (float velocity, bool allowTailOff) override;
    
    void pitchWheelMoved (int newValue) override;
    void controllerMoved (int controllerNumber, int newValue) override;
    
    void renderNextBlock (AudioSampleBuffer&, int startSample, int numSamples) override;
    
    
    
    
private:
    //==============================================================================
    int noteNumber;
    float noteVelocity;
    uint64 noteStartingPosition, noteEndPosition;
    double pitchRatio;
    double sourceSamplePosition;
    double playEndPosition;
    uint32 playLength;
    uint64 timer;
    int maxPulses, numPulses;
    BKNoteType bkType;
    PianoSamplerNoteType playType;
    PianoSamplerNoteDirection playDirection;
    float lgain, rgain, rampOnOffLevel, rampOnDelta, rampOffDelta;
    bool isInRampOn, isInRampOff;
    
    JUCE_LEAK_DETECTOR (BKPianoSamplerVoice)
};


#endif  // BKPIANOSAMPLER_H_INCLUDED
