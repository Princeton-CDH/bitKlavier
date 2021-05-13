
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
                         public MidiInputCallback,
                         public HighResolutionTimer
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
    void loadGalleryFromXml(XmlElement* xml, bool resetHistory = true);
    void loadJsonGalleryFromPath(String path);
    void saveCurrentGalleryAs(void);
    void saveCurrentGallery(void);
    void createNewGallery(String name, std::shared_ptr<XmlElement> xml = nullptr);
    void renameGallery(String name);
    void duplicateGallery(String name);
    void deleteGallery(void);
    
    void updateGalleryFromXml(XmlElement* xml);
    
    void importCurrentGallery(void);
    void exportCurrentGallery(void);
    
    void importSoundfont(void);
    
    void writeCurrentGalleryToURL(String url);
    void deleteGalleryAtURL(String url);
    
    String firstGallery(void);
    void initializeGallery(void);
    
    std::unique_ptr<FileChooser> fc;
    
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
    
    StringArray mikroetudes, ns_etudes, bk_examples, machines_for_listening;
    
    StringArray                         galleryNames;
    String                              currentGallery;
    
    // Full path names of soundfonts in no particular order
    StringArray                         soundfontNames;
    
    // Names of soundfont instruments keyed by sound set ids
    HashMap<int, StringArray>           instrumentNames;
    
    StringArray                         customSampleSetNames;
    
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
    
    bool firstPedalDown;
    
    bool                                defaultLoaded;
    String                              defaultName;

    SampleTouchThread touchThread;
    
    FileSearchPath sampleSearchPath;

    void updateGalleries(void);
    
    void collectGalleries(void);
    void collectPianos(void);
    void collectPreparations(void);
    void collectSoundfonts(void);
    void collectCustomSamples(void);
    
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
    
    OwnedArray<HashMap<String, int>>    sourcedNotesOn;
    OwnedArray<HashMap<String, int>>&   getSourcedNotesOn();
    OwnedArray<HashMap<String, float>>  sourcedNoteVelocities;
    
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
    
    void hiResTimerCallback() override;
    
//    inline const Array<MidiDeviceInfo> getDefaultMidiInputDevices(void) { return defaultMidiInputDevices; }
//    inline void setDefaultMidiInputDevices(Array<MidiDeviceInfo> sources) { defaultMidiInputDevices = sources; }
    
    inline const Array<String> getDefaultMidiInputNames(void) { return defaultMidiInputNames; }
    inline void setDefaultMidiInputNames(Array<String> names) { defaultMidiInputNames = names; }
    
    inline const Array<String> getDefaultMidiInputIdentifiers(void) { return defaultMidiInputIdentifiers; }
    inline void setDefaultMidiInputIdentifiers(Array<String> identifiers) { defaultMidiInputIdentifiers = identifiers; }
    
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
    
    void handleNoteOn(int noteNumber, float velocity, int channel, int mappedFrom, String source, bool harmonizer = false);
    void handleNoteOff(int noteNumber, float velocity, int channel, int mappedFrom, String source, bool harmonizer = false);
    
    void handlePianoPostRelease(Piano::Ptr piano, int noteNumber, float velocity, int channel, int mappedFrom, String source, bool harmonizer = false);

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
        // Clear connections in the existing clipboard to avoid
        // reference between items which would cause leaks
        for (auto item : clipboard)
            item->clearConnections();
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
    
    int screenWidth = 1920;
    int screenHeight = 1080;
    
    double progress;
    double progressInc;
    bool didLoadHammersAndRes, didLoadMainPianoSamples;
    
    void clearBitKlavier(void);
    
    void loadSamplesStartup(void);

    inline void setSustainFromMenu(bool toSet)
    {
        if (toSet)
        {
            sustainActivate();
        }
        else
        {
            sustainDeactivate();
        }
    }

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
                
                if (firstPedalDown) {firstPedalDown = false; return;}
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
                
                if (firstPedalDown) { firstPedalDown = false; return; }
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

	inline bool areHotkeysEnabled(void) { return hotkeysEnabled.getValue(); }
	inline Value getHotkeysEnabled(void) { return hotkeysEnabled; }
	inline void setHotkeysEnabled(bool enabled) { hotkeysEnabled.setValue(enabled); }
    
    inline bool isMemoryMappingEnabled(void) { return memoryMappingEnabled.getValue(); }
    inline Value getMemoryMappingEnabled(void) { return memoryMappingEnabled; }
    inline void setMemoryMappingEnabled(bool enabled) { memoryMappingEnabled.setValue(enabled); }

    void handleAllNotesOff();
    
    Array<std::shared_ptr<XmlElement>> galleryHistory;
    int undoDepth;
    
#define UNDO_HISTORY_SIZE 100
    
    inline void saveGalleryToHistory(String actionDesc = String())
    {
        int start = galleryHistory.size() - undoDepth;
        galleryHistory.removeRange(start, undoDepth);
        
        ValueTree galleryVT = gallery->getState();
        
        // Maybe should do this by piano instead of gallery
        // Loading some galleries is very slow
//        ValueTree galleryVT = currentPiano->getState();
        
        galleryVT.setProperty("actionDesc", actionDesc, 0);
        galleryHistory.add(galleryVT.createXml());
        if (galleryHistory.size() > UNDO_HISTORY_SIZE) galleryHistory.remove(0);
        undoDepth = 0;
    }
    
    inline void resetGalleryHistory()
    {
        galleryHistory.clear();
        saveGalleryToHistory("Root");
    }
    
    inline String undoGallery()
    {
        if (undoDepth >= galleryHistory.size() - 1) return String();
        XmlElement* prevGalleryVT = galleryHistory.getUnchecked(galleryHistory.size() - 1 - undoDepth).get();
        undoDepth++;
        XmlElement* galleryVT = galleryHistory.getUnchecked(galleryHistory.size() - 1 - undoDepth).get();
        
        loadGalleryFromXml(galleryVT, false);
        
//        currentPiano->setState(galleryVT.createXml().get(), &gallery->idmap, gallery->idcounts);
//        currentPiano->configure();
        
        return "Undo " + prevGalleryVT->getStringAttribute("actionDesc");
    }
    
    inline String redoGallery()
    {
        if (undoDepth == 0) return String();
        undoDepth--;
        XmlElement* galleryVT = galleryHistory.getUnchecked(galleryHistory.size() - 1 - undoDepth).get();
        
        loadGalleryFromXml(galleryVT, false);
        
//        currentPiano->setState(galleryVT.createXml().get(), &gallery->idmap, gallery->idcounts);
//        currentPiano->configure();
        
        return "Redo " + galleryVT->getStringAttribute("actionDesc");
    }
    
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
    
    bool midiReady;
    Array<String> defaultMidiInputNames;
    Array<String> defaultMidiInputIdentifiers;
    
    BKAudioProcessorEditor* editor;
    
    Value tooltipsEnabled;
    
	Value hotkeysEnabled;
    
    Value memoryMappingEnabled;
  
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BKAudioProcessor)
};


#endif  // PLUGINPROCESSOR_H_INCLUDED
