
#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "BKPianoSampler.h"

#define NOST_KEY_OFF 1

//==============================================================================
BKAudioProcessor::BKAudioProcessor():
general                 (new GeneralSettings()),
mainPianoSynth          (general),
hammerReleaseSynth      (general),
resonanceReleaseSynth   (general),
sPreparation            (SynchronicPreparation::CSPtrArr()),
nPreparation            (NostalgicPreparation::CSPtrArr()),
dPreparation            (DirectPreparation::CSPtrArr()),
tPreparation            (TuningPreparation::CSPtrArr()),
bkKeymaps               (Keymap::PtrArr()),
currentPiano            (Piano::Ptr()),
prevPianos              (Piano::PtrArr()),
bkPianos                (Piano::PtrArr()),
noteOn                  (Array<int>())
{
    
    //allocate storage
    bkKeymaps.ensureStorageAllocated(aMaxNumPreparationKeymaps);
    bkPianos.ensureStorageAllocated(aMaxNumPianos);
    prevPianos.ensureStorageAllocated(aMaxNumPianos);
    tPreparation.ensureStorageAllocated(aMaxTuningPreparations);

    // Make a bunch of keymaps.
    for (int i = 0; i < aMaxNumKeymaps; i++)
        bkKeymaps.add(new Keymap(i));
    
    // Make a bunch of tunings.
    for (int i = 0; i < (aMaxTuningPreparations); i++)
        tPreparation.add(new TuningPreparation(i));
    
    // Make a bunch of preparations. Default to zeroth tuning.
    for (int i = 0; i < aMaxTotalPreparations; i++) {
        sPreparation.add(new SynchronicPreparation(i, tPreparation[0]));
        nPreparation.add(new NostalgicPreparation(i, tPreparation[0]));
        dPreparation.add(new DirectPreparation(i, tPreparation[0]));
        
    }
    
    // Make a bunch of pianos. Default to zeroth keymap.
    for (int i = 0 ; i < aMaxNumPianos; i++)
        bkPianos.set(i, new Piano(&mainPianoSynth, &resonanceReleaseSynth, &hammerReleaseSynth, bkKeymaps[0], i)); // initializing piano 0
  
    // Initialize first piano.
    currentPiano = bkPianos[0];
    
    // For testing and developing, let's keep directory of samples in home folder on disk.
    BKSampleLoader::loadMainPianoSamples(&mainPianoSynth, aNumSampleLayers);
    BKSampleLoader::loadHammerReleaseSamples(&hammerReleaseSynth);
    BKSampleLoader::loadResonanceReleaseSamples(&resonanceReleaseSynth);
    
}

BKAudioProcessor::~BKAudioProcessor()
{
    
}

//==============================================================================
void BKAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    
    mainPianoSynth.setCurrentPlaybackSampleRate(sampleRate);
    hammerReleaseSynth.setCurrentPlaybackSampleRate(sampleRate);
    resonanceReleaseSynth.setCurrentPlaybackSampleRate(sampleRate);
       
    for (int i = aMaxNumPianos; --i >= 0;)
        bkPianos[i]->prepareToPlay(sampleRate);
    
}

void BKAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    buffer.clear();
    
    int time;
    MidiMessage m;
    
    int numSamples = buffer.getNumSamples();
    
    // Process all active prep maps in current piano
    for (int p = currentPiano->activePMaps.size(); --p >= 0;)
        currentPiano->activePMaps[p]->processBlock(numSamples, m.getChannel());
    
    for (MidiBuffer::Iterator i (midiMessages); i.getNextEvent (m, time);)
    {
        int noteNumber = m.getNoteNumber();
        float velocity = m.getFloatVelocity();
        int p, pm; // piano, prepmap
        
        channel = m.getChannel();
        
        if (m.isNoteOn())
        {
            ++noteOnCount;
            
            if (allNotesOff)   allNotesOff = false;
            
            // Send key on to each pmap in current piano
            for (p = currentPiano->activePMaps.size(); --p >= 0;)
                currentPiano->activePMaps[p]->keyPressed(noteNumber, velocity, channel);
        }
        else if (m.isNoteOff())
        {
            
            // Send key off to each pmap in current piano
            for (p = currentPiano->activePMaps.size(); --p >= 0;)
                currentPiano->activePMaps[p]->keyReleased(noteNumber, velocity, channel);
            
            // This is to make sure note offs are sent to Direct and Nostalgic processors from previous pianos with holdover notes.
            for (p = prevPianos.size(); --p >= 0;) {
                for (pm = prevPianos[p]->activePMaps.size(); --pm >= 0;) {
                    prevPianos[p]->activePMaps[pm]->postRelease(noteNumber, velocity, channel);
                }
            }
            
            --noteOnCount;
            
            if (!allNotesOff && !noteOnCount) {
                prevPianos.clearQuick();
                allNotesOff = true;
            }
        }
        else if (m.isAftertouch())
        {
        }
        else if (m.isPitchWheel())
        {
            
        }

    }

    mainPianoSynth.renderNextBlock(buffer,midiMessages,0, numSamples);
    hammerReleaseSynth.renderNextBlock(buffer,midiMessages,0, numSamples);
    resonanceReleaseSynth.renderNextBlock(buffer,midiMessages,0, numSamples);
    
    
    
}

void  BKAudioProcessor::setCurrentPiano(int which)
{
    if (noteOnCount)  prevPianos.add(currentPiano);
    
    currentPiano = bkPianos[which];

    //currentPiano->activePMaps.addIfNotAlreadyThere(currentPiano->prepMaps[0]);
}

void BKAudioProcessor::releaseResources() {
    
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    //fileBuffer.setSize (0, 0);
    
}


#ifndef JucePlugin_PreferredChannelConfigurations
bool BKAudioProcessor::setPreferredBusArrangement (bool isInput, int bus, const AudioChannelSet& preferredSet) {
    
    // Reject any bus arrangements that are not compatible with your plugin
    
    const int numChannels = preferredSet.size();
    
#if JucePlugin_IsMidiEffect
    if (numChannels != 0)
        return false;
#elif JucePlugin_IsSynth
    if (isInput || (numChannels != 1 && numChannels != 2))
        return false;
#else
    if (numChannels != 1 && numChannels != 2)
        return false;
    
    if (! AudioProcessor::setPreferredBusArrangement (! isInput, bus, preferredSet))
        return false;
#endif
    
    return AudioProcessor::setPreferredBusArrangement (isInput, bus, preferredSet);
}
#endif



//==============================================================================
void BKAudioProcessor::changeListenerCallback(ChangeBroadcaster *source)
{
    
}

//==============================================================================
bool BKAudioProcessor::hasEditor() const
{
    
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* BKAudioProcessor::createEditor()
{
    return new BKAudioProcessorEditor (*this);
}

//==============================================================================
void BKAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void BKAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
const String BKAudioProcessor::getName() const {
    
    return JucePlugin_Name;
}

bool BKAudioProcessor::acceptsMidi() const {
    
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool BKAudioProcessor::producesMidi() const {
    
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

double BKAudioProcessor::getTailLengthSeconds() const {
    
    return 0.0;
}

int BKAudioProcessor::getNumPrograms() {
    
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int BKAudioProcessor::getCurrentProgram() {
    
    return 0;
}

void BKAudioProcessor::setCurrentProgram (int index) {
    
}

const String BKAudioProcessor::getProgramName (int index) {
    
    return String("bitKlavier");
}

void BKAudioProcessor::changeProgramName (int index, const String& newName) {
    
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    
    return new BKAudioProcessor();
}

