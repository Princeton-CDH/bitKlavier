
#ifndef PLUGINPROCESSOR_H_INCLUDED
#define PLUGINPROCESSOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

#include "BKUtilities.h"

#include "BKGalleryLoader.h"

#include "BKSampleLoader.h"

#include "BKSynthesiser.h"

#include "BKUpdateState.h"

#include "Keymap.h"

#include "Tuning.h"

#include "Tempo.h"

#include "General.h"

#include "Piano.h"

#include "Gallery.h"

#include "ItemMapper.h"

class StandalonePluginHolder;
class BKAudioProcessorEditor;


//==============================================================================
/**
*/
class BKAudioProcessor : public AudioProcessor,
                         public ChangeListener,
                         public MidiInputCallback
{
    
public:
    //==============================================================================
    BKAudioProcessor();
    ~BKAudioProcessor();
    
    StandalonePluginHolder* getPluginHolder(void);
    AudioDeviceManager* getAudioDeviceManager(void);
    AudioProcessorPlayer* getAudioProcessorPlayer(void);
    double getCurrentSampleRate(void);
    
    void addMidiInputDeviceCallback(MidiInputCallback* callback);
    void removeMidiInputDeviceCallback(MidiInputCallback* callback);
    
    void loadGalleries(void);
    
    void loadGalleryDialog(void);
    void loadJsonGalleryDialog(void);
    void loadGalleryFromPath(String path);
    void loadGalleryFromXml(XmlElement* xml);
    void loadJsonGalleryFromPath(String path);
    void saveCurrentGalleryAs(void);
    void saveCurrentGallery(void);
    void createNewGallery(String name, std::shared_ptr<XmlElement> xml = nullptr);
    void renameGallery(String name);
    void duplicateGallery(String name);
    void deleteGallery(void);
    
    
    void importCurrentGallery(void);
    void exportCurrentGallery(void);
    
    void importSoundfont(void);
    
    void writeCurrentGalleryToURL(String url);
    void deleteGalleryAtURL(String url);
    
    String firstGallery(void);
    void initializeGallery(void);
    
    FileChooser* fc;
    
    Gallery::Ptr                        gallery;
    
    BKUpdateState::Ptr                  updateState;

    void loadSFZ(File sfzFile);
    
    void loadSF2(File sfzFile);
    
    void openSoundfont(void);
    
    ThreadPool loader;
    
    HashMap<int, sfzero::Region::PtrArr> regions;
    
    AudioFormatManager formatManager;
    std::unique_ptr<AudioFormatReader> sampleReader;
    std::unique_ptr<AudioSampleBuffer> sampleBuffer;
    
    // Synthesisers.
    BKSynthesiser                       mainPianoSynth;
    BKSynthesiser                       hammerReleaseSynth;
    BKSynthesiser                       resonanceReleaseSynth;
    BKSynthesiser                       pedalSynth;
    
    //sfzero::Synth                       synth;
    
    Piano::Ptr                          prevPiano;
    Piano::Ptr                          currentPiano;
    Piano::PtrArr                       prevPianos;
    
    StringArray mikroetudes, ns_etudes, bk_examples;
    
    StringArray                         galleryNames;
    String                              currentGallery;
    
    // Full path names of soundfonts in no particular order
    StringArray                         soundfontNames;
    
    // Names of soundfont instruments keyed by sound set ids
    HashMap<int, StringArray>           instrumentNames;
    
    OwnedArray<StringArray>             exportedPreparations;
    StringArray                         exportedPianos;
        
    BKSampleLoadType                    loadingSampleType;
    String                              loadingSoundfont;
    int                                 loadingInstrument;
    
    BKSampleLoadType                    globalSampleType;
    String                              globalSoundfont;
    int                                 globalInstrument;
    
    String                              loadingSoundSet;
    int                                 loadingSoundSetId;
    StringArray                         loadedSoundSets;
    
    int                                 globalSoundSetId;
    
    bool firstTime;
    
    bool                                defaultLoaded;
    String                              defaultName;

    void updateGalleries(void);
    
    void collectGalleries(void);
    void collectPianos(void);
    void collectPreparations(void);
    void collectSoundfonts(void);
    
    void collectGalleriesFromFolder(File folder);
    void collectPianosFromFolder(File folder);
    void collectPreparationsFromFolder(File folder);
    void collectSoundfontsFromFolder(File folder);
    
    void updateUI(void);
    
    /*
    void memoryWarningReceived() override
    {
        DBG("using too much memory!");
    }
     */
    
    Array<bool>                         noteOn; //which notes are on, for the UI
    Array<bool>                         getNoteOns() { return noteOn; }
    Array<float>                        noteVelocity;
    
    void                                noteOnUI (int noteNumber) { if(didLoadMainPianoSamples) notesOnUI.add(noteNumber); }
    void                                noteOffUI(int noteNumber) { if(didLoadMainPianoSamples) notesOffUI.add(noteNumber); }
    
    int                                 noteOnCount;
    bool                                allNotesOff;
    
    int count;
    
    int note;
    int notes[3];
    int times[3];

    int channel;

    // Change listener callback implementation
    void changeListenerCallback(ChangeBroadcaster *source) override;
    
    void handleIncomingMidiMessage(MidiInput* source, const MidiMessage& m) override;
    
    inline const Array<String> getDefaultMidiInputSources(void) { return defaultMidiInputSources; }
    inline void setDefaultMidiInputSources(Array<String> sources) { defaultMidiInputSources = sources; }
    
    //==============================================================================
    int loadSamples(BKSampleLoadType type, String path ="", int subsound=0, bool updateGlobalSet=true);
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool setPreferredBusArrangement (bool isInput, int bus, const AudioChannelSet& preferredSet) override;
   #endif

    void processBlock (AudioSampleBuffer&, MidiBuffer&) override;
    
    void  setCurrentPiano(int which);
    void  performModifications(int noteNumber, String source);
    void  performResets(int noteNumber, String source);
    
    void handleNoteOn(int noteNumber, float velocity, int channel, String source);
    void handleNoteOff(int noteNumber, float velocity, int channel, String source);

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;
    AudioProcessorEditor* getEditor();

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
    
    double getLevelL();
    double getLevelR();

    BKItem::PtrArr clipboard;
    
    inline void setClipboard(BKItem::PtrArr items)
    {
        clipboard = items;
    }
    
    inline BKItem::PtrArr getClipboard(void)
    {
        return clipboard;
    }

    
    inline bool clipboardContains(BKItem::Ptr thisItem)
    {
        for (auto item : clipboard)
        {
            if (item->getType() == thisItem->getType() && item->getId() == thisItem->getId()) return true;
        }
        return false;
    }
    
    void reset(BKPreparationType type, int Id);
    void clear(BKPreparationType type, int Id);
    
    BKPlatform      platform;

    float uiScaleFactor;
    
    
    float paddingScalarX;
    float paddingScalarY;
    
    int screenWidth;
    int screenHeight;
    
    double progress;
    double progressInc;
    bool didLoadHammersAndRes, didLoadMainPianoSamples;
    
    void clearBitKlavier(void);
    
    void loadSamplesStartup(void);

    inline void setSustainInversion(bool sus)
    {
        sustainInverted = sus;
        
        if (sustainInverted)
        {
            if(sustainIsDown)
            {
                sustainIsDown = false;
                
                currentPiano->prepMap->sustainPedalReleased();
                
                if(prevPiano != currentPiano)
                {
                    DBG("sustain inverted, sustain is now released");
                    prevPiano->prepMap->sustainPedalReleased(true);
                }
            }
            else
            {
                sustainIsDown = true;
                DBG("sustain inverted, sustain is now pressed");
                
                if (firstTime) {firstTime = false; return;}
                else currentPiano->prepMap->sustainPedalPressed();
            }
        }
        else
        {
            if(!sustainIsDown)
            {
                sustainIsDown = false;
                
                currentPiano->prepMap->sustainPedalReleased();
                
                if(prevPiano != currentPiano) prevPiano->prepMap->sustainPedalReleased(true);
            }
            else
            {
                sustainIsDown = true;
                
                if (firstTime) { firstTime = false; return; }
                else currentPiano->prepMap->sustainPedalPressed();
            }
        }
    }

    
    inline bool getSustainInversion(void) { return sustainInverted; }
    
    ValueTree getPreparationState(BKPreparationType type, int Id);
    void setPreparationState(BKPreparationType type, int Id, XmlElement* xml);
    
    void exportPreparation(BKPreparationType type, int Id, String name);
    void importPreparation(BKPreparationType type, int Id, int importId);
    
    void exportPiano(int Id, String name);
    void importPiano(int Id, int importId);
    
    Array<MidiDeviceInfo> getMidiOutputDevices();
    Array<MidiDeviceInfo> getMidiInputDevices();
    
    std::unique_ptr<MidiInput> openMidiInputDevice(const String& deviceIdentifier, MidiInputCallback* callback);
    
    inline bool isMidiReady(void) { return midiReady; }
    inline void setMidiReady(bool ready) { midiReady = ready; }
    
    inline bool areTooltipsEnabled(void) { return tooltipsEnabled.getValue(); }
    inline Value getTooltipsEnabled(void) { return tooltipsEnabled; }
    inline void setTooltipsEnabled(bool enabled) { tooltipsEnabled.setValue(enabled); }

    /*
	inline bool areKeystrokesEnabled(void) { return keystrokesEnabled.getValue(); }
	inline Value getKeystrokesEnabled(void) { return keystrokesEnabled; }
	inline void setKeystrokesEnabled(bool enabled) { keystrokesEnabled.setValue(enabled); }
    */

	inline bool areHotkeysEnabled(void) { return hotkeysEnabled.getValue(); }
	inline Value getHotkeysEnabled(void) { return hotkeysEnabled; }
	inline void setHotkeysEnabled(bool enabled) { hotkeysEnabled.setValue(enabled); }

    void handleAllNotesOff();

    
private:
    double currentSampleRate;
    
    int  currentPianoId;
    
    bool sustainIsDown = false;
    bool sustainInverted = false;
    bool noteOnSetsNoteOffVelocity = true;
   
    void sustainActivate(void);
    void sustainDeactivate(void);
    
    double pitchbendVal;
    
    bool doneWithSetStateInfo;
    
    AudioSampleBuffer levelBuf; //for storing samples for metering/RMS calculation
    
    Array<float> tempoAlreadyLoaded;
    
    Array<int> notesOnUI;
    Array<int> notesOffUI;
    
    File lastGalleryPath;
    
    AudioPlayHead* playHead;
    AudioPlayHead::CurrentPositionInfo currentPositionInfo;
    double hostTempo;
    
    Array<MidiDeviceInfo> midiInputDevices;
    
    bool midiReady;
    Array<String> defaultMidiInputSources;
    
    BKAudioProcessorEditor* editor;
    
    Value tooltipsEnabled;

	//Value keystrokesEnabled;

	Value hotkeysEnabled;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKAudioProcessor)
};


#endif  // PLUGINPROCESSOR_H_INCLUDED
