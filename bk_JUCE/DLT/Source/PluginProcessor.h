
#ifndef PLUGINPROCESSOR_H_INCLUDED
#define PLUGINPROCESSOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

#include "ReferenceCountedBuffer.h"

#include "BKSynthesiser.h"



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

    BKSynthesiser mainPianoSynth;
    BKSynthesiser secondaryPianoSynth;
    
    BKSynthesiser hammerReleaseSynth;
    BKSynthesiser resonanceReleaseSynth;
    
    ScopedPointer<AudioFormatReader> sampleReader;
    ScopedPointer<AudioSampleBuffer> sampleBuffer;
    
    ReferenceCountedArray<ReferenceCountedBuffer, CriticalSection> sampleBuffers;
    
    //ScopedArray<AudioSampleBuffer> sampleBuffers;
    int position;
    bool off,end,ramp;
    bool lastnotetype;
    float decay,val;
    int samplesDecay;
    int decayCount;
    
    BigInteger sampleCounter; 
    double currentTime;
    
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
