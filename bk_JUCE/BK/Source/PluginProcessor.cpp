
#include "PluginProcessor.h"
#include "PluginEditor.h"

#include "BKPianoSampler.h"

#define NOST_KEY_OFF 1

String notes[4] = {"A","C","D#","F#"};

//==============================================================================
BKAudioProcessor::BKAudioProcessor()
{
    
    numSynchronicLayers = 1;
    
    numNostalgicLayers = 1;

    sProcessor = OwnedArray<SynchronicProcessor, CriticalSection>();
    sProcessor.ensureStorageAllocated(aMaxNumLayers);
    
    nProcessor = OwnedArray<NostalgicProcessor, CriticalSection>();
    nProcessor.ensureStorageAllocated(aMaxNumLayers);
    
    sPreparation = Array<SynchronicPreparation::Ptr, CriticalSection>();
    nPreparation = Array<NostalgicPreparation::Ptr, CriticalSection>();
    
    // For testing and developing, let's keep directory of samples in home folder on disk.
    loadMainPianoSamples(&mainPianoSynth, aNumSampleLayers);
    loadHammerReleaseSamples(&hammerReleaseSynth);
    loadResonanceRelaseSamples(&resonanceReleaseSynth);
}

BKAudioProcessor::~BKAudioProcessor()
{
    
}

//==============================================================================
void BKAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock) {
    
    // Use this method as the place to do any pre-playback
    // initialisation that you need.
    
    mainPianoSynth.setCurrentPlaybackSampleRate(sampleRate);
    
    hammerReleaseSynth.setCurrentPlaybackSampleRate(sampleRate);
    
    resonanceReleaseSynth.setCurrentPlaybackSampleRate(sampleRate);
    
    for (int i = 0; i < aMaxNumLayers; i++)
    {
        SynchronicPreparation::Ptr sPrep = new SynchronicPreparation();
        sPreparation.insert(i, sPrep);
        sProcessor.insert(i, new SynchronicProcessor(&mainPianoSynth, sPrep));
    }
    
    for (int i = 0; i < aMaxNumLayers; i++)
    {
        NostalgicPreparation::Ptr nPrep = new NostalgicPreparation();
        nPreparation.insert(i, nPrep);
        nProcessor.insert(i, new NostalgicProcessor(&mainPianoSynth, nPrep));
    }
    
    currentSynchronicPreparation = sPreparation.getUnchecked(0);
    currentNostalgicPreparation = nPreparation.getUnchecked(0);
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
    
    Array<float> tuningOffsets = Array<float>(aPartialTuning,aNumScaleDegrees);
    int tuningBasePitch = 0;
    
    int numSamples = buffer.getNumSamples();
    
    // Process each layer.
    // Could but nostalgic->keyOff/playNote here: (1) have synchronic->processBlock return 1 if pulse happened, otherwise 0. if nostalgic->mode == SynchronicSync and make sure layer corresponds
    for (int layer = 0; layer < numSynchronicLayers; layer++)
    {
        sProcessor[layer]->processBlock(numSamples, m.getChannel());
    }
    
    for (int layer = 0; layer < numNostalgicLayers; layer++)
    {
        nProcessor[layer]->processBlock(numSamples, m.getChannel());
    }
    
    for (MidiBuffer::Iterator i (midiMessages); i.getNextEvent (m, time);)
    {
        int noteNumber = m.getNoteNumber();
        float velocity = m.getFloatVelocity();
        
        channel = m.getChannel();
        
        if (m.isNoteOn())
        {
            
            // Send key on to each layer.
            for (int layer = 0; layer < numSynchronicLayers; layer++)
            {
                sProcessor[layer]->keyPressed(noteNumber, velocity);
            }
            
            for (int layer = 0; layer < numNostalgicLayers; layer++)
            {
                nProcessor[layer]->keyPressed(noteNumber, velocity);
            }
            
            mainPianoSynth.keyOn(
                                 channel,
                                 noteNumber,
                                 0,
                                 velocity * aGlobalGain,
                                 tuningOffsets,
                                 tuningBasePitch,
                                 Forward,
                                 Normal,
                                 BKNoteTypeNil,
                                 1000, // start
                                 1000, // length
                                 3,
                                 3 );
            
            
        }
        else if (m.isNoteOff())
        {
            
            //need to integrate sProcess layer number here as well, just defaulting for the moment
            for (int i = 0; i < numNostalgicLayers; i++)
            {
                nProcessor[i]->keyReleased(
                                                   noteNumber,
                                                   channel,
                                                   sProcessor[i]->getTimeToNext(), // [i] should really be [target]
                                                   sProcessor[i]->getBeatLength()); // should be in preparation/processor 
                
            }
            
            for (int i = 0; i < numSynchronicLayers; i++)
            {
                sProcessor[i]->keyReleased(noteNumber);
            }
            
            mainPianoSynth.keyOff(
                                  channel,
                                  noteNumber,
                                  velocity,
                                  true
                                  );
            
            hammerReleaseSynth.keyOn(
                                     channel,
                                     noteNumber,
                                     0,
                                     velocity * 0.0025 * aGlobalGain, //will want hammerGain multipler that user can set
                                     tuningOffsets,
                                     tuningBasePitch,
                                     Forward,
                                     FixedLength,
                                     BKNoteTypeNil,
                                     0,
                                     2000,
                                     3,
                                     3 );
            
            resonanceReleaseSynth.keyOn(
                                        channel,
                                        noteNumber,
                                        0,
                                        velocity * aGlobalGain, //will also want multiplier for resonance gain...
                                        tuningOffsets,
                                        tuningBasePitch,
                                        Forward,
                                        FixedLength,
                                        BKNoteTypeNil,
                                        0,
                                        2000,
                                        3,
                                        3 );
            
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

void BKAudioProcessor::loadMainPianoSamples(BKSynthesiser *synth, int numLayers)
{
    WavAudioFormat wavFormat;
    
    String path = "~/samples/";
    
    // 88 or more seems to work well
    for (int i = 0; i < 100; i++)
    {
        mainPianoSynth.addVoice(new BKPianoSamplerVoice());
    }
    
    mainPianoSynth.clearSounds();
    
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 4; j++) {
            if ((i == 0) && (j > 0)) continue;
            
            for (int k = 0; k < numLayers; k++)
            {
                
                String temp = path;
                temp.append(notes[j],3);
                temp.append(String(i),3);
                temp.append("v",2);
                
                if (numLayers == 8)
                {
                    temp.append(String(((k*2)+1)),3);
                }
                else if (numLayers == 4)
                {
                    temp.append(String(((k*4)+2)),3);
                }
                else if (numLayers == 2)
                {
                    temp.append(String(k*7+8),3);
                }
                else
                {
                    
                }
                
                temp.append(".wav",5);
                File file(temp);
                
                FileInputStream inputStream(file);
                
                if (inputStream.openedOk()) {
                    
                    DBG("file opened OK: " + file.getFileName());
                    
                    String soundName = file.getFileName();
                    
                    sampleReader = wavFormat.createReaderFor(new FileInputStream(file), true);
                    
                    BigInteger noteRange;
                    
                    int root = 0;
                    if (j == 0) {
                        root = (9+12*i) + 12;
                        if (i == 7) {
                            // High C.
                            noteRange.setRange(root-1,5,true);
                        }else {
                            noteRange.setRange(root-1,3,true);
                        }
                    } else if (j == 1) {
                        root = (0+12*i) + 12;
                        noteRange.setRange(root-1,3,true);
                    } else if (j == 2) {
                        root = (3+12*i) + 12;
                        noteRange.setRange(root-1,3,true);
                    } else if (j == 3) {
                        root = (6+12*i) + 12;
                        noteRange.setRange(root-1,3,true);
                    } else {
                        
                    }
                    
                    BigInteger velocityRange;
                    if (numLayers == 8)
                    {
                        velocityRange.setRange(aVelocityThresh_Eight[k], (aVelocityThresh_Eight[k+1] - aVelocityThresh_Eight[k]), true);
                    }
                    else if (numLayers == 4)
                    {
                        velocityRange.setRange(aVelocityThresh_Four[k], (aVelocityThresh_Four[k+1] - aVelocityThresh_Four[k]), true);
                    }
                    else if (numLayers == 2)
                    {
                        velocityRange.setRange(aVelocityThresh_Two[k], (aVelocityThresh_Two[k+1] - aVelocityThresh_Two[k]), true);
                    }
                    else
                    {
                        
                    }
                    
                    
                    double sourceSampleRate = sampleReader->sampleRate;
                    const int numChannels = sampleReader->numChannels;
                    uint64 maxLength;
                    
                    if (sourceSampleRate <= 0 || sampleReader->lengthInSamples <= 0) {
                        maxLength = 0;
                        
                    } else {
                        maxLength = jmin((uint64)sampleReader->lengthInSamples, (uint64) (aMaxSampleLengthSec * sourceSampleRate));
                        
                        ReferenceCountedBuffer::Ptr newBuffer = new ReferenceCountedBuffer(file.getFileName(),jmin(2, numChannels),maxLength);
                        sampleReader->read(newBuffer->getAudioSampleBuffer(), 0, sampleReader->lengthInSamples, 0, true, true);
                        
                        mainPianoSynth.addSound(new BKPianoSamplerSound(soundName,
                                                                newBuffer,
                                                                maxLength,
                                                                sourceSampleRate,
                                                                noteRange,
                                                                root,
                                                                velocityRange));
                    }
                    
                    
                    
                    
                    
                } else {
                    DBG("file not opened OK: " + temp);
                }
                
            }
            
        }
    }
}

void BKAudioProcessor::loadResonanceRelaseSamples(BKSynthesiser *synth)
{
    WavAudioFormat wavFormat;
    
    String path = "~/samples/";
    
    for (int i = 0; i < 88; i++)
    {
        resonanceReleaseSynth.addVoice(new BKPianoSamplerVoice() );
    }
    
    resonanceReleaseSynth.clearSounds();
    
    //load release resonance samples
    for (int i = 0; i < 7; i++) {       //i => octave
        for (int j = 0; j < 4; j++) {   //j => note name
            
            if ((i == 0) && (j > 0)) continue;
            if ((i == 6) && (j != 1) && (j != 2) ) continue;
            
            for (int k = 0; k < 3; k++) //k => velocity layer
            {
                String temp = path;
                temp += "harm";
                if(k==0) temp += "V3";
                else if(k==1) temp += "S";
                else if(k==2) temp += "L";
                temp += notes[j];
                temp += std::to_string(i);
                temp += ".wav";
                
                File file(temp);
                FileInputStream inputStream(file);
                
                if (inputStream.openedOk()) {
                    
                    DBG("file opened OK: " + file.getFileName());
                    String soundName = file.getFileName();
                    sampleReader = wavFormat.createReaderFor(new FileInputStream(file), true);
                    
                    //keymap assignment
                    BigInteger noteRange;
                    int root = 0;
                    if (j == 0)
                    {
                        root = (9+12*i) + 12;
                        noteRange.setRange(root-1,3,true);
                    }
                    else if (j == 1)
                    {
                        root = (0+12*i) + 12;
                        noteRange.setRange(root-1,3,true);
                    }
                    else if (j == 2)
                    {
                        root = (3+12*i) + 12;
                        noteRange.setRange(root-1,3,true);
                    }
                    else if (j == 3)
                    {
                        root = (6+12*i) + 12;
                        noteRange.setRange(root-1,3,true);
                    }
                    
                    //velocity switching
                    BigInteger velocityRange;
                    velocityRange.setRange(aResonanceVelocityThresh[k], (aResonanceVelocityThresh[k+1] - aResonanceVelocityThresh[k]), true);
                    
                    //load the sample, add to synth
                    double sourceSampleRate = sampleReader->sampleRate;
                    const int numChannels = sampleReader->numChannels;
                    uint64 maxLength;
                    
                    if (sourceSampleRate <= 0 || sampleReader->lengthInSamples <= 0)
                    {
                        maxLength = 0;
                    }
                    else
                    {
                        maxLength = jmin((uint64)sampleReader->lengthInSamples, (uint64) (aMaxSampleLengthSec * sourceSampleRate));
                        
                        ReferenceCountedBuffer::Ptr newBuffer = new ReferenceCountedBuffer(file.getFileName(),jmin(2, numChannels),maxLength);
                        sampleReader->read(newBuffer->getAudioSampleBuffer(), 0, sampleReader->lengthInSamples, 0, true, true);
                        
                        //DBG("added resonance: " + std::to_string(noteRange.toInteger()) + " " + std::to_string(root) + " " + std::to_string(velocityRange.toInteger()) );
                        resonanceReleaseSynth.addSound(new BKPianoSamplerSound(soundName,
                                                                newBuffer,
                                                                maxLength,
                                                                sourceSampleRate,
                                                                noteRange,
                                                                root,
                                                                velocityRange));
                    }
                }
                else
                {
                    DBG("file not opened OK: " + temp);
                }
            }
        }
    }
}

void BKAudioProcessor::loadHammerReleaseSamples(BKSynthesiser *synth)
{
    WavAudioFormat wavFormat;
    
    String path = "~/samples/";
    
    for (int i = 0; i < 88; i++)
    {
        hammerReleaseSynth.addVoice(new BKPianoSamplerVoice() );
    }
    
    hammerReleaseSynth.clearSounds();
    
    //load hammer release samples
    for (int i = 1; i <= 88; i++) {
        
        String temp = path;
        temp += "rel";
        temp += std::to_string(i);
        temp += ".wav";
        
        File file(temp);
        FileInputStream inputStream(file);
        
        if (inputStream.openedOk()) {
            
            DBG("file opened OK: " + file.getFileName());
            String soundName = file.getFileName();
            sampleReader = wavFormat.createReaderFor(new FileInputStream(file), true);
            
            BigInteger noteRange;
            noteRange.setRange(20 + i, 1, true);
            
            BigInteger velocityRange;
            velocityRange.setRange(0, 128, true);
            
            int root = 20 + i;
            
            double sourceSampleRate = sampleReader->sampleRate;
            const int numChannels = sampleReader->numChannels;
            uint64 maxLength;
            
            if (sourceSampleRate <= 0 || sampleReader->lengthInSamples <= 0) {
                maxLength = 0;
                
            } else {
                maxLength = jmin((uint64)sampleReader->lengthInSamples, (uint64) (aMaxSampleLengthSec * sourceSampleRate));
                
                ReferenceCountedBuffer::Ptr newBuffer = new ReferenceCountedBuffer(file.getFileName(),jmin(2, numChannels),maxLength);
                sampleReader->read(newBuffer->getAudioSampleBuffer(), 0, sampleReader->lengthInSamples, 0, true, true);
                
                hammerReleaseSynth.addSound(new BKPianoSamplerSound(soundName,
                                                        newBuffer,
                                                        maxLength,
                                                        sourceSampleRate,
                                                        noteRange,
                                                        root,
                                                        velocityRange));
            }
        } else {
            DBG("file not opened OK: " + temp);
        }
    }
    
}

