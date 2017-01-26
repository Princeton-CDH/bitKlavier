
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

#include "PreparationsMap.h"

#include "Piano.h"

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
  
    // General settings.
    GeneralSettings::Ptr                general;
    
    // Synthesisers.
    BKSynthesiser                       mainPianoSynth;
    BKSynthesiser                       hammerReleaseSynth;
    BKSynthesiser                       resonanceReleaseSynth;
    
    // PreparationsMaps
    PreparationsMap::CSPtrArr           prepMaps;
    PreparationsMap::Ptr                currentPrepMap;
    
    // Preparations.
    SynchronicPreparation::CSPtrArr     sPreparation;
    NostalgicPreparation::CSPtrArr      nPreparation;
    DirectPreparation::CSPtrArr         dPreparation;
    TuningPreparation::CSPtrArr         tPreparation;
    
    // Keymaps.
    Keymap::PtrArr                      bkKeymaps;
    
    // Active Maps and Pianos
    PreparationsMap::CSPtrArr activePrepMaps;   //this array is what a Piano stores
    Piano::Ptr currentPiano;

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
    
    PreparationsMap::Ptr setCurrentPrepMap(int which);
    
    inline PreparationsMap::Ptr getCurrentPrepMap() const { return currentPrepMap;}

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
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKAudioProcessor)
};


#endif  // PLUGINPROCESSOR_H_INCLUDED
