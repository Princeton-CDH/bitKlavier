
#ifndef PLUGINPROCESSOR_H_INCLUDED
#define PLUGINPROCESSOR_H_INCLUDED

#include "BKUtilities.h"

#include "BKSampleLoader.h"

#include "BKSynthesiser.h"

#include "BKUpdateState.h"

#include "Preparation.h"

#include "Keymap.h"

#include "Tuning.h"

#include "Tempo.h"

#include "General.h"

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
  
    ScopedPointer<XmlElement>  saveGallery(void);
    void loadGalleryFromXml(ScopedPointer<XmlElement> xml);
    void loadGalleryFromPath(String path);
    void loadGalleryDialog(void);
    void loadJsonGalleryDialog(void);
    void loadJsonGalleryFromPath(String path);
    void loadJsonGalleryFromVar(var myJson);
    
    // General settings.
    GeneralSettings::Ptr                general;
    BKUpdateState::Ptr                  updateState;
    
    // Synthesisers.
    BKSynthesiser                       mainPianoSynth;
    BKSynthesiser                       hammerReleaseSynth;
    BKSynthesiser                       resonanceReleaseSynth;
    
    // Preparations.
    Tuning::PtrArr                      tuning;
    Tempo::PtrArr                       tempo;
    
    // Keymaps.
    Keymap::PtrArr                      bkKeymaps;
    
    // Nostalgic, Synchronic, Direct classes
    Synchronic::PtrArr                  synchronic;
    Nostalgic::PtrArr                   nostalgic;
    Direct::PtrArr                      direct;
    
    SynchronicModPreparation::PtrArr    modSynchronic;
    DirectModPreparation::PtrArr        modDirect;
    NostalgicModPreparation::PtrArr     modNostalgic;
    TuningModPreparation::PtrArr        modTuning;
    TempoModPreparation::PtrArr         modTempo;
    
    Piano::Ptr                          prevPiano;
    Piano::Ptr                          currentPiano;
    Piano::PtrArr                       prevPianos;
    Piano::PtrArr                       bkPianos;
    
    StringArray                         galleryNames;
    String                              currentGallery;
    
    void updateGalleries(void);
    
    void collectGalleries(void);
    
    void addSynchronic(void);
    void addSynchronic(SynchronicPreparation::Ptr);
    int addSynchronicIfNotAlreadyThere(SynchronicPreparation::Ptr);
    
    void addNostalgic(void);
    void addNostalgic(NostalgicPreparation::Ptr);
    int addNostalgicIfNotAlreadyThere(NostalgicPreparation::Ptr);
    
    void addTuning(void);
    void addTuning(TuningPreparation::Ptr);
    int addTuningIfNotAlreadyThere(TuningPreparation::Ptr);
    
    void addTempo(void);
    void addTempo(TempoPreparation::Ptr);
    int addTempoIfNotAlreadyThere(TempoPreparation::Ptr);
    
    void addDirect(void);
    void addDirect(DirectPreparation::Ptr);
    int addDirectIfNotAlreadyThere(DirectPreparation::Ptr);
    
    void addPiano(void);
    
    
    void addKeymap(void);
    void addKeymap(Keymap::Ptr);
    void addDirectMod(void);
    void addNostalgicMod(void);
    void addSynchronicMod(void);
    void addTuningMod(void);
    void addTuningMod(TuningModPreparation::Ptr tmod);
    void addTempoMod(void);
    void addTempoMod(TempoModPreparation::Ptr tmod);
    
    void updateUI(void);
    
    /*
    //change these to pointers, pass these to Preparations, remove extra layer of checks in PreparationMap
    bool                                pianoDidChange;
    bool                                directPreparationDidChange;
    bool                                nostalgicPreparationDidChange;
    bool                                synchronicPreparationDidChange;
    bool                                tuningPreparationDidChange;
    bool                                generalSettingsDidChange;
    */
    
    Array<int>                          noteOn;
    
    int                                 noteOnCount;
    bool                                allNotesOff;

    SynchronicProcessor::Ptr getSynchronicProcessor(int id);

    int channel;

    // Change listener callback implementation
    void changeListenerCallback(ChangeBroadcaster *source) override;
    
    
    //==============================================================================
    void loadPianoSamples(BKSampleLoadType type);
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool setPreferredBusArrangement (bool isInput, int bus, const AudioChannelSet& preferredSet) override;
   #endif

    void processBlock (AudioSampleBuffer&, MidiBuffer&) override;
    
    void  setCurrentPiano(int which);
    void  performModifications(int noteNumber);
    void  performResets(int noteNumber);

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
    int  currentPianoId;
    
    double bkSampleRate;
    
    bool didLoadHammersAndRes, didLoadMainPianoSamples;
    
    void loadMainPianoSamples(BKSynthesiser *synth, int numLayers);
    void loadResonanceRelaseSamples(BKSynthesiser *synth);
    void loadHammerReleaseSamples(BKSynthesiser *synth);
    
    Array<float> tempoAlreadyLoaded;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKAudioProcessor)
};


#endif  // PLUGINPROCESSOR_H_INCLUDED
