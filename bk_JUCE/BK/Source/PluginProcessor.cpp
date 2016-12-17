
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
pianos                  (Piano::CSArr()),
sProcessor(SynchronicProcessor::CSArr()),
nProcessor(NostalgicProcessor::Arr()),
dProcessor(DirectProcessor::Arr()),
sPreparation(SynchronicPreparation::CSArr()),
nPreparation(NostalgicPreparation::CSArr()),
dPreparation(DirectPreparation::CSArr()),
tPreparation(TuningPreparation::CSArr()),
bkKeymaps(Keymap::CSArr()),
numSynchronicLayers(12),
numNostalgicLayers(12),
numDirectLayers(12)
{
    pianos.ensureStorageAllocated(aMaxNumPianos);
    
    
    sProcessor.ensureStorageAllocated(aMaxNumPianos);
    nProcessor.ensureStorageAllocated(aMaxNumPianos);
    dProcessor.ensureStorageAllocated(aMaxNumPianos);
    
    tPreparation.ensureStorageAllocated(aMaxNumPianos * 3);
    bkKeymaps.ensureStorageAllocated(aMaxNumPianos * 3);
    
    
    
    for (int i = 0; i < (3 * aMaxNumPianos); i++)
    {
        bkKeymaps.add(new Keymap(i));
        tPreparation.add(new TuningPreparation(i));
    }
    
    for (int i = 0; i < aMaxNumPianos; i++)
    {
        sPreparation.add(new SynchronicPreparation(i, tPreparation[0]));
        nPreparation.add(new NostalgicPreparation(i, tPreparation[0]));
        dPreparation.add(new DirectPreparation(i, tPreparation[0]));
    }
    
    
    for (int i = 0; i < aMaxNumPianos; i++)
    {
        pianos.insert(i, new Piano(i, &mainPianoSynth, &resonanceReleaseSynth, &hammerReleaseSynth));
        
        
        sProcessor.insert(i, new SynchronicProcessor(&mainPianoSynth, bkKeymaps[0], sPreparation[0], i));
        nProcessor.insert(i, new NostalgicProcessor(&mainPianoSynth, bkKeymaps[0], nPreparation[0], sProcessor, i));
        dProcessor.insert(i, new DirectProcessor(&mainPianoSynth, &resonanceReleaseSynth, &hammerReleaseSynth, bkKeymaps[0], dPreparation[0], i));
         
    }
    
    currentPiano = pianos[0];
    
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
    
    for (int i = 0; i < aMaxNumPianos; i++)
    {
        pianos[i]->setCurrentPlaybackSampleRate(sampleRate);
        
        /*
        sProcessor[i]->setCurrentPlaybackSampleRate(sampleRate);
        nProcessor[i]->setCurrentPlaybackSampleRate(sampleRate);
        dProcessor[i]->setCurrentPlaybackSampleRate(sampleRate);
         */
    }
}

void BKAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages) {
    
    //const int totalNumInputChannels  = getTotalNumInputChannels();
    //const int totalNumOutputChannels = getTotalNumOutputChannels();
    
    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming fee`dback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    /*
    for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i) {
        buffer.clear (i, 0, buffer.getNumSamples());
    }
     */
    
    buffer.clear();
    
    MidiBuffer processedMidi;
    int time;
    MidiMessage m;
    
    int numSamples = buffer.getNumSamples();
    
     for (int p = 0; p < aMaxNumPianos; p++)
     {
        pianos[p]->processBlock(numSamples, m.getChannel()); //precede with if(pianos[p]->active)
     }
    
    // Process each layer.
    // Could put nostalgic->keyOff/playNote here: (1) have synchronic->processBlock return 1 if pulse happened, otherwise 0. if nostalgic->mode == SynchronicSync and make sure layer corresponds
    
    /*
    for (int layer = 0; layer < numSynchronicLayers; layer++)
    {
        sProcessor[layer]->processBlock(numSamples, m.getChannel());
    }
    
    for (int layer = 0; layer < numNostalgicLayers; layer++)
    {
        nProcessor[layer]->processBlock(numSamples, m.getChannel());
    }
    
    for (int layer = 0; layer < numDirectLayers; layer++)
    {
        dProcessor[layer]->processBlock(numSamples, m.getChannel());
    }
    */
    
    for (MidiBuffer::Iterator i (midiMessages); i.getNextEvent (m, time);)
    {
        int noteNumber = m.getNoteNumber();
        float velocity = m.getFloatVelocity();
        
        channel = m.getChannel();
        
        if (m.isNoteOn())
        {
            
            
            for (int p = 0; p < aMaxNumPianos; p++)  //or < numCurrentPianos...
            {
                pianos[p]->keyPressed(noteNumber, velocity, channel); //precede with if(pianos[p]->active)
            }
            
            /*
            // Send key on to each layer.
            for (int layer = 0; layer < numSynchronicLayers; layer++)
            {
                sProcessor[layer]->keyPressed(noteNumber, velocity);
            }
            
            for (int layer = 0; layer < numNostalgicLayers; layer++)
            {
                nProcessor[layer]->keyPressed(noteNumber, velocity);
            }
            
            for (int layer = 0; layer < numDirectLayers; layer++)
            {
                dProcessor[layer]->keyPressed(noteNumber, velocity, channel);
            }
             */
        }
        else if (m.isNoteOff())
        {
            
            for (int p = 0; p < aMaxNumPianos; p++)
            {
                pianos[p]->keyReleased(noteNumber, velocity, channel); //precede with if(pianos[p]->active)
            }
            
            /*
            //need to integrate sProcess layer number here as well, just defaulting for the moment
            for (int i = 0; i < numNostalgicLayers; i++)
            {
                nProcessor[i]->keyReleased(noteNumber, channel); // should be in preparation/processor
                
            }
            
            for (int i = 0; i < numSynchronicLayers; i++)
            {
                sProcessor[i]->keyReleased(noteNumber, channel);
            }
            
            for (int i = 0; i < numDirectLayers; i++)
            {
                dProcessor[i]->keyReleased(noteNumber,
                                           velocity,
                                           channel);
            }
             */
            
        }
        else if (m.isAftertouch())
        {
        }
        else if (m.isPitchWheel())
        {
        }
        
        processedMidi.addEvent (m, time);
    }
    
    midiMessages.swapWith (processedMidi);

    mainPianoSynth.renderNextBlock(buffer,midiMessages,0, numSamples);
    hammerReleaseSynth.renderNextBlock(buffer,midiMessages,0, numSamples);
    resonanceReleaseSynth.renderNextBlock(buffer,midiMessages,0, numSamples);
    
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

