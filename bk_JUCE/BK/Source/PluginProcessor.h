
#ifndef PLUGINPROCESSOR_H_INCLUDED
#define PLUGINPROCESSOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

#include "ReferenceCountedBuffer.h"

#include "Synchronic.h"
#include "Nostalgic.h"

#include "AudioConstants.h"

#include "BKSynthesiser.h"

#include "BKTextField.h"

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
    
    
    int numSynchronicLayers;
    int currentSynchronicLayer;

    // Public instance varables.
    // MidiInput midiInput;
    AudioFormatManager formatManager;
    
    ScopedPointer<AudioFormatReader> sampleReader;
    ScopedPointer<AudioSampleBuffer> sampleBuffer;
    
    BKSynthesiser mainPianoSynth;
#if USE_SECOND_SYNTH
    BKSynthesiser secondPianoSynth;
#endif
    
    SynchronicPreparation::Ptr sPrep;
    ScopedPointer<SynchronicProcessor> sProcess;
    
    NostalgicProcessor nostalgic;
    
    BKSynthesiser hammerReleaseSynth;
    
    BKSynthesiser resonanceReleaseSynth;
    
    OwnedArray<SynchronicProcessor, CriticalSection> synchronic;
    
    int channel;
    
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
    void loadMainPianoSamples(BKSynthesiser *synth, int numLayers);
    void loadResonanceRelaseSamples(BKSynthesiser *synth);
    void loadHammerReleaseSamples(BKSynthesiser *synth);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MrmAudioProcessor)
};


#endif  // PLUGINPROCESSOR_H_INCLUDED
