
#ifndef PLUGINPROCESSOR_H_INCLUDED
#define PLUGINPROCESSOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

#include "ReferenceCountedBuffer.h"

#include "Preparations.h"

#include "AudioConstants.h"

#include "BKSynthesiser.h"

#define USE_SECOND_SYNTH 0
#define USE_SYNCHRONIC_TWO 0

//==============================================================================
/**
*/
class MrmAudioProcessor  : public AudioProcessor,
                           public ChangeListener
{
    
public:
    //==============================================================================
    MrmAudioProcessor();
    ~MrmAudioProcessor();

    // Public instance varables.
    // MidiInput midiInput;
    AudioFormatManager formatManager;
    
    ScopedPointer<AudioFormatReader> sampleReader;
    ScopedPointer<AudioSampleBuffer> sampleBuffer;
    
    BKSynthesiser mainPianoSynth;
#if USE_SECOND_SYNTH
    BKSynthesiser secondPianoSynth;
#endif
    
    BKSynthesiser hammerReleaseSynth;
    
    BKSynthesiser resonanceReleaseSynth;
    
    ReferenceCountedArray<ReferenceCountedBuffer, CriticalSection> sampleBuffers;

    SynchronicProcessor synchronic1, synchronic2;
    
    int channel;
    //ScopedArray<AudioSampleBuffer> sampleBuffers;
    int position;
    bool off,end,ramp;
    bool lastnotetype;
    float decay,val;
    int samplesDecay;
    int decayCount;
    
    // Change listener callback implementation
    void changeListenerCallback(ChangeBroadcaster *source) override;
    
    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool setPreferredBusArrangement (bool isInput, int bus, const AudioChannelSet& preferredSet) override;
   #endif

    void processBlock (AudioSampleBuffer&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MrmAudioProcessor)
};


#endif  // PLUGINPROCESSOR_H_INCLUDED
