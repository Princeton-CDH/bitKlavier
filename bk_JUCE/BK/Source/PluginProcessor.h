
#ifndef PLUGINPROCESSOR_H_INCLUDED
#define PLUGINPROCESSOR_H_INCLUDED

#include "BKUtilities.h"

#include "BKSampleLoader.h"

#include "BKSynthesiser.h"

#include "Synchronic.h"

#include "Nostalgic.h"

#include "Direct.h"

#include "Keymap.h"

#include "Tuning.h"

#include "General.h"

#include "Layer.h"

#define USE_SYNCHRONIC_TWO 0

//==============================================================================
/**
*/
class BKAudioProcessor  : public AudioProcessor,
                           public ChangeListener,
                            public BKSampleLoader
{
    
public:
    //==============================================================================
    BKAudioProcessor();
    ~BKAudioProcessor();
    
    Layer::Ptr currentLayer;
    
    // General settings.
    GeneralSettings::Ptr general;
    
    // Tuning.
    TuningProcessor::Ptr tuner;
    
    // Synthesisers.
    BKSynthesiser mainPianoSynth;
    BKSynthesiser hammerReleaseSynth;
    BKSynthesiser resonanceReleaseSynth;

    // Processors.
    SynchronicProcessor::CSArr      sProcessor;
    NostalgicProcessor::Arr         nProcessor;
    DirectProcessor::Arr            dProcessor;
    
    
    // Preparations.
    SynchronicPreparation::CSArr    sPreparation;
    NostalgicPreparation::CSArr     nPreparation;
    DirectPreparation::CSArr        dPreparation;
    
    // Keymaps.
    Keymap::CSArr                   bkKeymaps;
    
    int numSynchronicLayers, numNostalgicLayers, numDirectLayers;
    
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
    
    TuningSystem mainTuning = EqualTemperament;
    PitchClass tuningBasePitch = C;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKAudioProcessor)
};


#endif  // PLUGINPROCESSOR_H_INCLUDED
