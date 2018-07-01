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
#include "General.h"

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
                         int transpose,
                         const BigInteger& midiVelocities,
                         sfzero::Region* region = nullptr);

    
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
    bool isSoundfontSound(void);
    
private:
    //==============================================================================
    friend class BKPianoSamplerVoice;
    
    String name;
    
    BKReferenceCountedBuffer::Ptr data;
    
    double sourceSampleRate;
    BigInteger midiNotes;
    BigInteger midiVelocities;
    uint64 soundLength;
    int midiRootNote, transpose;
    int rampOnSamples, rampOffSamples;

    int64 loopStart, loopEnd;
    int64 delayS, holdS;
    int64 start, end;
    float attack,decay,sustain,release, hold, delay; // for sfz
    int loopMode; // sample_loop = 0, no_loop, one_shot, loop_continuous, loop_sustain
    /*
     0
     1 indicates a sound reproduced with no loop,
     2 is unused but should be interpreted as indicating no loop, and
     3 indicates a sound which loops for the duration of key depression then proceeds to play the remainder of the sample
     */
    
    bool isSoundfont;
    
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
    BKPianoSamplerVoice(GeneralSettings::Ptr);
    
    /** Destructor. */
    ~BKPianoSamplerVoice();
    
    //==============================================================================
    
    //GeneralSettings::Ptr generalSettings;
    
    bool canPlaySound (BKSynthesiserSound*) override;
    
    void startNote (
                    float midiNoteNumber,
                    float velocity,
                    PianoSamplerNoteDirection direction,
                    PianoSamplerNoteType type,
                    BKNoteType bktype,
                    uint64 startingPosition,
                    uint64 length,
                    uint64 voiceRampOn,
                    uint64 voiceRampOff,
                    BKSynthesiserSound* sound
                    ) override;
    
    void startNote (
                    float midiNoteNumber,
                    int pitchWheelValue,
                    float velocity,
                    PianoSamplerNoteDirection direction,
                    PianoSamplerNoteType type,
                    BKNoteType bktype,
                    uint64 startingPosition,
                    uint64 length,
                    uint64 adsrAttack,
                    uint64 adsrDecay,
                    float adsrSustain,
                    uint64 adsrRelease,
                    BKSynthesiserSound* sound
                    ) override;
    
    void stopNote (float velocity, bool allowTailOff) override;
    
    void pitchWheelMoved (int newValue) override;
    void controllerMoved (int controllerNumber, int newValue) override;
    
    void renderNextBlock (AudioSampleBuffer&, int startSample, int numSamples) override;
    
    
    void processPiano(AudioSampleBuffer& outputBuffer,
                      int startSample, int numSamples,
                      const BKPianoSamplerSound* playingSound);
    
    void processSoundfontNoLoop(AudioSampleBuffer& outputBuffer,
                                 int startSample, int numSamples,
                                 const BKPianoSamplerSound* playingSound);
    
    void processSoundfontLoop(AudioSampleBuffer& outputBuffer,
                                int startSample, int numSamples,
                                const BKPianoSamplerSound* playingSound);
    
    
private:
    //==============================================================================
    int noteNumber;
    int layer;
    float noteVelocity;
    uint64 noteStartingPosition, noteEndPosition;
    
    double pitchRatio;
    double pitchbendMultiplier;
    double bentRatio;
    
    double sourceSamplePosition;
    
    double fadeTracker,fadeOffset;
    double lengthTracker, lengthEnv;

    double playEndPosition;
    double playLength, playLengthSF2;
    uint64 timer;
    BKNoteType bkType;
    PianoSamplerNoteType playType;
    PianoSamplerNoteDirection playDirection;
    bool revRamped;
    float lgain, rgain, rampOnOffLevel, rampOnDelta, rampOffDelta;
    bool isInRampOn, isInRampOff;
    
    double offset;
    double cfSamples; // number of samples for crossfade
    
    //double beginPosition, loopPosition, endPosition;
    stk::Envelope sampleEnv, loopEnv;
    
    stk::Envelope renv1,renv2,renv3;
    float renvVal;

    bool sfzEnvApplied;
    
    double samplePosition, loopPosition;
    
    stk::ADSR adsr;
    stk::AHDSR sfzadsr;
    
    bool lastRamp;
    int numLoops;
    
    bool inLoop;
    stk::ADSR lastIn, lastOut;
    
    
    
    
    
    
    
    
    
    
    JUCE_LEAK_DETECTOR (BKPianoSamplerVoice)
};


#endif  // BKPIANOSAMPLER_H_INCLUDED
